package server

import (
	"encoding/json"
	"fmt"
	"log"
	"net/http"
	"strconv"
)

// package-level singletons initialised by Start.
var (
	cfg        Config
	luno       *lunoClient
	valr       *valrClient
	bybit      *bybitClient
	binance    *binanceClient
	bybitp2p   *bybitP2PClient
	binancep2p *binanceP2PClient
	lcsp2p     *lcsP2PClient
)

// Start loads config, wires up the exchange clients, launches the scheduler
// goroutines, and blocks serving the HTTP/WebSocket API.
// Call it in its own goroutine from main: go server.Start()
func Start() {
	cfg = loadConfig()
	luno = newLunoClient(cfg)
	valr = newVALRClient(cfg)
	bybit = newBybitClient(cfg)
	binance = newBinanceClient(cfg)
	bybitp2p = newBybitP2PClient(cfg)
	binancep2p = newBinanceP2PClient(cfg)
	lcsp2p = newLCSP2PClient(cfg)

	startScheduler(luno, valr, bybit, binance)

	mux := http.NewServeMux()

	// WebSocket push stream
	mux.HandleFunc("GET /ws", handleWs)

	// Market — public, no credentials needed
	mux.HandleFunc("GET /market/pairs", handlePairs)
	mux.HandleFunc("GET /market/orderbook", handleOrderBook)
	mux.HandleFunc("GET /market/ticker", handleTicker)
	mux.HandleFunc("GET /market/trades", handleTrades)
	mux.HandleFunc("GET /market/fees", handleFees)
	mux.HandleFunc("GET /market/zar-prices", handleZARPrices)
	// VALR page proxy — strips X-Frame-Options so the exchange page can be
	// embedded as an iframe.  Matches /valr-proxy/ and all sub-paths.
	mux.HandleFunc("/valr-proxy/", handleVALRProxy)

	// P2P — public dropdown data for Bybit P2P and LocalCoinSwap P2P
	mux.HandleFunc("GET /p2p/cryptos", handleP2PCryptos)
	mux.HandleFunc("GET /p2p/fiats", handleP2PFiats)
	mux.HandleFunc("GET /p2p/payment-methods", handleP2PPaymentMethods)
	// P2P — LCS content tabs (ads listing, my ads, trade history)
	mux.HandleFunc("GET /p2p/lcs/ads", handleLCSAds)
	mux.HandleFunc("GET /p2p/lcs/my-ads", handleLCSMyAds)
	mux.HandleFunc("GET /p2p/lcs/trades", handleLCSTrades)
	// P2P — Bybit P2P public listings + authenticated tabs
	mux.HandleFunc("GET /p2p/bybit/ads", handleBybitAds)
	mux.HandleFunc("GET /p2p/bybit/my-ads", handleBybitMyAds)
	mux.HandleFunc("GET /p2p/bybit/trades", handleBybitTrades)
	// P2P — Binance P2P public listings + authenticated trade history
	mux.HandleFunc("GET /p2p/binance/ads", handleBinanceAds)
	mux.HandleFunc("GET /p2p/binance/trades", handleBinanceTrades)

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
	fmt.Printf("TradeBot HTTP/WS server listening on %s\n", addr)
	log.Fatal(http.ListenAndServe(addr, corsMiddleware(mux)))
}

