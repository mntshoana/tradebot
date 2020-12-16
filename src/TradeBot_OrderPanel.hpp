#ifndef TradeBot_OrderPanel_hpp
#define TradeBot_OrderPanel_hpp

#include "TradeBot_Label.hpp"

#include <QLayout>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>


class OrderPanel {
    Label* orderViewLabel, *tradeViewLabel, *liveTradeLabel;
public:
    QTextBrowser* orderview, *tradeview;
    QGroupBox *livetradeview;
    QGridLayout *livetradeviewLayout;
    QLabel *lblPrice, *lblAmount;
    QLineEdit* txtPrice, *txtAmount;
    
    OrderPanel(QWidget* parent = nullptr);
};

#endif /* TradeBot_OrderPanel_hpp */
