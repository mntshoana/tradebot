#ifndef LIVE_PANEL_HEADER
#define LIVE_PANEL_HEADER

#include "label.hpp"

#include <QLayout>
#include <QScrollBar>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>

#include "lunoclient.hpp"
#include "lineBlock.hpp"

#include "orderPanel.hpp"

//
class LivePanel : public QWidget {
    Q_OBJECT
    Label* liveTradeLabel;
public:
    bool isBuy;
    OrderPanel* orderview;
    OrderPanel *tradeview;
    QGroupBox *livetradeview;
    QGridLayout *livetradeviewLayout;
    QLabel *lblPrice, *lblAmount;
    LineBlock* txtPrice, *txtAmount;
    QPushButton* request;
    
    LivePanel(QWidget* parent = nullptr);
    
private slots:
    void clickedLink(const QUrl& url);
    void changeIsBuy(const QUrl& url);
};

#endif /* TradeBot_OrderPanel_hpp */
