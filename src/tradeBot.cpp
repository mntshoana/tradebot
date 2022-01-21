#include "tradeBot.hpp"

TradeBot* TradeBot::staticThis = nullptr;

// Constructor
TradeBot::TradeBot () : QWidget(nullptr), manager() {
    staticThis = this;
    
    updateExchange(-1);
    
    connect(qApp, &QApplication::aboutToQuit, this, [this] (){
        home->closing=true;
        cleanup();
    });
    
    installEventFilter(this);
    
}

void TradeBot::cleanup(){
    Client::abort = true;
    manager.stop();
    home->workPanel->autoPlayground->deleteSharedMem();

    // Not clean, but not important
    emit close();
}


void TradeBot::onEnqueueUserOrder(std::string orderID){
    manager.enqueue(home->toAppendOpenUserOrder(orderID));
}


void TradeBot::loadTickData(){
    // Loading local data could take a long time
    Task* job = new Task( [this]() {
        thread = std::thread([this]() {
            home->loadLocalTicks();
            downloadTickData();
            displayTickData();
        });
        thread.detach();
    });
    job->updateWaitTime(0); // no delay
    job->setRepeat(false);
    job->setToAlwaysExecute();
    
    manager.enqueue(job);
}

void TradeBot::downloadTickData(){
    manager.enqueue(home->toDownloadTicks() );
}

void TradeBot::displayTickData(){
    // display ticks within application
    Task* job = new Task( [this]() {
        auto y = home->livePanel->tradeview->verticalScrollBar()->value();
        home->livePanel->tradeview->setHtml(home->lastTrades().c_str());
        home->livePanel->tradeview->verticalScrollBar()->setValue(y);
    }, true);
    job->updateWaitTime(2); // every two seconds
    job->wait = 0;
    job->setRepeat(true);
    job->setToAlwaysExecute();
    
    manager.enqueue(job);
}

void TradeBot::updateTheme(){
    // display ticks within application
    Task* job = new Task( [this]() {
        home->updateTheme();
    }, true);
    job->updateWaitTime(5); // every five seconds
    job->wait = 0;
    job->setRepeat(true);
    job->setToAlwaysExecute();
    
    manager.enqueue(job);
}

void TradeBot::updateExchange(int exchange){
    
    if (exchange > -1){
        manager.restart();
        home->workPanel->autoPlayground->deleteSharedMem();
        home->workPanel->close();
        
        if (exchange == LUNO_EXCHANGE)
            home = new LunoHomeView(this); // active home screen window;
        if (exchange == VALR_EXCHANGE)
            home = new VALRHomeView(this); // active home screen window;
    }
    else
        home = new LunoHomeView(this); // default
    
    // on enqueueUserOrder
    connect(home->livePanel->livetrade, &TradePanel::enqueueUserOrder,
            this, &TradeBot::onEnqueueUserOrder);
    
    // begin job manager
    loadTickData();
    updateTheme();
    updatePanels();
    manager.enqueue(home->toUpdateOrderBook());
    manager.enqueue(home->toUpdateOpenUserOrders());
    
}

void TradeBot::updatePanels(){
    
    Task* job = new Task( [this]() {
        // Balance Panel
        // TODO: seperate api call function from the following
        home->workPanel->userBalances->reloadItems();
        
        //  Withdrawal Panel
        home->workPanel->withdrawals->reloadItemsUsing(home->workPanel->userBalances->userBalances);
    }, true);
    job->updateWaitTime(30); // every 30 seconds
    job->wait = 0;
    job->setRepeat(true);
    
    manager.enqueue(job);
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


void TradeBot::enqueueJob(Task* job){
    manager.enqueue(job);
    return;
}
