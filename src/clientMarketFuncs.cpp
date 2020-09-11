#include "client.hpp"

// GET ORDERBOOK
//
//
OrderBook Client::getOrderBook(std::string pair){
    std::string uri = "https://api.mybitx.com/api/1/orderbook_top?pair=" + pair;
    std::string res = this->request("GET", uri.c_str());
    
    if (httpCode != 200)
        throw ResponseEx("Error - " + res);
    
    OrderBook ob;
    size_t last = 0, next = 0;
    
    // timestamp
    last = res.find("timestamp", last);
    last = res.find(":", last) + 1;
    next = res.find(",", last);
    std::string token = res.substr(last, next-last);
    ob.timestamp = atoll(token.c_str());
    last = next + 1;
    
    // asks and bids
    last = res.find("asks", last);
    last = res.find("[", last) + 1;
    next = res.find("]", last);
    std::string asks = res.substr(last, next-last);
    last = next + 1;
    
    last = res.find("bids", last);
    last = res.find("[", last) + 1;
    next = res.find("]", last);
    std::string bids = res.substr(last, next-last);
    res.erase();
    
    last = next = 0;
    while ((last = asks.find("{", last)) != std::string::npos) {
        Order order;
        // price
        last = asks.find(":", last) + 2;
        next = asks.find("\"", last);
        token = asks.substr(last, next-last);
        order.price = atof(token.c_str());
        last = next + 1;
        
        // volume
        last = asks.find(":", last) + 2;
        next = asks.find("\"", last);
        token = asks.substr(last, next-last);
        order.volume = atof(token.c_str());
        last = next + 1;
        ob.asks.push_back(order);
    }
    asks.erase();
    
    last = next = 0;
    while ((last = bids.find("{", last)) != std::string::npos) {
        Order order;
        // price
        last = bids.find(":", last) + 2;
        next = bids.find("\"", last);
        token = bids.substr(last, next-last);
        order.price = atof(token.c_str());
        last = next + 1;
        
        // volume
        last = bids.find(":", last) + 2;
        next = bids.find("\"", last);
        token = bids.substr(last, next-last);
        order.volume = atof(token.c_str());
        last = next + 1;
        ob.bids.push_back(order);
    }
    return ob;
}
// GET FULL ORDERBOOK
//
//
OrderBook Client::getFullOrderBook(std::string pair){
    std::string uri = "https://api.mybitx.com/api/1/orderbook?pair=" + pair;
    std::string res = this->request("GET", uri.c_str());
    
    if (httpCode != 200)
        throw ResponseEx("Error - " + res);
    
    OrderBook ob;
    size_t last = 0, next = 0;
    
    // timestamp
    last = res.find("timestamp", last);
    last = res.find(":", last) + 1;
    next = res.find(",", last);
    std::string token = res.substr(last, next-last);
    ob.timestamp = atoll(token.c_str());
    last = next + 1;
    
    // asks and bids
    last = res.find("asks", last);
    last = res.find("[", last) + 1;
    next = res.find("]", last);
    std::string asks = res.substr(last, next-last);
    last = next + 1;
    
    last = res.find("bids", last);
    last = res.find("[", last) + 1;
    next = res.find("]", last);
    std::string bids = res.substr(last, next-last);
    res.erase();
    
    last = next = 0;
    while ((last = asks.find("{", last)) != std::string::npos) {
        Order order;
        // price
        last = asks.find(":", last) + 2;
        next = asks.find("\"", last);
        token = asks.substr(last, next-last);
        order.price = atof(token.c_str());
        last = next + 1;
        
        // volume
        last = asks.find(":", last) + 2;
        next = asks.find("\"", last);
        token = asks.substr(last, next-last);
        order.volume = atof(token.c_str());
        last = next + 1;
        ob.asks.push_back(order);
    }
    asks.erase();
    
    last = next = 0;
    while ((last = bids.find("{", last)) != std::string::npos) {
        Order order;
        // price
        last = bids.find(":", last) + 2;
        next = bids.find("\"", last);
        token = bids.substr(last, next-last);
        order.price = atof(token.c_str());
        last = next + 1;
        
        // volume
        last = bids.find(":", last) + 2;
        next = bids.find("\"", last);
        token = bids.substr(last, next-last);
        order.volume = atof(token.c_str());
        last = next + 1;
        ob.bids.push_back(order);
    }
    return ob;
}

