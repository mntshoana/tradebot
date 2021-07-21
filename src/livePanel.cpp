#include "livePanel.hpp"

LivePanel::LivePanel(QWidget* parent) : QWidget(parent) {
    setGeometry(930, 0, 200, 720);
    
    orderview = new OrderPanel(this, "PRICE", "VOLUME");
    orderview->setGeometry(0, 0, 200, 358);
    
    tradeview = new OrderPanel(this, "Last Trade");
    tradeview->setGeometry(0, 356, 200, 225);
    
    isBuy = true;
    liveTradeLabel = new Label("<th class=Chosen><a href=buy> Buy </a> </th>"
                               "<th> <a href=sell> Sell </a> </th>", this);
    liveTradeLabel->setGeometry(0, 580, 200, 30);
    livetradeview = new QGroupBox(this);
    livetradeview->setGeometry(0, 610, 200, 109);
    lblPrice = new QLabel("Price", this);
    lblAmount = new QLabel("Amount", this);
    
    request = new QPushButton(this);
    request->setGeometry(40, 670, 90, 30);
    request->setText("Bid");
    
    txtPrice = new LineBlock(this, request);
    txtAmount = new LineBlock(this, request);
    livetradeviewLayout = new QGridLayout;
    livetradeviewLayout->addWidget(lblPrice, 1, 1);
    livetradeviewLayout->addWidget(txtPrice, 1, 2);
    livetradeviewLayout->addWidget(lblAmount, 2, 1);
    livetradeviewLayout->addWidget(txtAmount, 2, 2);
    
    
    
    livetradeviewLayout->addWidget(request, 3, 1, 1, 2, Qt::AlignmentFlag::AlignCenter);
    livetradeview->setLayout(livetradeviewLayout);
    
    
    connect(orderview, &OrderPanel::anchorClicked, this, &LivePanel::clickedLink);
    connect(tradeview, &OrderPanel::anchorClicked, this, &LivePanel::clickedLink);
    connect(liveTradeLabel, &QTextBrowser::anchorClicked, this, &LivePanel::changeIsBuy);
}

void LivePanel::clickedLink(const QUrl& url){
    txtPrice->setText( url.path().toStdString().c_str());
    return;
}

void LivePanel::changeIsBuy(const QUrl& url) {
    if (url.path().toStdString() == "buy"){
        isBuy = true;
        liveTradeLabel->setLabel("<th class=Chosen><a href=buy> Buy </a> </th>"
                               "<th> <a href=sell> Sell </a> </th>");
        request->setText("Bid");
    }
    else {
        isBuy = false;
        liveTradeLabel->setLabel("<th><a href=buy> Buy </a> </th>"
                               "<th class=Chosen> <a href=sell> Sell </a> </th>");
        request->setText("Ask");
    }
    
}
