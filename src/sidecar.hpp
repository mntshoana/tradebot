// sidecar.hpp — C++ bridge to the Go sidecar running on localhost:8080.
//
// All functions are blocking (use a nested QEventLoop internally) and must be
// called from the main Qt thread, which is where all job lambdas execute.
//
// Credentials live in the Go sidecar (env vars). This layer has zero knowledge
// of exchange authentication, rate limiting, or wire protocols.
//
// LUNO_EXCHANGE / VALR_EXCHANGE constants are defined here so all panels that
// previously pulled them from client.hpp can just include sidecar.hpp.
#ifndef sidecar_hpp
#define sidecar_hpp

#define LUNO_EXCHANGE 0
#define VALR_EXCHANGE 1

#include <string>
#include <vector>
#include "exceptions.hpp"
#include "client/lunoclient/lunoTypes.hpp"
#include "client/valrclient/valrTypes.hpp"

namespace Sidecar {

    // ── Market (public, no credentials) ──────────────────────────────────────
    Luno::OrderBook              getLunoOrderBook(const std::string& pair);
    VALR::OrderBook              getVALROrderBook(const std::string& pair);

    std::vector<Luno::Trade>     getLunoTrades(const std::string& pair,
                                               unsigned long long since = 0);
    std::vector<VALR::Trade>     getVALRTrades(const std::string& pair,
                                               unsigned long long since = 0);

    // ── Account — Luno (authenticated) ───────────────────────────────────────
    std::vector<Luno::Balance>     getLunoBalances(const std::string& asset = "");
    std::vector<Luno::UserOrder>   getLunoOpenOrders(const std::string& pair = "");
    Luno::UserOrder                getLunoOrderDetails(const std::string& orderID);
    std::vector<Luno::Beneficiary> listLunoBeneficiaries();

    // ── Orders — Luno (authenticated) ────────────────────────────────────────
    std::string      postLunoLimitOrder(const std::string& pair,
                                        const std::string& side,  // "buy" or "sell"
                                        float price, float volume);
    bool             cancelLunoOrder(const std::string& orderID);
    Luno::Withdrawal lunoWithdraw(float amount, bool isFast,
                                  const std::string& beneficiaryID = "");

    // ── Withdrawals — Luno (authenticated) ───────────────────────────────────
    std::vector<Luno::Withdrawal> getLunoWithdrawals();
    std::string cancelLunoWithdrawal(const std::string& id);
}

#endif /* sidecar_hpp */
