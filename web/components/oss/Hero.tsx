"use client";

import { ParallaxCard } from "../ParallaxCard";
import { HeroPluginScreenshot } from "../HeroPluginScreenshot";
import { ossCopy } from "@/content/opensource";

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
              A cube of sound.
              <br />
              <span className="sketch-underline" style={{ color: "var(--color-accent)" }}>
                Free
              </span>{" "}
              and open.
            </h1>

            <p
              className="mt-8 max-w-xl text-[17px] leading-[1.55] font-mono-accent"
              style={{ color: "var(--color-ink-soft)" }}
            >
              {ossCopy.heroSubtitle}
            </p>

            <div className="mt-10 flex flex-wrap items-center gap-4">
              <a
                href={ossCopy.heroPrimary.href}
                className="inline-flex items-center gap-2 px-5 py-3 font-mono-accent text-sm transition-transform hover:translate-y-[-1px]"
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
                className="inline-flex items-center gap-2 border px-5 py-3 font-mono-accent text-sm transition-colors hover:bg-[var(--color-ink)] hover:text-[var(--color-bg-2)]"
                style={{
                  borderColor: "var(--color-ink)",
                  color: "var(--color-ink)",
                  borderRadius: "2px",
                }}
              >
                <GithubIcon /> {ossCopy.heroSecondary.label}
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

            <div
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
            </div>
          </div>

          <ParallaxCard className="relative">
            <div className="relative">
              <HeroPluginScreenshot variant="oss" />
              {/* hand-drawn callout */}
              <div
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
              </div>
              <div
                className="absolute -right-4 bottom-10 hidden max-w-[180px] rotate-[3deg] font-mono-accent text-[11px] leading-tight md:block"
                style={{ color: "var(--color-accent)" }}
              >
                8 wavetables at the<br />corners. trilinear<br />interpolation between.
              </div>
            </div>
          </ParallaxCard>
        </div>
      </div>
    </section>
  );
}

function GithubIcon() {
  return (
    <svg width="14" height="14" viewBox="0 0 24 24" fill="currentColor" aria-hidden>
      <path d="M12 .5C5.6.5.5 5.6.5 12c0 5.1 3.3 9.4 7.9 10.9.6.1.8-.3.8-.6v-2c-3.2.7-3.9-1.5-3.9-1.5-.5-1.3-1.3-1.6-1.3-1.6-1.1-.7.1-.7.1-.7 1.2.1 1.8 1.2 1.8 1.2 1 1.8 2.8 1.3 3.5 1 .1-.8.4-1.3.7-1.6-2.6-.3-5.3-1.3-5.3-5.7 0-1.3.5-2.3 1.2-3.1-.1-.3-.5-1.5.1-3.1 0 0 1-.3 3.2 1.2.9-.3 1.9-.4 2.9-.4s2 .1 2.9.4c2.2-1.5 3.2-1.2 3.2-1.2.6 1.6.2 2.8.1 3.1.8.8 1.2 1.9 1.2 3.1 0 4.4-2.7 5.4-5.3 5.7.4.4.8 1.1.8 2.2v3.2c0 .3.2.7.8.6 4.6-1.5 7.9-5.8 7.9-10.9C23.5 5.6 18.4.5 12 .5z" />
    </svg>
  );
}
