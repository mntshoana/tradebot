#include "lunoclient.hpp"

extern Client client;

namespace Luno {

    // create account
    // update account
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
