// Normalised, exchange-agnostic types returned by all API handlers.
// These are the shapes the future web UI (and CCXT backend) will consume.
package main

// OrderLevel is a single price level in an order book.
type OrderLevel struct {
	Price  float64 `json:"price"`
	Volume float64 `json:"volume"`
}

// OrderBookResponse is the normalised order book for any exchange/pair.
type OrderBookResponse struct {
	Exchange  string       `json:"exchange"`
	Pair      string       `json:"pair"`
	Timestamp int64        `json:"timestamp"`
	Asks      []OrderLevel `json:"asks"`
	Bids      []OrderLevel `json:"bids"`
}

// TickerResponse is the normalised ticker for any exchange/pair.
type TickerResponse struct {
	Exchange  string  `json:"exchange"`
	Pair      string  `json:"pair"`
	Timestamp int64   `json:"timestamp"`
	Bid       float64 `json:"bid"`
	Ask       float64 `json:"ask"`
	LastTrade float64 `json:"lastTrade"`
	Volume    float64 `json:"volume"`
}

// TradeItem is a single completed trade.
type TradeItem struct {
	Sequence  int64   `json:"sequence"`
	Timestamp int64   `json:"timestamp"`
	Price     float64 `json:"price"`
	Volume    float64 `json:"volume"`
	IsBuy     bool    `json:"isBuy"`
}

// TradesResponse is a list of recent trades for any exchange/pair.
type TradesResponse struct {
	Exchange string      `json:"exchange"`
	Pair     string      `json:"pair"`
	Trades   []TradeItem `json:"trades"`
}

// Balance is a single asset balance.
type Balance struct {
	Asset       string  `json:"asset"`
	AccountID   string  `json:"accountId"`
	Available   float64 `json:"available"`
	Reserved    float64 `json:"reserved"`
	Unconfirmed float64 `json:"unconfirmed"`
}

// BalancesResponse is a list of account balances for any exchange.
type BalancesResponse struct {
	Exchange string    `json:"exchange"`
	Balances []Balance `json:"balances"`
}

// OpenOrder is a single open/pending order.
type OpenOrder struct {
	ID        string  `json:"id"`
	Pair      string  `json:"pair"`
	Side      string  `json:"side"`      // "buy" or "sell"
	Price     float64 `json:"price"`
	Volume    float64 `json:"volume"`
	Remaining float64 `json:"remaining"`
	Status    string  `json:"status"`
	CreatedAt int64   `json:"createdAt"` // ms since epoch
}

// OpenOrdersResponse is a list of open orders for any exchange.
type OpenOrdersResponse struct {
	Exchange string      `json:"exchange"`
	Orders   []OpenOrder `json:"orders"`
}

// PostLimitOrderRequest is the request body for POST /account/orders/limit.
type PostLimitOrderRequest struct {
	Exchange string  `json:"exchange"`
	Pair     string  `json:"pair"`
	Side     string  `json:"side"`   // "buy" or "sell"
	Price    float64 `json:"price"`
	Volume   float64 `json:"volume"`
}

// PostOrderResponse is returned after a successful order placement.
type PostOrderResponse struct {
	OrderID string `json:"orderId"`
}

// ErrorResponse wraps a plain error message as JSON.
type ErrorResponse struct {
	Error string `json:"error"`
}

// BeneficiaryItem is a single withdrawal beneficiary.
type BeneficiaryItem struct {
	ID            string `json:"id"`
	Bank          string `json:"bank"`
	Branch        string `json:"branch"`
	AccountNumber string `json:"accountNumber"`
	AccountHolder string `json:"accountHolder"`
	AccountType   string `json:"accountType"`
	Country       string `json:"country"`
	IsFast        bool   `json:"isFast"`
}

// BeneficiariesResponse lists all beneficiaries for an exchange.
type BeneficiariesResponse struct {
	Exchange      string            `json:"exchange"`
	Beneficiaries []BeneficiaryItem `json:"beneficiaries"`
}

// PostWithdrawalRequest is the body for POST /account/withdrawals.
type PostWithdrawalRequest struct {
	Exchange      string  `json:"exchange"`
	Asset         string  `json:"asset"`
	Amount        float64 `json:"amount"`
	IsFast        bool    `json:"isFast"`
	BeneficiaryID string  `json:"beneficiaryId,omitempty"`
}

// WithdrawalResponse is returned after a successful withdrawal request.
type WithdrawalResponse struct {
	ID        string `json:"id"`
	Status    string `json:"status"`
	CreatedAt int64  `json:"createdAt"`
}
// WithdrawalItem is a single entry in the withdrawal history list.
type WithdrawalItem struct {
	ID        int64   `json:"id"`
	Status    string  `json:"status"`
	Type      string  `json:"type"`
	Currency  string  `json:"currency"`
	Amount    float64 `json:"amount"`
	Fee       float64 `json:"fee"`
	CreatedAt int64   `json:"createdAt"`
}

// WithdrawalsResponse wraps a list of withdrawals.
type WithdrawalsResponse struct {
	Exchange    string           `json:"exchange"`
	Withdrawals []WithdrawalItem `json:"withdrawals"`
}