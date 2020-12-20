#include "TradeBot.hpp"

#define CSV_FILE_PATH "/Users/macgod/Dev/tradebot2/tradebot/src/data/"

QTextEdit& operator<< (QTextEdit& stream, std::string str)
{
    stream.append(str.c_str());
    return stream;
}
QTextBrowser& operator<< (QTextBrowser& stream, std::string str)
{
    stream.append(str.c_str());
    return stream;
}

// Constructor
TradeBot::TradeBot (QWidget *parent ) : QWidget(parent) {
    text = new QTextEdit(this);
    text->setGeometry(0, 500, 1180, 220);
    text->setText("");

    orderPanel = new OrderPanel(this);
    connect(orderPanel->request, &QPushButton::clicked, this,[this](){
        try {
        if (orderPanel->isBuy)
            *text << lunoClient.postLimitOrder("XBTZAR", "BID", atof(orderPanel->txtAmount->text().toStdString().c_str()),
                atoi(orderPanel->txtPrice->text().toStdString().c_str()));
       else
           *text << lunoClient.postLimitOrder("XBTZAR", "ASK", atof(orderPanel->txtAmount->text().toStdString().c_str()),
               atof(orderPanel->txtPrice->text().toStdString().c_str()));
        }
        catch (ResponseEx ex){
            *text << ex.String();
        }
    });
    
    chartPanel = new ChartPanel(this);
    
    connect(this, &TradeBot::finishedUpdate, this, &TradeBot::OnFinishedUpdate);
    connect(chartPanel->timeframe, &QComboBox::currentTextChanged,
        this, [this](const QString &str){*text << str.toStdString();});
    
    // Theme
    if (isDarkMode())
        darkTheme();
    else
        lightTheme();
    
    timestamp = new unsigned long long(0);
    latestTimestamp = new unsigned long long(0);
    
    timer = new QTimer(this);
    timer->setSingleShot(true);
    connect(timer, &QTimer::timeout, this, &TradeBot::OnUpdate);
    count = new size_t(0); // counts the timeouts triggered by timer
    timer->start(100);
}

TradeBot::~TradeBot() {
    delete count;
    count = nullptr;
    
    delete timestamp;
    timestamp = nullptr;
    
    delete high; delete low; delete open; delete close; delete limit;
    high = low = open = close = limit = nullptr;
}

void TradeBot::darkTheme(){
    // Theme
    QColor chartBackground(30,30,30);
    QColor light(230,230,230);
    QColor dark(50,50,50);
    QColor darker(25,25,25);
    orderPanel->livetradeview->setStyleSheet(R"(QGroupBox {
                                        background-color: #1e1e1e;
                                        color: white;
                                        border: none;
                                 } QGroupBox::title {
                                        background-color:transparent;
                                 })");
    QPalette p = chartPanel->palette();
    p.setColor(QPalette::Window, darker);
    chartPanel->setPalette(p);
    p.setColor(QPalette::Window, darker);
    chartPanel->chart->setPalette(p);
    nightmode = true;
}
void TradeBot::lightTheme(){
    // Theme
    QColor light(253,253,253);
    QBrush dark(QColor(20,20,20));
    
    orderPanel->livetradeview->setStyleSheet(R"(QGroupBox {
                                        background-color: white;
                                        color: black;
                                        border: none;
                                 } QGroupBox::title {
                                        background-color:transparent;
                                 })");
    
    QPalette p = chartPanel->palette();
    p.setColor(QPalette::Window, Qt::white);
    chartPanel->setPalette(p);
    p.setColor(QPalette::Window, light);
    chartPanel->chart->setPalette(p);
    nightmode = false;
}

