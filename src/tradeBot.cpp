#include "tradeBot.hpp"

// Constructor
TradeBot::TradeBot (QWidget *parent ) : QWidget(parent), manager(parent, LUNO_EXCHANGE) {
    current = home = new LunoHomeView(this); // active home screen window
    
    // on enqueueUserOrder
    connect(home->livePanel->livetrade, &TradePanel::enqueueUserOrder,
            this, &TradeBot::onEnqueueUserOrder);
    
    
    connect(qApp, &QApplication::aboutToQuit, this, [this] (){
        home->closing=true;
        // TODO: Empty the job manager
        // jobmanager.clear()
        
        cleanup();
    });
    
    
    // begin job manager
    loadTickData();
    updateTheme();
    updatePanels();
    manager.enqueue(home->toUpdateOrderBook());
    manager.enqueue(home->toUpdateOpenUserOrders());
    
    // # only testing
    //*home->workPanel->text  << Luno::LunoClient::listBeneficiaries();
    
    installEventFilter(this);
}

void TradeBot::cleanup(){
    Client::abort = true;
    manager.stop();

    if (home)
        delete home;
    
    // Not clean, but not important
    home->workPanel->autoPlayground->deleteSharedMem();
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
        current->updateTheme();
    }, true);
    job->updateWaitTime(5); // every five seconds
    job->wait = 0;
    job->setRepeat(true);
    job->setToAlwaysExecute();
    
    manager.enqueue(job);
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
