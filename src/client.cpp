#include "lunoclient.hpp"
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

std::string Client::request (const char* method, const char* uri, bool auth) {
    buffer.str(""); // clear stream
    if(curl) {
        curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, method);
        curl_easy_setopt(curl, CURLOPT_URL, uri);
        curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1L); // Don't use signals as this always locks easy_curl_perform
        curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 2L);

        std::string proxyStr = proxyInfo();
        if (!proxyStr.empty())
            curl_easy_setopt(curl, CURLOPT_PROXY, proxyStr.c_str());
        
        if (auth){
        #include "credentials.hpp"
            curl_easy_setopt(curl, CURLOPT_USERNAME, MY_USERNAME);
            curl_easy_setopt(curl, CURLOPT_PASSWORD, MY_PASSWORD);
        #undef MY_USERNAME // security issue (need to find a better way)
        #undef MY_PASSWORD // security issue (need to find a better way)
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
