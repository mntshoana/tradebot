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
        ob.timestamp = token;
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

/*
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

    std::string Ticker::toString(){
        std::stringstream ss;
        ss << "Pair: " << pair << "\n";
        ss << "Timestamp: " << timestamp << "\n";
        ss << "Bid: " << bid << "\n";
        ss << "Ask: " << ask << "\n";
        ss << "Last trade: " << lastTrade << "\n";
        ss << "Rolling 24 hour volume: " << rollingVolume << "\n";
        ss << "Status: " << status << "\n";
        return ss.str();
    }
    template <class T> T& operator << (T& stream, Ticker& ticker) {
        
        stream.append(ticker.toString().c_str());
        return stream;
    }
    template QTextEdit& operator << <QTextEdit>(QTextEdit& stream, Ticker& ticker);

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
                                                                        
    template <class T> T& operator << (T& stream,  std::vector<Ticker>& tickers) {
        for (Ticker& ticker : tickers){
            stream << ticker;
        }
        return stream;
    }
    template QTextEdit& operator << <QTextEdit>(QTextEdit& stream, std::vector<Ticker>& tickers);

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
    std::fstream& operator << (std::fstream& stream, std::vector<Trade>& trades){
        for (Trade& trade : trades)
            stream << trade.sequence << ", "
            << trade.timestamp << ", "
            << trade.price << ", "
            << trade.volume << ", "
            << trade.isBuy
               << "\n";
        return stream;
    }
    std::fstream& operator >> (std::fstream& stream, std::vector<Trade>& trades){
        std::string line, token;
        size_t index = 0;
        while (getline(stream, line)) {
            if (Client::abort)
                return stream;
            index = 0;
            std::stringstream s(line);
            while (getline(s, token, ',')) {
                if (index == 0){
                    trades.push_back(Trade());
                    trades.back().sequence = atoll(token.c_str());
                }
                if (index == 1)
                    trades.back().timestamp = atoll(token.c_str());
                if (index == 2)
                    trades.back().price = atof(token.c_str());
                if (index == 3)
                    trades.back().volume = atof(token.c_str());
                if (index == 4)
                    trades.back().isBuy = atoi(token.c_str());
                index++;
            }
        }
        return stream;
    }*/
}
