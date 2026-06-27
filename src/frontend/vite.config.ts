import react from "@vitejs/plugin-react";
import { defineConfig } from "vite";

// No Wails bindings needed — the React app talks directly to the Go server
// at http://localhost:8080 via fetch and WebSocket.
export default defineConfig({
  server: {
    host: "127.0.0.1",
    port: Number(process.env.WAILS_VITE_PORT) || 9245,
    strictPort: true,
  },
  plugins: [react()],
});
