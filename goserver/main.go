// TradeBot Go sidecar — Phase 2
//
// Exchange-agnostic REST + WebSocket API alongside the C++ Qt app.
// Credentials are loaded from environment variables:
//
//	LUNO_API_KEY / LUNO_API_SECRET
//	VALR_API_KEY / VALR_API_SECRET
//	PORT (optional, defaults to 8080)
//
// Routes:
//
//	GET  /ws                             WebSocket: streams orderbook + trades
//	GET  /market/orderbook?exchange=luno&pair=XBTZAR
//	GET  /market/ticker?exchange=luno&pair=XBTZAR
//	GET  /market/trades?exchange=luno&pair=XBTZAR[&since=<seq>]
//	GET  /account/balances?exchange=luno
//	GET  /account/orders/open?exchange=luno[&pair=XBTZAR]
//	POST /account/orders/limit          body: PostLimitOrderRequest JSON
//	DEL  /account/orders/{id}?exchange=luno[&pair=BTCZAR]  (pair required for VALR)
package main

import (
	"encoding/json"
	"fmt"
	"log"
	"net/http"
	"strconv"
)

var cfg Config
var luno *lunoClient
var valr *valrClient

func main() {
	cfg = loadConfig()
	luno = newLunoClient(cfg)
	valr = newVALRClient(cfg)

	startScheduler(luno, valr)

	mux := http.NewServeMux()

	// WebSocket push stream
	mux.HandleFunc("GET /ws", handleWs)

	// Market — public, no credentials needed
	mux.HandleFunc("GET /market/orderbook", handleOrderBook)
	mux.HandleFunc("GET /market/ticker", handleTicker)
	mux.HandleFunc("GET /market/trades", handleTrades)

	// Account — requires exchange credentials
	mux.HandleFunc("GET /account/balances", handleBalances)
	mux.HandleFunc("GET /account/orders/open", handleOpenOrders)

	// Order management — requires exchange credentials
	mux.HandleFunc("POST /account/orders/limit", handlePostLimitOrder)
	mux.HandleFunc("DELETE /account/orders/{id}", handleCancelOrder)

	// Single order detail, beneficiaries, withdrawals
	mux.HandleFunc("GET /account/orders/{id}", handleGetOrderDetail)
	mux.HandleFunc("GET /account/beneficiaries", handleBeneficiaries)
	mux.HandleFunc("POST /account/withdrawals", handleWithdraw)
	mux.HandleFunc("GET /account/withdrawals", handleGetWithdrawals)
	mux.HandleFunc("DELETE /account/withdrawals/{id}", handleCancelWithdrawal)

	addr := ":" + cfg.Port
	fmt.Printf("TradeBot sidecar listening on %s\n", addr)
	log.Fatal(http.ListenAndServe(addr, mux))
}

// ── helpers ──────────────────────────────────────────────────────────────────

func writeJSON(w http.ResponseWriter, status int, v any) {
	w.Header().Set("Content-Type", "application/json")
	w.WriteHeader(status)
	json.NewEncoder(w).Encode(v)
}

func writeError(w http.ResponseWriter, status int, msg string) {
	writeJSON(w, status, ErrorResponse{Error: msg})
}

// ── market handlers ───────────────────────────────────────────────────────────

func handleOrderBook(w http.ResponseWriter, r *http.Request) {
	exchange := r.URL.Query().Get("exchange")
	pair := r.URL.Query().Get("pair")
	if exchange == "" || pair == "" {
		writeError(w, http.StatusBadRequest, "exchange and pair query params required")
		return
	}

	switch exchange {
	case "luno":
		ob, err := luno.GetOrderBook(pair)
		if err != nil {
			writeError(w, http.StatusBadGateway, err.Error())
			return
		}
		writeJSON(w, http.StatusOK, ob)
	case "valr":
		ob, err := valr.GetOrderBook(pair)
		if err != nil {
			writeError(w, http.StatusBadGateway, err.Error())
			return
		}
		writeJSON(w, http.StatusOK, ob)
	default:
		writeError(w, http.StatusBadRequest, "unknown exchange: "+exchange)
	}
}

