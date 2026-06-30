package server

import "os"

// Config holds credentials and runtime settings loaded from environment variables.
type Config struct {
	LunoAPIKey       string
	LunoAPISecret    string
	VALRAPIKey       string
	VALRAPISecret    string
	BybitAPIKey      string
	BybitAPISecret   string
	BinanceAPIKey    string
	BinanceAPISecret string
	LCSAPIToken      string // LocalCoinSwap API token (optional; enables live P2P data)
	Port             string
}

func loadConfig() Config {
	port := os.Getenv("PORT")
	if port == "" {
		port = "8080"
	}
	return Config{
		LunoAPIKey:       os.Getenv("LUNO_API_KEY"),
		LunoAPISecret:    os.Getenv("LUNO_API_SECRET"),
		VALRAPIKey:       os.Getenv("VALR_API_KEY"),
		VALRAPISecret:    os.Getenv("VALR_API_SECRET"),
		BybitAPIKey:      os.Getenv("BYBIT_API_KEY"),
		BybitAPISecret:   os.Getenv("BYBIT_API_SECRET"),
		BinanceAPIKey:    os.Getenv("BINANCE_API_KEY"),
		BinanceAPISecret: os.Getenv("BINANCE_API_SECRET"),
		LCSAPIToken:      os.Getenv("LCS_API_TOKEN"),
		Port:             port,
	}
}
