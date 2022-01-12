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
        std::string token = extractNextString(res, last, ",", last, "timestamp");
        ob.timestamp = atoll(token.c_str());
        
        // asks and bids
        std::string asks = extractNextStringBlock(res, last, "[", "]", last, "asks");

        std::string bids = extractNextStringBlock(res, last, "[", "]", last, "bids");
        res.erase();
        
        last = 0;
        while ((last = asks.find("{", last)) != std::string::npos) {
            Order order;
            // price
            token =  extractNextString(asks, last, last, "price");
            order.price = atof(token.c_str());
            
            // volume
            token = extractNextString(asks, last, last, "volume");
            order.volume = atof(token.c_str());
            
            ob.asks.push_back(order);
        }
        asks.erase();
        
        last = 0;
        while ((last = bids.find("{", last)) != std::string::npos) {
            Order order;
            // price
            token = extractNextString(bids, last, last, "price");
            order.price = atof(token.c_str());
            
            // volume
            token = extractNextString(bids, last, last, "volume");
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
        std::string token = extractNextString(res, last, ",", last, "timestamp");
        ob.timestamp = atoll(token.c_str());
        
        // asks and bids
        std::string asks = extractNextStringBlock(res, last, "[", "]", last, "asks");
        std::string bids = extractNextStringBlock(res, last, "[", "]", last, "bids");
        res.erase();
        
        last = 0;
        while ((last = asks.find("{", last)) != std::string::npos) {
            Order order;
            
            // price
            token = extractNextString(asks, last, last, "price");
            order.price = atof(token.c_str());
            
            // volume
            token =  extractNextString(asks, last, last, "volume");
            order.volume = atof(token.c_str());
            
            ob.asks.push_back(order);
        }
        asks.erase();
        
        last = 0;
        while ((last = bids.find("{", last)) != std::string::npos) {
            Order order;
            // price
            token = extractNextString(bids, last, last, "price");
            order.price = atof(token.c_str());
            
            // volume
            token = extractNextString(bids, last, last, "volume");
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
        ticker.pair = extractNextString(res, last, last, "pair");
        
        // timestamp
        std::string token = extractNextString(res, last, ",", last, "timestamp");
        ticker.timestamp = atoll(token.c_str());
        
        // bid
        token = extractNextString(res, last, last, "bid");
        ticker.bid = atof(token.c_str());
        
        // asks
        token = extractNextString(res, last, last, "ask");
        ticker.ask = atof(token.c_str());
        
        // last trade
        token = extractNextString(res, last, last, "last_trade");
        ticker.lastTrade = atof(token.c_str());
        
        // rolling 24 hour volume
        token = extractNextString(res, last, last, "rolling_24_hour_volume");
        ticker.rollingVolume = atof(token.c_str());
        
        // status
        ticker.status = extractNextString(res, last, last, "status");
        
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
            tickers.back().pair = extractNextString(res, last, last, "pair");
            
            // timestamp
            std::string token = extractNextString(res, last, ",", last, "timestamp");
            tickers.back().timestamp = atoll(token.c_str());
            
            // bid
            token = extractNextString(res, last, last, "bid");
            tickers.back().bid = atof(token.c_str());
            
            // asks
            token = extractNextString(res, last, last, "ask");
            tickers.back().ask = atof(token.c_str());
            
            // last trade
            token = extractNextString(res, last, last, "last_trade");
            tickers.back().lastTrade = atof(token.c_str());
            
            // rolling 24 hour volume
            token = extractNextString(res, last, last, "rolling_24_hour_volume");
            tickers.back().rollingVolume = atof(token.c_str());
            
            // status
            tickers.back().status = extractNextString(res, last, last, "status");
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
        size_t last = res.find("[", 0);
        
        if (last == std::string::npos)
            return trades;
        
        while ((last = res.find("{", last)) != std::string::npos) {
            trades.push_back(Trade());
           
            // sequence
            std::string token = extractNextString(res, last, ",", last, "sequence");
            trades.back().sequence = atoll(token.c_str());
               
            // timestamp
            token = extractNextString(res, last, ",", last, "timestamp");
            trades.back().timestamp = atoll(token.c_str());
            
            // price
            token = extractNextString(res, last, last, "price");
            trades.back().price = atof(token.c_str());
               
            // volume
            token = extractNextString(res, last, last, "volume");
            trades.back().volume = atof(token.c_str());
               
            // isBuy
            token = extractNextString(res, last,"}" , last, "is_buy");
            trades.back().isBuy = (token == "true") ? true : false;
        }
        
        return trades;
    }

    // GET CANDLES
    //
    // Returns candlestick data from a specified time until now.
    // Note: parameters
    //          sinceTimestamp: expects time since unix in seconds
    //          duration      : expects integer in seconds.
    //                          60 (1m), 300 (5m), 900 (15m), 1800 (30m), 3600 (1h),
    //                          10800 (3h), 14400 (4h), 28800 (8h), 86400 (24h),
    //                          259200 (3d), 604800 (7d).
    CandleData LunoClient::getCandles(std::string pair, unsigned long long sinceTimestamp, int duration ){
        std::string uri = "https://api.mybitx.com/api/exchange/1/candles?pair="
                             + pair
                            + "&since=" + std::to_string(sinceTimestamp)
                            + "&duration=" + std::to_string(duration);
        
        std::string res = client.request("GET", uri.c_str());
        
        int httpCode = client.getHttpCode();
        if (httpCode != 200)
            throw ResponseEx("Error " + std::to_string(httpCode) + " - " + res);
        
        CandleData data;
        size_t last = 0;

        // pair
        data.pair = extractNextString(res, last, last, "pair");
        
        // duration
        std::string token = extractNextString(res, last, ",", last, "duration");
        data.duration = atoi(token.c_str());
        
        // candles
        std::string candles = extractNextStringBlock(res, last, "[", "]", last, "candles");
        last = 0;
        
        while ((last = candles.find("{", last)) != std::string::npos) {
            Candle candle;
           
            // timestamp
            token = extractNextString(candles, last, ",", last, "timestamp");
            candle.timestamp = atoll(token.c_str());
            
            // close
            token = extractNextString(candles, last, last, "close");
            candle.close = atof(token.c_str());
            
            // high
            token = extractNextString(candles, last, last, "high");
            candle.high = atof(token.c_str());
            
            // low
            token = extractNextString(candles, last, last, "low");
            candle.low = atof(token.c_str());
            
            // open
            token = extractNextString(candles, last, last, "open");
            candle.open = atof(token.c_str());
            
            // volume
            token = extractNextString(candles, last, last, "volume");
            candle.volume = atof(token.c_str());
            
            data.candles.push_back(candle);
        }
        
        return data;
    }


    // GET MARKET INFO
    //
    // Returns supported market information like price scale, min and max order volumes and market ID.
    std::vector<CurrencyPairInfo> LunoClient::getMarketInfo(){
        std::string uri = "https://api.mybitx.com/api/exchange/1/markets";
    
        std::string res = client.request("GET", uri.c_str());
        
        int httpCode = client.getHttpCode();
        if (httpCode != 200)
            throw ResponseEx("Error " + std::to_string(httpCode) + " - " + res);
        
        std::vector<CurrencyPairInfo> data;
        size_t last = 0;

        // markets
        res = extractNextStringBlock(res, last, "[", "]", "markets");
        std::string token;
        
        while ((last = res.find("{", last)) != std::string::npos && last < res.size()) {
            CurrencyPairInfo info;
            // market_id
            info.symbol = extractNextString(res, last, last, "market_id");
            
            // trading_status
            info.tradeStatus = extractNextString(res, last, last, "trading_status");
            
            // base_currency
            info.baseCurrency = extractNextString(res, last, last, "base_currency");
            
            // counter_currency
            info.quoteCurrency = extractNextString(res, last, last, "counter_currency");
            
            // min_volume
            token = extractNextString(res, last, last, "min_price");
            info.baseMin = atof(token.c_str());
            
            // max_volume
            token = extractNextString(res, last, last, "max_volume");
            info.baseMax = atof(token.c_str());
            
            // volume_scale
            token = extractNextString(res, last, ",", last, "volume_scale");
            info.baseDecimalCount = atoi(token.c_str());
            
            // min_price
            token = extractNextString(res, last, last, "min_price");
            info.quoteMin = atof(token.c_str());
                      
            // max_price
            token = extractNextString(res, last, last, "max_price");
            info.quoteMax = atof(token.c_str());
            
            // price_scale
            token = extractNextString(res, last, ",", last, "price_scale");
            info.quoteDecimalCount = atoi(token.c_str());
            
            // fee_scale
            token = extractNextString(res, last, "}", last, "fee_scale");
            info.feeDecimalCount = atoi(token.c_str());

            
            data.push_back(info);
        }
        
        return data;
    }

}


