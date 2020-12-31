#include "lunoclient.hpp"
namespace Luno {
    // GET FEE INFO
    //
    //
    Fee LunoClient::getFeeInfo(std::string pair){
        std::string uri = "https://api.mybitx.com/api/1/fee_info?pair=" + pair;
        std::string res = this->request("GET", uri.c_str(), true);
        
        if (httpCode != 200)
            throw ResponseEx("Error " + std::to_string(httpCode) + " - " + res);
        
        Fee fee;
        size_t last = 0, next = 0;
        
        // 30 day volume
        last = res.find("thirty_day_volume", last);
        last = res.find(":", last) + 2;
        next = res.find("\"", last);
        std::string token = res.substr(last, next-last);
        fee.thirtyDayVolume = atof(token.c_str());
        last = next + 1;
        
        // maker fee
        last = res.find("maker_fee", last);
        last = res.find(":", last) + 2;
        next = res.find("\"", last);
        token = res.substr(last, next-last);
        fee.maker = atof(token.c_str());
        last = next + 1;
        
        // taker fee
        last = res.find("taker_fee", last);
        last = res.find(":", last) + 2;
        next = res.find("\"", last);
        token = res.substr(last, next-last);
        fee.taker = atof(token.c_str());
        last = next + 1;
        return fee;
    }

    template <class T> T& operator << (T& stream, Fee& fee) {
        std::stringstream ss;
        ss << "30 day volume: " << fee.thirtyDayVolume << "\n";
        ss << "Maker fee: " << fee.maker << "\n";
        ss << "Taker fee: "  << fee.taker << "\n";

        stream.append(ss.str().c_str());
        return stream;
    }
    template QTextEdit& operator << <QTextEdit>(QTextEdit&, Fee& fee);

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
        
        std::string res = this->request("GET", uri.c_str(), true);
        
        if (httpCode != 200)
            throw ResponseEx("Error " + std::to_string(httpCode) + " - " + res);
        
        std::vector<UserOrder> orders;
        size_t last = 0, next = 0;
        last = res.find("[", last) + 1;
        
