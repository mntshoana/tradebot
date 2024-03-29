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
        
        // label
        keyInfo.label =  extractNextString(res, last, last, "label");
        
        // permissions
        std::string permissions = extractNextStringBlock(res, last, "[", "]", last, "permissions");
        
        // addedAt
        keyInfo.createdAt = extractNextString(res, last, last, "addedAt");
        
        // is sub account
        std::string token = extractNextString(res, last, "}", last, "isSubAccount");
        keyInfo.isSubAccount = (token == "true") ? true : false;
        
        // allowed Ip Address
        keyInfo.allowedIP = extractNextString(res, last, last, "allowedIpAddressCidr");
        
        // allowed Withdraw Address List
        std::string allowedWithdrawlist;
        last = res.find("allowedWithdrawAddressList", last);
        if (last != std::string::npos)
            allowedWithdrawlist = extractNextStringBlock(res, last, "[", "]", last, "allowedWithdrawAddressList");
        
        last = 0;
        int count = 0;
        while ((last = permissions.find("\"", last)) != std::string::npos) {
            token = extractNextStringBlock(permissions, last, "\"", "\"", last, nullptr);
            
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
            std::string asset = extractNextString(res, last, last, "currency");
            
            // address
            token = extractNextString(res, last, last, "address");
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
        
        std::string token;
        while ((last = res.find("{", last)) != std::string::npos) {
            Account acc;
        
            // volume
            acc.label = extractNextString(res, last, last, "volume");
            
            // price
            token = extractNextString(res, last, last, "price");
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
        
        std::string subAccID = extractNextString(res, 0, "id");

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
       
        last = res.find("[", last) + 1;
        
        while ((last = res.find("{", last)) != std::string::npos) {
            AccountSummary summary;
            
            // Accounts
            last = res.find(":", last) + 2; // get to colon marking "accounts:"
            //account:label
            summary.account.label = extractNextString(res, last, last, "label");
            //account:id
            std::string token = extractNextString(res, last, last, "id");
            summary.account.id = atoll(token.c_str());
            
            // balances
            std::string balanceList = extractNextStringBlock(res, last, "[", "]", last, "balances");
            
            size_t tempLast = 0;
            while ((tempLast = balanceList.find("{", tempLast)) != std::string::npos) {
                Balance balance;
                
                //balance:currency
                balance.asset  =  extractNextString(balanceList, tempLast, tempLast, "currency");
                
                //balance:available
                token = extractNextString(balanceList, tempLast, tempLast, "available");
                balance.available = atof(token.c_str());
                
                //balance:reserved
                token = extractNextString(balanceList, tempLast, tempLast, "reserved");
                balance.reserved = atof(token.c_str());
                
                //balance:balance
                token = extractNextString(balanceList, tempLast, tempLast, "balance");
                balance.balance = atof(token.c_str());
                
                //balance:last Updated
                token = extractNextString(balanceList, tempLast, tempLast, "updatedAt");
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
       
        last = res.find("[", last) + 1;
        
        while ((last = res.find("{", last)) != std::string::npos) {
            
            Balance balance;
            
            //balance:currency
            balance.asset  =  extractNextString(res, last, last, "currency");
            
            //balance:available
            std::string token = extractNextString(res, last, last, "available");
            balance.available = atof(token.c_str());
            
            //balance:reserved
            token = extractNextString(res, last, last, "reserved");
            balance.reserved = atof(token.c_str());
            
            //balance:balance
            token = extractNextString(res, last, last, "balance");
            balance.balance = atof(token.c_str());
            
            //balance:last Updated
            token = extractNextString(res, last, last, "updatedAt");
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
        
        
        while ((last = res.find("{", last)) != std::string::npos) {
            // Seperate the json data first
            
            std::string type = extractNextStringBlock(res, last, "{", "}", last, "transactionType");

            std::string theRest, additionalInfo, transaID;
            
            for (size_t index = last; index < res.size(); index++){
                if (res[index] == '{'){
                    // means it contains more additional info.
                    // NOTE: "additionalInfo" == 16 characters
                    
                    // first take everything between transaction type and aditional info
                    theRest = res.substr(last, index - last-1 - 16);
                    // then, separate Additional Info
                    additionalInfo = extractNextStringBlock(res, index, "{", "}", last, "additionalInfo");
                    // Lastly, take the ID too
                    transaID = extractNextString(res, last, "}", last, "id");
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

            // type
            transaction.type = extractNextString(type, tempPos, tempPos, "type");
            // description
            transaction.description = extractNextString(type, tempPos, tempPos, "description");
            
            tempPos = theRest.find("debitCurrency", 0);
            if (tempPos != std::string::npos){
                // debitCurrency
                transaction.debitAsset = extractNextString(theRest, tempPos, tempPos, "debitCurrency");
                // debitValue
                token = extractNextString(theRest, tempPos, tempPos, "debitValue");
                transaction.debitValue = atof(token.c_str());
            }
            
            tempPos = theRest.find("creditCurrency", 0);
            if (tempPos != std::string::npos){
                transaction.creditAsset = extractNextString(theRest, tempPos, tempPos, "creditCurrency");
                token = extractNextString(theRest, tempPos, tempPos, "creditValue");
                transaction.creditValue = atof(token.c_str());
            }
            
            tempPos = theRest.find("feeCurrency", 0);
            if (tempPos != std::string::npos){
                transaction.feeAsset = extractNextString(theRest, tempPos, tempPos, "feeCurrency");
                token = extractNextString(theRest, tempPos, tempPos, "feeValue");
                transaction.feeValue = atof(token.c_str());
            }
            
            tempPos = theRest.find("eventAt", 0);
            token = extractNextString(theRest, tempPos, tempPos, "eventAt");
            transaction.timestamp = get_seconds_since_epoch(token);
            
            transaction.additionalInfo = additionalInfo;
            
            if (transaID != "")
                transaction.id = transaID;
            else
                transaction.id = extractNextString(theRest, tempPos, tempPos, "id");
            
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
        
        while ((last = res.find("{", last)) != std::string::npos) {
            UserTrade trade;

            // price
            std::string token = extractNextString(res, last, last, "price");
            trade.price = atof(token.c_str());
            
            // volume (quantity)
            token = extractNextString(res, last, last, "quantity");
            trade.baseVolume = atof(token.c_str());
            
            // pair
            token = extractNextString(res, last, last, "currencyPair");
            trade.pair = token;
            
            // timestamp
            token = extractNextString(res, last, last, "tradedAt");
            trade.timestamp = get_seconds_since_epoch(token);
            
            // isBuy
            token = extractNextString(res, last, last, "side");
            trade.isBuy = (token == "buy") ? true : false;
            
            // sequenceID
            token = extractNextString(res, last, ",", last, "sequenceId");
            trade.sequence = atoll(token.c_str());
            
            // id
            trade.id = extractNextString(res, last, last, "id");
            
            // orderId
            trade.orderID = extractNextString(res, last, last, "orderId");
            
            // qupte volume
            trade.quoteVolume = trade.baseVolume * trade.price;
            
            trades.push_back(trade);
        }
        
        return trades;
    }
}
