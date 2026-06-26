#include "tradeBot.hpp"

TradeBot* TradeBot::staticThis = nullptr;

// Constructor
TradeBot::TradeBot () : QWidget(nullptr) {
    staticThis = this;
    
    updateExchange(-1);
    
    connect(qApp, &QApplication::aboutToQuit, this, [this] (){
        home->closing = true;
        cleanup();
    });
    
    installEventFilter(this);
}

void TradeBot::cleanup() {
    if (ws) {
        ws->disconnect();
        ws->close();
        ws->deleteLater();
        ws = nullptr;
    }
    if (displayTimer)  { displayTimer->stop();  displayTimer->deleteLater();  displayTimer  = nullptr; }
    if (balanceTimer)  { balanceTimer->stop();  balanceTimer->deleteLater();  balanceTimer  = nullptr; }
    if (ordersTimer)   { ordersTimer->stop();   ordersTimer->deleteLater();   ordersTimer   = nullptr; }
    if (themeTimer)    { themeTimer->stop();    themeTimer->deleteLater();    themeTimer    = nullptr; }
    home->workPanel->autoPlayground->deleteSharedMem();
    emit close();
}

void TradeBot::onEnqueueUserOrder(std::string /*orderID*/) {
    // Refresh the full pending-orders list immediately; ordersTimer will keep it current
    home->refreshOpenOrders();
}

void TradeBot::connectWebSocket() {
    ws = new QWebSocket(QString(), QWebSocketProtocol::VersionLatest, this);
    connect(ws, &QWebSocket::textMessageReceived, this, &TradeBot::onWsMessage);
    connect(ws, &QWebSocket::disconnected, this, [this]() {
        if (home && !home->closing)
            QTimer::singleShot(3000, this, &TradeBot::connectWebSocket);
    });
    ws->open(QUrl("ws://localhost:8080/ws"));
}

void TradeBot::onWsMessage(const QString& msg) {
    auto frame = QJsonDocument::fromJson(msg.toUtf8()).object();
    const QString type = frame["type"].toString();
    if (type == "orderbook")
        home->onOrderBook(frame["data"].toObject());
    else if (type == "trades")
        home->onTrades(frame["data"].toArray());
}

void TradeBot::setupTimers() {
    // Trade view refresh — every 2 sec
    displayTimer = new QTimer(this);
    connect(displayTimer, &QTimer::timeout, this, [this]() {
        auto y = home->livePanel->tradeview->verticalScrollBar()->value();
        home->livePanel->tradeview->setHtml(home->lastTrades().c_str());
        home->livePanel->tradeview->verticalScrollBar()->setValue(y);
    });
    displayTimer->start(2000);

    // Pending orders refresh — every 5 sec, fire once immediately
    ordersTimer = new QTimer(this);
    connect(ordersTimer, &QTimer::timeout, this, [this]() {
        home->refreshOpenOrders();
    });
    ordersTimer->start(5000);
    home->refreshOpenOrders(); // immediate first call

    // Balance + withdrawal refresh — every 30 sec
    balanceTimer = new QTimer(this);
    connect(balanceTimer, &QTimer::timeout, this, [this]() {
        home->workPanel->userBalances->reloadItems();
        home->workPanel->withdrawals->reloadItemsUsing(
            home->workPanel->userBalances->userBalances);
    });
    balanceTimer->start(30000);

    // Theme check — every 5 sec
    themeTimer = new QTimer(this);
    connect(themeTimer, &QTimer::timeout, this, [this]() {
        home->updateTheme();
    });
    themeTimer->start(5000);
}

void TradeBot::updateExchange(int exchange) {
    if (exchange > -1) {
        // Stop timers and WS for the previous exchange
        if (ws)           { ws->disconnect(); ws->close(); ws->deleteLater(); ws = nullptr; }
        if (displayTimer) { displayTimer->stop(); displayTimer->deleteLater(); displayTimer = nullptr; }
        if (balanceTimer) { balanceTimer->stop(); balanceTimer->deleteLater(); balanceTimer = nullptr; }
        if (ordersTimer)  { ordersTimer->stop();  ordersTimer->deleteLater();  ordersTimer  = nullptr; }
        if (themeTimer)   { themeTimer->stop();   themeTimer->deleteLater();   themeTimer   = nullptr; }

        home->workPanel->autoPlayground->deleteSharedMem();
        home->workPanel->close();

        if (exchange == LUNO_EXCHANGE)
            home = new LunoHomeView(this);
        if (exchange == VALR_EXCHANGE)
            home = new VALRHomeView(this);
    } else {
        home = new LunoHomeView(this); // default
    }

    // Wire trade panel → order-enqueue signal
    connect(home->livePanel->livetrade, &TradePanel::enqueueUserOrder,
            this, &TradeBot::onEnqueueUserOrder);

    // Load historical ticks from disk in background
    thread = std::thread([this]() { home->loadLocalTicks(); });
    thread.detach();

    // Connect WebSocket and start all periodic timers
    connectWebSocket();
    setupTimers();
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



