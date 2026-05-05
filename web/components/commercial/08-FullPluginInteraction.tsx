export function FullPluginInteraction() {
  return (
    <section
      id="full-plugin-interaction"
      className="relative px-6 py-20 md:py-28"
      style={{ background: "var(--color-bg-3)" }}
    >
      <div className="mx-auto max-w-7xl">
        <div className="mx-auto mb-8 max-w-5xl md:mb-10">
          <div
            className="font-mono-accent text-[10px] uppercase tracking-[0.24em]"
            style={{ color: "var(--color-accent)" }}
          >
            Full plugin interaction
          </div>
        </div>
        <div className="mx-auto max-w-5xl overflow-hidden rounded-[2px] bg-black">
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
          className="mx-auto w-full max-w-4xl flex justify-start mt-6 text-start text-3xl text-balance leading-tight border-l-4 border-accent pl-4 my-8"
          style={{ color: "var(--color-ink)" }}
        >
          Easy control of 8 oscilators and one 3d cursor. 
        </p>
      </div>
    </section>
  );
}
