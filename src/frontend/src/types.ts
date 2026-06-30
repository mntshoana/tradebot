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

// P2P dropdown option — used for crypto, fiat, and payment method selectors.
export interface P2POption {
  symbol: string;
  label:  string;
  slug?:  string; // LCS currency/payment slug used for search API calls
}

// Response type for /p2p/payment-methods — includes pagination info.
export interface P2PPaymentMethodsResponse {
  exchange:   string;
  options:    P2POption[];
  hasMore:    boolean;
  nextOffset: number;
}

// LCS public ad (offer) from /p2p/lcs/ads
export interface LCSAdCreator {
  username:        string;
  feedbackScore:   number;
  completedTrades: number;
  avgResponseTime: number;
  activityStatus:  string;
}

export interface LCSAd {
  uuid:          string;
  slug:          string;
  createdBy:     LCSAdCreator;
  tradingType:   string; // "buy" | "sell"
  cryptoSymbol:  string;
  fiatSymbol:    string;
  paymentMethod: string;
  headline:      string;
  minTradeSize:  string;
  maxTradeSize:  string;
  currentPrice:  string;
  isActive:      boolean;
}

export interface LCSAdsResponse {
  total:      number;
  ads:        LCSAd[];
  hasMore:    boolean;
  nextOffset: number;
}

// LCS trade from /p2p/lcs/trades
export interface LCSTrade {
  uuid:          string;
  status:        string;
  tradingType:   string;
  cryptoSymbol:  string;
  fiatSymbol:    string;
  paymentMethod: string;
  partner:       string;
  fiatAmount:    string;
  coinAmount:    string;
  timeCreated:   number;
}

export interface LCSTradesResponse {
  total:      number;
  trades:     LCSTrade[];
  hasMore:    boolean;
  nextOffset: number;
}
