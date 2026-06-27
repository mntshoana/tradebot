import { useEffect, useState } from "react";
import { api } from "../api/client";
import type { Balance, OpenOrder } from "../types";

type Tab = "Output" | "Open Orders" | "User Balances" | "Withdrawals" | "Auto Playground";

const TABS: Tab[] = ["Output", "Open Orders", "User Balances", "Withdrawals", "Auto Playground"];

interface WithdrawalRow {
  id:        number;
  status:    string;
  type:      string;
  currency:  string;
  amount:    number;
  fee:       number;
  createdAt: number;
}

function fmtDate(ms: number): string {
  return new Date(ms * 1000).toLocaleDateString();
}

export function BottomPanel({ connected, exchange }: { connected: boolean; exchange: string }) {
  const [active,  setActive]  = useState<Tab>("Output");
  const [orders,  setOrders]  = useState<OpenOrder[]    | null>(null);
  const [ordErr,  setOrdErr]  = useState<string         | null>(null);
  const [bals,    setBals]    = useState<Balance[]      | null>(null);
  const [balErr,  setBalErr]  = useState<string         | null>(null);
  const [wds,     setWds]     = useState<WithdrawalRow[]| null>(null);
  const [wdErr,   setWdErr]   = useState<string         | null>(null);

  // Clear cached data whenever the active exchange changes
  useEffect(() => {
    setActive("Output");
    setOrders(null); setOrdErr(null);
    setBals(null);   setBalErr(null);
    setWds(null);    setWdErr(null);
  }, [exchange]);

  // Fetch data when a data tab is first opened
  useEffect(() => {
    if (active === "Open Orders"   && orders === null)
      api.openOrders(exchange)
        .then(r  => setOrders(r.orders))
        .catch(e => setOrdErr((e as Error).message));

    if (active === "User Balances" && bals === null)
      api.balances(exchange)
        .then(r  => setBals(r.balances))
        .catch(e => setBalErr((e as Error).message));

    if (active === "Withdrawals"   && wds === null)
      api.withdrawals(exchange)
        .then(r  => setWds(r.withdrawals as WithdrawalRow[]))
        .catch(e => setWdErr((e as Error).message));
  }, [active]); // eslint-disable-line react-hooks/exhaustive-deps

  // Re-clicking the active tab refreshes it
  const handleClick = (tab: Tab) => {
    if (tab === active) {
      if (tab === "Open Orders")   { setOrders(null);  setOrdErr(null); }
      if (tab === "User Balances") { setBals(null);    setBalErr(null); }
      if (tab === "Withdrawals")   { setWds(null);     setWdErr(null);  }
    }
    setActive(tab);
  };

  return (
    <div className="bottom-panel">
      <div className="tab-bar">
        {TABS.map(t => (
          <button
            key={t}
            className={`tab-btn${active === t ? " active" : ""}`}
            onClick={() => handleClick(t)}
          >{t}</button>
        ))}
      </div>

      <div className="tab-content">

        {active === "Output" && (
          <span className={connected ? "conn" : "muted"}>
            {connected ? "● WebSocket: connected" : "○ WebSocket: reconnecting…"}
          </span>
        )}

        {active === "Open Orders" && (
          ordErr     ? <div className="err">{ordErr}</div>
          : !orders  ? <div className="muted">Loading…</div>
          : orders.length === 0 ? <div className="muted">No open orders.</div>
          : <table>
              <thead><tr>
                <th>ID</th><th>Pair</th><th>Side</th>
                <th>Price</th><th>Amount</th><th>Filled</th><th>Status</th>
              </tr></thead>
              <tbody>
                {orders.map(o => (
                  <tr key={o.id}>
                    <td title={o.id}>{o.id.slice(0, 8)}…</td>
                    <td>{o.symbol}</td>
                    <td className={o.side}>{o.side}</td>
                    <td>{o.price.toFixed(2)}</td>
                    <td>{o.amount.toFixed(6)}</td>
                    <td>{o.filled.toFixed(6)}</td>
                    <td>{o.status}</td>
                  </tr>
                ))}
              </tbody>
            </table>
        )}

        {active === "User Balances" && (
          balErr   ? <div className="err">{balErr}</div>
          : !bals  ? <div className="muted">Loading…</div>
          : bals.length === 0 ? <div className="muted">No balances found.</div>
          : <table>
              <thead><tr>
                <th>Asset</th><th>Free</th><th>Used</th><th>Total</th><th>Unconfirmed</th>
              </tr></thead>
              <tbody>
                {bals.filter(b => b.total > 0 || b.unconfirmed > 0).map(b => (
                  <tr key={b.asset + b.accountId}>
                    <td>{b.asset}</td>
                    <td>{b.free}</td>
                    <td>{b.used}</td>
                    <td>{b.total}</td>
                    <td>{b.unconfirmed > 0 ? b.unconfirmed : "—"}</td>
                  </tr>
                ))}
              </tbody>
            </table>
        )}

        {active === "Withdrawals" && (
          wdErr   ? <div className="err">{wdErr}</div>
          : !wds  ? <div className="muted">Loading…</div>
          : wds.length === 0 ? <div className="muted">No withdrawals found.</div>
          : <table>
              <thead><tr>
                <th>ID</th><th>Status</th><th>Type</th>
                <th>Currency</th><th>Amount</th><th>Fee</th><th>Date</th>
              </tr></thead>
              <tbody>
                {wds.map(w => (
                  <tr key={w.id}>
                    <td>{w.id}</td>
                    <td>{w.status}</td>
                    <td>{w.type}</td>
                    <td>{w.currency}</td>
                    <td>{w.amount}</td>
                    <td>{w.fee}</td>
                    <td>{fmtDate(w.createdAt)}</td>
                  </tr>
                ))}
              </tbody>
            </table>
        )}

        {active === "Auto Playground" && (
          <div className="muted">Auto Playground — coming soon.</div>
        )}

      </div>
    </div>
  );
}
