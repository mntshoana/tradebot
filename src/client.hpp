
#ifndef client_hpp
#define client_hpp

#include <QTextEdit>
#include <curl/curl.h>
#include <sstream>
#include <iomanip>
#include <vector>
#include <fstream>
#include <filesystem>
#include <QNetworkProxy>

#include "exceptions.hpp"
#include "objectivec.h"

/* Internet client for accessing the REST API */
class Client {
protected:
    CURL *curl;
    CURLcode res; // with respect to curl's attempt to communicate
    int httpCode; // after successful communication with server
    
    virtual std::string  request (const char* method, const char* uri, bool auth = false);
    static size_t redirect (char *ptr, size_t size, size_t nmemb, void *stream);
public:
    static bool abort; // exit a tediously long task
    
    static std::stringstream buffer;
    Client();
    ~Client();
    
};
#endif /* client_hpp */
