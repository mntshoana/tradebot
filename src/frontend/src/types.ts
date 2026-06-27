// Mirrors the Go types in goserver/types.go and goserver/scheduler.go

export type Side = "buy" | "sell";

export interface OrderBook {
  exchange:  string;
  symbol:    string;
  timestamp: number;
  asks:      [number, number][]; // [price, amount] — cheapest first
  bids:      [number, number][]; // [price, amount] — highest first
}

export interface Trade {
  sequence:  number;
  timestamp: number;
  price:     number;
  amount:    number;
  cost:      number; // price × amount
  side:      Side;
}

export interface Balance {
  asset:       string;
  accountId:   string;
  free:        number;
  used:        number;
  total:       number;
  unconfirmed: number;
}

export interface OpenOrder {
  id:        string;
  symbol:    string;
  side:      Side;
  price:     number;
  amount:    number;
  filled:    number;
  remaining: number;
  status:    string;
  createdAt: number;
}

// WebSocket push frame streamed by goserver/scheduler.go
export type WsFrame =
  | { type: "orderbook"; exchange: string; symbol: string; data: OrderBook }
  | { type: "trades";    exchange: string; symbol: string; data: Trade[] };
