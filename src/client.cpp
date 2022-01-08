#include "client.hpp"
#include "hmac.hpp"
#include <chrono>
#include <locale>
#include <iomanip>
#include <ctime>

#include "textPanel.hpp"


// to calculate time since epoch (milliseconds)
decltype(std::chrono::milliseconds().count()) get_seconds_since_epoch()
{
    const auto now = std::chrono::system_clock::now();
    // duration since the epoch
    const auto epoch = now.time_since_epoch();
    // casted into seconds
    const auto mseconds = std::chrono::duration_cast<std::chrono::milliseconds>(epoch);
    // return the number of seconds
    return mseconds.count();
}

// parse iso8601 string format into time since epoch (seconds)
long long get_seconds_since_epoch(std::string time) {
    std::tm t = {};
    std::istringstream ss(time);

    ss >> std::get_time(&t, "%Y-%m-%dT%H:%M:%S"); // note! this is not very precise, but it will do
    return std::mktime(&t);
}

// parse iso8601 string format into time since epoch (seconds)
std::string get_timestamp_iso8601_string(long seconds) {
    time_t now {seconds}; // limited to long by time_t
    char buf[sizeof "2021-12-02T21:47:42.717496Z"];
    strftime(buf, sizeof buf, "%FT%TZ", gmtime(&now));
    return buf;
}

// Creates a single line string for JSON marshalling
// ex: "value":"key"
std::string createJSONlabel(std::string label, std::string key){
    return R"(")" + label + R"(":")" + key + R"(")" ;
}

// Creates a single line string for JSON marshalling
// ex: "value":"key"
std::string createJSONlabelUnquoted(std::string label, std::string key){
    return R"(")" + label + R"(":)" + key ;
}

void verifyLabel(const char* label, std::string source, size_t colonPos){
    size_t length = strlen(label);
    if (length <= 0)
        throw std::invalid_argument("Expecting a valid json label but recieved an empty string!");
    
    int spaceAdjustment = 0;
    while (source[colonPos-spaceAdjustment] != '"')
           spaceAdjustment++;
    while (source[colonPos-spaceAdjustment] != '"')
           spaceAdjustment++;
    
    size_t pos = colonPos - length - spaceAdjustment;
   // "asks" :
    std::string expectedString = source.substr(pos, length);
    if (strcmp(label, expectedString.c_str()) != 0)
        throw std::invalid_argument("Expecting json label \"" + std::string(label) + "\" but encountered \"" + expectedString +"\"!");

    return;
}

// only returns portion of string immediately following a colon
//   this string will be within opening ["] and ending ["] delimiters
std::string extractNextString(std::string source, size_t start, size_t& jumpTo, const char* label) {
    size_t posStart = source.find(":", start);
    verifyLabel(label, source, posStart);
    if (posStart != std::string::npos){
        posStart = source.find("\"", posStart) + 1;
        size_t posEnd = source.find("\"", posStart);
        jumpTo = posEnd + 1;
        return source.substr(posStart, posEnd-posStart);
    }
    jumpTo = std::string::npos;
    return "";
}

std::string extractNextString(std::string source, size_t start, const char* label) {
    return extractNextString(source, start, start, label);
}

// only returns portion of string immediately following a colon
//   this string is not surrounded by any quotations marks.
//   the ending position is marked by a delimiter
std::string extractNextString(std::string source, size_t start, const char* readUntil, size_t& jumpTo, const char* label) {
    size_t posStart = source.find(":", start);
    verifyLabel(label, source, posStart);

    if (posStart != std::string::npos) {
        posStart += 1;
        if (source[posStart] == ' ')
            posStart++;
        size_t posEnd = source.find(readUntil, posStart);
        jumpTo = posEnd + 1;
        return source.substr(posStart, posEnd-posStart);
    }
    jumpTo = std::string::npos;
    return "";
}

std::string extractNextString(std::string source, size_t start, const char* readUntil, const char* label){
    return extractNextString(source, start, readUntil, start, label);
}

