#ifndef ORDER_PANEL_HEADER
#define ORDER_PANEL_HEADER

#include <QLayout>
#include <QScrollBar>

#include "label.hpp"

class OrderPanel : public QWidget {
    Q_OBJECT
    Label* header;
    QTextBrowser* body;
public:
    bool orderViewIsEmpty;
    explicit OrderPanel(QWidget* parent, std::string titleL, std::string titleR = "") : QWidget(parent) {
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
            labelBuilder = "<th>";
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
    
    void setGeometry(int ax, int ay, int aw, int ah) {
        header->setGeometry(ax, ay, aw, 30);
        body->setGeometry(ax, ay+29, aw, ah - 30);
    }
    
    void setHtml(const char* html){
        body->setHtml(html);
    }
    
    QScrollBar *verticalScrollBar() const{
        return body->verticalScrollBar();
    }
        
    template<class T>
    friend OrderPanel& operator<< (OrderPanel& stream, T obj);
    
Q_SIGNALS:
    void anchorClicked(const QUrl &link);

};

template <class T> OrderPanel& operator<< (OrderPanel& stream, T obj){
    stream.body->append(obj.toString().c_str());
    return stream;
}

template <>
OrderPanel& operator<< <std::string>(OrderPanel& stream, std::string str);

#endif /* exceptions_hpp */
