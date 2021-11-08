#include "valrTypes.hpp"

bool abortStatus();
namespace VALR {
    /* Account Functions Types*/
    std::string OrderBook::toString() const{
        std::stringstream ss;
        ss << "\n" << "Timestamp: " << timestamp << "\n";
        
        ss << "Asks\n";
        for (Order order : asks){
            if (!order.id.empty())
                ss << "ID: " << order.id << ",  ";
            ss << "Price: " << order.price << " ";
            ss << "Volume: " << order.volume << ",  ";
            ss << "Count: " << order.count << "\n\n";
        }

        ss << "Bids\n";
        for (Order order : this->bids){
            if (!order.id.empty())
                ss << "ID: " << order.id << ",  ";
            ss << "Price: " << order.price << " ";
            ss << "Volume: " << order.volume << ",  ";
            ss << "Count: " << order.count << "\n\n";
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

    std::string OrderBook::FormatHTMLWith(std::vector<UserOrder>* userOrders) {
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
            
        std::string countString = std::to_string(count);
        std::for_each(userOrders->begin(), userOrders->end(),
                      [&tradeOpenByUser](VALR::UserOrder& order) {
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


    std::string CurrencyInfo::toString() const{
        std::stringstream ss;
        ss << "Symbol: " << symbol << "\n";
        ss << "Is Active: " << isActive << "\n";
        ss << "Short Name: " << shortName << "\n";
        ss << "Long Name: " << longName << "\n";
        ss << "Decimals: " << decimalCount << " ";
        ss << "(withdrawal: " << withdrawalDecimalCount << ")\n";
        return ss.str();
    }
   


    std::string CurrencyPairInfo::toString()const{
        std::stringstream ss;
        ss << "Symbol: " << symbol << "\n";
        ss << "Base currency: " << baseCurrency << "\n";
        ss << "Quote currency: " << quoteCurrency << "\n";
        ss << "Is active: " << isActive << "\n";
        ss << "Short name: " << shortName << "\n";
        ss << "Minimum base amount: " << baseMinTradable << "\n";
        ss << "Maximum base amount: " << baseMaxTradable << "\n";
        ss << "Minimum quote amount: " << quoteMinTradable << "\n";
        ss << "Maximum base amount: " << quoteMaxTradable << "\n";
        ss << "Tick size: " << tickSize << "\n";
        ss << "Base decimal places: " << baseDecimalCount << "\n";
        
        return ss.str();
    }

    std::string OrderTypeInfo::toString()const{
        std::stringstream ss;
        ss << "Symbol: " << symbol << "\n";
        ss << "Order Types: " << orderTypes << "\n";
        
        return ss.str();
    }

    std::string Ticker::toString() const {
        std::stringstream ss;
        ss << "Pair: " << pair << "\n";
        ss << "Timestamp: " << timestamp << "\n";
        ss << "Bid: " << bid << "\n";
        ss << "Ask: " << ask << "\n";
        ss << "Last trade: " << lastTrade << "\n";
        ss << "Last closing price: " << lastClosed << "\n";
        ss << "Last high price: " << high << "\n";
        ss << "Last low price: " << low << "\n";
        ss << "Base volume: " << baseVolume << "\n";
        ss << "Change from previouse: " << changeFromPrevious << "\n";
        return ss.str();
    }

    std::string Trade::toString(std::string formatType) const {
        std::stringstream ss;
        if (formatType == "csv"){
            ss << sequence << ", "
            << timestamp << ", "
            << price << ", "
            << baseVolume << ", "
            << isBuy
               << "\n";
        }
        else {
            ss << "Pair: " << pair << " - timesamp " << timestamp << "\n";
            ss << "Price: " << price << "\n";
            ss << "Volume: " << baseVolume << " (base)\n";
            ss << "Volume: " << quoteVolume << " (quote)\n";
            ss << "Sequence: " << sequence << "\n";
            ss << "Trade ID: " << id << "\n";
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
                    trades.back().baseVolume = atof(token.c_str());
                if (index == 4)
                    trades.back().isBuy = atoi(token.c_str());
                index++;
            }
        }
        return stream;
    }

    /* Account Function Types*/
    std::string KeyInfo::toString() const {
        std::stringstream ss;
        ss << "Label: " << label << std::endl;
        ss << "Permissions: " << permission << std::endl;
        ss << "Added at: " << createdAt << std::endl;
        ss << "Is sub account: " << (isSubAccount ? "True" : "False") << std::endl;
        ss << "Allowed IP Address" << allowedIP << std::endl;
        ss << "Allowed Withdraw" << allowedWithdraw << std::endl;
        return ss.str();
    }

    std::string Account::toString() const {
        std::stringstream ss;
        ss << "Label: " << label << std::endl;
        ss << "Id: : " << id << std::endl;
        return ss.str();
    }

    std::string Balance::toString() const {
        std::stringstream ss;
        ss << "Asset: " << asset << "\n";
        ss << "Balance: " << balance << "\n";
        ss << "Reserved: " << reserved << "\n";
        ss << "Available: " << available << "\n";
        ss << "Last confirmed: " << lastUpdated << "\n";
        return ss.str();
    }

    std::string AccountSummary::toString() const{
        std::stringstream ss;
        ss << "Account Info... " << "\n";
        ss << account.toString() << "\n";
        
        for (const Balance& bal : balances){
            ss << bal.toString() << "\n";
        }
        if (balances.empty())
            ss << "Balances: [no non-zero assets]\n";
        
        return ss.str();
    }

    /* Order Function Types*/
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

}
printableDefinition(VALR::OrderBook);
printableList(VALR::OrderBook);
printableDefinition(VALR::CurrencyInfo);
printableList(VALR::CurrencyInfo);
printableDefinition(VALR::CurrencyPairInfo);
printableList(VALR::CurrencyPairInfo);
printableDefinition(VALR::OrderTypeInfo);
printableList(VALR::OrderTypeInfo);
printableDefinition(VALR::Ticker);
printableList(VALR::Ticker);
printableDefinition(VALR::Trade);
printableList(VALR::Trade);

/* Account Functions */
printableDefinition(VALR::KeyInfo);
printableList(VALR::KeyInfo);
printableDefinition(VALR::Account);
printableList(VALR::Account);
printableDefinition(VALR::Balance);
printableList(VALR::Balance);
printableDefinition(VALR::AccountSummary);
printableList(VALR::AccountSummary);

printableDefinition(VALR::UserOrder);
printableList(VALR::UserOrder);
