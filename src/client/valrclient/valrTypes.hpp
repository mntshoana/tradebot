#ifndef valrTypes_h
#define valrTypes_h

#include "clientInterface.hpp"
#include "textPanel.hpp"
#include <string>
#include <iomanip>
#include <vector>
#include <sstream>
#include <fstream>

namespace VALR {
    /* Market Functions Types */
    class Order {
    public:
        Order() {}
        std::string id;
        float price;
        float volume;
        int count; // count represents a tally with partial OrderBook (40 bids and 40 asks prices, orders with matching prices are tallied by count)
                   // count represents a position in queue when OrderBook is complete. Orders with matching price are seperate orders and are not tallied)
    };

    /* Market Functions Types */
    class UserOrder;
    class OrderBook {
    public:
        OrderBook() {};
        std::vector<Order> asks;
        std::vector<Order> bids;
        unsigned long long timestamp;
        std::string toString()const;
        std::string FormatHTML();
        std::string FormatHTMLWith(std::vector<UserOrder>*);
    };

    class CurrencyInfo {
    public:
        std::string symbol;
        bool isActive;
        std::string shortName;
        std::string longName;
        int decimalCount;
        int withdrawalDecimalCount;
        
        std::string toString()const;
    };
    

    class CurrencyPairInfo {
    public:
        std::string symbol;
        std::string baseCurrency;
        std::string quoteCurrency;
        bool isActive;
        std::string shortName;
        float baseMinTradable;
        float baseMaxTradable;
        float quoteMinTradable;
        float quoteMaxTradable;
        int tickSize;
        int baseDecimalCount;
        
        std::string toString()const;
    };

    class OrderTypeInfo {
    public:
        std::string symbol;
        std::string orderTypes;
    
        std::string toString()const;
    };

   
    class Ticker {
    public:
        Ticker() {}
        std::string pair;
        unsigned long long timestamp;
        float bid;
        float ask;
        float lastTrade;
        float lastClosed;
        float high;
        float low;
        float baseVolume;
        float changeFromPrevious;
        std::string toString() const;
    };
 

    class Trade {
    public:
        Trade() {}
        unsigned long long sequence;
        unsigned long long timestamp;
        float price;
        float baseVolume;
        float quoteVolume;
        std::string pair;
        std::string id;
        bool isBuy;
        std::string toString(std::string formatType = "") const;
    };

     std::fstream& operator << (std::fstream& stream, std::vector<Trade>& trades);
     std::fstream& operator >> (std::fstream& stream, std::vector<Trade>& trades);


    /* Account Function Types*/
    class KeyInfo {
    public:
        KeyInfo(){}
        std::string label;
        std::string permission;
        std::string createdAt;
        bool isSubAccount;
        std::string allowedIP;
        std::string allowedWithdraw;
        
        std::string toString() const;
    };
    
    class Account {
    public:
        Account(){}
        std::string label;
        unsigned long long id;
        
        std::string toString() const;
    };

    class Balance {
    public:
        Balance(){}
        std::string asset;
        float available;
        float balance;
        float reserved;
        unsigned long long lastUpdated; // timestamp
        std::string toString() const;
    };
    
    class AccountSummary {
    public:
        AccountSummary(){}
        Account account;
        std::vector<Balance> balances;
        std::string toString() const;
    };

    class TransactionInfo {
    public:
        TransactionInfo(){}
        std::string type;
        std::string description;
        std::string creditAsset;
        std::string debitAsset;
        std::string feeAsset;
        float creditValue;
        float debitValue;
        float feeValue;
        unsigned long long timestamp;
        std::string id;
        std::string additionalInfo;
        std::string toString() const;
    };
    /* Order Function Types*/
    /*class Fee {
    public:
        Fee(){}
        float thirtyDayVolume;
        float maker;
        float taker;
        std::string toString();
    };
    declarePrintable(Fee);
    declarePrintableList(Fee);
*/
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
        std::string orderID;
          
