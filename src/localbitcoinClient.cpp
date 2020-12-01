#include "localbitcoinClient.hpp"

namespace LocalBitcoin {

    std::string LocalBitcoinClient::request (const char* method, const char* uri, bool auth) {
        buffer.str(""); // clear stream
        if(curl) {
            std::string proxyStr = proxyInfo();
            if (!proxyStr.empty())
                curl_easy_setopt(curl, CURLOPT_PROXY, proxyStr.c_str());
            else {
                buffer << "Requires proxy.\n";
                return buffer.str();
            }
            
            curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, method);
            curl_easy_setopt(curl, CURLOPT_URL, uri);
            curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1L); // Don't use signals as this always locks easy_curl_perform
            curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 5L);
            curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

            
            curl_slist *headers = NULL;
            if (auth){
            #include "credentials.hpp"
                long long timestamp = QDateTime::currentDateTime().toSecsSinceEpoch();
                const char* endpoint = uri + strlen("https://localbitcoins.com");
                std::string signature = hmac<SHA256>(std::to_string(timestamp) + LKEY + endpoint, LSECRET );
                headers = curl_slist_append(headers, "Apiauth-Key: " LKEY);
                headers = curl_slist_append(headers,
                            ("Apiauth-Nonce: " + std::to_string(timestamp)).c_str());
                headers = curl_slist_append(headers, ("Apiauth-Signature: " + signature).c_str());
                curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
            #undef MY_USERNAME
            #undef MY_PASSWORD
            #undef LKEY
            #undef LSECRET
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

    std::string LocalBitcoinClient::getUserAds(int visible, std::string type, std::string currency, std::string country) {
        std::string uri = "https://localbitcoins.com/api/ads/";
        
        // add visible, type, currency and country when provided
        int args = 0;
        if (visible != -1){
            uri += (args++ ? "&" : "?");
            uri += "visible=" + std::to_string((bool)visible);
        }
        if (type != ""){ // type = LOCAL_SELL, LOCAL_BUY, ONLINE_SELL, ONLINE_BUY
            uri += (args++ ? "&" : "?");
            uri += "trade_type=" + type;
        }
        if (currency != ""){
            uri += (args++ ? "&" : "?");
            uri += "currency=" + currency;
        }
        if (country != ""){
            uri += (args++ ? "&" : "?");
            uri += "countrycode" + country;
        }
        std::string res = this->request("GET", uri.c_str(), true);
        
        if (httpCode != 200)
            throw ResponseEx("Error " + std::to_string(httpCode) + " - " + res);
        
        return res;
    }
    std::string LocalBitcoinClient::getUserAd(std::string adID){
        std::string uri = "https://localbitcoins.com/api/ad-get/" + adID + "/";
        std::string res = this->request("GET", uri.c_str(), true);
        
        if (httpCode != 200)
            throw ResponseEx("Error " + std::to_string(httpCode) + " - " + res);
        
        return res;
    }

    std::string LocalBitcoinClient::getWallet(){
        std::string uri = "https://localbitcoins.com/api/wallet/";
        std::string res = this->request("GET", uri.c_str(), true);
        
        if (httpCode != 200)
            throw ResponseEx("Error " + std::to_string(httpCode) + " - " + res);
        
        return res;
    }

    std::string LocalBitcoinClient::getBuyAds (std::string countryCode, std::string country, std::string paymentMethod){
        std::string uri = "https://localbitcoins.com/buy-bitcoins-online/";
        uri += countryCode + "/";
        uri += country + "/";
        if (paymentMethod != "")
            uri += paymentMethod + "/";
        uri += ".json";
        std::string res = this->request("GET", uri.c_str());
        
        if (httpCode != 200)
            throw ResponseEx("Error " + std::to_string(httpCode) + " - " + res);
        
        return res;
    }

    std::string LocalBitcoinClient::paymentMethods(){
        std::string uri = "https://localbitcoins.com/api/payment_methods/";
        std::string res = this->request("GET", uri.c_str());
        
        if (httpCode != 200)
            throw ResponseEx("Error " + std::to_string(httpCode) + " - " + res);
        
        return res;
    }
}
