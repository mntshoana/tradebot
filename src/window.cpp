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
    text = new QTextEdit();
    text->setGeometry(0, 500, 1180, 220);
    text->setText("");

    openOrderPanel = new OpenOrderPanel();

    tabWidget = new QTabWidget(parent);
    tabWidget->setGeometry(0, 500, 1180, 220);
    tabWidget->addTab(text, tr("Output"));
    tabWidget->addTab(openOrderPanel, tr("Open Orders"));
    tabWidget->show();
    //tabWidget->setCurrentWidget(text);
    
    orderPanel = new OrderPanel(parent);
    chartPanel = new ChartPanel(parent);
    chartPanel->playground = new AutoPlayground(text);
    
    
    // request button
    // click event
    connect(orderPanel->request,
            &QPushButton::clicked, this,[this](){
        
        const char *action = (orderPanel->isBuy)
                ? "BID" : "ASK";
        try {
            *(text) << lunoClient->postLimitOrder("XBTZAR", action,
                                                  atof(orderPanel->txtAmount->text().toStdString().c_str()),
                                                  atoi(orderPanel->txtPrice->text().toStdString().c_str()));
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
