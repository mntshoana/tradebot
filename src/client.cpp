#include "client.hpp"

std::stringstream Client::ss = std::stringstream();

Client::Client() {
    curl = curl_easy_init();
    curl_easy_setopt(curl, CURLOPT_DEFAULT_PROTOCOL, "https");
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, redirect);
}
Client::~Client() {
    curl_easy_cleanup(curl);
}

size_t Client::redirect (char *ptr, size_t size, size_t nmemb, void *stream) {
    ss << ptr;
    return size * nmemb;
}

std::string Client::request (const char* method, const char* uri, bool auth) {
    ss.str(""); // clear stream
    if(curl) {
        curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, method);
        curl_easy_setopt(curl, CURLOPT_URL, uri);
        if (auth){
        #include "credentials.hpp"
            curl_easy_setopt(curl, CURLOPT_USERNAME, MY_USERNAME);
            curl_easy_setopt(curl, CURLOPT_PASSWORD, MY_PASSWORD);
        #undef MY_USERNAME
        #undef MY_PASSWORD
        }
        res = curl_easy_perform(curl);
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &httpCode);
        if (res != CURLE_OK){
            ss << "curl_easy_perform() failed: "
             << curl_easy_strerror(res)
             << "\n";
        }

    }
    return ss.str();
}
