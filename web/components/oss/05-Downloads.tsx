import {
  donationTiers,
  downloadPlatforms,
  ossCopy,
} from "@/content/opensource";
import { FaApple, FaLinux, FaWindows } from "react-icons/fa6";
import type { IconType } from "react-icons";

const platformLabels: Record<string, string> = {
  kofi: "ko-fi",
  sponsors: "github sponsors",
  bmac: "buymeacoffee",
};
const platformHrefs: Record<string, string> = {
  kofi: "https://ko-fi.com/",
  sponsors: "https://github.com/sponsors/",
  bmac: "https://buymeacoffee.com/",
};

const osIcons: Record<string, IconType> = {
  Windows: FaWindows,
  macOS: FaApple,
  Linux: FaLinux,
};

export function Downloads() {
  return (
    <section
      id="pricing"
      className="relative px-6 py-24 md:py-32"
      style={{
        background: "var(--color-ink)",
        color: "var(--color-bg-2)",
      }}
    >
      <div className="mx-auto max-w-6xl">
        <div
          className="font-mono-accent text-[11px] uppercase tracking-[0.28em]"
          style={{ color: "rgba(255,255,255,0.55)" }}
        >
          $ ./install.sh
        </div>
        <h2
          className="mt-4 max-w-3xl"
          style={{
            fontFamily: "var(--font-space-grotesk)",
            fontSize: "clamp(2.2rem, 5vw, 4rem)",
            lineHeight: 1,
            fontWeight: 600,
            letterSpacing: "-0.03em",
          }}
        >
          Download. Free.<br />
          <span style={{ color: "var(--color-accent-2)" }}>GPLv3.</span>{" "}

        </h2>

        <div className="mt-14 grid gap-3 md:grid-cols-3">
          {downloadPlatforms.map((p) => (
            <a
              key={p.os}
              href={p.href}
              className="group relative border p-6 transition-colors"
              style={{
                borderColor: "rgba(255,255,255,0.12)",
                borderRadius: "3px",
                background: "rgba(255,255,255,0.03)",
              }}
            >
              <div className="flex items-center gap-2">
                {(() => {
                  const OsIcon = osIcons[p.os];
                  return OsIcon ? (
                    <OsIcon className="text-base" style={{ color: "var(--color-accent-2)" }} />
                  ) : null;
                })()}
                <div
                  className="font-mono-accent text-[11px] uppercase tracking-[0.24em]"
                  style={{ color: "var(--color-accent-2)" }}
                >
                  {p.os}
                </div>
              </div>
              <div
                className="mt-4 flex items-baseline gap-2"
                style={{ fontFamily: "var(--font-space-grotesk)", fontWeight: 600 }}
              >
                <span className="text-2xl">Download</span>
                <span style={{ color: "rgba(255,255,255,0.4)" }}>↓</span>
              </div>
              <div className="mt-4 flex flex-wrap gap-2">
                {p.formats.map((f) => (
                  <span
                    key={f}
                    className="font-mono-accent text-[10px] uppercase tracking-[0.16em]"
                    style={{
                      border: "1px solid rgba(255,255,255,0.18)",
                      padding: "3px 8px",
                      borderRadius: "2px",
                      color: "rgba(255,255,255,0.78)",
                    }}
                  >
                    {f}
                  </span>
                ))}
              </div>
            </a>
          ))}
        </div>

        {/* or install via CLI */}
        {/* <div
          className="mt-6 overflow-x-auto border p-5 font-mono-accent text-[13px]"
          style={{
            borderColor: "rgba(255,255,255,0.12)",
            borderRadius: "3px",
            background: "rgba(255,255,255,0.03)",
            color: "rgba(255,255,255,0.78)",
          }}
        >
          <span style={{ color: "var(--color-accent-2)" }}>$</span> brew install --cask volumetric-vector-synth
          <span style={{ color: "rgba(255,255,255,0.4)" }}> # macOS</span>
          <br />
          <span style={{ color: "var(--color-accent-2)" }}>$</span> curl -L get.vvs.audio | sh
          <span style={{ color: "rgba(255,255,255,0.4)" }}> # or this</span>
        </div> */}
{/* 
        <div
          className="mt-24 mb-10 flex items-end justify-between gap-6 border-t pt-10"
          style={{ borderColor: "rgba(255,255,255,0.12)" }}
        >
          <div>
            <div
              className="font-mono-accent text-[11px] uppercase tracking-[0.28em]"
              style={{ color: "rgba(255,255,255,0.55)" }}
            >
              # fund-the-cube
            </div>
            <h3
              className="mt-3"
              style={{
                fontFamily: "var(--font-space-grotesk)",
                fontSize: "clamp(1.8rem, 3.6vw, 2.6rem)",
                fontWeight: 600,
                letterSpacing: "-0.02em",
                lineHeight: 1.05,
              }}
            >
              Free to use. <span style={{ color: "var(--color-accent-3)" }}>Expensive to build.</span>
            </h3>
          </div>
          <div
            className="hidden max-w-sm font-mono-accent text-[13px] md:block"
            style={{ color: "rgba(255,255,255,0.6)" }}
          >
            Donations buy time. Time ships releases. No tiers, no paywalls, no feature
            gating — tip what you can, when you can.
          </div>
        </div> */}

        {/* <div className="grid gap-3 md:grid-cols-3">
          {donationTiers.map((tier) => (
            <article
              key={tier.name}
              className="relative border p-6"
              style={{
                borderColor: "rgba(255,255,255,0.12)",
                borderRadius: "3px",
                background: "rgba(255,255,255,0.03)",
              }}
            >
              <div className="flex items-baseline justify-between">
                <div
                  style={{
                    fontFamily: "var(--font-space-grotesk)",
                    fontWeight: 600,
                    fontSize: "1.2rem",
                  }}
                >
                  {tier.name}
                </div>
                <div
                  className="font-mono-accent text-lg"
                  style={{ color: "var(--color-accent-3)" }}
                >
                  {tier.amount}
                </div>
              </div>
              <div
                className="mt-2 font-mono-accent text-[12px]"
                style={{ color: "rgba(255,255,255,0.65)" }}
              >
                {tier.tagline}
              </div>
              <ul className="mt-5 space-y-1.5 font-mono-accent text-[12px]">
                {tier.perks.map((p) => (
                  <li
                    key={p}
                    className="flex items-start gap-2"
                    style={{ color: "rgba(255,255,255,0.8)" }}
                  >
                    <span style={{ color: "var(--color-accent-2)" }}>+</span> {p}
                  </li>
                ))}
              </ul>
              <a
                href={platformHrefs[tier.platform]}
                target="_blank"
                rel="noreferrer"
                className="mt-6 inline-flex w-full items-center justify-center border px-4 py-2 font-mono-accent text-[12px] uppercase tracking-[0.18em] transition-colors hover:bg-[var(--color-accent-2)] hover:text-[var(--color-ink)]"
                style={{
                  borderColor: "rgba(255,255,255,0.25)",
                  color: "var(--color-bg-2)",
                  borderRadius: "2px",
                }}
              >
                → {platformLabels[tier.platform]}
              </a>
            </article>
          ))}
        </div> */}

        <a
          href={ossCopy.repoUrl}
          className="mt-6 flex flex-wrap items-center justify-between gap-4 border p-5 font-mono-accent text-sm transition-colors hover:border-accent-2"
          style={{
            borderColor: "rgba(255,255,255,0.12)",
            borderRadius: "3px",
          }}
        >
          <div>
            <div style={{ color: "var(--color-accent-2)" }}>★ star the repo</div>
            <div className="mt-1" style={{ color: "rgba(255,255,255,0.55)" }}>
              costs nothing, helps more than you'd think
            </div>
          </div>
          <div style={{ color: "var(--color-bg-2)" }}>github.com/rp-bot/ASE-final-project →</div>
        </a>
      </div>
    </section>
  );
}