//  returns portion of string immediately following a delimiter
//   and ending by a delimiter
std::string extractNextStringBlock(std::string source, size_t start,
                                   const char* startDelim, const char* stopDelim, size_t& jumpTo, const char* label) {
    size_t posStart = source.find(startDelim, start);
    bool isNamelessJavaObject = (strcmp(startDelim, "{") == 0 && strcmp(stopDelim, "}") == 0 && label == nullptr);
    bool isNamelessJavaArray = (strcmp(startDelim, "[") == 0 && strcmp(stopDelim, "]") == 0  && label == nullptr);
    bool isNamelessJavaString = (strcmp(startDelim, "\"") == 0 && strcmp(stopDelim, "\"") == 0  && label == nullptr);
    
    if ( !( isNamelessJavaObject || isNamelessJavaArray || isNamelessJavaString)  )
        verifyLabel(label, source, posStart);

    if (posStart != std::string::npos) {
        posStart += 1;
        size_t posEnd = source.find(stopDelim, posStart);
        jumpTo = posEnd + 1;
        return source.substr(posStart, posEnd-posStart);
    }
    jumpTo = std::string::npos;
    return "";
}
std::string extractNextStringBlock(std::string source, size_t start,
                                   const char* startDelim, const char* stopDelim, const char* label ) {
    return extractNextStringBlock(source, start, startDelim, stopDelim, start, label);
}



Client client;

bool Client::abort = false;

std::stringstream Client::buffer = std::stringstream();

Client::Client() {
    curl = nullptr;
    curl = curl_easy_init();
    curl_easy_setopt(curl, CURLOPT_DEFAULT_PROTOCOL, "https");
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, redirect);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *) &buffer);
}
Client::~Client() {
    curl_easy_cleanup(curl);
}

size_t Client::redirect (char *ptr, size_t size, size_t nmemb, void *stream) {
    *((std::stringstream*)stream) << ptr;
    return size * nmemb;
}

int Client::getHttpCode(){
    return httpCode;
}



std::string Client::request (const char* method, const char* uri, bool auth, int exchange, const char* payload, const char* body) {
    buffer.str(""); // clear stream
    if(curl) {
        curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, method);
        curl_easy_setopt(curl, CURLOPT_URL, uri);
        curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1L); // Don't use signals as this always locks easy_curl_perform
        curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 2L);
        curl_easy_setopt(curl, CURLOPT_DEFAULT_PROTOCOL, "https");
        
        // USE SYSTEM PROXY
        std::string proxyStr = proxyInfo();
        if (!proxyStr.empty())
            curl_easy_setopt(curl, CURLOPT_PROXY, proxyStr.c_str());
                
        struct curl_slist *headers = NULL; // Required for VALR

        if (auth){
            // Please note the following file is not included. ADD IT YOURSELF. It is a header file within in the src folder and it defines the following string literals
            //      LUNO_USERNAME, LUNO_PASSWORD, VALR_PASSWORD, VALR_USERNAME, LKEY (localbitcoin) and LSECRET.
            //      If you intend not to them, define them in ur file to be empty strings ""
            #include "credentials.hpp"
            if (exchange == LUNO_EXCHANGE){
                // HTTP basic authentication
                curl_easy_setopt(curl, CURLOPT_USERNAME, LUNO_USERNAME);
                curl_easy_setopt(curl, CURLOPT_PASSWORD, LUNO_PASSWORD);
            }
            if (exchange == VALR_EXCHANGE){
                // HMAC SHA512
                std::string timestamp = std::to_string( get_seconds_since_epoch());
                std::string signature = hmac::get_hmac(VALR_PASSWORD, timestamp + method + payload + body);
                headers = curl_slist_append(headers, "X-VALR-API-KEY: " VALR_USERNAME);
                headers = curl_slist_append(headers, ("X-VALR-SIGNATURE: " + signature).c_str());
                headers = curl_slist_append(headers, ("X-VALR-TIMESTAMP: " + timestamp).c_str());
                curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
                
                if ( ( strcmp(method, "POST") == 0 || strcmp(method, "DELETE") == 0 )
                    && strlen(body) > 0){
                    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, body);
                }

            }
            #undef LUNO_USERNAME // security issue (need to find a better way)
            #undef LUNO_PASSWORD // security issue (need to find a better way)
            #undef VALR_USERNAME// security issue (need to find a better way)
            #undef VALR_PASSWORD // security issue (need to find a better way)
            #undef LKEY // security issue (need to find a better way)
            #undef LSECRET // security issue (need to find a better way)
        }
        res = curl_easy_perform(curl);
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &httpCode);
        if (res != CURLE_OK){
            buffer << "curl_easy_perform() failed: "
             << curl_easy_strerror(res)
             << "\n";
        }

    }
    return buffer.str();
}


bool abortStatus(){
    return Client::abort;
}
