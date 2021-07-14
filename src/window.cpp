#include "window.hpp"
#include <QTimer>



QTextEdit& operator<< (QTextEdit& stream, std::string str)
{
    stream.append(str.c_str());
    return stream;
}
QTextBrowser& operator<< (QTextBrowser& stream, std::string str)
{
    stream.append(str.c_str());
    return stream;
}

HomeView::HomeView (QWidget *parent, Luno::LunoClient* client) : QWidget(parent), lunoClient(client) {
    orderPanel = new OrderPanel(parent);
    
    //chartPanel = new ChartPanel(parent);
    
    text = new QTextEdit();
    text->setGeometry(0, 500, 930, 220);
    text->setStyleSheet("QTextEdit { padding-left:5; padding-top:10;}");
    text->setText("");

    
    // will be moved to a chart widget
    view = new QWebEngineView(parent);
    view->load(QUrl("https://d32exi8v9av3ux.cloudfront.net/static/scripts/tradingview.prod.html?symbol=XBTZAR&res=60&lang=en"));
    view->setGeometry(0, 0, 930, 500);
    if (!isDarkMode())
        QTimer::singleShot(4000, this, [this](){
                view->page()->runJavaScript( R"java(
                tvWidget.changeTheme("Light");
                bgcolor(white);
            )java");
        });
    
    view->show();
    // end of new chart widget
    
    openOrderPanel = new OpenOrderPanel(nullptr, lunoClient, text);
    openOrderPanel->setObjectName("OpenOrderPanel");
    
    tabWidget = new QTabWidget(parent);
    tabWidget->setGeometry(0, 480, 930, 240);
    tabWidget->addTab(text, tr("Output"));
    tabWidget->addTab(openOrderPanel, tr("Open Orders"));
    
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
    
    // home window timeframe Combo Box text changed event
    /*connect(chartPanel->timeframe, &QComboBox::currentTextChanged,
        this, [this](const QString &str){
        *(text) << str.toStdString();
        chartPanel->loadChart(ticks.begin(), ticks.end());
        chartPanel->chart->left = (chartPanel->chart->count()+1) * chartPanel->chart->scaledXIncrements - 980;
        chartPanel->chart->update();
        chartPanel->update();
    });*/
    
    // home window simulate auto trade button to event
    /*connect(chartPanel->simulate, &QPushButton::clicked,
            this, [this](){
        chartPanel->simulate->setText("training");
        std::thread th([this]{
            chartPanel->playground->runScript();
            chartPanel->simulate->setText("Simulate");
        });
        th.detach();
    });*/
    
    // Theme
    if (isDarkMode())
        darkTheme();
    else
        lightTheme();
}

HomeView::~HomeView(){
    //delete chartPanel->playground;
    //delete chartPanel;
    delete orderPanel;
    delete text;
    text = nullptr;
    //chartPanel = nullptr;
    orderPanel = nullptr;
    lunoClient = nullptr;
}

void HomeView::darkTheme(){
    // Theme
    view->page()->runJavaScript( R"java(
        tvWidget.changeTheme("Dark");
    )java");
    //QColor darker(25,25,25);
    orderPanel->livetradeview->setStyleSheet(R"(QGroupBox {
                                        background-color: #1e1e1e;
                                        color: white;
                                        border: none;
                                 } QGroupBox::title {
                                        background-color:transparent;
                                 })");
    //QPalette p = chartPanel->palette();
    //p.setColor(QPalette::Window, darker);
    //chartPanel->setPalette(p);
    //p.setColor(QPalette::Window, darker);
    //chartPanel->chart->setPalette(p);
    
    tabWidget->setStyleSheet(R"(QTabWidget::tab-bar  {
                                left: 5px;
                                top: 26px;
                             } QTabWidget::pane {
                             border-top: 25px solid #1a1a1a;
                             } QTextEdit, #OpenOrderPanel {
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
    
    //QColor light(253,253,253);
    //QBrush dark(QColor(20,20,20));
    
    orderPanel->livetradeview->setStyleSheet(R"(QGroupBox {
                                        background-color: white;
                                        color: black;
                                        border: none;
                                 } QGroupBox::title {
                                        background-color:transparent;
                                 })");
    
    //QPalette p = chartPanel->palette();
    //p.setColor(QPalette::Window, Qt::white);
    //chartPanel->setPalette(p);
    //p.setColor(QPalette::Window, light);
    //chartPanel->chart->setPalette(p);
    
    tabWidget->setStyleSheet(R"(QTabWidget::tab-bar  {
                                left: 5px;
                                top: 26px;
                             } QTabWidget::pane {
                             border-top: 25px solid #fdfdfd;
                             } QTextEdit, #OpenOrderPanel {
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

//-------------------------------------------------------

P2PView::P2PView (QWidget *parent) {
    text = new QTextEdit(parent);
    text->setGeometry(0, 500, 930, 220);
    text->document()->setDocumentMargin(5);
    text->setText("Peer 2 Peer View");
    text->show();
    
    // Theme
    if (isDarkMode())
        darkTheme();
    else
        lightTheme();
}

P2PView::~P2PView(){
    delete text;
}

void P2PView::darkTheme() {
    // Todo
}

void P2PView::lightTheme() {
    // Todo
}

void P2PView::updateTheme(){
    //Theme
    if (nightmode && !isDarkMode())
        lightTheme();
    if (!nightmode && isDarkMode())
        darkTheme();
}
void P2PView::forceDarkTheme(){
    darkTheme();
}
