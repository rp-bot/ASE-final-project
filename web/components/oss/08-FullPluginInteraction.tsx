export function FullPluginInteraction() {
  return (
    <section
      id="full-plugin-interaction"
      className="relative px-6 py-24 md:py-32"
      style={{ background: "var(--color-bg-3)" }}
    >
      <div className="mx-auto max-w-7xl">
        <div className="mx-auto mb-10 max-w-5xl">
          <div
            className="font-mono-accent text-[11px] uppercase tracking-[0.28em]"
            style={{ color: "var(--color-accent)" }}
          >
            Full plugin interaction
          </div>
        </div>
        <div
          className="mx-auto max-w-5xl overflow-hidden border"
          style={{ borderColor: "var(--color-hairline)", borderRadius: "3px" }}
        >
          <video
            src="/images/full_plugin_interaction.mp4"
            autoPlay
            muted
            loop
            playsInline
            preload="metadata"
            className="h-full w-full object-cover"
          />
        </div>
        <p
          className="mx-auto mt-8 w-full max-w-4xl border-l-4 pl-4 text-start text-2xl leading-tight md:text-3xl"
          style={{ color: "var(--color-ink)", borderColor: "var(--color-accent-2)" }}
        >
          8 corner oscillators are mixed by cursor XYZ via trilinear interpolation with unity-sum weights.
          GUI cursor and trajectory state are transported lock-free into the audio-thread parameter snapshot.
        </p>
      </div>
    </section>
  );
}
