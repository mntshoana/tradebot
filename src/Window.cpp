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
