import type { PriceCache } from "../hooks/usePriceCache";

/**
 * Some exchanges use different ticker symbols for the same asset.
 * Map them to VALR's naming convention since VALR is the price source.
 * VALR uses BTC; Luno (and others) use XBT.
 */
const ASSET_ALIAS: Record<string, string> = {
  XBT: "BTC", // Luno uses XBT; VALR uses BTC → price key is BTCZAR
};

function normalizeAsset(asset: string): string {
  const upper = asset.toUpperCase();
  return ASSET_ALIAS[upper] ?? upper;
}

/**
 * Converts `amount` of `asset` to its ZAR value using the price cache.
 * - Returns the amount unchanged if the asset is already ZAR.
 * - Returns null if no price is available in the cache.
 */
export function toZAR(
  asset: string,
  amount: number,
  prices: PriceCache,
): number | null {
  if (amount === 0) return 0;
  const norm = normalizeAsset(asset);
  if (norm === "ZAR") return amount;
  const price = prices[`${norm}ZAR`];
  if (price == null) return null;
  return amount * price;
}

/** Formats a ZAR amount as "R 1,234.56" */
export function fmtZAR(amount: number): string {
  return (
    "R\u00a0" +
    amount.toLocaleString("en-US", {
      minimumFractionDigits: 2,
      maximumFractionDigits: 2,
    })
  );
}
