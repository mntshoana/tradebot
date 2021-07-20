#include "lunoclient.hpp"
namespace Luno {
    extern Client client;

    // GET WITHDRAWAL LIST
    //
    //
    std::string LunoClient::getWithdrawalList(){
        std::string uri = "https://api.mybitx.com/api/1/withdrawals";
        std::string res = client.request("GET", uri.c_str(), true);
        
        int httpCode = client.getHttpCode();
        if (httpCode != 200)
            throw ResponseEx("Error " + std::to_string(httpCode) + " - " + res);
        
        return res;
    }

    // REQUEST WITHDRAW
    //
    //
    std::string LunoClient::withdraw(float amount, bool isFast){
        std::string uri = "https://api.mybitx.com/api/1/withdrawals";
        uri += "?type=ZAR_EFT";
        uri += "&amount=" + std::to_string(amount);
        if (isFast)
            uri += "&fast=true";
        
        std::string res = client.request("POST", uri.c_str(), true);
        
        int httpCode = client.getHttpCode();
        if (httpCode != 200)
            throw ResponseEx("Error " + std::to_string(httpCode) + " - " + res);
        
        return res;
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
