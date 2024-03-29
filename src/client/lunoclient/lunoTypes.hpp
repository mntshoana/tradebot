#ifndef lunoTypes_h
#define lunoTypes_h

#include "clientInterface.hpp"
#include "textPanel.hpp"
#include <string>
#include <iomanip>
#include <vector>
#include <sstream>
#include <fstream>

namespace Luno {

    /* Market Functions Types */
    class Order {
    public:
        Order() {}
        float price;
        float volume;
    };

    class UserOrder;
    class OrderBook {
    private:
    public:
        OrderBook() {};
        std::vector<Order> asks;
        std::vector<Order> bids;
        long long timestamp;
        std::string toString()const;
        std::string FormatHTML();
        std::string FormatHTMLWith(std::vector<UserOrder>*);
    };
    

    class Ticker {
    public:
        Ticker() {}
        std::string pair;
        unsigned long long timestamp;
        float bid;
        float ask;
        float lastTrade;
        float rollingVolume;
        std::string status;
        std::string toString()const;
    };
   

    class Trade {
    public:
        Trade() {}
        unsigned long long sequence;
        unsigned long long timestamp;
        float price;
        float volume;
        bool isBuy;
        std::string toString(std::string formatType = "")const;
    };

    std::fstream& operator << (std::fstream& stream, std::vector<Trade>& trades);
    std::fstream& operator >> (std::fstream& stream, std::vector<Trade>& trades);

    class Candle {
    public:
        Candle() {}
        float open;
        float close;
        float high;
        float low;
        unsigned long long timestamp;
        float volume;
        std::string toString () const;
    };

    class CandleData {
    public:
        CandleData() {}
        std::string pair;
        int duration;
        std::vector<Candle> candles;
    
        std::string toString () const;
    };

    class CurrencyPairInfo {
    public:
        std::string symbol;
        std::string baseCurrency;
        std::string quoteCurrency;
        std::string tradeStatus;
        float baseMin;
        float baseMax;
        float quoteMin;
        float quoteMax;
        int baseDecimalCount;
        int quoteDecimalCount;
        int feeDecimalCount;
        std::string toString()const;
        
    };


    /* Order Functions Types*/
    class Fee {
    public:
        Fee(){}
        float thirtyDayVolume;
        float maker;
        float taker;
        std::string toString()const;
    };

    class UserOrder : public OrderType {
    public:
        UserOrder(){}
        std::string orderID;
        long long createdTime;
        long long expirationTime;
        long long completedTime;
        std::string type;
        std::string state;
        float price;
        float volume;
        float baseValue;
        float counterValue;
        float baseFee;
        float counterFee;
        std::string pair;
        
        std::string toString() const;

        virtual std::string getType() const;
        virtual std::string getID() const;
        virtual long long getTimestamp() const;
        virtual float getPrice() const;
        virtual float getVolume() const;
        virtual float getBaseValue() const;
        virtual float getQuoteValue() const;
    };
    

    class UserTrade : public Trade {
    public:
        UserTrade() : Trade() {}
        std::string pair;
        std::string orderID;
        std::string type;
        float baseValue, baseFee;
        float counterValue, counterFee;
        
        std::string toString()const;
    };
    
    class MoveSummary {
    public:
        MoveSummary(){}
        std::string id;
        std::string customID;
        std::string status;
        
        std::string toString()const;
    };

    class MoveResult : public MoveSummary {
    public:
        MoveResult(){}
        float amount;
        unsigned long long timestamp;
        unsigned long long lastUpdatedAt;
        std::string sourceAccountID;
        std::string destinationAccountID;
        
        std::string toString() const;
    };

    /* Beneficiaries Function Types*/
    class Beneficiary {
    public:
        Beneficiary() {}
        std::string id;
        std::string bank;
        std::string branch;
        std::string accountNumber;
        std::string accountType;
        std::string country;
        std::string accountHolder;
        bool supportsFastWithdrawal;
        unsigned long long timestamp;
        
        std::string toString() const;
    };

    /* Transfer Functions Types*/
    class Withdrawal {
    public:
        Withdrawal(){}
        long long id;
        std::string status;
        long long createdTime;
        std::string type;
        std::string currency;
        float amount;
        float fee;
        
        std::string toString()const;
    };
   
    /* Account Functions Types*/
    class Balance : public BalanceType{
    public:
        Balance(){}
        std::string accountID;
        std::string asset;
        float balance;
        float reserved;
        float uncomfirmed;
        
        virtual std::string getAccountID() const override;
        virtual std::string getAsset() const override;
        virtual float getBalance() const override;
        virtual float getReserved() const override;
        virtual float getUncomfirmed() const override;
        std::string toString() const;
    };
}

/* Market Functions Types */
declarePrintable(Luno::OrderBook);
declarePrintableList(Luno::OrderBook);
declarePrintable(Luno::Ticker);
declarePrintableList(Luno::Ticker);
declarePrintable(Luno::Trade);
declarePrintableList(Luno::Trade);
declarePrintable(Luno::Candle);
declarePrintableList(Luno::Candle);
declarePrintable(Luno::CandleData);
declarePrintableList(Luno::CandleData);
declarePrintable(Luno::CurrencyPairInfo);
declarePrintableList(Luno::CurrencyPairInfo);

/* Account Functions */
declarePrintable(Luno::Balance);
declarePrintableList(Luno::Balance);
declarePrintable(Luno::MoveSummary);
declarePrintableList(Luno::MoveSummary);
declarePrintable(Luno::MoveResult);
declarePrintableList(Luno::MoveResult);

/* Beneficiaries Function Types*/
declarePrintable(Luno::Beneficiary);
declarePrintableList(Luno::Beneficiary);

/* Order Functions Types*/
declarePrintable(Luno::UserOrder);
declarePrintableList(Luno::UserOrder);
declarePrintable(Luno::Fee);
declarePrintableList(Luno::Fee);
declarePrintable(Luno::UserTrade);
declarePrintableList(Luno::UserTrade);

/* Transfers Functions */
declarePrintable(Luno::Withdrawal);
declarePrintableList(Luno::Withdrawal);

#endif /* lunoTypes_h */
