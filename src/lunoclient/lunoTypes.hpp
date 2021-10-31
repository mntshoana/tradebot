#ifndef lunoTypes_h
#define lunoTypes_h


#include "textPanel.hpp"
#include <string>
#include <iomanip>
#include <vector>
#include <sstream>
#include <fstream>

namespace Luno {
    /* Account Functions Types*/
    class Balance {
    public:
        Balance(){}
        std::string accountID;
        std::string asset;
        float balance;
        float reserved;
        float uncomfirmed;
        std::string toString() const;
    };

    /* Market Functions Types */
    class Order {
    public:
        Order() {}
        float price;
        float volume;
    };

    class UserOrder;
    class OrderBook {
    private:
    public:
        OrderBook() {};
        std::vector<Order> asks;
        std::vector<Order> bids;
        long long timestamp;
        std::string toString()const;
        std::string FormatHTML()const;
        std::string FormatHTMLWith(std::vector<UserOrder>*);
    };
    

    class Ticker {
    public:
        Ticker() {}
        std::string pair;
        unsigned long long timestamp;
        float bid;
        float ask;
        float lastTrade;
        float rollingVolume;
        std::string status;
        unsigned long long getTimestamp(){return timestamp;}
        std::string toString()const;
    };
   

    class Trade {
    public:
        Trade() {}
        unsigned long long sequence;
        unsigned long long timestamp;
        float price;
        float volume;
        bool isBuy;
        std::string toString(std::string formatType = "")const;
    };

    std::fstream& operator << (std::fstream& stream, std::vector<Trade>& trades);
    std::fstream& operator >> (std::fstream& stream, std::vector<Trade>& trades);

    /* Order Functions Types*/
    class Fee {
    public:
        Fee(){}
        float thirtyDayVolume;
        float maker;
        float taker;
        std::string toString()const;
    };

    class UserOrder {
    public:
        UserOrder(){}
        std::string orderID;
        long long createdTime;
        long long expirationTime;
        long long completedTime;
        std::string type;
        std::string state;
        float price;
        float volume;
        float baseValue;
        float counterValue;
        float baseFee;
        float counterFee;
        std::string pair;
        
        std::string toString()const;
    };
    

    class UserTrade : public Trade {
    public:
        UserTrade() : Trade() {}
        std::string pair;
        std::string orderID;
        std::string type;
        float baseValue, baseFee;
        float counterValue, counterFee;
        
        std::string toString()const;
    };
    

    /* Transfer Functions Types*/
    class Withdrawal {
    public:
        Withdrawal(){}
        long long id;
        std::string status;
        long long createdTime;
        std::string type;
        std::string currency;
        float amount;
        float fee;
        
        std::string toString()const;
    };
   
}
declarePrintable(Luno::Balance);
declarePrintableList(Luno::Balance);
declarePrintable(Luno::OrderBook);
declarePrintableList(Luno::OrderBook);
declarePrintable(Luno::Ticker);
declarePrintableList(Luno::Ticker);
declarePrintable(Luno::Trade);
declarePrintableList(Luno::Trade);
declarePrintable(Luno::Fee);
declarePrintableList(Luno::Fee);
declarePrintable(Luno::UserOrder);
declarePrintableList(Luno::UserOrder);
declarePrintable(Luno::UserTrade);
declarePrintableList(Luno::UserTrade);
declarePrintable(Luno::Withdrawal);
declarePrintableList(Luno::Withdrawal);

#endif /* lunoTypes_h */
