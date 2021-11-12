#include "valrclient.hpp"
#include <iostream>
#include <sstream>
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
        size_t last = 0;
        
        // erase spaces
        res.erase(remove( res.begin(), res.end(), ' ' ),res.end());
        
        // asks and bids
        last = res.find("Asks", last);
        std::string asks = extractNextStringBlock(res, last, "[", "]", last);
        
        last = res.find("Bids", last);
        std::string bids = extractNextStringBlock(res, last, "[", "]", last);
        
        last = 0;
        std::string token;
        while ((last = asks.find("{", last)) != std::string::npos) {
            Order order;
            
            // ignore "side":"..."
            last = asks.find(":", last) + 2;
            
            // volume
            token = extractNextString(asks, last, last);
            order.volume = atof(token.c_str());
            
            // price
            token = extractNextString(asks, last, last);
            order.price = atof(token.c_str());
            
            // ignore currencPair
            last = asks.find(":", last) + 2;
            
            // orderCount
            token = extractNextString(asks, last, "}", last);
            order.count = atoi(token.c_str());
            
            ob.asks.push_back(order);
        }
        asks.erase();
        
        last = 0;
        while ((last = bids.find("{", last)) != std::string::npos) {
            Order order;
            
            // ignore "side":"..."
            last = bids.find(":", last) + 2;
            
            // volume
            token = extractNextString(bids, last, last);
            order.volume = atof(token.c_str());
            
            // price
            token = extractNextString(bids, last, last);
            order.price = atof(token.c_str());
            
            // ignore currencPair
            last = bids.find(":", last) + 2;
            
            // orderCount
            token = extractNextString(bids, last, "}", last);
            order.count = atoi(token.c_str());
            
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
        // erase spaces
        res.erase(remove( res.begin(), res.end(), ' ' ),res.end());
        
        std::string token;
        
        // asks and bids
        last = res.find("Asks", last);
        std::string asks = extractNextStringBlock(res, last, "[", "]", last);
        
        last = res.find("Bids", last);
        std::string bids = extractNextStringBlock(res, last, "[", "]", last);
        
        // timestamp
        last = res.find("LastChange", last);
        token = extractNextString(res, last, last);
        ob.timestamp = get_seconds_since_epoch(token);
        
        res.erase();
        
        last = 0;
        while ((last = asks.find("{", last)) != std::string::npos) {
            Order order;
            
            // ignore "side":"..."
            last = asks.find(":", last) + 2;
            
            // volume
            token = extractNextString(asks, last, last);
            order.volume = atof(token.c_str());
            
            // price
            token = extractNextString(asks, last, last);
            order.price = atof(token.c_str());
            
            // ignore currencPair
            last = asks.find(":", last) + 2;
            
            // id
            order.id = extractNextString(asks, last, last);
            
            // orderCount
            token = extractNextString(asks, last, "}", last);
            order.count = atoi(token.c_str());
            
            ob.asks.push_back(order);
        }
        asks.erase();
        
        last = 0;
        while ((last = bids.find("{", last)) != std::string::npos) {
            Order order;
            
            // ignore "side":"..."
            last = bids.find(":", last) + 2;
            
            // volume
            token = extractNextString(bids, last, last);
            order.volume = atof(token.c_str());
            
            // price
            token = extractNextString(bids, last, last);
            order.price = atof(token.c_str());
            
            // ignore currencPair
            last = bids.find(":", last) + 2;
            
            // id
            order.id = extractNextString(bids, last, last);
            
            // orderCount
            token = extractNextString(bids, last, "}", last);
            order.count = atoi(token.c_str());
            
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
        size_t last = 0;
        
        std::string token;

        // erase spaces
        res.erase(remove( res.begin(), res.end(), ' ' ),res.end());
        
        while ((last = res.find("{", last)) != std::string::npos) {
            CurrencyInfo info;
            
            // Symbol
            info.symbol = extractNextString(res, last, last);
            
            // isActive
            token = extractNextString(res, last, ",", last);
            info.isActive = (token == "true") ? true : false;
            
            // short Name
            info.shortName = extractNextString(res, last, last);
            
            // long Name
            info.longName = extractNextString(res, last, last);
            
            // decimalPlace
            token = extractNextString(res, last, last);
            info.decimalCount = atoi(token.c_str());
            
            // withdrawalDecimalPlace
            token = extractNextString(res, last, last);;
            info.withdrawalDecimalCount = atoi(token.c_str());
            
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
        size_t last = 0;
        
        std::string token;

        // erase spaces
        res.erase(remove( res.begin(), res.end(), ' ' ),res.end());
        
        while ((last = res.find("{", last)) != std::string::npos) {
            CurrencyPairInfo info;
            
            // Symbol
            info.symbol = extractNextString(res, last, last);
            
            // Base currency
            info.baseCurrency = extractNextString(res, last, last);
            
            // Quote currency
            info.quoteCurrency = extractNextString(res, last, last);
            
            // Currency pair's short name
            info.shortName = extractNextString(res, last, last);
            
            // isActive
            token = extractNextString(res, last, ",", last);
            info.isActive = (token == "true") ? true : false;
            
            // min base amount
            token = extractNextString(res, last, last);
            info.baseMinTradable = atof(token.c_str());
            
            // max base amount
            
            token = extractNextString(res, last, last);
            info.baseMaxTradable = atof(token.c_str());
            
            // min quote amount
            token = extractNextString(res, last, last);
            info.quoteMinTradable = atof(token.c_str());
            
            // max quote amount
            token = extractNextString(res, last, last);
            info.quoteMaxTradable = atof(token.c_str());
            
            // Tick size
            token = extractNextString(res, last, last);
            info.tickSize = atoi(token.c_str());
            
            // decimalPlace
            token = extractNextString(res, last, last);
            info.baseDecimalCount = atoi(token.c_str());
            
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
        size_t last = 0;
        
        std::string token;

        // erase spaces
        res.erase(remove( res.begin(), res.end(), ' ' ),res.end());
        
        while ((last = res.find("{", last)) != std::string::npos) {
            OrderTypeInfo info;
            // Symbol
            info.symbol = extractNextString(res, last, last);
            // Base currency
            token = extractNextStringBlock(res, last, "[", "]", last);
            /*remove all the " characters*/
            token.erase(remove( token.begin(), token.end(), '\"' ),token.end());
            info.orderTypes = token;
            
            list.push_back(info);
        }
        last = 0;
        if (pair.length() > 0){
            OrderTypeInfo info;
            // Symbol
            info.symbol = pair;
            // Base currency
            token = extractNextStringBlock(res, last, "[", "]", last);
            /*remove all the " characters*/
            token.erase(remove( token.begin(), token.end(), '\"' ),token.end());
            info.orderTypes = token;
            
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
        size_t last = 0;
        
        std::string token;
        // erase spaces
        res.erase(remove( res.begin(), res.end(), ' ' ),res.end());
        
        while ((last = res.find("{", last)) != std::string::npos) {
            list.push_back(Ticker());
        
            // pair
            list.back().pair = extractNextString(res, last, last);
            
            // asks
            last = res.find("ask", last);
            token = extractNextString(res, last, last);
            list.back().ask = atof(token.c_str());
            
            // bid
            token = extractNextString(res, last, last);
            list.back().bid = atof(token.c_str());
            
            // last trade
            token = extractNextString(res, last, last);
            list.back().lastTrade = atof(token.c_str());
            
            // previous close
            token = extractNextString(res, last, last);
            list.back().lastClosed = atof(token.c_str());
            
            // Base volume
            token = extractNextString(res, last, last);
            list.back().baseVolume = atof(token.c_str());
            
            // high price
            token = extractNextString(res, last, last);
            list.back().high = atof(token.c_str());
            
            // low price
            token = extractNextString(res, last, last);
            list.back().low = atof(token.c_str());
            
            // timestamp
            std::string token = extractNextString(res, last, last);
            list.back().timestamp = get_seconds_since_epoch(token);
            
            // Changed from previous
            token = extractNextString(res, last, last);
            list.back().changeFromPrevious = atof(token.c_str());
        }
        return list;
    }
                                                                        
    
    // GET TRADES
    //
    // Returns 100 trades only (cannot be changed), from a default of since the last 24 hours
    std::vector<Trade> VALR::VALRClient::getTrades(std::string pair, unsigned long long since, unsigned long long until, unsigned skip, unsigned limit, std::string beforeID ){
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
        size_t last = 0;
        last = res.find("[", last) + 1;
        
        // erase spaces
        res.erase(remove( res.begin(), res.end(), ' ' ),res.end());
        
        while ((last = res.find("{", last)) != std::string::npos) {
            trades.push_back(Trade());
            // price
            std::string token = extractNextString(res, last, last);
            trades.back().price = atof(token.c_str());
            
            // volume (quantity)
            token = extractNextString(res, last, last);
            trades.back().baseVolume = atof(token.c_str());
            
            // pair
            token = extractNextString(res, last, last);
            trades.back().pair = token;
            
            // timestamp
            token = extractNextString(res, last, last);
            trades.back().timestamp = get_seconds_since_epoch(token);
            
            // isBuy
            token = extractNextString(res, last, last);
            trades.back().isBuy = (token == "buy") ? true : false;
            
            // sequenceID
            token = extractNextString(res, last, ",", last);
            trades.back().sequence = atoll(token.c_str());
            
            // id
            trades.back().id = extractNextString(res, last, last);;
            
            // qupte volume
            token = extractNextString(res, last, last);
            trades.back().quoteVolume = atof(token.c_str());
        }
        
        return trades;
    }

    std::string VALR::VALRClient::getServerTime(){
        std::string path = "/v1/public/time";
        std::string res = client.request("GET", (host+path).c_str(), false, VALR_EXCHANGE);
        
        int httpCode = client.getHttpCode();
        if (httpCode != 200)
            throw ResponseEx("Error " + std::to_string(httpCode) + " - " + res);
          
        // erase spaces
        res.erase(remove( res.begin(), res.end(), ' ' ),res.end());
        
        std::stringstream ss;
        size_t last = 0, next = 0;
        
        last = res.find(":", last) + 1;
        next = res.find(",", last);
        ss << "epoch: " <<  res.substr(last, next-last) << std::endl;
        last = next + 1;

        last = res.find(":", last) + 2;
        next = res.find("\"", last);
        ss << "string: "
            << res.substr(last, next-last)
            << std::endl;

        last = next + 1;
        
        return ss.str();
    }

    // "online" when all functionality is available
    // "read-only" when only GET and OPTIONS requests are accepted.
    std::string VALR::VALRClient::getServerStatus(){
        std::string path = "/v1/public/status";
        std::string res = client.request("GET", (host+path).c_str(), false, VALR_EXCHANGE);
        
        int httpCode = client.getHttpCode();
        if (httpCode != 200)
            throw ResponseEx("Error " + std::to_string(httpCode) + " - " + res);
            
        // erase spaces
        res.erase(remove( res.begin(), res.end(), ' ' ),res.end());

        std::stringstream ss;
        size_t last = 0, next = 0;
    
        last = res.find(":") + 2;
        next = res.find("\"", last); //  json format unneccessary
        std::string token = res.substr(last, next-last);
        ss << "status: " << token
            << std::endl;

        
        return ss.str();
    }
}
