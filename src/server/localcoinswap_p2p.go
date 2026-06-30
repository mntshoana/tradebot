// LocalCoinSwap P2P exchange client.
// Fetches crypto, fiat, and payment-method lists from the LCS REST API.
// Auth: Bearer token set via LCS_API_TOKEN env var.
// If no token is configured the client returns empty lists — the UI will
// show the dropdowns as disabled rather than populated with guesses.
// Docs: https://api.localcoinswap.com/api-docs/
package server

import (
	"encoding/json"
	"fmt"
	"net/http"
	"strconv"
	"strings"
	"time"
)

const lcsBaseURL = "https://api.localcoinswap.com/api/v2"

type lcsP2PClient struct {
	apiToken string
	http     *http.Client
}

func newLCSP2PClient(cfg Config) *lcsP2PClient {
	return &lcsP2PClient{
		apiToken: cfg.LCSAPIToken,
		http:     &http.Client{Timeout: 15 * time.Second},
	}
}

// get performs an authenticated GET against the LCS API.
func (c *lcsP2PClient) get(path string, out interface{}) error {
	req, err := http.NewRequest("GET", lcsBaseURL+path, nil)
	if err != nil {
		return err
	}
	req.Header.Set("Content-Type", "application/json")
	req.Header.Set("Authorization", "Token "+c.apiToken)
	resp, err := c.http.Do(req)
	if err != nil {
		return err
	}
	defer resp.Body.Close()
	if resp.StatusCode != http.StatusOK {
		return fmt.Errorf("LCS API returned HTTP %d for %s", resp.StatusCode, path)
	}
	return json.NewDecoder(resp.Body).Decode(out)
}

// cryptos returns the list of cryptocurrencies active on LocalCoinSwap.
// Returns an empty slice when no token is configured or the API call fails.
func (c *lcsP2PClient) cryptos() ([]P2POption, error) {
	if c.apiToken == "" {
		return []P2POption{}, nil
	}
	var result struct {
		Results []struct {
			Symbol        string `json:"symbol"`
			Title         string `json:"title"`
			Chain         string `json:"chain"`
			TokenStandard string `json:"token_standard"`
			Slug          string `json:"slug"`
		} `json:"results"`
	}
	if err := c.get("/currencies/active-cryptos/?limit=100", &result); err != nil {
		return []P2POption{}, nil
	}
	opts := make([]P2POption, 0, len(result.Results))
	for _, r := range result.Results {
		// Build a deterministic unique value: SYMBOL, SYMBOL-TOKENSTANDARD, or
		// SYMBOL-CHAIN. Owning the format keeps defaults reliable and React keys unique.
		value := r.Symbol
		qualifier := ""
		switch {
		case r.TokenStandard != "":
			value = r.Symbol + "-" + r.TokenStandard
			qualifier = " · " + r.TokenStandard
		case r.Chain != "":
			value = r.Symbol + "-" + r.Chain
			qualifier = " · " + r.Chain
		}
		label := r.Title + " (" + r.Symbol + ")" + qualifier
		opts = append(opts, P2POption{Symbol: value, Label: label, Slug: r.Slug})
	}
	return opts, nil
}

// fiats returns the list of fiat currencies supported on LocalCoinSwap.
// Returns an empty slice when no token is configured or the API call fails.
func (c *lcsP2PClient) fiats() ([]P2POption, error) {
	if c.apiToken == "" {
		return []P2POption{}, nil
	}
	var result struct {
		Results []struct {
			Symbol string `json:"symbol"`
			Title  string `json:"title"`
		} `json:"results"`
	}
	if err := c.get("/currencies/fiat-currencies/?limit=200", &result); err != nil {
		return []P2POption{}, nil
	}
	opts := make([]P2POption, 0, len(result.Results))
	for _, r := range result.Results {
		opts = append(opts, P2POption{
			Symbol: r.Symbol,
			Label:  r.Title + " (" + r.Symbol + ")",
		})
	}
	return opts, nil
}

