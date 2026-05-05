"use client";

import { ParallaxCard } from "../ParallaxCard";
import { HeroPluginScreenshot } from "../HeroPluginScreenshot";
import { ossCopy } from "@/content/opensource";
import { FaGithub } from "react-icons/fa";

export function Hero() {
  return (
    <section className="relative overflow-hidden px-6 pt-32 pb-20 md:pt-40 md:pb-24">
      <div className="absolute inset-0 graph-paper -z-10" aria-hidden />

      <div className="mx-auto max-w-7xl">
        {/* Terminal header */}
        <div
          className="mb-10 inline-flex items-center gap-3 rounded-sm border px-3 py-1.5 font-mono-accent text-[11px]"
          style={{
            borderColor: "var(--color-hairline)",
            background: "var(--color-bg-2)",
            color: "var(--color-ink-soft)",
          }}
        >
          <span style={{ color: "var(--color-accent-2)" }}>$</span>
          <span>git clone https://github.com/rp-bot/ASE-final-project.git</span>
          <span
            className="ml-1 inline-block h-3 w-2 animate-pulse"
            style={{ background: "var(--color-ink)" }}
          />
        </div>

        <div className="grid gap-14 lg:grid-cols-[1.15fr_1fr] lg:items-center">
          <div>
            <h1
              className="text-balance"
              style={{
                fontFamily: "var(--font-space-grotesk), sans-serif",
                fontSize: "clamp(3rem, 7.5vw, 6.4rem)",
                lineHeight: 0.98,
                fontWeight: 600,
                letterSpacing: "-0.04em",
              }}
            >
              Move through sound,
              <br />
              <span className="sketch-underline" style={{ color: "var(--color-accent)" }}>
                not menus.
              </span>{" "}
              
            </h1>

           

            <div className="mt-10 flex flex-wrap items-center gap-4">
              <a
                href={ossCopy.heroPrimary.href}
                className="inline-flex items-center gap-2 px-5 py-3 font-mono-accent text-sm transition-transform hover:-translate-y-px"
                style={{
                  background: "var(--color-ink)",
                  color: "var(--color-bg-2)",
                  borderRadius: "2px",
                }}
              >
                <span>↓</span> {ossCopy.heroPrimary.label}
              </a>
              <a
                href={ossCopy.repoUrl}
                className="inline-flex items-center gap-2 border px-5 py-3 font-mono-accent text-sm transition-colors hover:bg-ink"
                style={{
                  borderColor: "var(--color-ink)",
                  color: "var(--color-ink)",
                  borderRadius: "2px",
                }}
                onMouseEnter={e => { e.currentTarget.style.color = "#fff"; }}
                onMouseLeave={e => { e.currentTarget.style.color = "var(--color-ink)"; }}
              >
         
                <FaGithub aria-hidden size={14} /> {ossCopy.heroSecondary.label}
              </a>
              <div
                className="flex items-center gap-3 font-mono-accent text-xs"
                style={{ color: "var(--color-ink-soft)" }}
              >
                <span
                  className="inline-flex h-2 w-2 rounded-full"
                  style={{ background: "var(--color-accent-2)" }}
                />
                build passing · v1.0.0
              </div>
            </div>

            {/* <div
              className="mt-10 grid max-w-md grid-cols-3 gap-3 font-mono-accent text-xs"
              style={{ color: "var(--color-ink-soft)" }}
            >
              {[
                ["★", "1.2k", "stars"],
                ["⑂", "47", "forks"],
                ["◷", "3d", "last commit"],
              ].map(([icon, val, label]) => (
                <div
                  key={label}
                  className="border bg-[var(--color-bg-2)] p-3"
                  style={{ borderColor: "var(--color-hairline)" }}
                >
                  <div
                    className="text-lg"
                    style={{
                      color: "var(--color-accent-2)",
                      fontFamily: "var(--font-space-grotesk)",
                      fontWeight: 600,
                    }}
                  >
                    {icon} {val}
                  </div>
                  <div className="mt-1 uppercase tracking-[0.18em] text-[10px]">{label}</div>
                </div>
              ))}
            </div> */}
          </div>

          <ParallaxCard className="relative">
            <div className="relative">
              <HeroPluginScreenshot variant="oss" />
              {/* hand-drawn callout */}
              {/* <div
                className="absolute -left-6 top-8 hidden font-mono-accent text-[11px] md:block"
                style={{ color: "var(--color-accent-3)" }}
              >
                <svg width="80" height="40" viewBox="0 0 80 40" className="absolute left-16 top-3" aria-hidden>
                  <path
                    d="M2 20 Q 30 10, 70 18"
                    stroke="currentColor"
                    strokeWidth="1.4"
                    fill="none"
                    strokeDasharray="3 3"
                  />
                  <path d="M68 14 L76 18 L68 22" stroke="currentColor" strokeWidth="1.4" fill="none" />
                </svg>
                &lt;-- drag the cursor
              </div> */}
              {/* <div
                className="absolute -right-4 bottom-10 hidden max-w-[180px] rotate-[3deg] font-mono-accent text-[11px] leading-tight md:block"
                style={{ color: "var(--color-accent)" }}
              >
                8 wavetables at the<br />corners. trilinear<br />interpolation between.
              </div> */}
            </div>
          </ParallaxCard>
        </div>
      </div>
    </section>
  );
}
