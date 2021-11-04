#include "valrclient.hpp"
#include <iostream>
extern Client client;

namespace VALR {
    std::string host = "https://api.valr.com";
    // GET ORDERBOOK
    //
    //
    OrderBook VALRClient::getOrderBook(std::string pair){
        std::string path = "/v1/public/" + pair + "/orderbook";
        std::string res = client.request("GET", (host+path).c_str(), false, VALR_EXCHANGE);
        
        int httpCode = client.getHttpCode();
        if (httpCode != 200)
            throw ResponseEx("Error " + std::to_string(httpCode) + " - " + res);
        
        OrderBook ob;
        size_t last = 0, next = 0;
        
        std::string token;
        // asks and bids
        last = res.find("Asks", last);
        last = res.find("[", last) + 1;
        next = res.find("]", last);
        std::string asks = res.substr(last, next-last);
        last = next + 1;
        
        last = res.find("Bids", last);
        last = res.find("[", last) + 1;
        next = res.find("]", last);
        std::string bids = res.substr(last, next-last);
        res.erase();
        
        last = next = 0;
        while ((last = asks.find("{", last)) != std::string::npos) {
            Order order;
            
            // ignore "side":"..."
            last = asks.find(":", last) + 2;
            
            // volume
            last = asks.find(":", last) + 2;
            next = asks.find("\"", last);
            token = asks.substr(last, next-last);
            order.volume = atof(token.c_str());
            last = next + 1;
            
            // price
            last = asks.find(":", last) + 2;
            next = asks.find("\"", last);
            token = asks.substr(last, next-last);
            order.price = atof(token.c_str());
            last = next + 1;
            
            // ignore currencPair
            last = asks.find(":", last) + 2;
            
            // orderCount
            last = asks.find(":", last) + 1;
            next = asks.find("}", last);
            token = asks.substr(last, next-last);
            order.count = atoi(token.c_str());
            last = next + 1;
            
            ob.asks.push_back(order);
        }
        asks.erase();
        
        last = next = 0;
        while ((last = bids.find("{", last)) != std::string::npos) {
            Order order;
            
            // ignore "side":"..."
            last = bids.find(":", last) + 2;
            
            // volume
            last = bids.find(":", last) + 2;
            next = bids.find("\"", last);
            token = bids.substr(last, next-last);
            order.volume = atof(token.c_str());
            last = next + 1;
            
            // price
            last = bids.find(":", last) + 2;
            next = bids.find("\"", last);
            token = bids.substr(last, next-last);
            order.price = atof(token.c_str());
            last = next + 1;
            
            // ignore currencPair
            last = bids.find(":", last) + 2;
            
            // orderCount
            last = bids.find(":", last) + 1;
            next = bids.find("}", last);
            token = bids.substr(last, next-last);
            order.count = atoi(token.c_str());
            last = next + 1;
            
            ob.bids.push_back(order);
        }
        return ob;
    }


    // GET FULL ORDERBOOK
    //
    //
    OrderBook VALRClient::getFullOrderBook(std::string pair){
        std::string path = "/v1/public/" + pair + "/orderbook/full";
        std::string res = client.request("GET", (host+path).c_str(), false, VALR_EXCHANGE);
        
        int httpCode = client.getHttpCode();
        if (httpCode != 200)
            throw ResponseEx("Error " + std::to_string(httpCode) + " - " + res);
        
        OrderBook ob;
        size_t last = 0, next = 0;
        std::cout << res << std::endl;
        std::string token;
        
        // asks and bids
        last = res.find("Asks", last);
        last = res.find("[", last) + 1;
        next = res.find("]", last);
        std::string asks = res.substr(last, next-last);
        last = next + 1;
        
        last = res.find("Bids", last);
        last = res.find("[", last) + 1;
        next = res.find("]", last);
        std::string bids = res.substr(last, next-last);
        
        // timestamp
        last = res.find("LastChange", last);
        last = res.find(":", last) + 2;
        next = res.find("\"", last);
        token = res.substr(last, next-last);
        ob.timestamp = get_seconds_since_epoch(token);
        last = next + 1;
        
        res.erase();
        
        last = next = 0;
        while ((last = asks.find("{", last)) != std::string::npos) {
            Order order;
            
            // ignore "side":"..."
            last = asks.find(":", last) + 2;
            
            // volume
            last = asks.find(":", last) + 2;
            next = asks.find("\"", last);
            token = asks.substr(last, next-last);
            order.volume = atof(token.c_str());
            last = next + 1;
            
            // price
            last = asks.find(":", last) + 2;
            next = asks.find("\"", last);
            token = asks.substr(last, next-last);
            order.price = atof(token.c_str());
            last = next + 1;
            
            // ignore currencPair
            last = asks.find(":", last) + 2;
            
            // id
            last = asks.find(":", last) + 2;
            next = asks.find("\"", last);
            token = asks.substr(last, next-last);
            order.id = token;
            last = next + 1;
            
            // orderCount
            last = asks.find(":", last) + 1;
            next = asks.find("}", last);
            token = asks.substr(last, next-last);
            order.count = atoi(token.c_str());
            last = next + 1;
            
            ob.asks.push_back(order);
        }
        asks.erase();
        
        last = next = 0;
        while ((last = bids.find("{", last)) != std::string::npos) {
            Order order;
            
            // ignore "side":"..."
            last = bids.find(":", last) + 2;
            
            // volume
            last = bids.find(":", last) + 2;
            next = bids.find("\"", last);
            token = bids.substr(last, next-last);
            order.volume = atof(token.c_str());
            last = next + 1;
            
            // price
            last = bids.find(":", last) + 2;
            next = bids.find("\"", last);
            token = bids.substr(last, next-last);
            order.price = atof(token.c_str());
            last = next + 1;
            
            // ignore currencPair
            last = bids.find(":", last) + 2;
            
            // id
            last = bids.find(":", last) + 2;
            next = bids.find("\"", last);
            token = bids.substr(last, next-last);
            order.id = token;
            last = next + 1;
            
            // orderCount
            last = bids.find(":", last) + 1;
            next = bids.find("}", last);
            token = bids.substr(last, next-last);
            order.count = atoi(token.c_str());
            last = next + 1;
            
            ob.bids.push_back(order);
        }
        return ob;
    }


