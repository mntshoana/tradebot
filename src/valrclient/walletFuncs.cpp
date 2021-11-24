#include "valrclient.hpp"

extern Client client;

namespace VALR {
    extern std::string host;
    
    // GET ADDRESS (Crypto)
    // The default deposit address associated with currency specified in the path variable
    std::string  VALRClient::getAddress(std::string asset){
        std::string path = "/v1/wallet/crypto/" + asset + "/deposit/address";
        
        std::string res = client.request("GET", (host+path).c_str(), true, VALR_EXCHANGE, path.c_str());
        
        int httpCode = client.getHttpCode();
        if (httpCode != 200)
            throw ResponseEx("Error " + std::to_string(httpCode) + " - " + res);
        
        size_t last = 0;
        // erase spaces
        res.erase(remove( res.begin(), res.end(), ' ' ),res.end());
            
        //currency
        std::string resultedAsset  =  extractNextString(res, last, last);
            
        //address
        std::string address = extractNextString(res, last, last);
        
        return address;
    }
    
    // GET WITHDRAWAL ADDRESS ENTRIES
    // Returns all the withdrawal addresses whitelisted for the account
    // Able to filter for a particular asset/currency
    std::vector<AddressEntry> VALRClient::getWithdrawalAddressEntries(std::string asset){
        std::string path = "/v1/wallet/crypto/address-book";
        if (asset != ""){
            path += "/" + asset;
        }
        std::string res = client.request("GET", (host+path).c_str(), true, VALR_EXCHANGE, path.c_str());
        
        int httpCode = client.getHttpCode();
        if (httpCode != 200)
            throw ResponseEx("Error " + std::to_string(httpCode) + " - " + res);
        
        std::vector<AddressEntry> whitelistedAddresses;
        size_t last = 0;
        
        // erase spaces
        res.erase(remove( res.begin(), res.end(), ' ' ),res.end());
        std::string token;
        while ((last = res.find("{", last)) != std::string::npos) {
            AddressEntry entry;
        
            // id
            entry.id = extractNextString(res, last, last);
            
            // label
            entry.label = extractNextString(res, last, last);
            
            // currency
            entry.asset = extractNextString(res, last, last);
            
            // address
            entry.address    = extractNextString(res, last, last);
            
            // createdAt
            entry.timestamp = extractNextString(res, last, last);
            whitelistedAddresses.push_back(entry);
        }
        
        return whitelistedAddresses;
    }

    // GET WITHDRAWAL INFO
    // Information about withdrawing a given currency from your account.
    //
    WithdrawalDetail VALRClient::getWithdrawalInfo(std::string asset){
        std::string path = "/v1/wallet/crypto/"+ asset +"/withdraw";
        std::string res = client.request("GET", (host+path).c_str(), true, VALR_EXCHANGE, path.c_str());
        
        int httpCode = client.getHttpCode();
        if (httpCode != 200)
            throw ResponseEx("Error " + std::to_string(httpCode) + " - " + res);
        
        size_t last = 0;
        
        // erase spaces
        res.erase(remove( res.begin(), res.end(), ' ' ),res.end());
        std::string token;
        WithdrawalDetail info;
    
        // currency
        info.asset = extractNextString(res, last, last);
        
        // minimumWithdrawAmount
        token = extractNextString(res, last, last);
        info.min = atof(token.c_str());
        
        // withdrawalDecimalPlaces
        token = extractNextString(res, last, last);
        info.decimals = atoi(token.c_str());
        
        // isActive
        token = extractNextString(res, last, last);
        info.isActive = (token == "true" ? true : false);

        /*
         "withdrawCost": "0.00013",
         "supportsPaymentReference": false*/
        
        // withdrawCost
        token = extractNextString(res, last, last);
        info.fee = atof(token.c_str());
        
        // supportsPaymentReference
        token = extractNextString(res, last, last);
        info.supportsReference = (token == "true" ? true : false);
    
        return info;
    }
}
/*
// create account
// update account
// list pending transaction
*/
