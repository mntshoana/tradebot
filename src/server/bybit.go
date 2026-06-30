// Bybit exchange API client.
// Auth: HMAC-SHA256 — signs (timestamp + apiKey + recvWindow + queryString/body) with the API secret.
// Docs: https://bybit-exchange.github.io/docs/v5/intro
package server

import (
	"crypto/hmac"
	"crypto/sha256"
	"encoding/hex"
	"encoding/json"
	"fmt"
	"io"
	"net/http"
	"strconv"
	"strings"
	"time"
)

const bybitBaseURL = "https://api.bybit.com"
const bybitRecvWindow = "5000"

type bybitClient struct {
	apiKey      string
	apiSecret   string
	http        *http.Client
	clockOffset time.Duration // offset to correct local clock drift vs Bybit server time
}

func newBybitClient(cfg Config) *bybitClient {
	c := &bybitClient{
		apiKey:    cfg.BybitAPIKey,
		apiSecret: cfg.BybitAPISecret,
		http:      &http.Client{Timeout: 15 * time.Second},
	}
	c.syncClock()
	return c
}

// syncClock fetches Bybit server time and stores the offset so signed requests
// use a timestamp that is in sync with the exchange, avoiding error 10002.
func (c *bybitClient) syncClock() {
	resp, err := c.http.Get(bybitBaseURL + "/v5/market/time")
	if err != nil {
		return
	}
	defer resp.Body.Close()
	var result struct {
		Result struct {
			TimeSecond string `json:"timeSecond"`
			TimeNano   string `json:"timeNano"`
		} `json:"result"`
	}
	if err := json.NewDecoder(resp.Body).Decode(&result); err != nil {
		return
	}
	serverMs, err := strconv.ParseInt(result.Result.TimeSecond, 10, 64)
	if err != nil {
		return
	}
	serverMs *= 1000
	localMs := time.Now().UnixMilli()
	c.clockOffset = time.Duration(serverMs-localMs) * time.Millisecond
}

// sign produces the HMAC-SHA256 hex signature required by Bybit V5.
// payload is the raw query string for GET requests, or the JSON body for POST requests.
func (c *bybitClient) sign(timestamp, payload string) string {
	msg := timestamp + c.apiKey + bybitRecvWindow + payload
	mac := hmac.New(sha256.New, []byte(c.apiSecret))
	mac.Write([]byte(msg))
	return hex.EncodeToString(mac.Sum(nil))
}

// doRequest executes a Bybit V5 API request.
// For GET: body is the raw query string (e.g. "symbol=BTCUSDT&category=spot"); it is
// appended to the URL as the query string and also used as the sign payload for auth.
// For POST: body is the JSON-encoded request body.
func (c *bybitClient) doRequest(method, path string, auth bool, body string) ([]byte, error) {
	var fullURL string
	var bodyReader io.Reader

	if method == "GET" {
		if body != "" {
			fullURL = bybitBaseURL + path + "?" + body
		} else {
			fullURL = bybitBaseURL + path
		}
	} else {
		fullURL = bybitBaseURL + path
		if body != "" {
			bodyReader = strings.NewReader(body)
		}
	}

	req, err := http.NewRequest(method, fullURL, bodyReader)
	if err != nil {
		return nil, err
	}
	if method == "POST" && body != "" {
		req.Header.Set("Content-Type", "application/json")
	}
	if auth {
		ts := strconv.FormatInt(time.Now().Add(c.clockOffset).UnixMilli(), 10)
		sig := c.sign(ts, body)
		req.Header.Set("X-BAPI-API-KEY", c.apiKey)
		req.Header.Set("X-BAPI-TIMESTAMP", ts)
		req.Header.Set("X-BAPI-SIGN", sig)
		req.Header.Set("X-BAPI-RECV-WINDOW", bybitRecvWindow)
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
		return nil, fmt.Errorf("bybit API error %d: %s", resp.StatusCode, string(data))
	}

	var envelope struct {
		RetCode int    `json:"retCode"`
		RetMsg  string `json:"retMsg"`
	}
	if err := json.Unmarshal(data, &envelope); err == nil && envelope.RetCode != 0 {
		return nil, fmt.Errorf("bybit error %d: %s", envelope.RetCode, envelope.RetMsg)
	}
	return data, nil
}

