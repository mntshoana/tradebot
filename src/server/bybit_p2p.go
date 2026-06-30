// Bybit P2P exchange client.
// Ad browsing is public (no auth required). Supported tokens/fiats are curated
// from Bybit's documented P2P market. Auth credentials (reused from the spot
// client) will be needed in a future phase for managing your own ads.
package server

import (
	"encoding/json"
	"fmt"
	"io"
	"strconv"
	"strings"

	http "github.com/bogdanfinn/fhttp"
	tls_client "github.com/bogdanfinn/tls-client"
	"github.com/bogdanfinn/tls-client/profiles"
)

type bybitP2PClient struct {
	http tls_client.HttpClient
}

func newBybitP2PClient(_ Config) *bybitP2PClient {
	// api2.bybit.com is behind Akamai Bot Manager which fingerprints both the
	// TLS ClientHello (JA3/JA4) and the HTTP/2 framing layer (SETTINGS values,
	// pseudo-header order). Go's stdlib http stack is silently dropped. We use
	// bogdanfinn/tls-client with a Chrome profile, which emits browser-shaped
	// TLS + h2 fingerprints and gets through cleanly.
	opts := []tls_client.HttpClientOption{
		tls_client.WithTimeoutSeconds(20),
		tls_client.WithClientProfile(profiles.Chrome_133),
		tls_client.WithNotFollowRedirects(),
	}
	client, _ := tls_client.NewHttpClient(tls_client.NewNoopLogger(), opts...)
	return &bybitP2PClient{http: client}
}

func (c *bybitP2PClient) cryptos() []P2POption {
	return []P2POption{
		{Symbol: "USDT", Label: "Tether (USDT)"},
		{Symbol: "BTC", Label: "Bitcoin (BTC)"},
		{Symbol: "ETH", Label: "Ethereum (ETH)"},
		{Symbol: "USDC", Label: "USD Coin (USDC)"},
		{Symbol: "SOL", Label: "Solana (SOL)"},
		{Symbol: "DAI", Label: "Dai (DAI)"},
		{Symbol: "BNB", Label: "BNB (BNB)"},
		{Symbol: "XRP", Label: "XRP (XRP)"},
		{Symbol: "TRX", Label: "TRON (TRX)"},
		{Symbol: "LTC", Label: "Litecoin (LTC)"},
		{Symbol: "DOGE", Label: "Dogecoin (DOGE)"},
		{Symbol: "MATIC", Label: "Polygon (MATIC)"},
	}
}

func (c *bybitP2PClient) fiats() []P2POption {
	return []P2POption{
		{Symbol: "ZAR", Label: "South African Rand (ZAR)"},
		{Symbol: "USD", Label: "US Dollar (USD)"},
		{Symbol: "EUR", Label: "Euro (EUR)"},
		{Symbol: "GBP", Label: "British Pound (GBP)"},
		{Symbol: "AED", Label: "UAE Dirham (AED)"},
		{Symbol: "AUD", Label: "Australian Dollar (AUD)"},
		{Symbol: "BDT", Label: "Bangladeshi Taka (BDT)"},
		{Symbol: "BRL", Label: "Brazilian Real (BRL)"},
		{Symbol: "CAD", Label: "Canadian Dollar (CAD)"},
		{Symbol: "CHF", Label: "Swiss Franc (CHF)"},
		{Symbol: "CNY", Label: "Chinese Yuan (CNY)"},
		{Symbol: "CZK", Label: "Czech Koruna (CZK)"},
		{Symbol: "DKK", Label: "Danish Krone (DKK)"},
		{Symbol: "EGP", Label: "Egyptian Pound (EGP)"},
		{Symbol: "GHS", Label: "Ghanaian Cedi (GHS)"},
		{Symbol: "HKD", Label: "Hong Kong Dollar (HKD)"},
		{Symbol: "HUF", Label: "Hungarian Forint (HUF)"},
		{Symbol: "IDR", Label: "Indonesian Rupiah (IDR)"},
		{Symbol: "ILS", Label: "Israeli New Shekel (ILS)"},
		{Symbol: "INR", Label: "Indian Rupee (INR)"},
		{Symbol: "JPY", Label: "Japanese Yen (JPY)"},
		{Symbol: "KES", Label: "Kenyan Shilling (KES)"},
		{Symbol: "KWD", Label: "Kuwaiti Dinar (KWD)"},
		{Symbol: "MAD", Label: "Moroccan Dirham (MAD)"},
		{Symbol: "MXN", Label: "Mexican Peso (MXN)"},
		{Symbol: "MYR", Label: "Malaysian Ringgit (MYR)"},
		{Symbol: "NGN", Label: "Nigerian Naira (NGN)"},
		{Symbol: "NOK", Label: "Norwegian Krone (NOK)"},
		{Symbol: "NZD", Label: "New Zealand Dollar (NZD)"},
		{Symbol: "PHP", Label: "Philippine Peso (PHP)"},
		{Symbol: "PKR", Label: "Pakistani Rupee (PKR)"},
		{Symbol: "PLN", Label: "Polish Zloty (PLN)"},
		{Symbol: "QAR", Label: "Qatari Riyal (QAR)"},
		{Symbol: "RON", Label: "Romanian Leu (RON)"},
		{Symbol: "RUB", Label: "Russian Ruble (RUB)"},
		{Symbol: "SAR", Label: "Saudi Riyal (SAR)"},
		{Symbol: "SEK", Label: "Swedish Krona (SEK)"},
		{Symbol: "SGD", Label: "Singapore Dollar (SGD)"},
		{Symbol: "THB", Label: "Thai Baht (THB)"},
		{Symbol: "TRY", Label: "Turkish Lira (TRY)"},
		{Symbol: "TWD", Label: "New Taiwan Dollar (TWD)"},
		{Symbol: "UAH", Label: "Ukrainian Hryvnia (UAH)"},
		{Symbol: "VND", Label: "Vietnamese Dong (VND)"},
	}
}

