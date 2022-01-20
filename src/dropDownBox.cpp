#include "dropDownBox.hpp"
#include "window.hpp"
#include "tradeBot.hpp"

DropDownBox::DropDownBox(QWidget* parent): QComboBox(parent){
    int exchange = HomeView::getExchangeVal();
    QStringList timeframeList;
    timeframeList << "LUNO" << "VALR";
    setGeometry(816, 523, 120, 18);
    addItems(timeframeList);
    
    setCurrentIndex(exchange);
    setVisible(true);
    connect(this, &QComboBox::currentTextChanged, this, [this, exchange](const QString &str){
        *TextPanel::textPanel << str.toStdString();
        *TextPanel::textPanel << "Exchange: " + std::to_string(exchange);
    
        std::string strSelection = str.toStdString();
        if (strSelection == "LUNO")
            TradeBot::staticThis->updateExchange(LUNO_EXCHANGE);
        else if (strSelection == "VALR")
            TradeBot::staticThis->updateExchange(VALR_EXCHANGE);
        
        close();
    });
}
