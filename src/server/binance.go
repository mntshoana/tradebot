// Binance spot exchange API client.
// Auth: HMAC-SHA256 — signs the urlencoded query string (incl. timestamp+recvWindow)
// with the API secret, signature appended as &signature=<hex>. API key goes in
// the X-MBX-APIKEY header.
// Docs: https://binance-docs.github.io/apidocs/spot/en/
package server

import (
	"crypto/hmac"
	"crypto/sha256"
	"encoding/hex"
	"encoding/json"
	"fmt"
	"io"
	"net/http"
	"net/url"
	"strconv"
	"strings"
	"time"
)

const binanceBaseURL = "https://api.binance.com"
const binanceRecvWindow = "5000"

type binanceClient struct {
	apiKey      string
	apiSecret   string
	http        *http.Client
	clockOffset time.Duration
}

func newBinanceClient(cfg Config) *binanceClient {
	c := &binanceClient{
		apiKey:    cfg.BinanceAPIKey,
		apiSecret: cfg.BinanceAPISecret,
		http:      &http.Client{Timeout: 15 * time.Second},
	}
	c.syncClock()
	return c
}

// syncClock fetches Binance server time and stores the offset so signed
// requests use a timestamp in sync with the exchange.
func (c *binanceClient) syncClock() {
	resp, err := c.http.Get(binanceBaseURL + "/api/v3/time")
	if err != nil {
		return
	}
	defer resp.Body.Close()
	var t struct {
		ServerTime int64 `json:"serverTime"`
	}
	if err := json.NewDecoder(resp.Body).Decode(&t); err != nil {
		return
	}
	c.clockOffset = time.Duration(t.ServerTime-time.Now().UnixMilli()) * time.Millisecond
}

func (c *binanceClient) sign(query string) string {
	mac := hmac.New(sha256.New, []byte(c.apiSecret))
	mac.Write([]byte(query))
	return hex.EncodeToString(mac.Sum(nil))
}

// doRequest executes a Binance REST request.
// `params` is the query string (without leading '?'). For signed requests, the
// timestamp + recvWindow are appended and the whole string is HMAC-signed.
// On POST/DELETE the same params travel in the query string per Binance convention.
func (c *binanceClient) doRequest(method, path string, signed bool, params string) ([]byte, error) {
	if signed {
		ts := strconv.FormatInt(time.Now().Add(c.clockOffset).UnixMilli(), 10)
		if params != "" {
			params += "&"
		}
		params += "timestamp=" + ts + "&recvWindow=" + binanceRecvWindow
		params += "&signature=" + c.sign(params)
	}

	fullURL := binanceBaseURL + path
	if params != "" {
		fullURL += "?" + params
	}

	req, err := http.NewRequest(method, fullURL, nil)
	if err != nil {
		return nil, err
	}
	if signed || c.apiKey != "" {
		req.Header.Set("X-MBX-APIKEY", c.apiKey)
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
		// Binance returns {"code":-XXXX,"msg":"..."} on error.
		var e struct {
			Code int    `json:"code"`
			Msg  string `json:"msg"`
		}
		if json.Unmarshal(data, &e) == nil && e.Msg != "" {
			return nil, fmt.Errorf("binance error %d: %s", e.Code, e.Msg)
		}
		return nil, fmt.Errorf("binance HTTP %d: %s", resp.StatusCode, string(data))
	}
	return data, nil
}

// GetOrderBook returns the top-of-book for a symbol.
func (c *binanceClient) GetOrderBook(pair string) (*OrderBookResponse, error) {
	data, err := c.doRequest("GET", "/api/v3/depth", false, "symbol="+pair+"&limit=25")
	if err != nil {
		return nil, err
	}
	var raw struct {
		Bids [][2]string `json:"bids"`
		Asks [][2]string `json:"asks"`
	}
	if err := json.Unmarshal(data, &raw); err != nil {
		return nil, err
	}
	ob := &OrderBookResponse{
		Exchange:  "binance",
		Symbol:    pair,
		Timestamp: time.Now().UnixMilli(),
	}
	for _, a := range raw.Asks {
		p, _ := strconv.ParseFloat(a[0], 64)
		v, _ := strconv.ParseFloat(a[1], 64)
		ob.Asks = append(ob.Asks, [2]float64{p, v})
	}
	for _, b := range raw.Bids {
		p, _ := strconv.ParseFloat(b[0], 64)
		v, _ := strconv.ParseFloat(b[1], 64)
		ob.Bids = append(ob.Bids, [2]float64{p, v})
	}
	return ob, nil
}

