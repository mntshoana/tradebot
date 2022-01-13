#include "window.hpp"
#include <QTimer>

HomeView::HomeView (QWidget *parent, int exchange) : QWidget(parent) {
    this->timestamp = new unsigned long long();
    this->exchange = exchange;
    
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


LunoHomeView::LunoHomeView (QWidget *parent) : HomeView(parent) {
    this->exchange = LUNO_EXCHANGE;

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

Task* LunoHomeView::toUpdateOrderBook(std::string pair) {
    if (pair == "DEFAULT")
        pair = "XBTZAR";
    Task* job = new Task( [this, pair]() {
        Luno::OrderBook orderBook = Luno::LunoClient::getOrderBook(pair);
        livePanel->orderview << orderBook.FormatHTMLWith(&lunoOrders);
    });
    job->updateWaitTime(2);
    job->setRepeat(true);
    return job;
}

Task* LunoHomeView::toUpdateOpenUserOrders() {
    Task* job = new Task( [this]() {
        workPanel->pendingOrders->clearItems();
        lunoOrders = Luno::LunoClient::getUserOrders("XBTZAR", "PENDING");
        
        std::vector<OrderType*> temp;
        std::for_each(lunoOrders.begin(), lunoOrders.end(), [&temp](Luno::UserOrder& entry){
            temp.push_back( &entry);
        });
        workPanel->pendingOrders->addOrders(&temp);
        temp.clear();
    });
    job->updateWaitTime(5);
    job->wait = 0; // execute one time immediately before waiting
    job->setRepeat(true);
    return job;
}

Task* LunoHomeView::toAppendOpenUserOrder(std::string orderID) {
    Task* job = new Task( [this, orderID]() {
        Luno::UserOrder details = Luno::LunoClient::getOrderDetails(orderID);
        
        TextPanel::textPanel << details;
        std::vector<OrderType*> temp;
        
        temp.push_back( &details);
        
        workPanel->pendingOrders->addOrders(&temp);
        temp.clear();
    });
    
    return job;
}

void LunoHomeView::loadLocalTicks(){
    
    file.open(path + "XBTZAR.csv" , std::ios::in);
    
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
            file.open(path + "XBTZAR.csv", std::ofstream::out | std::ofstream::trunc);
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

void LunoHomeView::downloadTicks(std::string pair){
    moreticks = Luno::LunoClient::getTrades(pair, *timestamp); // order = newest to oldest
    while (ticks.size() > 0
            && moreticks.size() > 0
            && moreticks.back().sequence <= ticks.back().sequence)
        moreticks.pop_back();
    
    std::reverse(moreticks.begin(), moreticks.end()); // order = oldest to newest
    
    for (int i = 0; moreticks.size() > 0 && i < ((int) moreticks.size()) -2; i++){
        // ensure ordered oldest to newest sequence is in tact
        if (moreticks[i].sequence +1 != moreticks[i+1].sequence){
            do {
                moreticks.pop_back();
            } while (i < ((int) moreticks.size()) -2);
        }
    }
    
    if (moreticks.size() > 0){
        ticks.insert(ticks.end(), moreticks.begin(), moreticks.end());
        *timestamp = ticks.back().timestamp;
        file.open( path + pair + ".csv", std::ios::out | std::ios::app);
        if (file.good()){
            file << moreticks;
            file.close();
        }
        else {
            file.clear();
            *TextPanel::textPanel << std::string("[Error] : At ")
                            + __FILE__ + ": line " + std::to_string(__LINE__)
                        + ". Couldn't write ticks to file.";
        }
        moreticks.clear();
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
    
    
    for (int i = ticks.size() -1, limit = 1000; i >= 0 && limit >= 0; i--, limit--){
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

Task* LunoHomeView::toDownloadTicks(std::string pair){
    if (pair == "DEFAULT")
        pair = "XBTZAR";
    Task* job = new Task( [this, pair]() {downloadTicks(pair);}, false);
    job->updateWaitTime(3);
    job->setRepeat(true);
    return job;
}

VALRHomeView::VALRHomeView (QWidget *parent) : HomeView(parent) {
    this->exchange = VALR_EXCHANGE;

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
Task* VALRHomeView::toUpdateOrderBook(std::string pair) {
    if (pair == "DEFAULT")
        pair = "BTCZAR";
    Task* job = new Task( [this, pair]() {
        VALR::OrderBook orderBook = VALR::VALRClient::getOrderBook(pair, true);
        /*
        // TODO: Preferred method should use list of currently open orders
        std::vector<VALR::OrderBook>* currentOrders; // = &(workPanel->pendingOrders->openUserOrders);
        livePanel->orderview << orderBook.FormatHTMLWith(currentOrders);
         */
        livePanel->orderview << orderBook.FormatHTML();
    });
    
    job->updateWaitTime(2);
    job->setAsFast();
    return job;
}

Task* VALRHomeView::toUpdateOpenUserOrders() {
    Task* job = new Task( [this]() {
        // TODO: this, but for VALR
        *TextPanel::textPanel << "TODO: toUpdateOpenUserOrders but for VALR";
        /*
        workPanel->pendingOrders->clearItems();
        lunoOrders = Luno::LunoClient::getUserOrders("XBTZAR", "PENDING");
        
        std::vector<OrderType*> temp;
        std::for_each(lunoOrders.begin(), lunoOrders.end(), [&temp](Luno::UserOrder& entry){
            temp.push_back( &entry);
        });
        workPanel->pendingOrders->addOrders(&temp);
        temp.clear();
         */
    });
    job->updateWaitTime(5);
    job->wait = 0; // execute one time immediately before waiting
    job->setRepeat(true);
    return job;
}

Task* VALRHomeView::toAppendOpenUserOrder(std::string orderID) {
    Task* job = new Task( [this, orderID]() {
        // TODO: this, but for VALR
        *TextPanel::textPanel << "TODO: toAppendOpenUserOrder but for VALR";
        /*
        Luno::UserOrder details = Luno::LunoClient::getOrderDetails(orderID);
        
        TextPanel::textPanel << details;
        std::vector<OrderType*> temp;
        
        temp.push_back( &details);
        
        workPanel->pendingOrders->addOrders(&temp);
        temp.clear();
         */
    });
    
    return job;
}

void VALRHomeView::loadLocalTicks(){
    *TextPanel::textPanel << "TO DO LOAD LOCAL TICKS";
    /*
    file.open(path + "XBTZAR.csv" , std::ios::in);
    
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
            file.open(path + "XBTZAR.csv", std::ofstream::out | std::ofstream::trunc);
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
    }*/
}

void VALRHomeView::downloadTicks(std::string pair){
    *TextPanel::textPanel << "TO DO LOAD LOCAL TICKS";
    /*
    moreticks = Luno::LunoClient::getTrades(pair, *timestamp); // order = newest to oldest
    while (ticks.size() > 0
            && moreticks.size() > 0
            && moreticks.back().sequence <= ticks.back().sequence)
        moreticks.pop_back();
    
    std::reverse(moreticks.begin(), moreticks.end()); // order = oldest to newest
    
    for (int i = 0; moreticks.size() > 0 && i < ((int) moreticks.size()) -2; i++){
        // ensure ordered oldest to newest sequence is in tact
        if (moreticks[i].sequence +1 != moreticks[i+1].sequence){
            do {
                moreticks.pop_back();
            } while (i < ((int) moreticks.size()) -2);
        }
    }
    
    if (moreticks.size() > 0){
        ticks.insert(ticks.end(), moreticks.begin(), moreticks.end());
        *timestamp = ticks.back().timestamp;
        file.open( path + pair + ".csv", std::ios::out | std::ios::app);
        if (file.good()){
            file << moreticks;
            file.close();
        }
        else {
            file.clear();
            *TextPanel::textPanel << std::string("[Error] : At ")
                            + __FILE__ + ": line " + std::to_string(__LINE__)
                        + ". Couldn't write ticks to file.";
        }
        moreticks.clear();
    }
     */
}

std::string VALRHomeView::lastTrades() {
    std::stringstream ss;
    /*ss << std::fixed;
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
    
    
    for (int i = ticks.size() -1, limit = 1000; i >= 0 && limit >= 0; i--, limit--){
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
    */
    ss << "STILL TO DO";
    return ss.str();
}
Task* VALRHomeView::toDownloadTicks(std::string pair) {
    if (pair == "DEFAULT")
        pair = "BTCZAR";
    Task* job = new Task( []() {/*downloadTicks(pair);}, false*/});
    job->updateWaitTime(3);
    job->setRepeat(true);
    return job;
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
