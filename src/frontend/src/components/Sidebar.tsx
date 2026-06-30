import { useState, useEffect } from "react";
import { api } from "../api/client";
import { EXCHANGE_META } from "../exchanges";
import type { PairCfg } from "../exchanges";
import type { OrderBook, Trade, Balance, P2POption } from "../types";
import { SearchableSelect } from "./SearchableSelect";

const OB_ROWS = 8;

function fmtTime(ms: number): string {
  return new Date(ms).toTimeString().slice(0, 5);
}

// Decimal places based on price magnitude so every pair looks right:
// XBT/ZAR ~1 500 000 → 0 dp | ETH/ZAR ~50 000 → 0 dp
// AAVE/ZAR ~1 500  → 2 dp | USDT/ZAR ~16.56 → 4 dp | sub-1 → 6 dp
function fmtPrice(p: number): string {
  if (p >= 10_000) return p.toFixed(0);
  if (p >= 100)    return p.toFixed(2);
  if (p >= 1)      return p.toFixed(4);
  return p.toFixed(6);
}

// Format a fee decimal (e.g. 0.001) as a percentage string (e.g. "0.1%").
function fmtFee(v: number): string {
  return (v * 100).toFixed(4).replace(/\.?0+$/, '') + '%';
}

interface Props {
  exchange:         string;
  pair:             string;
  pairId:           string;
  pairs:            Record<string, PairCfg>;
  orderBook:        OrderBook | null;
  trades:           Trade[];
  onExchangeChange: (e: string) => void;
  onPairChange:     (p: string) => void;
  // P2P-specific props (only used when isP2P is true)
  isP2P?:                boolean;
  p2pCryptos?:           P2POption[];
  p2pFiats?:             P2POption[];
  p2pPaymentMethods?:    P2POption[];
  p2pCrypto?:            string;
  p2pFiat?:              string;
  p2pPaymentMethod?:     string;
  p2pTradeType?:         "buy" | "sell";
  onP2PCryptoChange?:        (v: string) => void;
  onP2PFiatChange?:          (v: string) => void;
  onP2PPaymentMethodChange?: (v: string) => void;
  onP2PTradeTypeChange?:     (v: "buy" | "sell") => void;
  p2pPaymentMethodsHasMore?: boolean;
  onLoadMorePaymentMethods?: () => void;
}

