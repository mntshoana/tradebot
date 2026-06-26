#include "window.hpp"
#include <QTimer>
int HomeView::exchange = LUNO_EXCHANGE;

HomeView::HomeView (int exchange, QWidget *parent) : QWidget(parent) {
    this->timestamp = new unsigned long long();
    HomeView::exchange = exchange;
    
    // for xcode, app is built in relative path "./build/Debug/" which is undesired
    // find absolute path to resource folder  "../../src/data/";
    this->path = absolutePath();
    size_t pos = this->path.find_last_of("/", path.length()-1);
    pos = this->path.find_last_of("/", pos-1);
    pos = this->path.find_last_of("/", pos-1);
    this->path = path.substr(0, pos) + "/src/data/";
    
    this->closing = false;
    
    TextPanel::init(parent);
    
    livePanel = new LivePanel(parent);

    // Chart Displayed using QWebEngine browser
    view = new QWebEngineView(parent); // to do: move VIEW to a chart widget
    view->setGeometry(0, 0, 930, 500);
    
    workPanel = new WorkspacePanel(parent);
    workPanel->setGeometry(0, 472, 930, 248);

}

HomeView::~HomeView(){
    delete livePanel;
    livePanel = nullptr;
    
    delete workPanel;
    workPanel = nullptr;
    
    delete timestamp;
}

void HomeView::darkTheme(){
    // Theme
    workPanel->darkTheme();
    livePanel->darkTheme();
    
}
void HomeView::lightTheme() {
    // Theme
    livePanel->lightTheme();
    workPanel->lightTheme();
    
    nightmode = false;
}


void HomeView::updateTheme(){
    //Theme
    if (nightmode && !isDarkMode())
        lightTheme();
    if (!nightmode && isDarkMode())
        darkTheme();
}

void HomeView::forceDarkTheme(){
    darkTheme();
}

std::string HomeView::getTickFileName(){
    if (exchange == LUNO_EXCHANGE)
        return "luno_" "XBTZAR"/* + pair + */".csv";
    if (exchange == VALR_EXCHANGE)
        return "valr_" "BTCZAR"/*+ pair +*/ ".csv";
}

LunoHomeView::LunoHomeView (QWidget *parent) : HomeView(LUNO_EXCHANGE, parent) {
    HomeView::exchange = LUNO_EXCHANGE;

    std::string chartUrl = "https://d32exi8v9av3ux.cloudfront.net/static/scripts/tradingview.prod.html?symbol=XBTZAR&res=60&lang=en";
    view->load(QUrl(chartUrl.c_str()));
    
    
    // Note: the next line only works if the browser has already loaded, hence delay
    if (!isDarkMode())
        QTimer::singleShot(4500, this, [this](){
                view->page()->runJavaScript( R"java(
                tvWidget.changeTheme("Light");
                bgcolor(white);
            )java");
        });
        
    view->show();
    
    // Theme
    if (isDarkMode())
        darkTheme();
    else
        lightTheme();
}

LunoHomeView::~LunoHomeView(){
    //
}
void LunoHomeView::darkTheme(){
    // Theme
    HomeView::darkTheme();
    view->page()->runJavaScript( R"javascript(
                                    tvWidget.changeTheme("Dark");
                                    )javascript");
}
void LunoHomeView::lightTheme() {
    // Theme
    HomeView::lightTheme();
    view->page()->runJavaScript( R"javascript(
                                    tvWidget.changeTheme("Light");
                                    )javascript");
}

void LunoHomeView::onOrderBook(const QJsonObject& data) {
    Luno::OrderBook ob;
    ob.timestamp = data["timestamp"].toVariant().toLongLong();
    for (const QJsonValue& v : data["asks"].toArray()) {
        Luno::Order o;
        o.price  = (float)v[0].toDouble();
        o.volume = (float)v[1].toDouble();
        ob.asks.push_back(o);
    }
    for (const QJsonValue& v : data["bids"].toArray()) {
        Luno::Order o;
        o.price  = (float)v[0].toDouble();
        o.volume = (float)v[1].toDouble();
        ob.bids.push_back(o);
    }
    livePanel->orderview << ob.FormatHTMLWith(&lunoOrders);
}