// paymentMethodsPage returns one page of payment methods from LocalCoinSwap.
// offset is the 0-based item offset (use 0 for the first page).
// hasMore is true when the API response contained a non-empty "next" URL.
// Returns an empty slice + hasMore=false when no token is configured or the
// API call fails.
func (c *lcsP2PClient) paymentMethodsPage(offset int) (opts []P2POption, hasMore bool, nextOffset int, err error) {
	if c.apiToken == "" {
		return []P2POption{}, false, 0, nil
	}
	const pageSize = 50
	var result struct {
		Next    string `json:"next"` // non-empty when another page exists
		Results []struct {
			Name string `json:"name"`
			Slug string `json:"slug"`
		} `json:"results"`
	}
	path := fmt.Sprintf("/offers/payment-methods/?limit=%d&offset=%d", pageSize, offset)
	if apiErr := c.get(path, &result); apiErr != nil {
		return []P2POption{}, false, 0, nil
	}
	opts = make([]P2POption, 0, len(result.Results))
	for _, r := range result.Results {
		opts = append(opts, P2POption{Symbol: r.Slug, Label: r.Name})
	}
	hasMore = result.Next != ""
	nextOffset = offset + len(result.Results)
	return opts, hasMore, nextOffset, nil
}

// ─── Ads + Trades ────────────────────────────────────────────────────────────

// lcsRawOffer is the JSON shape returned by /offers/search/ and /offers/.
// Several numeric fields come back as strings from the LCS API (e.g. feedback_score)
// so we receive them as strings and parse them in toAd().
type lcsRawOffer struct {
	UUID      string `json:"uuid"`
	Slug      string `json:"slug"`
	IsActive  bool   `json:"is_active"`
	CreatedBy struct {
		Username        string `json:"username"`
		FeedbackScore   string `json:"feedback_score"` // returned as "5.0" etc.
		CompletedTrades int    `json:"completed_trades"`
		AvgResponseTime int    `json:"avg_response_time"`
		ActivityStatus  string `json:"activity_status"`
	} `json:"created_by"`
	TradingType struct {
		Slug string `json:"slug"`
		Name string `json:"name"`
	} `json:"trading_type"`
	CoinCurrency struct {
		Symbol string `json:"symbol"`
	} `json:"coin_currency"`
	FiatCurrency struct {
		Symbol string `json:"symbol"`
	} `json:"fiat_currency"`
	PaymentMethod struct {
		Name string `json:"name"`
	} `json:"payment_method"`
	Headline     string `json:"headline"`
	MinTradeSize string `json:"min_trade_size"`
	MaxTradeSize string `json:"max_trade_size"`
	CurrentPrice string `json:"price_formula_value"`
}

func toAd(r lcsRawOffer) LCSAd {
	feedback, _ := strconv.ParseFloat(r.CreatedBy.FeedbackScore, 64)
	return LCSAd{
		UUID: r.UUID,
		Slug: r.Slug,
		CreatedBy: LCSAdCreator{
			Username:        r.CreatedBy.Username,
			FeedbackScore:   feedback,
			CompletedTrades: r.CreatedBy.CompletedTrades,
			AvgResponseTime: r.CreatedBy.AvgResponseTime,
			ActivityStatus:  r.CreatedBy.ActivityStatus,
		},
		TradingType:   r.TradingType.Slug,
		CryptoSymbol:  r.CoinCurrency.Symbol,
		FiatSymbol:    r.FiatCurrency.Symbol,
		PaymentMethod: r.PaymentMethod.Name,
		Headline:      r.Headline,
		MinTradeSize:  r.MinTradeSize,
		MaxTradeSize:  r.MaxTradeSize,
		CurrentPrice:  r.CurrentPrice,
		IsActive:      r.IsActive,
	}
}

// lcsRawMyOffer is the JSON shape returned by /offers/ (the authenticated
// user's own offers). It differs from /offers/search/: foreign keys come
// back as numeric IDs rather than nested objects.
type lcsRawMyOffer struct {
	UUID         string `json:"uuid"`
	Slug         string `json:"slug"`
	IsActive     bool   `json:"is_active"`
	TradingType  int    `json:"trading_type"`
	Headline     string `json:"headline"`
	MinTradeSize string `json:"min_trade_size"`
	MaxTradeSize string `json:"max_trade_size"`
	CurrentPrice string `json:"price_formula_value"`
}

