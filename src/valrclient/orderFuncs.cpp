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

    // FORM LIMIT ORDER PAYLOAD
    // Creates the json payload string that allows on to make a LIMIT order on VALR
    //
    std::string VALRClient::formLimitPayload(std::string pair, std::string action, float volume, float price, bool isBatch){
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
        
        if (isBatch){
            std::string batchPayload = R"({
                "type": "PLACE_LIMIT",
                "data": )"
                + payload
                + "\n" "}";
            return batchPayload;
        }
        return payload;
    }

    // POST LIMIT ORDER
    // Request a limit buy or sell order
    //     returns order id on success  (this only means it was accepted, not successful)
    // parameters
    //     pair:     string {must be "BTCZAR", "ETHZAR" or "XRPZAR"}
    //     action:   string {must be  "BID" or "ASK" }
    //     volume:   base amount u wish to make BUY / SELL
    //     price:    quote amount u wish your order to be made at
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
        
        std::string payload = formLimitPayload(pair, action, volume, price);

        std::string res = client.request("POST", (host+path).c_str(), true, VALR_EXCHANGE, path.c_str(), payload.c_str());
        
        int httpCode = client.getHttpCode();
        if (httpCode != 202)
            throw ResponseEx("Error " + std::to_string(httpCode) + " - " + res);
        
        // NOTE: recieving an id does not always mean that the order has been placed.
        // IT COULD STILL HAVE FAILED
        std::string id = extractNextString(res, 0);
        
        return id;
    }

    // FORM MARKET ORDER PAYLOAD
    // Creates the json payload string that allows on to make a MARKET order on VALR
    //
    std::string VALRClient::formMarketPayload(std::string pair, std::string action, float amount, bool isOfBaseCurrency, bool isBatch){
        if (!(action == "BID" || action == "ASK"))
            throw std::invalid_argument("'action' expects string value \"BID\" or \"ASK\" only! ");
        bool isBuy = (action == "BID") ? true : false;
        
        std::string amountLabel = (isOfBaseCurrency ? "baseAmount" : "quoteAmount");
        std::string baseCurrency = pair.substr(0, 3);
        std::string quoteCurrency = pair.substr(3, 3);
        int decimals;
        
        if (currencies.size() == 0)
            currencies = VALRClient::getCurrencies();
        for (const CurrencyInfo& currency : currencies) {
            if (isOfBaseCurrency && currency.shortName == baseCurrency)
                decimals = currency.decimalCount;
            if ( (!isOfBaseCurrency) && currency.shortName == quoteCurrency)
                decimals = currency.decimalCount;
        }
        
        // note: takers fee will be subtracted
        std::ostringstream strAmount;
        strAmount.precision(decimals);
        strAmount << std::fixed << amount;
        
        std::string payload = "{";
        payload += "\n\t" + createJSONlabel("side", (isBuy ? "BUY" : "SELL")) + ",";
        payload += "\n\t" + createJSONlabel(amountLabel , strAmount.str()) + ",";
        payload += "\n\t" + createJSONlabel("pair", pair);
        payload +=  "\n" "}";
        // NOTE: you may also add "customerOrderId": "1234" to manage open orders using cusom ids
        // customerOrderId must be alphanumeric with no special chars, limit of 50 characters.

        if (isBatch){
            std::string batchPayload = R"({
                "type": "PLACE_MARKET",
                "data": )"
                + payload
                + "\n" "}";
            return batchPayload;
        }
        return payload;
    }

    // POST MARKET ORDER
    // Request a market buy or sell order. specify the amount you are willing to spend and that order will be filled immidiately
    //     returns order id on success  (this only means it was accepted, not successful)
    // parameters
    //     pair: string {must be "BTCZAR", "ETHZAR" or "XRPZAR"}
    //     action:   string {must be  "BID" or "ASK" }
    //     amount:   amount u wish to make BUY / SELL (a fee will be subtracted)
    //     isOfBaseCurrency:   bool {true when quoting the Crypto value "BTC, ETH or XRP"}
    //                              {false when quoting the ZAR value "ZAR" }
    //
    // NOTE:
    // fee     (for takers)
    //      for bids will be paid in base  price,
    //      for asks will be paid in quote price
    // rewards (for makers)
    //      for bids will be paid in quote price,
    //      for asks will be paid in base  price
    std::string VALRClient::postMarketOrder(std::string pair, std::string action, float amount, bool isOfBaseCurrency){
        std::string path = "/v1/orders/market";
        
        std::string payload = formMarketPayload(pair, action, amount, isOfBaseCurrency);
        
        std::string res = client.request("POST", (host+path).c_str(), true, VALR_EXCHANGE, path.c_str(), payload.c_str());
        
        int httpCode = client.getHttpCode();
        if (httpCode != 202)
            throw ResponseEx("Error " + std::to_string(httpCode) + " - " + res);
        
        // NOTE: recieving an id does not always mean that the order has been placed.
        // IT COULD STILL HAVE FAILED
        std::string id = extractNextString(res, 0);
        
        return id;
    }

    // FORM STOP LIMIT ORDER PAYLOAD
    // Creates the json payload string that allows on to make a STOP LIMIT order on VALR
    //
    std::string VALRClient::formStopLimitPayload(std::string pair, std::string action, float volume, float price, float stopPrice, bool isStopLossLimit, bool isBatch){
        
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
        
        std::ostringstream strPrice, strStopPrice;
        strPrice.precision(decimalsQuote);
        strPrice << std::fixed << price;
        strStopPrice.precision(decimalsQuote);
        strStopPrice << std::fixed << stopPrice;
        
        std::ostringstream strVolume;
        strVolume.precision(decimalsBase);
        strVolume << std::fixed << volume;
        
        std::string orderType = (isStopLossLimit ? "TAKE_PROFIT_LIMIT" : "STOP_LOSS_LIMIT");
        std::string payload = "{";
        payload += "\n\t" + createJSONlabel("side", (isBuy ? "BUY" : "SELL")) + ",";
        payload += "\n\t" + createJSONlabel("quantity", strVolume.str()) + ",";
        payload += "\n\t" + createJSONlabel("price", strPrice.str()) + ",";
        payload += "\n\t" + createJSONlabel("pair", pair)+ "," ;
        payload += "\n\t" + createJSONlabel("stopPrice", strStopPrice.str()) + ",";
        payload += "\n\t" + createJSONlabel("type", orderType);
        payload +=  "\n" "}";
        
        // NOTE: you may also add "customerOrderId": "1234" to manage open orders using cusom ids
        // customerOrderId must be alphanumeric with no special chars, limit of 50 characters.

        // NOTE: you may also add "timeInForce": "GTC"
        // values of timeInForce can be only:
        //      "GTC" (Good Till Cancelled)  - this is the default
        //      "FOK" (Fill or Kill)
        //      or "IOC" (Immediate or Cancel)


        if (isBatch){
            std::string batchPayload = R"({
                "type": "PLACE_STOP_LIMIT",
                "data": )"
                + payload
                + "\n" "}";
            return batchPayload;
        }
        
        return payload;
    }

    // POST STOP LIMIT ORDER
    // Request a stop limit buy or sell order
    //
    // returns
    //     order id on success (this only means it was accepted, not successful)
    // parameters
    //     pair:         string {must be "BTCZAR", "ETHZAR" or "XRPZAR"}
    //     action:       string {must be  "BID" or "ASK" }
    //     volume:       base amount u wish to make BUY / SELL
    //     price:        quote amount u wish your order to be made at
    //     stopPrice:    quote amount which will trigger your order
    //     isStopLossLimit: if order is TAKE_PROFIT_LIMIT order or STOP_LOSS_LIMIT
    //                          NB! stop price below last traded price for:
    //                                 SELL order [makes] stop-loss order
    //                                 BUY order [makes] take-profit order
    //                          NB! stop price above last traded price for:
    //                                 SELL order [makes] take-profit order
    //                                 BUY order [makes] stop-loss order
    // NOTE:
    // fee     (for takers)
    //      for bids will be paid in base  price,
    //      for asks will be paid in quote price
    // rewards (for makers)
    //      for bids will be paid in quote price,
    //      for asks will be paid in base  price
    std::string VALRClient::postStopLimitOrder(std::string pair, std::string action, float volume, float price, float stopPrice, bool isStopLossLimit){
        std::string path = "/v1/orders/stop/limit";
        
        std::string payload = formStopLimitPayload(pair, action, volume, price, stopPrice, isStopLossLimit);

        
        std::string res = client.request("POST", (host+path).c_str(), true, VALR_EXCHANGE, path.c_str(), payload.c_str());
        
        int httpCode = client.getHttpCode();
        if (httpCode != 202)
            throw ResponseEx("Error " + std::to_string(httpCode) + " - " + res);
        
        // NOTE: recieving an id does not always mean that the order has been placed.
        // IT COULD STILL HAVE FAILED
        std::string id = extractNextString(res, 0);
        
        return id;
    }

    // Packs a list of individual payloads into a single batch payload supported by VALR exchange
    std::string VALRClient::packBatchPayloadFromList(std::vector<std::string> payloadList){
        if (payloadList.size() == 0)
            throw std::invalid_argument("Error! Unable to construct batch payload. Cannot pass empty vector to this function.");
        std::stringstream ss;
        ss << "{\n";
        ss << "\t\"requests\": [";
        for (size_t i = 0; i < payloadList.size(); i++){
            if (i > 0)
                ss << ",";
            ss << "\n" << payloadList[i];
        }
        ss << "\n]";
        ss << "\n}";
        
        return ss.str();
    }
    // POST BATCH ORDERS
    // Create a batch of multiple orders, or cancel orders, in a single request
    //
    // returns
    //     list of result for each order. NB! orders with status success only means it was accepted, not executed successfully)
    // parameters
    //     payload:     batch orders all packed into one JSON string
    //                  NB! batch allows type PLACE_MARKET, PLACE_LIMIT, PLACE_STOP LIMIT or CANCEL_ORDER only
    BatchOrderOutcome VALRClient::postBatchOrders( std::string payload){
        std::string path = "/v1/batch/orders";
        
        std::string res = client.request("POST", (host+path).c_str(), true, VALR_EXCHANGE, path.c_str(), payload.c_str());
        
        int httpCode = client.getHttpCode();
        if (httpCode != 200)
            throw ResponseEx("Error " + std::to_string(httpCode) + " - " + res);
        
        BatchOrderOutcome results;
        size_t last = 0;
        
        std::string list = extractNextStringBlock(res, last, "[", "]", last);
         
        // batchId
        results.batchId = extractNextString(res, last, "}", last);
        
        last = 0;
        while ((last = list.find("{", last)) != std::string::npos) {
            OrderOutcome singleOutcome;
            
            // accepted
            std::string token = extractNextString(list, last, ",");
                if (token == "true" || token == "false")
                    token = extractNextString(list, last, ",", last);
                else
                    token = extractNextString(list, last, "}", last);
            singleOutcome.accepted = (token == "true" ? true : false);
            
            if (singleOutcome.accepted) {
                token = extractNextStringBlock(list, last, "\"", "\"");
                if (token == "orderId")
                    // orderId
                    singleOutcome.orderId = extractNextString(list, last, last);
                    // recieving this id does not always mean that the order has been placed.
                    // IT COULD STILL HAVE FAILED
            } else {
                std::string errorBlock = extractNextStringBlock(list, last, "{", "}", last);
                size_t pos = 0;
                // code
                token = extractNextString(errorBlock, pos, ",", pos);
                singleOutcome.errorCode = atoi(token.c_str());
                
                // message
                singleOutcome.message = extractNextString(errorBlock, pos, pos);
            }

            results.orders.push_back(singleOutcome);
        }
        
        return results;
    }

    //
    //
    OrderIDOutcome VALRClient::getOrderDetails(std::string pair, std::string id, bool isCustomerOrderID){
        std::string path = "/v1/orders/" + pair;
        if (!isCustomerOrderID)
            path += "/orderid/" + id;
        else
            path += "/customerorderid/" + id;

        std::string res = client.request("GET", (host+path).c_str(), true, VALR_EXCHANGE, path.c_str());
        
        int httpCode = client.getHttpCode();
        if (httpCode != 200)
            throw ResponseEx("Error " + std::to_string(httpCode) + " - " + res);
        
        size_t last = 0;
                
        OrderIDOutcome orderStatus;
        
        // orderId
        orderStatus.orderID = extractNextString(res, last, last);
        
        // orderStatusType
        orderStatus.status = extractNextString(res, last, last);
        
        // currencyPair
        orderStatus.pair = extractNextString(res, last, last);
        
        // originalPrice
        std::string token = extractNextString(res, last, last);
        orderStatus.price = atof(token.c_str());
        
        // remainingQuantity
        token = extractNextString(res, last, last);
        orderStatus.volumeRemaining = atof(token.c_str());
        
        // originalQuantity
        token = extractNextString(res, last, last);
        orderStatus.volume = atof(token.c_str());
        
        // orderSide
        token = extractNextString(res, last, last);
        orderStatus.isBuy = (token == "buy" ? true : false);
        
        // orderType
        orderStatus.orderType = extractNextString(res, last, last);
        
        // failedReason
        orderStatus.message = extractNextString(res, last, last);

        token =  extractNextStringBlock(res, last, "\"", "\"");
        if (token == "customerOrderId"){
            // customerOrderId
            orderStatus.customerOrderID = extractNextString(res, last, last);
            
            // orderUpdatedAt
            orderStatus.lastUpdated = extractNextString(res, last, last);
            
            // orderCreatedAt
            orderStatus.timestamp = extractNextString(res, last, last);
        }
        else {
            // orderUpdatedAt
            orderStatus.lastUpdated = extractNextString(res, last, last);
            // orderCreatedAt
            orderStatus.timestamp = extractNextString(res, last, last);
        }
        
        token =  extractNextStringBlock(res, last, "\"", "\"");
        if (token == "timeInForce"){
            // timeInForce
            orderStatus.message = extractNextString(res, last, last);
        }
        
        return orderStatus;
    }

    //
    //
    std::vector<OpenOrder> VALRClient::getAllOpenOrders(){
        std::string path = "/v1/orders/open";

        std::string res = client.request("GET", (host+path).c_str(), true, VALR_EXCHANGE, path.c_str());
        
        int httpCode = client.getHttpCode();
        if (httpCode != 200)
            throw ResponseEx("Error " + std::to_string(httpCode) + " - " + res);
        
                
        std::vector<OpenOrder> orders;
        
        size_t last = 0;
        while ((last = res.find("{", last)) != std::string::npos) {
            OpenOrder order;
            
            // orderId
            order.orderID = extractNextString(res, last, last);
            
            // side
            std::string token = extractNextString(res, last, last);
            order.isBuy = (token == "buy" ? true : false);
            
            // remainingQuantity
            token = extractNextString(res, last, last);
            order.volumeRemaining = atof(token.c_str());
            
            // price
            token = extractNextString(res, last, last);
            order.price = atof(token.c_str());
            
            // currencyPair
            order.pair = extractNextString(res, last, last);
            
            // createdAt
            order.timestamp = extractNextString(res, last, last);
            
            // originalQuantity
            token = extractNextString(res, last, last);
            order.volume = atof(token.c_str());
            
            // filledPercentage
            token = extractNextString(res, last, last);
            order.filledPercentage = atof(token.c_str());
            
            // stopPrice
            token = extractNextString(res, last, last);
            order.stopPrice = atof(token.c_str());
            
            // updatedAt
            order.lastUpdated = extractNextString(res, last, last);
            
            // status
            order.status = extractNextString(res, last, last);
            
            // orderType
            order.orderType = extractNextString(res, last, last);
            
            token =  extractNextStringBlock(res, last, "\"", "\"");
            if (token == "timeInForce"){
                // timeInForce
                order.message = extractNextString(res, last, last);
            }
            
            orders.push_back(order);
        }
        return orders;
    }
}