void LunoHomeView::onTrades(const QJsonArray& tradesArray) {
    std::vector<Luno::Trade> newTrades;
    for (const QJsonValue& v : tradesArray) {
        unsigned long long seq = (unsigned long long)v["sequence"].toVariant().toLongLong();
        if (!ticks.empty() && seq <= ticks.back().sequence)
            continue;
        Luno::Trade t;
        t.sequence  = seq;
        t.timestamp = (unsigned long long)v["timestamp"].toVariant().toLongLong();
        t.price     = (float)v["price"].toDouble();
        t.volume    = (float)v["amount"].toDouble();
        t.isBuy     = (v["side"].toString() == "buy");
        newTrades.push_back(t);
    }
    if (newTrades.empty()) return;
    ticks.insert(ticks.end(), newTrades.begin(), newTrades.end());
    *timestamp = ticks.back().timestamp;
    file.open(path + "luno_XBTZAR.csv", std::ios::out | std::ios::app);
    if (file.good()) {
        file << newTrades;
        file.close();
    } else {
        file.clear();
    }
}

void LunoHomeView::refreshOpenOrders() {
    try {
        workPanel->pendingOrders->clearItems();
        lunoOrders = Sidecar::getLunoOpenOrders("XBTZAR");
        std::vector<OrderType*> temp;
        std::for_each(lunoOrders.begin(), lunoOrders.end(), [&temp](Luno::UserOrder& entry){
            temp.push_back(&entry);
        });
        workPanel->pendingOrders->addOrders(&temp);
    } catch (ResponseEx& ex) {
        *TextPanel::textPanel << errorLiner + ex.String().c_str();
    } catch (std::invalid_argument& ex) {
        *TextPanel::textPanel << errorLiner + ex.what();
    }
}

void LunoHomeView::loadLocalTicks(std::string pair){
    if (pair == "DEFAULT")
        pair = "XBTZAR";
    
    
    file.open(path + getTickFileName() , std::ios::in);
    
    if (file.good()){
        file >> moreticks; // <- may take extremely long
        file.close();
        
        if (!closing && moreticks.size() > 0) {
            unsigned long long now = QDateTime::currentMSecsSinceEpoch();
            size_t i  = 0;
            while (i < moreticks.size()
            && (moreticks[i].timestamp  <= (now - 60 * 60 * 1000))){
                i++;
            }
                
            ticks.insert(ticks.end(), moreticks.begin() + i, moreticks.end());
            
            moreticks.clear();
            
            
            for (int i = 0; ticks.size() > 0 && i < (int)ticks.size()-2; i++){
                // ensure ordered oldest to newest
                if (ticks[i].sequence +1 != ticks[i+1].sequence){
                    do {
                        ticks.pop_back();
                    } while (i < (int)ticks.size() - 2);
                }
            }
              
            // empty file and replace content
            file.open(path + "luno_" + pair + ".csv", std::ofstream::out | std::ofstream::trunc);
            file << ticks;
            file.close();
            
        }
        if (!closing && ticks.size() > 0){
            *timestamp = ticks.back().timestamp;
        }
        else
            *timestamp = QDateTime::currentMSecsSinceEpoch();
    }
    else {
        *timestamp = QDateTime::currentMSecsSinceEpoch();
        file.clear();
    }
}


