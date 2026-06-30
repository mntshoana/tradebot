// Inline HTML page that loads TradingView's Advanced Chart Widget.
// Used for VALR (and any exchange whose site blocks iframe embedding).
// The widget appends a 32px "Powered by TradingView" bar — we push the
// container 32px taller and clip it with overflow:hidden on the body.
function tvSrcDoc(symbol: string): string {
  const config = JSON.stringify({
    autosize:           true,
    symbol:             symbol,
    interval:           "60",
    timezone:           "Africa/Johannesburg",
    theme:              "dark",
    style:              "1",
    locale:             "en",
    hide_side_toolbar:  true,
    allow_symbol_change: false,
    save_image:         false,
    calendar:           false,
    support_host:       "https://www.tradingview.com",
  });
  return `<!DOCTYPE html>
<html>
<head>
  <meta charset="utf-8">
  <style>
    html, body {
      margin: 0; padding: 0; width: 100%; height: 100%;
      background: #0d0e14; overflow: hidden;
    }
    .tradingview-widget-container { width: 100%; height: calc(100% + 32px); }
    .tradingview-widget-container__widget { width: 100%; height: 100%; }
  </style>
</head>
<body>
  <div class="tradingview-widget-container">
    <div class="tradingview-widget-container__widget"></div>
    <script type="text/javascript"
      src="https://s3.tradingview.com/external-embedding/embed-widget-advanced-chart.js"
      async>${config}<\/script>
  </div>
</body>
</html>`;
}

interface Props {
  chartUrl?:    string;  // direct iframe src (Luno CloudFront)
  tvSymbol?:    string;  // TradingView symbol → rendered via srcDoc
  externalUrl?: string;  // cannot embed — open in system browser
}

export function TradingViewChart({ chartUrl, tvSymbol, externalUrl }: Props) {
  if (chartUrl) {
    // Luno's CloudFront page has ~40 px of dark padding around the chart that
    // we cannot remove from outside (cross-origin).  Extend the iframe beyond
    // the container and let overflow:hidden clip the dead space.
    // For every other URL (e.g. the local VALR proxy) use a plain full-size iframe.
    const isLuno = chartUrl.includes("cloudfront.net");

    if (isLuno) {
      return (
        <div style={{ position: "relative", width: "100%", height: "100%", overflow: "hidden" }}>
          <iframe
            key={chartUrl}
            title="chart"
            src={chartUrl}
            style={{
              display: "block",
              position: "absolute",
              top: -40,
              left: -5,
              width: "calc(100% + 10px)",
              height: "calc(100% + 45px)",
              border: "none",
            }}
            allow="fullscreen"
          />
        </div>
      );
    }

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
    <div style={{ width: "100%", height: "100%", background: "#0d0e14",
                  display: "flex", flexDirection: "column",
                  alignItems: "center", justifyContent: "center", gap: 12 }}>
      {externalUrl && (
        <>
          <span style={{ color: "#9aa6c0", fontSize: 12 }}>
            Chart cannot be embedded
          </span>
          <a
            href={externalUrl}
            target="_blank"
            rel="noreferrer"
            style={{
              background: "#ff4d4d", color: "#fff", border: "none",
              borderRadius: 4, padding: "6px 16px", fontSize: 12,
              cursor: "pointer", textDecoration: "none",
            }}
          >
            Open chart →
          </a>
        </>
      )}
    </div>
  );
}