template <class T> T& operator << (T& stream, OrderBook& ob) {
    std::stringstream ss;
    ss << "Timestamp: " << ob.timestamp << "\n";
    stream.append(ss.str().c_str());
    ss.str(""); // clear content of string string
    
    ss << "Asks\n";
    for (Order order : ob.asks){
        ss << "Price: " << order.price << " ";
        ss << "Volume: " << order.volume << ",  ";
    }
    stream.append(ss.str().c_str());
    ss.str(""); // clear content of string string
    
    ss << "\n" << "Bids\n";
    for (Order order : ob.bids){
        ss << "Price: " << order.price << " ";
        ss << "Volume: " << order.volume << ",  ";
    }
    stream.append(ss.str().c_str());
    return stream;
}
template QTextEdit& operator << <QTextEdit>(QTextEdit&, OrderBook& ob);

// GET TICK
//
//
Tick Client::getTick(std::string pair){
    std::string uri = "https://api.mybitx.com/api/1/ticker?pair=" + pair;
    std::string res = this->request("GET", uri.c_str());
    
    if (httpCode != 200)
        throw ResponseEx("Error - " + res);
    
    Tick tick;
    size_t last = 0, next = 0;
    
    // pair
    last = res.find("pair", last);
    last = res.find(":", last) + 2;
    next = res.find("\"", last);
    tick.pair = res.substr(last, next-last);
    last = next + 1;
    
    // timestamp
    last = res.find("timestamp", last);
    last = res.find(":", last) + 1;
    next = res.find(",", last);
    std::string token = res.substr(last, next-last);
    tick.timestamp = atoll(token.c_str());
    last = next + 1;
    
    // bid
    last = res.find("bid", last);
    last = res.find(":", last) + 2;
    next = res.find("\"", last);
    token = res.substr(last, next-last);
    tick.bid = atof(token.c_str());
    last = next + 1;
    
    // asks
    last = res.find("ask", last);
    last = res.find(":", last) + 2;
    next = res.find("\"", last);
    token = res.substr(last, next-last);
    tick.ask = atof(token.c_str());
    last = next + 1;
    
    // last trade
    last = res.find("last_trade", last);
    last = res.find(":", last) + 2;
    next = res.find("\"", last);
    token = res.substr(last, next-last);
    tick.lastTrade = atof(token.c_str());
    last = next + 1;
    
    // rolling 24 hour volume
    last = res.find("rolling_24_hour_volume", last);
    last = res.find(":", last) + 2;
    next = res.find("\"", last);
    token = res.substr(last, next-last);
    tick.rollingVolume = atof(token.c_str());
    last = next + 1;
    
    // status
    last = res.find("status", last);
    last = res.find(":", last) + 2;
    next = res.find("\"", last);
    tick.status = res.substr(last, next-last);
    last = next + 1;
    
    return tick;
}

template <class T> T& operator << (T& stream, Tick& tick) {
    std::stringstream ss;
    ss << "Pair: " << tick.pair << "\n";
    ss << "Timestamp: " << tick.timestamp << "\n";
    ss << "Bid: " << tick.bid << "\n";
    ss << "Ask: " << tick.ask << "\n";
    ss << "Last trade: " << tick.lastTrade << "\n";
    ss << "Rolling 24 hour volume: " << tick.rollingVolume << "\n";
    ss << "Status: " << tick.status << "\n";
    stream.append(ss.str().c_str());
    return stream;
}
template QTextEdit& operator << <QTextEdit>(QTextEdit& stream, Tick& tick);

