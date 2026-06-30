// Thin wrapper around fetch for the Go sidecar REST API at localhost:8080.
// The React frontend never touches exchange APIs directly — all actions go
// through the Go server.

import type { Balance, OpenOrder, OrderBook, Trade, P2POption, P2PPaymentMethodsResponse, LCSAdsResponse, LCSTradesResponse } from "../types";

const BASE = "http://localhost:8080";

async function request<T>(
  method: string,
  path: string,
  body?: unknown
): Promise<T> {
  const res = await fetch(BASE + path, {
    method,
    headers: { "Content-Type": "application/json" },
    body: body !== undefined ? JSON.stringify(body) : undefined,
  });
  if (!res.ok) {
    const err = await res.json().catch(() => ({ error: res.statusText }));
    throw new Error((err as { error?: string }).error ?? res.statusText);
  }
  return res.json() as Promise<T>;
}

export const api = {
  // ── Market ──────────────────────────────────────────────────────────────
  pairs: (exchange: string) =>
    request<Array<{ pairId: string; label: string; minBase: number }>>(
      "GET",
      `/market/pairs?exchange=${exchange}`
    ),
  // ── P2P dropdowns ───────────────────────────────────────────────────────────────
  p2pCryptos: (exchange: string) =>
    request<{ exchange: string; options: P2POption[] }>(
      "GET",
      `/p2p/cryptos?exchange=${exchange}`
    ),
  p2pFiats: (exchange: string) =>
    request<{ exchange: string; options: P2POption[] }>(
      "GET",
      `/p2p/fiats?exchange=${exchange}`
    ),
  p2pPaymentMethods: (exchange: string, offset = 0) =>
    request<P2PPaymentMethodsResponse>(
      "GET",
      `/p2p/payment-methods?exchange=${exchange}&offset=${offset}`
    ),
  // ── LCS content tabs ───────────────────────────────────────────────────────
  lcsAds: (params: { crypto?: string; fiat?: string; payment?: string; tradeType?: string; offset?: number }) => {
    const q = new URLSearchParams();
    if (params.crypto)    q.set("crypto",    params.crypto);
    if (params.fiat)      q.set("fiat",      params.fiat);
    if (params.payment)   q.set("payment",   params.payment);
    if (params.tradeType) q.set("tradeType", params.tradeType);
    if (params.offset)    q.set("offset",    String(params.offset));
    return request<LCSAdsResponse>("GET", `/p2p/lcs/ads?${q}`);
  },
  lcsMyAds: (offset = 0) =>
    request<LCSAdsResponse>("GET", `/p2p/lcs/my-ads?offset=${offset}`),
  lcsTrades: (params: { crypto?: string; fiat?: string; payment?: string; tradeType?: string; offset?: number }) => {
    const q = new URLSearchParams();
    if (params.crypto)    q.set("crypto",    params.crypto);
    if (params.fiat)      q.set("fiat",      params.fiat);
    if (params.payment)   q.set("payment",   params.payment);
    if (params.tradeType) q.set("tradeType", params.tradeType);
    if (params.offset)    q.set("offset",    String(params.offset));
    return request<LCSTradesResponse>("GET", `/p2p/lcs/trades?${q}`);
  },
  // ── Bybit P2P content tabs ─────────────────────────────────────────────────
  bybitAds: (params: { crypto?: string; fiat?: string; tradeType?: string; offset?: number }) => {
    const q = new URLSearchParams();
    if (params.crypto)    q.set("crypto",    params.crypto);
    if (params.fiat)      q.set("fiat",      params.fiat);
    if (params.tradeType) q.set("tradeType", params.tradeType);
    if (params.offset)    q.set("offset",    String(params.offset ?? 1));
    return request<LCSAdsResponse>("GET", `/p2p/bybit/ads?${q}`);
  },
  bybitMyAds: (offset = 1) =>
    request<LCSAdsResponse>("GET", `/p2p/bybit/my-ads?offset=${offset}`),
  bybitTrades: (params: { crypto?: string; fiat?: string; tradeType?: string; offset?: number }) => {
    const q = new URLSearchParams();
    if (params.crypto)    q.set("crypto",    params.crypto);
    if (params.fiat)      q.set("fiat",      params.fiat);
    if (params.tradeType) q.set("tradeType", params.tradeType);
    if (params.offset)    q.set("offset",    String(params.offset ?? 1));
    return request<LCSTradesResponse>("GET", `/p2p/bybit/trades?${q}`);
  },
  // ── Binance P2P content tabs ───────────────────────────────────────────────
  binanceAds: (params: { crypto?: string; fiat?: string; payment?: string; tradeType?: string; offset?: number }) => {
    const q = new URLSearchParams();
    if (params.crypto)    q.set("crypto",    params.crypto);
    if (params.fiat)      q.set("fiat",      params.fiat);
    if (params.payment)   q.set("payment",   params.payment);
    if (params.tradeType) q.set("tradeType", params.tradeType);
    if (params.offset)    q.set("offset",    String(params.offset ?? 1));
    return request<LCSAdsResponse>("GET", `/p2p/binance/ads?${q}`);
  },
  binanceTrades: (params: { crypto?: string; fiat?: string; tradeType?: string; offset?: number }) => {
    const q = new URLSearchParams();
    if (params.crypto)    q.set("crypto",    params.crypto);
    if (params.fiat)      q.set("fiat",      params.fiat);
    if (params.tradeType) q.set("tradeType", params.tradeType);
    if (params.offset)    q.set("offset",    String(params.offset ?? 1));
    return request<LCSTradesResponse>("GET", `/p2p/binance/trades?${q}`);
  },
  orderBook: (exchange: string, pair: string) =>
    request<{ exchange: string; symbol: string; timestamp: number } & OrderBook>(
      "GET",
      `/market/orderbook?exchange=${exchange}&pair=${pair}`
    ),

  fees: (exchange: string, pair: string) =>
    request<{ exchange: string; symbol: string; maker: number; taker: number }>(
      "GET",
      `/market/fees?exchange=${exchange}&pair=${pair}`
    ),

  // ── Account ──────────────────────────────────────────────────────────────
  balances: (exchange: string) =>
    request<{ exchange: string; balances: Balance[] }>(
      "GET",
      `/account/balances?exchange=${exchange}`
    ),

  openOrders: (exchange: string, pair?: string) =>
    request<{ exchange: string; orders: OpenOrder[] }>(
      "GET",
      `/account/orders/open?exchange=${exchange}${pair ? `&pair=${pair}` : ""}`
    ),

  // ── Order management ─────────────────────────────────────────────────────
  postLimitOrder: (body: {
    exchange: string;
    pair: string;
    side: string;
    price: number;
    amount: number;
    postOnly: boolean;
  }) => request<{ orderId: string }>("POST", "/account/orders/limit", body),

  cancelOrder: (id: string, exchange: string, pair?: string) =>
    request<void>(
      "DELETE",
      `/account/orders/${id}?exchange=${exchange}${pair ? `&pair=${pair}` : ""}`
    ),

  getOrder: (id: string, exchange: string) =>
    request<OpenOrder>("GET", `/account/orders/${id}?exchange=${exchange}`),

  // ── Withdrawals ──────────────────────────────────────────────────────────
  beneficiaries: (exchange: string) =>
    request<{ exchange: string; beneficiaries: unknown[] }>(
      "GET",
      `/account/beneficiaries?exchange=${exchange}`
    ),

  withdraw: (body: {
    exchange: string;
    asset: string;
    amount: number;
    isFast: boolean;
    beneficiaryId?: string;
  }) => request<{ id: string; status: string; createdAt: number }>("POST", "/account/withdrawals", body),

  withdrawals: (exchange: string) =>
    request<{ exchange: string; withdrawals: unknown[] }>(
      "GET",
      `/account/withdrawals?exchange=${exchange}`
    ),

  cancelWithdrawal: (id: string, exchange: string) =>
    request<void>("DELETE", `/account/withdrawals/${id}?exchange=${exchange}`),
} as const;

// Re-export Trade so consumers don't need a separate import
export type { Trade };
