#include "client.hpp"
// WITHDRAW
//
//
std::string Client::withdraw(float amount){
    std::string uri = "https://api.mybitx.com/api/1/withdrawals?type=ZAR_EFT&amount" + std::to_string(amount);
    
    std::string res = this->request("POST", uri.c_str(), true);
    
    if (httpCode != 200)
        throw ResponseEx("Error - " + res);
    
    return res;
}
