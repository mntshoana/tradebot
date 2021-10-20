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
    template <class T> T& operator << (T& stream, std::vector<Balance>& balances){
        for (Balance& balance : balances){
            stream << balance;
        }
        return stream;
    }
    template QTextEdit& operator << <QTextEdit>(QTextEdit&, Balance& balance);
    template QTextEdit& operator << <QTextEdit>(QTextEdit&, std::vector<Balance>& balances);

}
