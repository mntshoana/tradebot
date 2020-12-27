#ifndef TradeBot_Label_hpp
#define TradeBot_Label_hpp

#include <QTextBrowser>
#include <sstream>

class Label : public QTextBrowser {
public:
    Label(std::string title, QWidget* parent = nullptr );
    void rename(std::string title);
};

#endif /* TradeBot_OrderBookPanel_hpp */
