#ifndef LIVE_PANEL_HEADER
#define LIVE_PANEL_HEADER

#include <QScrollBar>


#include "lunoclient.hpp"
#include "tradePanel.hpp"

#include "orderPanel.hpp"

//
class LivePanel : public QWidget {
    Q_OBJECT
    
public:
    OrderPanel* orderview;
    OrderPanel* tradeview;
    TradePanel* livetrade;
    
    LivePanel(QWidget* parent = nullptr);
    
    void lightTheme();
    void darkTheme();
};

#endif