// GetTicker returns the 24h ticker for a symbol.
func (c *binanceClient) GetTicker(pair string) (*TickerResponse, error) {
	data, err := c.doRequest("GET", "/api/v3/ticker/24hr", false, "symbol="+pair)
	if err != nil {
		return nil, err
	}
	var raw struct {
		Symbol    string `json:"symbol"`
		BidPrice  string `json:"bidPrice"`
		AskPrice  string `json:"askPrice"`
		LastPrice string `json:"lastPrice"`
		Volume    string `json:"volume"`
		CloseTime int64  `json:"closeTime"`
	}
	if err := json.Unmarshal(data, &raw); err != nil {
		return nil, err
	}
	bid, _ := strconv.ParseFloat(raw.BidPrice, 64)
	ask, _ := strconv.ParseFloat(raw.AskPrice, 64)
	last, _ := strconv.ParseFloat(raw.LastPrice, 64)
	vol, _ := strconv.ParseFloat(raw.Volume, 64)
	return &TickerResponse{
		Exchange:   "binance",
		Symbol:     pair,
		Timestamp:  raw.CloseTime,
		Bid:        bid,
		Ask:        ask,
		Last:       last,
		BaseVolume: vol,
	}, nil
}

// GetTrades returns recent public trades for a symbol.
func (c *binanceClient) GetTrades(pair string, _ int64) (*TradesResponse, error) {
	data, err := c.doRequest("GET", "/api/v3/trades", false, "symbol="+pair+"&limit=60")
	if err != nil {
		return nil, err
	}
	var raw []struct {
		ID           int64  `json:"id"`
		Price        string `json:"price"`
		Qty          string `json:"qty"`
		Time         int64  `json:"time"`
		IsBuyerMaker bool   `json:"isBuyerMaker"`
	}
	if err := json.Unmarshal(data, &raw); err != nil {
		return nil, err
	}
	resp := &TradesResponse{Exchange: "binance", Symbol: pair}
	for _, t := range raw {
		p, _ := strconv.ParseFloat(t.Price, 64)
		v, _ := strconv.ParseFloat(t.Qty, 64)
		// isBuyerMaker = true means the taker was the SELLER (aggressed downward).
		side := "buy"
		if t.IsBuyerMaker {
			side = "sell"
		}
		resp.Trades = append(resp.Trades, TradeItem{
			Sequence:  t.ID,
			Timestamp: t.Time,
			Price:     p,
			Amount:    v,
			Cost:      p * v,
			Side:      side,
		})
	}
	return resp, nil
}

// GetBalances returns all account balances. Requires auth.
func (c *binanceClient) GetBalances() (*BalancesResponse, error) {
	data, err := c.doRequest("GET", "/api/v3/account", true, "omitZeroBalances=true")
	if err != nil {
		return nil, err
	}
	var raw struct {
		Balances []struct {
			Asset  string `json:"asset"`
			Free   string `json:"free"`
			Locked string `json:"locked"`
		} `json:"balances"`
	}
	if err := json.Unmarshal(data, &raw); err != nil {
		return nil, err
	}
	resp := &BalancesResponse{Exchange: "binance", Balances: []Balance{}}
	for _, b := range raw.Balances {
		free, _ := strconv.ParseFloat(b.Free, 64)
		locked, _ := strconv.ParseFloat(b.Locked, 64)
		resp.Balances = append(resp.Balances, Balance{
			Asset: b.Asset,
			Free:  free,
			Used:  locked,
			Total: free + locked,
		})
	}
	return resp, nil
}

// GetOpenOrders returns all open spot orders, optionally filtered by pair. Requires auth.
func (c *binanceClient) GetOpenOrders(pair string) (*OpenOrdersResponse, error) {
	params := ""
	if pair != "" {
		params = "symbol=" + pair
	}
	data, err := c.doRequest("GET", "/api/v3/openOrders", true, params)
	if err != nil {
		return nil, err
	}
	var raw []struct {
		OrderID     int64  `json:"orderId"`
		Symbol      string `json:"symbol"`
		Side        string `json:"side"`
		Price       string `json:"price"`
		OrigQty     string `json:"origQty"`
		ExecutedQty string `json:"executedQty"`
		Status      string `json:"status"`
		Time        int64  `json:"time"`
	}
	if err := json.Unmarshal(data, &raw); err != nil {
		return nil, err
	}
	resp := &OpenOrdersResponse{Exchange: "binance", Orders: []OpenOrder{}}
	for _, o := range raw {
		price, _ := strconv.ParseFloat(o.Price, 64)
		amount, _ := strconv.ParseFloat(o.OrigQty, 64)
		filled, _ := strconv.ParseFloat(o.ExecutedQty, 64)
		resp.Orders = append(resp.Orders, OpenOrder{
			ID:        strconv.FormatInt(o.OrderID, 10),
			Symbol:    o.Symbol,
			Side:      strings.ToLower(o.Side),
			Price:     price,
			Amount:    amount,
			Filled:    filled,
			Remaining: amount - filled,
			Status:    o.Status,
			CreatedAt: o.Time,
		})
	}
	return resp, nil
}