// GET TICKS
//
//
std::vector<Tick> Client::getTicks(){
    std::string uri = "https://api.mybitx.com/api/1/tickers";
    std::string res = this->request("GET", uri.c_str());
    
    if (httpCode != 200)
        throw ResponseEx("Error - " + res);
    
    std::vector<Tick> ticks;
    size_t last = 0, next = 0;
    last = res.find("[", last) + 1;
    
    while ((last = res.find("{", last)) != std::string::npos) {
        ticks.push_back(Tick());
    
        // pair
        last = res.find("pair", last);
        last = res.find(":", last) + 2;
        next = res.find("\"", last);
        ticks.back().pair = res.substr(last, next-last);
        last = next + 1;
        
        // timestamp
        last = res.find("timestamp", last);
        last = res.find(":", last) + 1;
        next = res.find(",", last);
        std::string token = res.substr(last, next-last);
        ticks.back().timestamp = atoll(token.c_str());
        last = next + 1;
        
        // bid
        last = res.find("bid", last);
        last = res.find(":", last) + 2;
        next = res.find("\"", last);
        token = res.substr(last, next-last);
        ticks.back().bid = atof(token.c_str());
        last = next + 1;
        
        // asks
        last = res.find("ask", last);
        last = res.find(":", last) + 2;
        next = res.find("\"", last);
        token = res.substr(last, next-last);
        ticks.back().ask = atof(token.c_str());
        last = next + 1;
        
        // last trade
        last = res.find("last_trade", last);
        last = res.find(":", last) + 2;
        next = res.find("\"", last);
        token = res.substr(last, next-last);
        ticks.back().lastTrade = atof(token.c_str());
        last = next + 1;
        
        // rolling 24 hour volume
        last = res.find("rolling_24_hour_volume", last);
        last = res.find(":", last) + 2;
        next = res.find("\"", last);
        token = res.substr(last, next-last);
        ticks.back().rollingVolume = atof(token.c_str());
        last = next + 1;
        
        // status
        last = res.find("status", last);
        last = res.find(":", last) + 2;
        next = res.find("\"", last);
        ticks.back().status = res.substr(last, next-last);
        last = next + 1;
    }
    return ticks;
}

template <class T> T& operator << (T& stream,  std::vector<Tick>& ticks) {
    for (Tick& tick : ticks){
        stream << tick;
    }
    return stream;
}
template QTextEdit& operator << <QTextEdit>(QTextEdit& stream, std::vector<Tick>& ticks);

// GET TRADES
//
// Returns 100 trades, from a default of since the last 24 hours
std::vector<Trade> Client::getTrades(std::string pair, long long since){
    std::string uri = "https://api.mybitx.com/api/1/trades?pair=" + pair;
    if (since != 0)
        uri += "&since=" + std::to_string(since);
    std::string res = this->request("GET", uri.c_str());
    
    if (httpCode != 200)
        throw ResponseEx("Error - " + res);
    
    std::vector<Trade> trades;
    size_t last = 0, next = 0;
    last = res.find("[", last) + 1;
    
    while ((last = res.find("{", last)) != std::string::npos) {
        trades.push_back(Trade());
       
        // sequence
        last = res.find("sequence", last);
        last = res.find(":", last) + 1;
        next = res.find(",", last);
        std::string token = res.substr(last, next-last);
        trades.back().sequence = atoll(token.c_str());
        last = next + 1;
           
        // timestamp
        last = res.find("timestamp", last);
        last = res.find(":", last) + 1;
        next = res.find(",", last);
        token = res.substr(last, next-last);
        trades.back().timestamp = atoll(token.c_str());
        last = next + 1;
        
        // price
        last = res.find("price", last);
        last = res.find(":", last) + 2;
        next = res.find("\"", last);
        token = res.substr(last, next-last);
        trades.back().price = atof(token.c_str());
        last = next + 1;
           
        // volume
        last = res.find("volume", last);
        last = res.find(":", last) + 2;
        next = res.find("\"", last);
        token = res.substr(last, next-last);
        trades.back().volume = atof(token.c_str());
        last = next + 1;
           
        // isBuy
        last = res.find("is_buy", last);
        last = res.find(":", last) + 1;
        next = res.find("}", last);
        token = res.substr(last, next-last);
        trades.back().isBuy = (token == "true") ? true : false;
        last = next + 1;
    }
    
    return trades;
}

template <class T> T& operator << (T& stream, Trade& trade){
    std::stringstream ss;
    ss << "Sequence: " << trade.sequence << "\n";
    ss << "Timestamp: " << trade.timestamp << "\n";
    ss << "Price: " << trade.price << "\n";
    ss << "Volume: " << trade.volume << "\n";
    ss << "Is buy: " << (trade.isBuy ? "true":"false") << "\n";
    stream.append(ss.str().c_str());
    return stream;
}
template <class T> T& operator << (T& stream, std::vector<Trade>& trades){
    for (Trade& trade : trades){
        stream << trade;
    }
    return stream;
}
template QTextEdit& operator << <QTextEdit>(QTextEdit& stream, Trade& trade);
template QTextEdit& operator << <QTextEdit>(QTextEdit& stream, std::vector<Trade>& trades);