    // Get Currencies
    //
    //
    std::vector<CurrencyInfo> VALRClient::getCurrencies(){
        std::string path = "/v1/public/currencies";
        std::string res = client.request("GET", (host+path).c_str(), false, VALR_EXCHANGE);
        
        int httpCode = client.getHttpCode();
        if (httpCode != 200)
            throw ResponseEx("Error " + std::to_string(httpCode) + " - " + res);
        
        std::vector<CurrencyInfo> list;
        size_t last = 0, next = 0;
        
        std::string token;

        while ((last = res.find("{", last)) != std::string::npos) {
            CurrencyInfo info;
            
            // Symbol
            last = res.find(":", last) + 2;
            next = res.find("\"", last);
            token = res.substr(last, next-last);
            info.symbol = token;
            last = next + 1;
            
            // isActive
            last = res.find(":", last) + 1;
            next = res.find(",", last);
            token = res.substr(last, next-last);
            info.isActive = (token == "true") ? true : false;
            last = next + 1;
            
            // short Name
            last = res.find(":", last) + 2;
            next = res.find("\"", last);
            token = res.substr(last, next-last);
            info.shortName = token;
            last = next + 1;
            
            // long Name
            last = res.find(":", last) + 2;
            next = res.find("\"", last);
            token = res.substr(last, next-last);
            info.longName = token;
            last = next + 1;
            
            // decimalPlace
            last = res.find(":", last) + 2;
            next = res.find("\"", last);
            token = res.substr(last, next-last);
            info.decimalCount = atoi(token.c_str());
            last = next + 1;
            
            // withdrawalDecimalPlace
            last = res.find(":", last) + 2;
            next = res.find("\"", last);
            token = res.substr(last, next-last);
            info.withdrawalDecimalCount = atoi(token.c_str());
            last = next + 1;
            
            list.push_back(info);
        }
        res.erase();
        return list;
    }

