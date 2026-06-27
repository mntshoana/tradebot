import type { Trade } from "../types";

interface Props {
  trades: Trade[];
  title:  string;
}

const fmtPrice  = (n: number) => n.toLocaleString("en-ZA", { minimumFractionDigits: 0, maximumFractionDigits: 0 });
const fmtAmount = (n: number) => n.toLocaleString("en-ZA", { minimumFractionDigits: 6, maximumFractionDigits: 6 });
const fmtTime   = (ts: number) =>
  new Date(ts).toLocaleTimeString("en-ZA", { hour: "2-digit", minute: "2-digit", second: "2-digit" });

export function TradesFeed({ trades, title }: Props) {
  // Show most-recent 50, newest at top
  const recent = [...trades].reverse().slice(0, 50);

  return (
    <div className="panel trades-panel">
      <h3>{title}</h3>
      {recent.length === 0 ? (
        <p className="waiting">Waiting for trades…</p>
      ) : (
        <div className="scroll-area">
          <table>
            <thead>
              <tr>
                <th>Time</th>
                <th>Price</th>
                <th>Amount</th>
              </tr>
            </thead>
            <tbody>
              {recent.map((t) => (
                <tr key={t.sequence} className={t.side}>
                  <td>{fmtTime(t.timestamp)}</td>
                  <td>{fmtPrice(t.price)}</td>
                  <td>{fmtAmount(t.amount)}</td>
                </tr>
              ))}
            </tbody>
          </table>
        </div>
      )}
    </div>
  );
}
