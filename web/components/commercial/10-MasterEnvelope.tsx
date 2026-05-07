export function MasterEnvelope() {
  return (
    <section id="master-envelope" className="relative px-6 py-12 md:py-16">
      <div className="mx-auto max-w-6xl">
        <div className="mb-6 text-right md:mb-8">
          <div
            className="font-mono-accent text-[10px] uppercase tracking-[0.24em]"
            style={{ color: "var(--color-accent)" }}
          >
            Master envelope
          </div>
        </div>
        <div className="grid items-center gap-8 md:grid-cols-12">
          <div className="md:col-span-5 md:block mx-auto w-full h-full flex flex-col justify-evenly items-start text-3xl gap-4">
            <p className="leading-relaxed text-start border-l-4 border-accent pl-4 my-8">
              Shape contour and response with a dedicated envelope performance view.
            </p>
            <p className="leading-relaxed text-start border-l-4 border-accent pl-4 my-8">
            Toggle corresponding oscillator buttons to adjust multiple oscillator envelopes at once.
            </p>
          </div>
          <div className="md:col-span-7 md:col-start-6">
            <div className="ml-auto w-full max-w-sm overflow-hidden rounded-[2px] bg-black">
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