func (c *bybitP2PClient) paymentMethods() []P2POption {
	return []P2POption{
		{Symbol: "bank_transfer", Label: "Bank Transfer"},
		{Symbol: "paypal", Label: "PayPal"},
		{Symbol: "wise", Label: "Wise (TransferWise)"},
		{Symbol: "revolut", Label: "Revolut"},
		{Symbol: "alipay", Label: "Alipay"},
		{Symbol: "wechat_pay", Label: "WeChat Pay"},
		{Symbol: "cash_deposit", Label: "Cash Deposit"},
		{Symbol: "western_union", Label: "Western Union"},
		{Symbol: "moneygram", Label: "MoneyGram"},
		{Symbol: "imps", Label: "IMPS (India)"},
		{Symbol: "upi", Label: "UPI (India)"},
		{Symbol: "mpesa", Label: "M-Pesa"},
		{Symbol: "sepa", Label: "SEPA Transfer"},
		{Symbol: "faster_payments", Label: "Faster Payments (UK)"},
		{Symbol: "interac", Label: "Interac e-Transfer (CA)"},
		{Symbol: "pix", Label: "PIX (Brazil)"},
		{Symbol: "fnb", Label: "FNB (ZA)"},
		{Symbol: "capitec", Label: "Capitec (ZA)"},
		{Symbol: "absa", Label: "ABSA (ZA)"},
		{Symbol: "standard_bank", Label: "Standard Bank (ZA)"},
		{Symbol: "nedbank", Label: "Nedbank (ZA)"},
	}
}

