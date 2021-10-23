#ifndef LIVE_PANEL_HEADER
#define LIVE_PANEL_HEADER

#include <QScrollBar>


#include "lunoclient.hpp"
#include "valrclient.hpp"
#include "tradePanel.hpp"

#include "orderPanel.hpp"

//
class LivePanel : public QWidget {
    Q_OBJECT
    int exchange;
    
public:
    OrderPanel* orderview;
    OrderPanel* tradeview;
    TradePanel* livetrade;
    
    LivePanel(QWidget* parent = nullptr, int exchange = LUNO_EXCHANGE);
    
    void lightTheme();
    void darkTheme();
};

#endif