// parseSlug extracts trading-type, crypto, and fiat from an LCS offer slug.
// Examples:
//
//	"sell-usdt-trc20-for-zar-in-south-africa" → ("sell", "USDT-TRC20", "ZAR")
//	"buy-usdt-trc20-for-zar-in-south-africa"  → ("buy",  "USDT-TRC20", "ZAR")
//	"usdt-trc20-for-zar-in-south-africa"      → ("",     "USDT-TRC20", "ZAR")
func parseSlug(slug string) (tradingType, crypto, fiat string) {
	rest := slug
	if strings.HasPrefix(rest, "sell-") {
		tradingType = "sell"
		rest = rest[len("sell-"):]
	} else if strings.HasPrefix(rest, "buy-") {
		tradingType = "buy"
		rest = rest[len("buy-"):]
	}
	parts := strings.SplitN(rest, "-for-", 2)
	if len(parts) >= 1 {
		crypto = strings.ToUpper(parts[0])
	}
	if len(parts) == 2 {
		fiat = strings.ToUpper(strings.SplitN(parts[1], "-", 2)[0])
	}
	return
}

// LCS trading_type integer IDs (observed from /offers/ payload). The slug
// also encodes the side, so we fall back to the slug when the integer
// mapping doesn't match what we expect.
const (
	lcsTradingTypeBuy  = 1
	lcsTradingTypeSell = 2
)

func toMyAd(r lcsRawMyOffer) LCSAd {
	slugType, crypto, fiat := parseSlug(r.Slug)
	tradingType := slugType
	if tradingType == "" {
		switch r.TradingType {
		case lcsTradingTypeBuy:
			tradingType = "buy"
		case lcsTradingTypeSell:
			tradingType = "sell"
		}
	}
	return LCSAd{
		UUID:          r.UUID,
		Slug:          r.Slug,
		TradingType:   tradingType,
		CryptoSymbol:  crypto,
		FiatSymbol:    fiat,
		PaymentMethod: "",
		Headline:      r.Headline,
		MinTradeSize:  r.MinTradeSize,
		MaxTradeSize:  r.MaxTradeSize,
		CurrentPrice:  r.CurrentPrice,
		IsActive:      r.IsActive,
	}
}

const lcsPageSize = 20

// searchAds returns a paginated page of public offers matching the filters.
// cryptoSlug is the LCS currency slug; pass "" to skip.
// fiat is the fiat symbol (e.g. "ZAR"); pass "" to skip.
// paymentSlug is the payment-method slug; pass "" for all.
// tradingType is from the *user's* perspective: "buy" means the user wants to
// buy crypto, so we ask LCS for offers from makers selling crypto (and vice
// versa). LCS's trading_type field is from the maker's perspective.
func (c *lcsP2PClient) searchAds(cryptoSlug, fiat, paymentSlug, tradingType string, offset int) (ads []LCSAd, hasMore bool, nextOffset int, err error) {
	if c.apiToken == "" {
		return []LCSAd{}, false, 0, nil
	}
	makerType := "sell"
	if tradingType == "sell" {
		makerType = "buy"
	}
	path := fmt.Sprintf("/offers/search/?trading_type=%s&limit=%d&offset=%d", makerType, lcsPageSize, offset)
	if cryptoSlug != "" {
		path += "&coin_currency=" + cryptoSlug
	}
	if fiat != "" {
		path += "&fiat_currency=" + fiat
	}
	if paymentSlug != "" {
		path += "&payment_method=" + paymentSlug
	}
	var result struct {
		Next    string        `json:"next"`
		Count   int           `json:"count"`
		Results []lcsRawOffer `json:"results"`
	}
	if apiErr := c.get(path, &result); apiErr != nil {
		return []LCSAd{}, false, 0, apiErr
	}
	ads = make([]LCSAd, 0, len(result.Results))
	for _, r := range result.Results {
		ad := toAd(r)
		// Override with user's perspective so the UI knows whether this row
		// represents a buy or sell opportunity for the viewer.
		ad.TradingType = tradingType
		ads = append(ads, ad)
	}
	return ads, result.Next != "", offset + len(result.Results), nil
}

