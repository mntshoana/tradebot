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

    // TRANSACTION HISTORY (for your account)
    //
    // Returns 10 trades (max 200). Able to skip a number of trades from list or restrict history to show those which were made before a given tradeId
    // Able to filter using:
    // transactionType LIMIT_BUY,LIMIT_SELL,MARKET_BUY,MARKET_SELL,SIMPLE_BUY,SIMPLE_SELL,AUTO_BUY,MAKER_REWARD,BLOCKCHAIN_RECEIVE,BLOCKCHAIN_SEND,FIAT_DEPOSIT,FIAT_WITHDRAWAL,REFERRAL_REBATE,REFERRAL_REWARD,PROMOTIONAL_REBATE,INTERNAL_TRANSFER,FIAT_WITHDRAWAL_REVERSAL,PAYMENT_SENT,PAYMENT_RECEIVED,PAYMENT_REVERSED,PAYMENT_REWARD,OFF_CHAIN_BLOCKCHAIN_WITHDRAW,OFF_CHAIN_BLOCKCHAIN_DEPOSIT,AUTO_BUY,SIMPLE_SWAP_BUY,SIMPLE_SWAP_SELL,SIMPLE_SWAP_FAILURE_REVERSAL
    // currency (the creditCurrency or the debitCurrency matches this parameter)
    // startTime and startTime (ISO 8601 strings)
    std::vector<TransactionInfo> VALR::VALRClient::getTransactionHistory(std::string asset, std::string transactionType, int skip, int limit, std::string startT, std::string endT, std::string beforeID){
        std::string path = "/v1/account/transactionhistory";
        int args = 0;
        if (asset != ""){
            path += (args++ ? "&" : "?");
            path += "currency=" + asset;
        }
        if (transactionType != ""){
            path += (args++ ? "&" : "?");
            path += "transactionTypes=" + transactionType;
        }
        if (skip != 0){
            path += (args++ ? "&" : "?");
            path += "skip=" + std::to_string(skip);
        }
        if (limit != 0){
            path += (args++ ? "&" : "?");
            path += "limit=" + std::to_string(limit);
        }
        if (startT != ""){
            path += (args++ ? "&" : "?");
            path += "startTime=" + startT;
        }
        if (endT != ""){
            path += (args++ ? "&" : "?");
            path += "endTime=" + endT;
        }
        if (endT != ""){
            path += (args++ ? "&" : "?");
            path += "beforeId=" + beforeID;
        }

        std::string res = client.request("GET", (host+path).c_str(), true, VALR_EXCHANGE, path.c_str());
        int httpCode = client.getHttpCode();
        if (httpCode != 200)
            throw ResponseEx("Error " + std::to_string(httpCode) + " - " + res);
        
        std::vector<TransactionInfo> history;
        size_t last = 0;
        
        // erase spaces
        res.erase(remove( res.begin(), res.end(), ' ' ),res.end());
        
        
        while ((last = res.find("{", last)) != std::string::npos) {
            // Seperate the json data first
            
            last = res.find("transactionType", last) + 1;
            std::string type = extractNextStringBlock(res, last, "{", "}", last);

            std::string theRest, additionalInfo, transaID;
            
            for (size_t index = last; index < res.size(); index++){
                if (res[index] == '{'){
                    // means it contains more additional info.
                    // NOTE: "additionalInfo" == 16 characters
                    
                    // first take everything between transaction type and aditional info
                    theRest = res.substr(last, index - last-1 - 16);
                    // then, separate Additional Info
                    additionalInfo = extractNextStringBlock(res, index, "{", "}", last);
                    // Lastly, take the ID too
                    transaID = extractNextString(res, last, "}", last);
                    break;
                }
                else if (res[index] == '}'){
                    // means no additional info, just take everything as is
                    theRest = res.substr(last, index - last -1);
                    last += index + 1;
                    break;
                }
                else continue;
            }
            // now sort through the current data (unmarshal as normal)
            std::string token;
            TransactionInfo transaction;
            size_t tempPos = 0;

            transaction.type = extractNextString(type, tempPos, tempPos);
            transaction.description = extractNextString(type, tempPos, tempPos);
            
            tempPos = theRest.find("debitCurrency", 0);
            if (tempPos != std::string::npos){
                transaction.debitAsset = extractNextString(theRest, tempPos, tempPos);
                token = extractNextString(theRest, tempPos, tempPos);
                transaction.debitValue = atof(token.c_str());
            }
            
            tempPos = theRest.find("creditCurrency", 0);
            if (tempPos != std::string::npos){
                transaction.creditAsset = extractNextString(theRest, tempPos, tempPos);
                token = extractNextString(theRest, tempPos, tempPos);
                transaction.creditValue = atof(token.c_str());
            }
            
            tempPos = theRest.find("feeCurrency", 0);
            if (tempPos != std::string::npos){
                transaction.feeAsset = extractNextString(theRest, tempPos, tempPos);
                token = extractNextString(theRest, tempPos, tempPos);
                transaction.feeValue = atof(token.c_str());
            }
            
            tempPos = theRest.find("eventAt", 0);
            token = extractNextString(theRest, tempPos, tempPos);
            transaction.timestamp = get_seconds_since_epoch(token);
            
            transaction.additionalInfo = additionalInfo;
            
            if (transaID != "")
                transaction.id = transaID;
            else
                transaction.id = extractNextString(theRest, tempPos, tempPos);
            
            history.push_back(transaction);
        }
        
        return history;
    }

    // USER TRADES
    // Lists the trade history executed by the user
    //
    std::vector<UserTrade> VALR::VALRClient::getUserTrades(std::string pair,  int limit){
        std::string path = "/v1/account/" + pair + "/tradehistory";
        if (limit != 0){
            path += "?limit=" + std::to_string(limit);
        }
        
        std::string res = client.request("GET", (host+path).c_str(), true, VALR_EXCHANGE, path.c_str());
        
        int httpCode = client.getHttpCode();
        if (httpCode != 200)
            throw ResponseEx("Error " + std::to_string(httpCode) + " - " + res);
        
        std::vector<UserTrade> trades;
        size_t last = 0;
        last = res.find("[", last) + 1;
        
        // erase spaces
        res.erase(remove( res.begin(), res.end(), ' ' ),res.end());
        
        while ((last = res.find("{", last)) != std::string::npos) {
            UserTrade trade;

            // price
            std::string token = extractNextString(res, last, last);
            trade.price = atof(token.c_str());
            
            // volume (quantity)
            token = extractNextString(res, last, last);
            trade.baseVolume = atof(token.c_str());
            
            // pair
            token = extractNextString(res, last, last);
            trade.pair = token;
            
            // timestamp
            token = extractNextString(res, last, last);
            trade.timestamp = get_seconds_since_epoch(token);
            
            // isBuy
            token = extractNextString(res, last, last);
            trade.isBuy = (token == "buy") ? true : false;
            
            // sequenceID
            token = extractNextString(res, last, ",", last);
            trade.sequence = atoll(token.c_str());
            
            // id
            trade.id = extractNextString(res, last, last);
            
            // orderId
            trade.orderID = extractNextString(res, last, last);
            
            // qupte volume
            trade.quoteVolume = trade.baseVolume * trade.price;
            
            trades.push_back(trade);
        }
        
        return trades;
    }
}
