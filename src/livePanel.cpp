#include "livePanel.hpp"

LivePanel::LivePanel(QWidget* parent, int exchange) : QWidget(parent) {
    setGeometry(930, 0, 200, 720);
    this->exchange = exchange;
    
    // Order Books
    orderview = new OrderPanel(this, "PRICE", "VOLUME");
    orderview->setGeometry(0, 0, 200, 358);
    
    // Public Trades
    tradeview = new OrderPanel(this, "Last Trade");
    tradeview->setGeometry(0, 356, 200, 225);
    
    // Private Bid/Ask
    livetrade = new TradePanel(this, exchange);
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
