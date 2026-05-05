"use client";

import { ParallaxCard } from "../ParallaxCard";
import { PluginWindow } from "../PluginWindow";

const annotations = [
  {
    label: "01",
    title: "Blob visualizer",
    body: "Raymarched SDF. Fragment shader at /shaders/blob.glsl.",
  },
  {
    label: "02",
    title: "Oscilloscope nodes",
    body: "Instanced draw, 8 verts, audio-reactive scale + brightness.",
  },
  {
    label: "03",
    title: "Trajectory ribbons",
    body: "Trajectory mode state is wired end-to-end; path interpolation is planned next.",
  },
  {
    label: "04",
    title: "Single GL context",
    body: "Attached to top-level editor, hybrid 2D/3D, no Z fights.",
  },
];

export function Showcase() {
  return (
    <section
      id="showcase"
      className="relative px-6 py-24 md:py-32"
      style={{ background: "var(--color-bg-3)" }}
    >
      <div className="mx-auto max-w-7xl">
        <div className="mb-12 flex flex-wrap items-end justify-between gap-6">
          <div>
            <div
              className="font-mono-accent text-[11px] uppercase tracking-[0.28em]"
              style={{ color: "var(--color-ink-soft)" }}
            >
              /docs/screenshots
            </div>
            <h2
              className="mt-4"
              style={{
                fontFamily: "var(--font-space-grotesk)",
                fontSize: "clamp(2rem, 4.4vw, 3.6rem)",
                lineHeight: 1.02,
                fontWeight: 600,
                letterSpacing: "-0.03em",
              }}
            >
              The <span className="sketch-underline">interface</span>, annotated.
            </h2>
          </div>
        </div>

        <div className="grid gap-8 lg:grid-cols-[1.4fr_1fr]">
          <ParallaxCard maxTiltDeg={4}>
            <PluginWindow variant="oss" />
          </ParallaxCard>

          <ul className="grid content-start gap-3">
            {annotations.map((a) => (
              <li
                key={a.label}
                className="grid grid-cols-[auto_1fr] items-start gap-4 border bg-[var(--color-bg-2)] p-4"
                style={{ borderColor: "var(--color-hairline)", borderRadius: "2px" }}
              >
                <span
                  className="font-mono-accent text-[11px]"
                  style={{
                    background: "var(--color-ink)",
                    color: "var(--color-bg-2)",
                    padding: "4px 8px",
                    borderRadius: "2px",
                  }}
                >
                  {a.label}
                </span>
                <div>
                  <div
                    style={{
                      fontFamily: "var(--font-space-grotesk)",
                      fontSize: "0.95rem",
                      fontWeight: 600,
                    }}
                  >
                    {a.title}
                  </div>
                  <div
                    className="mt-1 font-mono-accent text-xs leading-relaxed"
                    style={{ color: "var(--color-ink-soft)" }}
                  >
                    {a.body}
                  </div>
                </div>
              </li>
            ))}
          </ul>
        </div>
      </div>
    </section>
  );
}
