import { ossCopy } from "@/content/opensource";

export function Footer() {
  return (
    <footer
      className="relative px-6 py-16"
      style={{
        borderTop: "1px solid var(--color-hairline)",
        background: "var(--color-bg-2)",
      }}
    >
      <div className="mx-auto max-w-6xl">
        {/* <pre
          className="m-0 overflow-x-auto font-mono-accent text-[13px] leading-[1.55]"
          style={{ color: "var(--color-ink-soft)" }}
        >
{`# volumetric vector synthesis
# license: GPLv3
# made with: JUCE 8 · OpenGL 3.3+ · GLM · C++17
# funded with: coffee
#
# issues    → github.com/rp-bot/vvs/issues
# discuss   → github.com/rp-bot/vvs/discussions
# sponsors  → github.com/sponsors/rp-bot
`}
        </pre> */}

        <div className="mt-10 flex flex-wrap items-center justify-between gap-6">
          <div
            className="font-mono-accent text-[11px]"
            style={{ color: "var(--color-ink-soft)" }}
          >
            {ossCopy.footerNote}
          </div>
          <nav
            className="flex flex-wrap gap-5 font-mono-accent text-[11px] uppercase tracking-[0.18em]"
            style={{ color: "var(--color-ink-soft)" }}
          >
            <a href="#features" className="hover:text-[var(--color-accent-2)]">features</a>
            <a href="#showcase" className="hover:text-[var(--color-accent-2)]">screenshots</a>
            <a href="#algorithms" className="hover:text-[var(--color-accent-2)]">dsp</a>
            <a href="#pricing" className="hover:text-[var(--color-accent-2)]">download</a>
            <a href="#specs" className="hover:text-[var(--color-accent-2)]">specs</a>
            <a href={ossCopy.repoUrl} className="hover:text-[var(--color-accent-2)]">repo</a>
          </nav>
        </div>
      </div>
    </footer>
  );
}
