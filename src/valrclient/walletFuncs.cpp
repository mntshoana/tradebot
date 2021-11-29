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
    
    // GET WITHDRAWAL ADDRESS ENTRIES (Crypto)
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

    // GET WITHDRAWAL INFO (Crypto)
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
        token = extractNextString(res, last, ",", last);
        info.isActive = (token == "true" ? true : false);
        
        // withdrawCost
        token = extractNextString(res, last, last);
        info.fee = atof(token.c_str());
        
        // supportsPaymentReference
        token = extractNextString(res, last, ",", last);
        info.supportsReference = (token == "true" ? true : false);
    
        return info;
    }
    
    // Withdraw (Crypto)
    // This will withdraw a cryptocurrency amount to an address.
    std::string VALRClient::withdraw( std::string asset, float amount, std::string address){
        std::string path = "/v1/wallet/crypto/" + asset + "/withdraw";
        std::string payload = "{";
        payload += "\n\t" + createJSONlabel("amount", std::to_string(amount)) + ",";
        payload += "\n\t" + createJSONlabel("address", address) ;
        payload +=  "\n" "}";

        std::string res = client.request("POST", (host+path).c_str(), true, VALR_EXCHANGE, path.c_str(), payload.c_str());
        
        int httpCode = client.getHttpCode();
        if (httpCode != 200)
            throw ResponseEx("Error " + std::to_string(httpCode) + " - " + res);
        
        std::string id = extractNextString(res, 0);
        
        return id;
    }

    // GET WITHDRAWAL BY ID (Crypto)
    // Information about a given currency withdrawal from your account using a known ID.
    //
    WithdrawalInfo VALRClient::getWithdrawalByID(std::string asset, std::string id){
        std::string path = "/v1/wallet/crypto/" + asset + "/withdraw/" + id;
        std::string res = client.request("GET", (host+path).c_str(), true, VALR_EXCHANGE, path.c_str());
        
        int httpCode = client.getHttpCode();
        if (httpCode != 200)
            throw ResponseEx("Error " + std::to_string(httpCode) + " - " + res);
        
        size_t last = 0;
        
        // erase spaces
        res.erase(remove( res.begin(), res.end(), ' ' ),res.end());
        std::string token;
        WithdrawalInfo info;

        // currency
        info.asset = extractNextString(res, last, last);
        
        // address
        info.address = extractNextString(res, last, last);
        
        // amount
        token = extractNextString(res, last, last);
        info.amount = atof(token.c_str());
        
        // feeAmount
        token = extractNextString(res, last, last);
        info.fee = atof(token.c_str());
        
        // transactionHash
        info.transactionHash = extractNextString(res, last, last);
        
        // confirmations
        token = extractNextString(res, last, ",", last);
        info.confirmations = atoi(token.c_str());
        
        // lastConfirmationAt
        info.lastConfrimationAt = extractNextString(res, last, last);
        
        // uniqueId
        info.id = extractNextString(res, last, last);
        
        // createdAt
        info.timestamp = extractNextString(res, last, last);
        
        // verified
        token = extractNextString(res, last, last);
        info.isVerified = (token == "true" ? true : false);
        
        // status
        info.status = extractNextString(res, last, last);

        return info;
    }

    // DEPOSIT HISTORY (Crypto)
    // the deposit history records for a given crypto currency
    //
    std::vector<DepositInfo> VALRClient::getDepositHistory(std::string asset, int skip, int limit){
        std::string path = "/v1/wallet/crypto/" + asset + "/deposit/history";
        int args = 0;
        if (skip != 0){
            path += (args++ ? "&" : "?");
            path += "skip=" + std::to_string(skip);
        }
        if (limit != 0){
            path += (args++ ? "&" : "?");
            path += "limit=" + std::to_string(limit);
        }
        std::string res = client.request("GET", (host+path).c_str(), true, VALR_EXCHANGE, path.c_str());
        
        int httpCode = client.getHttpCode();
        if (httpCode != 200)
            throw ResponseEx("Error " + std::to_string(httpCode) + " - " + res);
        
        size_t last = 0;
        
        // erase spaces
        res.erase(remove( res.begin(), res.end(), ' ' ),res.end());
        std::string token;
        std::vector<DepositInfo> list;
        
        while ((last = res.find("{", last)) != std::string::npos) {
            DepositInfo info;

            // currencyCode
            info.asset = extractNextString(res, last, last);
            
            // receiveAddress
            info.address = extractNextString(res, last, last);
            
            // transactionHash
            info.transactionHash = extractNextString(res, last, last);
            
            // amount
            token = extractNextString(res, last, last);
            info.amount = atof(token.c_str());
            
            // No fee
            info.fee = 0.0f;
            
            // createdAt
            info.timestamp = extractNextString(res, last, last);
            
            // confirmations
            token = extractNextString(res, last, last);
            info.confirmations = atoi(token.c_str());
           
            // confirmed
            token = extractNextString(res, last, ",",  last);
            info.isVerified = (token == "true" ? true : false);
            
            // lastConfirmationAt
            info.lastConfrimationAt = extractNextString(res, last, last);
            
            // no ID
            info.id = "";
            
            // no status
            info.status = "";
            
            list.push_back(info);
        }
        return list;
    }

    // WITHDRAW HISTORY (Crypto)
    // the withdrawaL history records for a given crypto currency
    //
    std::vector<WithdrawalInfo> VALRClient::getWithdrawalHistory(std::string asset, int skip, int limit){
        std::string path = "/v1/wallet/crypto/" + asset + "/withdraw/history";
        int args = 0;
        if (skip != 0){
            path += (args++ ? "&" : "?");
            path += "skip=" + std::to_string(skip);
        }
        if (limit != 0){
            path += (args++ ? "&" : "?");
            path += "limit=" + std::to_string(limit);
        }
        std::string res = client.request("GET", (host+path).c_str(), true, VALR_EXCHANGE, path.c_str());
        
        int httpCode = client.getHttpCode();
        if (httpCode != 200)
            throw ResponseEx("Error " + std::to_string(httpCode) + " - " + res);
        
        size_t last = 0;
        
        // erase spaces
        res.erase(remove( res.begin(), res.end(), ' ' ),res.end());
        std::string token;
        std::vector<WithdrawalInfo> list;
        
        while ((last = res.find("{", last)) != std::string::npos) {
            WithdrawalInfo info;
            
            // currency
            info.asset = extractNextString(res, last, last);
            
            // address
            info.address = extractNextString(res, last, last);
            
            // amount
            token = extractNextString(res, last, last);
            info.amount = atof(token.c_str());
            
            // feeAmount
            token = extractNextString(res, last, last);
            info.fee = atof(token.c_str());
            
            // transactionHash
            info.transactionHash = extractNextString(res, last, last);
            
            // confirmations
            token = extractNextString(res, last, ",", last);
            info.confirmations = atoi(token.c_str());
            
            // lastConfirmationAt
            info.lastConfrimationAt = extractNextString(res, last, last);
            
            // uniqueId
            info.id = extractNextString(res, last, last);
            
            // createdAt
            info.timestamp = extractNextString(res, last, last);
            
            // verified
            token = extractNextString(res, last, last);
            info.isVerified = (token == "true" ? true : false);
            
            // status
            info.status = extractNextString(res, last, last);
            
            list.push_back(info);
        }
        return list;
    }

    // WITHDRAWAL BANK (FIAT)
    // lists bank accounts linked to your account.
    //
    std::vector<BankInfo> VALRClient::getBankAccounts(std::string asset){
        std::string path = "/v1/wallet/fiat/" + asset + "/accounts";
        
        std::string res = client.request("GET", (host+path).c_str(), true, VALR_EXCHANGE, path.c_str());
        
        int httpCode = client.getHttpCode();
        if (httpCode != 200)
            throw ResponseEx("Error " + std::to_string(httpCode) + " - " + res);
        
        size_t last = 0;
        
        // erase spaces
        res.erase(remove( res.begin(), res.end(), ' ' ),res.end());
        std::string token;
        std::vector<BankInfo> list;
        
        while ((last = res.find("{", last)) != std::string::npos) {
            BankInfo account;
            
            // id
            account.id = extractNextString(res, last, last);
            
            // bank
            account.bank = extractNextString(res, last, last);
            
            // accountHolder
            account.accountHolder = extractNextString(res, last, last);

            // accountNumber
            account.accountNumber = extractNextString(res, last, last);
            
            // branchCode
            account.branchCode = extractNextString(res, last, last);
            
            // accountType
            account.accountType = extractNextString(res, last, last);
           
            // createdAt
            account.timestamp = extractNextString(res, last, last);

            list.push_back(account);
        }
        return list;
    }
}
/*
// create account
// update account
// list pending transaction
*/
