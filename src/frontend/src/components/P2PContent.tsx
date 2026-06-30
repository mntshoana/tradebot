import { useState, useEffect, useCallback } from "react";
import type { LCSAd, LCSTrade } from "../types";
import { api } from "../api/client";

// ─── helpers ─────────────────────────────────────────────────────────────────

function fmtPrice(s: string, fiat: string): string {
  const n = parseFloat(s);
  if (isNaN(n)) return s;
  return n.toLocaleString(undefined, { minimumFractionDigits: 2, maximumFractionDigits: 2 }) + " " + fiat;
}

function fmtLimits(min: string, max: string, fiat: string): string {
  const lo = parseFloat(min), hi = parseFloat(max);
  if (isNaN(lo) && isNaN(hi)) return "—";
  const fmt = (n: number) => n.toLocaleString(undefined, { maximumFractionDigits: 0 });
  return `${fmt(lo)} – ${fmt(hi)} ${fiat}`;
}

function fmtDate(ts: number): string {
  if (!ts) return "—";
  return new Date(ts * 1000).toLocaleDateString(undefined, { year: "numeric", month: "short", day: "numeric" });
}

function statusBadge(status: string): string {
  const map: Record<string, string> = {
    COMPLETED: "badge-green",
    CRYPTO_ESC: "badge-yellow",
    FUND_PAID: "badge-yellow",
    CANCELLED: "badge-red",
    EXPIRED: "badge-red",
    DISPUTED: "badge-orange",
  };
  return map[status] ?? "badge-dim";
}

// ─── Tab 1: public listings ───────────────────────────────────────────────────

interface AdsTabProps {
  exchange: string;
  cryptoSlug: string;
  fiat: string;
  payment: string;
  tradeType: string;
  isLCS: boolean;
  isBybit: boolean;
  isBinance: boolean;
}