func handleTicker(w http.ResponseWriter, r *http.Request) {
	exchange := r.URL.Query().Get("exchange")
	pair := r.URL.Query().Get("pair")
	if exchange == "" || pair == "" {
		writeError(w, http.StatusBadRequest, "exchange and pair query params required")
		return
	}

	switch exchange {
	case "luno":
		t, err := luno.GetTicker(pair)
		if err != nil {
			writeError(w, http.StatusBadGateway, err.Error())
			return
		}
		writeJSON(w, http.StatusOK, t)
	case "valr":
		t, err := valr.GetTicker(pair)
		if err != nil {
			writeError(w, http.StatusBadGateway, err.Error())
			return
		}
		writeJSON(w, http.StatusOK, t)
	default:
		writeError(w, http.StatusBadRequest, "unknown exchange: "+exchange)
	}
}

func handleTrades(w http.ResponseWriter, r *http.Request) {
	exchange := r.URL.Query().Get("exchange")
	pair := r.URL.Query().Get("pair")
	if exchange == "" || pair == "" {
		writeError(w, http.StatusBadRequest, "exchange and pair query params required")
		return
	}

	var since int64
	if s := r.URL.Query().Get("since"); s != "" {
		since, _ = strconv.ParseInt(s, 10, 64)
	}

	switch exchange {
	case "luno":
		trades, err := luno.GetTrades(pair, since)
		if err != nil {
			writeError(w, http.StatusBadGateway, err.Error())
			return
		}
		writeJSON(w, http.StatusOK, trades)
	case "valr":
		trades, err := valr.GetTrades(pair, since)
		if err != nil {
			writeError(w, http.StatusBadGateway, err.Error())
			return
		}
		writeJSON(w, http.StatusOK, trades)
	default:
		writeError(w, http.StatusBadRequest, "unknown exchange: "+exchange)
	}
}

// ── account handlers ──────────────────────────────────────────────────────────

func handleBalances(w http.ResponseWriter, r *http.Request) {
	exchange := r.URL.Query().Get("exchange")

	switch exchange {
	case "luno":
		balances, err := luno.GetBalances()
		if err != nil {
			writeError(w, http.StatusBadGateway, err.Error())
			return
		}
		writeJSON(w, http.StatusOK, balances)
	case "valr":
		balances, err := valr.GetBalances()
		if err != nil {
			writeError(w, http.StatusBadGateway, err.Error())
			return
		}
		writeJSON(w, http.StatusOK, balances)
	default:
		writeError(w, http.StatusBadRequest, "exchange query param required (luno or valr)")
	}
}

func handleOpenOrders(w http.ResponseWriter, r *http.Request) {
	exchange := r.URL.Query().Get("exchange")
	pair := r.URL.Query().Get("pair") // optional

	switch exchange {
	case "luno":
		orders, err := luno.GetOpenOrders(pair)
		if err != nil {
			writeError(w, http.StatusBadGateway, err.Error())
			return
		}
		writeJSON(w, http.StatusOK, orders)
	case "valr":
		orders, err := valr.GetOpenOrders(pair)
		if err != nil {
			writeError(w, http.StatusBadGateway, err.Error())
			return
		}
		writeJSON(w, http.StatusOK, orders)
	default:
		writeError(w, http.StatusBadRequest, "exchange query param required (luno or valr)")
	}
}

// ── order management handlers ─────────────────────────────────────────────────

func handlePostLimitOrder(w http.ResponseWriter, r *http.Request) {
	var req PostLimitOrderRequest
	if err := json.NewDecoder(r.Body).Decode(&req); err != nil {
		writeError(w, http.StatusBadRequest, "invalid JSON body: "+err.Error())
		return
	}
	if req.Exchange == "" || req.Pair == "" || req.Side == "" || req.Price <= 0 || req.Amount <= 0 {
		writeError(w, http.StatusBadRequest, "exchange, pair, side, price and volume are all required")
		return
	}
	if req.Side != "buy" && req.Side != "sell" {
		writeError(w, http.StatusBadRequest, `side must be "buy" or "sell"`)
		return
	}

	switch req.Exchange {
	case "luno":
		resp, err := luno.PostLimitOrder(req.Pair, req.Side, req.Price, req.Amount)
		if err != nil {
			writeError(w, http.StatusBadGateway, err.Error())
			return
		}
		writeJSON(w, http.StatusCreated, resp)
	case "valr":
		resp, err := valr.PostLimitOrder(req.Pair, req.Side, req.Price, req.Amount)
		if err != nil {
			writeError(w, http.StatusBadGateway, err.Error())
			return
		}
		writeJSON(w, http.StatusCreated, resp)
	default:
		writeError(w, http.StatusBadRequest, "unknown exchange: "+req.Exchange)
	}
}

