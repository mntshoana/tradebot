#ifndef CLIENT_INTERFACE_HEADER
#define CLIENT_INTERFACE_HEADER

#include <string>
// types
class OrderType {
public:
    virtual std::string getType() const = 0;
    virtual std::string getID() const = 0;
    virtual long long getTimestamp() const = 0;
    virtual float getPrice() const = 0;
    virtual float getVolume() const = 0;
    virtual float getBaseValue() const = 0;
    virtual float getQuoteValue() const = 0;
    
};

class BalanceType {
public:
    virtual std::string getAccountID() const = 0;
    virtual std::string getAsset() const = 0;
    virtual float getBalance() const = 0;
    virtual float getReserved() const = 0;
    virtual float getUncomfirmed() const = 0;
};

#endif /*CLIENT_INTERFACE_HEADER*/
