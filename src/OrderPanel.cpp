#include "OrderPanel.hpp"

OrderPanel::OrderPanel(QWidget* parent) : QWidget(parent) {
    setGeometry(1180, 0, 200, 720);
    
    orderViewLabel = new Label("<th>PRICE</th> <th>VOLUME</th>", this);
    orderViewLabel->setGeometry(0, 0, 200, 30);
    orderview = new QTextBrowser(this);
    orderview->setGeometry(0, 29, 200, 328);
    orderview->setText("");
    orderview->setReadOnly(true);
    orderview->setOpenLinks(false);
    
    tradeViewLabel = new Label("<th colspan=2>Last Trade</th>", this);
    tradeViewLabel->setGeometry(0, 356, 200, 30);
    tradeview = new QTextBrowser(this);
    tradeview->setGeometry(0, 385, 200, 196);
    tradeview->setText("");
    tradeview->setReadOnly(true);
    tradeview->setOpenLinks(false);
    
    isBuy = true;
    liveTradeLabel = new Label("<th class=Chosen><a href=buy> Buy </a> </th>"
                               "<th> <a href=sell> Sell </a> </th>", this);
    liveTradeLabel->setGeometry(0, 580, 200, 30);
    livetradeview = new QGroupBox(this);
    livetradeview->setGeometry(0, 610, 200, 109);
    lblPrice = new QLabel("Price", this);
    lblAmount = new QLabel("Amount", this);
    txtPrice = new QLineEdit(this);
    txtAmount = new QLineEdit(this);
    livetradeviewLayout = new QGridLayout;
    livetradeviewLayout->addWidget(lblPrice, 1, 1);
    livetradeviewLayout->addWidget(txtPrice, 1, 2);
    livetradeviewLayout->addWidget(lblAmount, 2, 1);
    livetradeviewLayout->addWidget(txtAmount, 2, 2);
    
    request = new QPushButton(this);
    request->setGeometry(40, 670, 90, 30);
    request->setText("Bid");
    
    livetradeviewLayout->addWidget(request, 3, 1, 1, 2, Qt::AlignmentFlag::AlignCenter);
    livetradeview->setLayout(livetradeviewLayout);
    
    
    connect(orderview, &QTextBrowser::anchorClicked, this, &OrderPanel::clickedLink);
    connect(tradeview, &QTextBrowser::anchorClicked, this, &OrderPanel::clickedLink);
    connect(liveTradeLabel, &QTextBrowser::anchorClicked, this, &OrderPanel::changeIsBuy);
}

void OrderPanel::clickedLink(const QUrl& url){
    txtPrice->setText( url.path().toStdString().c_str());
    return;
}

void OrderPanel::changeIsBuy(const QUrl& url) {
    if (url.path().toStdString() == "buy"){
        isBuy = true;
        liveTradeLabel->rename("<th class=Chosen><a href=buy> Buy </a> </th>"
                               "<th> <a href=sell> Sell </a> </th>");
        request->setText("Bid");
    }
    else {
        isBuy = false;
        liveTradeLabel->rename("<th><a href=buy> Buy </a> </th>"
                               "<th class=Chosen> <a href=sell> Sell </a> </th>");
        request->setText("Ask");
    }
    
}
