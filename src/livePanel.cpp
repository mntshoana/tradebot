#include "livePanel.hpp"

LivePanel::LivePanel(QWidget* parent) : QWidget(parent) {
    setGeometry(930, 0, 200, 720);
    
    orderview = new OrderPanel(this, "PRICE", "VOLUME");
    orderview->setGeometry(0, 0, 200, 358);
    
    tradeview = new OrderPanel(this, "Last Trade");
    tradeview->setGeometry(0, 356, 200, 225);
    
    livetrade = new TradePanel(this);
    livetrade->setGeometry(0, 580, 200, 139);
    
    
    connect(orderview, &OrderPanel::anchorClicked, livetrade, &TradePanel::clickedLink);
    connect(tradeview, &OrderPanel::anchorClicked, livetrade, &TradePanel::clickedLink);
}
