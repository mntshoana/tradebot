#include "tradeBot.hpp"

// Constructor
TradeBot::TradeBot (QWidget *parent ) : QWidget(parent) {
    p2p = nullptr; // peer to peer window, not active
    current = home = new HomeView(this, &lunoClient); // active home screen window
    
    // on update event
    connect(this, &TradeBot::finishedUpdate,
            this, &TradeBot::OnFinishedUpdate);
    
    
    timestamp = new unsigned long long(0);
    latestTimestamp = new unsigned long long(0);
    loadingTicks = false;
    
    timer = new QTimer(this);
    timer->setSingleShot(true);
    connect(timer, &QTimer::timeout, this, &TradeBot::OnUpdate);
    timerCount = new size_t(0); // counts the timeouts triggered by timer
    timer->start(100);
    
    path = absolutePath();//"../../src/data/";
    size_t pos = path.find_last_of("/", path.length()-1);
    pos = path.find_last_of("/", pos-1);
    pos = path.find_last_of("/", pos-1);
    path = path.substr(0, pos) + "/src/data/";
    
    closing = false;
    connect(home->chartPanel->P2Pbutton, &QPushButton::clicked, this, [this]() {
        timer->stop();
        manager.stop();
        delete home;
        home = nullptr;
        p2p = new P2PView(this);
        current = p2p;
        // todo
        // change back to home
        // ...
    });
    
    connect(qApp, &QApplication::aboutToQuit, this, [this] (){
        closing=true;
        Cleanup();
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
}

void TradeBot::Cleanup(){
    Client::abort = true;
    manager.stop();
    
    delete timer;
    delete timerCount;
    delete timestamp;
    if (home)
        delete home;
    if (p2p)
        delete p2p;
    
    emit close();
}

void TradeBot::OnFinishedUpdate(){
    if (*timerCount == 0){
        home->chartPanel->loadChart(home->ticks.begin(), home->ticks.end());
        home->chartPanel->chart->update();
        home->chartPanel->update();
        manager.enqueue(new func1(this,
                                 &TradeBot::downloadTicks,
                                 std::string("XBTZAR"),
                                 home->text),
                        true);
    }
    *timerCount = *timerCount +1;
    if (!closing)
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
        
        if (*timerCount % 10 == 0){
            auto openOrders = home->lunoClient->getUserOrders("XBTZAR", "PENDING");
            home->openOrderPanel->AddItem(openOrders, &lunoClient);
        }
        
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
        
        // refresh chart
        home->chartPanel->loadChart(home->ticks.begin(), home->ticks.end());
        home->chartPanel->chart->update();
        home->chartPanel->update();
        emit finishedUpdate();
    }
    else if (*timerCount % 2 == 1 ){

        auto y = home->orderPanel->tradeview->verticalScrollBar()->value();
        home->orderPanel->tradeview->setHtml(lastTrades().c_str());
        home->orderPanel->tradeview->verticalScrollBar()->setValue(y);
        
        if (latestTimestamp && *latestTimestamp < *timestamp)
            // data is outdated, maybe show different color
            ;
        
        emit finishedUpdate();
    }
    else {
        emit finishedUpdate();
    }
}

void TradeBot::loadLocalTicks(){
    loadingTicks = true;
    
    file.open(path + "XBTZAR.csv" , std::ios::in);
    if (file.good()){
        file >> home->ticks; // <- may take extremely long
            
        if (!closing && home->ticks.size() > 0){
            if (home->ticks.back().sequence != home->ticks.size())
            {
                while (home->ticks.size() > 0 && home->ticks.back().sequence != home->ticks.size())
                    home->ticks.pop_back();
                file.close();
                file.open(path, std::ios::out | std::ios::app);
                file << home->ticks;
                file.close();
                remove( (path + "XBTZAR.csv").c_str() );
                rename( (path + "XBTZAR2.csv").c_str(),
                        (path + "XBTZAR.csv" ).c_str());
            }
        }
        if (!closing && home->ticks.size() > 0){
            *timestamp = home->ticks.back().timestamp;
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
    
    home->moreticks = lunoClient.getTrades(pair, *timestamp-1); // order = newest to oldest
    while (home->ticks.size() > 0
            && home->moreticks.size() > 0
            && home->moreticks.back().sequence <= home->ticks.back().sequence)
        home->moreticks.pop_back();
    std::reverse(home->moreticks.begin(), home->moreticks.end()); // order = oldest to newest
    while (home->moreticks.size() > 0
            && home->moreticks.back().sequence != home->ticks.size()+home->moreticks.size())
            home->moreticks.pop_back();
    // Ensure sequenctial timestamp
    for (unsigned short index = 0; index < home->moreticks.size(); index++){
        if (home->moreticks[index].timestamp < *timestamp)
                home->moreticks.erase( home->moreticks.begin() + index, home->moreticks.end());
    }
    if (home->moreticks.size() > 0){
        home->ticks.insert(home->ticks.end(), home->moreticks.begin(), home->moreticks.end());
        *timestamp = home->moreticks.back().timestamp;
        file.open( path + pair + ".csv", std::ios::out | std::ios::app);
        file << home->moreticks;
        file.close();
    }
    home->moreticks.clear();
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
    
    if (home->ticks.size() > 0){
        
        for (size_t i = home->ticks.size() - 1; i >= home->ticks.size()- 21; i--){
            ss << "\n<tr> <a href=\"" << home->ticks[i].price << "\">";
            ss << "\n<td>" << QDateTime::fromMSecsSinceEpoch(home->ticks[i].timestamp).toString("hh:mm").toStdString() << "</td>";
            ss << (home->ticks[i].isBuy ? "\n<td class=Ask>" : "\n<td class=Bid>" ) ;
            ss << std::setprecision(0);
            ss << "<a href=\"" << home->ticks[i].price << "\">";
            ss  << home->ticks[i].price;
            ss << "</a></td>";
            ss << "\n<td>" << std::setprecision(6)<< home->ticks[i].volume << "</td>";
            ss << "\n</a></tr>";
        }
        ss << "</table>\n";
    }
    return ss.str();
}
