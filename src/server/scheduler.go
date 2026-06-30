package server

import (
	"encoding/json"
	"log"
	"net/http"
	"sync"
	"time"

	"github.com/gorilla/websocket"
)

// WsFrame is a typed push message streamed to all connected UI clients.
type WsFrame struct {
	Type     string      `json:"type"`
	Exchange string      `json:"exchange"`
	Symbol   string      `json:"symbol"`
	Data     interface{} `json:"data"`
}

// ── Hub ──────────────────────────────────────────────────────────────────────

type wsConn struct {
	conn *websocket.Conn
	send chan []byte
}

type hub struct {
	mu      sync.RWMutex
	clients map[*wsConn]struct{}
	cacheMu sync.RWMutex
	cache   map[string][]byte // last serialised WsFrame per "type:exchange:symbol" key
}

var wsHub = &hub{
	clients: make(map[*wsConn]struct{}),
	cache:   make(map[string][]byte),
}

func (h *hub) register(c *wsConn) {
	h.mu.Lock()
	h.clients[c] = struct{}{}
	h.mu.Unlock()

	// Immediately flush all cached frames to the new client so it has
	// current market depth without waiting for the next poll cycle.
	h.cacheMu.RLock()
	snapshot := make([][]byte, 0, len(h.cache))
	for _, b := range h.cache {
		snapshot = append(snapshot, b)
	}
	h.cacheMu.RUnlock()

	for _, b := range snapshot {
		select {
		case c.send <- b:
		default:
		}
	}
}

func (h *hub) unregister(c *wsConn) {
	h.mu.Lock()
	delete(h.clients, c)
	h.mu.Unlock()
}

func (h *hub) broadcast(b []byte) {
	h.mu.RLock()
	defer h.mu.RUnlock()
	for c := range h.clients {
		select {
		case c.send <- b:
		default: // slow client — drop frame
		}
	}
}

func broadcastFrame(frame WsFrame) {
	b, err := json.Marshal(frame)
	if err != nil {
		log.Printf("ws marshal: %v", err)
		return
	}
	// Cache the latest frame for each stream before broadcasting.
	key := frame.Type + ":" + frame.Exchange + ":" + frame.Symbol
	wsHub.cacheMu.Lock()
	wsHub.cache[key] = b
	wsHub.cacheMu.Unlock()

	wsHub.broadcast(b)
}

// ── WebSocket HTTP handler ────────────────────────────────────────────────────

var upgrader = websocket.Upgrader{
	CheckOrigin: func(r *http.Request) bool { return true }, // localhost only
}

func handleWs(w http.ResponseWriter, r *http.Request) {
	conn, err := upgrader.Upgrade(w, r, nil)
	if err != nil {
		log.Printf("ws upgrade: %v", err)
		return
	}

	c := &wsConn{conn: conn, send: make(chan []byte, 64)}
	wsHub.register(c)

	// write pump — sends queued messages to the client
	go func() {
		defer func() {
			wsHub.unregister(c)
			conn.Close()
		}()
		for msg := range c.send {
			if err := conn.WriteMessage(websocket.TextMessage, msg); err != nil {
				return
			}
		}
	}()

	// read pump — receives subscribe messages and detects client disconnect
	go func() {
		defer close(c.send)
		for {
			_, msg, err := conn.ReadMessage()
			if err != nil {
				return
			}
			var req struct {
				Action   string `json:"action"`
				Exchange string `json:"exchange"`
				Symbol   string `json:"symbol"`
			}
			if json.Unmarshal(msg, &req) == nil &&
				req.Action == "subscribe" &&
				req.Exchange != "" && req.Symbol != "" {
				subs.set(req.Exchange, req.Symbol)
			}
		}
	}()
}

// ── Active pair subscription ──────────────────────────────────────────────────

// pairSub tracks which symbol is currently being polled per exchange.
// Each scheduler goroutine keeps its own lastVersion; when the version
// increments it knows the symbol changed and resets incremental state (e.g.
// lastSeq for trades).
type pairSub struct {
	mu      sync.Mutex
	current map[string]string
	version map[string]int64
}

var subs = &pairSub{
	current: map[string]string{"luno": "USDTZAR", "valr": "USDTZAR", "bybit": "USDTUSD", "binance": "BTCUSDT"},
	version: map[string]int64{},
}

func (s *pairSub) get(exchange string) (symbol string, version int64) {
	s.mu.Lock()
	defer s.mu.Unlock()
	return s.current[exchange], s.version[exchange]
}

func (s *pairSub) set(exchange, symbol string) {
	s.mu.Lock()
	defer s.mu.Unlock()
	if s.current[exchange] != symbol {
		s.current[exchange] = symbol
		s.version[exchange]++
	}
}

// ── Scheduler ────────────────────────────────────────────────────────────────

func reverseTradeItems(s []TradeItem) {
	for i, j := 0, len(s)-1; i < j; i, j = i+1, j-1 {
		s[i], s[j] = s[j], s[i]
	}
}

