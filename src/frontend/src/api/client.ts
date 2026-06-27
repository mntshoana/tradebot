// Thin wrapper around fetch for the Go sidecar REST API at localhost:8080.
// The React frontend never touches exchange APIs directly — all actions go
// through the Go server.

import type { Balance, OpenOrder, OrderBook, Trade } from "../types";

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
  orderBook: (exchange: string, pair: string) =>
    request<{ exchange: string; symbol: string; timestamp: number } & OrderBook>(
      "GET",
      `/market/orderbook?exchange=${exchange}&pair=${pair}`
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
