// Luno exchange API client.
// Auth: HTTP Basic Auth — API Key ID as username, API Key Secret as password.
// Docs: https://www.luno.com/en/developers/api
package server

import (
	"encoding/json"
	"fmt"
	"io"
	"net/http"
	"net/url"
	"strconv"
	"strings"
	"time"
)

const lunoBaseURL = "https://api.luno.com/api/1"

type lunoClient struct {
	apiKey    string
	apiSecret string
	http      *http.Client
}

func newLunoClient(cfg Config) *lunoClient {
	return &lunoClient{
		apiKey:    cfg.LunoAPIKey,
		apiSecret: cfg.LunoAPISecret,
		http:      &http.Client{Timeout: 15 * time.Second},
	}
}

func (c *lunoClient) doRequest(method, path string, auth bool, body string) ([]byte, error) {
	var bodyReader io.Reader
	if body != "" {
		bodyReader = strings.NewReader(body)
	}

	req, err := http.NewRequest(method, lunoBaseURL+path, bodyReader)
	if err != nil {
		return nil, err
	}
	if body != "" {
		req.Header.Set("Content-Type", "application/x-www-form-urlencoded")
	}
	if auth {
		req.SetBasicAuth(c.apiKey, c.apiSecret)
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
		return nil, fmt.Errorf("luno API error %d: %s", resp.StatusCode, string(data))
	}
	return data, nil
}

// GetOrderBook returns the top-of-book for a currency pair.
func (c *lunoClient) GetOrderBook(pair string) (*OrderBookResponse, error) {
	data, err := c.doRequest("GET", "/orderbook_top?pair="+pair, false, "")
	if err != nil {
		return nil, err
	}

	var raw struct {
		Timestamp int64 `json:"timestamp"`
		Asks      []struct {
			Price  string `json:"price"`
			Volume string `json:"volume"`
		} `json:"asks"`
		Bids []struct {
			Price  string `json:"price"`
			Volume string `json:"volume"`
		} `json:"bids"`
	}
	if err := json.Unmarshal(data, &raw); err != nil {
		return nil, err
	}

	ob := &OrderBookResponse{
		Exchange:  "luno",
		Symbol:    pair,
		Timestamp: raw.Timestamp,
	}
	for _, a := range raw.Asks {
		p, _ := strconv.ParseFloat(a.Price, 64)
		v, _ := strconv.ParseFloat(a.Volume, 64)
		ob.Asks = append(ob.Asks, [2]float64{p, v})
	}
	for _, b := range raw.Bids {
		p, _ := strconv.ParseFloat(b.Price, 64)
		v, _ := strconv.ParseFloat(b.Volume, 64)
		ob.Bids = append(ob.Bids, [2]float64{p, v})
	}
	return ob, nil
}

// GetTicker returns the current ticker for a currency pair.
func (c *lunoClient) GetTicker(pair string) (*TickerResponse, error) {
	data, err := c.doRequest("GET", "/ticker?pair="+pair, false, "")
	if err != nil {
		return nil, err
	}

	var raw struct {
		Pair                string `json:"pair"`
		Timestamp           int64  `json:"timestamp"`
		Bid                 string `json:"bid"`
		Ask                 string `json:"ask"`
		LastTrade           string `json:"last_trade"`
		Rolling24HourVolume string `json:"rolling_24_hour_volume"`
	}
	if err := json.Unmarshal(data, &raw); err != nil {
		return nil, err
	}

	bid, _ := strconv.ParseFloat(raw.Bid, 64)
	ask, _ := strconv.ParseFloat(raw.Ask, 64)
	last, _ := strconv.ParseFloat(raw.LastTrade, 64)
	vol, _ := strconv.ParseFloat(raw.Rolling24HourVolume, 64)

	return &TickerResponse{
		Exchange:   "luno",
		Symbol:     pair,
		Timestamp:  raw.Timestamp,
		Bid:        bid,
		Ask:        ask,
		Last:       last,
		BaseVolume: vol,
	}, nil
}

