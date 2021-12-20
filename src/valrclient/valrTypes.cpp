#include "valrTypes.hpp"

bool abortStatus();
namespace VALR {
    /* Account Functions Types*/
    std::string OrderBook::toString() const{
        std::stringstream ss;
        ss << "\n" << "Timestamp: " << timestamp << "\n";
        
        ss << "Asks\n";
        for (Order order : asks){
            if (!order.id.empty())
                ss << "ID: " << order.id << ",  ";
            ss << "Price: " << order.price << " ";
            ss << "Volume: " << order.volume << ",  ";
            ss << "Count: " << order.count << "\n\n";
        }

        ss << "Bids\n";
        for (Order order : this->bids){
            if (!order.id.empty())
                ss << "ID: " << order.id << ",  ";
            ss << "Price: " << order.price << " ";
            ss << "Volume: " << order.volume << ",  ";
            ss << "Count: " << order.count << "\n\n";
        }
        return ss.str();
    }
    

    std::string OrderBook::FormatHTML() {
        std::stringstream ss;
        ss << std::fixed;
        ss << R"(
                <style>
                table {width: 100%;}
                tr { padding: 15px;}
                a {
                    color: inherit;
                    text-decoration: none;
                }
                td {
                    padding: 2px 4px 1px 2px;
                    text-align: center;
                    font-size: 15px;
                    font-weight: 700;
                }
                .Ask a {color: rgb(192, 51, 35);}
                .Bid a {color: rgb(54, 136, 87);}
                .Mid {padding: 5px 2px;}
                </style>
                <table width=100%>
        )";
        
