#include "lunoclient.hpp"

extern Client client;

namespace Luno {

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
        last = res.find("thirty_day_volume", last);
        std::string token = extractNextString(res, last, last);
        fee.thirtyDayVolume = atof(token.c_str());
        
        // maker fee
        last = res.find("maker_fee", last);
        token = extractNextString(res, last, last);
        fee.maker = atof(token.c_str());
        
        // taker fee
        last = res.find("taker_fee", last);
        token = extractNextString(res, last, last);
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
            last = res.find("order_id", last);
            orders.back().orderID = extractNextString(res, last, last);
            
            // Creation Timestamp
            last = res.find("creation_", last);
            std::string token = extractNextString(res, last, ",", last);
            orders.back().createdTime = atoll(token.c_str());
            
            // Expiration Timestamp
            last = res.find("expiration_", last);
            token = extractNextString(res, last, ",", last);
            orders.back().expirationTime = atoll(token.c_str());
            
            // Completion Timestamp
            last = res.find("completed_", last);
            token = extractNextString(res, last, ",", last);
            orders.back().completedTime = atoll(token.c_str());
            
            // Type
            last = res.find("type", last);
            orders.back().type = extractNextString(res, last, last);
            
            // State
            last = res.find("state", last);
            orders.back().state = extractNextString(res, last, last);
            
            // Price
            last = res.find("limit_price", last);
            token = extractNextString(res, last, last);
            orders.back().price = atof(token.c_str());
            
            // Volume
            last = res.find("limit_volume", last);
            token = extractNextString(res, last, last);
            orders.back().volume = atof(token.c_str());
            
            // Base
            last = res.find("base", last);
            token = extractNextString(res, last, last);
            orders.back().baseValue = atof(token.c_str());
            
            // Counter
            last = res.find("counter", last);
            token = extractNextString(res, last, last);
            orders.back().counterValue = atof(token.c_str());
            
            // Base Fee
            last = res.find("fee_base", last);
            token = extractNextString(res, last, last);
            orders.back().baseFee = atof(token.c_str());
            
            // Counter Fee
            last = res.find("fee_counter", last);
            token = extractNextString(res, last, last);
            orders.back().counterFee = atof(token.c_str());
            
            // Pair
            last = res.find("pair", last);
            orders.back().pair = extractNextString(res, last, last);
            
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
               last = res.find("pair", last);
               trades.back().pair = extractNextString(res, last, last);
               
               // sequence
               last = res.find("sequ", last);
               std::string token = extractNextString(res, last, ",", last);
               trades.back().sequence = atoll(token.c_str());
               
               // Order ID
               last = res.find("order_id", last);
               trades.back().orderID = extractNextString(res, last, last);
               
               // Type
               last = res.find("type", last);
               trades.back().type = extractNextString(res, last, last);
               
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
               
               // base
               last = res.find("base", last);
               token = extractNextString(res, last, last);
               trades.back().baseValue = atof(token.c_str());
               
               // counter
               last = res.find("counter", last);
               token = extractNextString(res, last, last);
               trades.back().counterValue = atof(token.c_str());
               
               // base fee
               last = res.find("fee_base", last);
               token = extractNextString(res, last, last);
               trades.back().baseFee = atof(token.c_str());
               
               // counter fee
               last = res.find("fee_counter", last);
               token = extractNextString(res, last, last);
               trades.back().counterFee = atof(token.c_str());
               
               // is buy
               last = res.find("is_buy", last);
               token = extractNextString(res, last, "}", last);
               trades.back().isBuy = (token == "true") ? true : false;
           }
        return trades;
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
        
        return res;
    }

    // STOP ORDER
    //
    //
    std::string LunoClient::stopOrder(std::string orderId){
        std::string uri = "https://api.mybitx.com/api/1/stoporder?order_id=" + orderId;
        
        std::string res = client.request("POST", uri.c_str(), true);
        
        int httpCode = client.getHttpCode();
        if (httpCode != 200)
            throw ResponseEx("Error " + std::to_string(httpCode) + " - " + res);
        
        return res;
    }
}
