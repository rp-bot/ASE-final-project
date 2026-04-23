import { ossFeatures } from "@/content/features";

export function Readme() {
  return (
    <section id="features" className="relative px-6 py-24 md:py-32">
      <div className="mx-auto max-w-6xl">
        <div
          className="overflow-hidden border bg-[var(--color-bg-2)]"
          style={{ borderColor: "var(--color-hairline)", borderRadius: "4px" }}
        >
          {/* file header bar */}
          <div
            className="flex items-center justify-between border-b px-5 py-3 font-mono-accent text-[11px]"
            style={{
              borderColor: "var(--color-hairline)",
              background: "var(--color-bg-3)",
              color: "var(--color-ink-soft)",
            }}
          >
            <div className="flex items-center gap-2">
              <span>◉</span>
              <span>README.md</span>
            </div>
            <div className="flex items-center gap-4">
              <span>raw</span>
              <span>blame</span>
              <span>history</span>
            </div>
          </div>

          <div className="px-8 py-12 md:px-14 md:py-16">
            <div
              className="font-mono-accent text-xs"
              style={{ color: "var(--color-ink-soft)" }}
            >
              # Volumetric Vector Synthesis Plugin
            </div>
            <h2
              className="mt-3"
              style={{
                fontFamily: "var(--font-space-grotesk)",
                fontSize: "clamp(2rem, 4.2vw, 3.4rem)",
                lineHeight: 1.02,
                fontWeight: 600,
                letterSpacing: "-0.03em",
              }}
            >
              ## What this thing <span style={{ color: "var(--color-accent-2)" }}>actually does</span>
            </h2>
            <p
              className="mt-6 max-w-2xl text-[15px] leading-relaxed font-mono-accent"
              style={{ color: "var(--color-ink-soft)" }}
            >
              &gt; The 2D XY pad has served its forty years. This is what replaces it —
              &gt; eight wavetables at the corners of a cube, a cursor that moves in three
              &gt; dimensions, and open DSP you can inspect and extend. Source is yours.
            </p>

            <ul className="mt-12 grid gap-5 md:grid-cols-2">
              {ossFeatures.map((f, i) => (
                <li
                  key={f.title}
                  className="flex gap-4 border p-5 transition-colors hover:border-[var(--color-accent-2)]"
                  style={{
                    borderColor: "var(--color-hairline)",
                    background: "var(--color-bg)",
                    borderRadius: "2px",
                  }}
                >
                  <span
                    className="mt-0.5 flex h-5 w-5 flex-none items-center justify-center font-mono-accent text-[11px]"
                    style={{
                      background: "var(--color-accent-2)",
                      color: "var(--color-bg-2)",
                      borderRadius: "2px",
                    }}
                  >
                    ✓
                  </span>
                  <div>
                    <h3
                      className="text-base"
                      style={{
                        fontFamily: "var(--font-space-grotesk)",
                        fontWeight: 600,
                        letterSpacing: "-0.01em",
                      }}
                    >
                      {f.title}
                    </h3>
                    <p
                      className="mt-1.5 text-sm leading-relaxed font-mono-accent"
                      style={{ color: "var(--color-ink-soft)" }}
                    >
                      {f.body}
                    </p>
                    <div
                      className="mt-2 font-mono-accent text-[10px] uppercase tracking-[0.2em]"
                      style={{ color: "var(--color-accent)" }}
                    >
                      [feature-{String(i + 1).padStart(2, "0")}]
                    </div>
                  </div>
                </li>
              ))}
            </ul>
          </div>
        </div>
      </div>
    </section>
  );
}
