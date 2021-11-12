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
        size_t last = 0;
        
        // timestamp
        last = res.find("timestamp", last);
        std::string token = extractNextString(res, last, ",", last);
        ob.timestamp = atoll(token.c_str());
        
        // asks and bids
        last = res.find("asks", last);
        std::string asks = extractNextStringBlock(res, last, "[", "]", last);

        last = res.find("bids", last);
        std::string bids = extractNextStringBlock(res, last, "[", "]", last);
        res.erase();
        
        last = 0;
        while ((last = asks.find("{", last)) != std::string::npos) {
            Order order;
            // price
            token =  extractNextString(asks, last, last);
            order.price = atof(token.c_str());
            
            // volume
            token = extractNextString(asks, last, last);
            order.volume = atof(token.c_str());
            
            ob.asks.push_back(order);
        }
        asks.erase();
        
        last = 0;
        while ((last = bids.find("{", last)) != std::string::npos) {
            Order order;
            // price
            token = extractNextString(bids, last, last);
            order.price = atof(token.c_str());
            
            // volume
            token = extractNextString(bids, last, last);
            order.volume = atof(token.c_str());
            
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
        size_t last = 0;
        
        // timestamp
        last = res.find("timestamp", last);
        std::string token = extractNextString(res, last, ",", last);
        ob.timestamp = atoll(token.c_str());
        
        // asks and bids
        last = res.find("asks", last);
        std::string asks = extractNextStringBlock(res, last, "[", "]", last);

        last = res.find("bids", last);
        std::string bids = extractNextStringBlock(res, last, "[", "]", last);
        res.erase();
        
        last = 0;
        while ((last = asks.find("{", last)) != std::string::npos) {
            Order order;
            // price
            
            token = extractNextString(asks, last, last);
            order.price = atof(token.c_str());
            
            // volume
            token =  extractNextString(asks, last, last);
            order.volume = atof(token.c_str());
            
            ob.asks.push_back(order);
        }
        asks.erase();
        
        last = 0;
        while ((last = bids.find("{", last)) != std::string::npos) {
            Order order;
            // price
            token = extractNextString(bids, last, last);
            order.price = atof(token.c_str());
            
            // volume
            token = extractNextString(bids, last, last);
            order.volume = atof(token.c_str());
            
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
        size_t last = 0;
        
        // pair
        last = res.find("pair", last);
        ticker.pair = extractNextString(res, last, last);
        
        // timestamp
        last = res.find("timestamp", last);
        std::string token = extractNextString(res, last, ",", last);
        ticker.timestamp = atoll(token.c_str());
        
        // bid
        last = res.find("bid", last);
        token = extractNextString(res, last, last);
        ticker.bid = atof(token.c_str());
        
        // asks
        last = res.find("ask", last);
        token = extractNextString(res, last, last);
        ticker.ask = atof(token.c_str());
        
        // last trade
        last = res.find("last_trade", last);
        token = extractNextString(res, last, last);
        ticker.lastTrade = atof(token.c_str());
        
        // rolling 24 hour volume
        last = res.find("rolling_24_hour_volume", last);
        token = extractNextString(res, last, last);
        ticker.rollingVolume = atof(token.c_str());
        
        // status
        last = res.find("status", last);
        ticker.status = extractNextString(res, last, last);
        
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
        size_t last = 0;
        last = res.find("[", last) + 1;
        
        while ((last = res.find("{", last)) != std::string::npos) {
            tickers.push_back(Ticker());
        
            // pair
            last = res.find("pair", last);
            tickers.back().pair = extractNextString(res, last, last);
            
            // timestamp
            last = res.find("timestamp", last);
            std::string token = extractNextString(res, last, ",", last);
            tickers.back().timestamp = atoll(token.c_str());
            
            // bid
            last = res.find("bid", last);
            token = extractNextString(res, last, last);
            tickers.back().bid = atof(token.c_str());
            
            // asks
            last = res.find("ask", last);
            token = extractNextString(res, last, last);
            tickers.back().ask = atof(token.c_str());
            
            // last trade
            last = res.find("last_trade", last);
            token = extractNextString(res, last, last);
            tickers.back().lastTrade = atof(token.c_str());
            
            // rolling 24 hour volume
            last = res.find("rolling_24_hour_volume", last);
            token = extractNextString(res, last, last);
            tickers.back().rollingVolume = atof(token.c_str());
            
            // status
            last = res.find("status", last);
            tickers.back().status = extractNextString(res, last, last);
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
        size_t last = 0;
        last = res.find("[", last) + 1;
        
        while ((last = res.find("{", last)) != std::string::npos) {
            trades.push_back(Trade());
           
            // sequence
            last = res.find("sequence", last);
            std::string token = extractNextString(res, last, ",", last);
            trades.back().sequence = atoll(token.c_str());
               
            // timestamp
            last = res.find("timestamp", last);
            token = extractNextString(res, last, ",", last);
            trades.back().timestamp = atoll(token.c_str());
            
            // price
            last = res.find("price", last);
            token = extractNextString(res, last, last);
            trades.back().price = atof(token.c_str());
               
            // volume
            last = res.find("volume", last);
            token = extractNextString(res, last, last);
            trades.back().volume = atof(token.c_str());
               
            // isBuy
            last = res.find("is_buy", last);
            token = extractNextString(res, last,"}" , last);
            trades.back().isBuy = (token == "true") ? true : false;
        }
        
        return trades;
    }

}
