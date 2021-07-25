#include "livePanel.hpp"

LivePanel::LivePanel(QWidget* parent) : QWidget(parent) {
    setGeometry(930, 0, 200, 720);
    
    orderview = new OrderPanel(this, "PRICE", "VOLUME");
    orderview->setGeometry(0, 0, 200, 358);
    
    tradeview = new OrderPanel(this, "Last Trade");
    tradeview->setGeometry(0, 356, 200, 225);
    
    livetrade = new TradePanel(this);
    livetrade->setGeometry(0, 580, 200, 141);
    
    
    connect(orderview, &OrderPanel::anchorClicked, livetrade, &TradePanel::clickedLink);
    connect(tradeview, &OrderPanel::anchorClicked, livetrade, &TradePanel::clickedLink);
}

void LivePanel::lightTheme(){
    livetrade->livetradeview->setStyleSheet(R"(QGroupBox {
                                       background-color: white;
                                       color: black;
                                       border: none;
                                   } QGroupBox::title {
                                       background-color:transparent;
                                   })");
}

void LivePanel::darkTheme(){
    livetrade->livetradeview->setStyleSheet(R"(QGroupBox {
                                       background-color: #1e1e1e;
                                       color: white;
                                       border: none;
                                   } QGroupBox::title {
                                       background-color:transparent;
                       })");
}