        for (auto order = this->asks.rbegin(); order != this->asks.rend(); order++){
            ss << "<tr>\n";
            ss << "<td class=Ask>";
            ss << std::setprecision(0);
            ss << "<a href=\"" << order->price << "\">";
            ss  << order->price;
            ss << "</a></td>\n";
            
            ss << "<td>"
                << std::setprecision(6)
                << "<p>" <<  order->volume << "</p>"
                << "</td>\n";
            ss << "</tr>";
        }
        ss << "\n<tr><td class=Mid colspan=2>"
                << std::setprecision(0) << (this->asks[0].price - this->bids[0].price)
                << " Spread </td></tr>";
        for (Order order : this->bids){
            ss << "\n<tr>";
            ss << "\n<td class=Bid>";
            ss << std::setprecision(0);
            ss << "<a href=\"" << order.price << "\">";
            ss << order.price;
            ss << "</a></td>";
            ss << "\n<td>" << std::setprecision(6) << order.volume<< "</td>";
            ss << "\n</tr>";
        }
        ss << "</table>\n";
        return ss.str();
    }

    std::string OrderBook::FormatHTMLWith(std::vector<UserOrder>* userOrders) {
        std::stringstream ss;
        ss << std::fixed;
        ss << R"(
                <style>
                table {width: 100%; border-collapse:collapse;}
                tr { padding: 15px;}
                a {
                    color: inherit;
                    text-decoration: none;
                }
                td {
                    padding: 2px 4px 1px 2px;
                    text-align: center;
                    font-size: 15px;
                    font-weight: 700;
                }
                .Ask a {color: rgb(192, 51, 35);}
                .Bid a {color: rgb(54, 136, 87);}
                .Mid {padding: 5px 2px;}
                </style>
                <table width=100%>
        )";
            
        std::map<int, bool> tradeOpenByUser;
            
        size_t count = userOrders->size();
            
        std::string countString = std::to_string(count);
        std::for_each(userOrders->begin(), userOrders->end(),
                      [&tradeOpenByUser](VALR::UserOrder& order) {
                        tradeOpenByUser[order.price] = true;
                        });
        for (auto order = this->asks.rbegin(); order != this->asks.rend(); order++){
            float trans = order->volume / 20.0;
            ss << "<tr "
                << "style=\""
                << "background-color:rgba(192, 51, 35, " << trans << ");\""
                << ">\n";
            ss << "<td class=Ask>";
            ss << std::setprecision(0);
            ss << "<a href=\"" << order->price << "\">";
            ss  << ((tradeOpenByUser[order->price]) ? "*" : " ") << order->price;
            ss << "</a></td>\n";
            ss << "<td> <span style=\"background: none;\">"
            << std::setprecision(6)<< order->volume << "<span></td>\n";
            ss << "</tr>";
        }
        ss << "\n<tr><td class=Mid colspan=2>"
                << std::setprecision(0) << (this->asks[0].price - this->bids[0].price)
                << " Spread </td></tr>";
        for (Order order : this->bids){
            float trans = order.volume / 20.0;
            ss << "<tr "
                << "style=\""
                << "background-color:rgba(54, 136, 87, " << trans << ");\""
                << ">\n";
            ss << "<td class=Bid>";
            ss << std::setprecision(0);
            ss << "<a href=\"" << order.price << "\">";
            ss  << ((tradeOpenByUser[order.price]) ? "*" : " ") << order.price;
            ss << "</a></td>\n";
            ss << "<td> <span style=\"background: none;\">"
            << std::setprecision(6) << order.volume<< "<span></td>\n";
            ss << "</tr>";
        }
        ss << "</table>\n";
        return ss.str();
    }


    std::string CurrencyInfo::toString() const{
        std::stringstream ss;
        ss << "Symbol: " << symbol << "\n";
        ss << "Is Active: " << isActive << "\n";
        ss << "Short Name: " << shortName << "\n";
        ss << "Long Name: " << longName << "\n";
        ss << "Decimals: " << decimalCount << " ";
        ss << "(withdrawal: " << withdrawalDecimalCount << ")\n";
        return ss.str();
    }
   


    std::string CurrencyPairInfo::toString()const{
        std::stringstream ss;
        ss << "Symbol: " << symbol << "\n";
        ss << "Base currency: " << baseCurrency << "\n";
        ss << "Quote currency: " << quoteCurrency << "\n";
        ss << "Is active: " << isActive << "\n";
        ss << "Short name: " << shortName << "\n";
        ss << "Minimum base amount: " << baseMinTradable << "\n";
        ss << "Maximum base amount: " << baseMaxTradable << "\n";
        ss << "Minimum quote amount: " << quoteMinTradable << "\n";
        ss << "Maximum base amount: " << quoteMaxTradable << "\n";
        ss << "Tick size: " << tickSize << "\n";
        ss << "Base decimal places: " << baseDecimalCount << "\n";
        
        return ss.str();
    }

    std::string OrderTypeInfo::toString()const{
        std::stringstream ss;
        ss << "Symbol: " << symbol << "\n";
        ss << "Order Types: " << orderTypes << "\n";
        
        return ss.str();
    }

    std::string Ticker::toString() const {
        std::stringstream ss;
        ss << "Pair: " << pair << "\n";
        ss << "Timestamp: " << timestamp << "\n";
        ss << "Bid: " << bid << "\n";
        ss << "Ask: " << ask << "\n";
        ss << "Last trade: " << lastTrade << "\n";
        ss << "Last closing price: " << lastClosed << "\n";
        ss << "Last high price: " << high << "\n";
        ss << "Last low price: " << low << "\n";
        ss << "Base volume: " << baseVolume << "\n";
        ss << "Change from previous: " << changeFromPrevious << "\n";
        return ss.str();
    }

    std::string Trade::toString(std::string formatType) const {
        std::stringstream ss;
        if (formatType == "csv"){
            ss << sequence << ", "
            << timestamp << ", "
            << price << ", "
            << baseVolume << ", "
            << isBuy
               << "\n";
        }
        else {
            ss << "Pair: " << pair << " - timesamp " << timestamp << "\n";
            ss << "Price: " << price << "\n";
            ss << "Volume: " << baseVolume << " (base)\n";
            ss << "Volume: " << quoteVolume << " (quote)\n";
            ss << "Sequence: " << sequence << "\n";
            ss << "Trade ID: " << id << "\n";
            ss << "Is buy: " << (isBuy ? "true":"false") << "\n";
        }
        return ss.str();
    }

    std::fstream& operator << (std::fstream& stream, std::vector<Trade>& trades){
        for (Trade& trade : trades)
            stream << trade.toString("csv");
        return stream;
    }
    std::fstream& operator >> (std::fstream& stream, std::vector<Trade>& trades){
        std::string line, token;
        size_t index = 0;
        while (getline(stream, line)) {
            if (abortStatus())
                return stream;
            index = 0;
            std::stringstream s(line);
            while (getline(s, token, ',')) {
                if (index == 0){
                    trades.push_back(Trade());
                    trades.back().sequence = atoll(token.c_str());
                }
                if (index == 1)
                    trades.back().timestamp = atoll(token.c_str());
                if (index == 2)
                    trades.back().price = atof(token.c_str());
                if (index == 3)
                    trades.back().baseVolume = atof(token.c_str());
                if (index == 4)
                    trades.back().isBuy = atoi(token.c_str());
                index++;
            }
        }
        return stream;
    }

    /* Account Function Types*/
    std::string KeyInfo::toString() const {
        std::stringstream ss;
        ss << "Label: " << label << std::endl;
        ss << "Permissions: " << permission << std::endl;
        ss << "Added at: " << createdAt << std::endl;
        ss << "Is sub account: " << (isSubAccount ? "True" : "False") << std::endl;
        ss << "Allowed IP Address" << allowedIP << std::endl;
        ss << "Allowed Withdraw" << allowedWithdraw << std::endl;
        return ss.str();
    }

    std::string Account::toString() const {
        std::stringstream ss;
        ss << "Label: " << label << std::endl;
        ss << "Id: " << id << std::endl;
        return ss.str();
    }

    std::string Balance::toString() const {
        std::stringstream ss;
        ss << "Asset: " << asset << "\n";
        ss << "Balance: " << balance << "\n";
        ss << "Reserved: " << reserved << "\n";
        ss << "Available: " << available << "\n";
        ss << "Last confirmed: " << lastUpdated << "\n";
        return ss.str();
    }

    std::string AccountSummary::toString() const{
        std::stringstream ss;
        ss << "Account Info... " << "\n";
        ss << account.toString() << "\n";
        
        for (const Balance& bal : balances){
            ss << bal.toString() << "\n";
        }
        if (balances.empty())
            ss << "Balances: [no non-zero assets]\n";
        
        return ss.str();
    }

    std::string TransactionInfo::toString() const{
        std::stringstream ss;
        ss << "Transaction Info... " << "\n";
        ss << "Type: " << type << "\n";
        ss << "Description: " << description << "\n";
        ss << "ID: " << id << "\n";
        
        if (creditAsset != ""){
            ss << "Credited "  << creditValue <<  " (" <<  creditAsset << ")\n";
        }
        if (debitAsset != ""){
            ss << "Debited "  << debitValue <<  " (" <<  debitAsset << ")\n";
        }
        if (feeAsset != ""){
            ss << "Charged "  << feeValue <<  " (" <<  feeAsset << ")\n";
        }
        ss << " Timestamp " << timestamp << "\n";
        
        if (additionalInfo != ""){
            ss << " Additional Info: " << additionalInfo;
        }
        
        return ss.str();
    }

    /* Order Function Types*/
    std::string UserOrder::toString() const{
        std::stringstream ss;
        ss << "ID: : " << orderID << "\n";
        ss << "Created: " << createdTime
            << ", expired: " << expirationTime
            << ", completed: " << completedTime << "\n";
        ss << "Type: " << type << "\n";
        ss << "State: " << state << "\n";
        ss << "Price: " << price << "\n";
        ss << "Volume: " << volume << "\n";
        ss << "Base: " << baseValue
        << ", fee: " << baseFee << "\n";
        ss << "Counter: " << counterValue
        << ", fee: " << counterFee << "\n";
        ss << "Pair: " << pair << "\n";
        return ss.str();
    }

    std::string UserTrade::toString() const{
        std::stringstream ss;
        ss << Trade::toString();
        ss << "Order ID: " << orderID << "\n";

        return ss.str();
    }

    /* Wallet Function Types */
    std::string AddressEntry::toString() const{
        std::stringstream ss;
        ss << "Label: " << label << "\n";
        ss << "Asset: " << asset << "\n";
        ss << "Address: " << address << "\n";
        ss << "Timestamp: " <<  timestamp << "\n";
        return ss.str();
    }

    std::string WithdrawalDetail::toString() const{
        std::stringstream ss;
        ss << "Asset: " << asset << "\n";
        ss << "Minimum withdrawal allowed: " << min << "\n";
        ss << "Withdrawal decimal places: " << decimals << "\n";
        ss << "Is active? : " <<  (isActive ? "YES" : "NO") << "\n";
        ss << "Withdrawal fee : " <<  fee << "\n";
        ss << "Supports payment reference? : " <<  (supportsReference ? "YES" : "NO") << "\n";
        return ss.str();
    }

    std::string WithdrawalInfo::toString() const{
        std::stringstream ss;
        ss << "Asset: " << asset << "\n";
        ss << "Address: " << address << "\n";
        ss << "Amount: " << amount << "\n";
        ss << "Fee: " << fee << "\n";
        ss << "Transaction hash: " << transactionHash << "\n";
        ss << "Confirmations: " << confirmations << "\n";
        ss << "Last confirmation at: " << lastConfrimationAt << "\n";
        ss << "ID: " << id << "\n";
        ss << "Created at: " << timestamp << "\n";
        ss << "Is Verified? : " << (isVerified ? "Yes" : "No") << "\n";
        ss << "Status: " << status << "\n";
        
        return ss.str();
    }

    std::string BankInfo::toString() const {
        std::stringstream ss;
        ss << "ID: " << id << "\n";
        ss << "Bank: " << bank << "\n";
        ss << "Account holder: " << accountHolder << "\n";
        ss << "Account number: " << accountNumber << "\n";
        ss << "Branch code: " << branchCode << "\n";
        ss << "Account type" << accountType << "\n";
        ss << "Created at: " << timestamp << "\n";
        
        return ss.str();
    }

    std::string InternationalBankInfo::toString() const {
        std::stringstream ss;
        ss << "ID: " << id << "\n";
        ss << "Account number: " << accountNumber << "\n";
        if (routingNumber != "")
            ss << "Routing number: " << routingNumber << "\n";
        ss << "Billing details: " << billingDetails << "\n";
        ss << "Status: " << status << "\n";
        ss << "Type: " << type << "\n";
        ss << "Created at: " << timestamp << "\n";
        
        return ss.str();
    }

    std::string InternationalBankInstructions::toString() const {
        std::stringstream ss;
        ss << "Tracking reference: " << reference << "\n";
        ss << "Beneficiary: \n " << beneficiary << "\n";
        ss << "Beneficiary bank: \n " << beneficiaryBank << "\n";
        
        return ss.str();
    }

    /* Order Functions */
    std::string OrderMatched::toString() const {
            std::stringstream ss;
            ss << "Volume: " << volume  <<"\t";
            ss << "at price: " << price <<  "\n";
            return ss.str();
    }

    std::string SimpleQuote::toString() const {
        std::stringstream ss;
        ss << "Pair: " << pair  <<"\n";
        ss << "You exchange: " << amount <<  "\n";
        ss << "You will recieve: " << willRecieve <<  "\n";
        ss << "You will be charged: " << fee <<  " " << feeAsset << "\n";
        ss << "Timestamp: " << timestamp << "\n";
        ss << "Quote ID: " << id << "\n";
        ss << "Will be matching: \n";
        for (const OrderMatched& order : ordersMatched)
            ss << order.toString();
        ss << std::endl;
        
        return ss.str();
    }
    
    std::string ExcercisedQuote::toString() const{
        return std::to_string(code) + ": " + message;
    }

    std::string SimpleOrderStatus::toString() const {
        std::stringstream ss;
        ss << "ID: " << id  <<"\n";
        ss << "Is complete: "<<  success << "\n";
        ss << "Is processing: "<<  processing << "\n";
        ss << "Amount paid: " <<  paidAmount << " " << paidAsset << "\n";
        ss << "Amount received: "<<  receivedAmount << " " << receivedAsset<< "\n";
        ss << "Fee charged: " <<  fee << " " << feeAsset << "\n";
        ss << "Timestamp: "<<  timestamp << "\n";
        ss << std::endl;
        
        return ss.str();
    }

    std::string VALR_PAY_Result::toString() const {
        std::stringstream ss;
        ss << "Identifier: " << id << "\n";
        ss << "Transaction ID: " << transactionID << "\n";
        return ss.str();
    }

    std::string PaymentLimitInfo::toString() const {
        std::stringstream ss;
        ss << "Maximum amount limited to: " << maxAmount << "\n";
        ss << "Minimum amount limited to: " << minAmount << "\n";
        ss << "For currency: " << asset << "\n";
        ss << "How is this limit applied: " << limitedByWhat << "\n";
        return ss.str();
    }

    std::string VALR_PAY_History_Entry::toString() const {
        std::stringstream ss;
        ss << "Transaction ID: " << transactionID << "\n";
        ss << "Timestamp: " << timestamp << "\n";
        ss << "Payment ID: " << paymentID << "\n";
        ss << "Other party's ID: " << otherPartysID << "\n";
        ss << "Amount: " << amount << "\n";
        ss << "Type: " << type << "\t " << (isAnonymous ? "(anonymous)\n" : "\n");
        if (status.length() > 0)
            ss << "Status: " << status << "\n";
        if (myReference.length() > 0)
            ss << "My reference: " << myReference << "\n";
        if (beneficiaryReference.length() > 0)
            ss << "Beneficiary reference: " << beneficiaryReference << "\n";
        
        return ss.str();
    }

    std::string OrderOutcome::toString() const {
        std::stringstream ss;
        ss << "Accepted: " << (accepted ? "true" : "false") << "\n";
        if (accepted){
            if (orderId.length() > 0){
            ss << "Order ID: " << orderId << "\n";
            }
        }
        else {
            ss << "Error code: " << errorCode << "\n";
            ss << "With message: " << message << "\n";
        }
    
        return ss.str();
    }

