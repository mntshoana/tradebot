#ifndef valrclient_hpp
#define valrclient_hpp

#include "valrTypes.hpp"
#include "client.hpp"

namespace VALR {
    class VALRClient : public Client {
    public:
        /* Market Functions */
        static OrderBook getOrderBook(std::string pair);
        static OrderBook getFullOrderBook(std::string pair);
        static std::vector<CurrencyInfo> getCurrencies();
        static std::vector<CurrencyPairInfo> getCurrencyPairs();
        static std::vector<OrderTypeInfo> getOrderTypes(std::string pair = "");
        static std::vector<Ticker> getTickers(std::string pair = "");
        static std::vector<Trade> getTrades(std::string pair, unsigned long long since = 0, unsigned long long until = 0, unsigned skip = 0, unsigned limit = 0, std::string beforeID = "");
        static std::string getServerTime();
        static std::string getServerStatus();

        /* Account Functions */
        static KeyInfo getCurrentKeyInfo();
        static std::vector<Account> getSubAccounts();
        static std::string createSubAccount(std::string label);
        static std::vector<AccountSummary> getNonZeroBalances();
        static void internalTransfer(std::string fromID, std::string toID,
                                     std::string asset, float amount);
        static std::vector<Balance> getBalances();
        static std::vector<TransactionInfo> getTransactionHistory(std::string asset = "", std::string transactionType = "", int skip = 0, int limit = 10,  std::string startT = "", std::string endT = "", std::string beforeID = "");
        static std::vector<UserTrade> getUserTrades(std::string pair,  int limit = 0);
        
        /* Wallet Functions */
        static std::string getAddress(std::string asset);
        static std::vector<AddressEntry> getWithdrawalAddressEntries(std::string asset = "");
        static WithdrawalDetail getWithdrawalInfo(std::string asset);
        static std::string withdraw (std::string asset, float amount, std::string address);
        static WithdrawalInfo getWithdrawalByID(std::string asset, std::string id);
        
        // create account
        // update account
        // list pending transaction
        // list transaction
        
        
        /* Order Functions */
        /*static Fee getFeeInfo(std::string pair);
        static std::vector<UserOrder> getUserOrders(std::string pair = "",
                                     std::string state = "",
                                     int limit = 0,
                                     long long before = 0);
        
        // Post market order (mission funciton)
        static std::string getOrderDetails(std::string id);
        static std::string postLimitOrder(std::string pair, std::string type, float volume, float price);
            // returns order id
        static std::string stopOrder(std::string orderId);
            // returns boolean true or false

        /* Quotes Functions */
        // Create quote
        // Get quote
        // Excercise quote
        // Discard quote

        /* Send and Receive Functions */
        /*static std::string getRecieveAddress(std::string asset);
        // Create receive address
        */
        /*prototype*/
        /* static std::string sendToAddress(std::string asset, std::string address, float amount);
        /* Danger: could lose asset*/

        /* Transfers Functions */
       /* static std::vector<Withdrawal> getWithdrawalList();
        static Withdrawal withdraw(float amount, bool isFast);
        static std::string getWithdrawal(std::string id);
        static std::string cancelWithdrawal(std::string id);
*/
    };
}
#endif /* valrclient_hpp */
