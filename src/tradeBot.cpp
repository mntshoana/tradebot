#include "tradeBot.hpp"

// Constructor
TradeBot::TradeBot (QWidget *parent ) : QWidget(parent), manager(parent, LUNO_EXCHANGE) {
    current = home = new HomeView(this, LUNO_EXCHANGE); // active home screen window
    
    // on update event
    connect(this, &TradeBot::finishedUpdate,
            this, &TradeBot::OnFinishedUpdate);
    
    timestamp = new unsigned long long();
    
    
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
    
    connect(qApp, &QApplication::aboutToQuit, this, [this] (){
        closing=true;
        Cleanup();
    });
    // begin job manager
    JobBase* updateOrderBook = new Job1WPArg(
                                          home->livePanel->orderview,
                                          &Luno::LunoClient::getOrderBook,
                                          std::string("XBTZAR"),
                                          &Luno::OrderBook::FormatHTMLWith,
                                          &(home->workPanel->pendingOrders->openUserOrders));
    updateOrderBook->updateWaitTime(2);
    manager.enqueue(updateOrderBook, true);
    
    /*JobBase* updateOrderBook = new Job2(
                                          home->livePanel->orderview,
                                          &VALR::VALRClient::getOrderBook,
                                          std::string("BTCZAR"), bool(true),
                                          &VALR::OrderBook::FormatHTML);
    updateOrderBook->updateWaitTime(2);
    manager.enqueue(updateOrderBook, false);*/
    
    /*manager.enqueue(new Job1WPArg(
                              home->livePanel->orderview,
                              &VALR::VALRClient::getFullOrderBook,
                              std::string("BTCZAR"),
                              &VALR::OrderBook::FormatHTMLWith,
                              &(home->workPanel->pendingOrders->openUserOrders)),
                     true);*/
    /*manager.enqueue(new Job1(
                             home->livePanel->orderview,
                             &VALR::VALRClient::getOrderBook,
                             std::string("BTCZAR"),
                             &VALR::OrderBook::FormatHTML),
                    true);*/
    
    /*std::vector<std::string> batch;
    batch.push_back(VALR::VALRClient::formMarketPayload("BTCZAR", "ASK", 100, false, true));
    batch.push_back(VALR::VALRClient::formLimitPayload("BTCZAR", "BID", 0.002, 100000, true));
    batch.push_back(VALR::VALRClient::formLimitPayload("ETHZAR", "ASK", 0.2, 32000, true));
    batch.push_back(VALR::VALRClient::formStopLimitPayload("BTCZAR", "ASK", 0.002, 100000, 110000, false, true));
    batch.push_back(VALR::VALRClient::formStopLimitPayload("BTCZAR", "ASK", 0.0003, 1150000, 110000, true, true));
    batch.push_back(VALR::VALRClient::formStopLimitPayload("BTCZAR", "ASK", 0.00000002, 100000, 110000, true, true));
    batch.push_back(R"(
            {
                "type": "CANCEL_ORDER",
                "data": {
                    "orderId":"e5886f2d-191b-4330-a221-c7b41b0bc553",
                    "pair": "ETHZAR"
                }
            })");
   
    std::string batchPayload = VALR::VALRClient::packBatchPayloadFromList(batch);
    *home->workPanel->text  << VALR::VALRClient::postBatchOrders(batchPayload);*/
    *home->workPanel->text << VALR::VALRClient::getAllOpenOrders();
    installEventFilter(this);
}

void TradeBot::Cleanup(){
    Client::abort = true;
    manager.stop();
    
    delete timer;
    delete timerCount;
    delete timestamp;
    if (home)
        delete home;
    
    // Not clean, but not important
    home->workPanel->autoPlayground->deleteSharedMem();
    emit close();
}

void TradeBot::OnFinishedUpdate(){
    if (*timerCount == 0)
        manager.enqueue(new func1(this,
                                 &TradeBot::downloadTicks,
                                 std::string("XBTZAR")
                                 ), true);
    
    *timerCount = *timerCount +1;
    if (!closing)
        timer->start(1000);
}
void TradeBot::OnUpdate() {
    timer->stop();
    if (*timerCount > 60) {
        *timerCount = 1;
    }
    
    if (*timerCount == 0) { // Initiate ticks
        home->livePanel->tradeview->setHtml(lastTrades().c_str());
        thread = std::thread([this]{
            loadLocalTicks();
            emit finishedUpdate();
        });
        thread.detach();
    }
    
    else if (*timerCount % 5 == 0){
        //Theme
        current->updateTheme();
    
        if (*timerCount % 10 == 0){
            home->workPanel->pendingOrders->clearItems();
            home->workPanel->pendingOrders->addOrders(); 
          //  home->workPanel->autoPlayground->runScript();
        }
            
        if (*timerCount % 30 == 0){
            home->workPanel->userBalances->reloadItems();
            home->workPanel->withdrawals->reloadItemsUsing(home->workPanel->userBalances->userBalances);
        }
        
        emit finishedUpdate();
    }
    else if (*timerCount % 2 == 1 ){
        auto y = home->livePanel->tradeview->verticalScrollBar()->value();
        home->livePanel->tradeview->setHtml(lastTrades().c_str());
        home->livePanel->tradeview->verticalScrollBar()->setValue(y);
        
        emit finishedUpdate();
    }
    else {
        emit finishedUpdate();
    }
}

