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

HomeView::HomeView (QWidget *parent) : QWidget(parent), text(parent) {
    livePanel = new LivePanel(parent);
    
    // Output stream for logs

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
    
    pendingOrders = new PendingOrdersPanel(nullptr);
    pendingOrders->setObjectName("PendingOrders");
    
    userBalances = new BalancePanel(nullptr);
    userBalances->setObjectName("UserBalances");
    
    withdrawals = new WithdrawPanel(nullptr);
    withdrawals->setObjectName("Withdrawals");
    
    tabWidget = new QTabWidget(parent);
    tabWidget->setGeometry(0, 472, 930, 248);
    tabWidget->addTab(&text, tr("Output"));
    tabWidget->addTab(pendingOrders, tr("Open Orders"));
    tabWidget->addTab(userBalances, tr("User Balances"));
    tabWidget->addTab(withdrawals, tr("User Balances"));
    
    //chartPanel->playground = new AutoPlayground(text);
    
    // Theme
    if (isDarkMode())
        darkTheme();
    else
        lightTheme();
}

HomeView::~HomeView(){
    delete livePanel;
    livePanel = nullptr;
    lunoClient = nullptr;
}

void HomeView::darkTheme(){
    // Theme
    view->page()->runJavaScript( R"java(
        tvWidget.changeTheme("Dark");
    )java");
    
    const char* groupBoxTheme = R"(QGroupBox {
                                    background-color: #1e1e1e;
                                    color: white;
                                    border: none;
                                } QGroupBox::title {
                                    background-color:transparent;
    })";
    livePanel->livetrade->livetradeview->setStyleSheet(groupBoxTheme);
    withdrawals->boundingBox->setStyleSheet(groupBoxTheme);
    
    tabWidget->setStyleSheet(R"(QTabWidget::tab-bar  {
                                left: 5px;
                                top: 28px;
                             } QTabWidget::pane {
                                border-top: 25px solid #1a1a1a;
                             } QTabBar::tab {
                                border-style: outset;
                                border-radius: 10px;
                                padding-left: 8px;
                                padding-right: 8px;
                                padding-top: 2px;
                                padding-bottom: 2px;
                                margin-bottom: 2px;
                             } QTabBar::tab:!first:!last {
                                border-radius: 0px;
                             } QTabBar::tab:first {
                                border-top-right-radius: 0px;
                                border-bottom-right-radius: 0px;
                             } QTabBar::tab:last {
                                border-top-left-radius: 0px;
                                border-bottom-left-radius: 0px;
                             } QTabBar::tab:selected {
                                color: rgb(241 , 231, 242);
                                border-top: 1px solid rgb(26 ,120, 220);
                                background-color: rgb(21 ,111, 215);
                                border-bottom: 1px solid rgb(10 ,55, 107);
                                margin-top: 2px;
                                margin-bottom: 0px;
                             } QTabBar::tab:!selected {
                                color: rgb(241 , 231, 242);
                                border-top: 1px solid rgb(87 ,87, 87);
                                background-color: rgb(83 ,83, 83);
                                border-bottom: 1px solid rgb(77 ,77, 77);
                             } QTextEdit, #PendingOrders, #UserBalances, #Withdrawals {
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
    
    const char* groupBoxTheme = R"(QGroupBox {
                                        background-color: white;
                                        color: black;
                                        border: none;
                                 } QGroupBox::title {
                                        background-color:transparent;
                                 })";
    
    livePanel->livetrade->livetradeview->setStyleSheet(groupBoxTheme);
    withdrawals->boundingBox->setStyleSheet(groupBoxTheme);
    
    tabWidget->setStyleSheet(R"(QTabWidget::tab-bar  {
                                left: 5px;
                                top: 28px;
                             } QTabWidget::pane {
                                border-top: 25px solid #fdfdfd;
                             } QTabBar::tab {
                                border-style: outset;
                                border-radius: 10px;
                                padding-left: 8px;
                                padding-right: 8px;
                                padding-top: 2px;
                                padding-bottom: 2px;
                                margin-bottom: 2px;
                              } QTabBar::tab:!first:!last {
                                border-radius: 0px;
                              } QTabBar::tab:first {
                                border-top-right-radius: 0px;
                                border-bottom-right-radius: 0px;
                              } QTabBar::tab:last {
                                border-top-left-radius: 0px;
                                border-bottom-left-radius: 0px;
                              } QTabBar::tab:selected {
                                color: rgb(255 , 255, 255);
                                border-top: 1px solid rgb(41 ,155, 255);
                                background-color: rgb(21 ,133, 255);
                                border-bottom: 1px solid rgb(1 ,123, 255);
                                margin-top: 2px;
                                margin-bottom: 0px;
                              } QTabBar::tab:!selected {
                                color: rgb(69 , 69, 69);
                                border-top: 1px solid rgb(230 ,230, 230);
                                background-color: rgb(255 ,255, 255);
                                border-bottom: 1px solid rgb(209, 209, 209);
                              } QTabBar::tab:!last:!selected {
                                 border-right: 1px solid rgb(209, 209, 209);
                              } QTextEdit, #PendingOrders, #UserBalances, #Withdrawals {
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
