#include "Window.hpp"

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

HomeView::HomeView (QWidget *parent) {
    text = new QTextEdit(parent);
    text->setGeometry(0, 500, 1180, 220);
    text->setText("");

    orderPanel = new OrderPanel(parent);
    chartPanel = new ChartPanel(parent);
    
    
    // Theme
    if (isDarkMode())
        darkTheme();
    else
        lightTheme();
}

HomeView::~HomeView(){
    delete text;
    delete chartPanel;
    delete orderPanel;
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

void P2PView::darkTheme() {
    
}

void P2PView::lightTheme() {
    
}

void P2PView::updateTheme(){
    //Theme
    if (nightmode && !isDarkMode())
        lightTheme();
    if (!nightmode && isDarkMode())
        darkTheme();
}
