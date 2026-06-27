import { useState } from "react";
import { api } from "../api/client";
import { EXCHANGES } from "../exchanges";
import type { OrderBook, Trade } from "../types";

const OB_ROWS = 8;

function fmtTime(ms: number): string {
  return new Date(ms).toTimeString().slice(0, 5);
}

interface Props {
  exchange:         string;
  pair:             string;   // currently selected pair key (e.g. "USDTZAR")
  pairId:           string;   // exchange-specific pair identifier for API calls
  orderBook:        OrderBook | null;
  trades:           Trade[];
  onExchangeChange: (e: string) => void;
  onPairChange:     (p: string) => void;
}

export function Sidebar({ exchange, pair, pairId, orderBook, trades, onExchangeChange, onPairChange }: Props) {
  const [side,    setSide]    = useState<"buy" | "sell">("buy");
  const [price,   setPrice]   = useState("");
  const [amount,  setAmount]  = useState("");
  const [msg,     setMsg]     = useState<{ text: string; ok: boolean } | null>(null);
  const [loading, setLoading] = useState(false);

  const rawAsks = orderBook?.asks ?? [];
  const rawBids = orderBook?.bids ?? [];

  // asks: raw is cheapest-first → take top OB_ROWS, then reverse so
  // highest price is at top and best ask sits just above the spread.
  const displayAsks = [...rawAsks.slice(0, OB_ROWS)].reverse();

  // bids: raw is highest-first — take top OB_ROWS (best bid stays at top).
  const displayBids = rawBids.slice(0, OB_ROWS);

  const bestAsk = rawAsks[0]?.[0] ?? 0;
  const bestBid = rawBids[0]?.[0] ?? 0;
  const spread  = bestAsk && bestBid ? Math.round(bestAsk - bestBid) : null;

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
      const res = await api.postLimitOrder({ exchange, pair: pairId, side, price: p, amount: a });
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

      {/* ── Exchange + pair selectors ──────────────────────────────── */}
      <div className="exchange-bar">
        <select
          className="exchange-select"
          value={exchange}
          onChange={e => onExchangeChange(e.target.value)}
        >
          {Object.entries(EXCHANGES).map(([key, cfg]) => (
            <option key={key} value={key}>{cfg.label}</option>
          ))}
        </select>
        <select
          className="exchange-select pair-select"
          value={pair}
          onChange={e => onPairChange(e.target.value)}
        >
          {Object.entries(EXCHANGES[exchange].pairs).map(([key, pcfg]) => (
            <option key={key} value={key}>{pcfg.label}</option>
          ))}
        </select>
      </div>

      {/* ── Order book ────────────────────────────────────────────── */}
      <div className="ob">
        <div className="ob-header">
          <span>PRICE</span>
          <span>VOLUME</span>
        </div>

        {/* Asks: displayed highest-first; flex-end pushes best ask to bottom */}
        <div className="asks-list">
          {displayAsks.map(([p, v], i) => (
            <div key={i} className="ob-row ask">
              <span>{p.toFixed(0)}</span>
              <span>{v.toFixed(6)}</span>
            </div>
          ))}
        </div>

        <div className="ob-spread">
          {spread !== null ? `${spread} Spread` : "– Spread"}
        </div>

        {/* Bids: displayed highest-first; best bid sits just below spread */}
        <div className="bids-list">
          {displayBids.map(([p, v], i) => (
            <div key={i} className="ob-row bid">
              <span>{p.toFixed(0)}</span>
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
                <span className={`t-price ${t.side}`}>{t.price.toFixed(0)}</span>
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

    </div>
  );
}