std::string LunoHomeView::lastTrades() {
    std::stringstream ss;
    ss << std::fixed;
    ss << R"html(
            <style>
            table {width: 100%;}
            tr { padding: 15px;}
            a {
                color: inherit;
                text-decoration: none;
            }
            td {
                padding: 2px 1px 1px 1px;
                text-align: center;
                font-size: 14px;
                font-weight: 700;
            }
            .Ask a {color: rgb(192, 51, 35);}
            .Bid a {color: rgb(54, 136, 87);}
            </style>
            <table width=100%>
    )html";
    
    
    for (int i = ((int)ticks.size()) -1, limit = 1000; i >= 0 && limit >= 0; i--, limit--){
        ss << "\n<tr> <a href=\"" << ticks[i].price << "\">";
        ss << "\n<td>" << QDateTime::fromMSecsSinceEpoch(ticks[i].timestamp).toString("hh:mm").toStdString() << "</td>";
        ss << (ticks[i].isBuy ? "\n<td class=Ask>" : "\n<td class=Bid>" ) ;
        ss << std::setprecision(0);
        ss << "<a href=\"" << ticks[i].price << "\">";
        ss  << ticks[i].price;
        ss << "</a></td>";
        ss << "\n<td>" << std::setprecision(6)<< ticks[i].volume << "</td>";
        ss << "\n</a></tr>";
        
    }
    ss << "</table>\n";
    
    return ss.str();
}

VALRHomeView::VALRHomeView (QWidget *parent) : HomeView(VALR_EXCHANGE, parent) {
    HomeView::exchange = VALR_EXCHANGE;

    std::string chartUrl = "https://www.valr.com/exchange/BTC/ZAR";
    view->load(QUrl(chartUrl.c_str()));
    
    
    // Note: the next line only works if the browser has already loaded, hence delay
    if (!isDarkMode())
        QTimer::singleShot(4500, this, [this](){
                view->page()->runJavaScript( R"java(
                document.getElementById("root").replaceWith(document.getElementById("tvChartWidget"));
            )java");
        });
        
    view->show();
    
    // Theme
    if (isDarkMode())
        darkTheme();
    else
        lightTheme();
}

VALRHomeView::~VALRHomeView(){
    //
}
void VALRHomeView::darkTheme(){
    // Theme
    HomeView::darkTheme();
}
void VALRHomeView::lightTheme() {
    // Theme
    HomeView::lightTheme();
}
void VALRHomeView::onOrderBook(const QJsonObject& data) {
    VALR::OrderBook ob;
    ob.timestamp = (unsigned long long)data["timestamp"].toVariant().toLongLong();
    for (const QJsonValue& v : data["asks"].toArray()) {
        VALR::Order o;
        o.price  = (float)v[0].toDouble();
        o.volume = (float)v[1].toDouble();
        o.count  = 0;
        ob.asks.push_back(o);
    }
    for (const QJsonValue& v : data["bids"].toArray()) {
        VALR::Order o;
        o.price  = (float)v[0].toDouble();
        o.volume = (float)v[1].toDouble();
        o.count  = 0;
        ob.bids.push_back(o);
    }
    livePanel->orderview << ob.FormatHTMLWith(&valrOrders);
}

void VALRHomeView::onTrades(const QJsonArray& tradesArray) {
    std::vector<VALR::Trade> newTrades;
    for (const QJsonValue& v : tradesArray) {
        unsigned long long seq = (unsigned long long)v["sequence"].toVariant().toLongLong();
        if (!ticks.empty() && seq <= ticks.back().sequence)
            continue;
        VALR::Trade t;
        t.sequence    = seq;
        t.timestamp   = (unsigned long long)v["timestamp"].toVariant().toLongLong();
        t.price       = (float)v["price"].toDouble();
        t.baseVolume  = (float)v["amount"].toDouble();
        t.quoteVolume = t.price * t.baseVolume;
        t.pair        = "BTCZAR";
        t.id          = "";
        t.isBuy       = (v["side"].toString() == "buy");
        newTrades.push_back(t);
    }
    if (newTrades.empty()) return;
    ticks.insert(ticks.end(), newTrades.begin(), newTrades.end());
    *timestamp = ticks.back().timestamp;
    file.open(path + "valr_BTCZAR.csv", std::ios::out | std::ios::app);
    if (file.good()) {
        file << newTrades;
        file.close();
    } else {
        file.clear();
    }
}

