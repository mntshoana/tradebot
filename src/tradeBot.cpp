#include "tradeBot.hpp"

#define CSV_FILE_PATH "/Users/macgod/Dev/tradebot2/tradebot/src/data/"

// Constructor
TradeBot::TradeBot (QWidget *parent ) : QWidget(parent) {
    current = home = new HomeView(this);
    p2p = nullptr;
    
    connect(home->orderPanel->request,
            &QPushButton::clicked, this,[this](){
        try {
        if (home->orderPanel->isBuy)
            *(home->text) << lunoClient.postLimitOrder("XBTZAR", "BID", atof(home->orderPanel->txtAmount->text().toStdString().c_str()),
                atoi(home->orderPanel->txtPrice->text().toStdString().c_str()));
       else
           *(home->text) << lunoClient.postLimitOrder("XBTZAR", "ASK", atof(home->orderPanel->txtAmount->text().toStdString().c_str()),
               atof(home->orderPanel->txtPrice->text().toStdString().c_str()));
        }
        catch (ResponseEx ex){
            *(home->text) << ex.String();
        }
    });
    connect(this, &TradeBot::finishedUpdate, this, &TradeBot::OnFinishedUpdate);
    connect(home->chartPanel->timeframe, &QComboBox::currentTextChanged,
        this, [this](const QString &str){
        *(home->text) << str.toStdString();
        home->chartPanel->loadChart(ticks.begin(), ticks.end());
        home->chartPanel->chart->update();
        home->chartPanel->update();
    });
    
    timestamp = new unsigned long long(0);
    latestTimestamp = new unsigned long long(0);
    
    timer = new QTimer(this);
    timer->setSingleShot(true);
    connect(timer, &QTimer::timeout, this, &TradeBot::OnUpdate);
    timerCount = new size_t(0); // counts the timeouts triggered by timer
    timer->start(100);
    
    connect(home->chartPanel->P2Pbutton, &QPushButton::clicked, this, [this]() {
        timer->stop();
        delete home;
        home = nullptr;
        p2p = new P2PView(this);
        current = p2p;
        
    });
    Job test(&lunoClient, home->text, &Luno::LunoClient::getTickers);
    Job1 test2(&lunoClient, home->text, &Luno::LunoClient::getOrderBook, std::string("XBTZAR"), &Luno::OrderBook::Format);
    manager.enqueue(new Job(test), true);
    manager.enqueue(new Job1(test2), true);
}

TradeBot::~TradeBot() {
    delete timerCount;
    timerCount = nullptr;
    
    delete timestamp;
    timestamp = nullptr;
}

void TradeBot::OnFinishedUpdate(){
    if (*timerCount == 0){
        home->chartPanel->loadChart(ticks.begin(), ticks.end());
        home->chartPanel->chart->update();
        home->chartPanel->update();
    }
    *timerCount = *timerCount +1;
    timer->start(1000);
}
void TradeBot::OnUpdate() {
    timer->stop();
    if (*timerCount > 60) {
        *timerCount = 1;
    }
    
    if (*timerCount == 0) { // Initiate app
        try{
            *(home->orderPanel->orderview) << lunoClient.getOrderBook("XBTZAR").Format();
            auto step = home->orderPanel->orderview->verticalScrollBar()->singleStep();
            home->orderPanel->orderview->verticalScrollBar()->setValue(step * 97.8);
        }
        catch (ResponseEx ex){
            *(home->text) << ex.String();
        }
        
        home->orderPanel->tradeview->setHtml(lastTrades().c_str());
        
        try {
            std::this_thread::sleep_for(std::chrono::milliseconds(1100));
            *latestTimestamp = lunoClient.getTicker("XBTZAR").timestamp;
        }
        catch (ResponseEx ex){
            *(home->text) << ex.String();
        }
        /*
        try {
            //*(homeWindow->text) << LocalBclient.getBuyAds("cn", "China");
            
            *(homeWindow->text) << lunoClient.getWithdrawal("11495685");
        }
        catch (ResponseEx ex){
            *(homeWindow->text) << ex.String();
        }*/
         
        home->orderPanel->tradeview->setHtml(lastTrades().c_str());
        thread = std::thread([this]{
            loadLocalTicks();
            emit finishedUpdate();
        });
        thread.detach();
    }
    else if (*timerCount % 5 == 0){
        try{
            auto y = home->orderPanel->orderview->verticalScrollBar()->value();
            std::string update = lunoClient.getOrderBook("XBTZAR").Format();
            home->orderPanel->orderview->setHtml("");
            *(home->orderPanel->orderview) << update;
            if (y != 0)
                home->orderPanel->orderview->verticalScrollBar()->setValue(y);
            else{
                auto step = home->orderPanel->orderview->verticalScrollBar()->singleStep();
                home->orderPanel->orderview->verticalScrollBar()->setValue(step * 90);
            }
        }
        catch (ResponseEx ex){
            *(home->text) << ex.String();
        }
        
        auto y = home->orderPanel->tradeview->verticalScrollBar()->value();
        home->orderPanel->tradeview->setHtml(lastTrades().c_str());
        home->orderPanel->tradeview->verticalScrollBar()->setValue(y);
        
        if (*timerCount % 30 == 0){
            //Theme
            home->updateTheme();
            //Check errors
            if (*latestTimestamp == 0){
                try {
                    std::this_thread::sleep_for(std::chrono::milliseconds(1100));
                    *latestTimestamp = lunoClient.getTicker("XBTZAR").timestamp;
                }
                catch (ResponseEx ex){
                    *(home->text) << ex.String();
                }
            }
        }
        emit finishedUpdate();
    }
    else if (*timerCount % 2 == 1 ){

        auto y = home->orderPanel->tradeview->verticalScrollBar()->value();
        home->orderPanel->tradeview->setHtml(lastTrades().c_str());
        home->orderPanel->tradeview->verticalScrollBar()->setValue(y);
        
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
            *(home->text) << ex.String();
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(1100));
    }
    try{
        downloadTicks();
    }
    catch (ResponseEx ex){
        *(home->text) << ex.String();
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
