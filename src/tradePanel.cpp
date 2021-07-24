#include "tradePanel.hpp"

TradePanel::TradePanel(QWidget* parent){
    isBuy = true;
    header = new Label( loadHeader(), parent);
    
    livetradeview = new QGroupBox(parent);
    
    lblPrice = new QLabel("Price", parent);
    lblAmount = new QLabel("Amount", parent);
    
    request = new QPushButton(parent);
    
    request->setText("Bid");
    
    txtPrice = new LineBlock(parent, request);
    txtAmount = new LineBlock(parent, request);
    livetradeviewLayout = new QGridLayout;
    livetradeviewLayout->addWidget(lblPrice, 1, 1);
    livetradeviewLayout->addWidget(txtPrice, 1, 2);
    livetradeviewLayout->addWidget(lblAmount, 2, 1);
    livetradeviewLayout->addWidget(txtAmount, 2, 2);
    
    
    
    livetradeviewLayout->addWidget(request, 3, 1, 1, 2, Qt::AlignmentFlag::AlignCenter);
    livetradeview->setLayout(livetradeviewLayout);
    
    connect(header, &QTextBrowser::anchorClicked, this, &TradePanel::changeIsBuy);
    
    connect(request, &QPushButton::clicked, this, &TradePanel::executeTrade);
}

std::string TradePanel::loadHeader(){
    std::string labelBuilder;
    labelBuilder  = "<th ";
    labelBuilder += (isBuy ? "class=Chosen>" : ">");
    labelBuilder += "<a href=buy>";
    labelBuilder += "Buy";
    labelBuilder += "</a>";
    labelBuilder += "</th>";
    
    labelBuilder += "<th ";
    labelBuilder += (!isBuy ? "class=Chosen>" : ">");
    labelBuilder += "<a href=sell>";
    labelBuilder += "Sell";
    labelBuilder += "</a>";
    labelBuilder += "</th>";
    
    return labelBuilder;
}

void TradePanel::setGeometry(int ax, int ay, int aw, int ah) {
    header->setGeometry(ax, ay, aw, 30);
    livetradeview->setGeometry(ax, ay+30, aw, ah - 31);
    request->setGeometry(40, ay + 90, 90, 30);
}

void TradePanel::clickedLink(const QUrl& url){
    txtPrice->setText( url.path().toStdString().c_str());
    return;
}

void TradePanel::changeIsBuy(const QUrl& url) {
    if (url.path().toStdString() == "buy"){
        isBuy = true;
        header->setLabel(loadHeader());
        request->setText("Bid");
    }
    else {
        isBuy = false;
        header->setLabel(loadHeader());
        request->setText("Ask");
    }
}

void TradePanel::executeTrade(){
    
    int price = atoi(txtPrice->text().toStdString().c_str());
    float amount = atof(txtAmount->text().toStdString().c_str());
    
    if (price == 0){
        text << "Error - Price cannot be empty.";
        return;
    }
    
    const char *action = (isBuy) ? "BID" : "ASK";
    
        if (amount == 0.0f){
            if (isBuy){
                auto balances = Luno::LunoClient::getBalances("ZAR");
                amount = (balances[0].balance - balances[0].reserved) / float(price);
            }
            else if (!isBuy){
                auto balances = Luno::LunoClient::getBalances("XBT");
                amount = balances[0].balance - balances[0].reserved;
            }
        }
       
        else if (amount < 0.0005f){
            text << "Error - cannot trade for less than 0.000500 BTC";
            return;
        }
    try {
        // output
        std::string result = Luno::LunoClient::postLimitOrder("XBTZAR", action, amount, price);
        text << std::string(action) + " order at price: " + std::to_string(price);
        text << "   Amount: " + std::to_string(amount);
        text << "   Valued: " + std::to_string(amount * price);
        text << "COMPLETE: " + result;
        
    } catch (ResponseEx ex){
        text << " [Error] Unable to place order! at " + std::string(__FILE__) + ", line: " + std::to_string(__LINE__);
        text << ex.String();
    }
}
