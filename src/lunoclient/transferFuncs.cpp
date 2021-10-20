#include "lunoclient.hpp"

extern Client client;

namespace Luno {

    // GET WITHDRAWAL LIST
    //
    //
    std::vector<Withdrawal> LunoClient::getWithdrawalList(){
        std::string uri = "https://api.mybitx.com/api/1/withdrawals";
        std::string res = client.request("GET", uri.c_str(), true);
        
        int httpCode = client.getHttpCode();
        if (httpCode != 200)
            throw ResponseEx("Error " + std::to_string(httpCode) + " - " + res);
        
        std::vector<Withdrawal> withdrawals;
        size_t last = 0, next = 0;
        last = res.find("[", last) + 1;
        
        while ((last = res.find("{", last)) != std::string::npos) {
            withdrawals.push_back(Withdrawal());
            
            // id
            last = res.find("id", last);
            last = res.find(":", last) + 2;
            next = res.find("\"", last);
            std::string token = res.substr(last, next-last);
            withdrawals.back().id = atoll(token.c_str());
            last = next + 1;
               
            // status
            last = res.find("status", last);
            last = res.find(":", last) + 2;
            next = res.find("\"", last);
            token = res.substr(last, next-last);
            withdrawals.back().status = token.c_str();
            last = next + 1;
            
            // created_at
            last = res.find("created_at", last);
            last = res.find(":", last) + 1;
            next = res.find(",", last);
            token = res.substr(last, next-last);
            withdrawals.back().createdTime = atoll(token.c_str());
            last = next + 1;

            // type
            last = res.find("type", last);
            last = res.find(":", last) + 2;
            next = res.find("\"", last);
            token = res.substr(last, next-last);
            withdrawals.back().type = token.c_str();
            last = next + 1;
            
            // currency
            last = res.find("currency", last);
            last = res.find(":", last) + 2;
            next = res.find("\"", last);
            token = res.substr(last, next-last);
            withdrawals.back().currency = token.c_str();
            last = next + 1;
            
            // amount
            last = res.find("amount", last);
            last = res.find(":", last) + 2;
            next = res.find("\"", last);
            token = res.substr(last, next-last);
            withdrawals.back().amount = atof(token.c_str());
            last = next + 1;
               
            // fee
            last = res.find("fee", last);
            last = res.find(":", last) + 2;
            next = res.find("\"", last);
            token = res.substr(last, next-last);
            withdrawals.back().fee = atof(token.c_str());
            last = next + 1;
        }
        
        return withdrawals;
    }

    template <class T> T& operator << (T& stream, Withdrawal& w) {
        std::stringstream ss;

        ss << "Timestamp: " << w.createdTime << "\n";
        ss << "Withdrawal ID: " << w.id << "\n";
        ss << "Type: " << w.type;
        ss << " (Currency: " << w.currency << "\n";
        ss << "Status: " << w.status << "\n";
        ss << "Amount: "  << w.amount << "\n";
        ss << "Fee: "  << w.fee << "\n";

        stream.append(ss.str().c_str());
        return stream;
    }
    template <class T> T& operator << (T& stream, std::vector<Withdrawal>& w){
        for (Withdrawal& item : w){
            stream << item;
        }
        return stream;
    }
    template QTextEdit& operator << <QTextEdit>(QTextEdit&, Withdrawal& w);
    template QTextEdit& operator << <QTextEdit>(QTextEdit&, std::vector<Withdrawal>& w);

    // REQUEST WITHDRAW
    //
    //
    Withdrawal LunoClient::withdraw(float amount, bool isFast){
        std::string uri = "https://api.mybitx.com/api/1/withdrawals";
        uri += "?type=ZAR_EFT";
        
        std::ostringstream strAmount;
        strAmount.precision(2);
        strAmount << std::fixed << amount;
        
        uri += "&amount=" + strAmount.str();
        if (isFast)
            uri += "&fast=true";
        
        std::string res = client.request("POST", uri.c_str(), true);
        
        int httpCode = client.getHttpCode();
        if (httpCode != 200)
            throw ResponseEx("Error " + std::to_string(httpCode) + " - " + res);
        
        Withdrawal withdrawal;
        size_t last = 0, next = 0;
        

        // id
        last = res.find("id", last);
        last = res.find(":", last) + 2;
        next = res.find("\"", last);
        std::string token = res.substr(last, next-last);
        withdrawal.id = atoll(token.c_str());
        last = next + 1;
           
        // status
        last = res.find("status", last);
        last = res.find(":", last) + 2;
        next = res.find("\"", last);
        token = res.substr(last, next-last);
        withdrawal.status = token.c_str();
        last = next + 1;
        
        // created_at
        last = res.find("created_at", last);
        last = res.find(":", last) + 1;
        next = res.find(",", last);
        token = res.substr(last, next-last);
        withdrawal.createdTime = atoll(token.c_str());
        last = next + 1;

        // type
        last = res.find("type", last);
        last = res.find(":", last) + 2;
        next = res.find("\"", last);
        token = res.substr(last, next-last);
        withdrawal.type = token.c_str();
        last = next + 1;
        
        // currency
        last = res.find("currency", last);
        last = res.find(":", last) + 2;
        next = res.find("\"", last);
        token = res.substr(last, next-last);
        withdrawal.currency = token.c_str();
        last = next + 1;
        
        // amount
        last = res.find("amount", last);
        last = res.find(":", last) + 2;
        next = res.find("\"", last);
        token = res.substr(last, next-last);
        withdrawal.amount = atof(token.c_str());
        last = next + 1;
           
        // fee
        last = res.find("fee", last);
        last = res.find(":", last) + 2;
        next = res.find("\"", last);
        token = res.substr(last, next-last);
        withdrawal.fee = atof(token.c_str());
        last = next + 1;
        
        
        return withdrawal;
    }

    // GET WITHDRAWAL
    //
    //
    std::string LunoClient::getWithdrawal(std::string id){
        std::string uri = "https://api.mybitx.com/api/1/withdrawals/" + id;
        std::string res = client.request("GET", uri.c_str(), true);
        
        int httpCode = client.getHttpCode();
        if (httpCode != 200)
            throw ResponseEx("Error " + std::to_string(httpCode) + " - " + res);
        
        return res;
    }

    // CANCEL WITHDRAWAL
    //
    //
    std::string LunoClient::cancelWithdrawal(std::string id){
        std::string uri = "https://api.mybitx.com/api/1/withdrawals/" + id;
        std::string res = client.request("DELETE", uri.c_str(), true);
        
        int httpCode = client.getHttpCode();
        if (httpCode != 200)
            throw ResponseEx("Error " + std::to_string(httpCode) + " - " + res);
        
        return res;
    }
}
