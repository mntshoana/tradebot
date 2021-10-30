#include "lunoclient.hpp"

extern Client client;

namespace Luno {
    // GET ORDERBOOK
    //
    //
    OrderBook LunoClient::getOrderBook(std::string pair){
        std::string uri = "https://api.mybitx.com/api/1/orderbook_top?pair=" + pair;
        std::string res = client.request("GET", uri.c_str());
        
        int httpCode = client.getHttpCode();
        if (httpCode != 200)
            throw ResponseEx("Error " + std::to_string(httpCode) + " - " + res);
        
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
    OrderBook LunoClient::getFullOrderBook(std::string pair){
        std::string uri = "https://api.mybitx.com/api/1/orderbook?pair=" + pair;
        std::string res = client.request("GET", uri.c_str());
        
        int httpCode = client.getHttpCode();
        if (httpCode != 200)
            throw ResponseEx("Error " + std::to_string(httpCode) + " - " + res);
        
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

    

    // GET Ticker
    //
    //
    Ticker LunoClient::getTicker(std::string pair){
        std::string uri = "https://api.mybitx.com/api/1/ticker?pair=" + pair;
        std::string res = client.request("GET", uri.c_str());
        
        int httpCode = client.getHttpCode();
        if (httpCode != 200)
            throw ResponseEx("Error " + std::to_string(httpCode) + " - " + res);
        
        Ticker ticker;
        size_t last = 0, next = 0;
        
        // pair
        last = res.find("pair", last);
        last = res.find(":", last) + 2;
        next = res.find("\"", last);
        ticker.pair = res.substr(last, next-last);
        last = next + 1;
        
        // timestamp
        last = res.find("timestamp", last);
        last = res.find(":", last) + 1;
        next = res.find(",", last);
        std::string token = res.substr(last, next-last);
        ticker.timestamp = atoll(token.c_str());
        last = next + 1;
        
        // bid
        last = res.find("bid", last);
        last = res.find(":", last) + 2;
        next = res.find("\"", last);
        token = res.substr(last, next-last);
        ticker.bid = atof(token.c_str());
        last = next + 1;
        
        // asks
        last = res.find("ask", last);
        last = res.find(":", last) + 2;
        next = res.find("\"", last);
        token = res.substr(last, next-last);
        ticker.ask = atof(token.c_str());
        last = next + 1;
        
        // last trade
        last = res.find("last_trade", last);
        last = res.find(":", last) + 2;
        next = res.find("\"", last);
        token = res.substr(last, next-last);
        ticker.lastTrade = atof(token.c_str());
        last = next + 1;
        
        // rolling 24 hour volume
        last = res.find("rolling_24_hour_volume", last);
        last = res.find(":", last) + 2;
        next = res.find("\"", last);
        token = res.substr(last, next-last);
        ticker.rollingVolume = atof(token.c_str());
        last = next + 1;
        
        // status
        last = res.find("status", last);
        last = res.find(":", last) + 2;
        next = res.find("\"", last);
        ticker.status = res.substr(last, next-last);
        last = next + 1;
        
        return ticker;
    }

    // GET TICKERS
    //
    //
    std::vector<Ticker> LunoClient::getTickers(){
        std::string uri = "https://api.mybitx.com/api/1/tickers";
        std::string res = client.request("GET", uri.c_str());
        
        int httpCode = client.getHttpCode();
        if (httpCode != 200)
            throw ResponseEx("Error " + std::to_string(httpCode) + " - " + res);
        
        std::vector<Ticker> tickers;
        size_t last = 0, next = 0;
        last = res.find("[", last) + 1;
        
        while ((last = res.find("{", last)) != std::string::npos) {
            tickers.push_back(Ticker());
        
            // pair
            last = res.find("pair", last);
            last = res.find(":", last) + 2;
            next = res.find("\"", last);
            tickers.back().pair = res.substr(last, next-last);
            last = next + 1;
            
            // timestamp
            last = res.find("timestamp", last);
            last = res.find(":", last) + 1;
            next = res.find(",", last);
            std::string token = res.substr(last, next-last);
            tickers.back().timestamp = atoll(token.c_str());
            last = next + 1;
            
            // bid
            last = res.find("bid", last);
            last = res.find(":", last) + 2;
            next = res.find("\"", last);
            token = res.substr(last, next-last);
            tickers.back().bid = atof(token.c_str());
            last = next + 1;
            
            // asks
            last = res.find("ask", last);
            last = res.find(":", last) + 2;
            next = res.find("\"", last);
            token = res.substr(last, next-last);
            tickers.back().ask = atof(token.c_str());
            last = next + 1;
            
            // last trade
            last = res.find("last_trade", last);
            last = res.find(":", last) + 2;
            next = res.find("\"", last);
            token = res.substr(last, next-last);
            tickers.back().lastTrade = atof(token.c_str());
            last = next + 1;
            
            // rolling 24 hour volume
            last = res.find("rolling_24_hour_volume", last);
            last = res.find(":", last) + 2;
            next = res.find("\"", last);
            token = res.substr(last, next-last);
            tickers.back().rollingVolume = atof(token.c_str());
            last = next + 1;
            
            // status
            last = res.find("status", last);
            last = res.find(":", last) + 2;
            next = res.find("\"", last);
            tickers.back().status = res.substr(last, next-last);
            last = next + 1;
        }
        return tickers;
    }

    // GET TRADES
    //
    // Returns 100 trades only (cannot be changed), from a default of since the last 24 hours
    std::vector<Trade> LunoClient::getTrades(std::string pair, unsigned long long since ){
        std::string uri = "https://api.mybitx.com/api/1/trades?pair=" + pair;
        if (since != 0)
            uri += "&since=" + std::to_string(since);
        std::string res = client.request("GET", uri.c_str());
        
        int httpCode = client.getHttpCode();
        if (httpCode != 200)
            throw ResponseEx("Error " + std::to_string(httpCode) + " - " + res);
        
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

}
