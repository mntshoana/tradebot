// Inline HTML page that loads TradingView's tv.js widget library.
// Used for VALR (and any exchange whose site blocks iframe embedding).
function tvSrcDoc(symbol: string): string {
  return `<!DOCTYPE html>
<html>
<head>
  <meta charset="utf-8">
  <style>
    html, body { margin: 0; padding: 0; width: 100%; height: 100%; background: #0d0e14; overflow: hidden; }
    #tv_chart  { position: absolute; inset: 0; }
  </style>
</head>
<body>
  <div id="tv_chart"></div>
  <script src="https://s3.tradingview.com/tv.js"><\/script>
  <script>
    new TradingView.widget({
      container_id:       "tv_chart",
      autosize:           true,
      symbol:             ${JSON.stringify(symbol)},
      interval:           "60",
      theme:              "dark",
      style:              "1",
      locale:             "en",
      toolbar_bg:         "#0d0e14",
      enable_publishing:  false,
      hide_side_toolbar:  true,
      allow_symbol_change: false
    });
  <\/script>
</body>
</html>`;
}

interface Props {
  chartUrl?: string;  // direct iframe src (Luno CloudFront)
  tvSymbol?: string;  // TradingView symbol → rendered via srcDoc
}

export function TradingViewChart({ chartUrl, tvSymbol }: Props) {
  if (chartUrl) {
    return (
      <iframe
        key={chartUrl}
        title="chart"
        src={chartUrl}
        style={{ display: "block", width: "100%", height: "100%", border: "none" }}
        allow="fullscreen"
      />
    );
  }
  if (tvSymbol) {
    return (
      <iframe
        key={tvSymbol}
        title="chart"
        srcDoc={tvSrcDoc(tvSymbol)}
        style={{ display: "block", width: "100%", height: "100%", border: "none" }}
        allow="fullscreen"
      />
    );
  }
  return (
    <div style={{ width: "100%", height: "100%", background: "#0d0e14" }} />
  );
}