function AdsTab({ exchange, cryptoSlug, fiat, payment, tradeType, isLCS, isBybit, isBinance }: AdsTabProps) {
  const [ads,        setAds]        = useState<LCSAd[]>([]);
  const [loading,    setLoading]    = useState(false);
  const [hasMore,    setHasMore]    = useState(false);
  const [nextOffset, setNextOffset] = useState(0);
  const [error,      setError]      = useState("");

  const supported = isLCS || isBybit || isBinance;

  // Reload when any filter changes. Use a cancellation flag so that responses
  // from a previous (stale) filter combination cannot overwrite results for
  // the current one — switching exchanges fires this effect twice in quick
  // succession (once with empty defaults, once with loaded options).
  useEffect(() => {
    if (!supported) return;
    let cancelled = false;
    setAds([]);
    setHasMore(false);
    setNextOffset(0);
    setError("");
    setLoading(true);
    const req = isBinance
      ? api.binanceAds({ crypto: cryptoSlug, fiat, payment, tradeType, offset: 1 })
      : isBybit
        ? api.bybitAds({ crypto: cryptoSlug, fiat, tradeType, offset: 1 })
        : api.lcsAds({ crypto: cryptoSlug, fiat, payment, tradeType, offset: 0 });
    req
      .then(r => {
        if (cancelled) return;
        setAds(r.ads);
        setHasMore(r.hasMore);
        setNextOffset(r.nextOffset);
      })
      .catch(e => { if (!cancelled) setError((e as Error).message); })
      .finally(() => { if (!cancelled) setLoading(false); });
    return () => { cancelled = true; };
  }, [supported, isBybit, isBinance, exchange, cryptoSlug, fiat, payment, tradeType]);

  const loadMore = useCallback(() => {
    if (!hasMore || loading) return;
    setLoading(true);
    const req = isBinance
      ? api.binanceAds({ crypto: cryptoSlug, fiat, payment, tradeType, offset: nextOffset })
      : isBybit
        ? api.bybitAds({ crypto: cryptoSlug, fiat, tradeType, offset: nextOffset })
        : api.lcsAds({ crypto: cryptoSlug, fiat, payment, tradeType, offset: nextOffset });
    req
      .then(r => {
        setAds(prev => [...prev, ...r.ads]);
        setHasMore(r.hasMore);
        setNextOffset(r.nextOffset);
      })
      .catch(e => setError((e as Error).message))
      .finally(() => setLoading(false));
  }, [hasMore, loading, isBybit, isBinance, cryptoSlug, fiat, payment, tradeType, nextOffset]);

  if (!supported) {
    return <div className="p2p-empty">Public listings not yet supported for {exchange}.</div>;
  }

  // Sort by price: ascending when buying (cheapest first = best deal for buyer),
  // descending when selling (highest first = best deal for seller).
  const sortedAds = [...ads].sort((a, b) => {
    const pa = parseFloat(a.currentPrice) || 0;
    const pb = parseFloat(b.currentPrice) || 0;
    return tradeType === "sell" ? pb - pa : pa - pb;
  });

  return (
    <div className="p2p-tab-content">
      {error && <div className="p2p-error">{error}</div>}
      {ads.length === 0 && !loading && !error && (
        <div className="p2p-empty">
          No {tradeType === "buy" ? "buy" : "sell"} ads found for the selected filters.
          <br />
          <span className="p2p-empty-hint">Try a different crypto network, fiat or payment method.</span>
        </div>
      )}
      {ads.length > 0 && (
        <div className="p2p-table-wrap">
          <table className="p2p-table">
            <thead>
              <tr>
                <th>Trader</th>
                <th>Ad Title</th>
                <th>Payment</th>
                <th>Limits</th>
                <th className="right">Price</th>
              </tr>
            </thead>
            <tbody>
              {sortedAds.map(ad => (
                <tr key={ad.uuid}>
                  <td>
                    <div className="p2p-trader">
                      <span className="p2p-username">{ad.createdBy.username}</span>
                      <span className="p2p-meta">
                        {ad.createdBy.completedTrades} trades
                        {ad.createdBy.feedbackScore > 0 && ` · ${ad.createdBy.feedbackScore.toFixed(0)}%`}
                        {ad.createdBy.avgResponseTime > 0 && ` · ~${ad.createdBy.avgResponseTime}m`}
                      </span>
                    </div>
                  </td>
                  <td className="p2p-headline">{ad.headline || `${ad.tradingType === "buy" ? "Buy" : "Sell"} ${ad.cryptoSymbol}`}</td>
                  <td>{ad.paymentMethod}</td>
                  <td className="mono">{fmtLimits(ad.minTradeSize, ad.maxTradeSize, ad.fiatSymbol)}</td>
                  <td className={`right mono p2p-price p2p-price--${tradeType === "sell" ? "sell" : "buy"}`}>{fmtPrice(ad.currentPrice, ad.fiatSymbol)}</td>
                </tr>
              ))}
            </tbody>
          </table>
        </div>
      )}
      {loading && <div className="p2p-loading">Loading…</div>}
      {!loading && hasMore && (
        <button className="p2p-load-more" onClick={loadMore}>Load more</button>
      )}
    </div>
  );
}

// ─── Tab 2: my ads ────────────────────────────────────────────────────────────

interface MyAdsTabProps {
  exchange: string;
  isLCS:    boolean;
  isBybit:  boolean;
}

