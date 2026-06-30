import { useState, useEffect } from "react";

const BASE = "http://localhost:8080";

/** Live ZAR price map polled from VALR, e.g. { XBTZAR: 1150000, USDTZAR: 18.5 } */
export type PriceCache = Record<string, number>;

/**
 * Polls /market/zar-prices every `intervalMs` milliseconds.
 * Returns the latest symbol→ZAR price map; starts as {} until the first fetch resolves.
 */
export function usePriceCache(intervalMs = 30_000): PriceCache {
  const [prices, setPrices] = useState<PriceCache>({});

  useEffect(() => {
    const load = () =>
      fetch(`${BASE}/market/zar-prices`)
        .then(r => r.json() as Promise<PriceCache>)
        .then(setPrices)
        .catch(() => {});
    load();
    const id = setInterval(load, intervalMs);
    return () => clearInterval(id);
  }, [intervalMs]);

  return prices;
}
