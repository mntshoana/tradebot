export interface PairCfg {
  label:     string;   // shown in the pair selector, e.g. "USDT/ZAR"
  pairId:    string;   // REST / WS identifier, e.g. "USDTZAR"
  chartUrl?: string;   // direct iframe URL (Luno CloudFront — no X-Frame-Options)
  tvSymbol?: string;   // TradingView symbol for widget-embed srcDoc (everything else)
}

export interface ExchangeCfg {
  label:       string;
  defaultPair: string;
  pairs:       Record<string, PairCfg>;
}

const LUNO_BASE = "https://d32exi8v9av3ux.cloudfront.net/static/scripts/tradingview.prod.html";
const lunoChart = (symbol: string) =>
  `${LUNO_BASE}?symbol=${symbol}&res=60&lang=en&hidesidetoolbar=1`;

// Add new exchanges / pairs here — they appear automatically in the selectors.
export const EXCHANGES: Record<string, ExchangeCfg> = {
  luno: {
    label:       "Luno",
    defaultPair: "USDTZAR",
    pairs: {
      USDTZAR: { label: "USDT/ZAR", pairId: "USDTZAR", chartUrl: lunoChart("USDTZAR") },
      XBTZAR:  { label: "XBT/ZAR",  pairId: "XBTZAR",  chartUrl: lunoChart("XBTZAR")  },
      ETHZAR:  { label: "ETH/ZAR",  pairId: "ETHZAR",  chartUrl: lunoChart("ETHZAR")  },
      XRPZAR:  { label: "XRP/ZAR",  pairId: "XRPZAR",  chartUrl: lunoChart("XRPZAR")  },
      LTCZAR:  { label: "LTC/ZAR",  pairId: "LTCZAR",  chartUrl: lunoChart("LTCZAR")  },
      BCHZAR:  { label: "BCH/ZAR",  pairId: "BCHZAR",  chartUrl: lunoChart("BCHZAR")  },
    },
  },
  valr: {
    label:       "VALR",
    defaultPair: "BTCZAR",
    // tvSymbol → rendered via TradingView widget srcDoc (avoids VALR iframe restrictions)
    pairs: {
      BTCZAR:  { label: "BTC/ZAR",  pairId: "BTCZAR",  tvSymbol: "VALR:BTCZAR"  },
      ETHZAR:  { label: "ETH/ZAR",  pairId: "ETHZAR",  tvSymbol: "VALR:ETHZAR"  },
      USDTZAR: { label: "USDT/ZAR", pairId: "USDTZAR", tvSymbol: "VALR:USDTZAR" },
      XRPZAR:  { label: "XRP/ZAR",  pairId: "XRPZAR",  tvSymbol: "VALR:XRPZAR"  },
      SOLZAR:  { label: "SOL/ZAR",  pairId: "SOLZAR",  tvSymbol: "VALR:SOLZAR"  },
      BNBZAR:  { label: "BNB/ZAR",  pairId: "BNBZAR",  tvSymbol: "VALR:BNBZAR"  },
    },
  },
};

export const DEFAULT_EXCHANGE = "luno";
