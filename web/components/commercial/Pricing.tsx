import { tiers } from "@/content/commercial";

export function Pricing() {
  return (
    <section
      id="pricing"
      className="relative px-6 py-28 md:py-36"
      style={{ background: "var(--color-ink)", color: "var(--color-bg)" }}
    >
      <div className="mx-auto max-w-6xl">
        <div className="grid items-end gap-8 md:grid-cols-[1fr_auto] md:gap-16">
          <div>
            <div
              className="font-mono-accent text-[11px] uppercase tracking-[0.3em]"
              style={{ color: "color-mix(in srgb, var(--color-bg) 60%, transparent)" }}
            >
              Acquisition
            </div>
            <h2
              className="mt-5 font-display text-balance"
              style={{
                fontSize: "clamp(2.6rem, 5.6vw, 4.8rem)",
                lineHeight: 1,
                fontWeight: 400,
                letterSpacing: "-0.02em",
              }}
            >
              A license.
              <br />
              <em style={{ color: "var(--color-accent-3)", fontStyle: "italic" }}>
                Not a subscription.
              </em>
            </h2>
          </div>
          <p
            className="max-w-sm text-[15px] leading-relaxed"
            style={{ color: "color-mix(in srgb, var(--color-bg) 75%, transparent)" }}
          >
            Purchase once, own forever. Minor updates included for life. Major updates
            included on Studio.
          </p>
        </div>

        <div className="mt-16 grid gap-0 border md:grid-cols-2"
             style={{ borderColor: "color-mix(in srgb, var(--color-bg) 15%, transparent)" }}>
          {tiers.map((tier, i) => (
            <article
              key={tier.name}
              className="relative p-10 md:p-12"
              style={{
                borderLeft: i === 1 ? "1px solid color-mix(in srgb, var(--color-bg) 15%, transparent)" : undefined,
                background: tier.featured
                  ? "color-mix(in srgb, var(--color-accent) 14%, transparent)"
                  : undefined,
              }}
            >
              {tier.featured && (
                <div
                  className="absolute right-6 top-6 font-mono-accent text-[10px] uppercase tracking-[0.2em]"
                  style={{ color: "var(--color-accent-3)" }}
                >
                  ● Most chosen
                </div>
              )}
              <div
                className="font-mono-accent text-[11px] uppercase tracking-[0.22em]"
                style={{ color: "color-mix(in srgb, var(--color-bg) 55%, transparent)" }}
              >
                {tier.name}
              </div>
              <div className="mt-5 flex items-baseline gap-2">
                <span
                  className="font-display"
                  style={{ fontSize: "4.5rem", lineHeight: 1, fontWeight: 400 }}
                >
                  ${tier.price}
                </span>
                <span
                  className="font-mono-accent text-xs uppercase tracking-[0.2em]"
                  style={{ color: "color-mix(in srgb, var(--color-bg) 55%, transparent)" }}
                >
                  once
                </span>
              </div>
              <p
                className="mt-4 font-display text-xl italic"
                style={{ color: "color-mix(in srgb, var(--color-bg) 85%, transparent)" }}
              >
                {tier.tagline}
              </p>
              <ul
                className="mt-8 divide-y text-sm"
                style={{ borderColor: "color-mix(in srgb, var(--color-bg) 12%, transparent)" }}
              >
                {tier.includes.map((item) => (
                  <li
                    key={item}
                    className="py-3"
                    style={{
                      borderColor: "color-mix(in srgb, var(--color-bg) 12%, transparent)",
                      color: "color-mix(in srgb, var(--color-bg) 85%, transparent)",
                    }}
                  >
                    {item}
                  </li>
                ))}
              </ul>
              <a
                href="#checkout"
                className="mt-10 inline-block px-7 py-3.5 text-sm font-medium tracking-wide transition-transform hover:translate-y-[-2px]"
                style={{
                  background: tier.featured ? "var(--color-accent)" : "var(--color-bg)",
                  color: tier.featured ? "var(--color-bg)" : "var(--color-ink)",
                  borderRadius: "2px",
                }}
              >
                {tier.cta}
              </a>
            </article>
          ))}
        </div>

        <p
          className="mt-10 text-center font-mono-accent text-[10px] uppercase tracking-[0.26em]"
          style={{ color: "color-mix(in srgb, var(--color-bg) 55%, transparent)" }}
        >
          30-day refund · VAT invoiced · Personal / Studio discounts available
        </p>
      </div>
    </section>
  );
}
