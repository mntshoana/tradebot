#ifndef valrTypes_h
#define valrTypes_h

#include "textPanel.hpp"
#include <string>
#include <iomanip>
#include <vector>
#include <sstream>
#include <fstream>

namespace VALR {
    /* Account Functions Types*/
    /*class Balance {
    public:
        Balance(){}
        std::string accountID;
        std::string asset;
        float balance;
        float reserved;
        float uncomfirmed;
        std::string toString();
    };

     declarePrintable(Balance);
     declarePrintableList(Balance);*/

    /* Market Functions Types */
    class Order {
    public:
        Order() {}
        std::string id;
        float price;
        float volume;
        int count; // count represents a tally with partial OrderBook (40 bids and 40 asks prices, orders with matching prices are tallied by count)
                   // count represents a position in queue when OrderBook is complete. Orders with matching price are seperate orders and are not tallied)
    };

    /* Market Functions Types */
    class UserOrder;
    class OrderBook {
    public:
        OrderBook() {};
        std::vector<Order> asks;
        std::vector<Order> bids;
        unsigned long long timestamp;
        std::string toString()const;
        std::string FormatHTML();
        std::string FormatHTMLWith(std::vector<UserOrder>*);
    };

    class CurrencyInfo {
    public:
        std::string symbol;
        bool isActive;
        std::string shortName;
        std::string longName;
        int decimalCount;
        int withdrawalDecimalCount;
        
        std::string toString()const;
    };
    

    class CurrencyPairInfo {
    public:
        std::string symbol;
        std::string baseCurrency;
        std::string quoteCurrency;
        bool isActive;
        std::string shortName;
        float baseMinTradable;
        float baseMaxTradable;
        float quoteMinTradable;
        float quoteMaxTradable;
        int tickSize;
        int baseDecimalCount;
        
        std::string toString()const;
    };

    class OrderTypeInfo {
    public:
        std::string symbol;
        std::string orderTypes;
    
        std::string toString()const;
    };

   
    class Ticker {
    public:
        Ticker() {}
        std::string pair;
        unsigned long long timestamp;
        float bid;
        float ask;
        float lastTrade;
        float lastClosed;
        float high;
        float low;
        float baseVolume;
        float changeFromPrevious;
        std::string toString() const;
    };
 

    class Trade {
    public:
        Trade() {}
        unsigned long long sequence;
        unsigned long long timestamp;
        float price;
        float baseVolume;
        float quoteVolume;
        std::string pair;
        std::string id;
        bool isBuy;
    std::string toString(std::string formatType = "") const;
    };

     std::fstream& operator << (std::fstream& stream, std::vector<Trade>& trades);
     std::fstream& operator >> (std::fstream& stream, std::vector<Trade>& trades);
//
    /* Order Functions Types*/
    /*class Fee {
    public:
        Fee(){}
        float thirtyDayVolume;
        float maker;
        float taker;
        std::string toString();
    };
    declarePrintable(Fee);
    declarePrintableList(Fee);
*/
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
        
        std::string toString() const;
    };
/*
    class UserTrade : public Trade {
    public:
        UserTrade() : Trade() {}
        std::string pair;
        std::string orderID;
        std::string type;
        float baseValue, baseFee;
        float counterValue, counterFee;
 
        std::string toString();
    };
    declarePrintable(UserTrade);*/

    /* Transfer Functions Types*/
    /*class Withdrawal {
    public:
        Withdrawal(){}
        long long id;
        std::string status;
        long long createdTime;
        std::string type;
        std::string currency;
        float amount;
        float fee;
     
        std::string toString();
    };
     declarePrintable(Withdrawal);
     */
}
declarePrintable(VALR::UserOrder);
declarePrintableList(VALR::UserOrder);
declarePrintable(VALR::OrderBook);
declarePrintableList(VALR::OrderBook);
declarePrintable(VALR::CurrencyInfo);
declarePrintableList(VALR::CurrencyInfo);
declarePrintable(VALR::CurrencyPairInfo);
declarePrintableList(VALR::CurrencyPairInfo);
declarePrintable(VALR::OrderTypeInfo);
declarePrintableList(VALR::OrderTypeInfo);
declarePrintable(VALR::Ticker);
declarePrintableList(VALR::Ticker);
declarePrintable(VALR::Trade);
declarePrintableList(VALR::Trade);
#endif /* valrTypes_h */