void VALRHomeView::loadLocalTicks(std::string pair){
    if (pair == "DEFAULT")
        pair = "BTCZAR";
    
    file.open(path + getTickFileName() , std::ios::in);
    
    if (file.good()){
        file >> moreticks; // <- may take extremely long
        file.close();
        
        if (!closing && moreticks.size() > 0) {
            unsigned long long now = QDateTime::currentMSecsSinceEpoch();
            size_t i  = 0;
            const unsigned long long hour = 60ull * 60ull * 1000ull;
            while (i < moreticks.size()
            && (moreticks[i].timestamp  <= (now - hour))){
                i++;
            }
                
            ticks.insert(ticks.end(), moreticks.begin() + i, moreticks.end());
            
            moreticks.clear();
            
            
            for (int i = 0; ticks.size() > 0 && i < (int)ticks.size()-2; i++){
                // ensure ordered oldest to newest
                if (ticks[i].sequence +1 != ticks[i+1].sequence){
                    do {
                        ticks.pop_back();
                    } while (i < (int)ticks.size() - 2);
                }
            }
              
            // empty file and replace content
            file.open(path + "valr_" + pair + ".csv", std::ofstream::out | std::ofstream::trunc);
            file << ticks;
            file.close();
            
        }
        if (!closing && ticks.size() > 0){
            *timestamp = ticks.back().timestamp;
        }
        else
            *timestamp = QDateTime::currentMSecsSinceEpoch() ; 
    }
    else {
        *timestamp = QDateTime::currentMSecsSinceEpoch();
        file.clear();
    }
}



std::string VALRHomeView::lastTrades() {
    std::stringstream ss;
    ss << std::fixed;
    ss << R"html(
            <style>
            table {width: 100%;}
            tr { padding: 15px;}
            a {
                color: inherit;
                text-decoration: none;
            }
            td {
                padding: 2px 1px 1px 1px;
                text-align: center;
                font-size: 14px;
                font-weight: 700;
            }
            .Ask a {color: rgb(192, 51, 35);}
            .Bid a {color: rgb(54, 136, 87);}
            </style>
            <table width=100%>
    )html";
    
    
    for (int i = ((int)ticks.size()) -1, limit = 1000; i >= 0 && limit >= 0; i--, limit--){
        ss << "\n<tr> <a href=\"" << ticks[i].price << "\">";
        ss << "\n<td>" << QDateTime::fromMSecsSinceEpoch(ticks[i].timestamp).toString("hh:mm").toStdString() << "</td>";
        ss << (ticks[i].isBuy ? "\n<td class=Ask>" : "\n<td class=Bid>" ) ;
        ss << std::setprecision(0);
        ss << "<a href=\"" << ticks[i].price << "\">";
        ss  << ticks[i].price;
        ss << "</a></td>";
        ss << "\n<td>" << std::setprecision(6)<< ticks[i].quoteVolume << "</td>";
        ss << "\n</a></tr>";
        
    }
    ss << "</table>\n";
    
    return ss.str();
}

/*
  note to self: this is not the best way to construct batch jobs, but remember it so you can improve it
     std::vector<std::string> batch;
     batch.push_back(VALR::VALRClient::formMarketPayload("BTCZAR", "ASK", 100, false, true));
     batch.push_back(VALR::VALRClient::formLimitPayload("BTCZAR", "BID", 0.002, 100000, true));
     batch.push_back(VALR::VALRClient::formLimitPayload("ETHZAR", "ASK", 0.2, 32000, true));
     batch.push_back(VALR::VALRClient::formStopLimitPayload("BTCZAR", "ASK", 0.002, 100000, 110000, false, true));
     batch.push_back(VALR::VALRClient::formStopLimitPayload("BTCZAR", "ASK", 0.0003, 1150000, 110000, true, true));
     batch.push_back(VALR::VALRClient::formStopLimitPayload("BTCZAR", "ASK", 0.00000002, 100000, 110000, true, true));
     batch.push_back(VALR::VALRClient::formCancelOrderPayload("ETHZAR", "e5886f2d-191b-4330-a221-c7b41b0bc553", false, true));

     std::string batchPayload = VALR::VALRClient::packBatchPayloadFromList(batch);

     *home->workPanel->text  << VALR::VALRClient::postBatchOrders(batchPayload);
 */
