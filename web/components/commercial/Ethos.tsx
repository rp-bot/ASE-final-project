import { commercialFeatures } from "@/content/features";

export function Ethos() {
  return (
    <section id="features" className="relative px-6 py-28 md:py-36">
      <div className="mx-auto max-w-7xl">
        <div className="grid gap-12 md:grid-cols-12">
          <div className="md:col-span-4">
            <div
              className="font-mono-accent text-[11px] uppercase tracking-[0.3em]"
              style={{ color: "var(--color-ink-soft)" }}
            >
              On the craft
            </div>
            <h2
              className="mt-6 font-display"
              style={{
                fontSize: "clamp(2.4rem, 4.8vw, 4rem)",
                lineHeight: 1.02,
                fontWeight: 400,
                letterSpacing: "-0.02em",
              }}
            >
              A different kind of{" "}
              <em style={{ color: "var(--color-accent)", fontStyle: "italic" }}>
                instrument.
              </em>
            </h2>
            <p
              className="mt-6 max-w-sm text-pretty text-[15px] leading-relaxed"
              style={{ color: "var(--color-ink-soft)" }}
            >
              The XY pad has served faithfully for forty years. We've built the thing
              that replaces it — a volumetric interface where timbre is a place, not a
              value.
            </p>
          </div>

          <div className="md:col-span-8">
            <ol className="divide-y" style={{ borderColor: "var(--color-hairline)" }}>
              {commercialFeatures.map((f, i) => (
                <li
                  key={f.title}
                  className="grid grid-cols-12 gap-6 py-8 first:pt-0 last:pb-0"
                  style={{ borderColor: "var(--color-hairline)" }}
                >
                  <div className="col-span-2 md:col-span-1">
                    <span
                      className="font-mono-accent text-sm"
                      style={{ color: "var(--color-accent)" }}
                    >
                      {String(i + 1).padStart(2, "0")}
                    </span>
                  </div>
                  <div className="col-span-10 md:col-span-11">
                    <h3
                      className="font-display text-2xl md:text-3xl"
                      style={{
                        fontWeight: 400,
                        lineHeight: 1.1,
                        letterSpacing: "-0.01em",
                      }}
                    >
                      {f.title}
                    </h3>
                    <p
                      className="mt-3 max-w-2xl text-[15px] leading-relaxed"
                      style={{ color: "var(--color-ink-soft)" }}
                    >
                      {f.body}
                    </p>
                  </div>
                </li>
              ))}
            </ol>
          </div>
        </div>
      </div>
    </section>
  );
}
