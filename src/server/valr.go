// VALR exchange API client.
// Auth: HMAC-SHA512 — signs (timestamp + METHOD + path + body) with the API secret.
// Docs: https://docs.valr.com
package server

import (
	"crypto/hmac"
	"crypto/sha512"
	"encoding/hex"
	"encoding/json"
	"fmt"
	"io"
	"net/http"
	"strconv"
	"strings"
	"time"
)

const valrBaseURL = "https://api.valr.com"

type valrClient struct {
	apiKey    string
	apiSecret string
	http      *http.Client
}

func newVALRClient(cfg Config) *valrClient {
	return &valrClient{
		apiKey:    cfg.VALRAPIKey,
		apiSecret: cfg.VALRAPISecret,
		http:      &http.Client{Timeout: 15 * time.Second},
	}
}

// sign produces the HMAC-SHA512 hex signature required by VALR.
func (c *valrClient) sign(timestamp, method, path, body string) string {
	msg := timestamp + strings.ToUpper(method) + path + body
	mac := hmac.New(sha512.New, []byte(c.apiSecret))
	mac.Write([]byte(msg))
	return hex.EncodeToString(mac.Sum(nil))
}

func (c *valrClient) doRequest(method, path string, auth bool, body string) ([]byte, error) {
	var bodyReader io.Reader
	if body != "" {
		bodyReader = strings.NewReader(body)
	}

	req, err := http.NewRequest(method, valrBaseURL+path, bodyReader)
	if err != nil {
		return nil, err
	}
	if body != "" {
		req.Header.Set("Content-Type", "application/json")
	}
	if auth {
		ts := strconv.FormatInt(time.Now().UnixMilli(), 10)
		sig := c.sign(ts, method, path, body)
		req.Header.Set("X-VALR-API-KEY", c.apiKey)
		req.Header.Set("X-VALR-SIGNATURE", sig)
		req.Header.Set("X-VALR-TIMESTAMP", ts)
	}

	resp, err := c.http.Do(req)
	if err != nil {
		return nil, err
	}
	defer resp.Body.Close()

	data, err := io.ReadAll(resp.Body)
	if err != nil {
		return nil, err
	}
	if resp.StatusCode != http.StatusOK {
		return nil, fmt.Errorf("VALR API error %d: %s", resp.StatusCode, string(data))
	}
	return data, nil
}

// GetOrderBook returns the public order book for a currency pair.
func (c *valrClient) GetOrderBook(pair string) (*OrderBookResponse, error) {
	path := "/v1/public/" + pair + "/orderbook"
	data, err := c.doRequest("GET", path, false, "")
	if err != nil {
		return nil, err
	}

	var raw struct {
		Asks []struct {
			Quantity string `json:"quantity"`
			Price    string `json:"price"`
		} `json:"Asks"`
		Bids []struct {
			Quantity string `json:"quantity"`
			Price    string `json:"price"`
		} `json:"Bids"`
	}
	if err := json.Unmarshal(data, &raw); err != nil {
		return nil, err
	}

	ob := &OrderBookResponse{
		Exchange:  "valr",
		Symbol:    pair,
		Timestamp: time.Now().UnixMilli(),
	}
	for _, a := range raw.Asks {
		p, _ := strconv.ParseFloat(a.Price, 64)
		v, _ := strconv.ParseFloat(a.Quantity, 64)
		ob.Asks = append(ob.Asks, [2]float64{p, v})
	}
	for _, b := range raw.Bids {
		p, _ := strconv.ParseFloat(b.Price, 64)
		v, _ := strconv.ParseFloat(b.Quantity, 64)
		ob.Bids = append(ob.Bids, [2]float64{p, v})
	}
	return ob, nil
}

// GetTicker returns the market summary (ticker) for a currency pair.
func (c *valrClient) GetTicker(pair string) (*TickerResponse, error) {
	path := "/v1/public/" + pair + "/marketsummary"
	data, err := c.doRequest("GET", path, false, "")
	if err != nil {
		return nil, err
	}

	var raw struct {
		CurrencyPair    string `json:"currencyPair"`
		AskPrice        string `json:"askPrice"`
		BidPrice        string `json:"bidPrice"`
		LastTradedPrice string `json:"lastTradedPrice"`
		BaseVolume      string `json:"baseVolume"`
		Created         string `json:"created"`
	}
	if err := json.Unmarshal(data, &raw); err != nil {
		return nil, err
	}

	ask, _ := strconv.ParseFloat(raw.AskPrice, 64)
	bid, _ := strconv.ParseFloat(raw.BidPrice, 64)
	last, _ := strconv.ParseFloat(raw.LastTradedPrice, 64)
	vol, _ := strconv.ParseFloat(raw.BaseVolume, 64)

	var ts int64
	if t, err := time.Parse(time.RFC3339, raw.Created); err == nil {
		ts = t.UnixMilli()
	}

	return &TickerResponse{
		Exchange:   "valr",
		Symbol:     pair,
		Timestamp:  ts,
		Bid:        bid,
		Ask:        ask,
		Last:       last,
		BaseVolume: vol,
	}, nil
}

