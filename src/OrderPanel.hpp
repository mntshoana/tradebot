#ifndef TradeBot_OrderPanel_hpp
#define TradeBot_OrderPanel_hpp

#include "Label.hpp"

#include <QLayout>
#include <QScrollBar>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>


class OrderPanel : public QObject {
    Q_OBJECT
    Label* orderViewLabel, *tradeViewLabel, *liveTradeLabel;
public:
    bool isBuy;
    
    QTextBrowser* orderview, *tradeview;
    QGroupBox *livetradeview;
    QGridLayout *livetradeviewLayout;
    QLabel *lblPrice, *lblAmount;
    QLineEdit* txtPrice, *txtAmount;
    QPushButton* request;
    
    OrderPanel(QWidget* parent = nullptr);
    virtual ~OrderPanel() {}
    
    
private slots:
    void clickedLink(const QUrl& url);
    void changeIsBuy(const QUrl& url);
};

#endif /* TradeBot_OrderPanel_hpp */
