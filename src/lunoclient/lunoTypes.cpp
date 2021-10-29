#include "lunoTypes.hpp"
#include "client.hpp"

#define printableDefinition(type) \
    template <class T> T& operator << (T& stream, type& variable) { \
        stream.append(variable.toString().c_str()); \
        return stream; \
    }       \
    template QTextEdit& operator << <QTextEdit>(QTextEdit& stream, type& variable); \
            \
    template <class T> T& operator << (T& stream, std::vector<type>& vars) {\
        for (type& variable : vars) \
            stream << variable; \
        return stream; \
    }       \
    template QTextEdit& operator << <QTextEdit>(QTextEdit& stream,  std::vector<type>& vars);

namespace Luno {
    /* Account Functions Types*/
    std::string Balance::toString(){
        std::stringstream ss;
        ss << "ID: : " << accountID << "\n";
        ss << "Asset: " << asset << "\n";
        ss << "Balance: " << balance << "\n";
        ss << "Reserved: " << reserved << "\n";
        ss << "Unconfirmed: " << uncomfirmed << "\n";
        ss.str();
        return ss.str();
    }
    printableDefinition(Balance);

    /* Market Functions Types */
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
    printableDefinition(Ticker);


    std::string Trade::toString(std::string formatType){
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
    printableDefinition(Trade);
    std::fstream& operator << (std::fstream& stream, std::vector<Trade>& trades){
        for (Trade& trade : trades)
            stream << trade.toString("csv");
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

    /* Order Functions Types*/
    std::string Fee::toString(){
        std::stringstream ss;
        ss << "30 day volume: " << thirtyDayVolume << "\n";
        ss << "Maker fee: " << maker << "\n";
        ss << "Taker fee: "  << taker << "\n";
        return ss.str();
    }
    printableDefinition(Fee);

    std::string UserOrder::toString(){
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
    printableDefinition(UserOrder);


    std::string UserTrade::toString(){
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
    printableDefinition(UserTrade);

    /* Transfer Functions Types*/
    std::string Withdrawal::toString(){
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
    printableDefinition(Withdrawal);
}
