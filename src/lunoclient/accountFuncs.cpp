#include "lunoclient.hpp"

extern Client client;

namespace Luno {
    // CREATE ACCOUNT
    //
    // Creates an Account for the specified currency.
    // Users can have a limit of 4 accounts per currency.
    std::string LunoClient::createAccount(std::string asset, std::string name){
        std::string uri = "https://api.mybitx.com/api/1/accounts";

        uri += "?currency=" + asset;
        uri += "&name=" + name;
        std::string res = client.request("POST", uri.c_str(), true);
        
        int httpCode = client.getHttpCode();
        if (httpCode != 200)
            throw ResponseEx("Error " + std::to_string(httpCode) + " - " + res);
        
        return res;
        // Result will look something like
        /*
         {
           "currency": "string",
           "id": "string",
           "name": "string",
           "pending": [
             {
               "account_id": "string",
               "available": "string",
               "available_delta": "string",
               "balance": "string",
               "balance_delta": "string",
               "currency": "string",
               "description": "string",
               "detail_fields": {
                 "crypto_details": {
                   "address": "string",
                   "txid": "string"
                 },
                 "trade_details": {
                   "pair": "string",
                   "price": "string",
                   "sequence": 0,
                   "volume": "string"
                 }
               },
               "details": {
                 "property1": "string",
                 "property2": "string"
               },
               "kind": null,
               "row_index": 0,
               "timestamp": "string"
             }
           ],
           "transactions": [
             {
               "account_id": "string",
               "available": "string",
               "available_delta": "string",
               "balance": "string",
               "balance_delta": "string",
               "currency": "string",
               "description": "string",
               "detail_fields": {
                 "crypto_details": {
                   "address": "string",
                   "txid": "string"
                 },
                 "trade_details": {
                   "pair": "string",
                   "price": "string",
                   "sequence": 0,
                   "volume": "string"
                 }
               },
               "details": {
                 "property1": "string",
                 "property2": "string"
               },
               "kind": null,
               "row_index": 0,
               "timestamp": "string"
             }
           ]
         }
         */
    }


    // UPDATE ACCOUNT
    //
    // Update the name of an existing account created using createAccount(...)
    // Users can have a limit of 4 accounts per currency.
    bool LunoClient::updateAccount(std::string id, std::string name){
        std::string uri = "https://api.mybitx.com/api/1/accounts/" + id + "/name";
        uri += "?name=" + name;
        
        std::string res = client.request("PUT", uri.c_str(), true);
        
        int httpCode = client.getHttpCode();
        if (httpCode != 200)
            throw ResponseEx("Error " + std::to_string(httpCode) + " - " + res);
        
        
        // success
        std::string token = extractNextString(res, 0, "}");
        return (token == "true" ? true : false);
    }
    
    // list pending transaction
    // list transaction

    // GET BALANCE
    //
    // assets can list multiple comma separated assets
    std::vector<Balance> LunoClient::getBalances(std::string assets){
        std::string uri = "https://api.mybitx.com/api/1/balance";
        if (assets != "")
            uri += "?assets=" + assets;
        std::string res = client.request("GET", uri.c_str(), true);
        
        int httpCode = client.getHttpCode();
        if (httpCode != 200)
            throw ResponseEx("Error " + std::to_string(httpCode) + " - " + res);
        
        std::vector<Balance> balances;
        size_t last = 0;
        last = res.find("[", last) + 1;
        
        while ((last = res.find("{", last)) != std::string::npos) {
            balances.push_back(Balance());
            
            //account_id
            last = res.find("account_id", last);
            balances.back().accountID = extractNextString(res, last, last);

            
            //asset
            last = res.find("asset", last);
            balances.back().asset = extractNextString(res, last, last);
            
            //balance
            last = res.find("balance", last);
            std::string token = extractNextString(res, last, last);
            balances.back().balance = atof(token.c_str());
            
            //reserved
            last = res.find("reserved", last);
            token = extractNextString(res, last, last);
            balances.back().reserved = atof(token.c_str());
            
            //unconfirmed
            last = res.find("unconfirmed", last);
            token = extractNextString(res, last, last);
            balances.back().uncomfirmed = atof(token.c_str());
        }
        return balances;
    }

}
