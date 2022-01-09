#include "tradeBot.hpp"

// Constructor
TradeBot::TradeBot (QWidget *parent ) : QWidget(parent), manager(parent, LUNO_EXCHANGE) {
    current = home = new LunoHomeView(this); // active home screen window
    
    // on update event
    connect(this, &TradeBot::finishedUpdate,
            this, &TradeBot::OnFinishedUpdate);
    
    timer = new QTimer(this);
    timer->setSingleShot(true);
    connect(timer, &QTimer::timeout, this, &TradeBot::OnUpdate);
    timerCount = new size_t(0); // counts the timeouts triggered by timer
    timer->start(100);
    
    
    
    connect(qApp, &QApplication::aboutToQuit, this, [this] (){
        home->closing=true;
        Cleanup();
    });
    
    // begin job manager
    manager.enqueue(home->toUpdateOrderBook());
    
    // # only testing
    //*home->workPanel->text  << Luno::LunoClient::listBeneficiaries();
    
    installEventFilter(this);
}

void TradeBot::Cleanup(){
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

void TradeBot::OnFinishedUpdate(){
    if (*timerCount == 0){
        manager.enqueue(home->toDownloadTicks());
    }
    
    *timerCount = *timerCount +1;
    if (!home->closing)
        timer->start(1000);
}
void TradeBot::OnUpdate() {
    timer->stop();
    if (*timerCount > 60) {
        *timerCount = 1;
    }
    
    if (*timerCount == 0) { // Initiate ticks
        home->livePanel->tradeview->setHtml(home->lastTrades().c_str());
        thread = std::thread([this]{
            home->loadLocalTicks();
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
        home->livePanel->tradeview->setHtml(home->lastTrades().c_str());
        home->livePanel->tradeview->verticalScrollBar()->setValue(y);
        
        emit finishedUpdate();
    }
    else {
        emit finishedUpdate();
    }
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