    std::vector<CurrencyPairInfo> VALRClient::getCurrencyPairs(){
        std::string path = "/v1/public/pairs";
        std::string res = client.request("GET", (host+path).c_str(), false, VALR_EXCHANGE);
        
        int httpCode = client.getHttpCode();
        if (httpCode != 200)
            throw ResponseEx("Error " + std::to_string(httpCode) + " - " + res);
        
        std::vector<CurrencyPairInfo> list;
        size_t last = 0, next = 0;
        
        std::string token;

        while ((last = res.find("{", last)) != std::string::npos) {
            CurrencyPairInfo info;
            
            // Symbol
            last = res.find(":", last) + 2;
            next = res.find("\"", last);
            token = res.substr(last, next-last);
            info.symbol = token;
            last = next + 1;
            
            // Base currency
            last = res.find(":", last) + 2;
            next = res.find("\"", last);
            token = res.substr(last, next-last);
            info.baseCurrency = token;
            last = next + 1;
            
            // Quote currency
            last = res.find(":", last) + 2;
            next = res.find("\"", last);
            token = res.substr(last, next-last);
            info.quoteCurrency = token;
            last = next + 1;
            
            // Currency pair's short name
            last = res.find(":", last) + 2;
            next = res.find("\"", last);
            token = res.substr(last, next-last);
            info.shortName = token;
            last = next + 1;
            
            // isActive
            last = res.find(":", last) + 1;
            next = res.find(",", last);
            token = res.substr(last, next-last);
            info.isActive = (token == "true") ? true : false;
            last = next + 1;
            
            // min base amount
            last = res.find(":", last) + 2;
            next = res.find("\"", last);
            token = res.substr(last, next-last);
            info.baseMinTradable = atof(token.c_str());
            last = next + 1;
            
            // max base amount
            last = res.find(":", last) + 2;
            next = res.find("\"", last);
            token = res.substr(last, next-last);
            info.baseMaxTradable = atof(token.c_str());
            last = next + 1;
            
            // min quote amount
            last = res.find(":", last) + 2;
            next = res.find("\"", last);
            token = res.substr(last, next-last);
            info.quoteMinTradable = atof(token.c_str());
            last = next + 1;
            
            // max quote amount
            last = res.find(":", last) + 2;
            next = res.find("\"", last);
            token = res.substr(last, next-last);
            info.quoteMaxTradable = atof(token.c_str());
            last = next + 1;
            
            // Tick size
            last = res.find(":", last) + 2;
            next = res.find("\"", last);
            token = res.substr(last, next-last);
            info.tickSize = atoi(token.c_str());
            last = next + 1;
            
            // decimalPlace
            last = res.find(":", last) + 2;
            next = res.find("\"", last);
            token = res.substr(last, next-last);
            info.baseDecimalCount = atoi(token.c_str());
            last = next + 1;
            
            list.push_back(info);
        }
        res.erase();
        return list;
    }

    std::vector<OrderTypeInfo> VALRClient::getOrderTypes(std::string pair){
        std::string path = "/v1/public/";
        if (path.length() > 0)
            path += pair + "/";
        path += "ordertypes";
        std::string res = client.request("GET", (host+path).c_str(), false, VALR_EXCHANGE);
        
        int httpCode = client.getHttpCode();
        if (httpCode != 200)
            throw ResponseEx("Error " + std::to_string(httpCode) + " - " + res);
        
        std::vector<OrderTypeInfo> list;
        size_t last = 0, next = 0;
        
        std::string token;

        while ((last = res.find("{", last)) != std::string::npos) {
            OrderTypeInfo info;
            
            // Symbol
            last = res.find(":", last) + 2;
            next = res.find("\"", last);
            token = res.substr(last, next-last);
            info.symbol = token;
            last = next + 1;
            
            // Base currency
            last = res.find("[", last) + 1;
            next = res.find("]", last);
            token = res.substr(last, next-last);
            /*remove all the " characters*/
            token.erase(remove( token.begin(), token.end(), '\"' ),token.end());
            info.orderTypes = token;
            last = next + 1;
            
            list.push_back(info);
        }
        if (pair.length() > 0){
            OrderTypeInfo info;
            info.symbol = pair;
            
            last = 0;
            // Base currency
            last = res.find("[", last) + 1;
            next = res.find("]", last);
            token = res.substr(last, next-last);
            /*remove all the " characters*/
            token.erase(remove( token.begin(), token.end(), '\"' ),token.end());
            info.orderTypes = token;
            last = next + 1;
            
            list.push_back(info);
        }
            
        res.erase();
        return list;
    }

