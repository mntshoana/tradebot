#include "tradeBot.hpp"

// Constructor
TradeBot::TradeBot (QWidget *parent ) : QWidget(parent), manager(parent, LUNO_EXCHANGE) {
    current = home = new LunoHomeView(this); // active home screen window
    
    // on update event
    connect(this, &TradeBot::finishedUpdate,
            this, &TradeBot::onFinishedUpdate);
    
    // on enqueueUserOrder
    connect(home->livePanel->livetrade, &TradePanel::enqueueUserOrder,
            this, &TradeBot::onEnqueueUserOrder);
    
    timer = new QTimer(this);
    timer->setSingleShot(true);
    connect(timer, &QTimer::timeout, this, &TradeBot::onUpdate);
    timerCount = new size_t(0); // counts the timeouts triggered by timer
    timer->start(100);
    
    
    
    connect(qApp, &QApplication::aboutToQuit, this, [this] (){
        home->closing=true;
        cleanup();
    });
    
    
    // begin job manager
    loadTickData();
    manager.enqueue(home->toUpdateOrderBook());
    manager.enqueue(home->toUpdateOpenUserOrders());
    
    // # only testing
    //*home->workPanel->text  << Luno::LunoClient::listBeneficiaries();
    
    installEventFilter(this);
}

void TradeBot::cleanup(){
    Client::abort = true;
    manager.stop();
    
    delete timer;
    delete timerCount;

    if (home)
        delete home;
    
    // Not clean, but not important
    home->workPanel->autoPlayground->deleteSharedMem();
    emit close();
}

void TradeBot::onFinishedUpdate(){
    *timerCount = *timerCount +1;
    if (!home->closing)
        timer->start(1000);
}

void TradeBot::onEnqueueUserOrder(std::string orderID){
    manager.enqueue(home->toAppendOpenUserOrder(orderID));
}

void TradeBot::onUpdate() {
    timer->stop();
    if (*timerCount > 60) {
        *timerCount = 1;
    }
    
    if (*timerCount == 0) { // Initiate ticks
            emit finishedUpdate();
    }
    
    else if (*timerCount % 5 == 0){
        //Theme
        current->updateTheme();
    
        if (*timerCount % 10 == 0){
          //  home->workPanel->autoPlayground->runScript();
        }
            
        if (*timerCount % 30 == 0){
            home->workPanel->userBalances->reloadItems();
            home->workPanel->withdrawals->reloadItemsUsing(home->workPanel->userBalances->userBalances);
        }
        
        emit finishedUpdate();
    }
    else {
        emit finishedUpdate();
    }
}

void TradeBot::downloadTickData(){
    manager.enqueue(home->toDownloadTicks() );
   
    // display ticks
    Task* job = new Task( [this]() {
        auto y = home->livePanel->tradeview->verticalScrollBar()->value();
        home->livePanel->tradeview->setHtml(home->lastTrades().c_str());
        home->livePanel->tradeview->verticalScrollBar()->setValue(y);
    }, true);
    job->updateWaitTime(2);
    job->wait = 0;
    job->setRepeat(true);
    
    manager.enqueue(job);
}
void TradeBot::loadTickData(){
    Task* job = new Task( [this]() {
        thread = std::thread([this]() {
            home->loadLocalTicks();
            downloadTickData();
        });
        thread.detach();
    });
    job->updateWaitTime(0);
    job->setRepeat(false);
    job->setToAlwaysExecute();
    
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
