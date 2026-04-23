"use client";

import {
  createContext,
  useCallback,
  useContext,
  useEffect,
  useMemo,
  useState,
} from "react";

export type Edition = "commercial" | "oss";

type EditionContextValue = {
  edition: Edition;
  setEdition: (next: Edition) => void;
  toggle: () => void;
};

const EditionContext = createContext<EditionContextValue | null>(null);
const STORAGE_KEY = "vvs:edition";

export function EditionProvider({ children }: { children: React.ReactNode }) {
  const [edition, setEditionState] = useState<Edition>("commercial");

  useEffect(() => {
    const params = new URLSearchParams(window.location.search);
    const fromUrl = params.get("edition");
    if (fromUrl === "oss" || fromUrl === "commercial") {
      setEditionState(fromUrl);
    } else {
      const stored = window.localStorage.getItem(STORAGE_KEY);
      if (stored === "oss" || stored === "commercial") {
        setEditionState(stored);
      }
    }

  }, []);

  // Reflect onto <html data-edition data-studio-theme> so CSS variable themes swap.
  useEffect(() => {
    document.documentElement.dataset.edition = edition;
    // Studio theme is intentionally locked to the approved version.
    document.documentElement.dataset.studioTheme = "studio-3";
  }, [edition]);

  const setEdition = useCallback((next: Edition) => {
    setEditionState(next);
    try {
      window.localStorage.setItem(STORAGE_KEY, next);
    } catch {}
    const url = new URL(window.location.href);
    url.searchParams.set("edition", next);
    window.history.replaceState({}, "", url);
  }, []);

  const toggle = useCallback(() => {
    setEdition(edition === "commercial" ? "oss" : "commercial");
  }, [edition, setEdition]);

  const value = useMemo(() => ({ edition, setEdition, toggle }), [edition, setEdition, toggle]);

  return (
    <EditionContext.Provider value={value}>{children}</EditionContext.Provider>
  );
}

export function useEdition() {
  const ctx = useContext(EditionContext);
  if (!ctx) throw new Error("useEdition must be used within EditionProvider");
  return ctx;
}
