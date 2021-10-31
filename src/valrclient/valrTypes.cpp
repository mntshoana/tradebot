#include "valrTypes.hpp"

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
    

    std::string OrderBook::FormatHTML() const{
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
}
printableDefinition(VALR::OrderBook);
printableList(VALR::OrderBook);
printableDefinition(VALR::CurrencyInfo);
printableList(VALR::CurrencyInfo);
printableDefinition(VALR::CurrencyPairInfo);
printableList(VALR::CurrencyPairInfo);