func handleCancelOrder(w http.ResponseWriter, r *http.Request) {
	id := r.PathValue("id")
	if id == "" {
		writeError(w, http.StatusBadRequest, "order id is required in path")
		return
	}
	exchange := r.URL.Query().Get("exchange")
	pair := r.URL.Query().Get("pair") // required for VALR

	switch exchange {
	case "luno":
		if err := luno.CancelOrder(id); err != nil {
			writeError(w, http.StatusBadGateway, err.Error())
			return
		}
		w.WriteHeader(http.StatusNoContent)
	case "valr":
		if pair == "" {
			writeError(w, http.StatusBadRequest, "pair query param required when cancelling a VALR order")
			return
		}
		if err := valr.CancelOrder(pair, id); err != nil {
			writeError(w, http.StatusBadGateway, err.Error())
			return
		}
		w.WriteHeader(http.StatusNoContent)
	default:
		writeError(w, http.StatusBadRequest, "exchange query param required (luno or valr)")
	}
}

func handleGetOrderDetail(w http.ResponseWriter, r *http.Request) {
	id := r.PathValue("id")
	exchange := r.URL.Query().Get("exchange")

	switch exchange {
	case "luno":
		order, err := luno.GetOrderDetails(id)
		if err != nil {
			writeError(w, http.StatusBadGateway, err.Error())
			return
		}
		writeJSON(w, http.StatusOK, order)
	default:
		writeError(w, http.StatusBadRequest, "exchange query param required (luno)")
	}
}

func handleBeneficiaries(w http.ResponseWriter, r *http.Request) {
	exchange := r.URL.Query().Get("exchange")

	switch exchange {
	case "luno":
		resp, err := luno.ListBeneficiaries()
		if err != nil {
			writeError(w, http.StatusBadGateway, err.Error())
			return
		}
		writeJSON(w, http.StatusOK, resp)
	default:
		writeError(w, http.StatusBadRequest, "exchange query param required (luno)")
	}
}

func handleWithdraw(w http.ResponseWriter, r *http.Request) {
	var req PostWithdrawalRequest
	if err := json.NewDecoder(r.Body).Decode(&req); err != nil {
		writeError(w, http.StatusBadRequest, "invalid JSON body: "+err.Error())
		return
	}
	if req.Amount <= 0 {
		writeError(w, http.StatusBadRequest, "amount must be > 0")
		return
	}

	switch req.Exchange {
	case "luno":
		resp, err := luno.Withdraw(req.Amount, req.IsFast, req.BeneficiaryID)
		if err != nil {
			writeError(w, http.StatusBadGateway, err.Error())
			return
		}
		writeJSON(w, http.StatusCreated, resp)
	default:
		writeError(w, http.StatusBadRequest, "unknown exchange: "+req.Exchange)
	}
}

func handleGetWithdrawals(w http.ResponseWriter, r *http.Request) {
	exchange := r.URL.Query().Get("exchange")

	switch exchange {
	case "luno":
		resp, err := luno.GetWithdrawals()
		if err != nil {
			writeError(w, http.StatusBadGateway, err.Error())
			return
		}
		writeJSON(w, http.StatusOK, resp)
	default:
		writeError(w, http.StatusBadRequest, "exchange query param required (luno)")
	}
}

func handleCancelWithdrawal(w http.ResponseWriter, r *http.Request) {
	id := r.PathValue("id")
	exchange := r.URL.Query().Get("exchange")

	switch exchange {
	case "luno":
		if err := luno.CancelWithdrawal(id); err != nil {
			writeError(w, http.StatusBadGateway, err.Error())
			return
		}
		w.WriteHeader(http.StatusNoContent)
	default:
		writeError(w, http.StatusBadRequest, "exchange query param required (luno)")
	}
}
