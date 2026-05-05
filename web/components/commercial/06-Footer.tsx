import { commercialCopy } from "@/content/commercial";

export function Footer() {
  return (
    <footer
      className="relative px-6 pt-20 pb-10"
      style={{ background: "var(--color-bg-3)" }}
    >
      <div className="mx-auto max-w-6xl">
        {/* <div
          className="font-display text-balance"
          style={{
            fontSize: "clamp(2.4rem, 5vw, 4.2rem)",
            lineHeight: 1.02,
            fontWeight: 400,
            letterSpacing: "-0.02em",
          }}
        >
          A synthesizer is not what you use.
          <br />
          <em style={{ color: "var(--color-accent)", fontStyle: "italic" }}>
            It's what you become while using it.
          </em>
        </div> */}

        <div
          className="mt-16 flex flex-wrap items-center justify-between gap-6 border-t pt-8 font-mono-accent text-[11px] uppercase tracking-[0.24em]"
          style={{ borderColor: "var(--color-hairline)", color: "var(--color-ink-soft)" }}
        >
          <div>{commercialCopy.footerNote}</div>
          <nav className="flex flex-wrap gap-6">
            <a href="#features">Features</a>
            <a href="#full-plugin-interaction">Interaction</a>
            <a href="#master-envelope">Envelope</a>
            <a href="#pricing">Licensing</a>
            <a href="#specs">Specs</a>
            <a href="#">Press</a>
          </nav>
        </div>
      </div>
    </footer>
  );
}
