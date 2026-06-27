package server

import "os"

// Config holds credentials and runtime settings loaded from environment variables.
type Config struct {
	LunoAPIKey    string
	LunoAPISecret string
	VALRAPIKey    string
	VALRAPISecret string
	Port          string
}

func loadConfig() Config {
	port := os.Getenv("PORT")
	if port == "" {
		port = "8080"
	}
	return Config{
		LunoAPIKey:    os.Getenv("LUNO_API_KEY"),
		LunoAPISecret: os.Getenv("LUNO_API_SECRET"),
		VALRAPIKey:    os.Getenv("VALR_API_KEY"),
		VALRAPISecret: os.Getenv("VALR_API_SECRET"),
		Port:          port,
	}
}