        std::string toString() const;
    };

    /* Wallet Function Types */
    class AddressEntry {
    public:
        AddressEntry() {}
        std::string id;
        std::string label;
        std::string asset;
        std::string address;
        std::string timestamp;
           
        std::string toString() const;
    };
    
    class WithdrawalDetail {
    public:
        WithdrawalDetail() {}

        std::string asset;
        float min;
        int decimals;
        bool isActive;
        float fee;
        bool supportsReference;
        std::string toString() const;
    };

    class WithdrawalInfo {
    public:
        WithdrawalInfo() {}
        std::string asset;
        std::string address;
        float amount;
        float fee;
        std::string transactionHash;
        int confirmations;
        std::string lastConfrimationAt;
        std::string id;
        std::string timestamp;
        bool isVerified;
        std::string status;
        
        std::string toString() const;
    };

    typedef WithdrawalInfo DepositInfo;

    class BankInfo {
    public:
        std::string id;
        std::string bank;
        std::string accountHolder;
        std::string accountNumber;
        std::string branchCode;
        std::string accountType;
        std::string timestamp;
        
        std::string toString() const;
    };

    class InternationalBankInfo {
    public:
        std::string id;
        std::string accountNumber;
        std::string routingNumber;
        // NB, the following variable will contain a string that is left unmarshalled
        std::string billingDetails;
        // NB, the following variable will contain a string that is left unmarshalled
        std::string bankAddress;
        std::string status;
        std::string type;
        std::string timestamp;
        
        std::string toString() const;
    };

    class InternationalBankInstructions {
    public:
        // Incomplete type
        // because not sure what to expect from server
        std::string reference;
        // NB, the following variable will contain a string that is left unmarshalled
        std::string beneficiary;
        // NB, the following variable will contain a string that is left unmarshalled
        std::string beneficiaryBank;
          
        std::string toString() const;
        
    };

    /* Order Function Types */
    class OrderMatched {
    public:
        double price;
        double volume;
        
        std::string toString() const;
    };

    class SimpleQuote {
    public:
        // Incomplete type
        // because not sure what to expect from server
        std::string pair;
        double amount;
        double willRecieve;
        double fee;
        std::string feeAsset;
        std::string timestamp;
        std::string id;
        std::vector<OrderMatched> ordersMatched;
          
        std::string toString() const;
        
    };

    class ExcercisedQuote {
    public:
        int code;
        std::string message;
        
        std::string toString() const;
    };

    class SimpleOrderStatus {
    public:
        std::string id;
        bool success;
        bool processing;
        double paidAmount;
        std::string paidAsset;
        double receivedAmount;
        std::string receivedAsset;
        double fee;
        std::string feeAsset;
        std::string timestamp;
        
        std::string toString() const;
    };

    class VALR_PAY_Result {
    public:
        std::string id;
        std::string transactionID;
        
        std::string toString() const;
    };
    
    class PaymentLimitInfo {
    public:
        float maxAmount;
        float minAmount;
        std::string asset;
        std::string limitedByWhat;

        std::string toString() const;
    };

    class VALR_PAY_History_Entry {
    public:
        std::string paymentID;
        std::string otherPartysID;
        std::string transactionID;
        float amount;
        std::string status;
        std::string timestamp;
        std::string myReference;
        std::string beneficiaryReference;
        bool isAnonymous;
        std::string type;
        
        std::string toString() const;
    };

    class OrderOutcome {
    public:
        OrderOutcome() {accepted = false;}
        bool accepted;
        std::string orderId;
        int errorCode;
        std::string message;
            
        std::string toString() const;
    };

    class BatchOrderOutcome {
    public:
        std::vector<OrderOutcome> orders;
        std::string batchId;
        
        std::string toString() const;
    };

    class OrderIDOutcome {
    public:
        std::string orderID;
        std::string status;
        std::string pair;
        float price;
        float volume;
        float volumeRemaining;
        bool isBuy;
        std::string orderType;
        std::string message;
        std::string customerOrderID;
        std::string timestamp;
        std::string lastUpdated;
        std::string timeInForce;
        
        std::string toString() const;
    };

    class OrderHistory : public OrderIDOutcome {
    public:
        float avgPrice;
        float total;
        float totalFee;
        std::string feeAsset;

        std::string toString() const;
    };

    class OpenOrder : public OrderIDOutcome {
    public:
        float stopPrice;
        float filledPercentage;
        std::string toString() const;
    };
}
declarePrintable(VALR::UserOrder);
declarePrintableList(VALR::UserOrder);
declarePrintable(VALR::OrderBook);
declarePrintableList(VALR::OrderBook);
declarePrintable(VALR::CurrencyInfo);
declarePrintableList(VALR::CurrencyInfo);
declarePrintable(VALR::CurrencyPairInfo);
declarePrintableList(VALR::CurrencyPairInfo);
declarePrintable(VALR::OrderTypeInfo);
declarePrintableList(VALR::OrderTypeInfo);
declarePrintable(VALR::Ticker);
declarePrintableList(VALR::Ticker);
declarePrintable(VALR::Trade);
declarePrintableList(VALR::Trade);

