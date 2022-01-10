#ifndef TRADE_PANEL_HEADER
#define TRADE_PANEL_HEADER

#include <QLayout>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>

#include "textPanel.hpp"
#include "label.hpp"
#include "lineBlock.hpp"
#include "lunoclient.hpp"
#include "exceptions.hpp"

class TradePanel : public QWidget {
    Q_OBJECT
    TextPanel* text;
    Label* header;
    int exchange;
    
    std::string loadHeader();
public:
    bool isBuy;
    
    TradePanel(QWidget* parent = nullptr, int exchange = LUNO_EXCHANGE);
    
    QGroupBox *livetradeview;
    QGridLayout *livetradeviewLayout;
    QLabel *lblPrice, *lblAmount;
    LineBlock* txtPrice, *txtAmount;
    QPushButton* request;
    
    void setGeometry(int ax, int ay, int aw, int ah);
    
    signals:
    void enqueueUserOrder(std::string orderID);
    
    public slots:
        void clickedLink(const QUrl& url);
    
    private slots:
        void changeIsBuy(const QUrl& url);
        void executeTrade();
    
    
};
#endif
