#include "lunoclient.hpp"
namespace Luno {
    extern Client client;

    // GET RECIEVE ADDRESS
    //
    //
    std::string LunoClient::getRecieveAddress(std::string asset){
        std::string uri = "https://api.mybitx.com/api/1/funding_address";
        uri += "?asset=" + asset;
        std::string res = client.request("GET", uri.c_str(), true);
        
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
        
        return res;
    }


}
