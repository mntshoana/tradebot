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
        std::string token = extractNextString(res, 0, "}", "success");
        return (token == "true" ? true : false);
    }
    
    std::string getPendingTransactions(std::string id){
        std::string uri = "https://api.mybitx.com/api/1/accounts/" + id + "/pending";
        
        std::string res = client.request("GET", uri.c_str(), true);
        
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

    // LIST TRANSACTIONS
    // Rows are numbered sequentially from 1 (the oldest).
    // The range returned is specified with the min row (inclusive) and max_row (exclusive) parameters.
    // MAXIMUM 1000 rows per call.
    // non-positive range wraps around the most recent row.
    // For example, the 100 most recent rows: use min_row=-100 and max_row=0.
    //
    std::string LunoClient::getTransactions(std::string id, int minRowRange, int maxRowRange){
        std::string uri = "https://api.mybitx.com/api/1/accounts/" + id + "/transactions";
    
        std::string res = client.request("GET", uri.c_str(), true);
        
        int httpCode = client.getHttpCode();
        if (httpCode != 200)
            throw ResponseEx("Error " + std::to_string(httpCode) + " - " + res);
        
        return res;
        
        // Results will look like:
        /*
         {
           "id": "string",
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
               "kind": "FEE",
               "row_index": 0,
               "timestamp": "string"
             }
           ]
         }
         */
    }
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
            balances.back().accountID = extractNextString(res, last, last, "account_id");

            
            //asset
            balances.back().asset = extractNextString(res, last, last, "asset");
            
            //balance
            std::string token = extractNextString(res, last, last, "balance");
            balances.back().balance = atof(token.c_str());
            
            //reserved
            token = extractNextString(res, last, last, "reserved");
            balances.back().reserved = atof(token.c_str());
            
            //unconfirmed
            token = extractNextString(res, last, last, "unconfirmed");
            balances.back().uncomfirmed = atof(token.c_str());
        }
        return balances;
    }

    // MOVE (BALANCE)
    //
    // Moves funds between two of your accounts (same currency)
    // Query the status of this move request as it is not guranteed to be moved on success .
    MoveSummary LunoClient::postMove(std::string sourceID, std::string destinationID, float amount, std::string customID){
        std::string uri = "https://api.mybitx.com/api/exchange/1/move";
        
        uri += "?amount=" + std::to_string(amount);
        uri += "&debit_account_id=" + sourceID;
        uri += "&credit_account_id=" + destinationID;
        if (customID != "")
            uri += "&client_move_id=" + customID;
        
        
        std::string res = client.request("POST", uri.c_str(), true);
        
        int httpCode = client.getHttpCode();
        if (httpCode != 200)
            throw ResponseEx("Error " + std::to_string(httpCode) + " - " + res);
        
        MoveSummary result;
        size_t last = 0;
        
        // id
        result.id = extractNextString(res, last, last, "id");
        
        // status
        result.status = extractNextString(res, last, last, "status");
        
        // client_move_id
        result.customID = extractNextString(res, last, last, "client_move_id");
        
        // Remember to query the results of this move as it is not guranteed to be successfully moved even with id and status "CREATED".
        return result;
    }

    // QUERY MOVE (BALANCE)
    //
    // Checks the status of a move-funds request between two of your accounts (same currency) was succe
    // Query the status of this move request as it is not guranteed to be moved on success .
    MoveResult LunoClient::queryMove(std::string id, bool isCustomID){
        std::string uri = "https://api.mybitx.com/api/exchange/1/move";
        
        if (isCustomID)
            uri += "?client_move_id=" + id;
        else
            uri += "&id=" + id;
        
        std::string res = client.request("GET", uri.c_str(), true);
        
        res.erase(std::remove(res.begin(), res.end(), '\n'), res.cend()); // '\n' breaks parsing
        int httpCode = client.getHttpCode();
        if (httpCode != 200)
            throw ResponseEx("Error " + std::to_string(httpCode) + " - " + res);
        
        MoveResult result;
        
        size_t last = 0;
        
        // amount
        std::string token = extractNextString(res, last, last, "amount");
        result.amount = atof(token.c_str());
        
        // client_move_id
        result.customID = extractNextString(res, last, last, "client_move_id");
        
        // created_at
        token = extractNextString(res, last, last, "created_at");
        result.timestamp = atoll(token.c_str());
        
        // credit_account_id
        result.destinationAccountID = extractNextString(res, last, last, "credit_account_id");
        
        // debit_account_id
        result.sourceAccountID = extractNextString(res, last, last, "debit_account_id");
        
        // id
        result.id = extractNextString(res, last, last, "id");
        
        // status
        result.status = extractNextString(res, last, last, "status");
        
        // updated_at
        token = extractNextString(res, last, last, "updated_at");
        result.lastUpdatedAt = atoll(token.c_str());
        
        return result;
    }

    // LIST MOVES HISTORY(BALANCE)
    //
    // Checks the status of a move-funds request between two of your accounts (same currency) was succe
    // Query the status of this move request as it is not guranteed to be moved on success .
    std::vector<MoveResult> LunoClient::ListMoveHistory(int limit, unsigned long long since){
        std::string uri = "https://api.mybitx.com/api/exchange/1/move/list_moves";
        
        if (since != 0)
            uri += "?before=" + std::to_string(since);
        
        uri += "&limit=" + std::to_string(limit);
        
        std::string res = client.request("GET", uri.c_str(), true);
        res.erase(std::remove(res.begin(), res.end(), '\n'), res.cend()); // '\n' breaks parsing
        
        int httpCode = client.getHttpCode();
        if (httpCode != 200)
            throw ResponseEx("Error " + std::to_string(httpCode) + " - " + res);
        
        std::vector<MoveResult> list;
        size_t last = 0;
        
        // moves
        res = extractNextStringBlock(res, last, "[", "]", "moves");
        
        while ((last = res.find("{", last)) != std::string::npos) {
            MoveResult result;
            
            std::string entry = extractNextStringBlock(res, last, "{", "}", last, nullptr); // this line updates last to after "}"
            
            // amount
            size_t pos = 0;
            std::string token = extractNextString(entry, pos, pos, "amount");
            result.amount = atof(token.c_str());
            
            // client_move_id
            result.customID = extractNextString(entry, pos, pos, "client_move_id");
            
            // created_at
            token = extractNextString(entry, pos, pos, "created_at");
            result.timestamp = atoll(token.c_str());
            
            // credit_account_id
            result.destinationAccountID = extractNextString(entry, pos, pos, "credit_account_id");
            
            // debit_account_id
            result.sourceAccountID = extractNextString(entry, pos, pos, "debit_account_id");
            
            // id
            result.id = extractNextString(entry, pos, pos, "id");
            
            // status
            result.status = extractNextString(entry, pos, pos, "status");
            
            // updated_at
            token = extractNextString(entry, pos, pos, "updated_at");
            result.lastUpdatedAt = atoll(token.c_str());
            
            list.push_back(result);
        }
        return list;
    }
}
