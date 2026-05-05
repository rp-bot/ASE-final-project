export function MasterEnvelope() {
  return (
    <section
      id="master-envelope"
      className="relative px-6 py-24 md:py-32"
      style={{ background: "var(--color-bg-3)" }}
    >
      <div className="mx-auto max-w-6xl">
        <div className="mb-8 text-right md:mb-10">
          <div
            className="font-mono-accent text-[11px] uppercase tracking-[0.28em]"
            style={{ color: "var(--color-accent)" }}
          >
            Master envelope
          </div>
        </div>
        <div className="grid items-center gap-8 md:grid-cols-12">
          <div className="mx-auto flex h-full w-full flex-col items-start justify-evenly gap-4 md:col-span-5">
            <p
              className="my-4 border-l-4 pl-4 text-start text-xl leading-relaxed md:text-2xl"
              style={{ borderColor: "var(--color-accent-2)", color: "var(--color-ink)" }}
            >
              Shape contour and response with a dedicated envelope view.
            </p>
            <p
              className="my-4 border-l-4 pl-4 text-start text-xl leading-relaxed md:text-2xl"
              style={{ borderColor: "var(--color-accent-2)", color: "var(--color-ink)" }}
            >
              Select exactly which oscillators the envelope should affect.
            </p>
          </div>
          <div className="md:col-span-7 md:col-start-6">
            <div
              className="mx-auto w-full max-w-sm overflow-hidden border bg-black md:ml-auto md:mr-0"
              style={{ borderColor: "var(--color-hairline)", borderRadius: "3px" }}
            >
              <video
                src="/images/master_envelope.mp4"
                autoPlay
                muted
                loop
                playsInline
                preload="metadata"
                className="h-full w-full object-cover"
              />
            </div>
          </div>
        </div>
      </div>
    </section>
  );
}
