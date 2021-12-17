#include "valrclient.hpp"

extern Client client;

namespace VALR {
    extern std::string host;
    
    std::vector<CurrencyInfo> currencies;

    // SIMPLE BUY/SELL QUOTE
    // Request a quote for an instant buy or sell order
    //     action   expects string {  "BID" or "ASK" }
    //
    SimpleQuote VALRClient::getOrderQuote(std::string pair, std::string action, float volume){
        std::string path = "/v1/simple/" + pair + "/quote";
        
        if (!(action == "BID" || action == "ASK"))
            throw std::invalid_argument("'action' expects string value \"BID\" or \"ASK\" only! ");
        bool isBuy = (action == "BID") ? true : false;
        
        std::string quoteCurrency = (isBuy ? pair.substr(3,3) : pair.substr(0, 3));
        int decimals;
        
        if (currencies.size() == 0)
            currencies = VALRClient::getCurrencies();
        for (const CurrencyInfo& currency : currencies) {
            if (currency.shortName == quoteCurrency){
                decimals = currency.decimalCount;
                break;
            }
        }
        
        std::ostringstream strVolume;
        strVolume.precision(decimals);
        strVolume << std::fixed << volume;
        
        std::string payload = "{";
        payload += "\n\t" + createJSONlabel("payInCurrency", quoteCurrency) + ",";
        payload += "\n\t" + createJSONlabel("payAmount", strVolume.str()) + ",";
        payload += "\n\t" + createJSONlabel("side", (isBuy ? "BUY" : "SELL")) ;
        payload +=  "\n" "}";

        std::string res = client.request("POST", (host+path).c_str(), true, VALR_EXCHANGE, path.c_str(), payload.c_str());
        
        int httpCode = client.getHttpCode();
        if (httpCode != 200)
            throw ResponseEx("Error " + std::to_string(httpCode) + " - " + res);
        
        size_t last = 0;
        
        SimpleQuote quote;
        
        // currencyPair
        quote.pair = extractNextString(res, last, last);
        
        // payAmount
        std::string token = extractNextString(res, last, last);
        quote.amount = std::atof(token.c_str());
        
        // receiveAmount
        token = extractNextString(res, last, last);
        quote.willRecieve = std::atof(token.c_str());
        
        // fee
        token = extractNextString(res, last, last);
        quote.fee = std::atof(token.c_str());
        
        // feeCurrency
        quote.feeAsset = extractNextString(res, last, last);
        
        // createdAt
        quote.timestamp  = extractNextString(res, last, last);
        
        // id
        quote.id  = extractNextString(res, last, last);
        
        std::string matched = extractNextStringBlock(res, last, "[", "]", last);
        
        last = 0;
        while ((last = matched.find("{", last)) != std::string::npos) {
            OrderMatched order;
            
            // price
            token = extractNextString(matched, last, last);
            order.price = std::atof(token.c_str());
            
            // quantity
            token = extractNextString(matched, last, last);
            order.volume = std::atof(token.c_str());
            
            quote.ordersMatched.push_back(order);
        }
        
        return quote;
    }

    // POST SIMPLE ORDER (Excercise Quote)
    // Request an instant buy or sell order
    //     action   expects string {  "BID" or "ASK" }
    //
    ExcercisedQuote VALRClient::excerciseOrderQuote(std::string pair, std::string action, float volume){
        std::string path = "/v1/simple/" + pair + "/order";
        
        if (!(action == "BID" || action == "ASK"))
            throw std::invalid_argument("'action' expects string value \"BID\" or \"ASK\" only! ");
        bool isBuy = (action == "BID") ? true : false;
        
        std::string quoteCurrency = (isBuy ? pair.substr(3,3) : pair.substr(0, 3));
        int decimals;
        
        if (currencies.size() == 0)
            currencies = VALRClient::getCurrencies();
        for (const CurrencyInfo& currency : currencies) {
            if (currency.shortName == quoteCurrency){
                decimals = currency.decimalCount;
                break;
            }
        }
        
        std::ostringstream strVolume;
        strVolume.precision(decimals);
        strVolume << std::fixed << volume;
        
        std::string payload = "{";
        payload += "\n\t" + createJSONlabel("payInCurrency", quoteCurrency) + ",";
        payload += "\n\t" + createJSONlabel("payAmount", strVolume.str()) + ",";
        payload += "\n\t" + createJSONlabel("side", (isBuy ? "BUY" : "SELL")) ;
        payload +=  "\n" "}";

        std::string res = client.request("POST", (host+path).c_str(), true, VALR_EXCHANGE, path.c_str(), payload.c_str());
        
        int httpCode = client.getHttpCode();
        if (httpCode != 200)
            throw ResponseEx("Error " + std::to_string(httpCode) + " - " + res);
        
        size_t last = 0;
        
        ExcercisedQuote execution;
        // code
        std::string token = extractNextString(res, last, last);
        execution.code = atoi(token.c_str());
        
        // message
        execution.message = extractNextString(res, last, last);
        
        return execution;
    }

