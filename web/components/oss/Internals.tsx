import { algorithms } from "@/content/features";

const snippets = [
  `// Math3D.cpp -- current trilinearWeights implementation
std::array<float, 8> trilinearWeights(float x, float y, float z) {
  x = std::clamp(x, 0.f, 1.f);
  y = std::clamp(y, 0.f, 1.f);
  z = std::clamp(z, 0.f, 1.f);

  std::array<float, 8> w{};
  for (int k = 0; k < 2; ++k)
    for (int j = 0; j < 2; ++j)
      for (int i = 0; i < 2; ++i) {
        const int c = 4 * k + 2 * j + i;
        w[c] = (i ? x : 1.f - x) * (j ? y : 1.f - y) * (k ? z : 1.f - z);
      }
  return w;
}`,
  `// AtomicGuiState.h  -- lock-free cursor transport
class AtomicGuiState {
public:
  void setCursorPosition(float x, float y, float z);
  void setTrajectoryActive(bool active);
  glm::vec3 getCursorPosition() const;
  bool isTrajectoryActive() const;
private:
  std::atomic<float> m_cursorX, m_cursorY, m_cursorZ;
  std::atomic<bool>  m_trajectoryActive;
};`,
  `// Math3DTest.cpp  -- interpolation safety net
beginTest("trilinearWeights sum to 1 for random points");
for (const auto& xyz : points) {
  auto w = Utils::trilinearWeights(xyz[0], xyz[1], xyz[2]);
  expectWithinAbsoluteError(std::accumulate(w.begin(), w.end(), 0.0f),
                            1.0f, 1.0e-6f);
}
beginTest("TrilinearMixer8 centers to equal gains");
// ... additional mixer edge/corner tests`,
];

export function Internals() {
  return (
    <section id="algorithms" className="relative px-6 py-24 md:py-32">
      <div className="mx-auto max-w-6xl">
        <div className="mb-12 max-w-2xl">
          <div
            className="font-mono-accent text-[11px] uppercase tracking-[0.28em]"
            style={{ color: "var(--color-ink-soft)" }}
          >
            /src/dsp
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
            <span className="sketch-underline">Real algorithms.</span>
            <br />
            Read the source.
          </h2>
          <p
            className="mt-5 text-[14px] font-mono-accent leading-relaxed"
            style={{ color: "var(--color-ink-soft)" }}
          >
            The engine lives in /Source/DSP. Three ideas, none of them new — none of
            them previously combined into a volumetric synthesizer.
          </p>
        </div>

        <div className="grid gap-6 lg:gap-8">
          {algorithms.map((algo, i) => (
            <article key={algo.name} className="grid items-start gap-5 lg:grid-cols-[1fr_1.1fr] lg:gap-6">
              <div>
                <div
                  className="font-mono-accent text-[11px] uppercase tracking-[0.2em]"
                  style={{ color: "var(--color-accent)" }}
                >
                  {String(i + 1).padStart(2, "0")} · {algo.subtitle}
                </div>
                <h3
                  className="mt-3"
                  style={{
                    fontFamily: "var(--font-space-grotesk)",
                    fontSize: "clamp(1.5rem, 2.6vw, 2rem)",
                    fontWeight: 600,
                    letterSpacing: "-0.02em",
                    lineHeight: 1.1,
                  }}
                >
                  {algo.name}
                </h3>
                <p
                  className="mt-4 text-[14px] leading-relaxed font-mono-accent"
                  style={{ color: "var(--color-ink-soft)" }}
                >
                  {algo.body}
                </p>
              </div>
              <div
                className="overflow-hidden border"
                style={{
                  borderColor: "var(--color-hairline)",
                  borderRadius: "3px",
                  background: "#0f1016",
                }}
              >
                <pre
                  className="m-0 overflow-x-auto p-4 text-[11px] leading-[1.5]"
                  style={{
                    fontFamily: "var(--font-mono)",
                    color: "#e4e4e7",
                  }}
                >
                  <code>{snippets[i]}</code>
                </pre>
              </div>
            </article>
          ))}
        </div>
      </div>
    </section>
  );
}
