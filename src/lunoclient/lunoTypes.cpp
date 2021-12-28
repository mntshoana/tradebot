#include "lunoTypes.hpp"

bool abortStatus();
namespace Luno {

    /* Market Functions Types */
    std::string OrderBook::toString() const{
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


    std::string OrderBook::FormatHTML() {
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

    std::string OrderBook::FormatHTMLWith(std::vector<UserOrder>* userOrders){
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

    std::string Ticker::toString() const{
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

    std::string Candle::toString() const{
        std::stringstream ss;
        ss << "Open: " << open << "\n";
        ss << "Close: " << close << "\n";
        ss << "High: " << high << "\n";
        ss << "Low: " << low << "\n";
        ss << "Volume: " << volume << "\n";
        ss << "Timestamp: " << timestamp << "\n";
        
        return ss.str();
    }

    std::string CandleData::toString() const{
        std::stringstream ss;
        ss << pair << " candle (duration: " << duration << ")...\n\n";
        
        if (candles.size() > 0)
            ss << "Candles Data:\n";
        for (Candle candle : candles){
            ss << " " << candle.toString() << "\n";
        }
        return ss.str();
    }


    std::string Trade::toString(std::string formatType) const{
        std::stringstream ss;
        if (formatType == "csv"){
            ss << sequence << ", "
            << timestamp << ", "
            << price << ", "
            << volume << ", "
            << isBuy
               << "\n";
        }
        else {
            ss << "Sequence: " << sequence << "\n";
            ss << "Timestamp: " << timestamp << "\n";
            ss << "Price: " << price << "\n";
            ss << "Volume: " << volume << "\n";
            ss << "Is buy: " << (isBuy ? "true":"false") << "\n";
        }
        return ss.str();
    }
    
    std::fstream& operator << (std::fstream& stream, std::vector<Trade>& trades){
        for (Trade& trade : trades)
            stream << trade.toString("csv");
        return stream;
    }
    std::fstream& operator >> (std::fstream& stream, std::vector<Trade>& trades){
        std::string line, token;
        size_t index = 0;
        while (getline(stream, line)) {
            if (abortStatus())
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

    /* Order Functions Types*/
    std::string Fee::toString() const{
        std::stringstream ss;
        ss << "30 day volume: " << thirtyDayVolume << "\n";
        ss << "Maker fee: " << maker << "\n";
        ss << "Taker fee: "  << taker << "\n";
        return ss.str();
    }
    

    std::string UserOrder::toString() const{
        std::stringstream ss;
        ss << "ID: : " << orderID << "\n";
        ss << "Created: " << createdTime
            << ", expired: " << expirationTime
            << ", completed: " << completedTime << "\n";
        ss << "Type: " << type << "\n";
        ss << "State: " << state << "\n";
        ss << "Price: " << price << "\n";
        ss << "Volume: " << volume << "\n";
        ss << "Base: " << baseValue
        << ", fee: " << baseFee << "\n";
        ss << "Counter: " << counterValue
        << ", fee: " << counterFee << "\n";
        ss << "Pair: " << pair << "\n";
        return ss.str();
    }
    

    std::string UserTrade::toString() const{
        std::stringstream ss;
        ss << "Pair: " << pair << "\n";
        ss << "Sequence: " << sequence << "\n";
        ss << "ID: " << orderID << "\n";
        ss << "Timestamp: " << timestamp << "\n";
        ss << "Type: " << type << "\n";
        ss << "Price: " << price << "\n";
        ss << "Volume: " << volume << "\n";
        ss << "Base: " << baseValue
        << ", fee: " << baseFee << "\n";
        ss << "Counter: " << counterValue
        << ", fee: " << counterFee << "\n";
        ss << "Is_buy: " << isBuy << "\n";
        return ss.str();
    }
    

    /* Transfer Functions Types*/
    std::string Withdrawal::toString() const{
        std::stringstream ss;
        ss << "Timestamp: " << createdTime << "\n";
        ss << "Withdrawal ID: " << id << "\n";
        ss << "Type: " << type;
        ss << " (Currency: " << currency << "\n";
        ss << "Status: " << status << "\n";
        ss << "Amount: "  << amount << "\n";
        ss << "Fee: "  << fee << "\n";
        return ss.str();
    }


    /* Account Functions Types*/
    std::string Balance::toString() const{
        std::stringstream ss;
        ss << "ID: : " << accountID << "\n";
        ss << "Asset: " << asset << "\n";
        ss << "Balance: " << balance << "\n";
        ss << "Reserved: " << reserved << "\n";
        ss << "Unconfirmed: " << uncomfirmed << "\n";
        ss.str();
        return ss.str();
    }
    
}

/* Market Functions Types */
printableDefinition(Luno::OrderBook);
printableList(Luno::OrderBook);
printableDefinition(Luno::Ticker);
printableList(Luno::Ticker);
printableDefinition(Luno::Trade);
printableList(Luno::Trade);
printableDefinition(Luno::Candle);
printableList(Luno::Candle);
printableDefinition(Luno::CandleData);
printableList(Luno::CandleData);

printableDefinition(Luno::Fee);
printableList(Luno::Fee);
printableDefinition(Luno::UserOrder);
printableList(Luno::UserOrder);
printableDefinition(Luno::UserTrade);
printableList(Luno::UserTrade);
printableDefinition(Luno::Withdrawal);
printableList(Luno::Withdrawal);
printableDefinition(Luno::Balance);
printableList(Luno::Balance);
