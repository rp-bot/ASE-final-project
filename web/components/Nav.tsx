"use client";

import { useEdition } from "@/lib/edition";
import { EditionToggle } from "./EditionToggle";

export function Nav() {
  const { edition } = useEdition();

  return (
    <header className="fixed inset-x-0 top-0 z-40">
      <div
        className="pointer-events-none absolute inset-x-0 top-0 h-24"
        style={{
          background: `linear-gradient(to bottom, color-mix(in srgb, var(--color-bg) 92%, transparent) 0%, transparent 100%)`,
          backdropFilter: "blur(10px)",
          WebkitBackdropFilter: "blur(10px)",
          mask: "linear-gradient(to bottom, black 0%, black 60%, transparent 100%)",
          WebkitMask: "linear-gradient(to bottom, black 0%, black 60%, transparent 100%)",
        }}
        aria-hidden
      />
      <div className="relative mx-auto flex max-w-7xl items-center justify-between px-6 py-4">
        <a href="#" className="flex items-center gap-3">
          <Wordmark />
          <span
            className="font-mono-accent text-[10px] uppercase tracking-[0.26em]"
            style={{ color: "var(--color-ink-soft)" }}
          >
            {edition === "commercial" ? "Volumetric Vector Synthesizer" : "Volumetric Vector Synthesizer · GPLv3"}
          </span>
        </a>
        <EditionToggle />
      </div>
    </header>
  );
}

function Wordmark() {
  return (
    <svg
      width="28"
      height="28"
      viewBox="0 0 40 40"
      aria-hidden
      className="flex-none"
    >
      <rect
        x="6"
        y="6"
        width="22"
        height="22"
        fill="none"
        stroke="var(--color-ink)"
        strokeWidth="1.6"
      />
      <rect
        x="12"
        y="12"
        width="22"
        height="22"
        fill="none"
        stroke="var(--color-accent)"
        strokeWidth="1.6"
      />
      <line x1="6" y1="6" x2="12" y2="12" stroke="var(--color-ink)" strokeWidth="1.4" />
      <line x1="28" y1="6" x2="34" y2="12" stroke="var(--color-ink)" strokeWidth="1.4" />
      <line x1="6" y1="28" x2="12" y2="34" stroke="var(--color-ink)" strokeWidth="1.4" />
      <line x1="28" y1="28" x2="34" y2="34" stroke="var(--color-ink)" strokeWidth="1.4" />
    </svg>
  );
}
