#include "valrclient.hpp"

extern Client client;

// To help validate emails, phone numbers and VALR IDs
namespace validator {
    bool isChar(char c) {
        return ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'));
    }
      
    bool isDigit(const char c) {
        return (c >= '0' && c <= '9');
    }

    // Validates a south african phone number
    bool isValidNumber(std::string number) {
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

    // Validates an email address
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

// VALR payment service functions
namespace VALR {
    extern std::string host;

    // NEW PAYMENT
    // Makes an instant payment using VALR Pay
    //  paramater required :
    //      -- amount (in rands)
    //      -- VALR_PAY_NOTIFICATION (enumerator)
    //      -- recipient email or recipient cellphone number or their account payment reference (VALR PayID)
    // the rest are optional
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
     
        VALR_PAY_Result result;
        result.id = extractNextString(res, last, last);
        result.transactionID = extractNextString(res, last, last);
        
        return result;
    }



    // PAYMENT INFO
    //
    // Gets all  payment limits applicable to your account
    PaymentLimitInfo VALRClient::getPaymentInfo(){
        std::string path = "/v1/pay/limits";
        
        std::string res = client.request("GET", (host+path).c_str(), true, VALR_EXCHANGE, path.c_str());
        
        int httpCode = client.getHttpCode();
        if (httpCode != 200)
            throw ResponseEx("Error " + std::to_string(httpCode) + " - " + res);
        
        PaymentLimitInfo info;
        size_t last = 0;
        
        
        // maxPaymentAmount
        std::string token = extractNextString(res, last, ",", last);
        info.maxAmount = atof(token.c_str());
        
        // minPaymentAmount
        token = extractNextString(res, last, ",", last);
        info.minAmount = atof(token.c_str());
        
        // paymentCurrency
        info.asset = extractNextString(res, last, last);
        
        // limitType
        info.limitedByWhat = extractNextString(res, last, last);
        
        
        return info;
    }

    // USER PAYMENT ID
    //
    // Retrieves your account's unique VALR PayID
    std::string VALRClient::getUserPaymentID(){
        std::string path = "/v1/pay/payid";
        
        std::string res = client.request("GET", (host+path).c_str(), true, VALR_EXCHANGE, path.c_str());
        
        int httpCode = client.getHttpCode();
        if (httpCode != 200)
            throw ResponseEx("Error " + std::to_string(httpCode) + " - " + res);
        
        
        // payId
        std::string id = extractNextString(res, 0);
        
        return id;
    }

    // PAYMENT HISTORY
    //
    // Retrieves your account's payments which were made from this account or made to this account.
    // Parameters:
    //      statusFilter: "INITIATED", "AUTHORISED", "COMPLETE", "RETURNED", "FAILED", "EXPIRED"
    //      skip: skips a number of entries
    //      limit: limits returned results to this number of entries
    std::vector<VALR_PAY_History_Entry> VALRClient::getUserPaymentHistory(std::string statusFilter, unsigned skip, unsigned limit){
        std::string path = "/v1/pay/history";
        int args = 0;
        if (statusFilter.length() > 0){
            path += (args++ ? "&" : "?");
            path += "status=" + statusFilter;
        }
        if (skip != 0){
            path += (args++ ? "&" : "?");
            path += "skip=" + std::to_string(skip);
        }
        if (limit != 0){
            path += (args++ ? "&" : "?");
            path += "limit=" + std::to_string(limit);
        }
        
        std::string res = client.request("GET", (host+path).c_str(), true, VALR_EXCHANGE, path.c_str());
        
        int httpCode = client.getHttpCode();
        if (httpCode != 200)
            throw ResponseEx("Error " + std::to_string(httpCode) + " - " + res);
        
        
        std::vector<VALR_PAY_History_Entry> history;
        size_t last = 0;
        
        while ((last = res.find("{", last)) != std::string::npos) {
            VALR_PAY_History_Entry entry;

            // identifier
            entry.paymentID = extractNextString(res, last, last);
            
            // otherPartyIdentifier
            entry.otherPartysID = extractNextString(res, last, last);
            
            // amount
            std::string token = extractNextString(res, last, ",", last);
            entry.amount = atof(token.c_str());
            
            token = extractNextStringBlock(res, last, "\"", "\"");
            if (token == "status"){
                // status
                entry.status = extractNextString(res, last, last);
            }
            
            // timestamp
            entry.timestamp = extractNextString(res, last, last);
            
            token = extractNextStringBlock(res, last, "\"", "\"");
            if (token == "senderNote"){
                // senderNote
                entry.myReference = extractNextString(res, last, last);
            }
            
            token = extractNextStringBlock(res, last, "\"", "\"");
            if (token == "recipientNote"){
                // senderNote
                entry.beneficiaryReference = extractNextString(res, last, last);
            }
            
            // transactionId
            entry.transactionID = extractNextString(res, last, last);
            
            // anonymous
            token = extractNextString(res, last, ",", last);
            entry.isAnonymous = (token == "true" ? true : false);
            
            // type
            entry.type = extractNextString(res, last, last);
            if (entry.type == "CREDIT")
                entry.type += " (incoming payment)";
            if (entry.type == "DEBIT")
                entry.type += " (outgoing payment)";
            
            history.push_back(entry);
        }
        return history;
    }

    // PAYMENT HISTORY ENTRY (LOOKUP BY PAYMENT ID)
    //
    // Retrieves details about a single entry from your account's payments history (made from this account or made to this account.
    // Parameters:
    //      id: payment identifier
    VALR_PAY_History_Entry VALRClient::getUserPaymentEntryByPaymentID(std::string id){
        std::string path = "/v1/pay/identifier/" + id;
        
        std::string res = client.request("GET", (host+path).c_str(), true, VALR_EXCHANGE, path.c_str());
        
        int httpCode = client.getHttpCode();
        if (httpCode != 200)
            throw ResponseEx("Error " + std::to_string(httpCode) + " - " + res);
        
        
        size_t last = 0;
    
        VALR_PAY_History_Entry entry;

        // identifier
        entry.paymentID = extractNextString(res, last, last);
        
        // otherPartyIdentifier
        entry.otherPartysID = extractNextString(res, last, last);
        
        // amount
        std::string token = extractNextString(res, last, ",", last);
        entry.amount = atof(token.c_str());
        
        token = extractNextStringBlock(res, last, "\"", "\"");
        if (token == "status"){
            // status
            entry.status = extractNextString(res, last, last);
        }
        
        // timestamp
        entry.timestamp = extractNextString(res, last, last);
        
        token = extractNextStringBlock(res, last, "\"", "\"");
        if (token == "senderNote"){
            // senderNote
            entry.myReference = extractNextString(res, last, last);
        }
        
        token = extractNextStringBlock(res, last, "\"", "\"");
        if (token == "recipientNote"){
            // senderNote
            entry.beneficiaryReference = extractNextString(res, last, last);
        }
        
        // transactionId
        entry.transactionID = extractNextString(res, last, last);
        
        // anonymous
        token = extractNextString(res, last, ",", last);
        entry.isAnonymous = (token == "true" ? true : false);
        
        // type
        entry.type = extractNextString(res, last, last);
        if (entry.type == "CREDIT")
            entry.type += " (incoming payment)";
        if (entry.type == "DEBIT")
            entry.type += " (outgoing payment)";
            
        return entry;
    }

    // PAYMENT HISTORY ENTRY (LOOKUP BY TRANSACTION ID)
    //
    // Retrieves details about a single entry from your account's payments history (made from this account or made to this account.
    // Parameters:
    //      id: transaction identifier
    VALR_PAY_History_Entry VALRClient::getUserPaymentEntryByTransactionID(std::string id){
        std::string path = "/v1/pay/transactionid/" + id;
        
        std::string res = client.request("GET", (host+path).c_str(), true, VALR_EXCHANGE, path.c_str());
        
        int httpCode = client.getHttpCode();
        if (httpCode != 200)
            throw ResponseEx("Error " + std::to_string(httpCode) + " - " + res);
        
        
        size_t last = 0;

        VALR_PAY_History_Entry entry;
        
        std::string token = extractNextStringBlock(res, last, "\"", "\"");
        if (token == "identifier")
            // identifier
            entry.paymentID = extractNextString(res, last, last);
        
        token = extractNextStringBlock(res, last, "\"", "\"");
        if (token == "otherPartyIdentifier")
            // otherPartyIdentifier
            entry.otherPartysID = extractNextString(res, last, last);
        
        token = extractNextStringBlock(res, last, "\"", "\"");
        if (token == "amount"){
            // amount
            std::string token = extractNextString(res, last, ",", last);
            entry.amount = atof(token.c_str());
        }
        
        token = extractNextStringBlock(res, last, "\"", "\"");
        if (token == "status"){
            // status
            entry.status = extractNextString(res, last, last);
        }
        
        token = extractNextStringBlock(res, last, "\"", "\"");
        if (token == "timestamp")
            // timestamp
            entry.timestamp = extractNextString(res, last, last);
        
        token = extractNextStringBlock(res, last, "\"", "\"");
        if (token == "senderNote"){
            // senderNote
            entry.myReference = extractNextString(res, last, last);
        }
        
        token = extractNextStringBlock(res, last, "\"", "\"");
        if (token == "recipientNote"){
            // recipientNote
            entry.beneficiaryReference = extractNextString(res, last, last);
        }
        
        token = extractNextStringBlock(res, last, "\"", "\"");
        if (token == "transactionId")
            // transactionId
            entry.transactionID = extractNextString(res, last, last);
        
        token = extractNextStringBlock(res, last, "\"", "\"");
        if (token == "anonymous"){
            // anonymous
            token = extractNextString(res, last, ",", last);
            entry.isAnonymous = (token == "true" ? true : false);
        }
        
        token = extractNextStringBlock(res, last, "\"", "\"");
        if (token == "type"){
            // type
            entry.type = extractNextString(res, last, last);
            if (entry.type == "CREDIT")
                entry.type += " (incoming payment)";
            if (entry.type == "DEBIT")
                entry.type += " (outgoing payment)";
        }
        return entry;
    }
}
