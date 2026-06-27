import { useCallback, useEffect, useRef, useState } from "react";
import type { OrderBook, Trade, WsFrame } from "../types";

const WS_URL = "ws://localhost:8080/ws";
const MAX_TRADES = 200; // keep at most this many trades per exchange in memory

export interface MarketState {
  lunoOrderBook: OrderBook | null;
  valrOrderBook: OrderBook | null;
  lunoTrades:    Trade[];
  valrTrades:    Trade[];
  connected:     boolean;
}

export function useWebSocket(): MarketState {
  const [state, setState] = useState<MarketState>({
    lunoOrderBook: null,
    valrOrderBook: null,
    lunoTrades:    [],
    valrTrades:    [],
    connected:     false,
  });

  const wsRef        = useRef<WebSocket | null>(null);
  const reconnectRef = useRef<ReturnType<typeof setTimeout> | null>(null);

  const connect = useCallback(() => {
    // Clear any pending reconnect timer
    if (reconnectRef.current !== null) {
      clearTimeout(reconnectRef.current);
      reconnectRef.current = null;
    }

    const ws = new WebSocket(WS_URL);
    wsRef.current = ws;

    ws.onopen = () => setState((s) => ({ ...s, connected: true }));

    ws.onmessage = (evt: MessageEvent<string>) => {
      const frame = JSON.parse(evt.data) as WsFrame;

      if (frame.type === "orderbook") {
        if (frame.exchange === "luno")
          setState((s) => ({ ...s, lunoOrderBook: frame.data }));
        else
          setState((s) => ({ ...s, valrOrderBook: frame.data }));
      } else if (frame.type === "trades") {
        if (frame.exchange === "luno")
          setState((s) => ({
            ...s,
            lunoTrades: [...s.lunoTrades, ...frame.data].slice(-MAX_TRADES),
          }));
        else
          setState((s) => ({
            ...s,
            valrTrades: [...s.valrTrades, ...frame.data].slice(-MAX_TRADES),
          }));
      }
    };

    ws.onclose = () => {
      setState((s) => ({ ...s, connected: false }));
      reconnectRef.current = setTimeout(connect, 3000);
    };

    ws.onerror = () => ws.close(); // onclose will trigger reconnect
  }, []); // eslint-disable-line react-hooks/exhaustive-deps

  useEffect(() => {
    connect();
    return () => {
      if (reconnectRef.current !== null) clearTimeout(reconnectRef.current);
      wsRef.current?.close();
    };
  }, [connect]);

  return state;
}