// GetOrderBook returns the top-of-book for a symbol.
func (c *bybitClient) GetOrderBook(pair string) (*OrderBookResponse, error) {
	query := "category=spot&symbol=" + pair + "&limit=25"
	data, err := c.doRequest("GET", "/v5/market/orderbook", false, query)
	if err != nil {
		return nil, err
	}

	var raw struct {
		Result struct {
			S  string      `json:"s"`
			Ts int64       `json:"ts"`
			A  [][2]string `json:"a"`
			B  [][2]string `json:"b"`
		} `json:"result"`
	}
	if err := json.Unmarshal(data, &raw); err != nil {
		return nil, err
	}

	ob := &OrderBookResponse{
		Exchange:  "bybit",
		Symbol:    pair,
		Timestamp: raw.Result.Ts,
	}
	for _, a := range raw.Result.A {
		p, _ := strconv.ParseFloat(a[0], 64)
		v, _ := strconv.ParseFloat(a[1], 64)
		ob.Asks = append(ob.Asks, [2]float64{p, v})
	}
	for _, b := range raw.Result.B {
		p, _ := strconv.ParseFloat(b[0], 64)
		v, _ := strconv.ParseFloat(b[1], 64)
		ob.Bids = append(ob.Bids, [2]float64{p, v})
	}
	return ob, nil
}

// GetTicker returns the current ticker for a symbol.
func (c *bybitClient) GetTicker(pair string) (*TickerResponse, error) {
	query := "category=spot&symbol=" + pair
	data, err := c.doRequest("GET", "/v5/market/tickers", false, query)
	if err != nil {
		return nil, err
	}

	var raw struct {
		Result struct {
			List []struct {
				Symbol    string `json:"symbol"`
				Bid1Price string `json:"bid1Price"`
				Ask1Price string `json:"ask1Price"`
				LastPrice string `json:"lastPrice"`
				Volume24h string `json:"volume24h"`
			} `json:"list"`
		} `json:"result"`
		Time int64 `json:"time"`
	}
	if err := json.Unmarshal(data, &raw); err != nil {
		return nil, err
	}
	if len(raw.Result.List) == 0 {
		return nil, fmt.Errorf("bybit: no ticker data for %s", pair)
	}

	t := raw.Result.List[0]
	bid, _ := strconv.ParseFloat(t.Bid1Price, 64)
	ask, _ := strconv.ParseFloat(t.Ask1Price, 64)
	last, _ := strconv.ParseFloat(t.LastPrice, 64)
	vol, _ := strconv.ParseFloat(t.Volume24h, 64)

	return &TickerResponse{
		Exchange:   "bybit",
		Symbol:     pair,
		Timestamp:  raw.Time,
		Bid:        bid,
		Ask:        ask,
		Last:       last,
		BaseVolume: vol,
	}, nil
}

// GetTrades returns recent public trades for a symbol.
func (c *bybitClient) GetTrades(pair string, _ int64) (*TradesResponse, error) {
	query := "category=spot&symbol=" + pair + "&limit=60"
	data, err := c.doRequest("GET", "/v5/market/recent-trade", false, query)
	if err != nil {
		return nil, err
	}

	var raw struct {
		Result struct {
			List []struct {
				Price string `json:"price"`
				Size  string `json:"size"`
				Side  string `json:"side"`
				Time  string `json:"time"`
			} `json:"list"`
		} `json:"result"`
	}
	if err := json.Unmarshal(data, &raw); err != nil {
		return nil, err
	}

	resp := &TradesResponse{Exchange: "bybit", Symbol: pair}
	for _, t := range raw.Result.List {
		p, _ := strconv.ParseFloat(t.Price, 64)
		v, _ := strconv.ParseFloat(t.Size, 64)
		ts, _ := strconv.ParseInt(t.Time, 10, 64)
		side := "buy"
		if t.Side == "Sell" {
			side = "sell"
		}
		resp.Trades = append(resp.Trades, TradeItem{
			Timestamp: ts,
			Price:     p,
			Amount:    v,
			Cost:      p * v,
			Side:      side,
		})
	}
	// Bybit returns newest-first; reverse to oldest-first for consumers.
	reverseTradeItems(resp.Trades)
	return resp, nil
}

