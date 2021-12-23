#ifndef valrclient_hpp
#define valrclient_hpp

#include "valrTypes.hpp"
#include "client.hpp"

enum class VALR_PAY_NOTIFICATION {
    ID = 0,
    EMAIL,
    CELL_NUMBER
};

namespace VALR {
    class VALRClient : public Client {
    public:
        /* Market Functions */
        static OrderBook getOrderBook(std::string pair, bool useAuthenticatedAPI);
        static OrderBook getFullOrderBook(std::string pair, bool useAuthenticatedAPI);
        static std::vector<CurrencyInfo> getCurrencies();
        static std::vector<CurrencyPairInfo> getCurrencyPairs();
        static std::vector<OrderTypeInfo> getOrderTypes(std::string pair = "");
        static std::vector<Ticker> getTickers(std::string pair = "");
        static std::vector<Trade> getTrades(std::string pair, bool useAuthenticatedAPI, unsigned long long since = 0, unsigned long long until = 0, unsigned skip = 0, unsigned limit = 0, std::string beforeID = "");
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
        static std::vector<DepositInfo> getDepositHistory(std::string asset, int skip = 0, int limit = 20);
        static std::vector<WithdrawalInfo> getCryptoWithdrawalHistory(std::string asset, int skip = 0, int limit = 20);
        static std::vector<BankInfo> getBankAccounts(std::string asset);
        static std::string getFiatDepositReference(std::string asset);

        #define VALR_DEFAULT_BANK_ID "8731a73f-4171-4c8a-a78d-d76c61f44d55"
        static std::string fiatWithdraw(float amount, bool isFast, std::string accountID = VALR_DEFAULT_BANK_ID, std::string asset = "ZAR");
        static std::vector<InternationalBankInfo> getInternationalBankAccounts();
        static InternationalBankInstructions getInternationalDepositInstructions(std::string id);
        
        /* Order Functions */
        static SimpleQuote getOrderQuote(std::string pair, std::string action, float volume);
        static ExcercisedQuote excerciseOrderQuote(std::string pair, std::string action, float volume);
        static SimpleOrderStatus getSimpleOrderStatus(std::string pair, std::string id);
        static std::string formLimitPayload(std::string pair, std::string action,
                                            float volume, float price,
                                            bool isBatch = false);
        static std::string postLimitOrder(std::string pair, std::string action, float volume, float price);
        static std::string formMarketPayload(std::string pair, std::string action,
                                             float amount, bool isOfBaseCurrency,
                                             bool isBatch = false);
        static std::string postMarketOrder(std::string pair, std::string action, float amount, bool isOfBaseCurrency);
        static std::string formStopLimitPayload(std::string pair, std::string action,
                                                float volume, float price,
                                                float stopPrice, bool isStopLossLimit,
                                                bool isBatch = false);
        static std::string postStopLimitOrder(std::string pair, std::string action, float volume, float price, float stopPrice, bool isStopLossLimit);
        static std::string packBatchPayloadFromList(std::vector<std::string> payloadList);
        static BatchOrderOutcome postBatchOrders( std::string payload);
        static OrderIDOutcome getOrderDetails(std::string pair, std::string id, bool isCustomerOrderID = false);
        
        /* Pay Service Functions */
        static VALR_PAY_Result postNewPayment(float amount, VALR_PAY_NOTIFICATION notificationMethod,
                                              std::string notificationString,
                                              std::string beneficiaryReference = "",
                                              std::string myReference = "",
                                              bool isAnonymous = false);
        static PaymentLimitInfo getPaymentInfo();
        static std::string getUserPaymentID();
        static std::vector<VALR_PAY_History_Entry> getUserPaymentHistory(std::string statusFilter = "", unsigned skip = 0, unsigned limit = 0);
        static VALR_PAY_History_Entry getUserPaymentEntryByPaymentID(std::string id);
        static VALR_PAY_History_Entry getUserPaymentEntryByTransactionID(std::string id);
        
        //static Fee getFeeInfo(std::string pair);
        //static std::vector<UserOrder> getUserOrders(std::string pair = "",
          //                           std::string state = "",
            //                         int limit = 0,
              //                       long long before = 0);
        
        // Post market order (mission funciton)
        //static std::string getOrderDetails(std::string id);
        //static std::string stopOrder(std::string orderId);
            // returns boolean true or false

    };
}
#endif /* valrclient_hpp */
