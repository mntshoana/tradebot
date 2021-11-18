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
        size_t last = 0;
        
        // erase spaces
        res.erase(remove( res.begin(), res.end(), ' ' ),res.end());
        
        // label
        keyInfo.label =  extractNextString(res, last, last);;
        
        // permissions
        last = res.find("permissions", last);
        std::string permissions = extractNextStringBlock(res, last, "[", "]", last);
        
        // addedAt
        keyInfo.createdAt = extractNextString(res, last, last);
        
        // is sub account
        std::string token = extractNextString(res, last, "}", last);
        keyInfo.isSubAccount = (token == "true") ? true : false;
        
        // allowed Ip Address
        keyInfo.allowedIP = extractNextString(res, last, last);
        
        // allowed Withdraw Address List
        std::string allowedWithdrawlist;
        last = res.find("allowedWithdrawAddressList", last);
        if (last != std::string::npos)
            allowedWithdrawlist = extractNextStringBlock(res, last, "[", "]", last);
        
        last = 0;
        int count = 0;
        while ((last = permissions.find("\"", last)) != std::string::npos) {
            token = extractNextStringBlock(permissions, last, "\"", "\"", last);
            
            keyInfo.permission += ((count++) ? ", ": "") +  token;
            
            if (last == permissions.length() || last == std::string::npos )
                break;
        }
        permissions.erase();
        
        last = 0;
        count = 0;
        while ((last = allowedWithdrawlist.find("{", last)) != std::string::npos) {
            // form "currency: address\n..."
            keyInfo.allowedWithdraw += ((count++) ? "\n" : "");
            
            // currency (but preffer asset)
            std::string asset = extractNextString(res, last, last);
            
            // address
            token = extractNextString(res, last, last);
            keyInfo.allowedWithdraw += asset + ": " + token;
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
        size_t last = 0;
        
        // erase spaces
        res.erase(remove( res.begin(), res.end(), ' ' ),res.end());
        std::string token;
        while ((last = res.find("{", last)) != std::string::npos) {
            Account acc;
        
            // volume
            acc.label = extractNextString(res, last, last);
            
            // price
            token = extractNextString(res, last, last);
            acc.id = atoll(token.c_str());
            
            subAccounts.push_back(acc);
        }
        
        return subAccounts;
    }

    // CREATE SUBACCOUNT
    //
    // Can only be called by a primary account API key.
    // Requires valid charactors only, ie, no periods .
    std::string VALRClient::createSubAccount(std::string label){
        std::string path = "/v1/account/subaccount";
        std::string body = "{";
        body += "\n\t" + createJSONlabel("label", label);
        body += "\n" "}";
        
        std::string res = client.request("POST", (host+path).c_str(), true, VALR_EXCHANGE, path.c_str(),
            body.c_str()
        );
        
        int httpCode = client.getHttpCode();
        if (httpCode != 200)
            throw ResponseEx("Error " + std::to_string(httpCode) + " - " + res);
        
        // erase spaces
        res.erase(remove( res.begin(), res.end(), ' ' ),res.end());
        
        std::string subAccID = extractNextString(res, 0);

        return subAccID;
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
        size_t last = 0;
        // erase spaces
        res.erase(remove( res.begin(), res.end(), ' ' ),res.end());
       
        last = res.find("[", last) + 1;
        
        while ((last = res.find("{", last)) != std::string::npos) {
            AccountSummary summary;
            
            // Accounts
            last = res.find(":", last) + 2; // get to colon marking "accounts:"
            //account:label
            summary.account.label = extractNextString(res, last, last);
            //account:id
            std::string token = extractNextString(res, last, last);
            summary.account.id = atoll(token.c_str());
            
            //balance
            last = res.find("balance", last);
            std::string balanceList = extractNextStringBlock(res, last, "[", "]", last);
            
            size_t tempLast = 0;
            while ((tempLast = balanceList.find("{", tempLast)) != std::string::npos) {
                Balance balance;
                
                //balance:currency
                balance.asset  =  extractNextString(balanceList, tempLast, tempLast);
                
                //balance:available
                token = extractNextString(balanceList, tempLast, tempLast);
                balance.available = atof(token.c_str());
                
                //balance:reserved
                token = extractNextString(balanceList, tempLast, tempLast);
                balance.reserved = atof(token.c_str());
                
                //balance:balance
                token = extractNextString(balanceList, tempLast, tempLast);
                balance.balance = atof(token.c_str());
                
                //balance:last Updated
                token = extractNextString(balanceList, tempLast, tempLast);
                balance.lastUpdated = get_seconds_since_epoch(token);
                
                summary.balances.push_back(balance);
            }

            accounts.push_back(summary);
        }
        return accounts;
    }


    // INTERNAL TRANSFER
    //
    // Transfers an amount of one asset from one subaccount to another
    // primary key can transfer to/from any subaccount.
    // subaccount key can transfer only from itself.
    void VALRClient::internalTransfer(std::string fromID, std::string toID, std::string asset, float amount){
        std::string path = "/v1/account/subaccounts/transfer";
        std::string marshalledBody = "{";
        marshalledBody += "\n\t" + createJSONlabel("fromId", fromID) + ",";
        marshalledBody += "\n\t" + createJSONlabel("toId", toID) + ",";
        marshalledBody += "\n\t" + createJSONlabel("currencyCode", asset) + ",";
        marshalledBody += "\n\t" + createJSONlabel("amount", std::to_string(amount)) + "\n";
        marshalledBody +=  "\n" "}";

        std::string res = client.request("POST", (host+path).c_str(), true, VALR_EXCHANGE, path.c_str(), marshalledBody.c_str());
        
        int httpCode = client.getHttpCode();
        if (httpCode != 200)
            throw ResponseEx("Error " + std::to_string(httpCode) + " - " + res);
        
        // expecting no response message on success
        return;
    }

    // GET BALANCE
    //
    // returns entire list of balances
    std::vector<Balance> VALRClient::getBalances(){
        std::string path = "/v1/account/balances";
        
        std::string res = client.request("GET", (host+path).c_str(), true, VALR_EXCHANGE, path.c_str());
        
        int httpCode = client.getHttpCode();
        if (httpCode != 200)
            throw ResponseEx("Error " + std::to_string(httpCode) + " - " + res);
        
        std::vector<Balance> balances;
        size_t last = 0;
        // erase spaces
        res.erase(remove( res.begin(), res.end(), ' ' ),res.end());
       
        last = res.find("[", last) + 1;
        
        while ((last = res.find("{", last)) != std::string::npos) {
            
            Balance balance;
            
            //balance:currency
            balance.asset  =  extractNextString(res, last, last);
            
            //balance:available
            std::string token = extractNextString(res, last, last);
            balance.available = atof(token.c_str());
            
            //balance:reserved
            token = extractNextString(res, last, last);
            balance.reserved = atof(token.c_str());
            
            //balance:balance
            token = extractNextString(res, last, last);
            balance.balance = atof(token.c_str());
            
            //balance:last Updated
            token = extractNextString(res, last, last);
            balance.lastUpdated = get_seconds_since_epoch(token);
            
            balances.push_back(balance);
        }
        
        return balances;
    }

    
 /*
 // create account
 // update account
 // list pending transaction
 // list transaction
*/
}
