#ifndef TradeBot_OrderPanel_hpp
#define TradeBot_OrderPanel_hpp

#include "label.hpp"

#include <QLayout>
#include <QScrollBar>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>

#include "lunoclient.hpp"
#include "lineBlock.hpp"

class OrderView : public QTextBrowser {
public:
    bool orderViewIsEmpty;
    explicit OrderView(QWidget* parent = nullptr) : QTextBrowser(parent) {
        orderViewIsEmpty = true;
        installEventFilter(this);
    }
    
};

template <class T> OrderView& operator<< (OrderView& stream, T obj){
    stream.append(obj.toString().c_str());
    return stream;
}

template <>
OrderView& operator<< <std::string>(OrderView& stream, std::string str);



//
class OrderPanel : public QWidget {
    Q_OBJECT
    Label* orderViewLabel, *tradeViewLabel, *liveTradeLabel;
public:
    bool isBuy;
    OrderView* orderview;
    QTextBrowser *tradeview;
    QGroupBox *livetradeview;
    QGridLayout *livetradeviewLayout;
    QLabel *lblPrice, *lblAmount;
    LineBlock* txtPrice, *txtAmount;
    QPushButton* request;
    
    OrderPanel(QWidget* parent = nullptr);    
    
private slots:
    void clickedLink(const QUrl& url);
    void changeIsBuy(const QUrl& url);
};

#endif /* TradeBot_OrderPanel_hpp */