// searchAds fetches the public Bybit P2P ad listings.
// Bybit's `side` describes the AD's own direction, not the viewer's:
//
//	side=0 → buy-side ads  (merchants buying crypto  → user is SELLING)
//	side=1 → sell-side ads (merchants selling crypto → user is BUYING)
//
// page is 1-based.
func (c *bybitP2PClient) searchAds(crypto, fiat, tradeType string, page int) (LCSAdsResponse, error) {
	if page < 1 {
		page = 1
	}
	side := "1"
	if tradeType == "sell" {
		side = "0"
	}
	const pageSize = 20

	body, _ := json.Marshal(map[string]any{
		"tokenId":    crypto,
		"currencyId": fiat,
		"payment":    []string{},
		"side":       side,
		"size":       strconv.Itoa(pageSize),
		"page":       strconv.Itoa(page),
		"amount":     "",
	})

	req, err := http.NewRequest(http.MethodPost, "https://api2.bybit.com/fiat/otc/item/online", strings.NewReader(string(body)))
	if err != nil {
		return LCSAdsResponse{}, fmt.Errorf("bybit p2p: build request: %w", err)
	}
	req.Header = http.Header{
		"content-type":    {"application/json"},
		"accept":          {"application/json, text/plain, */*"},
		"accept-language": {"en-US,en;q=0.9"},
		"origin":          {"https://www.bybit.com"},
		"referer":         {"https://www.bybit.com/"},
		"user-agent":      {"Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/133.0.0.0 Safari/537.36"},
		http.HeaderOrderKey: {
			"content-type",
			"accept",
			"accept-language",
			"origin",
			"referer",
			"user-agent",
		},
	}

	resp, err := c.http.Do(req)
	if err != nil {
		return LCSAdsResponse{}, fmt.Errorf("bybit p2p: request: %w", err)
	}
	defer resp.Body.Close()

	if resp.StatusCode != http.StatusOK {
		preview, _ := io.ReadAll(io.LimitReader(resp.Body, 512))
		return LCSAdsResponse{}, fmt.Errorf("bybit p2p: HTTP %d: %s", resp.StatusCode, string(preview))
	}

	var raw struct {
		RetCode int    `json:"ret_code"`
		RetMsg  string `json:"ret_msg"`
		Result  struct {
			Items []struct {
				ID         string   `json:"id"`
				NickName   string   `json:"nickName"`
				TokenId    string   `json:"tokenId"`
				CurrencyId string   `json:"currencyId"`
				Price      string   `json:"price"`
				Quantity   string   `json:"quantity"`
				MinAmount  string   `json:"minAmount"`
				MaxAmount  string   `json:"maxAmount"`
				Payments   []string `json:"payments"`
				FinishNum  int      `json:"finishNum"`
				RecentOrds int      `json:"recentOrderNum"`
				ExecRate   int      `json:"recentExecuteRate"`
				IsOnline   bool     `json:"isOnline"`
			} `json:"items"`
			Count int `json:"count"`
		} `json:"result"`
	}

	if err := json.NewDecoder(resp.Body).Decode(&raw); err != nil {
		return LCSAdsResponse{}, fmt.Errorf("bybit p2p: decode: %w", err)
	}
	if raw.RetCode != 0 {
		return LCSAdsResponse{}, fmt.Errorf("bybit p2p: API error %d: %s", raw.RetCode, raw.RetMsg)
	}

	ads := make([]LCSAd, 0, len(raw.Result.Items))
	for _, item := range raw.Result.Items {
		payment := "Bank Transfer"
		if len(item.Payments) > 1 {
			payment = fmt.Sprintf("%d payment methods", len(item.Payments))
		}
		status := "inactive"
		if item.IsOnline {
			status = "active"
		}
		ads = append(ads, LCSAd{
			UUID: item.ID,
			Slug: item.ID,
			CreatedBy: LCSAdCreator{
				Username:        item.NickName,
				FeedbackScore:   float64(item.ExecRate),
				CompletedTrades: item.FinishNum,
				AvgResponseTime: 0,
				ActivityStatus:  status,
			},
			TradingType:   tradeType,
			CryptoSymbol:  item.TokenId,
			FiatSymbol:    item.CurrencyId,
			PaymentMethod: payment,
			Headline:      "",
			MinTradeSize:  item.MinAmount,
			MaxTradeSize:  item.MaxAmount,
			CurrentPrice:  item.Price,
			IsActive:      true,
		})
	}

	// hasMore: we filled the page AND there's more in the total count.
	hasMore := len(raw.Result.Items) == pageSize && page*pageSize < raw.Result.Count
	return LCSAdsResponse{
		Total:      raw.Result.Count,
		Ads:        ads,
		HasMore:    hasMore,
		NextOffset: page + 1,
	}, nil
}

// ── Authenticated personal endpoints ─────────────────────────────────────────
// Live on api.bybit.com (not api2) and use the standard Bybit V5 HMAC auth
// scheme, so we delegate signing + transport to the existing spot client.

// myAds returns the authenticated user's own P2P advertisements.
// page is 1-based.
func (c *bybitP2PClient) myAds(page int) (LCSAdsResponse, error) {
	if page < 1 {
		page = 1
	}
	const pageSize = 20
	body, _ := json.Marshal(map[string]any{
		"page": strconv.Itoa(page),
		"size": strconv.Itoa(pageSize),
	})
	data, err := bybit.doRequest("POST", "/v5/p2p/item/personal/list", true, string(body))
	if err != nil {
		return LCSAdsResponse{}, fmt.Errorf("bybit p2p: my ads: %w", err)
	}
	var raw struct {
		RetCode int    `json:"ret_code"`
		RetMsg  string `json:"ret_msg"`
		Result  struct {
			Count int `json:"count"`
			Items []struct {
				ID         string   `json:"id"`
				NickName   string   `json:"nickName"`
				TokenId    string   `json:"tokenId"`
				CurrencyId string   `json:"currencyId"`
				Side       int      `json:"side"`
				Price      string   `json:"price"`
				Quantity   string   `json:"quantity"`
				MinAmount  string   `json:"minAmount"`
				MaxAmount  string   `json:"maxAmount"`
				Remark     string   `json:"remark"`
				Status     int      `json:"status"`
				Payments   []string `json:"payments"`
			} `json:"items"`
		} `json:"result"`
	}
	if err := json.Unmarshal(data, &raw); err != nil {
		return LCSAdsResponse{}, fmt.Errorf("bybit p2p: my ads decode: %w", err)
	}
	if raw.RetCode != 0 {
		return LCSAdsResponse{}, fmt.Errorf("bybit p2p: my ads API error %d: %s", raw.RetCode, raw.RetMsg)
	}

	ads := make([]LCSAd, 0, len(raw.Result.Items))
	for _, item := range raw.Result.Items {
		payment := "Bank Transfer"
		if len(item.Payments) > 1 {
			payment = fmt.Sprintf("%d payment methods", len(item.Payments))
		}
		trading := "buy"
		if item.Side == 1 {
			trading = "sell"
		}
		ads = append(ads, LCSAd{
			UUID: item.ID,
			Slug: item.ID,
			CreatedBy: LCSAdCreator{
				Username:       item.NickName,
				ActivityStatus: "active",
			},
			TradingType:   trading,
			CryptoSymbol:  item.TokenId,
			FiatSymbol:    item.CurrencyId,
			PaymentMethod: payment,
			Headline:      item.Remark,
			MinTradeSize:  item.MinAmount,
			MaxTradeSize:  item.MaxAmount,
			CurrentPrice:  item.Price,
			IsActive:      item.Status == 10,
		})
	}
	hasMore := len(raw.Result.Items) == pageSize
	return LCSAdsResponse{
		Total:      raw.Result.Count,
		Ads:        ads,
		HasMore:    hasMore,
		NextOffset: page + 1,
	}, nil
}

