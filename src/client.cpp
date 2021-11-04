#include "client.hpp"
#include "hmac.hpp"
#include <chrono>
#include <locale>
#include <iomanip>
#include <ctime>

#include "textPanel.hpp"


// to calculate time since epoch (seconds)
decltype(std::chrono::seconds().count()) get_seconds_since_epoch()
{
    const auto now = std::chrono::system_clock::now();
    // duration since the epoch
    const auto epoch = now.time_since_epoch();
    // casted into seconds
    const auto seconds = std::chrono::duration_cast<std::chrono::seconds>(epoch);
    // return the number of seconds
    return seconds.count();
}

// parse iso8601 string format into time since epoch (seconds)
long long get_seconds_since_epoch(std::string time) {
    std::tm t = {};
    std::istringstream ss(time);

    ss >> std::get_time(&t, "%Y-%m-%dT%H:%M:%S");
    return std::mktime(&t);
}

// parse iso8601 string format into time since epoch (seconds)
std::string get_timestamp_iso8601_string(long seconds) {
    time_t now {seconds}; // limited to long by time_t
    char buf[sizeof "2011-10-08T07:07:09Z"];
    strftime(buf, sizeof buf, "%FT%TZ", gmtime(&now));
    return buf;
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



std::string Client::request (const char* method, const char* uri, bool auth, int exchange, const char* payload) {
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
            #include "credentials.hpp"
            if (exchange == LUNO_EXCHANGE){
                // HTTP basic authentication
                curl_easy_setopt(curl, CURLOPT_USERNAME, LUNO_USERNAME);
                curl_easy_setopt(curl, CURLOPT_PASSWORD, LUNO_PASSWORD);
            }
            if (exchange == VALR_EXCHANGE){
                // HMAC SHA512
                std::string timestamp = std::to_string( get_seconds_since_epoch());
                std::string signature = hmac::get_hmac(VALR_PASSWORD, timestamp + payload);
                headers = curl_slist_append(headers, "X-VALR-API-KEY: " VALR_USERNAME);
                headers = curl_slist_append(headers, ("X-VALR-SIGNATURE: " + signature).c_str());
                headers = curl_slist_append(headers, ("X-VALR-TIMESTAMP: " + timestamp).c_str());
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
