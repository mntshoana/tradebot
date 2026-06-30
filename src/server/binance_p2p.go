// Binance P2P (C2C) client.
// Public ad search uses the unofficial-but-documented p2p.binance.com Bapi
// endpoint (no auth). Personal trade history goes through the official Sapi
// endpoint /sapi/v1/c2c/orderMatch/listUserOrderHistory using the same HMAC
// auth as the spot client. Managing your own ads is NOT exposed via the
// Binance public API, so My Ads stays unsupported here.
package server

import (
	"bytes"
	"encoding/json"
	"fmt"
	"io"
	"net/http"
	"strconv"
	"time"
)

type binanceP2PClient struct {
	http *http.Client
}

func newBinanceP2PClient(_ Config) *binanceP2PClient {
	return &binanceP2PClient{
		http: &http.Client{Timeout: 20 * time.Second},
	}
}

// ── Dropdown data ────────────────────────────────────────────────────────────
// Binance P2P offers many tokens and fiats; we curate the commonly-traded set
// the same way we do for Bybit.

func (c *binanceP2PClient) cryptos() []P2POption {
	return []P2POption{
		{Symbol: "USDT", Label: "Tether (USDT)"},
		{Symbol: "BTC", Label: "Bitcoin (BTC)"},
		{Symbol: "ETH", Label: "Ethereum (ETH)"},
		{Symbol: "BNB", Label: "BNB (BNB)"},
		{Symbol: "USDC", Label: "USD Coin (USDC)"},
		{Symbol: "FDUSD", Label: "First Digital USD (FDUSD)"},
		{Symbol: "DAI", Label: "Dai (DAI)"},
		{Symbol: "SOL", Label: "Solana (SOL)"},
		{Symbol: "XRP", Label: "XRP (XRP)"},
		{Symbol: "TRX", Label: "TRON (TRX)"},
		{Symbol: "ADA", Label: "Cardano (ADA)"},
		{Symbol: "DOGE", Label: "Dogecoin (DOGE)"},
		{Symbol: "SHIB", Label: "Shiba Inu (SHIB)"},
	}
}

func (c *binanceP2PClient) fiats() []P2POption {
	return []P2POption{
		{Symbol: "ZAR", Label: "South African Rand (ZAR)"},
		{Symbol: "USD", Label: "US Dollar (USD)"},
		{Symbol: "EUR", Label: "Euro (EUR)"},
		{Symbol: "GBP", Label: "British Pound (GBP)"},
		{Symbol: "AED", Label: "UAE Dirham (AED)"},
		{Symbol: "ARS", Label: "Argentine Peso (ARS)"},
		{Symbol: "AUD", Label: "Australian Dollar (AUD)"},
		{Symbol: "BDT", Label: "Bangladeshi Taka (BDT)"},
		{Symbol: "BRL", Label: "Brazilian Real (BRL)"},
		{Symbol: "CAD", Label: "Canadian Dollar (CAD)"},
		{Symbol: "CLP", Label: "Chilean Peso (CLP)"},
		{Symbol: "CNY", Label: "Chinese Yuan (CNY)"},
		{Symbol: "COP", Label: "Colombian Peso (COP)"},
		{Symbol: "CZK", Label: "Czech Koruna (CZK)"},
		{Symbol: "EGP", Label: "Egyptian Pound (EGP)"},
		{Symbol: "GHS", Label: "Ghanaian Cedi (GHS)"},
		{Symbol: "HKD", Label: "Hong Kong Dollar (HKD)"},
		{Symbol: "HUF", Label: "Hungarian Forint (HUF)"},
		{Symbol: "IDR", Label: "Indonesian Rupiah (IDR)"},
		{Symbol: "ILS", Label: "Israeli New Shekel (ILS)"},
		{Symbol: "INR", Label: "Indian Rupee (INR)"},
		{Symbol: "JPY", Label: "Japanese Yen (JPY)"},
		{Symbol: "KES", Label: "Kenyan Shilling (KES)"},
		{Symbol: "KRW", Label: "South Korean Won (KRW)"},
		{Symbol: "KZT", Label: "Kazakhstani Tenge (KZT)"},
		{Symbol: "MAD", Label: "Moroccan Dirham (MAD)"},
		{Symbol: "MXN", Label: "Mexican Peso (MXN)"},
		{Symbol: "MYR", Label: "Malaysian Ringgit (MYR)"},
		{Symbol: "NGN", Label: "Nigerian Naira (NGN)"},
		{Symbol: "NOK", Label: "Norwegian Krone (NOK)"},
		{Symbol: "NZD", Label: "New Zealand Dollar (NZD)"},
		{Symbol: "PEN", Label: "Peruvian Sol (PEN)"},
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
		{Symbol: "UGX", Label: "Ugandan Shilling (UGX)"},
		{Symbol: "VND", Label: "Vietnamese Dong (VND)"},
	}
}

