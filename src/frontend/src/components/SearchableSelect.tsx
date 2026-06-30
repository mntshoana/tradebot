import { useState, useRef, useEffect } from "react";

interface Option {
  value: string;
  label: string;
}

interface Props {
  value:     string;
  options:   Option[];
  onChange:  (value: string) => void;
  disabled?:      boolean;
  disabledLabel?: string;
  hasMore?:    boolean;
  onLoadMore?: () => void;
}

export function SearchableSelect({ value, options, onChange, disabled, disabledLabel, hasMore, onLoadMore }: Props) {
  const [open,       setOpen]       = useState(false);
  const [query,      setQuery]      = useState("");
  const [loadingMore, setLoadingMore] = useState(false);
  const containerRef = useRef<HTMLDivElement>(null);
  const inputRef     = useRef<HTMLInputElement>(null);
  const listRef      = useRef<HTMLDivElement>(null);

  const selected = options.find(o => o.value === value);

  const filtered = query
    ? options.filter(o => o.label.toLowerCase().includes(query.toLowerCase()))
    : options;

  // Focus the input whenever the dropdown opens.
  useEffect(() => {
    if (open) {
      setQuery("");
      // Defer slightly so the input is in the DOM.
      setTimeout(() => inputRef.current?.focus(), 0);
    }
  }, [open]);

  // Infinite scroll: when the user scrolls to the bottom of the list,
  // request the next page (only when not filtering, to keep UX predictable).
  useEffect(() => {
    if (!open || !hasMore || !onLoadMore) return;
    const list = listRef.current;
    if (!list) return;
    const handleScroll = () => {
      if (loadingMore) return;
      const nearBottom = list.scrollTop + list.clientHeight >= list.scrollHeight - 40;
      if (nearBottom) {
        setLoadingMore(true);
        onLoadMore();
      }
    };
    list.addEventListener("scroll", handleScroll);
    return () => list.removeEventListener("scroll", handleScroll);
  }, [open, hasMore, onLoadMore, loadingMore]);

  // Reset loadingMore once new options arrive (list length changed).
  useEffect(() => { setLoadingMore(false); }, [options.length]);

  // Close on outside click.
  useEffect(() => {
    const handler = (e: MouseEvent) => {
      if (containerRef.current && !containerRef.current.contains(e.target as Node)) {
        setOpen(false);
      }
    };
    document.addEventListener("mousedown", handler);
    return () => document.removeEventListener("mousedown", handler);
  }, []);

  const select = (val: string) => {
    onChange(val);
    setOpen(false);
  };

  const handleKeyDown = (e: React.KeyboardEvent<HTMLInputElement>) => {
    if (e.key === "Escape")                            { setOpen(false); return; }
    if (e.key === "Enter" && filtered.length === 1)   { select(filtered[0].value); }
  };

  return (
    <div ref={containerRef} className="ss-wrap">
      {/* Trigger — looks identical to .exchange-select */}
      <button
        className={`ss-trigger exchange-select${disabled ? " ss-disabled" : ""}`}
        onClick={() => !disabled && setOpen(o => !o)}
        disabled={disabled}
        type="button"
      >
        {disabled ? (disabledLabel ?? "No API key configured") : (selected?.label ?? "Select…")}
      </button>

      {open && (
        <div className="ss-dropdown">
          <input
            ref={inputRef}
            className="ss-search"
            value={query}
            onChange={e => setQuery(e.target.value)}
            onKeyDown={handleKeyDown}
            placeholder="Search…"
          />
          <div ref={listRef} className="ss-list">
            {filtered.map((o, i) => (
              <div
                key={`${o.value}__${i}`}
                className={`ss-option${o.value === value ? " ss-option--active" : ""}`}
                onMouseDown={() => select(o.value)}
              >
                {o.label}
              </div>
            ))}
            {filtered.length === 0 && (
              <div className="ss-empty">No matches</div>
            )}
            {hasMore && !query && (
              <div className="ss-load-more">{loadingMore ? "Loading…" : "Scroll for more"}</div>
            )}
          </div>
        </div>
      )}
    </div>
  );
}