/* Account Functions */
declarePrintable(VALR::KeyInfo);
declarePrintableList(VALR::KeyInfo);
declarePrintable(VALR::Account);
declarePrintableList(VALR::Account);
declarePrintable(VALR::Balance);
declarePrintableList(VALR::Balance);
declarePrintable(VALR::AccountSummary);
declarePrintableList(VALR::AccountSummary);
declarePrintable(VALR::TransactionInfo);
declarePrintableList(VALR::TransactionInfo);
declarePrintable(VALR::UserTrade);
declarePrintableList(VALR::UserTrade);

/* Wallet Functions */
declarePrintable(VALR::AddressEntry);
declarePrintableList(VALR::AddressEntry);
declarePrintable(VALR::WithdrawalDetail);
declarePrintableList(VALR::WithdrawalDetail);
declarePrintable(VALR::WithdrawalInfo);
declarePrintableList(VALR::WithdrawalInfo);
declarePrintable(VALR::BankInfo);
declarePrintableList(VALR::BankInfo);
declarePrintable(VALR::InternationalBankInfo);
declarePrintableList(VALR::InternationalBankInfo);
declarePrintable(VALR::InternationalBankInstructions);
declarePrintableList(VALR::InternationalBankInstructions);

/* Order Functions */
declarePrintable(VALR::OrderMatched);
declarePrintableList(VALR::OrderMatched);
declarePrintable(VALR::SimpleQuote);
declarePrintableList(VALR::SimpleQuote);
declarePrintable(VALR::ExcercisedQuote);
declarePrintableList(VALR::ExcercisedQuote);
declarePrintable(VALR::SimpleOrderStatus);
declarePrintableList(VALR::SimpleOrderStatus);
declarePrintable(VALR::OrderOutcome);
declarePrintableList(VALR::OrderOutcome);
declarePrintable(VALR::BatchOrderOutcome);
declarePrintableList(VALR::BatchOrderOutcome);
declarePrintable(VALR::OrderIDOutcome);
declarePrintableList(VALR::OrderIDOutcome);
declarePrintable(VALR::OrderHistory);
declarePrintableList(VALR::OrderHistory);
declarePrintable(VALR::OpenOrder);
declarePrintableList(VALR::OpenOrder);

/* Pay Service Functions */
declarePrintable(VALR::VALR_PAY_Result);
declarePrintableList(VALR::VALR_PAY_Result);
declarePrintable(VALR::PaymentLimitInfo);
declarePrintableList(VALR::PaymentLimitInfo);
declarePrintable(VALR::VALR_PAY_History_Entry);
declarePrintableList(VALR::VALR_PAY_History_Entry);

#endif /* valrTypes_h */
