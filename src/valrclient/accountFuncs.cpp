#include "valrclient.hpp"

extern Client client;

namespace VALR {
    extern std::string host;
    
    // GET CURRENT KEY INFO
    //
    // Returns API Key's information and permissions from the server
    // Will never reveal API KEYS and PASSWORD
    KeyInfo VALRClient::getCurrentKeyInfo(){
        std::string path = "/v1/account/api-keys/current";
        
        std::string res = client.request("GET", (host+path).c_str(), true, VALR_EXCHANGE, path.c_str());
        
        int httpCode = client.getHttpCode();
        if (httpCode != 200)
            throw ResponseEx("Error " + std::to_string(httpCode) + " - " + res);
        
        KeyInfo keyInfo;
        size_t last = 0, next = 0;
        
        // erase spaces
        res.erase(remove( res.begin(), res.end(), ' ' ),res.end());
        
        // label
        last = res.find(":", last) + 2;
        next = res.find("\"", last);
        std::string token = res.substr(last, next-last);
        keyInfo.label = token;
        last = next + 1;
        
        // permissions
        last = res.find("permissions", last);
        last = res.find("[", last) + 1;
        next = res.find("]", last);
        std::string permissions = res.substr(last, next-last);
        last = next + 1;
        
        // addedAt
        last = res.find(":", last) + 2;
        next = res.find("\"", last);
        token = res.substr(last, next-last);
        keyInfo.createdAt = token;
        last = next + 1;
        
        // is sub account
        last = res.find(":", last) + 1;
        next = res.find("}", last);
        token = res.substr(last, next-last);
        keyInfo.isSubAccount = (token == "true") ? true : false;;
        last = next + 1;
        
        // allowed Ip AddressCidr
        last = res.find(":", last);
        if (last != std::string::npos){
            last += 2;
            next = res.find("\"", last);
            token = res.substr(last, next-last);
            keyInfo.allowedIP = token;
            last = next + 1;
        }
        
        // allowed Withdraw Address List
        std::string allowedWithdrawlist;
        last = res.find("allowedWithdrawAddressList", last);
        if (last != std::string::npos){
            
            last = res.find("[", last) + 1;
            next = res.find("]", last);
            allowedWithdrawlist = res.substr(last, next-last);
            last = next + 1;
        }
        
        last = 0;
        int count = 0;
        while ((last = permissions.find("\"", last)) != std::string::npos) {
            last += 1; // ignore " character
            next = permissions.find("\"", last+1);
            keyInfo.permission += ((count++) ? ", ": "")
                                +  permissions.substr(last, next-last);
            last = next + 1;
            
            if (last == permissions.length() || last == std::string::npos )
                break;
        }
        permissions.erase();
        
        last = 0;
        count = 0;
        while ((last = allowedWithdrawlist.find("{", last)) != std::string::npos) {
            keyInfo.allowedWithdraw += ((count++) ? "\n": "");
            // currency
            last = allowedWithdrawlist.find(":", last);
            next = allowedWithdrawlist.find("\"", last);
            token = res.substr(last, next-last);
            keyInfo.allowedIP = token;
            last = next + 1;
            
            // address
            last = allowedWithdrawlist.find(":", last);
            next = allowedWithdrawlist.find("\"", last);
            token += ": " + res.substr(last, next-last);
            keyInfo.allowedWithdraw += token;
            last = next + 1;
        }
        allowedWithdrawlist.erase();
        
        return keyInfo;
    }

    // GET SUBACCOUNT  INFO
    //
    // Can only be called by a primary account API key.
    std::vector<Account> VALRClient::getSubAccounts(){
        std::string path = "/v1/account/subaccounts";
        
        std::string res = client.request("GET", (host+path).c_str(), true, VALR_EXCHANGE, path.c_str());
        
        int httpCode = client.getHttpCode();
        if (httpCode != 200)
            throw ResponseEx("Error " + std::to_string(httpCode) + " - " + res);
        
        std::vector<Account> subAccounts;
        size_t last = 0, next = 0;
        
        // erase spaces
        res.erase(remove( res.begin(), res.end(), ' ' ),res.end());
        std::string token;
        while ((last = res.find("{", last)) != std::string::npos) {
            Account acc;
        
            // volume
            last = res.find(":", last) + 2;
            next = res.find("\"", last);
            token = res.substr(last, next-last);
            acc.label = token;
            last = next + 1;
            
            // price
            last = res.find(":", last) + 2;
            next = res.find("\"", last);
            token = res.substr(last, next-last);
            acc.id = atoll(token.c_str());
            last = next + 1;
            
            subAccounts.push_back(acc);
        }
        
        return subAccounts;
    }