// myAds returns a paginated page of the authenticated user's own offers.
// The /offers/ endpoint returns a different shape than /offers/search/:
// created_by, trading_type, coin_currency, fiat_currency, and payment_method
// are numeric IDs, not nested objects. We parse what we can from the slug.
func (c *lcsP2PClient) myAds(offset int) (ads []LCSAd, hasMore bool, nextOffset int, err error) {
	if c.apiToken == "" {
		return []LCSAd{}, false, 0, nil
	}
	path := fmt.Sprintf("/offers/?limit=%d&offset=%d", lcsPageSize, offset)
	var result struct {
		Next    string          `json:"next"`
		Count   int             `json:"count"`
		Results []lcsRawMyOffer `json:"results"`
	}
	if apiErr := c.get(path, &result); apiErr != nil {
		return []LCSAd{}, false, 0, apiErr
	}
	ads = make([]LCSAd, 0, len(result.Results))
	for _, r := range result.Results {
		ads = append(ads, toMyAd(r))
	}
	return ads, result.Next != "", offset + len(result.Results), nil
}

// myTrades returns a paginated page of the authenticated user's trade history.
func (c *lcsP2PClient) myTrades(cryptoSlug, fiat, paymentSlug, tradingType string, offset int) (trades []LCSTrade, hasMore bool, nextOffset int, err error) {
	if c.apiToken == "" {
		return []LCSTrade{}, false, 0, nil
	}
	path := fmt.Sprintf("/trades/list/?ordering=-time_created&limit=%d&offset=%d", lcsPageSize, offset)
	if cryptoSlug != "" {
		path += "&coin_currency=" + cryptoSlug
	}
	if fiat != "" {
		path += "&fiat_currency=" + fiat
	}
	if paymentSlug != "" {
		path += "&payment_method=" + paymentSlug
	}
	if tradingType != "" {
		path += "&trade_type=" + tradingType
	}
	var result struct {
		Next    string `json:"next"`
		Count   int    `json:"count"`
		Results []struct {
			UUID   string `json:"uuid"`
			Status string `json:"status"`
			Ad     struct {
				TradingType struct {
					Slug string `json:"slug"`
				} `json:"trading_type"`
				CoinCurrency struct {
					Symbol string `json:"symbol"`
				} `json:"coin_currency"`
				FiatCurrency struct {
					Symbol string `json:"symbol"`
				} `json:"fiat_currency"`
				PaymentMethod struct {
					Name string `json:"name"`
				} `json:"payment_method"`
			} `json:"ad"`
			Buyer struct {
				Username string `json:"username"`
			} `json:"buyer"`
			Seller struct {
				Username string `json:"username"`
			} `json:"seller"`
			FiatAmount  string `json:"fiat_amount"`
			CoinAmount  string `json:"coin_amount"`
			TimeCreated int64  `json:"time_created"`
		} `json:"results"`
	}
	if apiErr := c.get(path, &result); apiErr != nil {
		return []LCSTrade{}, false, 0, apiErr
	}
	trades = make([]LCSTrade, 0, len(result.Results))
	for _, r := range result.Results {
		partner := r.Buyer.Username
		if r.Buyer.Username != "" && r.Seller.Username != "" {
			partner = r.Buyer.Username + " / " + r.Seller.Username
		}
		trades = append(trades, LCSTrade{
			UUID:          r.UUID,
			Status:        r.Status,
			TradingType:   r.Ad.TradingType.Slug,
			CryptoSymbol:  r.Ad.CoinCurrency.Symbol,
			FiatSymbol:    r.Ad.FiatCurrency.Symbol,
			PaymentMethod: r.Ad.PaymentMethod.Name,
			Partner:       partner,
			FiatAmount:    r.FiatAmount,
			CoinAmount:    r.CoinAmount,
			TimeCreated:   r.TimeCreated,
		})
	}
	return trades, result.Next != "", offset + len(result.Results), nil
}