void TradeBot::OnFinishedUpdate(){
    if (*count == 0)
        chartPanel->loadChart(ticks.begin(), ticks.end());
    
    *count = *count +1;
    timer->start(1000);
}
void TradeBot::OnUpdate() {
    timer->stop();
    if (*count > 60) {
        *count = 1;
    }
    
    if (*count == 0) { // Initiate app
        try{
            *(orderPanel->orderview) << lunoClient.getOrderBook("XBTZAR").Format();
            auto step = orderPanel->orderview->verticalScrollBar()->singleStep();
            orderPanel->orderview->verticalScrollBar()->setValue(step * 97.8);
        }
        catch (ResponseEx ex){
            *text << ex.String();
        }
        
        orderPanel->tradeview->setHtml(lastTrades().c_str());
        
        try {
            std::this_thread::sleep_for(std::chrono::milliseconds(1100));
            *latestTimestamp = lunoClient.getTicker("XBTZAR").timestamp;
        }
        catch (ResponseEx ex){
            *text << ex.String();
        }
        /*
        try {
            *text << LocalBclient.getBuyAds("cn", "China");
        }
        catch (ResponseEx ex){
            *text << ex.String();
        }*/
         
        orderPanel->tradeview->setHtml(lastTrades().c_str());
        thread = std::thread([this]{
            loadLocalTicks();
            emit finishedUpdate();
        });
        thread.detach();
    }
    else if (*count % 5 == 0){
        try{
            auto y = orderPanel->orderview->verticalScrollBar()->value();
            std::string update = lunoClient.getOrderBook("XBTZAR").Format();
            orderPanel->orderview->setHtml("");
            *(orderPanel->orderview) << update;
            if (y != 0)
                orderPanel->orderview->verticalScrollBar()->setValue(y);
            else{
                auto step = orderPanel->orderview->verticalScrollBar()->singleStep();
                orderPanel->orderview->verticalScrollBar()->setValue(step * 90);
            }
        }
        catch (ResponseEx ex){
            *text << ex.String();
        }
        
        auto y = orderPanel->tradeview->verticalScrollBar()->value();
        orderPanel->tradeview->setHtml(lastTrades().c_str());
        orderPanel->tradeview->verticalScrollBar()->setValue(y);
        
        //chartPanel->loadChart(ticks.begin(), ticks.end());
        
        if (*count % 30 == 0){
            //Theme
            if (nightmode && !isDarkMode())
                lightTheme();
            if (!nightmode && isDarkMode())
                darkTheme();
            //Check errors
            if (*latestTimestamp == 0){
                try {
                    std::this_thread::sleep_for(std::chrono::milliseconds(1100));
                    *latestTimestamp = lunoClient.getTicker("XBTZAR").timestamp;
                }
                catch (ResponseEx ex){
                    *text << ex.String();
                }
            }
        }
        emit finishedUpdate();
    }
    else if (*count % 2 == 1 ){

        auto y = orderPanel->tradeview->verticalScrollBar()->value();
        orderPanel->tradeview->setHtml(lastTrades().c_str());
        orderPanel->tradeview->verticalScrollBar()->setValue(y);
        
        if (*latestTimestamp == 0 || *latestTimestamp < *timestamp) {
            // latestTimestamp could be 0 (by internet error)
            // or
            // offline data is indeed up to date
            thread = std::thread([this]{downloadTicks(1);} );
            thread.detach();
        }
        else {
            // offline data is not up to date
            thread = std::thread([this]{downloadTicks(20);} );
            thread.detach();
        }
    }
    else {
        emit finishedUpdate();
    }
}

void TradeBot::loadLocalTicks(){
    file.open(std::string(CSV_FILE_PATH) + "XBTZAR.csv" , std::ios::in);
    if (file.good()){
        file >> ticks;
        if (ticks.size() > 0){
            if (ticks.back().sequence != ticks.size())
            {
                while (ticks.size() > 0 && ticks.back().sequence != ticks.size())
                    ticks.pop_back();
                file.close();
                file.open(CSV_FILE_PATH, std::ios::out | std::ios::app);
                file << ticks;
                file.close();
                remove( (std::string(CSV_FILE_PATH) + "XBTZAR.csv").c_str() );
                rename( (std::string(CSV_FILE_PATH) + "XBTZAR2.csv").c_str(),
                        (std::string(CSV_FILE_PATH) + "XBTZAR.csv" ).c_str());
            }
        }
        if (ticks.size() > 0){
            *timestamp = ticks.back().timestamp;
        }
        else
            *timestamp = QDate(2013, 1, 1).startOfDay().toMSecsSinceEpoch();
    }
    else {
        *timestamp = QDate(2013, 1, 1).startOfDay().toMSecsSinceEpoch();
        file.clear();
    }
    file.close();
}

void TradeBot::downloadTicks(){
    moreticks = lunoClient.getTrades("XBTZAR", *timestamp-1); // order = newest to oldest
    while (ticks.size() > 0
            && moreticks.size() > 0
            && moreticks.back().sequence <= ticks.back().sequence)
        moreticks.pop_back();
    std::reverse(moreticks.begin(), moreticks.end()); // order = oldest to newest
    while (moreticks.size() > 0
            && moreticks.back().sequence != ticks.size()+moreticks.size())
            moreticks.pop_back();
    // Ensure sequenctial timestamp
    for (unsigned short index = 0; index < moreticks.size(); index++){
        if (moreticks[index].timestamp < *timestamp)
                moreticks.erase( moreticks.begin() + index, moreticks.end());
    }
    if (moreticks.size() > 0){
        ticks.insert(ticks.end(), moreticks.begin(), moreticks.end());
        *timestamp = moreticks.back().timestamp;
        file.open( std::string(CSV_FILE_PATH) + "XBTZAR.csv", std::ios::out | std::ios::app);
        file << moreticks;
        file.close();
    }
    moreticks.clear();
}

void TradeBot::downloadTicks(size_t reps){
    for (size_t i = 1; i < reps; i++){
        try{
            downloadTicks();
        }
        catch (ResponseEx ex){
            *text << ex.String();
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(1100));
    }
    try{
        downloadTicks();
    }
    catch (ResponseEx ex){
        *text << ex.String();
    }
    emit finishedUpdate();
}

std::string TradeBot::lastTrades() {
    std::stringstream ss;
    ss << std::fixed;
    ss << R"(
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
            }
            .Ask a {color: rgb(222, 81, 65);}
            .Bid a {color: rgb(94, 186, 137);}
            </style>
            <table width=100%>)";
    
    if (ticks.size() > 0){
        
        for (size_t i = ticks.size() - 1; i >= ticks.size()- 21; i--){
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
    }
    return ss.str();
}
