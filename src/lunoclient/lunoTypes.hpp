#ifndef lunoTypes_h
#define lunoTypes_h

#include <string>
#include <vector>

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
    };

    template <class T> T& operator << (T& stream, Balance& balance);
    template <class T> T& operator << (T& stream, std::vector<Balance>& balances);

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
        std::string toString();
        std::string Format();
        std::string FormatToShowUserOrders(std::vector<UserOrder>*);
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
    };

    class Trade {
    public:
        Trade() {}
        unsigned long long sequence;
        unsigned long long timestamp;
        float price;
        float volume;
        bool isBuy;
    };

    //template <class T> T& operator << (T& stream, OrderBook& ob);
    template <class T> T& operator << (T& stream, Ticker& tick);
    template <class T> T& operator << (T& stream, std::vector<Ticker>& ticks);
    template <class T> T& operator << (T& stream, Trade& trade);
    template <class T> T& operator << (T& stream, std::vector<Trade>& trades);
    std::fstream& operator << (std::fstream& stream, std::vector<Trade>& trades);
    std::fstream& operator >> (std::fstream& stream, std::vector<Trade>& trades);

    /* Order Functions Types*/
    class Fee {
    public:
        Fee(){}
        float thirtyDayVolume;
        float maker;
        float taker;
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
    };

    class UserTrade : public Trade {
    public:
        UserTrade() : Trade() {}
        std::string pair;
        std::string orderID;
        std::string type;
        float baseValue, baseFee;
        float counterValue, counterFee;
    };

    template <class T> T& operator << (T& stream, Fee& fee);
    template <class T> T& operator << (T& stream, UserOrder& order);
    template <class T> T& operator << (T& stream, std::vector<UserOrder>& orders);
    template <class T> T& operator << (T& stream, UserTrade& trade);
    template <class T> T& operator << (T& stream, std::vector<UserTrade>& trades);

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
    };
    template <class T> T& operator << (T& stream, Withdrawal& fee);
    template <class T> T& operator << (T& stream, std::vector<Withdrawal>& trades);
}

#endif /* lunoTypes_h */