func startScheduler(lunoC *lunoClient, valrC *valrClient, bybitC *bybitClient, binanceC *binanceClient) {
	// Luno orderbook — every second
	go func() {
		t := time.NewTicker(time.Second)
		defer t.Stop()
		for range t.C {
			sym, _ := subs.get("luno")
			ob, err := lunoC.GetOrderBook(sym)
			if err != nil {
				log.Printf("luno orderbook: %v", err)
				continue
			}
			broadcastFrame(WsFrame{Type: "orderbook", Exchange: "luno", Symbol: sym, Data: ob})
		}
	}()

	// Luno trades — every 3 seconds, incremental by sequence
	go func() {
		var lastSeq int64
		var lastVer int64
		t := time.NewTicker(3 * time.Second)
		defer t.Stop()
		for range t.C {
			sym, ver := subs.get("luno")
			if ver != lastVer {
				lastSeq = 0
				lastVer = ver
			}
			// Always fetch without 'since' — Luno interprets it as a timestamp
			// and rejects values older than 24 h. Deduplicate client-side instead.
			resp, err := lunoC.GetTrades(sym, 0)
			if err != nil {
				log.Printf("luno trades: %v", err)
				continue
			}
			var newTrades []TradeItem
			for _, tr := range resp.Trades {
				if tr.Sequence > lastSeq {
					lastSeq = tr.Sequence
					newTrades = append(newTrades, tr)
				}
			}
			if len(newTrades) == 0 {
				continue
			}
			// Luno returns newest-first; reverse to oldest-first for consumers
			reverseTradeItems(newTrades)
			broadcastFrame(WsFrame{Type: "trades", Exchange: "luno", Symbol: sym, Data: newTrades})
		}
	}()

	// VALR orderbook — every 10 seconds
	go func() {
		t := time.NewTicker(10 * time.Second)
		defer t.Stop()
		for range t.C {
			sym, _ := subs.get("valr")
			ob, err := valrC.GetOrderBook(sym)
			if err != nil {
				log.Printf("valr orderbook: %v", err)
				continue
			}
			broadcastFrame(WsFrame{Type: "orderbook", Exchange: "valr", Symbol: sym, Data: ob})
		}
	}()

	// VALR trades — every 10 seconds, incremental by sequence
	go func() {
		var lastSeq int64
		var lastVer int64
		t := time.NewTicker(10 * time.Second)
		defer t.Stop()
		for range t.C {
			sym, ver := subs.get("valr")
			if ver != lastVer {
				lastSeq = 0
				lastVer = ver
			}
			resp, err := valrC.GetTrades(sym, lastSeq)
			if err != nil {
				log.Printf("valr trades: %v", err)
				continue
			}
			if len(resp.Trades) == 0 {
				continue
			}
			for _, tr := range resp.Trades {
				if tr.Sequence > lastSeq {
					lastSeq = tr.Sequence
				}
			}
			reverseTradeItems(resp.Trades)
			broadcastFrame(WsFrame{Type: "trades", Exchange: "valr", Symbol: sym, Data: resp.Trades})
		}
	}()

	// Bybit orderbook — every 5 seconds
	go func() {
		t := time.NewTicker(5 * time.Second)
		defer t.Stop()
		for range t.C {
			sym, _ := subs.get("bybit")
			ob, err := bybitC.GetOrderBook(sym)
			if err != nil {
				log.Printf("bybit orderbook: %v", err)
				continue
			}
			broadcastFrame(WsFrame{Type: "orderbook", Exchange: "bybit", Symbol: sym, Data: ob})
		}
	}()

	// Bybit trades — every 10 seconds
	go func() {
		var lastVer int64
		t := time.NewTicker(10 * time.Second)
		defer t.Stop()
		for range t.C {
			sym, ver := subs.get("bybit")
			if ver != lastVer {
				lastVer = ver
			}
			resp, err := bybitC.GetTrades(sym, 0)
			if err != nil {
				log.Printf("bybit trades: %v", err)
				continue
			}
			if len(resp.Trades) == 0 {
				continue
			}
			broadcastFrame(WsFrame{Type: "trades", Exchange: "bybit", Symbol: sym, Data: resp.Trades})
		}
	}()

	// Binance orderbook — every 5 seconds
	go func() {
		t := time.NewTicker(5 * time.Second)
		defer t.Stop()
		for range t.C {
			sym, _ := subs.get("binance")
			ob, err := binanceC.GetOrderBook(sym)
			if err != nil {
				log.Printf("binance orderbook: %v", err)
				continue
			}
			broadcastFrame(WsFrame{Type: "orderbook", Exchange: "binance", Symbol: sym, Data: ob})
		}
	}()

	// Binance trades — every 10 seconds, incremental by sequence
	go func() {
		var lastSeq int64
		var lastVer int64
		t := time.NewTicker(10 * time.Second)
		defer t.Stop()
		for range t.C {
			sym, ver := subs.get("binance")
			if ver != lastVer {
				lastSeq = 0
				lastVer = ver
			}
			resp, err := binanceC.GetTrades(sym, 0)
			if err != nil {
				log.Printf("binance trades: %v", err)
				continue
			}
			var newTrades []TradeItem
			for _, tr := range resp.Trades {
				if tr.Sequence > lastSeq {
					lastSeq = tr.Sequence
					newTrades = append(newTrades, tr)
				}
			}
			if len(newTrades) == 0 {
				continue
			}
			broadcastFrame(WsFrame{Type: "trades", Exchange: "binance", Symbol: sym, Data: newTrades})
		}
	}()
}