// GetBalances returns all account balances. Requires auth.
func (c *bybitClient) GetBalances() (*BalancesResponse, error) {
	query := "accountType=UNIFIED"
	data, err := c.doRequest("GET", "/v5/account/wallet-balance", true, query)
	if err != nil {
		return nil, err
	}

	var raw struct {
		Result struct {
			List []struct {
				Coin []struct {
					Coin                string `json:"coin"`
					WalletBalance       string `json:"walletBalance"`
					AvailableToWithdraw string `json:"availableToWithdraw"`
					Locked              string `json:"locked"`
				} `json:"coin"`
			} `json:"list"`
		} `json:"result"`
	}
	if err := json.Unmarshal(data, &raw); err != nil {
		return nil, err
	}

	resp := &BalancesResponse{Exchange: "bybit", Balances: []Balance{}}
	if len(raw.Result.List) > 0 {
		for _, coin := range raw.Result.List[0].Coin {
			wallet, _ := strconv.ParseFloat(coin.WalletBalance, 64)
			avail, _ := strconv.ParseFloat(coin.AvailableToWithdraw, 64)
			locked, _ := strconv.ParseFloat(coin.Locked, 64)
			resp.Balances = append(resp.Balances, Balance{
				Asset: coin.Coin,
				Free:  avail,
				Used:  locked,
				Total: wallet,
			})
		}
	}
	return resp, nil
}

// GetOpenOrders returns all open spot orders, optionally filtered by pair. Requires auth.
func (c *bybitClient) GetOpenOrders(pair string) (*OpenOrdersResponse, error) {
	query := "category=spot"
	if pair != "" {
		query += "&symbol=" + pair
	}
	data, err := c.doRequest("GET", "/v5/order/realtime", true, query)
	if err != nil {
		return nil, err
	}

	var raw struct {
		Result struct {
			List []struct {
				OrderID     string `json:"orderId"`
				Symbol      string `json:"symbol"`
				Side        string `json:"side"`
				Price       string `json:"price"`
				Qty         string `json:"qty"`
				LeavesQty   string `json:"leavesQty"`
				CumExecQty  string `json:"cumExecQty"`
				CreatedTime string `json:"createdTime"`
			} `json:"list"`
		} `json:"result"`
	}
	if err := json.Unmarshal(data, &raw); err != nil {
		return nil, err
	}

	resp := &OpenOrdersResponse{Exchange: "bybit", Orders: []OpenOrder{}}
	for _, o := range raw.Result.List {
		price, _ := strconv.ParseFloat(o.Price, 64)
		amount, _ := strconv.ParseFloat(o.Qty, 64)
		filled, _ := strconv.ParseFloat(o.CumExecQty, 64)
		remaining, _ := strconv.ParseFloat(o.LeavesQty, 64)
		ts, _ := strconv.ParseInt(o.CreatedTime, 10, 64)
		resp.Orders = append(resp.Orders, OpenOrder{
			ID:        o.OrderID,
			Symbol:    o.Symbol,
			Side:      strings.ToLower(o.Side),
			Price:     price,
			Amount:    amount,
			Filled:    filled,
			Remaining: remaining,
			CreatedAt: ts,
		})
	}
	return resp, nil
}