export function Sidebar({ exchange, pair, pairId, pairs, orderBook, trades, onExchangeChange, onPairChange,
  isP2P, p2pCryptos = [], p2pFiats = [], p2pPaymentMethods = [],
  p2pCrypto = "", p2pFiat = "", p2pPaymentMethod = "", p2pTradeType = "buy",
  onP2PCryptoChange, onP2PFiatChange, onP2PPaymentMethodChange, onP2PTradeTypeChange,
  p2pPaymentMethodsHasMore = false, onLoadMorePaymentMethods,
}: Props) {
  const [side,     setSide]    = useState<"buy" | "sell">("buy");
  const [price,    setPrice]   = useState("");
  const [amount,   setAmount]  = useState("");
  const [postOnly, setPostOnly] = useState(true);
  const [msg,      setMsg]     = useState<{ text: string; ok: boolean } | null>(null);
  const [loading,  setLoading] = useState(false);
  const [feeInfo,  setFeeInfo] = useState<{ maker: number; taker: number } | null>(null);
  const [balances, setBalances] = useState<Balance[]>([]);

  useEffect(() => {
    setFeeInfo(null);
    api.fees(exchange, pairId)
      .then(f => setFeeInfo({ maker: f.maker, taker: f.taker }))
      .catch(() => setFeeInfo(null));
  }, [exchange, pairId]);

  useEffect(() => {
    setBalances([]);
    api.balances(exchange)
      .then(r => setBalances(r.balances ?? []))
      .catch(() => setBalances([]));
  }, [exchange]);

  // Derive [base, quote] from the selected pair label (e.g. "USDT/ZAR" → ["USDT", "ZAR"])
  const [baseAsset, quoteAsset] = (pairs[pair]?.label ?? "").split("/");

  // Click a price in the order book or last trades: populate Price field and
  // auto-fill Amount to the maximum the user can trade given their balances.
  function handlePriceClick(p: number) {
    setPrice(fmtPrice(p));
    setMsg(null);
    if (!baseAsset || !quoteAsset) return;
    if (side === "buy") {
      const quoteBal = balances.find(b => b.asset.toUpperCase() === quoteAsset.toUpperCase())?.free ?? 0;
      if (quoteBal <= 0 || p <= 0) return;
      const raw = quoteBal / p;
      // Truncate to 8 significant decimal places without rounding up
      setAmount(parseFloat(raw.toFixed(8)).toString());
    } else {
      const baseBal = balances.find(b => b.asset.toUpperCase() === baseAsset.toUpperCase())?.free ?? 0;
      setAmount(parseFloat(baseBal.toFixed(8)).toString());
    }
  }

  const rawAsks = orderBook?.asks ?? [];
  const rawBids = orderBook?.bids ?? [];

  // asks: raw is cheapest-first → take top OB_ROWS, then reverse so
  // highest price is at top and best ask sits just above the spread.
  const displayAsks = [...rawAsks.slice(0, OB_ROWS)].reverse();

  // bids: raw is highest-first — take top OB_ROWS (best bid stays at top).
  const displayBids = rawBids.slice(0, OB_ROWS);

  const bestAsk = rawAsks[0]?.[0] ?? 0;
  const bestBid = rawBids[0]?.[0] ?? 0;
  const spread  = bestAsk && bestBid ? bestAsk - bestBid : null;

  // newest trades first, show last 5
  const lastTrades = [...trades].reverse().slice(0, 5);

  const handleSubmit = async () => {
    const p = parseFloat(price);
    const a = parseFloat(amount);
    if (!p || !a || p <= 0 || a <= 0) {
      setMsg({ text: "Enter a valid price and amount", ok: false });
      return;
    }
    setLoading(true);
    setMsg(null);
    try {
      const res = await api.postLimitOrder({ exchange, pair: pairId, side, price: p, amount: a, postOnly });
      setMsg({ text: `Order placed: ${res.orderId}`, ok: true });
      setPrice("");
      setAmount("");
    } catch (err) {
      setMsg({ text: (err as Error).message, ok: false });
    } finally {
      setLoading(false);
    }
  };

  return (
    <div className="sidebar">

      {/* ── Exchange selector (always visible) ────────────────────── */}
      <div className="exchange-bar">
        <select
          className="exchange-select"
          value={exchange}
          onChange={e => onExchangeChange(e.target.value)}
        >
          {Object.entries(EXCHANGE_META).map(([key, meta]) => (
            <option key={key} value={key}>{meta.label}</option>
          ))}
        </select>

        {/* Spot: pair selector */}
        {!isP2P && (
          <SearchableSelect
            value={pair}
            options={Object.entries(pairs).map(([key, pcfg]) => ({ value: key, label: pcfg.label }))}
            onChange={onPairChange}
          />
        )}
      </div>

      {/* ── P2P: crypto / fiat / payment-method selectors ─────────── */}
      {isP2P && (
        <div className="p2p-selectors">
          <SearchableSelect
            value={p2pCrypto}
            options={p2pCryptos.map(o => ({ value: o.symbol, label: o.label }))}
            onChange={v => onP2PCryptoChange?.(v)}
            disabled={p2pCryptos.length === 0}
          />
          <SearchableSelect
            value={p2pFiat}
            options={p2pFiats.map(o => ({ value: o.symbol, label: o.label }))}
            onChange={v => onP2PFiatChange?.(v)}
            disabled={p2pFiats.length === 0}
          />
          <SearchableSelect
            value={p2pPaymentMethod}
            options={[{ value: "", label: "All payment methods" }, ...p2pPaymentMethods.map(o => ({ value: o.symbol, label: o.label }))]}
            onChange={v => onP2PPaymentMethodChange?.(v)}
            disabled={p2pPaymentMethods.length === 0}
            hasMore={p2pPaymentMethodsHasMore}
            onLoadMore={onLoadMorePaymentMethods}
          />
          <SearchableSelect
            value={p2pTradeType}
            options={[
              { value: "buy",  label: "Buy Ads" },
              { value: "sell", label: "Sell Ads" },
            ]}
            onChange={v => onP2PTradeTypeChange?.(v as "buy" | "sell")}
          />
        </div>
      )}

      {/* ── Spot-only sections ────────────────────────────────────── */}
      {!isP2P && (<>
      <div className="ob">
        <div className="ob-header">
          <span>PRICE</span>
          <span>VOLUME</span>
        </div>

        {/* Asks: displayed highest-first; flex-end pushes best ask to bottom */}
        <div className="asks-list">
          {displayAsks.map(([p, v], i) => (
            <div key={i} className="ob-row ask" onClick={() => handlePriceClick(p)} style={{ cursor: "pointer" }}>
              <span>{fmtPrice(p)}</span>
              <span>{v.toFixed(6)}</span>
            </div>
          ))}
        </div>

        <div className="ob-spread">
          {spread !== null ? `${fmtPrice(spread)} Spread` : "– Spread"}
        </div>

        {/* Bids: displayed highest-first; best bid sits just below spread */}
        <div className="bids-list">
          {displayBids.map(([p, v], i) => (
            <div key={i} className="ob-row bid" onClick={() => handlePriceClick(p)} style={{ cursor: "pointer" }}>
              <span>{fmtPrice(p)}</span>
              <span>{v.toFixed(6)}</span>
            </div>
          ))}
        </div>
      </div>

      {/* ── Last trades ───────────────────────────────────────────── */}
      <div className="last-trades">
        <div className="last-trades-header">Last Trade</div>
        {lastTrades.length === 0
          ? <div className="trade-row"><span className="t-time">—</span></div>
          : lastTrades.map((t, i) => (
              <div key={i} className="trade-row">
                <span className="t-time">{fmtTime(t.timestamp)}</span>
                <span
                  className={`t-price ${t.side}`}
                  style={{ cursor: "pointer" }}
                  onClick={() => handlePriceClick(t.price)}
                >{fmtPrice(t.price)}</span>
                <span className="t-amount">{t.amount.toFixed(6)}</span>
              </div>
            ))
        }
      </div>

      {/* ── Order entry ───────────────────────────────────────────── */}
      <div className="order-form">
        <div className="side-tabs">
          <button
            className={side === "buy"  ? "active buy"  : ""}
            onClick={() => { setSide("buy");  setMsg(null); }}
          >Buy</button>
          <button
            className={side === "sell" ? "active sell" : ""}
            onClick={() => { setSide("sell"); setMsg(null); }}
          >Sell</button>
        </div>

        <div className="form-row">
          <label>Price</label>
          <input
            type="number"
            value={price}
            placeholder="0"
            onChange={e => setPrice(e.target.value)}
          />
        </div>
        <div className="form-row">
          <label>Amount</label>
          <input
            type="number"
            value={amount}
            placeholder="0.000000"
            onChange={e => setAmount(e.target.value)}
          />
        </div>
        <div className="form-row post-only-row">
          <label>
            <input
              type="checkbox"
              checked={postOnly}
              onChange={e => setPostOnly(e.target.checked)}
            />
            Post Only
          </label>
        </div>
        <div className="form-row fee-row">
          <label>Fee</label>
          <span className="fee-value">
            {feeInfo
              ? `${fmtFee(feeInfo.maker)} / ${fmtFee(feeInfo.taker)} maker/taker`
              : '—'}
          </span>
        </div>
        {(() => {
          const minBase = pairs[pair]?.minBase ?? 0;
          if (!minBase || !baseAsset) return null;
          // Format without scientific notation, trimming trailing zeros
          const abs = minBase;
          const decimals = abs < 1 && abs > 0
            ? Math.min(Math.max(4, Math.ceil(-Math.log10(abs)) + 2), 10)
            : 4;
          const fmtMin = parseFloat(minBase.toFixed(decimals)).toString();
          return (
            <div className="form-row fee-row">
              <label>Min</label>
              <span className="fee-value">{fmtMin} {baseAsset}</span>
            </div>
          );
        })()}

        {msg && (
          <div className={`order-msg ${msg.ok ? "ok" : "err"}`}>{msg.text}</div>
        )}

        <button
          className={`submit-btn ${side}`}
          onClick={handleSubmit}
          disabled={loading}
        >
          {side === "buy" ? "Bid" : "Ask"}
        </button>
      </div>

      </>)}

    </div>
  );
}
