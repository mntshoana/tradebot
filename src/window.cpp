#include "window.hpp"

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
    
    chartPanel = new ChartPanel(parent);
    
    text = new QTextEdit();
    text->setGeometry(0, 500, 1180, 220);
    text->setStyleSheet("QTextEdit { padding-left:5; padding-top:10;}");
    text->setText("");

    openOrderPanel = new OpenOrderPanel();
    openOrderPanel->setObjectName("OpenOrderPanel");
    
    tabWidget = new QTabWidget(parent);
    tabWidget->setGeometry(0, 480, 1180, 240);
    tabWidget->addTab(text, tr("Output"));
    tabWidget->addTab(openOrderPanel, tr("Open Orders"));
    
    chartPanel->playground = new AutoPlayground(text);
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
            if (orderPanel->isBuy && amount == 0.0f){
                auto balances = lunoClient->getBalances("ZAR");
                amount = (balances[0].balance - balances[0].reserved) / float(price);
        
                *(text) << "Price: " << std::to_string(price);
                *(text) << "Amount: " << std::to_string(amount);
            }
            else if (!orderPanel->isBuy && amount == 0.0f){
                auto balances = lunoClient->getBalances("XBT");
                amount = balances[0].balance / float(price);
                
                *(text) << "Price: " << std::to_string(price);
                *(text) << "Amount: " << std::to_string(amount);
            }
            
            if (amount < 0.0005f){
                *(text) << "Error - cannot trade for less than 0.000500 BTC";
                return;
            }
            *(text) << lunoClient->postLimitOrder("XBTZAR", action, amount, price);
        } catch (ResponseEx ex){
           *(text) << ex.String();
        }
    });
    
    // home window timeframe Combo Box text changed event
    connect(chartPanel->timeframe, &QComboBox::currentTextChanged,
        this, [this](const QString &str){
        *(text) << str.toStdString();
        chartPanel->loadChart(ticks.begin(), ticks.end());
        chartPanel->chart->update();
        chartPanel->update();
    });
    
    // home window simulate auto trade button to event
    connect(chartPanel->simulate, &QPushButton::clicked,
            this, [this](){
        chartPanel->simulate->setText("training");
        std::thread th([this]{
            chartPanel->playground->runScript();
            chartPanel->simulate->setText("Simulate");
        });
        th.detach();
    });
    
    // Theme
    if (isDarkMode())
        darkTheme();
    else
        lightTheme();
}

HomeView::~HomeView(){
    delete chartPanel->playground;
    delete chartPanel;
    delete orderPanel;
    delete text;
    text = nullptr;
    chartPanel = nullptr;
    orderPanel = nullptr;
    lunoClient = nullptr;
}

void HomeView::darkTheme(){
    // Theme
    QColor darker(25,25,25);
    orderPanel->livetradeview->setStyleSheet(R"(QGroupBox {
                                        background-color: #1e1e1e;
                                        color: white;
                                        border: none;
                                 } QGroupBox::title {
                                        background-color:transparent;
                                 })");
    QPalette p = chartPanel->palette();
    p.setColor(QPalette::Window, darker);
    chartPanel->setPalette(p);
    p.setColor(QPalette::Window, darker);
    chartPanel->chart->setPalette(p);
    
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
    QColor light(253,253,253);
    QBrush dark(QColor(20,20,20));
    
    orderPanel->livetradeview->setStyleSheet(R"(QGroupBox {
                                        background-color: white;
                                        color: black;
                                        border: none;
                                 } QGroupBox::title {
                                        background-color:transparent;
                                 })");
    
    QPalette p = chartPanel->palette();
    p.setColor(QPalette::Window, Qt::white);
    chartPanel->setPalette(p);
    p.setColor(QPalette::Window, light);
    chartPanel->chart->setPalette(p);
    
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

//-------------------------------------------------------

P2PView::P2PView (QWidget *parent) {
    text = new QTextEdit(parent);
    text->setGeometry(0, 500, 1180, 220);
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