// corsMiddleware allows the Wails/React webview to call the API.
func corsMiddleware(next http.Handler) http.Handler {
	return http.HandlerFunc(func(w http.ResponseWriter, r *http.Request) {
		w.Header().Set("Access-Control-Allow-Origin", "*")
		w.Header().Set("Access-Control-Allow-Methods", "GET, POST, DELETE, OPTIONS")
		w.Header().Set("Access-Control-Allow-Headers", "Content-Type")
		if r.Method == http.MethodOptions {
			w.WriteHeader(http.StatusNoContent)
			return
		}
		next.ServeHTTP(w, r)
	})
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

func handlePairs(w http.ResponseWriter, r *http.Request) {
	exchange := r.URL.Query().Get("exchange")
	var pairs []PairInfo
	var err error
	switch exchange {
	case "luno":
		pairs, err = luno.GetPairs()
	case "valr":
		pairs, err = valr.GetPairs()
	case "bybit":
		pairs, err = bybit.GetPairs()
	case "binance":
		pairs, err = binance.GetPairs()
	default:
		http.Error(w, "unknown exchange", http.StatusBadRequest)
		return
	}
	if err != nil {
		http.Error(w, err.Error(), http.StatusBadGateway)
		return
	}
	w.Header().Set("Content-Type", "application/json")
	json.NewEncoder(w).Encode(pairs)
}

func handleFees(w http.ResponseWriter, r *http.Request) {
	exchange := r.URL.Query().Get("exchange")
	pair := r.URL.Query().Get("pair")
	if exchange == "" || pair == "" {
		writeError(w, http.StatusBadRequest, "exchange and pair query params required")
		return
	}

	switch exchange {
	case "luno":
		info, err := luno.GetFeeInfo(pair)
		if err != nil {
			writeError(w, http.StatusBadGateway, err.Error())
			return
		}
		writeJSON(w, http.StatusOK, info)
	case "valr":
		info, err := valr.GetFeeInfo(pair)
		if err != nil {
			writeError(w, http.StatusBadGateway, err.Error())
			return
		}
		writeJSON(w, http.StatusOK, info)
	case "bybit":
		info, err := bybit.GetFeeInfo(pair)
		if err != nil {
			writeError(w, http.StatusBadGateway, err.Error())
			return
		}
		writeJSON(w, http.StatusOK, info)
	case "binance":
		info, err := binance.GetFeeInfo(pair)
		if err != nil {
			writeError(w, http.StatusBadGateway, err.Error())
			return
		}
		writeJSON(w, http.StatusOK, info)
	default:
		writeError(w, http.StatusBadRequest, "unknown exchange: "+exchange)
	}
}

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
	case "bybit":
		ob, err := bybit.GetOrderBook(pair)
		if err != nil {
			writeError(w, http.StatusBadGateway, err.Error())
			return
		}
		writeJSON(w, http.StatusOK, ob)
	case "binance":
		ob, err := binance.GetOrderBook(pair)
		if err != nil {
			writeError(w, http.StatusBadGateway, err.Error())
			return
		}
		writeJSON(w, http.StatusOK, ob)
	default:
		writeError(w, http.StatusBadRequest, "unknown exchange: "+exchange)
	}
}

// handleZARPrices returns VALR last-traded prices for all *ZAR pairs as a
// symbol→price map. Used by the frontend price cache for ZAR estimates.
func handleZARPrices(w http.ResponseWriter, _ *http.Request) {
	prices, err := valr.GetZARPrices()
	if err != nil {
		writeError(w, http.StatusBadGateway, err.Error())
		return
	}
	writeJSON(w, http.StatusOK, prices)
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
	case "bybit":
		t, err := bybit.GetTicker(pair)
		if err != nil {
			writeError(w, http.StatusBadGateway, err.Error())
			return
		}
		writeJSON(w, http.StatusOK, t)
	case "binance":
		t, err := binance.GetTicker(pair)
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
	case "bybit":
		trades, err := bybit.GetTrades(pair, since)
		if err != nil {
			writeError(w, http.StatusBadGateway, err.Error())
			return
		}
		writeJSON(w, http.StatusOK, trades)
	case "binance":
		trades, err := binance.GetTrades(pair, since)
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
	case "bybit":
		balances, err := bybit.GetBalances()
		if err != nil {
			writeError(w, http.StatusBadGateway, err.Error())
			return
		}
		writeJSON(w, http.StatusOK, balances)
	case "binance":
		balances, err := binance.GetBalances()
		if err != nil {
			writeError(w, http.StatusBadGateway, err.Error())
			return
		}
		writeJSON(w, http.StatusOK, balances)
	default:
		writeError(w, http.StatusBadRequest, "exchange query param required (luno, valr, or bybit)")
	}
}