    // GET NON-ZERO BALANCES
    //
    //
    std::vector<AccountSummary> VALRClient::getNonZeroBalances(){
        std::string path = "/v1/account/balances/all";
        
        std::string res = client.request("GET", (host+path).c_str(), true, VALR_EXCHANGE, path.c_str());
        
        int httpCode = client.getHttpCode();
        if (httpCode != 200)
            throw ResponseEx("Error " + std::to_string(httpCode) + " - " + res);
        
        std::vector<AccountSummary> accounts;
        size_t last = 0, next = 0;
        // erase spaces
        res.erase(remove( res.begin(), res.end(), ' ' ),res.end());
       
        last = res.find("[", last) + 1;
        
        while ((last = res.find("{", last)) != std::string::npos) {
            AccountSummary summary;
            
            // Accounts
            last = res.find(":", last) + 2; // get to colon "accounts:"
            //account:label
            last = res.find(":", last) + 2; // get to colon "label:"
            next = res.find("\"", last);
            summary.account.label = res.substr(last, next-last);
            last = next + 1;
            //account:id
            last = res.find(":", last) + 2;
            next = res.find("\"", last);
            std::string token = res.substr(last, next-last);
            summary.account.id = atoll(token.c_str());
            last = next + 1;
            
            //balance
            last = res.find("balance", last);
            last = res.find("[", last) + 1;
            next = res.find("]", last);
            
            std::string balanceList = res.substr(last, next-last);
            size_t tempLast = 0, tempNext = 0;
            while ((tempLast = balanceList.find("{", tempLast)) != std::string::npos) {
                Balance balance;
                
                //balance:currency
                tempLast = balanceList.find(":", tempLast) + 2;
                tempNext = balanceList.find("\"", tempLast);
                token = balanceList.substr(tempLast, tempNext-tempLast);
                balance.asset = token;
                tempLast = next + 1;
                
                // skip available
                tempLast = balanceList.find(":", tempLast) + 2;
                tempNext = balanceList.find("\"", tempLast);
                token = balanceList.substr(tempLast, tempNext-tempLast);
                balance.available = atof(token.c_str());
                tempLast = next + 1;
                
                //balance:reserved
                tempLast = balanceList.find(":", tempLast) + 2;
                tempNext = balanceList.find("\"", tempLast);
                token = balanceList.substr(tempLast, tempNext-tempLast);
                balance.reserved = atof(token.c_str());
                tempLast = next + 1;
                
                //balance:balance
                tempLast = balanceList.find(":", tempLast) + 2;
                tempNext = balanceList.find("\"", tempLast);
                token = balanceList.substr(tempLast, tempNext-tempLast);
                balance.balance = atof(token.c_str());
                tempLast = next + 1;
                
                //balance:last Updated
                tempLast = balanceList.find(":", tempLast) + 2;
                tempNext = balanceList.find("\"", tempLast);
                token = balanceList.substr(tempLast, tempNext-tempLast);
                balance.lastUpdated = get_seconds_since_epoch(token);
                balanceList = tempNext + 1;
                
                summary.balances.push_back(balance);
            }
            last = next + 1;

            accounts.push_back(summary);
        }
        return accounts;
    }
/*
    // GET BALANCE
    //
    // assets can list multiple comma separated assets
    std::vector<Balance> VALRClient::getBalances(std::string assets){
        std::string uri = "https://api.mybitx.com/api/1/balance";
        if (assets != "")
            uri += "?assets=" + assets;
        std::string res = client.request("GET", uri.c_str(), true);
        
        int httpCode = client.getHttpCode();
        if (httpCode != 200)
            throw ResponseEx("Error " + std::to_string(httpCode) + " - " + res);
        
        std::vector<Balance> balances;
        size_t last = 0, next = 0;
        last = res.find("[", last) + 1;
        
        while ((last = res.find("{", last)) != std::string::npos) {
            balances.push_back(Balance());
            
            //account_id
            last = res.find("account_id", last);
            last = res.find(":", last) + 2;
            next = res.find("\"", last);
            balances.back().accountID = res.substr(last, next-last);
            last = next + 1;
            
            //asset
            last = res.find("asset", last);
            last = res.find(":", last) + 2;
            next = res.find("\"", last);
            balances.back().asset = res.substr(last, next-last);
            last = next + 1;
            
            //balance
            last = res.find("balance", last);
            last = res.find(":", last) + 2;
            next = res.find("\"", last);
            std::string token = res.substr(last, next-last);
            balances.back().balance = atof(token.c_str());
            last = next + 1;
            
            //reserved
            last = res.find("reserved", last);
            last = res.find(":", last) + 2;
            next = res.find("\"", last);
            token = res.substr(last, next-last);
            balances.back().reserved = atof(token.c_str());
            last = next + 1;
            
            //unconfirmed
            last = res.find("unconfirmed", last);
            last = res.find(":", last) + 2;
            next = res.find("\"", last);
            token = res.substr(last, next-last);
            balances.back().uncomfirmed = atof(token.c_str());
            last = next + 1;
        }
        return balances;
    }

    template <class T> T& operator << (T& stream, Balance& balance) {
        std::stringstream ss;
        ss << "ID: : " << balance.accountID << "\n";
        ss << "Asset: " << balance.asset << "\n";
        ss << "Balance: " << balance.balance << "\n";
        ss << "Reserved: " << balance.reserved << "\n";
        ss << "Unconfirmed: " << balance.uncomfirmed << "\n";
        stream.append(ss.str().c_str());
        return stream;
    }

    
 
 // create account
 // update account
 // list pending transaction
 // list transaction
*/
}
