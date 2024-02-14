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
        
        res.erase(std::remove(res.begin(), res.end(), '\n'), res.cend()); // '\n' breaks parsing
        std::vector<Withdrawal> withdrawals;
        size_t last = 0;
        last = res.find("[", last) + 1;
        while ((last = res.find("{", last)) != std::string::npos) {
            withdrawals.push_back(Withdrawal());
            
            // id
            std::string token = extractNextString(res, last, last, "id");
            withdrawals.back().id = atoll(token.c_str());
               
            // status
            token = extractNextString(res, last, last, "status");
            withdrawals.back().status = token.c_str();
            
            // created_at
            token = extractNextString(res, last, ",", last, "created_at");
            withdrawals.back().createdTime = atoll(token.c_str());

            // type
            token = extractNextString(res, last, last, "type");
            withdrawals.back().type = token.c_str();
            
            // currency
            token = extractNextString(res, last, last, "currency");
            withdrawals.back().currency = token.c_str();
            
            // amount
            token = extractNextString(res, last, last, "amount");
            withdrawals.back().amount = atof(token.c_str());
               
            // fee
            token = extractNextString(res, last, last, "fee");
            withdrawals.back().fee = atof(token.c_str());
        }
        
        return withdrawals;
    }


    // REQUEST WITHDRAW
    //
    //
    Withdrawal LunoClient::withdraw(float amount, bool isFast, std::string beneficiaryID){
        std::string uri = "https://api.mybitx.com/api/1/withdrawals";
        uri += "?type=ZAR_EFT";
        
        std::ostringstream strAmount;
        strAmount.precision(2);
        strAmount << std::fixed << amount;
        
        uri += "&amount=" + strAmount.str();
        if (isFast)
            uri += "&fast=true";
        
        if (beneficiaryID != "")
            uri += "&beneficiary_id=" + beneficiaryID;
        
        std::string res = client.request("POST", uri.c_str(), true);
        
        res.erase(std::remove(res.begin(), res.end(), '\n'), res.cend()); // '\n' breaks parsing
        int httpCode = client.getHttpCode();
        if (httpCode != 200)
            throw ResponseEx("Error " + std::to_string(httpCode) + " - " + res);
        
        Withdrawal withdrawal;
        size_t last = 0;
        

        // id
        std::string token = extractNextString(res, last, last, "id");
        withdrawal.id = atoll(token.c_str());
           
        // status
        token = extractNextString(res, last, last, "status");
        withdrawal.status = token.c_str();
        
        // created_at
        token = extractNextString(res, last, ",", last, "created_at");
        withdrawal.createdTime = atoll(token.c_str());

        // type
        token = extractNextString(res, last, last, "type");
        withdrawal.type = token.c_str();
        
        // currency
        token = extractNextString(res, last, last, "currency");
        withdrawal.currency = token.c_str();
        
        // amount
        token = extractNextString(res, last, last, "amount");
        withdrawal.amount = atof(token.c_str());
           
        // fee
        token = extractNextString(res, last, last, "fee");
        withdrawal.fee = atof(token.c_str());
        
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
