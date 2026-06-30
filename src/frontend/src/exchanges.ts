export interface PairCfg {
  label:        string;   // shown in the pair selector, e.g. "USDT/ZAR"
  pairId:       string;   // REST / WS identifier, e.g. "USDTZAR"
  minBase:      number;   // minimum base-asset order quantity (0 = unknown)
  chartUrl?:    string;
  tvSymbol?:    string;
  externalUrl?: string;
}

// Exchange-level metadata — pairs are loaded dynamically from /market/pairs.
export interface ExchangeMeta {
  label:        string;
  isP2P?:       boolean;   // true for P2P exchanges; skips pair loading + spot UI
  p2pDefaults?: { crypto: string; fiat: string; paymentMethod: string; tradeType: "buy" | "sell" };
  buildPairCfg: (pairId: string, label: string, minBase: number) => PairCfg;
}

const LUNO_BASE = "https://d32exi8v9av3ux.cloudfront.net/static/scripts/tradingview.prod.html";
const lunoChart = (symbol: string) =>
  `${LUNO_BASE}?symbol=${symbol}&res=60&lang=en&hidesidetoolbar=1`;

export const EXCHANGE_META: Record<string, ExchangeMeta> = {
  luno: {
    label:        "Luno",
    buildPairCfg: (pairId, label, minBase) => ({ pairId, label, minBase, chartUrl: lunoChart(pairId) }),
  },
  valr: {
    label:        "VALR",
    buildPairCfg: (pairId, label, minBase) => ({
      pairId, label, minBase,
      externalUrl: `https://www.valr.com/en/exchange/${label}`,
    }),
  },
  bybit: {
    label:        "Bybit",
    buildPairCfg: (pairId, label, minBase) => ({
      pairId, label, minBase,
      externalUrl: `https://www.bybit.com/en/trade/spot/${label}`,
    }),
  },
  "bybit-p2p": {
    label:        "Bybit P2P",
    isP2P:        true,
    p2pDefaults:  { crypto: "USDT", fiat: "ZAR", paymentMethod: "", tradeType: "buy" },
    buildPairCfg: (pairId, label, minBase) => ({ pairId, label, minBase }),
  },
  binance: {
    label:        "Binance",
    buildPairCfg: (pairId, label, minBase) => ({
      pairId, label, minBase,
      externalUrl: `https://www.binance.com/en/trade/${label.replace("/", "_")}`,
    }),
  },
  "binance-p2p": {
    label:        "Binance P2P",
    isP2P:        true,
    p2pDefaults:  { crypto: "USDT", fiat: "ZAR", paymentMethod: "", tradeType: "buy" },
    buildPairCfg: (pairId, label, minBase) => ({ pairId, label, minBase }),
  },
  "localcoinswap-p2p": {
    label:        "LocalCoinSwap P2P",
    isP2P:        true,
    p2pDefaults:  { crypto: "USDT-TRC20", fiat: "ZAR", paymentMethod: "", tradeType: "buy" },
    buildPairCfg: (pairId, label, minBase) => ({ pairId, label, minBase }),
  },
};

export const DEFAULT_EXCHANGE = "luno";

// Preferred pair shown immediately while the pair list loads from the server.
export const DEFAULT_PAIR: Record<string, string> = {
  luno:    "USDTZAR",
  valr:    "USDTZAR",
  bybit:   "USDTUSD",
  binance: "BTCUSDT",
};

