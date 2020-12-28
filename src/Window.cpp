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

HomeWindow::HomeWindow (QWidget *parent) {
    text = new QTextEdit(parent);
    text->setGeometry(0, 500, 1180, 220);
    text->setText("");

    orderPanel = new OrderPanel(parent);
}

void HomeWindow::darkTheme(){
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
    nightmode = true;
}
void HomeWindow::lightTheme() {
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