void TradeBot::loadLocalTicks(){
    
    file.open(path + "XBTZAR.csv" , std::ios::in);
    
    if (file.good()){
        file >> home->moreticks; // <- may take extremely long
        file.close();
        
        if (!closing && home->moreticks.size() > 0) {
            unsigned long long now = QDateTime::currentMSecsSinceEpoch();
            size_t i  = 0;
            while (i < home->moreticks.size()
            && (home->moreticks[i].timestamp  <= (now - 60 * 60 * 1000))){
                i++;
            }
                
            home->ticks.insert(home->ticks.end(), home->moreticks.begin() + i, home->moreticks.end());
            
            home->moreticks.clear();
            
            
            for (int i = 0; home->ticks.size() > 0 && i < (int)home->ticks.size()-2; i++){
                // ensure ordered oldest to newest
                if (home->ticks[i].sequence +1 != home->ticks[i+1].sequence){
                    do {
                        home->ticks.pop_back();
                    } while (i < (int)home->ticks.size() - 2);
                }
            }
              
            // empty file and replace content
            file.open(path + "XBTZAR.csv", std::ofstream::out | std::ofstream::trunc);
            file << home->ticks;
            file.close();
            
        }
        if (!closing && home->ticks.size() > 0){
            *timestamp = home->ticks.back().timestamp;
        }
        else
            *timestamp = QDateTime::currentMSecsSinceEpoch();
    }
    else {
        *timestamp = QDateTime::currentMSecsSinceEpoch();
        file.clear();
    }
}

void TradeBot::downloadTicks(std::string pair){
    home->moreticks = Luno::LunoClient::getTrades(pair, *timestamp); // order = newest to oldest
    while (home->ticks.size() > 0
            && home->moreticks.size() > 0
            && home->moreticks.back().sequence <= home->ticks.back().sequence)
        home->moreticks.pop_back();
    
    std::reverse(home->moreticks.begin(), home->moreticks.end()); // order = oldest to newest
    
    for (int i = 0; home->moreticks.size() > 0 && i < ((int) home->moreticks.size()) -2; i++){
        // ensure ordered oldest to newest sequence is in tact
        if (home->moreticks[i].sequence +1 != home->moreticks[i+1].sequence){
            do {
                home->moreticks.pop_back();
            } while (i < ((int) home->moreticks.size()) -2);
        }
    }
    
    if (home->moreticks.size() > 0){
        home->ticks.insert(home->ticks.end(), home->moreticks.begin(), home->moreticks.end());
        *timestamp = home->ticks.back().timestamp;
        file.open( path + pair + ".csv", std::ios::out | std::ios::app);
        if (file.good()){
            file << home->moreticks;
            file.close();
        }
        else {
            file.clear();
            *TextPanel::textPanel << std::string("[Error] : At ")
                            + __FILE__ + ": line " + std::to_string(__LINE__)
                        + ". Couldn't write ticks to file.";
        }
        home->moreticks.clear();
    }
}

std::string TradeBot::lastTrades() {
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
    
    
    for (int i = home->ticks.size() -1, limit = 1000; i >= 0 && limit >= 0; i--, limit--){
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
    
    return ss.str();
}

bool TradeBot::eventFilter(QObject *obj, QEvent *event){
    if (event->type() == QEvent::KeyPress){
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
        if (keyEvent->key() == Qt::Key_Escape){
            home->workPanel->pendingOrders->popFrontOrder();
        }
        else if (keyEvent->key() == Qt::Key_BracketRight){
            std::string price = home->livePanel->livetrade->txtPrice->text().toStdString();
            if (price != ""){
                int priceInt = atoi(price.c_str());
                price = std::to_string(priceInt+1);
                
                home->livePanel->livetrade->txtPrice->setText(QString::fromStdString(price));
            }
        }
        else if (keyEvent->key() == Qt::Key_BracketLeft){
            std::string price = home->livePanel->livetrade->txtPrice->text().toStdString();
            if (price != ""){
                int priceInt = atoi(price.c_str());
                price = std::to_string(priceInt-1);
                
                home->livePanel->livetrade->txtPrice->setText(QString::fromStdString(price));
            }
        }
        else if (keyEvent->key() == Qt::Key_Return){
            emit home->livePanel->livetrade->request->clicked();
        }
        else
        {
            TradeBot::keyPressEvent(keyEvent);
        }
    }
    return QObject::eventFilter(obj, event);
}
