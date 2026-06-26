# Tradebot

A macOS cryptocurrency trading app. The architecture is a **Go backend** (exchange API client + WebSocket scheduler) with a **Qt/C++ frontend** that subscribes to live market data over WebSocket and handles account operations via REST.

Supported exchanges: **Luno** and **VALR**.

## Architecture

```
Go sidecar (goserver)
  ├── REST API  :8080   — account operations (balances, orders, withdrawals)
  └── WebSocket :8080/ws — pushes live orderbook + trades to Qt

Qt app (C++)
  ├── QWebSocket subscriber — renders orderbook and trade feed
  └── QTimers — polls REST for balances, open orders, theme
```

## Requirements

- **macOS** (tested on Sonoma and later)
- **Go 1.22+** — `brew install go`
- **Qt 6** with `WebEngineWidgets` and `WebSockets` modules — `brew install qt@6`
- **go-task** — `brew install go-task`
- **Xcode Command Line Tools** — `xcode-select --install`

### Credentials

Exchange API keys are read from environment variables at runtime by the Go sidecar — no credentials are compiled into the binary:

```
export LUNO_API_KEY=...
export LUNO_API_SECRET=...
export VALR_API_KEY=...
export VALR_API_SECRET=...
export PORT=8080          # optional, defaults to 8080
```

## Build

```bash
git clone git@github.com:mntshoana/tradebot.git
cd tradebot
mkdir -p build && cd build
qmake ../src/main.pro CONFIG+=sdk_no_version_check
cd ..
task build
```

`task build` compiles the Qt app and the Go sidecar in parallel, producing:
- `bin/release/tradebot.app` — Qt frontend
- `bin/goserver` — Go backend

## Run

```bash
task run
```

This starts the Go sidecar in the background (port 8080), then launches the Qt app. The Qt app connects to `ws://localhost:8080/ws` for live market data automatically.
