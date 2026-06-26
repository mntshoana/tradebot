// sidecar.cpp — implementation of the Go sidecar bridge.
//
// Uses Qt Network (already a project dependency) for HTTP and QJsonDocument
// for parsing — no libcurl, no hand-rolled JSON extraction.
//
// Each doRequest call spins a nested QEventLoop until the reply arrives.
// This is the standard Qt pattern for synchronous network calls; it is safe
// here because all job lambdas run on the main thread via QTimer.

#include "sidecar.hpp"

#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QNetworkProxy>
#include <QEventLoop>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QUrl>
#include <QByteArray>
#include <QBuffer>
#include <QString>

static const std::string BASE = "http://localhost:8080";

// ── internal HTTP helper ──────────────────────────────────────────────────────

static std::string doRequest(const char* method,
                              const std::string& url,
                              const std::string& body = "") {
    QNetworkAccessManager mgr;
    mgr.setProxy(QNetworkProxy::NoProxy); // never route localhost through system proxy

    QNetworkRequest req(QUrl(QString::fromStdString(url)));
    req.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QNetworkReply* reply = nullptr;
    std::string m(method);

    if (m == "GET") {
        reply = mgr.get(req);
    } else if (m == "POST") {
        reply = mgr.post(req, QByteArray(body.c_str(), (int)body.size()));
    } else {
        // DELETE with an optional JSON body
        reply = mgr.sendCustomRequest(req,
                    QByteArray(method),
                    QByteArray(body.c_str(), (int)body.size()));
    }

    QEventLoop loop;
    QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    loop.exec();

    std::string result   = QString::fromUtf8(reply->readAll()).toStdString();
    int         status   = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    bool        netError = (reply->error() != QNetworkReply::NoError);
    std::string errStr   = reply->errorString().toStdString();
    // mgr destructor owns the reply — do not call deleteLater

    if (netError && status == 0) {
        throw ResponseEx("Sidecar unavailable: " + errStr
                       + " — is the Go sidecar running? (task run:sidecar)");
    }
    if (status >= 400) {
        throw ResponseEx("Sidecar " + std::to_string(status) + ": " + result);
    }
    return result;
}

static QJsonObject parseObj(const std::string& json) {
    return QJsonDocument::fromJson(
               QByteArray(json.c_str(), (int)json.size())).object();
}

// ── Sidecar namespace ─────────────────────────────────────────────────────────