// GetZARPrices fetches all *ZAR pair last-traded prices from the public
// /v1/public/marketsummary endpoint and returns them as a symbol→price map.
// Used by the frontend price cache to estimate ZAR values of any asset.
func (c *valrClient) GetZARPrices() (map[string]float64, error) {
	data, err := c.doRequest("GET", "/v1/public/marketsummary", false, "")
	if err != nil {
		return nil, err
	}
	var raw []struct {
		CurrencyPair    string `json:"currencyPair"`
		LastTradedPrice string `json:"lastTradedPrice"`
	}
	if err := json.Unmarshal(data, &raw); err != nil {
		return nil, err
	}
	prices := make(map[string]float64)
	for _, item := range raw {
		if strings.HasSuffix(item.CurrencyPair, "ZAR") {
			if p, err := strconv.ParseFloat(item.LastTradedPrice, 64); err == nil && p > 0 {
				prices[item.CurrencyPair] = p
			}
		}
	}
	return prices, nil
}

// GetTrades returns recent public trades for a currency pair.
func (c *valrClient) GetTrades(pair string, _ int64) (*TradesResponse, error) {
	path := "/v1/public/" + pair + "/trades"
	data, err := c.doRequest("GET", path, false, "")
	if err != nil {
		return nil, err
	}

	var raw []struct {
		Price      string `json:"price"`
		Quantity   string `json:"quantity"`
		TradedAt   string `json:"tradedAt"`
		TakerSide  string `json:"takerSide"`
		SequenceID int64  `json:"sequenceId"`
	}
	if err := json.Unmarshal(data, &raw); err != nil {
		return nil, err
	}

	resp := &TradesResponse{Exchange: "valr", Symbol: pair}
	for _, t := range raw {
		p, _ := strconv.ParseFloat(t.Price, 64)
		v, _ := strconv.ParseFloat(t.Quantity, 64)

		var ts int64
		if tradedAt, err := time.Parse(time.RFC3339Nano, t.TradedAt); err == nil {
			ts = tradedAt.UnixMilli()
		}

		side := "buy"
		if t.TakerSide != "buy" {
			side = "sell"
		}
		resp.Trades = append(resp.Trades, TradeItem{
			Sequence:  t.SequenceID,
			Timestamp: ts,
			Price:     p,
			Amount:    v,
			Cost:      p * v,
			Side:      side,
		})
	}
	return resp, nil
}

// GetBalances returns all account balances. Requires auth.
func (c *valrClient) GetBalances() (*BalancesResponse, error) {
	path := "/v1/account/balances"
	data, err := c.doRequest("GET", path, true, "")
	if err != nil {
		return nil, err
	}

	var raw []struct {
		Currency  string `json:"currency"`
		Available string `json:"available"`
		Reserved  string `json:"reserved"`
	}
	if err := json.Unmarshal(data, &raw); err != nil {
		return nil, err
	}

	resp := &BalancesResponse{Exchange: "valr"}
	for _, b := range raw {
		avail, _ := strconv.ParseFloat(b.Available, 64)
		res, _ := strconv.ParseFloat(b.Reserved, 64)
		resp.Balances = append(resp.Balances, Balance{
			Asset: b.Currency,
			Free:  avail,
			Used:  res,
			Total: avail + res,
		})
	}
	return resp, nil
}

// GetOpenOrders returns all open orders, optionally filtered by pair. Requires auth.
func (c *valrClient) GetOpenOrders(pair string) (*OpenOrdersResponse, error) {
	path := "/v1/orders/open"
	data, err := c.doRequest("GET", path, true, "")
	if err != nil {
		return nil, err
	}

	var raw []struct {
		OrderID           string `json:"orderId"`
		Side              string `json:"side"`
		RemainingQuantity string `json:"remainingQuantity"`
		OriginalQuantity  string `json:"originalQuantity"`
		Price             string `json:"price"`
		CurrencyPair      string `json:"currencyPair"`
		CreatedAt         string `json:"createdAt"`
	}
	if err := json.Unmarshal(data, &raw); err != nil {
		return nil, err
	}

	resp := &OpenOrdersResponse{Exchange: "valr", Orders: []OpenOrder{}}
	for _, o := range raw {
		if pair != "" && o.CurrencyPair != pair {
			continue
		}
		price, _ := strconv.ParseFloat(o.Price, 64)
		amount, _ := strconv.ParseFloat(o.OriginalQuantity, 64)
		remaining, _ := strconv.ParseFloat(o.RemainingQuantity, 64)

		var ts int64
		if createdAt, err := time.Parse(time.RFC3339Nano, o.CreatedAt); err == nil {
			ts = createdAt.UnixMilli()
		}

		resp.Orders = append(resp.Orders, OpenOrder{
			ID:        o.OrderID,
			Symbol:    o.CurrencyPair,
			Side:      strings.ToLower(o.Side),
			Price:     price,
			Amount:    amount,
			Filled:    amount - remaining,
			Remaining: remaining,
			CreatedAt: ts,
		})
	}
	return resp, nil
}

