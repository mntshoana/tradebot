#include "lunoclient.hpp"

namespace Luno {
    extern Client client;
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

    std::string OrderBook::toString(){
        std::stringstream ss;
        ss << "Timestamp: " << this->timestamp << "\n";
        
        ss << "Asks\n";
        for (Order order : this->asks){
            ss << "Price: " << order.price << " ";
            ss << "Volume: " << order.volume << ",  ";
        }

        ss << "\n" << "Bids\n";
        for (Order order : this->bids){
            ss << "Price: " << order.price << " ";
            ss << "Volume: " << order.volume << ",  ";
        }
        return ss.str();
    }
    
    std::string OrderBook::Format(){
        std::stringstream ss;
        ss << std::fixed;
        ss << R"(
                <style>
                table {width: 100%;}
                tr { padding: 15px;}
                a {
                    color: inherit;
                    text-decoration: none;
                }
                td {
                    padding: 2px 4px 1px 2px;
                    text-align: center;
                    font-size: 15px;
                    font-weight: 700;
                }
                .Ask a {color: rgb(192, 51, 35);}
                .Bid a {color: rgb(54, 136, 87);}
                .Mid {padding: 5px 2px;}
                </style>
                <table width=100%>
        )";
        
        for (auto order = this->asks.rbegin(); order != this->asks.rend(); order++){
            ss << "<tr>\n";
            ss << "<td class=Ask>";
            ss << std::setprecision(0);
            ss << "<a href=\"" << order->price << "\">";
            ss  << order->price;
            ss << "</a></td>\n";
            
            ss << "<td>"
                << std::setprecision(6)
                << "<p>" <<  order->volume << "</p>"
                << "</td>\n";
            ss << "</tr>";
        }
        ss << "\n<tr><td class=Mid colspan=2>"
                << std::setprecision(0) << (this->asks[0].price - this->bids[0].price)
                << " Spread </td></tr>";
        for (Order order : this->bids){
            ss << "\n<tr>";
            ss << "\n<td class=Bid>";
            ss << std::setprecision(0);
            ss << "<a href=\"" << order.price << "\">";
            ss << order.price;
            ss << "</a></td>";
            ss << "\n<td>" << std::setprecision(6) << order.volume<< "</td>";
            ss << "\n</tr>";
        }
        ss << "</table>\n";
        return ss.str();
    }

    std::string OrderBook::FormatToShowUserOrders(std::vector<UserOrder>* userOrders){
            std::stringstream ss;
            ss << std::fixed;
            ss << R"(
                    <style>
                    table {width: 100%; border-collapse:collapse;}
                    tr { padding: 15px;}
                    a {
                        color: inherit;
                        text-decoration: none;
                    }
                    td {
                        padding: 2px 4px 1px 2px;
                        text-align: center;
                        font-size: 15px;
                        font-weight: 700;
                    }
                    .Ask a {color: rgb(192, 51, 35);}
                    .Bid a {color: rgb(54, 136, 87);}
                    .Mid {padding: 5px 2px;}
                    </style>
                    <table width=100%>
            )";
            
            std::map<int, bool> tradeOpenByUser;
                
            size_t count = userOrders->size();
            //for (int i = 0; i < count ; i++)
              //      tradeOpenByUser[(*userOrders)[i].price] = true;
        std::string countString = std::to_string(count);
        std::for_each(userOrders->begin(), userOrders->end(),
                      [&tradeOpenByUser](Luno::UserOrder& order) {
                        tradeOpenByUser[order.price] = true;
                        });
            for (auto order = this->asks.rbegin(); order != this->asks.rend(); order++){
                float trans = order->volume / 20.0;
                ss << "<tr "
                    << "style=\""
                    << "background-color:rgba(192, 51, 35, " << trans << ");\""
                    << ">\n";
                ss << "<td class=Ask>";
                ss << std::setprecision(0);
                ss << "<a href=\"" << order->price << "\">";
                ss  << ((tradeOpenByUser[order->price]) ? "*" : " ") << order->price;
                ss << "</a></td>\n";
                ss << "<td> <span style=\"background: none;\">"
                << std::setprecision(6)<< order->volume << "<span></td>\n";
                ss << "</tr>";
            }
            ss << "\n<tr><td class=Mid colspan=2>"
                    << std::setprecision(0) << (this->asks[0].price - this->bids[0].price)
                    << " Spread </td></tr>";
            for (Order order : this->bids){
                float trans = order.volume / 20.0;
                ss << "<tr "
                    << "style=\""
                    << "background-color:rgba(54, 136, 87, " << trans << ");\""
                    << ">\n";
                ss << "<td class=Bid>";
                ss << std::setprecision(0);
                ss << "<a href=\"" << order.price << "\">";
                ss  << ((tradeOpenByUser[order.price]) ? "*" : " ") << order.price;
                ss << "</a></td>\n";
                ss << "<td> <span style=\"background: none;\">"
                << std::setprecision(6) << order.volume<< "<span></td>\n";
                ss << "</tr>";
            }
            ss << "</table>\n";
            return ss.str();
    }
    /*template <class T> T& operator << (T& stream, OrderBook& ob) {
        stream.append(ob.toString().c_str());
        return stream;
    }*/
    /*template*/ QTextEdit& operator << /*<QTextEdit>*/(QTextEdit&, OrderBook& ob);

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

    template <class T> T& operator << (T& stream, Ticker& ticker) {
        std::stringstream ss;
        ss << "Pair: " << ticker.pair << "\n";
        ss << "Timestamp: " << ticker.timestamp << "\n";
        ss << "Bid: " << ticker.bid << "\n";
        ss << "Ask: " << ticker.ask << "\n";
        ss << "Last trade: " << ticker.lastTrade << "\n";
        ss << "Rolling 24 hour volume: " << ticker.rollingVolume << "\n";
        ss << "Status: " << ticker.status << "\n";
        stream.append(ss.str().c_str());
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
    }
}
