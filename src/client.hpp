
#ifndef client_hpp
#define client_hpp

#include <QTextEdit>
#include <curl/curl.h>
#include <sstream>
#include <vector>

#include "exceptions.hpp"

/* Market Funs Types */
class Order {
public:
    Order() {}
    float price;
    float volume;
};
class OrderBook {
private:
public:
    std::vector<Order> asks;
    std::vector<Order> bids;
    long long timestamp;
    OrderBook() {};
};

class Tick {
public:
    Tick() {}
    std::string pair;
    long long timestamp;
    float bid;
    float ask;
    float lastTrade;
    float rollingVolume;
    std::string status;
};

class Trade {
public:
    Trade() {}
    long long sequence;
    long long timestamp;
    float price;
    float volume;
    bool isBuy;
};

template <class T> T& operator << (T& stream, OrderBook& ob);
template <class T> T& operator << (T& stream, Tick& tick);
template <class T> T& operator << (T& stream, std::vector<Tick>& ticks);
template <class T> T& operator << (T& stream, Trade& trade);
template <class T> T& operator << (T& stream, std::vector<Trade>& trades);


/* Order Funcs Types*/
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

class Client {
private:
    CURL *curl;
    CURLcode res;
    int httpCode;
    
    std::string   request  (const char* method, const char* uri, bool auth = false);
    std::string parse (std::string& string);
    static size_t redirect (char *ptr, size_t size, size_t nmemb, void *stream);
public:
    static std::stringstream ss;
    Client();
    ~Client();
    /* Market Funcs */
    OrderBook getOrderBook(std::string pair);
    OrderBook getFullOrderBook(std::string pair);
    Tick getTick(std::string pair);
    std::vector<Tick> getTicks();
    std::vector<Trade> getTrades(std::string pair, long long since = 0);
    
    /* Order Funcs */
    Fee getFeeInfo(std::string pair);
    std::vector<UserOrder> getUserOrders(std::string pair = "",
                                 std::string state = "",
                                 int limit = 0,
                                 long long before = 0);
    std::vector<UserTrade> getUserTrades(std::string pair,
                                         long long since = 0,
                                         long long before = 0,
                                         long long after_seq = 0,
                                         long long before_seq = 0,
                                         bool sort = false,
                                         int limit = 0);
    // Post market order
    // Get order
    // Post limit order
    // Stop order
};


#endif /* client_hpp */

/*
 “Machine Learning, or AI, is when a computer program learns from an experience with respect to some tasks, so long as its performance at this task improves with experience.”
 
 Page 21 - Eigen Vectors -- WTF?

 */
