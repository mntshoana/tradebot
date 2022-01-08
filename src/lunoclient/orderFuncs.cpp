#include "lunoclient.hpp"

extern Client client;

namespace Luno {
    std::vector<CurrencyPairInfo> currencies;

    // GET FEE INFO
    //
    //
    Fee LunoClient::getFeeInfo(std::string pair){
        std::string uri = "https://api.mybitx.com/api/1/fee_info?pair=" + pair;
        std::string res = client.request("GET", uri.c_str(), true);
        
        int httpCode = client.getHttpCode();
        if (httpCode != 200)
            throw ResponseEx("Error " + std::to_string(httpCode) + " - " + res);
        
        Fee fee;
        size_t last = 0;
        
        // 30 day volume
        std::string token = extractNextString(res, last, last, "thirty_day_volume");
        fee.thirtyDayVolume = atof(token.c_str());
        
        // maker fee
        token = extractNextString(res, last, last, "maker_fee");
        fee.maker = atof(token.c_str());
        
        // taker fee
        token = extractNextString(res, last, last, "taker_fee");
        fee.taker = atof(token.c_str());
        
        return fee;
    }


    // GET USER ORDERS
    //
    // state == PENDING or COMPLETE
    // limit == 100 (default) -- number of orders returned
    // before -- filter orders created before (not on) this timestamp
    std::vector<UserOrder> LunoClient::getUserOrders(std::string pair,
                                         std::string state,
                                         int limit,
                                         long long before ){
        int args = 0;
        std::string uri = "https://api.mybitx.com/api/1/listorders";
        if (pair != ""){
            uri += (args++ ? "&" : "?");
            uri += "pair=" + pair;
        }
        if (state != ""){
            uri += (args++ ? "&" : "?");
            uri += "state=" + state;
        }
        if (limit != 0){
            uri += (args++ ? "&" : "?");
            uri += "limit=" + std::to_string(limit);
        }
        if (before != 0){
            uri += (args++ ? "&" : "?");
            uri += "created_before=" + std::to_string(before);
        }
        
        std::string res = client.request("GET", uri.c_str(), true);
        
        int httpCode = client.getHttpCode();
        if (httpCode != 200)
            throw ResponseEx("Error " + std::to_string(httpCode) + " - " + res);
        
        std::vector<UserOrder> orders;
        size_t last = 0;
        last = res.find("[", last) + 1;
        
        while ((last = res.find("{", last)) != std::string::npos) {
            orders.push_back(UserOrder());
     
            // Order ID
            orders.back().orderID = extractNextString(res, last, last, "order_id");
            
            // Creation Timestamp
            std::string token = extractNextString(res, last, ",", last, "creation_timestamp");
            orders.back().createdTime = atoll(token.c_str());
            
            // Expiration Timestamp
            token = extractNextString(res, last, ",", last, "expiration_timestamp");
            orders.back().expirationTime = atoll(token.c_str());
            
            // Completion Timestamp
            last = res.find("completed_", last);
            token = extractNextString(res, last, ",", last, "completed_timestamp");
            orders.back().completedTime = atoll(token.c_str());
            
            // Type
            orders.back().type = extractNextString(res, last, last, "type");
            
            // State
            orders.back().state = extractNextString(res, last, last, "state");
            
            // Price
            token = extractNextString(res, last, last, "limit_price");
            orders.back().price = atof(token.c_str());
            
            // Volume
            token = extractNextString(res, last, last, "limit_volume");
            orders.back().volume = atof(token.c_str());
            
            // Base
            token = extractNextString(res, last, last, "base");
            orders.back().baseValue = atof(token.c_str());
            
            // Counter
            token = extractNextString(res, last, last, "counter");
            orders.back().counterValue = atof(token.c_str());
            
            // Base Fee
            token = extractNextString(res, last, last, "fee_base");
            orders.back().baseFee = atof(token.c_str());
            
            // Counter Fee
            token = extractNextString(res, last, last, "fee_counter");
            orders.back().counterFee = atof(token.c_str());
            
            // Pair
            orders.back().pair = extractNextString(res, last, last, "pair");
            
            if (orders.back().state != "PENDING" && orders.back().baseValue == 0.0f
                && orders.back().counterValue == 0.0f)
                orders.pop_back();
        }
        return orders;
    }

