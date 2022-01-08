#include "lunoclient.hpp"

extern Client client;
namespace Luno {
    

    // GET RECIEVE ADDRESS
    //
    //
    std::string LunoClient::getRecieveAddress(std::string asset, std::string address){
        std::string uri = "https://api.mybitx.com/api/1/funding_address";
        uri += "?asset=" + asset;
        if (address.length() > 0)
            uri += "&address=" + address;
        
        std::string res = client.request("GET", uri.c_str(), true);
        
        int httpCode = client.getHttpCode();
        if (httpCode != 200)
            throw ResponseEx("Error " + std::to_string(httpCode) + " - " + res);
        
        return res;
    }

    // CREATE RECIEVE ADDRESS
    //
    //
    std::string LunoClient::createRecieveAddress(std::string asset, std::string name){
        std::string uri = "https://api.mybitx.com/api/1/funding_address";
        uri += "?asset=" + asset;
        if (name.length() > 0)
            uri += "&name=" + name;
        
        std::string res = client.request("POST", uri.c_str(), true);
        
        int httpCode = client.getHttpCode();
        if (httpCode != 200)
            throw ResponseEx("Error " + std::to_string(httpCode) + " - " + res);
        
        return res;
    }

    // SEND TO ADDRESS
    //
    //
    std::string LunoClient::sendToAddress(std::string asset, std::string address, float amount){
        std::string uri = "https://api.mybitx.com/api/1/send";
        uri += "?currency=" + asset;
        uri += "&address=" + address; /*This must be tested, verified first*/
        uri += "&amount=" + std::to_string(amount);
        
        std::string res = client.request("POST", uri.c_str(), true);
        
        int httpCode = client.getHttpCode();
        if (httpCode != 200)
            throw ResponseEx("Error " + std::to_string(httpCode) + " - " + res);
        
        /*
            size_t last = 0;
         
            // success
            std::string token = extractNextString(res, last, ",", last, "success");
            bool success = (token == "true" ? true : false);
            std::string id = extractNextString(res, last, last, "withdrawal_id");
            "withdrawal_id": "string"
         
            return (create type) { success, id }
        */
        return res;
    }

}
