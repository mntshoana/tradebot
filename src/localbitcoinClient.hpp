#ifndef localbitcoinClient_hpp
#define localbitcoinClient_hpp

#include <QtCore/QDateTime>
#include "client.hpp"

#include "sha256.h"
#include "hmac.h"

namespace LocalBitcoin {
    // Major difference between parent and child is in the method of authentication used in request
    class LocalBitcoinClient : public Client {
        std::string request (const char* method, const char* uri, bool auth = false) override final;
    
    // Need more generic returned types (need to improve)
    public:
        /* Advertisements Funcs */
        std::string getUserAds(int visible = -1, std::string type = "", std::string currency = "", std::string country = "");
        std::string getUserAd(std::string adID); // can include multiple comma separated ids
        
        ///api/ad/{ad_id}/
        // ...
        ///api/equation/{equation_string}
        
        /* Trades Funcs */
        // ...
        
        /* Account Funcs */
        // ...
        
        /* Wallet Funcs */
        std::string getWallet();
        // ...
        
        /* Public Market Data Funcs */
        // ...
        std::string getBuyAds (std::string countryCode, std::string country, std::string paymentMethod = "");
        
        /* Helper Funcs */
        std::string paymentMethods();
    };
}

#endif /* localbitcoinClient_hpp */
