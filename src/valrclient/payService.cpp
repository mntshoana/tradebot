#include "valrclient.hpp"

extern Client client;

namespace validator {
    bool isChar(char c)
    {
        return ((c >= 'a' && c <= 'z')
                || (c >= 'A' && c <= 'Z'));
    }
      
    bool isDigit(const char c)
    {
        return (c >= '0' && c <= '9');
    }

    bool isValidNumber(std::string number){
        if (number.length() < 10)
            return false; // Must be at least 10 digits
        
        if (number[0] == '0'){
            for (char& num : number)
                if (!isDigit(num))
                    return false;
            
            return true;
        } else if (number.substr(0,3) == "+27"){
            for (char& num : number.substr(3))
                 if (!isDigit(num))
                     return false;
            
            return true;
        }
        else return false;
    }
    bool isValidEmail(std::string email){
        if (email.length() < 3)
            return false; // shortest possible email letter @ letter == 3 chars
        
        // first character must be an alphabet or not
        if (!validator::isChar(email[0])) {
            return false;
        }

        size_t atPos = -1, dotPos = -1;
        for (size_t i = 0; i < email.length(); i++) {
            if (email[i] == '@')
                atPos = i;
            else if (email[i] == '.')
                dotPos = i;
        }
      
        if (atPos == std::string::npos || dotPos == std::string::npos)
            return false; // @ or dot not present
      
        
        if (atPos > dotPos)
            return 0; // Dot cannot be before @
      
        bool isDotAtEnd = dotPos >= (email.length() - 1);
        
        return !(isDotAtEnd); // Dot cannot be present at the end
    }
}
namespace VALR {
    extern std::string host;

    //
    //
    //
    VALR_PAY_Result VALRClient::postNewPayment(float amount, VALR_PAY_NOTIFICATION notificationMethod, std::string notificationString, std::string beneficiaryReference,    std::string myReference, bool isAnonymous){
        std::string path = "/v1/pay";
        
        // VALR currently only supports ZAR payments
        std::ostringstream strAmount;
        strAmount.precision(2);
        strAmount << std::fixed << amount;
        
        std::string jsonLine;
        switch (notificationMethod){
            case VALR_PAY_NOTIFICATION::ID :
                if (notificationString.length() <= 1)
                    throw std::invalid_argument("Invalid ID (empty)");
                
                jsonLine += "\n\t" + createJSONlabel("recipientPayId", notificationString);
                break;
            case VALR_PAY_NOTIFICATION::EMAIL :
                if (!validator::isValidEmail(notificationString))
                    throw std::invalid_argument("Invalid email address (" + notificationString + ").");
                
                jsonLine += "\n\t" + createJSONlabel("recipientEmail", notificationString);
                break;
            case VALR_PAY_NOTIFICATION::CELL_NUMBER :
                if (!validator::isValidNumber(notificationString))
                    throw std::invalid_argument("Invalid phone number (" + notificationString + ").");
                
                jsonLine += "\n\t" + createJSONlabel("recipientCellNumber", notificationString);
                break;
        }
          
        std::string payload = "{";
        payload += "\n\t" + createJSONlabel("currency", "ZAR") + ",";
        payload += "\n\t" + createJSONlabelUnquoted("amount", strAmount.str()) + ",";
        payload += jsonLine;
        if (beneficiaryReference.length() > 0)
            payload += ",\n\t" + createJSONlabel("recipientNote", beneficiaryReference);
        if (myReference.length() > 0)
            payload += ",\n\t" + createJSONlabel("senderNote", myReference);
        if (isAnonymous)
            payload += ",\n\t" + createJSONlabel("anonymous", "true");
        payload +=  "\n" "}";

            
        std::string res = client.request("POST", (host+path).c_str(), true, VALR_EXCHANGE, path.c_str(), payload.c_str());
        
        int httpCode = client.getHttpCode();
        if (httpCode != 200)
            throw ResponseEx("Error " + std::to_string(httpCode) + " - " + res);
        
        size_t last = 0;
        
        // erase spaces
        res.erase(remove( res.begin(), res.end(), ' ' ),res.end());
     
        VALR_PAY_Result result;
        result.id = extractNextString(res, last, last);
        result.transactionID = extractNextString(res, last, last);
        
        return result;
    }
}
