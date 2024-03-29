#ifndef lunoclient_hpp
#define lunoclient_hpp

#include "lunoTypes.hpp"
#include "client.hpp"

namespace Luno {
    static  std::vector<CurrencyPairInfo> currencies;

    class LunoClient : public Client {
    public:
        /* Market Functions */
        static OrderBook getOrderBook(std::string pair);
        static OrderBook getFullOrderBook(std::string pair);
        static Ticker getTicker(std::string pair);
        static std::vector<Ticker> getTickers();
        static std::vector<Trade> getTrades(std::string pair, unsigned long long since = 0);
        static CandleData getCandles(std::string pair, unsigned long long sinceTimestamp, int duration);
        static std::vector<CurrencyPairInfo> getMarketInfo();
        
        /* Account Functions */
        static std::string createAccount(std::string asset, std::string name);
        static bool updateAccount(std::string id, std::string name);
        static std::string getPendingTransactions(std::string id);
        static std::string getTransactions(std::string id, int minRowRange, int maxRowRange);
        static std::vector<Balance> getBalances(std::string asset = "");
        static MoveSummary postMove(std::string sourceID, std::string destinationID, float amount, std::string customID = "");
        static MoveResult queryMove(std::string id, bool isCustomID = false);
        static std::vector<MoveResult> ListMoveHistory(int limit = 100, unsigned long long since = 0);
        
        /* Beneficiaries Functions */
        static std::vector<Beneficiary> listBeneficiaries();
        
        /* Order Functions */
        static Fee getFeeInfo(std::string pair);
        static std::vector<UserOrder> getUserOrders(std::string pair = "",
                                     std::string state = "",
                                     int limit = 0,
                                     long long before = 0);
        static std::vector<UserTrade> getUserTrades(std::string pair,
                                             long long since = 0,
                                             long long before = 0,
                                             long long after_seq = 0,
                                             long long before_seq = 0,
                                             bool sort = false,
                                             int limit = 0);
        static std::string postMarketOrder(std::string pair, std::string action, float amount);
        static UserOrder getOrderDetails(std::string id);
        static std::string postLimitOrder(std::string pair, std::string action, float volume, float price);
        static bool cancelOrder(std::string orderId);
        // getOrder v2 and v3 (not needed)
        // getOrders v2 (not needed)

        /* Send and Receive Functions */
        static std::string getRecieveAddress(std::string asset, std::string name = "");
        static std::string createRecieveAddress(std::string asset, std::string name = "");
        
        /*prototype*/
        /**/ static std::string sendToAddress(std::string asset, std::string address, float amount);
        /* Danger: could lose asset if not careful - NOT TESTED YET*/

        /* Transfers Functions */
        static std::vector<Withdrawal> getWithdrawalList();
        static Withdrawal withdraw(float amount, bool isFast, std::string beneficiaryID = "");
        static std::string getWithdrawal(std::string id);
        static std::string cancelWithdrawal(std::string id);
        // getTransfers (not needed)
    };
}
#endif /* lunoclient_hpp */
