import { useState, useEffect, useMemo } from "react";
import "./App.css";
import { BottomPanel } from "./components/BottomPanel";
import { Sidebar } from "./components/Sidebar";
import { TradingViewChart } from "./components/TradingViewChart";
import { P2PContent } from "./components/P2PContent";
import { DEFAULT_EXCHANGE, DEFAULT_PAIR, EXCHANGE_META } from "./exchanges";
import type { PairCfg } from "./exchanges";
import type { P2POption } from "./types";
import { useWebSocket } from "./hooks/useWebSocket";
import { usePriceCache } from "./hooks/usePriceCache";
import { api } from "./api/client";

function App() {
  const [exchange, setExchange] = useState(DEFAULT_EXCHANGE);
  const [pair,     setPair]     = useState(DEFAULT_PAIR[DEFAULT_EXCHANGE]);
  const [pairs,    setPairs]    = useState<Record<string, PairCfg>>({});

  // P2P dropdown state
  const [p2pCryptos,               setP2PCryptos]               = useState<P2POption[]>([]);
  const [p2pFiats,                 setP2PFiats]                 = useState<P2POption[]>([]);
  const [p2pPaymentMethods,        setP2PPaymentMethods]        = useState<P2POption[]>([]);
  const [p2pPaymentMethodsHasMore, setP2PPaymentMethodsHasMore] = useState(false);
  const [p2pPaymentMethodsOffset,  setP2PPaymentMethodsOffset]  = useState(0);
  const [p2pCrypto,                setP2PCrypto]                = useState("USDT");
  const [p2pFiat,                  setP2PFiat]                  = useState("ZAR");
  const [p2pPaymentMethod,         setP2PPaymentMethod]         = useState("");
  const [p2pTradeType,             setP2PTradeType]             = useState<"buy" | "sell">("buy");

  const isP2P = EXCHANGE_META[exchange]?.isP2P ?? false;

  // Derive the per-exchange crypto identifier for the search API.
  // LCS populates `slug` (e.g. "USDT-TRC20"); Bybit leaves it empty and uses
  // the symbol itself ("USDT") as the API identifier.
  const p2pCryptoSlug = useMemo(
    () => {
      const opt = p2pCryptos.find(o => o.symbol === p2pCrypto);
      return opt?.slug || opt?.symbol || "";
    },
    [p2pCryptos, p2pCrypto]
  );

  // Derive payment-method slug (already stored as Symbol for payment methods).
  const p2pPaymentSlug = p2pPaymentMethod;

  const { orderBook, trades, connected } = useWebSocket(exchange, isP2P ? "" : pair);
  const prices = usePriceCache();

  // Load spot pairs — skipped for P2P exchanges.
  useEffect(() => {
    setPairs({});
    if (isP2P) return;
    api.pairs(exchange).then(list => {
      const built: Record<string, PairCfg> = {};
      for (const { pairId, label, minBase } of list) {
        built[pairId] = EXCHANGE_META[exchange].buildPairCfg(pairId, label, minBase);
      }
      setPairs(built);
      setPair(prev => built[prev] ? prev : (Object.keys(built)[0] ?? ""));
    });
  }, [exchange]);

  // Load P2P dropdown options when a P2P exchange is selected.
  useEffect(() => {
    if (!isP2P) {
      setP2PCryptos([]);
      setP2PFiats([]);
      setP2PPaymentMethods([]);
      return;
    }
    const defaults = EXCHANGE_META[exchange]?.p2pDefaults;
    setP2PCryptos([]);
    setP2PFiats([]);
    setP2PPaymentMethods([]);
    setP2PPaymentMethodsHasMore(false);
    setP2PPaymentMethodsOffset(0);
    if (defaults) {
      setP2PCrypto(defaults.crypto);
      setP2PFiat(defaults.fiat);
      setP2PPaymentMethod(defaults.paymentMethod);
      setP2PTradeType(defaults.tradeType);
    }
    api.p2pCryptos(exchange).then(r => {
      setP2PCryptos(r.options);
      if (defaults && r.options.length > 0) {
        const found = r.options.find(o => o.symbol === defaults.crypto);
        if (!found) throw new Error(`[P2P] Default crypto "${defaults.crypto}" not found in ${exchange} options — update p2pDefaults in exchanges.ts`);
      }
    }).catch(err => { throw err; });
    api.p2pFiats(exchange).then(r => {
      setP2PFiats(r.options);
      if (defaults && r.options.length > 0) {
        const found = r.options.find(o => o.symbol === defaults.fiat);
        if (!found) throw new Error(`[P2P] Default fiat "${defaults.fiat}" not found in ${exchange} options — update p2pDefaults in exchanges.ts`);
      }
    }).catch(err => { throw err; });
    api.p2pPaymentMethods(exchange, 0).then(r => {
      setP2PPaymentMethods(r.options);
      setP2PPaymentMethodsHasMore(r.hasMore);
      setP2PPaymentMethodsOffset(r.nextOffset);
    }).catch(err => { throw err; });
  }, [exchange]);

  // Append the next page of payment methods — called by the SearchableSelect
  // scroll-to-bottom handler. Only wired up for P2P exchanges.
  const loadMorePaymentMethods = () => {
    if (!p2pPaymentMethodsHasMore) return;
    api.p2pPaymentMethods(exchange, p2pPaymentMethodsOffset).then(r => {
      setP2PPaymentMethods(prev => [...prev, ...r.options]);
      setP2PPaymentMethodsHasMore(r.hasMore);
      setP2PPaymentMethodsOffset(r.nextOffset);
    }).catch(err => { throw err; });
  };

  const pairCfg = pairs[pair];

  const handleExchangeChange = (ex: string) => {
    setExchange(ex);
    setPair(DEFAULT_PAIR[ex] ?? "");
  };

  return (
    <div className="app">
      <div className="main-area">
        <div className="chart-pane">
          {isP2P
            ? <P2PContent
                exchange={exchange}
                cryptoSlug={p2pCryptoSlug}
                fiat={p2pFiat}
                payment={p2pPaymentSlug}
                tradeType={p2pTradeType}
              />
            : <TradingViewChart chartUrl={pairCfg?.chartUrl} tvSymbol={pairCfg?.tvSymbol} externalUrl={pairCfg?.externalUrl} />
          }
        </div>
        <Sidebar
          exchange={exchange}
          pair={pair}
          pairId={pairCfg?.pairId ?? pair}
          pairs={pairs}
          orderBook={orderBook}
          trades={trades}
          onExchangeChange={handleExchangeChange}
          onPairChange={setPair}
          isP2P={isP2P}
          p2pCryptos={p2pCryptos}
          p2pFiats={p2pFiats}
          p2pPaymentMethods={p2pPaymentMethods}
          p2pPaymentMethodsHasMore={p2pPaymentMethodsHasMore}
          onLoadMorePaymentMethods={loadMorePaymentMethods}
          p2pCrypto={p2pCrypto}
          p2pFiat={p2pFiat}
          p2pPaymentMethod={p2pPaymentMethod}
          p2pTradeType={p2pTradeType}
          onP2PCryptoChange={setP2PCrypto}
          onP2PFiatChange={setP2PFiat}
          onP2PPaymentMethodChange={setP2PPaymentMethod}
          onP2PTradeTypeChange={setP2PTradeType}
        />
      </div>
      <BottomPanel connected={connected} exchange={exchange} prices={prices} />
    </div>
  );
}

export default App;