    // GET TICKERS
    //
    //
    std::vector<Ticker> VALRClient::getTickers(std::string pair){
        std::string path = "/v1/public/";
        if (path.length() > 0)
            path += pair + "/";
        path += "marketsummary";
        std::string res = client.request("GET", (host+path).c_str(), false, VALR_EXCHANGE);
        
        int httpCode = client.getHttpCode();
        if (httpCode != 200)
            throw ResponseEx("Error " + std::to_string(httpCode) + " - " + res);
        
        std::vector<Ticker> list;
        size_t last = 0, next = 0;
        
        std::string token;
        
        while ((last = res.find("{", last)) != std::string::npos) {
            list.push_back(Ticker());
        
            // pair
            last = res.find(":", last) + 2;
            next = res.find("\"", last);
            list.back().pair = res.substr(last, next-last);
            last = next + 1;
            
            // asks
            last = res.find("ask", last);
            last = res.find(":", last) + 2;
            next = res.find("\"", last);
            token = res.substr(last, next-last);
            list.back().ask = atof(token.c_str());
            last = next + 1;
            
            // bid
            last = res.find(":", last) + 2;
            next = res.find("\"", last);
            token = res.substr(last, next-last);
            list.back().bid = atof(token.c_str());
            last = next + 1;
            
            // last trade
            last = res.find(":", last) + 2;
            next = res.find("\"", last);
            token = res.substr(last, next-last);
            list.back().lastTrade = atof(token.c_str());
            last = next + 1;
            
            // previous close
            last = res.find(":", last) + 2;
            next = res.find("\"", last);
            token = res.substr(last, next-last);
            list.back().lastClosed = atof(token.c_str());
            last = next + 1;
            
            // Base volume
            last = res.find(":", last) + 2;
            next = res.find("\"", last);
            token = res.substr(last, next-last);
            list.back().baseVolume = atof(token.c_str());
            last = next + 1;
            
            // high price
            last = res.find(":", last) + 2;
            next = res.find("\"", last);
            token = res.substr(last, next-last);
            list.back().high = atof(token.c_str());
            last = next + 1;
            
            // low price
            last = res.find(":", last) + 2;
            next = res.find("\"", last);
            token = res.substr(last, next-last);
            list.back().low = atof(token.c_str());
            last = next + 1;
            
            
            // timestamp
            last = res.find(":", last) + 2;
            next = res.find("\"", last);
            std::string token = res.substr(last, next-last);
            list.back().timestamp = get_seconds_since_epoch(token);
            last = next + 1;
            
            
            // Changed from previous
            last = res.find(":", last) + 2;
            next = res.find("\"", last);
            token = res.substr(last, next-last);
            list.back().changeFromPrevious = atof(token.c_str());
            last = next + 1;
        }
        return list;
    }
                                                                        
    
    // GET TRADES
    //
    // Returns 100 trades only (cannot be changed), from a default of since the last 24 hours
    std::vector<Trade> VALR::VALRClient::getTrades(std::string pair, unsigned long long since, unsigned long long until, unsigned skip , unsigned limit, std::string beforeID ){
        std::string path = "/v1/public/" + pair + "/trades";
        int args = 0;
        if (since != 0){
            path += (args++ ? "&" : "?");
            path += "startTime=" + get_timestamp_iso8601_string(since);// convert to ISO...
        }
        if (until != 0){
            path += (args++ ? "&" : "?");
            path += "endTime=" + get_timestamp_iso8601_string(until); // convert to ISO...
        }
        if (skip != 0){
            path += (args++ ? "&" : "?");
            path += "skip=" + std::to_string(skip);
        }
        if (limit != 0){
            path += (args++ ? "&" : "?");
            path += "limit=" + std::to_string(limit);
        }
        if (beforeID != "") {
            path += (args++ ? "&" : "?");
            path += "beforeId=" + beforeID;
        }
        std::string res = client.request("GET", (host+path).c_str(), false, VALR_EXCHANGE);
        
        int httpCode = client.getHttpCode();
        if (httpCode != 200)
            throw ResponseEx("Error " + std::to_string(httpCode) + " - " + res);
        
        std::vector<Trade> trades;
        size_t last = 0, next = 0;
        last = res.find("[", last) + 1;
        
        while ((last = res.find("{", last)) != std::string::npos) {
            trades.push_back(Trade());
            // price
            last = res.find(":", last) + 2;
            next = res.find("\"", last);
            std::string token = res.substr(last, next-last);
            trades.back().price = atof(token.c_str());
            last = next + 1;
            
            // volume (quantity)
            last = res.find(":", last) + 2;
            next = res.find("\"", last);
            token = res.substr(last, next-last);
            trades.back().baseVolume = atof(token.c_str());
            last = next + 1;
            
            // pair
            last = res.find(":", last) + 2;
            next = res.find("\"", last);
            token = res.substr(last, next-last);
            trades.back().pair = token;
            last = next + 1;
            
            // timestamp
            last = res.find(":", last) + 2;
            next = res.find("\"", last);
            token = res.substr(last, next-last);
            trades.back().timestamp = get_seconds_since_epoch(token);
            last = next + 1;
            
            // isBuy
            last = res.find(":", last) + 2;
            next = res.find("\"", last);
            token = res.substr(last, next-last);
            trades.back().isBuy = (token == "buy") ? true : false;
            last = next + 1;
            
            // sequenceID
            last = res.find(":", last) + 1;
            next = res.find(",", last);
            token = res.substr(last, next-last);
            trades.back().sequence = atoll(token.c_str());
            last = next + 1;
            
            // id
            last = res.find(":", last) + 2;
            next = res.find("\"", last);
            token = res.substr(last, next-last);
            trades.back().id = token;
            last = next + 1;
            
            // qupte volume
            last = res.find(":", last) + 2;
            next = res.find("\"", last);
            token = res.substr(last, next-last);
            trades.back().quoteVolume = atof(token.c_str());
            last = next + 1;               
        }
        
        return trades;
    }
}
