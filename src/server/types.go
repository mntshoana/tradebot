// Normalised, exchange-agnostic types returned by all API handlers.
// Field names match the CCXT unified schema so the React UI can stay
// exchange-agnostic.
package server

// OrderBookResponse is the normalised order book for any exchange/pair.
// Bids and Asks are [price, amount] pairs matching the CCXT unified format.
type OrderBookResponse struct {
	Exchange  string       `json:"exchange"`
	Symbol    string       `json:"symbol"`
	Timestamp int64        `json:"timestamp"`
	Asks      [][2]float64 `json:"asks"`
	Bids      [][2]float64 `json:"bids"`
}

// TickerResponse is the normalised ticker for any exchange/pair.
type TickerResponse struct {
	Exchange   string  `json:"exchange"`
	Symbol     string  `json:"symbol"`
	Timestamp  int64   `json:"timestamp"`
	Bid        float64 `json:"bid"`
	Ask        float64 `json:"ask"`
	Last       float64 `json:"last"`
	BaseVolume float64 `json:"baseVolume"`
}

// TradeItem is a single completed trade — field names match CCXT unified trade.
type TradeItem struct {
	Sequence  int64   `json:"sequence"`
	Timestamp int64   `json:"timestamp"`
	Price     float64 `json:"price"`
	Amount    float64 `json:"amount"`
	Cost      float64 `json:"cost"` // price × amount
	Side      string  `json:"side"` // "buy" or "sell"
}

// FeeInfo holds the maker and taker fee rates for a specific exchange/pair.
// Rates are stored as decimals (e.g. 0.001 = 0.1%).
type FeeInfo struct {
	Exchange string  `json:"exchange"`
	Symbol   string  `json:"symbol"`
	Maker    float64 `json:"maker"`
	Taker    float64 `json:"taker"`
}

// PairInfo is a single tradable pair returned by the /market/pairs endpoint.
type PairInfo struct {
	PairId  string  `json:"pairId"`
	Label   string  `json:"label"`
	MinBase float64 `json:"minBase"` // minimum base-asset order quantity (0 = unknown)
}

// TradesResponse is a list of recent trades for any exchange/pair.
type TradesResponse struct {
	Exchange string      `json:"exchange"`
	Symbol   string      `json:"symbol"`
	Trades   []TradeItem `json:"trades"`
}

// Balance is a single asset balance — field names match CCXT unified balance.
type Balance struct {
	Asset       string  `json:"asset"`
	AccountID   string  `json:"accountId"`
	Free        float64 `json:"free"`        // available to trade
	Used        float64 `json:"used"`        // reserved / in open orders
	Total       float64 `json:"total"`       // free + used
	Unconfirmed float64 `json:"unconfirmed"` // Luno-specific pending deposits
}

// BalancesResponse is a list of account balances for any exchange.
type BalancesResponse struct {
	Exchange string    `json:"exchange"`
	Balances []Balance `json:"balances"`
}

// OpenOrder is a single open/pending order — field names match CCXT unified order.
type OpenOrder struct {
	ID        string  `json:"id"`
	Symbol    string  `json:"symbol"`
	Side      string  `json:"side"` // "buy" or "sell"
	Price     float64 `json:"price"`
	Amount    float64 `json:"amount"`    // total order size
	Filled    float64 `json:"filled"`    // amount executed so far
	Remaining float64 `json:"remaining"` // amount - filled
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
	Side     string  `json:"side"` // "buy" or "sell"
	Price    float64 `json:"price"`
	Amount   float64 `json:"amount"`
	PostOnly bool    `json:"postOnly"`
}

// PostOrderResponse is returned after a successful order placement.
type PostOrderResponse struct {
	OrderID string `json:"orderId"`
}

// ErrorResponse wraps a plain error message as JSON.
type ErrorResponse struct {
	Error string `json:"error"`
}

// P2POption is a single selectable item for P2P crypto/fiat/payment dropdowns.
// Slug is the exchange-internal identifier used for search API calls (LCS only).
type P2POption struct {
	Symbol string `json:"symbol"`
	Label  string `json:"label"`
	Slug   string `json:"slug,omitempty"`
}

// P2POptionsResponse is returned by /p2p/cryptos, /p2p/fiats.
type P2POptionsResponse struct {
	Exchange string      `json:"exchange"`
	Options  []P2POption `json:"options"`
}

// P2PPaymentMethodsResponse is returned by /p2p/payment-methods.
// HasMore is true when further pages are available (use NextOffset to fetch them).
type P2PPaymentMethodsResponse struct {
	Exchange   string      `json:"exchange"`
	Options    []P2POption `json:"options"`
	HasMore    bool        `json:"hasMore"`
	NextOffset int         `json:"nextOffset"`
}

// LCSAdCreator is a trimmed profile attached to every ad/offer from LCS.
type LCSAdCreator struct {
	Username        string  `json:"username"`
	FeedbackScore   float64 `json:"feedbackScore"`
	CompletedTrades int     `json:"completedTrades"`
	AvgResponseTime int     `json:"avgResponseTime"`
	ActivityStatus  string  `json:"activityStatus"`
}

// LCSAd is a single public offer (ad) returned by the search or my-ads endpoints.
type LCSAd struct {
	UUID          string       `json:"uuid"`
	Slug          string       `json:"slug"`
	CreatedBy     LCSAdCreator `json:"createdBy"`
	TradingType   string       `json:"tradingType"` // "buy" or "sell"
	CryptoSymbol  string       `json:"cryptoSymbol"`
	FiatSymbol    string       `json:"fiatSymbol"`
	PaymentMethod string       `json:"paymentMethod"`
	Headline      string       `json:"headline"`
	MinTradeSize  string       `json:"minTradeSize"`
	MaxTradeSize  string       `json:"maxTradeSize"`
	CurrentPrice  string       `json:"currentPrice"`
	IsActive      bool         `json:"isActive"`
}

// LCSAdsResponse wraps a paginated list of LCS ads.
type LCSAdsResponse struct {
	Total      int     `json:"total"`
	Ads        []LCSAd `json:"ads"`
	HasMore    bool    `json:"hasMore"`
	NextOffset int     `json:"nextOffset"`
}

// LCSTrade is a single P2P trade from the authenticated user's history.
type LCSTrade struct {
	UUID          string `json:"uuid"`
	Status        string `json:"status"`
	TradingType   string `json:"tradingType"` // "buy" or "sell" (from the user's perspective)
	CryptoSymbol  string `json:"cryptoSymbol"`
	FiatSymbol    string `json:"fiatSymbol"`
	PaymentMethod string `json:"paymentMethod"`
	Partner       string `json:"partner"` // counterparty username
	FiatAmount    string `json:"fiatAmount"`
	CoinAmount    string `json:"coinAmount"`
	TimeCreated   int64  `json:"timeCreated"`
}

// LCSTradesResponse wraps a paginated list of LCS trades.
type LCSTradesResponse struct {
	Total      int        `json:"total"`
	Trades     []LCSTrade `json:"trades"`
	HasMore    bool       `json:"hasMore"`
	NextOffset int        `json:"nextOffset"`
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
