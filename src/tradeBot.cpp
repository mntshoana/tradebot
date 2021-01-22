#include "tradeBot.hpp"

#define CSV_FILE_PATH "/Users/macgod/Dev/tradebot2/tradebot/src/data/"

// Constructor
TradeBot::TradeBot (QWidget *parent ) : QWidget(parent) {
    p2p = nullptr; // peer to peer window, not active
    current = home = new HomeView(this, &lunoClient); // active home screen window
    
    // on update event
    connect(this, &TradeBot::finishedUpdate,
            this, &TradeBot::OnFinishedUpdate);
    
    // home window timeframe Combo Box text changed event
    connect(home->chartPanel->timeframe, &QComboBox::currentTextChanged,
        this, [this](const QString &str){
        *(home->text) << str.toStdString();
        home->chartPanel->loadChart(ticks.begin(), ticks.end());
        home->chartPanel->chart->update();
        home->chartPanel->update();
    });
    
    timestamp = new unsigned long long(0);
    latestTimestamp = new unsigned long long(0);
    loadingTicks = false;
    
    timer = new QTimer(this);
    timer->setSingleShot(true);
    connect(timer, &QTimer::timeout, this, &TradeBot::OnUpdate);
    timerCount = new size_t(0); // counts the timeouts triggered by timer
    timer->start(100);
    
    connect(home->chartPanel->P2Pbutton, &QPushButton::clicked, this, [this]() {
        timer->stop();
        manager.stop();
        delete home;
        home = nullptr;
        p2p = new P2PView(this);
        current = p2p;
        
    });
    
    // begin job manager
    manager.enqueue(new Job1(&lunoClient,
                             home->orderPanel->orderview,
                             &Luno::LunoClient::getOrderBook,
                             std::string("XBTZAR"),
                             &Luno::OrderBook::Format),
                    true);
    
    manager.enqueue(new Job1(&lunoClient,
                             latestTimestamp,
                             &Luno::LunoClient::getTicker,
                             std::string("XBTZAR"),
                             &Luno::Ticker::getTimestamp,
                             home->text, false),
                    true);
    manager.enqueue(new func1(this,
                             &TradeBot::downloadTicks,
                             std::string("XBTZAR"),
                             home->text),
                    true);
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
    
    if (*timerCount == 0) { // Initiate when count == 0
        /*
        try {
            // *(homeWindow->text) << LocalBclient.getBuyAds("cn", "China");
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
        auto y = home->orderPanel->tradeview->verticalScrollBar()->value();
        home->orderPanel->tradeview->setHtml(lastTrades().c_str());
        home->orderPanel->tradeview->verticalScrollBar()->setValue(y);
        
        if (*timerCount % 30 == 0){
            //Theme
            current->updateTheme();
            manager.enqueue(new Job1(&lunoClient,
                                     latestTimestamp,
                                     &Luno::LunoClient::getTicker,
                                     std::string("XBTZAR"),
                                     &Luno::Ticker::getTimestamp,
                                     home->text, false),
                            true);
        }
        emit finishedUpdate();
    }
    else if (*timerCount % 2 == 1 ){

        auto y = home->orderPanel->tradeview->verticalScrollBar()->value();
        home->orderPanel->tradeview->setHtml(lastTrades().c_str());
        home->orderPanel->tradeview->verticalScrollBar()->setValue(y);
        
        if (latestTimestamp && *latestTimestamp < *timestamp)
            // data is outdated, maybe show different color
            ;
    }
    else {
        emit finishedUpdate();
    }
}

void TradeBot::loadLocalTicks(){
    loadingTicks = true;
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
    loadingTicks = false;
}

void TradeBot::downloadTicks(std::string pair){
    if (loadingTicks)
        return;
    
    moreticks = lunoClient.getTrades(pair, *timestamp-1); // order = newest to oldest
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
        file.open( std::string(CSV_FILE_PATH) + pair + ".csv", std::ios::out | std::ios::app);
        file << moreticks;
        file.close();
    }
    moreticks.clear();
}

void TradeBot::downloadTicks(std::string pair, size_t reps){
    for (size_t i = 1; i < reps; i++){
        try{
            downloadTicks(pair);
        }
        catch (ResponseEx ex){
/////            *(home->text) << ex.String();
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
    try{
        downloadTicks(pair);
    }
    catch (ResponseEx ex){
/////        *(home->text) << ex.String();
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
