
#ifndef client_hpp
#define client_hpp

#define LUNO_EXCHANGE 0
#define VALR_EXCHANGE 1

#include <curl/curl.h>
#include <sstream>

#include <vector>

#include <QNetworkProxy>

#include "exceptions.hpp"
#include "objectivec.h"


/* Internet client for accessing the REST API */
class Client {
protected:
    CURL *curl;
    CURLcode res; // with respect to curl's attempt to communicate
    int httpCode; // after successful communication with server
    
    static size_t redirect (char *ptr, size_t size, size_t nmemb, void *stream);
public:
    static bool abort; // exit a tediously long task
    
    static std::stringstream buffer;
    Client();
    ~Client();
    int getHttpCode();
    virtual std::string  request (const char* method, const char* uri,  bool auth = false, int exchange = LUNO_EXCHANGE, const char* payload = "", const char* body = "");
    
};

bool abortStatus();
long long get_seconds_since_epoch(std::string time);
std::string get_timestamp_iso8601_string(long seconds);
#endif /* client_hpp */