// GetTrades returns recent trades for a currency pair.
// Pass since > 0 to fetch only trades after that sequence number.
func (c *lunoClient) GetTrades(pair string, since int64) (*TradesResponse, error) {
	path := "/trades?pair=" + pair
	if since > 0 {
		path += fmt.Sprintf("&since=%d", since)
	}
	data, err := c.doRequest("GET", path, false, "")
	if err != nil {
		return nil, err
	}

	var raw struct {
		Trades []struct {
			Sequence  int64  `json:"sequence"`
			Timestamp int64  `json:"timestamp"`
			Price     string `json:"price"`
			Volume    string `json:"volume"`
			IsBuy     bool   `json:"is_buy"`
		} `json:"trades"`
	}
	if err := json.Unmarshal(data, &raw); err != nil {
		return nil, err
	}

	resp := &TradesResponse{Exchange: "luno", Symbol: pair}
	for _, t := range raw.Trades {
		p, _ := strconv.ParseFloat(t.Price, 64)
		v, _ := strconv.ParseFloat(t.Volume, 64)
		side := "buy"
		if !t.IsBuy {
			side = "sell"
		}
		resp.Trades = append(resp.Trades, TradeItem{
			Sequence:  t.Sequence,
			Timestamp: t.Timestamp,
			Price:     p,
			Amount:    v,
			Cost:      p * v,
			Side:      side,
		})
	}
	return resp, nil
}

// GetBalances returns all account balances. Requires auth.
func (c *lunoClient) GetBalances() (*BalancesResponse, error) {
	data, err := c.doRequest("GET", "/balance", true, "")
	if err != nil {
		return nil, err
	}

	var raw struct {
		Balance []struct {
			AccountID   string `json:"account_id"`
			Asset       string `json:"asset"`
			Balance     string `json:"balance"`
			Reserved    string `json:"reserved"`
			Unconfirmed string `json:"unconfirmed"`
		} `json:"balance"`
	}
	if err := json.Unmarshal(data, &raw); err != nil {
		return nil, err
	}

	resp := &BalancesResponse{Exchange: "luno"}
	for _, b := range raw.Balance {
		avail, _ := strconv.ParseFloat(b.Balance, 64)
		res, _ := strconv.ParseFloat(b.Reserved, 64)
		unconf, _ := strconv.ParseFloat(b.Unconfirmed, 64)
		resp.Balances = append(resp.Balances, Balance{
			AccountID:   b.AccountID,
			Asset:       b.Asset,
			Free:        avail,
			Used:        res,
			Total:       avail + res,
			Unconfirmed: unconf,
		})
	}
	return resp, nil
}

// GetOpenOrders returns pending orders, optionally filtered by pair. Requires auth.
func (c *lunoClient) GetOpenOrders(pair string) (*OpenOrdersResponse, error) {
	path := "/listorders?state=PENDING"
	if pair != "" {
		path += "&pair=" + pair
	}
	data, err := c.doRequest("GET", path, true, "")
	if err != nil {
		return nil, err
	}

	var raw struct {
		Orders []struct {
			OrderID           string `json:"order_id"`
			Pair              string `json:"pair"`
			Type              string `json:"type"`
			LimitPrice        string `json:"limit_price"`
			LimitVolume       string `json:"limit_volume"`
			Base              string `json:"base"`
			CreationTimestamp int64  `json:"creation_timestamp"`
		} `json:"orders"`
	}
	if err := json.Unmarshal(data, &raw); err != nil {
		return nil, err
	}

	resp := &OpenOrdersResponse{Exchange: "luno", Orders: []OpenOrder{}}
	for _, o := range raw.Orders {
		price, _ := strconv.ParseFloat(o.LimitPrice, 64)
		amount, _ := strconv.ParseFloat(o.LimitVolume, 64)
		filled, _ := strconv.ParseFloat(o.Base, 64)
		side := "buy"
		if o.Type == "ASK" {
			side = "sell"
		}
		resp.Orders = append(resp.Orders, OpenOrder{
			ID:        o.OrderID,
			Symbol:    o.Pair,
			Side:      side,
			Price:     price,
			Amount:    amount,
			Filled:    filled,
			Remaining: amount - filled,
			CreatedAt: o.CreationTimestamp,
		})
	}
	return resp, nil
}

// PostLimitOrder places a limit order. Requires auth.
func (c *lunoClient) PostLimitOrder(pair, side string, price, volume float64, postOnly bool) (*PostOrderResponse, error) {
	orderType := "BID"
	if side == "sell" {
		orderType = "ASK"
	}

	form := url.Values{}
	form.Set("pair", pair)
	form.Set("type", orderType)
	form.Set("volume", strconv.FormatFloat(volume, 'f', -1, 64))
	form.Set("price", strconv.FormatFloat(price, 'f', -1, 64))
	if postOnly {
		form.Set("post_only", "true")
	}

	data, err := c.doRequest("POST", "/postorder", true, form.Encode())
	if err != nil {
		return nil, err
	}

	var raw struct {
		OrderID string `json:"order_id"`
	}
	if err := json.Unmarshal(data, &raw); err != nil {
		return nil, err
	}
	return &PostOrderResponse{OrderID: raw.OrderID}, nil
}