function MyAdsTab({ exchange, isLCS, isBybit }: MyAdsTabProps) {
  const [ads,        setAds]        = useState<LCSAd[]>([]);
  const [loading,    setLoading]    = useState(false);
  const [hasMore,    setHasMore]    = useState(false);
  const [nextOffset, setNextOffset] = useState(0);
  const [error,      setError]      = useState("");

  const supported = isLCS || isBybit;

  useEffect(() => {
    if (!supported) return;
    let cancelled = false;
    setAds([]); setHasMore(false); setNextOffset(0); setError("");
    setLoading(true);
    const req = isBybit ? api.bybitMyAds(1) : api.lcsMyAds(0);
    req
      .then(r => {
        if (cancelled) return;
        setAds(r.ads); setHasMore(r.hasMore); setNextOffset(r.nextOffset);
      })
      .catch(e => { if (!cancelled) setError((e as Error).message); })
      .finally(() => { if (!cancelled) setLoading(false); });
    return () => { cancelled = true; };
  }, [supported, isBybit, exchange]);

  const loadMore = useCallback(() => {
    if (!hasMore || loading) return;
    setLoading(true);
    const req = isBybit ? api.bybitMyAds(nextOffset) : api.lcsMyAds(nextOffset);
    req
      .then(r => {
        setAds(prev => [...prev, ...r.ads]);
        setHasMore(r.hasMore);
        setNextOffset(r.nextOffset);
      })
      .catch(e => setError((e as Error).message))
      .finally(() => setLoading(false));
  }, [hasMore, loading, isBybit, nextOffset]);

  if (!supported) {
    return <div className="p2p-empty">Ad management not yet supported for this exchange.</div>;
  }

  const newAdHref = isBybit
    ? "https://www.bybit.com/en/fiat/trade/otc/my-ads"
    : "https://localcoinswap.com/offers/create/";

  return (
    <div className="p2p-tab-content">
      <div className="p2p-myads-header">
        <span className="p2p-myads-title">My Ads</span>
        <a
          className="p2p-btn"
          href={newAdHref}
          target="_blank"
          rel="noopener noreferrer"
        >
          + New Ad
        </a>
      </div>
      {error && <div className="p2p-error">{error}</div>}
      {ads.length === 0 && !loading && !error && (
        <div className="p2p-empty">You have no ads yet.</div>
      )}
      {ads.length > 0 && (
        <div className="p2p-table-wrap">
          <table className="p2p-table">
            <thead>
              <tr>
                <th>Status</th>
                <th>Type</th>
                <th>Pair</th>
                <th>Payment</th>
                <th>Limits</th>
                <th className="right">Price</th>
              </tr>
            </thead>
            <tbody>
              {ads.map(ad => (
                <tr key={ad.uuid}>
                  <td>
                    <span className={`badge ${ad.isActive ? "badge-green" : "badge-dim"}`}>
                      {ad.isActive ? "Active" : "Inactive"}
                    </span>
                  </td>
                  <td className="p2p-type">{ad.tradingType === "buy" ? "Buy" : "Sell"}</td>
                  <td className="mono">{ad.cryptoSymbol}/{ad.fiatSymbol}</td>
                  <td>{ad.paymentMethod}</td>
                  <td className="mono">{fmtLimits(ad.minTradeSize, ad.maxTradeSize, ad.fiatSymbol)}</td>
                  <td className={`right mono p2p-price p2p-price--${ad.tradingType === "sell" ? "sell" : "buy"}`}>{fmtPrice(ad.currentPrice, ad.fiatSymbol)}</td>
                </tr>
              ))}
            </tbody>
          </table>
        </div>
      )}
      {loading && <div className="p2p-loading">Loading…</div>}
      {!loading && hasMore && (
        <button className="p2p-load-more" onClick={loadMore}>Load more</button>
      )}
    </div>
  );
}

// ─── Tab 3: trade history ─────────────────────────────────────────────────────

interface TradesTabProps {
  exchange:   string;
  cryptoSlug: string;
  fiat:       string;
  payment:    string;
  tradeType:  string;
  isLCS:      boolean;
  isBybit:    boolean;
  isBinance:  boolean;
}

