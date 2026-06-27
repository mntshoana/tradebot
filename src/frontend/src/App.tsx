import { useState } from "react";
import "./App.css";
import { BottomPanel } from "./components/BottomPanel";
import { Sidebar } from "./components/Sidebar";
import { TradingViewChart } from "./components/TradingViewChart";
import { DEFAULT_EXCHANGE, EXCHANGES } from "./exchanges";
import { useWebSocket } from "./hooks/useWebSocket";

function App() {
  const [exchange, setExchange] = useState(DEFAULT_EXCHANGE);
  const [pair,     setPair]     = useState(EXCHANGES[DEFAULT_EXCHANGE].defaultPair);

  const { lunoOrderBook, valrOrderBook, lunoTrades, valrTrades, connected } =
    useWebSocket();

  const exCfg   = EXCHANGES[exchange];
  const pairCfg = exCfg.pairs[pair] ?? Object.values(exCfg.pairs)[0];

  const orderBook = exchange === "luno" ? lunoOrderBook : valrOrderBook;
  const trades    = exchange === "luno" ? lunoTrades    : valrTrades;

  const handleExchangeChange = (ex: string) => {
    setExchange(ex);
    setPair(EXCHANGES[ex].defaultPair);
  };

  return (
    <div className="app">
      <div className="main-area">
        <div className="chart-pane">
          <TradingViewChart chartUrl={pairCfg.chartUrl} tvSymbol={pairCfg.tvSymbol} />
        </div>
        <Sidebar
          exchange={exchange}
          pair={pair}
          pairId={pairCfg.pairId}
          orderBook={orderBook}
          trades={trades}
          onExchangeChange={handleExchangeChange}
          onPairChange={setPair}
        />
      </div>
      <BottomPanel connected={connected} exchange={exchange} />
    </div>
  );
}

export default App;