// bybitOrderStatusLabel maps Bybit P2P numeric order statuses to the string
// labels the frontend statusBadge() helper expects.
func bybitOrderStatusLabel(status int) string {
	switch status {
	case 50:
		return "COMPLETED"
	case 40, 80:
		return "CANCELLED"
	case 70:
		return "EXPIRED"
	case 30, 100, 110:
		return "DISPUTED"
	case 20:
		return "CRYPTO_ESC"
	case 5, 10, 60, 90:
		return "FUND_PAID"
	default:
		return fmt.Sprintf("STATUS_%d", status)
	}
}

// myTrades returns the authenticated user's P2P order/trade history.
// The Bybit endpoint supports filtering by tokenId and side; fiat is filtered
// client-side because the API does not expose currencyId as a query param.
// page is 1-based.
func (c *bybitP2PClient) myTrades(crypto, fiat, tradeType string, page int) (LCSTradesResponse, error) {
	if page < 1 {
		page = 1
	}
	const pageSize = 30
	payload := map[string]any{
		"page": page,
		"size": pageSize,
	}
	if crypto != "" {
		payload["tokenId"] = crypto
	}
	if tradeType == "buy" {
		payload["side"] = 0
	} else if tradeType == "sell" {
		payload["side"] = 1
	}
	body, _ := json.Marshal(payload)
	data, err := bybit.doRequest("POST", "/v5/p2p/order/simplifyList", true, string(body))
	if err != nil {
		return LCSTradesResponse{}, fmt.Errorf("bybit p2p: trades: %w", err)
	}
	var raw struct {
		RetCode int    `json:"ret_code"`
		RetMsg  string `json:"ret_msg"`
		Result  struct {
			Count int `json:"count"`
			Items []struct {
				ID                  string `json:"id"`
				Side                int    `json:"side"`
				TokenId             string `json:"tokenId"`
				Amount              string `json:"amount"`
				CurrencyId          string `json:"currencyId"`
				Price               string `json:"price"`
				NotifyTokenQuantity string `json:"notifyTokenQuantity"`
				TargetNickName      string `json:"targetNickName"`
				Status              int    `json:"status"`
				CreateDate          string `json:"createDate"`
			} `json:"items"`
		} `json:"result"`
	}
	if err := json.Unmarshal(data, &raw); err != nil {
		return LCSTradesResponse{}, fmt.Errorf("bybit p2p: trades decode: %w", err)
	}
	if raw.RetCode != 0 {
		return LCSTradesResponse{}, fmt.Errorf("bybit p2p: trades API error %d: %s", raw.RetCode, raw.RetMsg)
	}

	trades := make([]LCSTrade, 0, len(raw.Result.Items))
	for _, item := range raw.Result.Items {
		if fiat != "" && item.CurrencyId != fiat {
			continue
		}
		trading := "buy"
		if item.Side == 1 {
			trading = "sell"
		}
		// createDate is a string of ms since epoch.
		var tsSec int64
		if ms, err := strconv.ParseInt(item.CreateDate, 10, 64); err == nil {
			tsSec = ms / 1000
		}
		trades = append(trades, LCSTrade{
			UUID:          item.ID,
			Status:        bybitOrderStatusLabel(item.Status),
			TradingType:   trading,
			CryptoSymbol:  item.TokenId,
			FiatSymbol:    item.CurrencyId,
			PaymentMethod: "",
			Partner:       item.TargetNickName,
			FiatAmount:    item.Amount,
			CoinAmount:    item.NotifyTokenQuantity,
			TimeCreated:   tsSec,
		})
	}
	hasMore := len(raw.Result.Items) == pageSize
	return LCSTradesResponse{
		Total:      raw.Result.Count,
		Trades:     trades,
		HasMore:    hasMore,
		NextOffset: page + 1,
	}, nil
}
