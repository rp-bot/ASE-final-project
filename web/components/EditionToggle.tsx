"use client";

import { motion } from "framer-motion";
import { useEdition, type Edition } from "@/lib/edition";

const options: { id: Edition; label: string }[] = [
  { id: "commercial", label: "Studio" },
  { id: "oss", label: "Open Source" },
];

export function EditionToggle({ className = "" }: { className?: string }) {
  const { edition, setEdition } = useEdition();

  return (
    <div
      role="tablist"
      aria-label="Edition"
      className={`relative inline-flex items-center rounded-full p-1 font-mono-accent text-[11px] uppercase tracking-[0.18em] ${className}`}
      style={{
        background: "color-mix(in srgb, var(--color-bg-2) 70%, transparent)",
        border: "1px solid var(--color-hairline)",
        backdropFilter: "blur(10px)",
        WebkitBackdropFilter: "blur(10px)",
      }}
    >
      {options.map((opt) => {
        const active = edition === opt.id;
        return (
          <button
            key={opt.id}
            role="tab"
            aria-selected={active}
            onClick={() => setEdition(opt.id)}
            className="relative z-10 px-4 py-1.5 transition-colors"
            style={{
              color: active ? "var(--color-bg-2)" : "var(--color-ink-soft)",
            }}
          >
            {active && (
              <motion.span
                layoutId="edition-pill"
                className="absolute inset-0 rounded-full"
                style={{ background: "var(--color-ink)" }}
                transition={{ type: "spring", stiffness: 380, damping: 32 }}
              />
            )}
            <span className="relative">{opt.label}</span>
          </button>
        );
      })}
    </div>
  );
}