std::string BatchOrderOutcome::toString() const {
    std::stringstream ss;
    ss << "Batch Result...\n";
    for (size_t i = 0; i < orders.size(); i++)
        ss << orders[i].toString() << "\n";
    ss << "Batch ID: " << batchId << "\n";
    return ss.str();
}

}
printableDefinition(VALR::OrderBook);
printableList(VALR::OrderBook);
printableDefinition(VALR::CurrencyInfo);
printableList(VALR::CurrencyInfo);
printableDefinition(VALR::CurrencyPairInfo);
printableList(VALR::CurrencyPairInfo);
printableDefinition(VALR::OrderTypeInfo);
printableList(VALR::OrderTypeInfo);
printableDefinition(VALR::Ticker);
printableList(VALR::Ticker);
printableDefinition(VALR::Trade);
printableList(VALR::Trade);

/* Account Functions */
printableDefinition(VALR::KeyInfo);
printableList(VALR::KeyInfo);
printableDefinition(VALR::Account);
printableList(VALR::Account);
printableDefinition(VALR::Balance);
printableList(VALR::Balance);
printableDefinition(VALR::AccountSummary);
printableList(VALR::AccountSummary);
printableDefinition(VALR::TransactionInfo);
printableList(VALR::TransactionInfo);
printableDefinition(VALR::UserTrade);
printableList(VALR::UserTrade);

