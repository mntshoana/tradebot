import { useCallback, useEffect, useRef, useState } from "react";
import type { OrderBook, Trade, WsFrame } from "../types";

const WS_URL  = "ws://localhost:8080/ws";
const MAX_TRADES = 200;

export function useWebSocket(exchange: string, symbol: string) {
  const [orderBook, setOrderBook] = useState<OrderBook | null>(null);
  const [trades,    setTrades]    = useState<Trade[]>([]);
  const [connected, setConnected] = useState(false);

  const wsRef        = useRef<WebSocket | null>(null);
  const reconnectRef = useRef<ReturnType<typeof setTimeout> | null>(null);

  // Always-current ref so WS callbacks don't capture stale exchange/symbol.
  const subRef = useRef({ exchange, symbol });
  subRef.current = { exchange, symbol };

  const sendSubscribe = useCallback((ws: WebSocket, ex: string, sym: string) => {
    if (ws.readyState === WebSocket.OPEN && ex && sym) {
      ws.send(JSON.stringify({ action: "subscribe", exchange: ex, symbol: sym }));
    }
  }, []);

  const connect = useCallback(() => {
    if (reconnectRef.current !== null) {
      clearTimeout(reconnectRef.current);
      reconnectRef.current = null;
    }

    const ws = new WebSocket(WS_URL);
    wsRef.current = ws;

    ws.onopen = () => {
      setConnected(true);
      const { exchange: ex, symbol: sym } = subRef.current;
      sendSubscribe(ws, ex, sym);
    };

    ws.onmessage = (evt: MessageEvent<string>) => {
      const frame = JSON.parse(evt.data) as WsFrame;
      const { exchange: ex, symbol: sym } = subRef.current;
      // Discard frames that don't match the currently subscribed pair.
      if (frame.exchange !== ex || frame.symbol !== sym) return;

      if (frame.type === "orderbook") {
        setOrderBook(frame.data);
      } else if (frame.type === "trades") {
        setTrades(prev => [...prev, ...frame.data].slice(-MAX_TRADES));
      }
    };

    ws.onclose = () => {
      setConnected(false);
      reconnectRef.current = setTimeout(connect, 3000);
    };

    ws.onerror = () => ws.close();
  }, [sendSubscribe]); // eslint-disable-line react-hooks/exhaustive-deps

  useEffect(() => {
    connect();
    return () => {
      if (reconnectRef.current !== null) clearTimeout(reconnectRef.current);
      wsRef.current?.close();
    };
  }, [connect]);

  // When the subscribed pair changes: clear stale data, tell the server.
  useEffect(() => {
    if (!exchange || !symbol) return;
    setOrderBook(null);
    setTrades([]);
    sendSubscribe(wsRef.current!, exchange, symbol);
  }, [exchange, symbol, sendSubscribe]);

  return { orderBook, trades, connected };
}