        while ((last = res.find("{", last)) != std::string::npos) {
            orders.push_back(UserOrder());
     
            // Order ID
            last = res.find("order_id", last);
            last = res.find(":", last) + 2;
            next = res.find("\"", last);
            orders.back().orderID = res.substr(last, next-last);
            last = next + 1;
            
            // Creation Timestamp
            last = res.find("creation_", last);
            last = res.find(":", last) + 1;
            next = res.find(",", last);
            std::string token = res.substr(last, next-last);
            orders.back().createdTime = atoll(token.c_str());
            last = next + 1;
            
            // Expiration Timestamp
            last = res.find("expiration_", last);
            last = res.find(":", last) + 1;
            next = res.find(",", last);
            token = res.substr(last, next-last);
            orders.back().expirationTime = atoll(token.c_str());
            last = next + 1;
            
            // Completion Timestamp
            last = res.find("completed_", last);
            last = res.find(":", last) + 1;
            next = res.find(",", last);
            token = res.substr(last, next-last);
            orders.back().completedTime = atoll(token.c_str());
            last = next + 1;
            
            // Type
            last = res.find("type", last);
            last = res.find(":", last) + 2;
            next = res.find("\"", last);
            orders.back().type = res.substr(last, next-last);
            last = next + 1;
            
            // State
            last = res.find("state", last);
            last = res.find(":", last) + 2;
            next = res.find("\"", last);
            orders.back().state = res.substr(last, next-last);
            last = next + 1;
            
            // Price
            last = res.find("limit_price", last);
            last = res.find(":", last) + 2;
            next = res.find("\"", last);
            token = res.substr(last, next-last);
            orders.back().price = atof(token.c_str());
            last = next + 1;
            
            // Volume
            last = res.find("limit_volume", last);
            last = res.find(":", last) + 2;
            next = res.find("\"", last);
            token = res.substr(last, next-last);
            orders.back().volume = atof(token.c_str());
            last = next + 1;
            
            // Base
            last = res.find("base", last);
            last = res.find(":", last) + 2;
            next = res.find("\"", last);
            token = res.substr(last, next-last);
            orders.back().baseValue = atof(token.c_str());
            last = next + 1;
            
            // Counter
            last = res.find("counter", last);
            last = res.find(":", last) + 2;
            next = res.find("\"", last);
            token = res.substr(last, next-last);
            orders.back().counterValue = atof(token.c_str());
            last = next + 1;
            
            // Base Fee
            last = res.find("fee_base", last);
            last = res.find(":", last) + 2;
            next = res.find("\"", last);
            token = res.substr(last, next-last);
            orders.back().baseFee = atof(token.c_str());
            last = next + 1;
            
            // Counter Fee
            last = res.find("fee_counter", last);
            last = res.find(":", last) + 2;
            next = res.find("\"", last);
            token = res.substr(last, next-last);
            orders.back().counterFee = atof(token.c_str());
            last = next + 1;
            
            // Pair
            last = res.find("pair", last);
            last = res.find(":", last) + 2;
            next = res.find("\"", last);
            orders.back().pair = res.substr(last, next-last);
            last = next + 1;
            
            if (orders.back().baseValue == 0.0f
                && orders.back().counterValue == 0.0f)
                orders.pop_back();
        }
        return orders;
    }

    template <class T> T& operator << (T& stream, UserOrder& order) {
        std::stringstream ss;
        ss << "ID: : " << order.orderID << "\n";
        ss << "Created: " << order.createdTime
            << ", expired: " << order.expirationTime
            << ", completed: " << order.completedTime << "\n";
        ss << "Type: " << order.type << "\n";
        ss << "State: " << order.state << "\n";
        ss << "Price: " << order.price << "\n";
        ss << "Volume: " << order.volume << "\n";
        ss << "Base: " << order.baseValue
        << ", fee: " << order.baseFee << "\n";
        ss << "Counter: " << order.counterValue
        << ", fee: " << order.counterFee << "\n";
        ss << "Pair: " << order.pair << "\n";
        stream.append(ss.str().c_str());
        return stream;
    }
    template <class T> T& operator << (T& stream, std::vector<UserOrder>& orders){
        for (UserOrder& order : orders){
            stream << order;
        }
        return stream;
    }
    template QTextEdit& operator << <QTextEdit>(QTextEdit&, UserOrder& order);
    template QTextEdit& operator << <QTextEdit>(QTextEdit&, std::vector<UserOrder>& order);

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
        std::string res = this->request("GET", uri.c_str(), true);
        
        if (httpCode != 200)
            throw ResponseEx("Error " + std::to_string(httpCode) + " - " + res);
        
        std::vector<UserTrade> trades;
           size_t last = 0, next = 0;
           last = res.find("[", last) + 1;
           
           while ((last = res.find("{", last)) != std::string::npos) {
               trades.push_back(UserTrade());
        
               // pair
               last = res.find("pair", last);
               last = res.find(":", last) + 2;
               next = res.find("\"", last);
               trades.back().pair = res.substr(last, next-last);
               last = next + 1;
               
               // sequence
               last = res.find("sequ", last);
               last = res.find(":", last) + 1;
               next = res.find(",", last);
               std::string token = res.substr(last, next-last);
               trades.back().sequence = atoll(token.c_str());
               last = next + 1;
               
               // Order ID
               last = res.find("order_id", last);
               last = res.find(":", last) + 2;
               next = res.find("\"", last);
               trades.back().orderID = res.substr(last, next-last);
               last = next + 1;
               
               // Type
               last = res.find("type", last);
               last = res.find(":", last) + 2;
               next = res.find("\"", last);
               trades.back().type = res.substr(last, next-last);
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
               
               // base
               last = res.find("base", last);
               last = res.find(":", last) + 2;
               next = res.find("\"", last);
               token = res.substr(last, next-last);
               trades.back().baseValue = atof(token.c_str());
               last = next + 1;
               
               // counter
               last = res.find("counter", last);
               last = res.find(":", last) + 2;
               next = res.find("\"", last);
               token = res.substr(last, next-last);
               trades.back().counterValue = atof(token.c_str());
               last = next + 1;
               
               // base fee
               last = res.find("fee_base", last);
               last = res.find(":", last) + 2;
               next = res.find("\"", last);
               token = res.substr(last, next-last);
               trades.back().baseFee = atof(token.c_str());
               last = next + 1;
               
               // counter fee
               last = res.find("fee_counter", last);
               last = res.find(":", last) + 2;
               next = res.find("\"", last);
               token = res.substr(last, next-last);
               trades.back().counterFee = atof(token.c_str());
               last = next + 1;
               
               // is buy
               last = res.find("is_buy", last);
               last = res.find(":", last) + 1;
               next = res.find("}", last);
               token = res.substr(last, next-last);
               trades.back().isBuy = (token == "true") ? true : false;
               last = next + 1;
           }
        return trades;
    }
    template <class T> T& operator << (T& stream, UserTrade& trade){
        std::stringstream ss;
        ss << "Pair: " << trade.pair << "\n";
        ss << "Sequence: " << trade.sequence << "\n";
        ss << "ID: " << trade.orderID << "\n";
        ss << "Timestamp: " << trade.timestamp << "\n";
        ss << "Type: " << trade.type << "\n";
        ss << "Price: " << trade.price << "\n";
        ss << "Volume: " << trade.volume << "\n";
        ss << "Base: " << trade.baseValue
        << ", fee: " << trade.baseFee << "\n";
        ss << "Counter: " << trade.counterValue
        << ", fee: " << trade.counterFee << "\n";
        ss << "Is_buy: " << trade.isBuy << "\n";

        stream.append(ss.str().c_str());
        return stream;
        
    }
    template <class T> T& operator << (T& stream, std::vector<UserTrade>& trades){
        for (UserTrade& trade : trades){
            stream << trade;
        }
        return stream;
    }
    template QTextEdit& operator << <QTextEdit>(QTextEdit&, UserTrade& order);
    template QTextEdit& operator << <QTextEdit>(QTextEdit&, std::vector<UserTrade>& order);

    // GET ORDER DETAILS
    //
    //
    std::string LunoClient::getOrderDetails(std::string id){
        std::string uri = "https://api.mybitx.com/api/1/orders/" + id;
        std::string res = this->request("GET", uri.c_str(), true);
        
        if (httpCode != 200)
            throw ResponseEx("Error " + std::to_string(httpCode) + " - " + res);
        
        return res;
    }
    // POST LIMIT ORDER
    //
    //
    std::string LunoClient::postLimitOrder(std::string pair, std::string type, float volume, float price){
        std::string uri = "https://api.mybitx.com/api/1/postorder?pair=" + pair;
        uri += "&type=" + type;
        uri += "&post_only=true";
        
        uri += "&volume=" + std::to_string(volume);
        uri += "&price=" + std::to_string(price);
        
        std::string res = this->request("POST", uri.c_str(), true);
        
        if (httpCode != 200)
            throw ResponseEx("Error " + std::to_string(httpCode) + " - " + res);
        
        return res;
    }

    // STOP ORDER
    //
    //
    std::string LunoClient::stopOrder(std::string orderId){
        std::string uri = "https://api.mybitx.com/api/1/stoporder?order_id=" + orderId;
        
        std::string res = this->request("POST", uri.c_str(), true);
        
        if (httpCode != 200)
            throw ResponseEx("Error " + std::to_string(httpCode) + " - " + res);
        
        return res;
    }

    // GET RECIEVE ADDRESS
    //
    //
    std::string LunoClient::getRecieveAddress(std::string asset){
        std::string uri = "https://api.mybitx.com/api/1/funding_address";
        uri += "?asset=" + asset;
        std::string res = this->request("GET", uri.c_str(), true);
        
        if (httpCode != 200)
            throw ResponseEx("Error " + std::to_string(httpCode) + " - " + res);
        
        return res;
    }

    // GET WITHDRAWAL LIST
    //
    //
    std::string LunoClient::getWithdrawalList(){
        std::string uri = "https://api.mybitx.com/api/1/withdrawals";
        std::string res = this->request("GET", uri.c_str(), true);
        
        if (httpCode != 200)
            throw ResponseEx("Error " + std::to_string(httpCode) + " - " + res);
        
        return res;
    }

    // REQUEST WITHDRAW
    //
    //
    std::string LunoClient::withdraw(float amount){
        std::string uri = "https://api.mybitx.com/api/1/withdrawals";
        uri += "?type=ZAR_EFT";
        uri += "&amount=" + std::to_string(amount);
        
        std::string res = this->request("POST", uri.c_str(), true);
        
        if (httpCode != 200)
            throw ResponseEx("Error " + std::to_string(httpCode) + " - " + res);
        
        return res;
    }

    // GET WITHDRAWAL
    //
    //
    std::string LunoClient::getWithdrawal(std::string id){
        std::string uri = "https://api.mybitx.com/api/1/withdrawals/" + id;
        std::string res = this->request("GET", uri.c_str(), true);
        
        if (httpCode != 200)
            throw ResponseEx("Error " + std::to_string(httpCode) + " - " + res);
        
        return res;
    }

    // CANCEL WITHDRAWAL
    //
    //
    std::string LunoClient::cancelWithdrawal(std::string id){
        std::string uri = "https://api.mybitx.com/api/1/withdrawals/" + id;
        std::string res = this->request("DELETE", uri.c_str(), true);
        
        if (httpCode != 200)
            throw ResponseEx("Error " + std::to_string(httpCode) + " - " + res);
        
        return res;
    }
}
