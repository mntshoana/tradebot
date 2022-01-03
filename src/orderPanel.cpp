#include "orderPanel.hpp"

OrderPanel::OrderPanel(QWidget* parent, std::string titleL, std::string titleR) : QWidget(parent) {
    orderViewIsEmpty = true;
    installEventFilter(this);
    
    std::string labelBuilder;
    if (titleR == ""){
        labelBuilder = "<th colspan=2>";
        labelBuilder += titleL;
        labelBuilder += "</th>";
    }
    else{
        labelBuilder = "<th>";
        labelBuilder += titleL;
        labelBuilder += "</th>";
        labelBuilder += "<th>";
        labelBuilder += titleR;
        labelBuilder += "</th>";
    }

    header = new Label(labelBuilder, parent);
    body = new QTextBrowser(parent);
    body->setText("");
    body->setReadOnly(true);
    body->setOpenLinks(false);
    
    connect(body, &QTextBrowser::anchorClicked, this, [this](const QUrl &link){emit anchorClicked(link);});
}

void OrderPanel::setGeometry(int ax, int ay, int aw, int ah) {
    header->setGeometry(ax, ay, aw, 30);
    body->setGeometry(ax, ay+29, aw, ah - 30);
}

QScrollBar* OrderPanel::verticalScrollBar() const{
    return body->verticalScrollBar();
}
void OrderPanel::setHtml(const char* html){
    body->setHtml(html);
}
template<>
OrderPanel& operator<< <std::string>(OrderPanel& stream, std::string str)
{
    if (stream.orderViewIsEmpty)
    {
        stream.body->append(str.c_str()); // update content
        auto step = stream.body->verticalScrollBar()->singleStep();
        stream.body->verticalScrollBar()->setValue(step * 107.8);
        stream.orderViewIsEmpty = false;
    }
    else {
        auto y = stream.body->verticalScrollBar()->value();
        stream.body->setHtml(str.c_str()); // update content
        if (y != 0)
            stream.body->verticalScrollBar()->setValue(y);
        else{
            auto step = stream.body->verticalScrollBar()->singleStep();
            stream.body->verticalScrollBar()->setValue(step * 90);
        }
    }
    return stream;
}

OrderPanel* operator<< (OrderPanel* stream, std::string str){
    (*stream) << str;
    return stream;
}