func (c *binanceP2PClient) paymentMethods() []P2POption {
	return []P2POption{
		{Symbol: "BANK", Label: "Bank Transfer"},
		{Symbol: "PayPal", Label: "PayPal"},
		{Symbol: "Wise", Label: "Wise (TransferWise)"},
		{Symbol: "Revolut", Label: "Revolut"},
		{Symbol: "Alipay", Label: "Alipay"},
		{Symbol: "WeChat", Label: "WeChat Pay"},
		{Symbol: "CashDeposit", Label: "Cash Deposit"},
		{Symbol: "WesternUnion", Label: "Western Union"},
		{Symbol: "MoneyGram", Label: "MoneyGram"},
		{Symbol: "IMPS", Label: "IMPS (India)"},
		{Symbol: "UPI", Label: "UPI (India)"},
		{Symbol: "MPesa", Label: "M-Pesa"},
		{Symbol: "SEPA", Label: "SEPA Transfer"},
		{Symbol: "FasterPayment", Label: "Faster Payments (UK)"},
		{Symbol: "Interac", Label: "Interac e-Transfer (CA)"},
		{Symbol: "PIX", Label: "PIX (Brazil)"},
		{Symbol: "FNB", Label: "FNB (ZA)"},
		{Symbol: "Capitec", Label: "Capitec (ZA)"},
		{Symbol: "ABSA", Label: "ABSA (ZA)"},
		{Symbol: "StandardBank", Label: "Standard Bank (ZA)"},
		{Symbol: "Nedbank", Label: "Nedbank (ZA)"},
	}
}

// ── Public ad listings ───────────────────────────────────────────────────────

// searchAds fetches public Binance P2P advertisements.
// Binance uses `tradeType` from the USER's perspective (BUY = the user wants
// to buy crypto, so advertisers are selling). page is 1-based.
func (c *binanceP2PClient) searchAds(crypto, fiat, payment, tradeType string, page int) (LCSAdsResponse, error) {
	if page < 1 {
		page = 1
	}
	const pageSize = 20

	tt := "BUY"
	if tradeType == "sell" {
		tt = "SELL"
	}
	payTypes := []string{}
	if payment != "" {
		payTypes = append(payTypes, payment)
	}
	body, _ := json.Marshal(map[string]any{
		"asset":         crypto,
		"fiat":          fiat,
		"merchantCheck": false,
		"page":          page,
		"payTypes":      payTypes,
		"publisherType": nil,
		"rows":          pageSize,
		"tradeType":     tt,
	})

	req, err := http.NewRequest(http.MethodPost,
		"https://p2p.binance.com/bapi/c2c/v2/friendly/c2c/adv/search",
		bytes.NewReader(body))
	if err != nil {
		return LCSAdsResponse{}, fmt.Errorf("binance p2p: build request: %w", err)
	}
	req.Header.Set("Content-Type", "application/json")
	req.Header.Set("Accept", "application/json")
	req.Header.Set("User-Agent", "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/133.0.0.0 Safari/537.36")

	resp, err := c.http.Do(req)
	if err != nil {
		return LCSAdsResponse{}, fmt.Errorf("binance p2p: request: %w", err)
	}
	defer resp.Body.Close()
	if resp.StatusCode != http.StatusOK {
		preview, _ := io.ReadAll(io.LimitReader(resp.Body, 512))
		return LCSAdsResponse{}, fmt.Errorf("binance p2p: HTTP %d: %s", resp.StatusCode, string(preview))
	}

	var raw struct {
		Code    string `json:"code"`
		Message string `json:"message"`
		Total   int    `json:"total"`
		Data    []struct {
			Adv struct {
				AdvNo                string `json:"advNo"`
				TradeType            string `json:"tradeType"`
				Asset                string `json:"asset"`
				FiatUnit             string `json:"fiatUnit"`
				Price                string `json:"price"`
				MinSingleTransAmount string `json:"minSingleTransAmount"`
				MaxSingleTransAmount string `json:"maxSingleTransAmount"`
				TradeMethods         []struct {
					Identifier      string `json:"identifier"`
					TradeMethodName string `json:"tradeMethodName"`
				} `json:"tradeMethods"`
			} `json:"adv"`
			Advertiser struct {
				UserNo          string  `json:"userNo"`
				NickName        string  `json:"nickName"`
				MonthOrderCount int     `json:"monthOrderCount"`
				MonthFinishRate float64 `json:"monthFinishRate"`
			} `json:"advertiser"`
		} `json:"data"`
	}
	if err := json.NewDecoder(resp.Body).Decode(&raw); err != nil {
		return LCSAdsResponse{}, fmt.Errorf("binance p2p: decode: %w", err)
	}
	if raw.Code != "000000" {
		return LCSAdsResponse{}, fmt.Errorf("binance p2p: API error %s: %s", raw.Code, raw.Message)
	}

	ads := make([]LCSAd, 0, len(raw.Data))
	for _, it := range raw.Data {
		paymentLabel := "Bank Transfer"
		if len(it.Adv.TradeMethods) == 1 {
			paymentLabel = it.Adv.TradeMethods[0].TradeMethodName
		} else if len(it.Adv.TradeMethods) > 1 {
			paymentLabel = fmt.Sprintf("%d payment methods", len(it.Adv.TradeMethods))
		}
		ads = append(ads, LCSAd{
			UUID: it.Adv.AdvNo,
			Slug: it.Adv.AdvNo,
			CreatedBy: LCSAdCreator{
				Username:        it.Advertiser.NickName,
				FeedbackScore:   it.Advertiser.MonthFinishRate * 100,
				CompletedTrades: it.Advertiser.MonthOrderCount,
				ActivityStatus:  "active",
			},
			TradingType:   tradeType,
			CryptoSymbol:  it.Adv.Asset,
			FiatSymbol:    it.Adv.FiatUnit,
			PaymentMethod: paymentLabel,
			MinTradeSize:  it.Adv.MinSingleTransAmount,
			MaxTradeSize:  it.Adv.MaxSingleTransAmount,
			CurrentPrice:  it.Adv.Price,
			IsActive:      true,
		})
	}
	hasMore := len(raw.Data) == pageSize && page*pageSize < raw.Total
	return LCSAdsResponse{
		Total:      raw.Total,
		Ads:        ads,
		HasMore:    hasMore,
		NextOffset: page + 1,
	}, nil
}

