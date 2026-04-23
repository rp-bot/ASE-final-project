import { commercialHighlights } from "@/content/features";

export function Craft() {
  return (
    <section id="algorithms" className="relative px-6 py-28 md:py-36">
      <div className="mx-auto max-w-6xl">
        <div className="mx-auto max-w-3xl text-center">
          <div
            className="font-mono-accent text-[11px] uppercase tracking-[0.3em]"
            style={{ color: "var(--color-ink-soft)" }}
          >
            Why it feels different
          </div>
          <h2
            className="mt-5 font-display"
            style={{
              fontSize: "clamp(2.4rem, 5vw, 4.2rem)",
              lineHeight: 1.02,
              fontWeight: 400,
              letterSpacing: "-0.02em",
            }}
          >
            Three things you
            <br />
            <em style={{ color: "var(--color-accent)", fontStyle: "italic" }}>
              hear immediately.
            </em>
          </h2>
          <p
            className="mx-auto mt-6 max-w-xl text-[16px] leading-relaxed"
            style={{ color: "var(--color-ink-soft)" }}
          >
            A synthesizer only matters for what comes out of it. Here is what comes out
            of this one — and why it doesn't sound like anything else you own.
          </p>
        </div>

        <div className="mt-20 grid gap-y-20 md:gap-y-28">
          {commercialHighlights.map((item, i) => (
            <article
              key={item.title}
              className={`grid items-center gap-10 md:grid-cols-12 ${
                i % 2 === 1 ? "md:[&>div:first-child]:order-2" : ""
              }`}
            >
              <div className="md:col-span-5">
                <div
                  className="font-mono-accent text-[11px] uppercase tracking-[0.24em]"
                  style={{ color: "var(--color-accent)" }}
                >
                  {String(i + 1).padStart(2, "0")} — {item.label}
                </div>
                <h3
                  className="mt-3 font-display"
                  style={{
                    fontSize: "clamp(1.8rem, 3.4vw, 2.6rem)",
                    lineHeight: 1.05,
                    fontWeight: 400,
                    letterSpacing: "-0.01em",
                  }}
                >
                  {item.title}
                </h3>
                <p
                  className="mt-5 text-[16px] leading-relaxed"
                  style={{ color: "var(--color-ink-soft)" }}
                >
                  {item.body}
                </p>
              </div>
              <div
                className="relative aspect-square md:col-span-7"
                style={{
                  background: "var(--color-bg-3)",
                  borderRadius: "2px",
                }}
              >
                <HighlightDiagram index={i} />
              </div>
            </article>
          ))}
        </div>
      </div>
    </section>
  );
}

function HighlightDiagram({ index }: { index: number }) {
  if (index === 0) {
    // Eight corners blending into one cursor — "seamless blending"
    return (
      <svg viewBox="0 0 400 400" className="absolute inset-0 h-full w-full p-12">
        <defs>
          <radialGradient id="cc-glow" cx="50%" cy="50%" r="50%">
            <stop offset="0%" stopColor="var(--color-accent)" stopOpacity="0.35" />
            <stop offset="100%" stopColor="var(--color-accent)" stopOpacity="0" />
          </radialGradient>
        </defs>
        <circle cx="210" cy="210" r="110" fill="url(#cc-glow)" />

        <g fill="none" stroke="var(--color-ink)" strokeWidth="1" opacity="0.8">
          <path d="M120 80 L300 80 L300 240 L120 240 Z" />
          <path d="M60 140 L340 140 L340 320 L60 320 Z" />
          <line x1="120" y1="80" x2="60" y2="140" />
          <line x1="300" y1="80" x2="340" y2="140" />
          <line x1="300" y1="240" x2="340" y2="320" />
          <line x1="120" y1="240" x2="60" y2="320" />
        </g>
        {[
          [120, 80], [300, 80], [120, 240], [300, 240],
          [60, 140], [340, 140], [60, 320], [340, 320],
        ].map(([x, y], i) => (
          <g key={i}>
            <line
              x1={x}
              y1={y}
              x2="210"
              y2="210"
              stroke="var(--color-accent)"
              strokeWidth="0.6"
              strokeDasharray="2 3"
              opacity="0.5"
            />
            <circle cx={x} cy={y} r="6" fill="var(--color-accent)" />
          </g>
        ))}
        <circle cx="210" cy="210" r="12" fill="var(--color-ink)" />
        <circle cx="210" cy="210" r="4" fill="var(--color-bg-2)" />
      </svg>
    );
  }
  if (index === 1) {
    // A sweeping curve traced by a cursor bead — "organic motion"
    return (
      <svg viewBox="0 0 400 400" className="absolute inset-0 h-full w-full p-12">
        <g stroke="var(--color-ink)" opacity="0.12" strokeWidth="1">
          {[0, 1, 2, 3, 4, 5].map((i) => (
            <line key={`h${i}`} x1="0" y1={i * 60 + 40} x2="400" y2={i * 60 + 40} />
          ))}
        </g>
        {/* soft shadow under the curve */}
        <path
          d="M30 340 C 80 340, 100 80, 180 180 S 260 340, 370 60"
          fill="none"
          stroke="var(--color-accent)"
          strokeOpacity="0.18"
          strokeWidth="14"
          strokeLinecap="round"
        />
        <path
          d="M30 340 C 80 340, 100 80, 180 180 S 260 340, 370 60"
          fill="none"
          stroke="var(--color-accent)"
          strokeWidth="2.5"
          strokeLinecap="round"
        />
        {/* traveling "bead" showing cursor at a point on the path */}
        <circle cx="230" cy="240" r="10" fill="var(--color-ink)" />
        <circle cx="230" cy="240" r="4" fill="var(--color-bg-2)" />
        {/* small anchor dots, not labeled */}
        {[
          [30, 340], [180, 180], [370, 60],
        ].map(([x, y], i) => (
          <circle
            key={i}
            cx={x}
            cy={y}
            r="5"
            fill="none"
            stroke="var(--color-ink)"
            strokeWidth="1"
            opacity="0.4"
          />
        ))}
      </svg>
    );
  }
  // Sixteen voice meters, lit at different heights — "effortless polyphony"
  return (
    <svg viewBox="0 0 400 400" className="absolute inset-0 h-full w-full p-12">
      {Array.from({ length: 16 }).map((_, i) => {
        const col = i % 8;
        const row = Math.floor(i / 8);
        const x = col * 42 + 24;
        const y = row * 150 + 60;
        const h = 30 + ((i * 37) % 90);
        return (
          <g key={i} transform={`translate(${x} ${y})`}>
            <rect
              width="28"
              height="110"
              fill="none"
              stroke="var(--color-ink)"
              strokeWidth="1"
              opacity="0.6"
            />
            <rect
              y={110 - h}
              width="28"
              height={h}
              fill="var(--color-accent)"
              opacity={0.35 + (i % 8) * 0.07}
            />
          </g>
        );
      })}
      <text
        x="200"
        y="380"
        textAnchor="middle"
        fontSize="11"
        fontFamily="var(--font-mono)"
        fill="var(--color-ink-soft)"
        letterSpacing="2"
      >
        16 VOICES · ALL PLAYING
      </text>
    </svg>
  );
}
