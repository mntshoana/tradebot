#include "TradeBot_OrderPanel.hpp"

OrderPanel::OrderPanel(QWidget* parent) {
    orderViewLabel = new Label("<th>PRICE</th> <th>VOLUME</th>", parent);
    orderViewLabel->setGeometry(1180, 0, 200, 30);
    orderview = new QTextBrowser(parent);
    orderview->setGeometry(1180, 29, 200, 328);
    orderview->setText("");
    orderview->setReadOnly(true);
    orderview->setOpenLinks(false);
    
    tradeViewLabel = new Label("<th colspan=2>Last Trade</th>", parent);
    tradeViewLabel->setGeometry(1180, 356, 200, 30);
    tradeview = new QTextBrowser(parent);
    tradeview->setGeometry(1180, 385, 200, 196);
    tradeview->setText("");
    tradeview->setReadOnly(true);
    tradeview->setOpenLinks(false);
    
    liveTradeLabel = new Label("<th>Buy</th> <th>Sell</th>", parent);
    liveTradeLabel->setGeometry(1180, 580, 200, 30);
    livetradeview = new QGroupBox(parent);
    livetradeview->setGeometry(1180, 610, 200, 109);
    lblPrice = new QLabel("Price", parent);
    lblAmount = new QLabel("Amount", parent);
    txtPrice = new QLineEdit(parent);
    txtAmount = new QLineEdit(parent);
    livetradeviewLayout = new QGridLayout;
    livetradeviewLayout->addWidget(lblPrice, 1, 1);
    livetradeviewLayout->addWidget(txtPrice, 1, 2);
    livetradeviewLayout->addWidget(lblAmount, 2, 1);
    livetradeviewLayout->addWidget(txtAmount, 2, 2);
    livetradeview->setLayout(livetradeviewLayout);
}
