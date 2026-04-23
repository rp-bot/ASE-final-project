"use client";

import { ParallaxCard } from "../ParallaxCard";
import { PluginWindow } from "../PluginWindow";

export function Showcase() {
  return (
    <section
      id="showcase"
      className="relative px-6 py-28 md:py-36"
      style={{ background: "var(--color-bg-3)" }}
    >
      <div className="mx-auto max-w-7xl">
        <div className="flex items-end justify-between gap-8">
          <div>
            <div
              className="font-mono-accent text-[11px] uppercase tracking-[0.3em]"
              style={{ color: "var(--color-ink-soft)" }}
            >
              The instrument
            </div>
            <h2
              className="mt-5 font-display max-w-2xl"
              style={{
                fontSize: "clamp(2.4rem, 5vw, 4.4rem)",
                lineHeight: 1,
                fontWeight: 400,
                letterSpacing: "-0.02em",
              }}
            >
              A glass cube.
              <br />
              <em style={{ color: "var(--color-accent)", fontStyle: "italic" }}>
                Rendered in real time.
              </em>
            </h2>
          </div>
          <div
            className="hidden max-w-xs text-right font-display text-xl italic md:block"
            style={{ color: "var(--color-ink-soft)", lineHeight: 1.4 }}
          >
            "The interface is not a feature — it's the instrument."
          </div>
        </div>

        <div className="mt-14 grid gap-10 lg:grid-cols-[1.4fr_1fr]">
          <ParallaxCard maxTiltDeg={4}>
            {/* Replace with <video src="/demo.mp4" .../> once ready */}
            <PluginWindow variant="commercial" />
            <div
              className="mt-3 font-mono-accent text-[10px] uppercase tracking-[0.22em]"
              style={{ color: "var(--color-ink-soft)" }}
            >
              Fig. 02 — Audio-reactive visual feedback
            </div>
          </ParallaxCard>

          <div className="grid gap-5">
            {[
              {
                label: "Blob visualizer",
                body: "The center form reacts to loudness and tone color in real time. Loud passages push it outward, softer moments pull it inward.",
              },
              {
                label: "Oscilloscope nodes",
                body: "Eight corner scopes brighten and scale as the cursor approaches, so you can see where the sound is coming from at a glance.",
              },
              {
                label: "Trajectory ribbons",
                body: "Glowing ribbons trace your motion paths, with a moving bead showing the current position so performance movement is easy to follow.",
              },
            ].map((item) => (
              <div
                key={item.label}
                className="border-l pl-6"
                style={{ borderColor: "var(--color-accent)" }}
              >
                <div
                  className="font-mono-accent text-[10px] uppercase tracking-[0.22em]"
                  style={{ color: "var(--color-accent)" }}
                >
                  {item.label}
                </div>
                <div
                  className="mt-2 font-display text-lg"
                  style={{ color: "var(--color-ink)", lineHeight: 1.35 }}
                >
                  {item.body}
                </div>
              </div>
            ))}
          </div>
        </div>
      </div>
    </section>
  );
}
