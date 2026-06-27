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
}

var wsHub = &hub{clients: make(map[*wsConn]struct{})}

func (h *hub) register(c *wsConn) {
	h.mu.Lock()
	h.clients[c] = struct{}{}
	h.mu.Unlock()
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

	// read pump — drains pings and detects client disconnect
	go func() {
		defer close(c.send)
		for {
			if _, _, err := conn.ReadMessage(); err != nil {
				return
			}
		}
	}()
}

// ── Scheduler ────────────────────────────────────────────────────────────────

func reverseTradeItems(s []TradeItem) {
	for i, j := 0, len(s)-1; i < j; i, j = i+1, j-1 {
		s[i], s[j] = s[j], s[i]
	}
}

func startScheduler(lunoC *lunoClient, valrC *valrClient) {
	// Luno orderbook — every second
	go func() {
		t := time.NewTicker(time.Second)
		defer t.Stop()
		for range t.C {
			ob, err := lunoC.GetOrderBook("XBTZAR")
			if err != nil {
				log.Printf("luno orderbook: %v", err)
				continue
			}
			broadcastFrame(WsFrame{Type: "orderbook", Exchange: "luno", Symbol: "XBTZAR", Data: ob})
		}
	}()

	// Luno trades — every 3 seconds, incremental by sequence
	go func() {
		var lastSeq int64
		t := time.NewTicker(3 * time.Second)
		defer t.Stop()
		for range t.C {
			resp, err := lunoC.GetTrades("XBTZAR", lastSeq)
			if err != nil {
				log.Printf("luno trades: %v", err)
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
			// Luno returns newest-first; reverse to oldest-first for consumers
			reverseTradeItems(resp.Trades)
			broadcastFrame(WsFrame{Type: "trades", Exchange: "luno", Symbol: "XBTZAR", Data: resp.Trades})
		}
	}()

	// VALR orderbook — every 10 seconds
	go func() {
		t := time.NewTicker(10 * time.Second)
		defer t.Stop()
		for range t.C {
			ob, err := valrC.GetOrderBook("BTCZAR")
			if err != nil {
				log.Printf("valr orderbook: %v", err)
				continue
			}
			broadcastFrame(WsFrame{Type: "orderbook", Exchange: "valr", Symbol: "BTCZAR", Data: ob})
		}
	}()

	// VALR trades — every 10 seconds, incremental by sequence
	go func() {
		var lastSeq int64
		t := time.NewTicker(10 * time.Second)
		defer t.Stop()
		for range t.C {
			resp, err := valrC.GetTrades("BTCZAR", lastSeq)
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
			broadcastFrame(WsFrame{Type: "trades", Exchange: "valr", Symbol: "BTCZAR", Data: resp.Trades})
		}
	}()
}