    // SIMPLE ORDER STATUS
    // get the status of an executed order (simple order)
    //
    SimpleOrderStatus VALRClient::getSimpleOrderStatus(std::string pair, std::string id){
        std::string path = "/v1/simple/" + pair + "/order/" + id;

        std::string res = client.request("GET", (host+path).c_str(), true, VALR_EXCHANGE, path.c_str());
        
        int httpCode = client.getHttpCode();
        if (httpCode != 200)
            throw ResponseEx("Error " + std::to_string(httpCode) + " - " + res);
        
        size_t last = 0;
                
        SimpleOrderStatus orderStatus;
        
        // orderId
        orderStatus.id = extractNextString(res, last, last);
        
        // success
        std::string token = extractNextString(res, last, ",", last);
        orderStatus.success = (token == "true" ? true : false);
        
        // processing
        token = extractNextString(res, last, ",", last);
        orderStatus.processing = (token == "true" ? true : false);
        
        // paidAmount
        token = extractNextString(res, last, last);
        orderStatus.paidAmount = atof(token.c_str());
        
        // paidCurrency
        orderStatus.paidAsset = extractNextString(res, last, last);
        
        // receivedAmount
        token = extractNextString(res, last, last);
        orderStatus.receivedAmount = atof(token.c_str());
        
        // receivedCurrency
        orderStatus.receivedAsset = extractNextString(res, last, last);
        
        // feeAmount
        token = extractNextString(res, last, last);
        orderStatus.fee = atof(token.c_str());
        
        // feeCurrency
        orderStatus.feeAsset = extractNextString(res, last, last);
        
        // feeCurrency
        orderStatus.timestamp = extractNextString(res, last, last);
        
        return orderStatus;
    }

    // POST LIMIT ORDER
    // Request a limit buy or sell order
    //     returns order id on success
    // parameters
    //     action   expects string {  "BID" or "ASK" }
    //
    // NOTE:
    // fee     (for takers)
    //      for bids will be paid in base  price,
    //      for asks will be paid in quote price
    // rewards (for makers)
    //      for bids will be paid in quote price,
    //      for asks will be paid in base  price
    std::string VALRClient::postLimitOrder(std::string pair, std::string action, float volume, float price){
        std::string path = "/v1/orders/limit";
        
        if (!(action == "BID" || action == "ASK"))
            throw std::invalid_argument("'action' expects string value \"BID\" or \"ASK\" only! ");
        bool isBuy = (action == "BID") ? true : false;
        
        std::string baseCurrency = pair.substr(0, 3);
        std::string quoteCurrency = pair.substr(3, 3);
        int decimalsBase, decimalsQuote;
        
        if (currencies.size() == 0)
            currencies = VALRClient::getCurrencies();
        for (const CurrencyInfo& currency : currencies) {
            if (currency.shortName == baseCurrency)
                decimalsBase = currency.decimalCount;
            if (currency.shortName == quoteCurrency)
                decimalsQuote = currency.decimalCount;
        }
        
        std::ostringstream strPrice;
        strPrice.precision(decimalsQuote);
        strPrice << std::fixed << price;
        
        std::ostringstream strVolume;
        strVolume.precision(decimalsBase);
        strVolume << std::fixed << volume;
        
        std::string payload = "{";
        payload += "\n\t" + createJSONlabel("side", (isBuy ? "BUY" : "SELL")) + ",";
        payload += "\n\t" + createJSONlabel("quantity", strVolume.str()) + ",";
        payload += "\n\t" + createJSONlabel("price", strPrice.str()) + ",";
        payload += "\n\t" + createJSONlabel("pair", pair)+ "," ;
        payload += "\n\t" + createJSONlabelUnquoted("postOnly", "true");
        payload +=  "\n" "}";
        
        // NOTE: you may also add "customerOrderId": "1234" to manage open orders using cusom ids
        // customerOrderId must be alphanumeric with no special chars, limit of 50 characters.

        // NOTE: you may also add "timeInForce": "GTC"
        // values of timeInForce can be only:
        //      "GTC" (Good Till Cancelled)  - this is the default
        //      "FOK" (Fill or Kill)
        //      or "IOC" (Immediate or Cancel)

        std::string res = client.request("POST", (host+path).c_str(), true, VALR_EXCHANGE, path.c_str(), payload.c_str());
        
        int httpCode = client.getHttpCode();
        if (httpCode != 202)
            throw ResponseEx("Error " + std::to_string(httpCode) + " - " + res);
        
        // NOTE: recieving an id does not always mean that the order has been placed.
        // IT COULD STILL HAVE FAILED
        std::string id = extractNextString(res, 0);
        
        return id;
    }
}
