"use client";

import { ParallaxCard } from "../ParallaxCard";
import { HeroPluginScreenshot } from "../HeroPluginScreenshot";
import { commercialCopy } from "@/content/commercial";
import { HeroBackdrop } from "./HeroBackdrop";

export function Hero() {
  return (
    <section className="relative overflow-hidden px-6 pt-32 pb-20 md:pt-44 md:pb-28">
      <HeroBackdrop />

      <div className="mx-auto max-w-7xl">
        {/* Magazine meta line */}
        {/* <div
          className="mb-10 flex items-center gap-4 font-mono-accent text-[10px] uppercase tracking-[0.3em]"
          style={{ color: "var(--color-ink-soft)" }}
        >
          <span>Issue Nº 01</span>
          <span className="h-px w-8" style={{ background: "var(--color-hairline)" }} />
          <span>Studio Edition</span>
          <span className="h-px w-8" style={{ background: "var(--color-hairline)" }} />
          <span>Spring 2026</span>
        </div> */}

        <div className="grid gap-14 lg:grid-cols-[1.1fr_1fr] lg:items-end">
          <div>
            <h1
              className="font-display text-balance"
              style={{
                fontSize: "clamp(3.2rem, 8vw, 7.5rem)",
                lineHeight: 0.95,
                fontWeight: 400,
                letterSpacing: "-0.03em",
              }}
            >
              Move through sound,
              <br />
              <em style={{ color: "var(--color-accent)", fontStyle: "italic" }}>
                not menus.
              </em>
            </h1>

            <div
              className="mt-10 grid max-w-xl gap-4 text-pretty text-[17px] leading-[1.55]"
              style={{ color: "var(--color-ink-soft)" }}
            >
              <p>{commercialCopy.heroSubtitle}</p>
            </div>

            {/* <div className="mt-12 flex flex-wrap items-center gap-5">
              <a
                href={commercialCopy.heroPrimary.href}
                className="group inline-flex items-center gap-3 px-7 py-3.5 text-sm font-medium tracking-wide transition-transform hover:translate-y-[-2px]"
                style={{
                  background: "var(--color-ink)",
                  color: "var(--color-bg-2)",
                  borderRadius: "2px",
                }}
              >
                {commercialCopy.heroPrimary.label}
                <span aria-hidden className="transition-transform group-hover:translate-x-1">
                  →
                </span>
              </a>
              <a
                href={commercialCopy.heroSecondary.href}
                className="text-sm font-medium underline decoration-1 underline-offset-[6px]"
                style={{ color: "var(--color-ink)" }}
              >
                {commercialCopy.heroSecondary.label}
              </a>
            </div> */}

            {/* <div
              className="mt-8 font-mono-accent text-[11px] uppercase tracking-[0.18em]"
              style={{ color: "var(--color-ink-dim)" }}
            >
              {commercialCopy.licenseNote}
            </div> */}
          </div>

          <ParallaxCard className="relative">
            <HeroPluginScreenshot variant="commercial" />
            {/* <div
              className="mt-4 flex items-baseline justify-between font-mono-accent text-[10px] uppercase tracking-[0.2em]"
              style={{ color: "var(--color-ink-soft)" }}
            >
              <span>Fig. 01 — The Glass Cube</span>
              <span>8 sources · smooth blend · realtime</span>
            </div> */}
          </ParallaxCard>
        </div>
      </div>

      {/* Ticker band at bottom of hero */}
      <div
        className="relative mt-24 overflow-hidden border-y py-4 font-display italic"
        style={{
          borderColor: "var(--color-hairline)",
          fontSize: "clamp(1.6rem, 3.2vw, 2.4rem)",
          lineHeight: 1,
          color: "var(--color-ink)",
        }}
      >
        <div className="ticker flex whitespace-nowrap gap-12">
          {Array.from({ length: 2 }).map((_, r) => (
            <div key={r} className="flex items-center gap-12">
              <span>Volumetric</span>
              <span style={{ color: "var(--color-accent)" }}>·</span>
              <span>Immersive</span>
              <span style={{ color: "var(--color-accent)" }}>·</span>
              <span>Eight timbres</span>
              {/* <span style={{ color: "var(--color-accent)" }}>·</span> */}
              {/* <span>One cursor</span> */}
              <span style={{ color: "var(--color-accent)" }}>·</span>
              <span>Sixteen voices</span>
              <span style={{ color: "var(--color-accent)" }}>·</span>
              <span>Smooth morphing</span>
              <span style={{ color: "var(--color-accent)" }}>·</span>
            </div>
          ))}
        </div>
      </div>
    </section>
  );
}
