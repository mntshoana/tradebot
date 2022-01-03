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
    explicit OrderPanel(QWidget* parent, std::string titleL, std::string titleR = "");
    
    void setGeometry(int ax, int ay, int aw, int ah);
    
    void setHtml(const char* html);
    
    QScrollBar *verticalScrollBar() const;
        
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

OrderPanel* operator<< (OrderPanel* stream, std::string str);
#endif /* exceptions_hpp */