// PostLimitOrder places a limit order. Requires auth.
func (c *bybitClient) PostLimitOrder(pair, side string, price, volume float64, postOnly bool) (*PostOrderResponse, error) {
	bybitSide := "Buy"
	if side == "sell" {
		bybitSide = "Sell"
	}
	tif := "GTC"
	if postOnly {
		tif = "PostOnly"
	}
	payload := fmt.Sprintf(
		`{"category":"spot","symbol":"%s","side":"%s","orderType":"Limit","qty":"%.8f","price":"%.8f","timeInForce":"%s"}`,
		pair, bybitSide, volume, price, tif,
	)
	data, err := c.doRequest("POST", "/v5/order/create", true, payload)
	if err != nil {
		return nil, err
	}

	var raw struct {
		Result struct {
			OrderID string `json:"orderId"`
		} `json:"result"`
	}
	if err := json.Unmarshal(data, &raw); err != nil {
		return nil, err
	}
	return &PostOrderResponse{OrderID: raw.Result.OrderID}, nil
}

// CancelOrder cancels an open order. Both pair and orderID are required by Bybit. Requires auth.
func (c *bybitClient) CancelOrder(pair, orderID string) error {
	payload := fmt.Sprintf(`{"category":"spot","symbol":"%s","orderId":"%s"}`, pair, orderID)
	_, err := c.doRequest("POST", "/v5/order/cancel", true, payload)
	return err
}

// GetFeeInfo returns the maker and taker fee rates for a symbol. Requires auth.
// Bybit V5 returns rates as decimal strings, e.g. "0.001" (0.1%).
func (c *bybitClient) GetFeeInfo(pair string) (*FeeInfo, error) {
	query := "category=spot&symbol=" + pair
	data, err := c.doRequest("GET", "/v5/account/fee-rate", true, query)
	if err != nil {
		return nil, err
	}
	var raw struct {
		Result struct {
			List []struct {
				MakerFeeRate string `json:"makerFeeRate"`
				TakerFeeRate string `json:"takerFeeRate"`
			} `json:"list"`
		} `json:"result"`
	}
	if err := json.Unmarshal(data, &raw); err != nil {
		return nil, err
	}
	if len(raw.Result.List) == 0 {
		return nil, fmt.Errorf("bybit: no fee data for %s", pair)
	}
	maker, _ := strconv.ParseFloat(raw.Result.List[0].MakerFeeRate, 64)
	taker, _ := strconv.ParseFloat(raw.Result.List[0].TakerFeeRate, 64)
	return &FeeInfo{Exchange: "bybit", Symbol: pair, Maker: maker, Taker: taker}, nil
}

// GetPairs returns all active spot trading pairs from Bybit.
func (c *bybitClient) GetPairs() ([]PairInfo, error) {
	query := "category=spot&limit=1000"
	data, err := c.doRequest("GET", "/v5/market/instruments-info", false, query)
	if err != nil {
		return nil, err
	}

	var raw struct {
		Result struct {
			List []struct {
				Symbol        string `json:"symbol"`
				BaseCoin      string `json:"baseCoin"`
				QuoteCoin     string `json:"quoteCoin"`
				Status        string `json:"status"`
				LotSizeFilter struct {
					MinOrderQty string `json:"minOrderQty"`
				} `json:"lotSizeFilter"`
			} `json:"list"`
		} `json:"result"`
	}
	if err := json.Unmarshal(data, &raw); err != nil {
		return nil, err
	}

	var pairs []PairInfo
	for _, p := range raw.Result.List {
		if p.Status == "Trading" {
			minBase, _ := strconv.ParseFloat(p.LotSizeFilter.MinOrderQty, 64)
			pairs = append(pairs, PairInfo{
				PairId:  p.Symbol,
				Label:   p.BaseCoin + "/" + p.QuoteCoin,
				MinBase: minBase,
			})
		}
	}
	return pairs, nil
}
