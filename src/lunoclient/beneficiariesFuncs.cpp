#include "lunoclient.hpp"

extern Client client;

namespace Luno {
    // LIST BENEFICIARIES
    //
    // Provides information about banks/financial institutions that are able to withdraw.
    std::vector<Beneficiary> LunoClient::listBeneficiaries(){
        std::string uri = "https://api.mybitx.com/api/1/beneficiaries";
    
        std::string res = client.request("GET", uri.c_str(), true);
        
        int httpCode = client.getHttpCode();
        if (httpCode != 200)
            throw ResponseEx("Error " + std::to_string(httpCode) + " - " + res);
        
        std::vector<Beneficiary> beneficiaries;
        size_t last = 0;
        last = res.find("[", last) + 1;
        
        while ((last = res.find("{", last)) != std::string::npos) {
            Beneficiary beneficiary;
            std::string block = extractNextStringBlock(res, last, "{", "}", last, nullptr);
            
            size_t pos = 0;
            // id
            beneficiary.id = extractNextString(block, pos, pos, "id");
            
            // created_at
            std::string token = extractNextString(block, pos, ",", pos, "created_at");
            beneficiary.timestamp = atoll(token.c_str());

            // bank_recipient
            beneficiary.accountHolder = extractNextString(block, pos, pos, "bank_recipient");
            
            // bank_account_number
            beneficiary.accountNumber = extractNextString(block, pos, pos, "bank_account_number");
            
            // bank_account_branch
            beneficiary.branch = extractNextString(block, pos, pos, "bank_account_branch");
            
            // bank_account_type
            beneficiary.accountType = extractNextString(block, pos, pos, "bank_account_type");
            
            // bank_name
            beneficiary.bank = extractNextString(block, pos, pos, "bank_name");
            
            // bank_country
            beneficiary.country = extractNextString(block, pos, pos, "bank_country");
            
            // supports_fast_withdrawals
            token = extractNextString(block, pos,"}" , pos, "supports_fast_withdrawals");
            beneficiary.supportsFastWithdrawal = (token == "true") ? true : false;
            
            beneficiaries.push_back(beneficiary);
        }
        
        return beneficiaries;

    }
}
