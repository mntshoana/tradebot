#include "window.hpp"
#include <QTimer>


QTextEdit& operator<< (QTextEdit& stream, std::string str) {
    stream.append(str.c_str());
    return stream;
}
QTextBrowser& operator<< (QTextBrowser& stream, std::string str){
    stream.append(str.c_str());
    return stream;
}

HomeView::HomeView (QWidget *parent, Luno::LunoClient* client) : QWidget(parent), lunoClient(client) {
    orderPanel = new OrderPanel(parent);
    
    // Output stream for logs
    text = new QTextEdit();
    text->setGeometry(0, 500, 930, 220);
    text->setStyleSheet("QTextEdit { padding-left:5; padding-top:10;}");
    text->setText("");

    // Chart Displayed using QWebEngine browser
    view = new QWebEngineView(parent); // to do: move VIEW to a chart widget
    view->setGeometry(0, 0, 930, 500);
    view->load(QUrl("https://d32exi8v9av3ux.cloudfront.net/static/scripts/tradingview.prod.html?symbol=XBTZAR&res=60&lang=en"));
    
    // Note: the next line only works if the browser has already loaded, hence delay
    if (!isDarkMode())
        QTimer::singleShot(4000, this, [this](){
                view->page()->runJavaScript( R"java(
                tvWidget.changeTheme("Light");
                bgcolor(white);
            )java");
        });
    
    view->show();
    
    pendingOrders = new PendingOrders(nullptr, lunoClient, text);
    pendingOrders->setObjectName("PendingOrders");
    
    tabWidget = new QTabWidget(parent);
    tabWidget->setGeometry(0, 480, 930, 240);
    tabWidget->addTab(text, tr("Output"));
    tabWidget->addTab(pendingOrders, tr("Open Orders"));
    
    //chartPanel->playground = new AutoPlayground(text);
    // request button
    // click event
    connect(orderPanel->request,
            &QPushButton::clicked, this,[this](){
        
        int price = atoi(orderPanel->txtPrice->text().toStdString().c_str());
        float amount = atof(orderPanel->txtAmount->text().toStdString().c_str());
        
        if (price == 0){
            *(text) << "Error - Price cannot be empty.";
            return;
        }
        
        const char *action = (orderPanel->isBuy)
                ? "BID" : "ASK";
        try {
            if (amount == 0.0f){
                if (orderPanel->isBuy){
                    auto balances = lunoClient->getBalances("ZAR");
                    amount = (balances[0].balance - balances[0].reserved) / float(price);
                }
                else if (!orderPanel->isBuy){
                    auto balances = lunoClient->getBalances("XBT");
                    amount = balances[0].balance - balances[0].reserved;
                }
            }
           
            else if (amount < 0.0005f){
                *(text) << "Error - cannot trade for less than 0.000500 BTC";
                return;
            }
            
            // output
            std::string result = lunoClient->postLimitOrder("XBTZAR", action, amount, price);
            *(text) << std::string(action) + " order at price: " + std::to_string(price);
            *(text) << "   Amount: " + std::to_string(amount);
            *(text) << "   Valued: " + std::to_string(amount * price);
            *(text) << "COMPLETE: " + result;
            
        } catch (ResponseEx ex){
            *(text) << " [Error] Unable to place order! at " + std::string(__FILE__) + ", line: " + std::to_string(__LINE__);
            *(text) << ex.String();
        }
    });
    
    // Theme
    if (isDarkMode())
        darkTheme();
    else
        lightTheme();
}

HomeView::~HomeView(){
    delete orderPanel;
    delete text;
    text = nullptr;
    orderPanel = nullptr;
    lunoClient = nullptr;
}

void HomeView::darkTheme(){
    // Theme
    view->page()->runJavaScript( R"java(
        tvWidget.changeTheme("Dark");
    )java");
    
    orderPanel->livetradeview->setStyleSheet(R"(QGroupBox {
                                        background-color: #1e1e1e;
                                        color: white;
                                        border: none;
                                 } QGroupBox::title {
                                        background-color:transparent;
                                 })");
    
    tabWidget->setStyleSheet(R"(QTabWidget::tab-bar  {
                                left: 5px;
                                top: 26px;
                             } QTabWidget::pane {
                             border-top: 25px solid #1a1a1a;
                             } QTextEdit, #PendingOrders {
                             background-color: rgb(28, 28, 28);
                             border-style: outset;
                             border-width: 0.5px;
                             border-top-color: rgb(25, 25, 25);
                             } )");
    
}
void HomeView::lightTheme() {
    // Theme
    view->page()->runJavaScript( R"java(
        tvWidget.changeTheme("Light");
    )java");
    
    orderPanel->livetradeview->setStyleSheet(R"(QGroupBox {
                                        background-color: white;
                                        color: black;
                                        border: none;
                                 } QGroupBox::title {
                                        background-color:transparent;
                                 })");
    
    tabWidget->setStyleSheet(R"(QTabWidget::tab-bar  {
                                left: 5px;
                                top: 26px;
                             } QTabWidget::pane {
                             border-top: 25px solid #fdfdfd;
                             } QTextEdit, #PendingOrders {
                             background-color: rgb(253, 253, 253);
                             border-style: outset;
                             border-width: 0.5px;
                             border-top-color: rgb(165, 165, 165);
                             border-bottom-color: rgb(245, 245, 245);
                             border-left-color: rgb(185, 185, 185);
                             border-right-color: rgb(225, 225, 225);
                             } )");
    
    nightmode = false;
}

void HomeView::updateTheme(){
    //Theme
    if (nightmode && !isDarkMode())
        lightTheme();
    if (!nightmode && isDarkMode())
        darkTheme();
}

void HomeView::forceDarkTheme(){
    darkTheme();
}
