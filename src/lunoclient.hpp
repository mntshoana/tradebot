#ifndef lunoclient_hpp
#define lunoclient_hpp

#include "lunoTypes.hpp"
#include "client.hpp"

namespace Luno {
    class LunoClient : public Client {
    public:
        /* Market Functions */
        OrderBook getOrderBook(std::string pair);
        OrderBook getFullOrderBook(std::string pair);
        Ticker getTicker(std::string pair);
        std::vector<Ticker> getTickers();
        std::vector<Trade> getTrades(std::string pair, unsigned long long since = 0);

        /* Order Functions */
        Fee getFeeInfo(std::string pair);
        std::vector<UserOrder> getUserOrders(std::string pair = "",
                                     std::string state = "",
                                     int limit = 0,
                                     long long before = 0);
        std::vector<UserTrade> getUserTrades(std::string pair,
                                             long long since = 0,
                                             long long before = 0,
                                             long long after_seq = 0,
                                             long long before_seq = 0,
                                             bool sort = false,
                                             int limit = 0);
        // Post market order (mission funciton)
        // Get order (missing function)
        std::string postLimitOrder(std::string pair, std::string type, float volume, float price);
            // returns order id
        std::string stopOrder(std::string orderId);
            // returns boolean true or false

        /* Quotes Functions */
        // Create quote
        // Get quote
        // Excercise quote
        // Discard quote

        /* Send and Receive Functions */
        // Get receive address
        // Create receive address
        // Send

        /* Withdrawals Functions */
        // List Withdrawal requests
        // Request a withdrawal
        std::string withdraw(float amount);
        // Get withdrawal request
        // Cancel Withdrawal

    };
}
#endif /* lunoclient_hpp */