    // GET USER TRADES
    //
    // since -- Filter to trades on or after this timestamp.
    // before -- Filter to trades before this timestamp.
    // after_seq  -- Filter to trades from (including) this sequence number
    // before_seq  -- Filter to trades from (excluding) this sequence number
    // sort -- descending order (default == false == ascending order)
    // limit -- Limits to this number of trades (default 100)
    std::vector<UserTrade> LunoClient::getUserTrades(std::string pair,
                                                 long long since,
                                                 long long before,
                                                 long long after_seq,
                                                 long long before_seq,
                                                 bool sort,
                                                 int limit){
        std::string uri = "https://api.mybitx.com/api/1/listtrades?pair=" + pair;
        if (since != 0)
            uri += "&since=" + std::to_string(since);
        if (before != 0)
            uri += "&before=" + std::to_string(before);
        if (after_seq != 0)
            uri += "&after_seq=" + std::to_string(after_seq);
        if (before_seq != 0)
            uri += "&before_seq=" + std::to_string(before_seq);
        if (sort)
            uri += "&sort=true";
        if (limit != 0)
            uri += "&limit=" + std::to_string(limit);
        std::string res = client.request("GET", uri.c_str(), true);
        
        int httpCode = client.getHttpCode();
        if (httpCode != 200)
            throw ResponseEx("Error " + std::to_string(httpCode) + " - " + res);
        
        std::vector<UserTrade> trades;
           size_t last = 0;
           last = res.find("[", last) + 1;
           
           while ((last = res.find("{", last)) != std::string::npos) {
               trades.push_back(UserTrade());
        
               // pair
               trades.back().pair = extractNextString(res, last, last, "pair");
               
               // sequence
               std::string token = extractNextString(res, last, ",", last, "sequence");
               trades.back().sequence = atoll(token.c_str());
               
               // Order ID
               trades.back().orderID = extractNextString(res, last, last, "order_id");
               
               // Type
               trades.back().type = extractNextString(res, last, last, "type");
               
               // timestamp
               token = extractNextString(res, last, ",", last, "timestamp");
               trades.back().timestamp = atoll(token.c_str());
               
               // price
               token = extractNextString(res, last, last, "price");
               trades.back().price = atof(token.c_str());
               
               // volume
               token = extractNextString(res, last, last, "volume");
               trades.back().volume = atof(token.c_str());
               
               // base
               token = extractNextString(res, last, last, "base");
               trades.back().baseValue = atof(token.c_str());
               
               // counter
               token = extractNextString(res, last, last, "counter");
               trades.back().counterValue = atof(token.c_str());
               
               // base fee
               token = extractNextString(res, last, last, "fee_base");
               trades.back().baseFee = atof(token.c_str());
               
               // counter fee
               token = extractNextString(res, last, last, "fee_counter");
               trades.back().counterFee = atof(token.c_str());
               
               // is buy
               token = extractNextString(res, last, "}", last, "is_buy");
               trades.back().isBuy = (token == "true") ? true : false;
           }
        return trades;
    }

    // POST MARKET ORDER
    //
    //
    std::string LunoClient::postMarketOrder(std::string pair, std::string action, float amount){
        std::string uri = "https://api.mybitx.com/api/1/marketorder";
    
        if (!(action == "BID" || action == "ASK")) // BUY or SELL (using bid ask for consistency)
            throw std::invalid_argument("'action' expects string value \"BID\" or \"ASK\" only! ");

        
        bool isOfBaseCurrency = (action == "ASK") ? true : false;
        std::string amountLabel = (isOfBaseCurrency ? "base_volume" : "counter_volume");
        
        std::string baseCurrency = pair.substr(0, 3);
        std::string quoteCurrency = pair.substr(3, 3);
        int decimals;
        
        if (currencies.size() == 0)
            currencies = getMarketInfo();
        for (const CurrencyPairInfo& currency : currencies) {
            if (isOfBaseCurrency && currency.baseCurrency == baseCurrency)
                decimals = currency.baseDecimalCount;
            if ( (!isOfBaseCurrency) && currency.quoteCurrency == quoteCurrency)
                decimals = currency.quoteDecimalCount;
        }
        
        // note: takers fee will be subtracted
        std::ostringstream strAmount;
        strAmount.precision(decimals);
        strAmount << std::fixed << amount;
        
        // base_account_id = default (no need to add it)
        // counter_account_id = default (no need to add it)
        // timestamp (needed only if using ttl)
        // ttl - time to live in miliseconds (no need to add it)
        // client_order_id (no need to add it)
        uri += "?pair=" + pair;
        
        uri += "&type=";
        uri += ((action == "BID") ? "BUY" : "SELL");
        
        uri += "&" + amountLabel+ "=";
        uri += strAmount.str();
        
        
        std::string res = client.request("POST", uri.c_str(), true);
        
        int httpCode = client.getHttpCode();
        if (httpCode != 200)
            throw ResponseEx("Error " + std::to_string(httpCode) + " - " + res);
        
        // order_id
        std::string orderID = extractNextString(res, 0, "order_id");
        return orderID;
    }

    // GET ORDER DETAILS
    //
    //
    std::string LunoClient::getOrderDetails(std::string id){
        std::string uri = "https://api.mybitx.com/api/1/orders/" + id;
        std::string res = client.request("GET", uri.c_str(), true);
        
        int httpCode = client.getHttpCode();
        if (httpCode != 200)
            throw ResponseEx("Error " + std::to_string(httpCode) + " - " + res);
        
        return res;
    }
    // POST LIMIT ORDER
    //
    //
    std::string LunoClient::postLimitOrder(std::string pair, std::string action, float volume, float price){
        std::string uri = "https://api.mybitx.com/api/1/postorder?pair=" + pair;
        uri += "&type=" + action; // BID or ASK
        uri += "&post_only=true";
        
        volume = std::trunc(1000 * 1000 * volume) / (1000 * 1000);
        uri += "&volume=" + std::to_string(volume);
        uri += "&price=" + std::to_string(price);
        
        std::string res = client.request("POST", uri.c_str(), true);
        
        int httpCode = client.getHttpCode();
        if (httpCode != 200)
            throw ResponseEx("Error " + std::to_string(httpCode) + " - " + res);
        
        // order_id
        std::string orderID = extractNextString(res, 0, "order_id");
        return orderID;
    }

    // STOP ORDER
    //
    //
    bool LunoClient::cancelOrder(std::string orderId){
        std::string uri = "https://api.mybitx.com/api/1/stoporder?order_id=" + orderId;
        
        std::string res = client.request("POST", uri.c_str(), true);
        
        int httpCode = client.getHttpCode();
        if (httpCode != 200)
            throw ResponseEx("Error " + std::to_string(httpCode) + " - " + res);
        
        // success
        std::string token = extractNextString(res, 0, "}", "success");
        return (token == "true" ? true : false);
    }
}