// CancelOrder cancels an open order by ID. Requires auth.
func (c *lunoClient) CancelOrder(orderID string) error {
	form := url.Values{}
	form.Set("order_id", orderID)
	_, err := c.doRequest("POST", "/stoporder", true, form.Encode())
	return err
}

// GetOrderDetails fetches a single order by ID. Requires auth.
func (c *lunoClient) GetOrderDetails(orderID string) (*OpenOrder, error) {
	data, err := c.doRequest("GET", "/orders/"+orderID, true, "")
	if err != nil {
		return nil, err
	}

	var raw struct {
		OrderID      string `json:"order_id"`
		Pair         string `json:"pair"`
		Type         string `json:"type"`
		State        string `json:"state"`
		LimitPrice   string `json:"limit_price"`
		LimitVolume  string `json:"limit_volume"`
		Base         string `json:"base"`
		CreationTime int64  `json:"creation_timestamp"`
	}
	if err := json.Unmarshal(data, &raw); err != nil {
		return nil, err
	}

	price, _ := strconv.ParseFloat(raw.LimitPrice, 64)
	amount, _ := strconv.ParseFloat(raw.LimitVolume, 64)
	filled, _ := strconv.ParseFloat(raw.Base, 64)
	side := "buy"
	if raw.Type == "ASK" {
		side = "sell"
	}
	return &OpenOrder{
		ID:        raw.OrderID,
		Symbol:    raw.Pair,
		Side:      side,
		Price:     price,
		Amount:    amount,
		Filled:    filled,
		Remaining: amount - filled,
		Status:    raw.State,
		CreatedAt: raw.CreationTime,
	}, nil
}

// ListBeneficiaries returns saved withdrawal beneficiaries. Requires auth.
func (c *lunoClient) ListBeneficiaries() (*BeneficiariesResponse, error) {
	data, err := c.doRequest("GET", "/beneficiaries", true, "")
	if err != nil {
		return nil, err
	}

	var raw struct {
		Beneficiaries []struct {
			ID                     string `json:"id"`
			BankCode               string `json:"bank_code"`
			BranchCode             string `json:"branch_code"`
			AccountNumber          string `json:"account_number"`
			AccountHolder          string `json:"account_holder"`
			AccountType            string `json:"account_type"`
			Country                string `json:"country"`
			SupportsFastWithdrawal bool   `json:"supports_fast_withdrawals"`
		} `json:"beneficiaries"`
	}
	if err := json.Unmarshal(data, &raw); err != nil {
		return nil, err
	}

	resp := &BeneficiariesResponse{Exchange: "luno"}
	for _, b := range raw.Beneficiaries {
		resp.Beneficiaries = append(resp.Beneficiaries, BeneficiaryItem{
			ID:            b.ID,
			Bank:          b.BankCode,
			Branch:        b.BranchCode,
			AccountNumber: b.AccountNumber,
			AccountHolder: b.AccountHolder,
			AccountType:   b.AccountType,
			Country:       b.Country,
			IsFast:        b.SupportsFastWithdrawal,
		})
	}
	return resp, nil
}

// Withdraw requests a ZAR withdrawal. Requires auth.
func (c *lunoClient) Withdraw(amount float64, isFast bool, beneficiaryID string) (*WithdrawalResponse, error) {
	path := fmt.Sprintf("/withdrawals?type=ZAR_EFT&amount=%.2f", amount)
	if isFast {
		path += "&fast=true"
	}
	if beneficiaryID != "" {
		path += "&beneficiary_id=" + beneficiaryID
	}
	data, err := c.doRequest("POST", path, true, "")
	if err != nil {
		return nil, err
	}

	var raw struct {
		ID        string `json:"id"`
		Status    string `json:"status"`
		CreatedAt int64  `json:"created_at"`
	}
	if err := json.Unmarshal(data, &raw); err != nil {
		return nil, err
	}
	return &WithdrawalResponse{
		ID:        raw.ID,
		Status:    raw.Status,
		CreatedAt: raw.CreatedAt,
	}, nil
}

