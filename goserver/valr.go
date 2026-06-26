// VALR exchange API client.
// Auth: HMAC-SHA512 — signs (timestamp + METHOD + path + body) with the API secret.
// Docs: https://docs.valr.com
package main

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
		http:      &http.Client{},
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

	resp := &OpenOrdersResponse{Exchange: "valr"}
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
func (c *valrClient) PostLimitOrder(pair, side string, price, volume float64) (*PostOrderResponse, error) {
	path := "/v1/orders/limit"
	payload := fmt.Sprintf(
		`{"pair":"%s","side":"%s","price":"%.8f","quantity":"%.8f","postOnly":false,"reduceOnly":false}`,
		pair, strings.ToUpper(side), price, volume,
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
