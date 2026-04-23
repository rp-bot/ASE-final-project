import { specs } from "@/content/features";

export function Specs() {
  return (
    <section id="specs" className="relative px-6 py-28 md:py-36">
      <div className="mx-auto max-w-6xl">
        <div className="flex items-end justify-between gap-6 border-b pb-6"
             style={{ borderColor: "var(--color-hairline)" }}>
          <div>
            <div
              className="font-mono-accent text-[11px] uppercase tracking-[0.3em]"
              style={{ color: "var(--color-ink-soft)" }}
            >
              Technical ledger
            </div>
            <h2
              className="mt-4 font-display"
              style={{
                fontSize: "clamp(2rem, 4vw, 3rem)",
                lineHeight: 1.02,
                fontWeight: 400,
                letterSpacing: "-0.01em",
              }}
            >
              Specification.
            </h2>
          </div>
          <div
            className="hidden font-mono-accent text-[10px] uppercase tracking-[0.24em] md:block"
            style={{ color: "var(--color-ink-dim)" }}
          >
            Plate III
          </div>
        </div>

        <dl className="grid grid-cols-1 md:grid-cols-2">
          {specs.map((s) => (
            <div
              key={s.label}
              className="grid grid-cols-[auto_1fr] items-baseline gap-6 border-b py-5"
              style={{ borderColor: "var(--color-hairline)" }}
            >
              <dt
                className="font-mono-accent text-[11px] uppercase tracking-[0.22em]"
                style={{ color: "var(--color-ink-soft)", minWidth: 180 }}
              >
                {s.label}
              </dt>
              <dd className="font-display text-lg" style={{ color: "var(--color-ink)" }}>
                {s.value}
              </dd>
            </div>
          ))}
        </dl>
      </div>
    </section>
  );
}