// GetWithdrawals lists all withdrawal requests. Requires auth.
func (c *lunoClient) GetWithdrawals() (*WithdrawalsResponse, error) {
	data, err := c.doRequest("GET", "/withdrawals", true, "")
	if err != nil {
		return nil, err
	}

	var raw struct {
		Withdrawals []struct {
			ID        string `json:"id"`
			Status    string `json:"status"`
			Type      string `json:"type"`
			Currency  string `json:"currency"`
			Amount    string `json:"amount"`
			Fee       string `json:"fee"`
			CreatedAt int64  `json:"created_at"`
		} `json:"withdrawals"`
	}
	if err := json.Unmarshal(data, &raw); err != nil {
		return nil, err
	}

	resp := &WithdrawalsResponse{Exchange: "luno", Withdrawals: []WithdrawalItem{}}
	for _, w := range raw.Withdrawals {
		id, _ := strconv.ParseInt(w.ID, 10, 64)
		amount, _ := strconv.ParseFloat(w.Amount, 64)
		fee, _ := strconv.ParseFloat(w.Fee, 64)
		resp.Withdrawals = append(resp.Withdrawals, WithdrawalItem{
			ID:        id,
			Status:    w.Status,
			Type:      w.Type,
			Currency:  w.Currency,
			Amount:    amount,
			Fee:       fee,
			CreatedAt: w.CreatedAt,
		})
	}
	return resp, nil
}

// CancelWithdrawal cancels a pending withdrawal by ID. Requires auth.
func (c *lunoClient) CancelWithdrawal(id string) error {
	_, err := c.doRequest("DELETE", "/withdrawals/"+id, true, "")
	return err
}

// GetFeeInfo returns the maker and taker fee rates for a currency pair. Requires auth.
// Luno returns fee rates as decimal strings, e.g. "0" (0%) and "0.001" (0.1%).
func (c *lunoClient) GetFeeInfo(pair string) (*FeeInfo, error) {
	data, err := c.doRequest("GET", "/fee_info?pair="+pair, true, "")
	if err != nil {
		return nil, err
	}
	var raw struct {
		MakerFee string `json:"maker_fee"`
		TakerFee string `json:"taker_fee"`
	}
	if err := json.Unmarshal(data, &raw); err != nil {
		return nil, err
	}
	maker, _ := strconv.ParseFloat(raw.MakerFee, 64)
	taker, _ := strconv.ParseFloat(raw.TakerFee, 64)
	return &FeeInfo{Exchange: "luno", Symbol: pair, Maker: maker, Taker: taker}, nil
}

// GetPairs returns all active trading pairs with their minimum order size.
// Pair list comes from /api/1/tickers (stable); min volumes from
// /api/exchange/1/markets (different base path, public endpoint).
func (c *lunoClient) GetPairs() ([]PairInfo, error) {
	// ── step 1: active pairs from tickers ──────────────────────────────────
	data, err := c.doRequest("GET", "/tickers", false, "")
	if err != nil {
		return nil, err
	}
	var tickers struct {
		Tickers []struct {
			Pair   string `json:"pair"`
			Status string `json:"status"`
		} `json:"tickers"`
	}
	if err := json.Unmarshal(data, &tickers); err != nil {
		return nil, err
	}

	// ── step 2: min volumes from the exchange markets endpoint ─────────────
	minBase := map[string]float64{}
	resp, err := c.http.Get("https://api.luno.com/api/exchange/1/markets")
	if err == nil {
		defer resp.Body.Close()
		raw, _ := io.ReadAll(resp.Body)
		var markets struct {
			Markets []struct {
				MarketID  string `json:"market_id"`
				MinVolume string `json:"min_volume"`
			} `json:"markets"`
		}
		if json.Unmarshal(raw, &markets) == nil {
			for _, m := range markets.Markets {
				if v, err := strconv.ParseFloat(m.MinVolume, 64); err == nil {
					minBase[m.MarketID] = v
				}
			}
		}
	}

	// ── step 3: merge ──────────────────────────────────────────────────────
	var pairs []PairInfo
	for _, t := range tickers.Tickers {
		if t.Status == "ACTIVE" {
			pairs = append(pairs, PairInfo{
				PairId:  t.Pair,
				Label:   t.Pair,
				MinBase: minBase[t.Pair],
			})
		}
	}
	return pairs, nil
}
