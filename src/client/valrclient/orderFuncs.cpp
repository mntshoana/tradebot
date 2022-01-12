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
        quote.pair = extractNextString(res, last, last, "currencyPair");
        
        // payAmount
        std::string token = extractNextString(res, last, last, "payAmount");
        quote.amount = std::atof(token.c_str());
        
        // receiveAmount
        token = extractNextString(res, last, last, "receiveAmount");
        quote.willRecieve = std::atof(token.c_str());
        
        // fee
        token = extractNextString(res, last, last, "fee");
        quote.fee = std::atof(token.c_str());
        
        // feeCurrency
        quote.feeAsset = extractNextString(res, last, last, "feeCurrency");
        
        // createdAt
        quote.timestamp  = extractNextString(res, last, last, "createdAt");
        
        // id
        quote.id  = extractNextString(res, last, last, "id");
        
        std::string matched = extractNextStringBlock(res, last, "[", "]", last, "matched");
        
        last = 0;
        while ((last = matched.find("{", last)) != std::string::npos) {
            OrderMatched order;
            
            // price
            token = extractNextString(matched, last, last, "price");
            order.price = std::atof(token.c_str());
            
            // quantity
            token = extractNextString(matched, last, last, "quantity");
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
        std::string token = extractNextString(res, last, last, "code");
        execution.code = atoi(token.c_str());
        
        // message
        execution.message = extractNextString(res, last, last, "message");
        
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
        orderStatus.id = extractNextString(res, last, last, "orderId");
        
        // success
        std::string token = extractNextString(res, last, ",", last, "success");
        orderStatus.success = (token == "true" ? true : false);
        
        // processing
        token = extractNextString(res, last, ",", last, "processing");
        orderStatus.processing = (token == "true" ? true : false);
        
        // paidAmount
        token = extractNextString(res, last, last, "paidAmount");
        orderStatus.paidAmount = atof(token.c_str());
        
        // paidCurrency
        orderStatus.paidAsset = extractNextString(res, last, last, "paidCurrency");
        
        // receivedAmount
        token = extractNextString(res, last, last, "receivedAmount");
        orderStatus.receivedAmount = atof(token.c_str());
        
        // receivedCurrency
        orderStatus.receivedAsset = extractNextString(res, last, last, "receivedCurrency");
        
        // feeAmount
        token = extractNextString(res, last, last, "feeAmount");
        orderStatus.fee = atof(token.c_str());
        
        // feeCurrency
        orderStatus.feeAsset = extractNextString(res, last, last, "feeCurrency");
        
        // feeCurrency
        orderStatus.timestamp = extractNextString(res, last, last, "feeCurrency");
        
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
        std::string id = extractNextString(res, 0, "id");
        
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
        std::string id = extractNextString(res, 0, "id");
        
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
        std::string id = extractNextString(res, 0, "id");
        
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
        
        // outcomes
        std::string list = extractNextStringBlock(res, last, "[", "]", last, "outcomes");
         
        // batchId
        results.batchId = extractNextString(res, last, "}", last, "batchId");
        
        last = 0;
        while ((last = list.find("{", last)) != std::string::npos) {
            OrderOutcome singleOutcome;
            
            // accepted
            std::string token = extractNextString(list, last, ",", "accepted");
                if (token == "true" || token == "false")
                    token = extractNextString(list, last, ",", last, "accepted");
                else
                    token = extractNextString(list, last, "}", last, "accepted");
            singleOutcome.accepted = (token == "true" ? true : false);
            
            if (singleOutcome.accepted) {
                token = extractNextStringBlock(list, last, "\"", "\"", nullptr);
                if (token == "orderId")
                    // orderId
                    singleOutcome.orderId = extractNextString(list, last, last, "orderId");
                    // recieving this id does not always mean that the order has been placed.
                    // IT COULD STILL HAVE FAILED
            } else {
                std::string errorBlock = extractNextStringBlock(list, last, "{", "}", last, "error");
                size_t pos = 0;
                // code
                token = extractNextString(errorBlock, pos, ",", pos, "code");
                singleOutcome.errorCode = atoi(token.c_str());
                
                // message
                singleOutcome.message = extractNextString(errorBlock, pos, pos, "message");
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
        orderStatus.orderID = extractNextString(res, last, last, "orderId");
        
        // orderStatusType
        orderStatus.status = extractNextString(res, last, last, "orderStatusType");
        
        // currencyPair
        orderStatus.pair = extractNextString(res, last, last, "currencyPair");
        
        // originalPrice
        std::string token = extractNextString(res, last, last, "originalPrice");
        orderStatus.price = atof(token.c_str());
        
        // remainingQuantity
        token = extractNextString(res, last, last, "remainingQuantity");
        orderStatus.volumeRemaining = atof(token.c_str());
        
        // originalQuantity
        token = extractNextString(res, last, last, "originalQuantity");
        orderStatus.volume = atof(token.c_str());
        
        // orderSide
        token = extractNextString(res, last, last, "orderSide");
        orderStatus.isBuy = (token == "buy" ? true : false);
        
        // orderType
        orderStatus.orderType = extractNextString(res, last, last, "orderType");
        
        // failedReason
        orderStatus.message = extractNextString(res, last, last, "failedReason");

        token =  extractNextStringBlock(res, last, "\"", "\"", nullptr);
        if (token == "customerOrderId"){
            // customerOrderId
            orderStatus.customerOrderID = extractNextString(res, last, last, "customerOrderId");
            
            // orderUpdatedAt
            orderStatus.lastUpdated = extractNextString(res, last, last, "orderUpdatedAt");
            
            // orderCreatedAt
            orderStatus.timestamp = extractNextString(res, last, last, "orderCreatedAt");
        }
        else {
            // orderUpdatedAt
            orderStatus.lastUpdated = extractNextString(res, last, last, "orderUpdatedAt");
            // orderCreatedAt
            orderStatus.timestamp = extractNextString(res, last, last, "orderCreatedAt");
        }
        
        token =  extractNextStringBlock(res, last, "\"", "\"", nullptr);
        if (token == "timeInForce"){
            // timeInForce
            orderStatus.timeInForce = extractNextString(res, last, last, "timeInForce");
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
            order.orderID = extractNextString(res, last, last, "orderId");
            
            // side
            std::string token = extractNextString(res, last, last, "side");
            order.isBuy = (token == "buy" ? true : false);
            
            // remainingQuantity
            token = extractNextString(res, last, last, "remainingQuantity");
            order.volumeRemaining = atof(token.c_str());
            
            // price
            token = extractNextString(res, last, last, "price");
            order.price = atof(token.c_str());
            
            // currencyPair
            order.pair = extractNextString(res, last, last, "currencyPair");
            
            // createdAt
            order.timestamp = extractNextString(res, last, last, "createdAt");
            
            // originalQuantity
            token = extractNextString(res, last, last, "originalQuantity");
            order.volume = atof(token.c_str());
            
            // filledPercentage
            token = extractNextString(res, last, last, "filledPercentage");
            order.filledPercentage = atof(token.c_str());
            
            token =  extractNextStringBlock(res, last, "\"", "\"", nullptr);
            if (token == "stopPrice"){
                // stopPrice
                token = extractNextString(res, last, last, "stopPrice");
                order.stopPrice = atof(token.c_str());
            }
            
            // updatedAt
            order.lastUpdated = extractNextString(res, last, last, "updatedAt");
            
            // status
            order.status = extractNextString(res, last, last, "status");
            
            // orderType
            order.orderType = extractNextString(res, last, last, "orderType");
            
            token =  extractNextStringBlock(res, last, "\"", "\"", nullptr);
            if (token == "timeInForce"){
                // timeInForce
                order.message = extractNextString(res, last, last, "timeInForce");
            }
            
            orders.push_back(order);
        }
        return orders;
    }

    //
    // Get historical orders placed by the user.
    std::vector<OrderHistory> VALRClient::getUserOrderHistory(int skip, int limit){
        std::string path = "/v1/orders/history";
        
        int args = 0;
        if (skip > 0) {
            path += (args++ >= 1) ? "&" : "?";
            path += "skip=";
            path += std::to_string(skip);
        }
        if (limit > 0) {
            path += (args++ >= 1) ? "&" : "?";
            path += "limit=";
            path += std::to_string(limit);
        }

        std::string res = client.request("GET", (host+path).c_str(), true, VALR_EXCHANGE, path.c_str());
        
        int httpCode = client.getHttpCode();
        if (httpCode != 200)
            throw ResponseEx("Error " + std::to_string(httpCode) + " - " + res);
        
        std::vector<OrderHistory> orders;
        
        size_t last = 0;
        while ((last = res.find("{", last)) != std::string::npos) {
            OrderHistory order;
            
            // orderId
            order.orderID = extractNextString(res, last, last, "orderId");
            
            std::string token =  extractNextStringBlock(res, last, "\"", "\"", nullptr);
            if (token == "customerOrderId"){
                // customerOrderId
                order.customerOrderID = extractNextString(res, last, last, "customerOrderId");
            }
            
            // orderStatusType
            order.status = extractNextString(res, last, last, "orderStatusType");
            
            // currencyPair
            order.pair = extractNextString(res, last, last, "currencyPair");
            
            // averagePrice
            token = extractNextString(res, last, last, "averagePrice");
            order.avgPrice = atof(token.c_str());
            
            // originalPrice
            token = extractNextString(res, last, last, "originalPrice");
            order.price = atof(token.c_str());
            
            // remainingQuantity
            token = extractNextString(res, last, last, "remainingQuantity");
            order.volumeRemaining = atof(token.c_str());
            
            // originalQuantity
            token = extractNextString(res, last, last, "originalQuantity");
            order.volume = atof(token.c_str());
            
            // total
            token = extractNextString(res, last, last, "total");
            order.total = atof(token.c_str());
            
            // totalFee
            token = extractNextString(res, last, last, "totalFee");
            order.totalFee = atof(token.c_str());
            
            // feeCurrency
            order.feeAsset = extractNextString(res, last, last, "feeCurrency");
            
            // orderSide
            token = extractNextString(res, last, last, "orderSide");
            order.isBuy = (token == "buy" ? true : false);
            
            // orderType
            order.orderType = extractNextString(res, last, last, "orderType");
            
            // failedReason
            order.message = extractNextString(res, last, last, "failedReason");
        
            // orderUpdatedAt
            order.lastUpdated = extractNextString(res, last, last, "orderUpdatedAt");
            
            // orderCreatedAt
            order.timestamp = extractNextString(res, last, last, "orderCreatedAt");
            
            token =  extractNextStringBlock(res, last, "\"", "\"", nullptr);
            if (token == "timeInForce"){
                // timeInForce
                order.timeInForce = extractNextString(res, last, last, "timeInForce");
            }
            
            orders.push_back(order);
        }
        return orders;
    }

    //
    // Get historical orders placed by the user.
    OrderHistory VALRClient::getUserOrderByID(std::string id, bool isCustomerOrderID){
        std::string path = "/v1/orders/history/summary";
        
        if (!isCustomerOrderID)
            path += "/orderid/" + id;
        else
            path += "/customerorderid/" + id;

        std::string res = client.request("GET", (host+path).c_str(), true, VALR_EXCHANGE, path.c_str());
        
        int httpCode = client.getHttpCode();
        if (httpCode != 200)
            throw ResponseEx("Error " + std::to_string(httpCode) + " - " + res);
        
        size_t last = 0;

        OrderHistory order;
        
        // orderId
        order.orderID = extractNextString(res, last, last, "orderId");
        
        std::string token =  extractNextStringBlock(res, last, "\"", "\"", nullptr);
        if (token == "customerOrderId"){
            // customerOrderId
            order.customerOrderID = extractNextString(res, last, last, "customerOrderId");
        }
        
        // orderStatusType
        order.status = extractNextString(res, last, last, "orderStatusType");
        
        // currencyPair
        order.pair = extractNextString(res, last, last, "currencyPair");
        
        // averagePrice
        token = extractNextString(res, last, last, "averagePrice");
        order.avgPrice = atof(token.c_str());
        
        // originalPrice
        token = extractNextString(res, last, last, "originalPrice");
        order.price = atof(token.c_str());
        
        // remainingQuantity
        token = extractNextString(res, last, last, "remainingQuantity");
        order.volumeRemaining = atof(token.c_str());
        
        // originalQuantity
        token = extractNextString(res, last, last, "originalQuantity");
        order.volume = atof(token.c_str());
        
        // total
        token = extractNextString(res, last, last, "total");
        order.total = atof(token.c_str());
        
        // totalFee
        token = extractNextString(res, last, last, "totalFee");
        order.totalFee = atof(token.c_str());
        
        // feeCurrency
        order.feeAsset = extractNextString(res, last, last, "feeCurrency");
        
        // orderSide
        token = extractNextString(res, last, last, "orderSide");
        order.isBuy = (token == "buy" ? true : false);
        
        // orderType
        order.orderType = extractNextString(res, last, last, "orderType");
        
        // failedReason
        order.message = extractNextString(res, last, last, "failedReason");
    
        // orderUpdatedAt
        order.lastUpdated = extractNextString(res, last, last, "orderUpdatedAt");
        
        // orderCreatedAt
        order.timestamp = extractNextString(res, last, last, "orderCreatedAt");
        
        token =  extractNextStringBlock(res, last, "\"", "\"", nullptr);
        if (token == "timeInForce"){
            // timeInForce
            order.timeInForce = extractNextString(res, last, last, "timeInForce");
        }
        
        return order;
    }

    // FORM LIMIT ORDER PAYLOAD
    // Creates the json payload string that allows on to make a LIMIT order on VALR
    //
    std::string VALRClient::formCancelOrderPayload(std::string pair, std::string id, bool isCustomerOrderID, bool isBatch){
        
        std::string label;
        if (!isCustomerOrderID)
            label = "orderId";
        else
            label = "customerOrderId";
        
        std::string payload = "{";
        payload += "\n\t" + createJSONlabel(label, id) + ",";
        payload += "\n\t" + createJSONlabel("pair", pair);
        payload +=  "\n" "}";
        
        if (isBatch){
            std::string batchPayload = R"({
                "type": "CANCEL_ORDER",
                "data": )"
                + payload
                + "\n" "}";
            return batchPayload;
        }
        return payload;
    }

    //
    // Get historical orders placed by the user.
    void VALRClient::cancelOrder(std::string pair, std::string id, bool isCustomerOrderID){
        std::string path = "/v1/orders/order";

        std::string payload = formCancelOrderPayload(pair, id, isCustomerOrderID);

        std::string res = client.request("DELETE", (host+path).c_str(), true, VALR_EXCHANGE, path.c_str(), payload.c_str());
        
        int httpCode = client.getHttpCode();
        if (httpCode != 200)
            throw ResponseEx("Error " + std::to_string(httpCode) + " - " + res);
        
        // order was accepted. (nothing to return)
        // Do use the other order status API to receive clearer a status about this result of this cancel request.
        return;
    }
}