namespace Sidecar {

std::vector<Luno::Balance> getLunoBalances(const std::string& asset) {
    auto obj = parseObj(doRequest("GET",
        BASE + "/account/balances?exchange=luno"));

    std::vector<Luno::Balance> result;
    for (const QJsonValue& v : obj["balances"].toArray()) {
        if (!asset.empty() && v["asset"].toString().toStdString() != asset)
            continue;
        Luno::Balance b;
        b.accountID   = v["accountId"].toString().toStdString();
        b.asset       = v["asset"].toString().toStdString();
        b.balance     = (float)v["free"].toDouble();
        b.reserved    = (float)v["used"].toDouble();
        b.uncomfirmed = (float)v["unconfirmed"].toDouble();
        result.push_back(b);
    }
    return result;
}

std::vector<Luno::UserOrder> getLunoOpenOrders(const std::string& pair) {
    std::string url = BASE + "/account/orders/open?exchange=luno";
    if (!pair.empty())
        url += "&pair=" + pair;
    auto obj = parseObj(doRequest("GET", url));

    std::vector<Luno::UserOrder> result;
    for (const QJsonValue& v : obj["orders"].toArray()) {
        Luno::UserOrder o;
        o.orderID      = v["id"].toString().toStdString();
        o.pair         = v["symbol"].toString().toStdString();
        o.type         = (v["side"].toString() == "buy") ? "BID" : "ASK";
        o.state        = "PENDING";
        o.price        = (float)v["price"].toDouble();
        o.volume       = (float)v["amount"].toDouble();
        o.baseValue    = (float)v["filled"].toDouble();
        o.createdTime  = v["createdAt"].toVariant().toLongLong();
        o.counterValue = 0.0f;
        o.baseFee      = 0.0f;
        o.counterFee   = 0.0f;
        result.push_back(o);
    }
    return result;
}

Luno::UserOrder getLunoOrderDetails(const std::string& orderID) {
    auto obj = parseObj(doRequest("GET",
        BASE + "/account/orders/" + orderID + "?exchange=luno"));

    Luno::UserOrder o;
    o.orderID      = obj["id"].toString().toStdString();
    o.pair         = obj["symbol"].toString().toStdString();
    o.type         = (obj["side"].toString() == "buy") ? "BID" : "ASK";
    o.state        = obj["status"].toString().toStdString();
    o.price        = (float)obj["price"].toDouble();
    o.volume       = (float)obj["amount"].toDouble();
    o.baseValue    = (float)obj["filled"].toDouble();
    o.createdTime  = obj["createdAt"].toVariant().toLongLong();
    o.counterValue = 0.0f;
    o.baseFee      = 0.0f;
    o.counterFee   = 0.0f;
    return o;
}

std::vector<Luno::Beneficiary> listLunoBeneficiaries() {
    auto obj = parseObj(doRequest("GET",
        BASE + "/account/beneficiaries?exchange=luno"));

    std::vector<Luno::Beneficiary> result;
    for (const QJsonValue& v : obj["beneficiaries"].toArray()) {
        Luno::Beneficiary b;
        b.id                    = v["id"].toString().toStdString();
        b.bank                  = v["bank"].toString().toStdString();
        b.branch                = v["branch"].toString().toStdString();
        b.accountNumber         = v["accountNumber"].toString().toStdString();
        b.accountHolder         = v["accountHolder"].toString().toStdString();
        b.accountType           = v["accountType"].toString().toStdString();
        b.country               = v["country"].toString().toStdString();
        b.supportsFastWithdrawal = v["isFast"].toBool();
        b.timestamp             = 0;
        result.push_back(b);
    }
    return result;
}

// ── Orders — Luno ─────────────────────────────────────────────────────────────

std::string postLunoLimitOrder(const std::string& pair,
                                const std::string& side,
                                float price, float volume) {
    std::string body = "{\"exchange\":\"luno\",\"pair\":\"" + pair
                     + "\",\"side\":\"" + side
                     + "\",\"price\":" + std::to_string(price)
                     + ",\"amount\":" + std::to_string(volume) + "}";
    auto obj = parseObj(doRequest("POST", BASE + "/account/orders/limit", body));
    return obj["orderId"].toString().toStdString();
}

bool cancelLunoOrder(const std::string& orderID) {
    try {
        doRequest("DELETE",
                  BASE + "/account/orders/" + orderID + "?exchange=luno");
        return true;
    } catch (const ResponseEx&) {
        return false;
    }
}

Luno::Withdrawal lunoWithdraw(float amount, bool isFast,
                               const std::string& beneficiaryID) {
    std::string body = "{\"exchange\":\"luno\",\"asset\":\"ZAR\""
                     + std::string(",\"amount\":") + std::to_string(amount)
                     + ",\"isFast\":" + (isFast ? "true" : "false");
    if (!beneficiaryID.empty())
        body += ",\"beneficiaryId\":\"" + beneficiaryID + "\"";
    body += "}";

    auto obj = parseObj(doRequest("POST", BASE + "/account/withdrawals", body));

    Luno::Withdrawal w;
    w.id          = obj["id"].toString().toLongLong();
    w.status      = obj["status"].toString().toStdString();
    w.createdTime = obj["createdAt"].toVariant().toLongLong();
    w.currency    = "ZAR";
    w.amount      = amount;
    w.fee         = 0.0f;
    return w;
}

std::vector<Luno::Withdrawal> getLunoWithdrawals() {
    auto obj = parseObj(doRequest("GET",
        BASE + "/account/withdrawals?exchange=luno"));

    std::vector<Luno::Withdrawal> result;
    for (const QJsonValue& v : obj["withdrawals"].toArray()) {
        Luno::Withdrawal w;
        w.id          = v["id"].toVariant().toLongLong();
        w.status      = v["status"].toString().toStdString();
        w.type        = v["type"].toString().toStdString();
        w.currency    = v["currency"].toString().toStdString();
        w.amount      = (float)v["amount"].toDouble();
        w.fee         = (float)v["fee"].toDouble();
        w.createdTime = v["createdAt"].toVariant().toLongLong();
        result.push_back(w);
    }
    return result;
}

std::string cancelLunoWithdrawal(const std::string& id) {
    doRequest("DELETE",
              BASE + "/account/withdrawals/" + id + "?exchange=luno");
    return "";
}

} // namespace Sidecar
