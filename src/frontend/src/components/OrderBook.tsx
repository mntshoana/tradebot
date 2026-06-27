import type { OrderBook } from "../types";

interface Props {
  book:  OrderBook | null;
  title: string;
}

const fmtPrice  = (n: number) => n.toLocaleString("en-ZA", { minimumFractionDigits: 0, maximumFractionDigits: 0 });
const fmtAmount = (n: number) => n.toLocaleString("en-ZA", { minimumFractionDigits: 6, maximumFractionDigits: 6 });

export function OrderBook({ book, title }: Props) {
  if (!book) {
    return (
      <div className="panel loading">
        <h3>{title}</h3>
        <p className="waiting">Waiting for data…</p>
      </div>
    );
  }

  return (
    <div className="panel">
      <h3>
        {title} <span className="symbol">{book.symbol}</span>
      </h3>
      <table>
        <thead>
          <tr>
            <th>Price (ZAR)</th>
            <th>Amount (BTC)</th>
          </tr>
        </thead>
        <tbody>
          {book.asks.slice(0, 10).reverse().map(([p, a], i) => (
            <tr key={i} className="ask">
              <td>{fmtPrice(p)}</td>
              <td>{fmtAmount(a)}</td>
            </tr>
          ))}
          <tr className="spread">
            <td colSpan={2}>─── spread ───</td>
          </tr>
          {book.bids.slice(0, 10).map(([p, a], i) => (
            <tr key={i} className="bid">
              <td>{fmtPrice(p)}</td>
              <td>{fmtAmount(a)}</td>
            </tr>
          ))}
        </tbody>
      </table>
    </div>
  );
}