// ── Authenticated trade history ──────────────────────────────────────────────
// Goes through the standard Sapi auth via binanceClient.doRequest.

// binanceP2POrderStatusLabel maps Binance C2C order statuses to the string
// labels the frontend statusBadge() helper expects.
func binanceP2POrderStatusLabel(status string) string {
	switch status {
	case "COMPLETED":
		return "COMPLETED"
	case "CANCELLED", "CANCELLED_BY_SYSTEM":
		return "CANCELLED"
	case "IN_APPEAL":
		return "DISPUTED"
	case "BUYER_PAYED":
		return "CRYPTO_ESC"
	case "PENDING", "TRADING", "DISTRIBUTING":
		return "FUND_PAID"
	default:
		return status
	}
}

// myTrades returns the authenticated user's Binance P2P trade history.
// Binance only accepts tradeType server-side; fiat is filtered client-side.
// page is 1-based.
func (c *binanceP2PClient) myTrades(crypto, fiat, tradeType string, page int) (LCSTradesResponse, error) {
	if page < 1 {
		page = 1
	}
	const pageSize = 100

	params := "page=" + strconv.Itoa(page) + "&rows=" + strconv.Itoa(pageSize)
	if tradeType == "buy" {
		params += "&tradeType=BUY"
	} else if tradeType == "sell" {
		params += "&tradeType=SELL"
	}

	data, err := binance.doRequest("GET", "/sapi/v1/c2c/orderMatch/listUserOrderHistory", true, params)
	if err != nil {
		return LCSTradesResponse{}, fmt.Errorf("binance p2p: trades: %w", err)
	}
	var raw struct {
		Code    string `json:"code"`
		Message string `json:"message"`
		Total   int    `json:"total"`
		Data    []struct {
			OrderNumber         string `json:"orderNumber"`
			TradeType           string `json:"tradeType"`
			Asset               string `json:"asset"`
			Fiat                string `json:"fiat"`
			Amount              string `json:"amount"`
			TotalPrice          string `json:"totalPrice"`
			UnitPrice           string `json:"unitPrice"`
			OrderStatus         string `json:"orderStatus"`
			CreateTime          int64  `json:"createTime"`
			CounterPartNickName string `json:"counterPartNickName"`
			PayMethodName       string `json:"payMethodName"`
		} `json:"data"`
	}
	if err := json.Unmarshal(data, &raw); err != nil {
		return LCSTradesResponse{}, fmt.Errorf("binance p2p: trades decode: %w", err)
	}
	if raw.Code != "000000" && raw.Code != "" {
		return LCSTradesResponse{}, fmt.Errorf("binance p2p: trades API error %s: %s", raw.Code, raw.Message)
	}

	trades := make([]LCSTrade, 0, len(raw.Data))
	for _, it := range raw.Data {
		if fiat != "" && it.Fiat != fiat {
			continue
		}
		if crypto != "" && it.Asset != crypto {
			continue
		}
		trading := "buy"
		if it.TradeType == "SELL" {
			trading = "sell"
		}
		trades = append(trades, LCSTrade{
			UUID:          it.OrderNumber,
			Status:        binanceP2POrderStatusLabel(it.OrderStatus),
			TradingType:   trading,
			CryptoSymbol:  it.Asset,
			FiatSymbol:    it.Fiat,
			PaymentMethod: it.PayMethodName,
			Partner:       it.CounterPartNickName,
			FiatAmount:    it.TotalPrice,
			CoinAmount:    it.Amount,
			TimeCreated:   it.CreateTime / 1000,
		})
	}
	hasMore := len(raw.Data) == pageSize
	return LCSTradesResponse{
		Total:      raw.Total,
		Trades:     trades,
		HasMore:    hasMore,
		NextOffset: page + 1,
	}, nil
}