function TradesTab({ exchange, cryptoSlug, fiat, payment, tradeType, isLCS, isBybit, isBinance }: TradesTabProps) {
  const [trades,     setTrades]     = useState<LCSTrade[]>([]);
  const [loading,    setLoading]    = useState(false);
  const [hasMore,    setHasMore]    = useState(false);
  const [nextOffset, setNextOffset] = useState(0);
  const [error,      setError]      = useState("");

  const supported = isLCS || isBybit || isBinance;

  useEffect(() => {
    if (!supported) return;
    let cancelled = false;
    setTrades([]);
    setHasMore(false);
    setNextOffset(0);
    setError("");
    setLoading(true);
    const req = isBinance
      ? api.binanceTrades({ crypto: cryptoSlug, fiat, tradeType, offset: 1 })
      : isBybit
        ? api.bybitTrades({ crypto: cryptoSlug, fiat, tradeType, offset: 1 })
        : api.lcsTrades({ crypto: cryptoSlug, fiat, payment, tradeType, offset: 0 });
    req
      .then(r => {
        if (cancelled) return;
        setTrades(r.trades); setHasMore(r.hasMore); setNextOffset(r.nextOffset);
      })
      .catch(e => { if (!cancelled) setError((e as Error).message); })
      .finally(() => { if (!cancelled) setLoading(false); });
    return () => { cancelled = true; };
  }, [supported, isBybit, isBinance, exchange, cryptoSlug, fiat, payment, tradeType]);

  const loadMore = useCallback(() => {
    if (!hasMore || loading) return;
    setLoading(true);
    const req = isBinance
      ? api.binanceTrades({ crypto: cryptoSlug, fiat, tradeType, offset: nextOffset })
      : isBybit
        ? api.bybitTrades({ crypto: cryptoSlug, fiat, tradeType, offset: nextOffset })
        : api.lcsTrades({ crypto: cryptoSlug, fiat, payment, tradeType, offset: nextOffset });
    req
      .then(r => {
        setTrades(prev => [...prev, ...r.trades]);
        setHasMore(r.hasMore);
        setNextOffset(r.nextOffset);
      })
      .catch(e => setError((e as Error).message))
      .finally(() => setLoading(false));
  }, [hasMore, loading, isBybit, isBinance, cryptoSlug, fiat, payment, tradeType, nextOffset]);

  if (!supported) {
    return <div className="p2p-empty">Trade history not yet supported for {exchange}.</div>;
  }

  return (
    <div className="p2p-tab-content">
      {error && <div className="p2p-error">{error}</div>}
      {trades.length === 0 && !loading && !error && (
        <div className="p2p-empty">No trades found for the selected filters.</div>
      )}
      {trades.length > 0 && (
        <div className="p2p-table-wrap">
          <table className="p2p-table">
            <thead>
              <tr>
                <th>Date</th>
                <th>Partner</th>
                <th>Status</th>
                <th>Type</th>
                <th>Crypto</th>
                <th>Fiat Amt</th>
                <th className="right">Coin Amt</th>
              </tr>
            </thead>
            <tbody>
              {trades.map(t => (
                <tr key={t.uuid}>
                  <td className="p2p-date">{fmtDate(t.timeCreated)}</td>
                  <td className="p2p-username">{t.partner}</td>
                  <td>
                    <span className={`badge ${statusBadge(t.status)}`}>
                      {t.status.replace(/_/g, " ")}
                    </span>
                  </td>
                  <td className="p2p-type">{t.tradingType === "buy" ? "Buy" : "Sell"}</td>
                  <td className="mono">{t.cryptoSymbol}</td>
                  <td className="mono">{t.fiatAmount} {t.fiatSymbol}</td>
                  <td className="right mono">{t.coinAmount}</td>
                </tr>
              ))}
            </tbody>
          </table>
        </div>
      )}
      {loading && <div className="p2p-loading">Loading…</div>}
      {!loading && hasMore && (
        <button className="p2p-load-more" onClick={loadMore}>Load more</button>
      )}
    </div>
  );
}

// ─── Shell ────────────────────────────────────────────────────────────────────

type Tab = "listings" | "my-ads" | "trades";

interface Props {
  exchange:   string;
  cryptoSlug: string; // LCS slug for the selected crypto; "" = unselected
  fiat:       string;
  payment:    string; // payment method slug
  tradeType:  string; // "buy" | "sell"
}

export function P2PContent({ exchange, cryptoSlug, fiat, payment, tradeType }: Props) {
  const [tab, setTab] = useState<Tab>("listings");
  const isLCS     = exchange === "localcoinswap-p2p";
  const isBybit   = exchange === "bybit-p2p";
  const isBinance = exchange === "binance-p2p";

  return (
    <div className="p2p-content">
      <div className="p2p-tabs">
        <button
          className={`p2p-tab${tab === "listings" ? " p2p-tab--active" : ""}`}
          onClick={() => setTab("listings")}
        >
          Listings
        </button>
        <button
          className={`p2p-tab${tab === "my-ads" ? " p2p-tab--active" : ""}`}
          onClick={() => setTab("my-ads")}
        >
          My Ads
        </button>
        <button
          className={`p2p-tab${tab === "trades" ? " p2p-tab--active" : ""}`}
          onClick={() => setTab("trades")}
        >
          Trade History
        </button>
      </div>

      {tab === "listings" && (
        <AdsTab
          exchange={exchange}
          cryptoSlug={cryptoSlug}
          fiat={fiat}
          payment={payment}
          tradeType={tradeType}
          isLCS={isLCS}
          isBybit={isBybit}
          isBinance={isBinance}
        />
      )}
      {tab === "my-ads" && <MyAdsTab exchange={exchange} isLCS={isLCS} isBybit={isBybit} />}
      {tab === "trades" && (
        <TradesTab
          exchange={exchange}
          cryptoSlug={cryptoSlug}
          fiat={fiat}
          payment={payment}
          tradeType={tradeType}
          isLCS={isLCS}
          isBybit={isBybit}
          isBinance={isBinance}
        />
      )}
    </div>
  );
}