func handleOpenOrders(w http.ResponseWriter, r *http.Request) {
	exchange := r.URL.Query().Get("exchange")
	pair := r.URL.Query().Get("pair")

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
	case "bybit":
		orders, err := bybit.GetOpenOrders(pair)
		if err != nil {
			writeError(w, http.StatusBadGateway, err.Error())
			return
		}
		writeJSON(w, http.StatusOK, orders)
	case "binance":
		orders, err := binance.GetOpenOrders(pair)
		if err != nil {
			writeError(w, http.StatusBadGateway, err.Error())
			return
		}
		writeJSON(w, http.StatusOK, orders)
	default:
		writeError(w, http.StatusBadRequest, "exchange query param required (luno, valr, or bybit)")
	}
}

func handlePostLimitOrder(w http.ResponseWriter, r *http.Request) {
	var req PostLimitOrderRequest
	if err := json.NewDecoder(r.Body).Decode(&req); err != nil {
		writeError(w, http.StatusBadRequest, "invalid JSON body: "+err.Error())
		return
	}
	if req.Exchange == "" || req.Pair == "" || req.Side == "" {
		writeError(w, http.StatusBadRequest, "exchange, pair, and side are required")
		return
	}
	if req.Price <= 0 || req.Amount <= 0 {
		writeError(w, http.StatusBadRequest, "price and amount must be > 0")
		return
	}

	switch req.Exchange {
	case "luno":
		resp, err := luno.PostLimitOrder(req.Pair, req.Side, req.Price, req.Amount, req.PostOnly)
		if err != nil {
			writeError(w, http.StatusBadGateway, err.Error())
			return
		}
		writeJSON(w, http.StatusCreated, resp)
	case "valr":
		resp, err := valr.PostLimitOrder(req.Pair, req.Side, req.Price, req.Amount, req.PostOnly)
		if err != nil {
			writeError(w, http.StatusBadGateway, err.Error())
			return
		}
		writeJSON(w, http.StatusCreated, resp)
	case "bybit":
		resp, err := bybit.PostLimitOrder(req.Pair, req.Side, req.Price, req.Amount, req.PostOnly)
		if err != nil {
			writeError(w, http.StatusBadGateway, err.Error())
			return
		}
		writeJSON(w, http.StatusCreated, resp)
	case "binance":
		resp, err := binance.PostLimitOrder(req.Pair, req.Side, req.Price, req.Amount, req.PostOnly)
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
	case "bybit":
		if pair == "" {
			writeError(w, http.StatusBadRequest, "pair query param required when cancelling a Bybit order")
			return
		}
		if err := bybit.CancelOrder(pair, id); err != nil {
			writeError(w, http.StatusBadGateway, err.Error())
			return
		}
		w.WriteHeader(http.StatusNoContent)
	case "binance":
		if pair == "" {
			writeError(w, http.StatusBadRequest, "pair query param required when cancelling a Binance order")
			return
		}
		if err := binance.CancelOrder(pair, id); err != nil {
			writeError(w, http.StatusBadGateway, err.Error())
			return
		}
		w.WriteHeader(http.StatusNoContent)
	default:
		writeError(w, http.StatusBadRequest, "exchange query param required (luno, valr, or bybit)")
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
	case "valr", "bybit", "binance":
		writeJSON(w, http.StatusOK, WithdrawalsResponse{Exchange: exchange, Withdrawals: []WithdrawalItem{}})
	default:
		writeError(w, http.StatusBadRequest, "exchange query param required (luno, valr, or bybit)")
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

// ── P2P handlers ──────────────────────────────────────────────────────────────

func handleP2PCryptos(w http.ResponseWriter, r *http.Request) {
	exchange := r.URL.Query().Get("exchange")
	switch exchange {
	case "bybit-p2p":
		writeJSON(w, http.StatusOK, P2POptionsResponse{Exchange: exchange, Options: bybitp2p.cryptos()})
	case "binance-p2p":
		writeJSON(w, http.StatusOK, P2POptionsResponse{Exchange: exchange, Options: binancep2p.cryptos()})
	case "localcoinswap-p2p":
		opts, err := lcsp2p.cryptos()
		if err != nil {
			writeError(w, http.StatusBadGateway, err.Error())
			return
		}
		writeJSON(w, http.StatusOK, P2POptionsResponse{Exchange: exchange, Options: opts})
	default:
		writeError(w, http.StatusBadRequest, "unknown P2P exchange: "+exchange)
	}
}

func handleP2PFiats(w http.ResponseWriter, r *http.Request) {
	exchange := r.URL.Query().Get("exchange")
	switch exchange {
	case "bybit-p2p":
		writeJSON(w, http.StatusOK, P2POptionsResponse{Exchange: exchange, Options: bybitp2p.fiats()})
	case "binance-p2p":
		writeJSON(w, http.StatusOK, P2POptionsResponse{Exchange: exchange, Options: binancep2p.fiats()})
	case "localcoinswap-p2p":
		opts, err := lcsp2p.fiats()
		if err != nil {
			writeError(w, http.StatusBadGateway, err.Error())
			return
		}
		writeJSON(w, http.StatusOK, P2POptionsResponse{Exchange: exchange, Options: opts})
	default:
		writeError(w, http.StatusBadRequest, "unknown P2P exchange: "+exchange)
	}
}

func handleP2PPaymentMethods(w http.ResponseWriter, r *http.Request) {
	exchange := r.URL.Query().Get("exchange")
	switch exchange {
	case "bybit-p2p":
		writeJSON(w, http.StatusOK, P2PPaymentMethodsResponse{
			Exchange: exchange,
			Options:  bybitp2p.paymentMethods(),
			HasMore:  false,
		})
	case "binance-p2p":
		writeJSON(w, http.StatusOK, P2PPaymentMethodsResponse{
			Exchange: exchange,
			Options:  binancep2p.paymentMethods(),
			HasMore:  false,
		})
	case "localcoinswap-p2p":
		offset := 0
		if s := r.URL.Query().Get("offset"); s != "" {
			if n, err := strconv.Atoi(s); err == nil && n >= 0 {
				offset = n
			}
		}
		opts, hasMore, nextOffset, err := lcsp2p.paymentMethodsPage(offset)
		if err != nil {
			writeError(w, http.StatusBadGateway, err.Error())
			return
		}
		writeJSON(w, http.StatusOK, P2PPaymentMethodsResponse{
			Exchange:   exchange,
			Options:    opts,
			HasMore:    hasMore,
			NextOffset: nextOffset,
		})
	default:
		writeError(w, http.StatusBadRequest, "unknown P2P exchange: "+exchange)
	}
}

// handleLCSAds returns a paginated page of publicly visible LCS offers,
// filtered by the crypto/fiat/payment-method/trade-type dropdowns.
func handleLCSAds(w http.ResponseWriter, r *http.Request) {
	q := r.URL.Query()
	offset := 0
	if s := q.Get("offset"); s != "" {
		if n, err := strconv.Atoi(s); err == nil && n >= 0 {
			offset = n
		}
	}
	ads, hasMore, nextOffset, err := lcsp2p.searchAds(
		q.Get("crypto"),
		q.Get("fiat"),
		q.Get("payment"),
		q.Get("tradeType"),
		offset,
	)
	if err != nil {
		writeError(w, http.StatusBadGateway, err.Error())
		return
	}
	writeJSON(w, http.StatusOK, LCSAdsResponse{
		Total:      len(ads),
		Ads:        ads,
		HasMore:    hasMore,
		NextOffset: nextOffset,
	})
}

// handleLCSMyAds returns the authenticated user's own ads — not filtered by dropdowns.
func handleLCSMyAds(w http.ResponseWriter, r *http.Request) {
	offset := 0
	if s := r.URL.Query().Get("offset"); s != "" {
		if n, err := strconv.Atoi(s); err == nil && n >= 0 {
			offset = n
		}
	}
	ads, hasMore, nextOffset, err := lcsp2p.myAds(offset)
	if err != nil {
		writeError(w, http.StatusBadGateway, err.Error())
		return
	}
	writeJSON(w, http.StatusOK, LCSAdsResponse{
		Total:      len(ads),
		Ads:        ads,
		HasMore:    hasMore,
		NextOffset: nextOffset,
	})
}

// handleLCSTrades returns the authenticated user's trade history, filtered by dropdowns.
func handleLCSTrades(w http.ResponseWriter, r *http.Request) {
	q := r.URL.Query()
	offset := 0
	if s := q.Get("offset"); s != "" {
		if n, err := strconv.Atoi(s); err == nil && n >= 0 {
			offset = n
		}
	}
	trades, hasMore, nextOffset, err := lcsp2p.myTrades(
		q.Get("crypto"),
		q.Get("fiat"),
		q.Get("payment"),
		q.Get("tradeType"),
		offset,
	)
	if err != nil {
		writeError(w, http.StatusBadGateway, err.Error())
		return
	}
	writeJSON(w, http.StatusOK, LCSTradesResponse{
		Total:      len(trades),
		Trades:     trades,
		HasMore:    hasMore,
		NextOffset: nextOffset,
	})
}

// handleBybitAds returns a paginated page of publicly visible Bybit P2P offers.
func handleBybitAds(w http.ResponseWriter, r *http.Request) {
	q := r.URL.Query()
	page := 1
	if s := q.Get("offset"); s != "" {
		if n, err := strconv.Atoi(s); err == nil && n >= 1 {
			page = n
		}
	}
	result, err := bybitp2p.searchAds(
		q.Get("crypto"),
		q.Get("fiat"),
		q.Get("tradeType"),
		page,
	)
	if err != nil {
		writeError(w, http.StatusBadGateway, err.Error())
		return
	}
	writeJSON(w, http.StatusOK, result)
}

// handleBybitMyAds returns the authenticated user's own Bybit P2P ads.
func handleBybitMyAds(w http.ResponseWriter, r *http.Request) {
	page := 1
	if s := r.URL.Query().Get("offset"); s != "" {
		if n, err := strconv.Atoi(s); err == nil && n >= 1 {
			page = n
		}
	}
	result, err := bybitp2p.myAds(page)
	if err != nil {
		writeError(w, http.StatusBadGateway, err.Error())
		return
	}
	writeJSON(w, http.StatusOK, result)
}

// handleBybitTrades returns the authenticated user's Bybit P2P trade history.
func handleBybitTrades(w http.ResponseWriter, r *http.Request) {
	q := r.URL.Query()
	page := 1
	if s := q.Get("offset"); s != "" {
		if n, err := strconv.Atoi(s); err == nil && n >= 1 {
			page = n
		}
	}
	result, err := bybitp2p.myTrades(
		q.Get("crypto"),
		q.Get("fiat"),
		q.Get("tradeType"),
		page,
	)
	if err != nil {
		writeError(w, http.StatusBadGateway, err.Error())
		return
	}
	writeJSON(w, http.StatusOK, result)
}

// handleBinanceAds returns a paginated page of publicly visible Binance P2P offers.
func handleBinanceAds(w http.ResponseWriter, r *http.Request) {
	q := r.URL.Query()
	page := 1
	if s := q.Get("offset"); s != "" {
		if n, err := strconv.Atoi(s); err == nil && n >= 1 {
			page = n
		}
	}
	result, err := binancep2p.searchAds(
		q.Get("crypto"),
		q.Get("fiat"),
		q.Get("payment"),
		q.Get("tradeType"),
		page,
	)
	if err != nil {
		writeError(w, http.StatusBadGateway, err.Error())
		return
	}
	writeJSON(w, http.StatusOK, result)
}

// handleBinanceTrades returns the authenticated user's Binance P2P trade history.
func handleBinanceTrades(w http.ResponseWriter, r *http.Request) {
	q := r.URL.Query()
	page := 1
	if s := q.Get("offset"); s != "" {
		if n, err := strconv.Atoi(s); err == nil && n >= 1 {
			page = n
		}
	}
	result, err := binancep2p.myTrades(
		q.Get("crypto"),
		q.Get("fiat"),
		q.Get("tradeType"),
		page,
	)
	if err != nil {
		writeError(w, http.StatusBadGateway, err.Error())
		return
	}
	writeJSON(w, http.StatusOK, result)
}