// PostLimitOrder places a limit order. Requires auth.
func (c *valrClient) PostLimitOrder(pair, side string, price, volume float64, postOnly bool) (*PostOrderResponse, error) {
	path := "/v1/orders/limit"
	payload := fmt.Sprintf(
		`{"pair":"%s","side":"%s","price":"%.8f","quantity":"%.8f","postOnly":%t,"reduceOnly":false}`,
		pair, strings.ToUpper(side), price, volume, postOnly,
	)

	data, err := c.doRequest("POST", path, true, payload)
	if err != nil {
		return nil, err
	}

	var raw struct {
		ID string `json:"id"`
	}
	if err := json.Unmarshal(data, &raw); err != nil {
		return nil, err
	}
	return &PostOrderResponse{OrderID: raw.ID}, nil
}

// CancelOrder cancels an open order. Both pair and orderID are required by VALR. Requires auth.
func (c *valrClient) CancelOrder(pair, orderID string) error {
	path := "/v1/orders/order"
	payload := fmt.Sprintf(`{"orderId":"%s","pair":"%s"}`, orderID, pair)
	_, err := c.doRequest("DELETE", path, true, payload)
	return err
}

// GetFeeInfo returns Tier-1 maker/taker rates for the given pair, classified
// by market type using the public pairs endpoint.
// The actual personalised rates live at /account/fees/tier-info which requires
// a Bearer JWT (web session token) — not accessible via API keys.
func (c *valrClient) GetFeeInfo(pair string) (*FeeInfo, error) {
	data, err := c.doRequest("GET", "/v1/public/pairs", false, "")
	if err == nil {
		var pairs []struct {
			Symbol        string `json:"symbol"`
			BaseCurrency  string `json:"baseCurrency"`
			QuoteCurrency string `json:"quoteCurrency"`
		}
		if json.Unmarshal(data, &pairs) == nil {
			for _, p := range pairs {
				if p.Symbol != pair {
					continue
				}
				maker, taker := valrTier1Fees(p.BaseCurrency, p.QuoteCurrency)
				return &FeeInfo{Exchange: "valr", Symbol: pair, Maker: maker, Taker: taker}, nil
			}
		}
	}
	// Pair not found — default to spot-fiat rates (most common ZAR pairs).
	return &FeeInfo{Exchange: "valr", Symbol: pair, Maker: 0.0018, Taker: 0.0035}, nil
}

// valrTier1Fees returns the Tier-1 maker/taker rates based on market type.
// Source: https://support.valr.com/hc/en-us/articles/360015777451
func valrTier1Fees(base, quote string) (maker, taker float64) {
	fiatCurrencies := map[string]bool{"ZAR": true, "USD": true, "EUR": true, "GBP": true}
	stableCoins := map[string]bool{"USDT": true, "USDC": true, "DAI": true, "BUSD": true, "TUSD": true}

	if fiatCurrencies[quote] {
		return 0.0018, 0.0035 // Spot Fiat Quote
	}
	if stableCoins[base] && stableCoins[quote] {
		return 0.0005, 0.001 // Stable-to-Stable
	}
	return 0.0008, 0.001 // Spot Crypto
}

// GetPairs returns all active ZAR trading pairs from the VALR public pairs endpoint.
func (c *valrClient) GetPairs() ([]PairInfo, error) {
	data, err := c.doRequest("GET", "/v1/public/pairs", false, "")
	if err != nil {
		return nil, err
	}
	var raw []struct {
		Symbol        string `json:"symbol"`
		BaseCurrency  string `json:"baseCurrency"`
		QuoteCurrency string `json:"quoteCurrency"`
		Active        bool   `json:"active"`
		MinBaseAmount string `json:"minBaseAmount"`
	}
	if err := json.Unmarshal(data, &raw); err != nil {
		return nil, err
	}
	var pairs []PairInfo
	for _, p := range raw {
		if p.Active {
			minBase, _ := strconv.ParseFloat(p.MinBaseAmount, 64)
			pairs = append(pairs, PairInfo{
				PairId:  p.Symbol,
				Label:   p.BaseCurrency + "/" + p.QuoteCurrency,
				MinBase: minBase,
			})
		}
	}
	return pairs, nil
}
