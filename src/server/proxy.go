// VALR page reverse proxy.
//
// VALR sets X-Frame-Options / Content-Security-Policy headers that prevent
// their exchange page from being embedded in an iframe.  This proxy:
//  1. Forwards /valr-proxy/* → https://www.valr.com/*
//     and  /valr-proxy/_api/* → https://api.valr.com/*
//  2. Strips frame-busting response headers.
//  3. Injects a <base> tag so relative-path static assets still resolve to VALR.
//  4. Injects a JS interceptor that rewrites every fetch / XHR call whose URL
//     starts with https://www.valr.com or https://api.valr.com so it routes
//     back through this proxy — bypassing CORS entirely.
package server

import (
	"bytes"
	"io"
	"net/http"
	"net/http/httputil"
	"strings"
)

// interceptorScript is injected at the very top of every HTML <head>,
// before any of VALR's own scripts execute.  It does two things:
//
//  1. Spoofs window.top / window.parent / window.frameElement so VALR's
//     anti-iframe guard (window.top !== window.self) passes cleanly.
//
//  2. Rewrites every fetch / XHR call whose URL starts with
//     https://www.valr.com or https://api.valr.com so the request routes
//     back through our local proxy — bypassing CORS entirely.
const interceptorScript = `<script>
(function(){
  // ── 1. Defeat anti-iframe detection ──────────────────────────────────────
  try {
    var _win = window;
    Object.defineProperty(_win, 'top',         { get: function(){ return _win; }, configurable: true });
    Object.defineProperty(_win, 'parent',      { get: function(){ return _win; }, configurable: true });
    Object.defineProperty(_win, 'frameElement',{ get: function(){ return null; }, configurable: true });
  } catch(e) {}

  // ── 2. Rewrite VALR API calls through the proxy (no CORS) ────────────────
  var PROXY = window.location.origin + '/valr-proxy';
  function rw(url){
    if(typeof url !== 'string') return url;
    if(url.startsWith('https://www.valr.com'))
      return PROXY + url.slice('https://www.valr.com'.length);
    if(url.startsWith('https://api.valr.com'))
      return PROXY + '/_api' + url.slice('https://api.valr.com'.length);
    return url;
  }
  var _fetch = window.fetch;
  window.fetch = function(input, init){
    if(typeof input === 'string') input = rw(input);
    else if(input && input.url) input = new Request(rw(input.url), input);
    return _fetch.call(this, input, init);
  };
  var _open = XMLHttpRequest.prototype.open;
  XMLHttpRequest.prototype.open = function(m, url){
    arguments[1] = rw(url);
    return _open.apply(this, arguments);
  };
})();
</script>`

var valrPageProxy = &httputil.ReverseProxy{
	Director: func(req *http.Request) {
		path := strings.TrimPrefix(req.URL.Path, "/valr-proxy")

		// /valr-proxy/_api/* → https://api.valr.com/*
		if strings.HasPrefix(path, "/_api") {
			req.URL.Scheme = "https"
			req.URL.Host = "api.valr.com"
			req.Host = "api.valr.com"
			req.URL.Path = strings.TrimPrefix(path, "/_api")
		} else {
			// /valr-proxy/* → https://www.valr.com/*
			req.URL.Scheme = "https"
			req.URL.Host = "www.valr.com"
			req.Host = "www.valr.com"
			req.URL.Path = path
		}
		if req.URL.Path == "" {
			req.URL.Path = "/"
		}

		// Request plain body so we can inspect and rewrite HTML.
		req.Header.Set("Accept-Encoding", "identity")
		req.Header.Set("User-Agent",
			"Mozilla/5.0 (Windows NT 10.0; Win64; x64) "+
				"AppleWebKit/537.36 (KHTML, like Gecko) "+
				"Chrome/125.0.0.0 Safari/537.36")
		req.Header.Del("Origin")
		req.Header.Del("Referer")
	},

	ModifyResponse: func(resp *http.Response) error {
		// Strip every header that prevents iframe embedding.
		resp.Header.Del("X-Frame-Options")
		resp.Header.Del("Content-Security-Policy")
		resp.Header.Del("Content-Security-Policy-Report-Only")
		resp.Header.Del("X-Content-Type-Options")

		if !strings.Contains(resp.Header.Get("Content-Type"), "text/html") {
			return nil
		}

		body, err := io.ReadAll(resp.Body)
		_ = resp.Body.Close()
		if err != nil {
			return err
		}

		lower := bytes.ToLower(body)

		// Build the injection: dark-background style + <base> tag + JS interceptor.
		// The style fires before any VALR CSS loads, preventing a white flash and
		// ensuring the background stays dark even while the SPA hydrates.
		const darkStyle = `<style>html,body{background:#0d0e14!important;color:#f4f6fb!important}</style>`
		const baseTag = `<base href="https://www.valr.com/">`
		inject := []byte(darkStyle + baseTag + interceptorScript)

		var out []byte
		if i := bytes.Index(lower, []byte("<head>")); i != -1 {
			cut := i + len("<head>")
			out = make([]byte, 0, len(body)+len(inject))
			out = append(out, body[:cut]...)
			out = append(out, inject...)
			out = append(out, body[cut:]...)
		} else {
			out = append(inject, body...)
		}

		resp.Body = io.NopCloser(bytes.NewReader(out))
		resp.ContentLength = int64(len(out))
		resp.Header.Del("Content-Encoding")
		return nil
	},
}

func handleVALRProxy(w http.ResponseWriter, r *http.Request) {
	valrPageProxy.ServeHTTP(w, r)
}