/* Wallet Functions */
printableDefinition(VALR::AddressEntry);
printableList(VALR::AddressEntry);
printableDefinition(VALR::WithdrawalDetail);
printableList(VALR::WithdrawalDetail);
printableDefinition(VALR::WithdrawalInfo);
printableList(VALR::WithdrawalInfo);
printableDefinition(VALR::BankInfo);
printableList(VALR::BankInfo);
printableDefinition(VALR::InternationalBankInfo);
printableList(VALR::InternationalBankInfo);
printableDefinition(VALR::InternationalBankInstructions);
printableList(VALR::InternationalBankInstructions);

/* Order Functions */
printableDefinition(VALR::OrderMatched);
printableList(VALR::OrderMatched);
printableDefinition(VALR::SimpleQuote);
printableList(VALR::SimpleQuote);
printableDefinition(VALR::ExcercisedQuote);
printableList(VALR::ExcercisedQuote);
printableDefinition(VALR::SimpleOrderStatus);
printableList(VALR::SimpleOrderStatus);
printableDefinition(VALR::OrderOutcome);
printableList(VALR::OrderOutcome);
printableDefinition(VALR::BatchOrderOutcome);
printableList(VALR::BatchOrderOutcome);

/* Pay Service Functions */
printableDefinition(VALR::VALR_PAY_Result);
printableList(VALR::VALR_PAY_Result);
printableDefinition(VALR::PaymentLimitInfo);
printableList(VALR::PaymentLimitInfo);
printableDefinition(VALR::VALR_PAY_History_Entry);
printableList(VALR::VALR_PAY_History_Entry);

printableDefinition(VALR::UserOrder);
printableList(VALR::UserOrder);