// PostLimitOrder places a limit order. Requires auth.
func (c *binanceClient) PostLimitOrder(pair, side string, price, volume float64, postOnly bool) (*PostOrderResponse, error) {
	binanceSide := "BUY"
	if side == "sell" {
		binanceSide = "SELL"
	}
	orderType := "LIMIT"
	q := url.Values{}
	q.Set("symbol", pair)
	q.Set("side", binanceSide)
	if postOnly {
		// LIMIT_MAKER orders are post-only and have no timeInForce.
		orderType = "LIMIT_MAKER"
	} else {
		q.Set("timeInForce", "GTC")
	}
	q.Set("type", orderType)
	q.Set("quantity", strconv.FormatFloat(volume, 'f', -1, 64))
	q.Set("price", strconv.FormatFloat(price, 'f', -1, 64))

	data, err := c.doRequest("POST", "/api/v3/order", true, q.Encode())
	if err != nil {
		return nil, err
	}
	var raw struct {
		OrderID int64 `json:"orderId"`
	}
	if err := json.Unmarshal(data, &raw); err != nil {
		return nil, err
	}
	return &PostOrderResponse{OrderID: strconv.FormatInt(raw.OrderID, 10)}, nil
}

// CancelOrder cancels an open order. Both symbol and orderId are required. Requires auth.
func (c *binanceClient) CancelOrder(pair, orderID string) error {
	q := url.Values{}
	q.Set("symbol", pair)
	q.Set("orderId", orderID)
	_, err := c.doRequest("DELETE", "/api/v3/order", true, q.Encode())
	return err
}

// GetFeeInfo returns the maker and taker fee rates for a symbol. Requires auth.
func (c *binanceClient) GetFeeInfo(pair string) (*FeeInfo, error) {
	data, err := c.doRequest("GET", "/api/v3/commissionRate", true, "symbol="+pair)
	if err != nil {
		return nil, err
	}
	var raw struct {
		MakerCommission string `json:"makerCommission"`
		TakerCommission string `json:"takerCommission"`
	}
	if err := json.Unmarshal(data, &raw); err != nil {
		return nil, err
	}
	maker, _ := strconv.ParseFloat(raw.MakerCommission, 64)
	taker, _ := strconv.ParseFloat(raw.TakerCommission, 64)
	return &FeeInfo{Exchange: "binance", Symbol: pair, Maker: maker, Taker: taker}, nil
}

// GetPairs returns all active spot trading pairs from Binance.
// Uses /api/v3/exchangeInfo with permissions=SPOT.
func (c *binanceClient) GetPairs() ([]PairInfo, error) {
	data, err := c.doRequest("GET", "/api/v3/exchangeInfo", false, "permissions=SPOT")
	if err != nil {
		return nil, err
	}
	var raw struct {
		Symbols []struct {
			Symbol     string `json:"symbol"`
			Status     string `json:"status"`
			BaseAsset  string `json:"baseAsset"`
			QuoteAsset string `json:"quoteAsset"`
			Filters    []struct {
				FilterType string `json:"filterType"`
				MinQty     string `json:"minQty"`
			} `json:"filters"`
		} `json:"symbols"`
	}
	if err := json.Unmarshal(data, &raw); err != nil {
		return nil, err
	}
	var pairs []PairInfo
	for _, s := range raw.Symbols {
		if s.Status != "TRADING" {
			continue
		}
		var minBase float64
		for _, f := range s.Filters {
			if f.FilterType == "LOT_SIZE" {
				minBase, _ = strconv.ParseFloat(f.MinQty, 64)
				break
			}
		}
		pairs = append(pairs, PairInfo{
			PairId:  s.Symbol,
			Label:   s.BaseAsset + "/" + s.QuoteAsset,
			MinBase: minBase,
		})
	}
	return pairs, nil
}
