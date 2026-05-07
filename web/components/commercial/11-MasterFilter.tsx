export function MasterFilter() {
  return (
    <section id="master-filter" className="relative px-6 py-12 md:py-16 md:pb-28">
      <div className="mx-auto max-w-6xl">
        <div className="mb-6 md:mb-8">
          <div
            className="font-mono-accent text-[10px] uppercase tracking-[0.24em]"
            style={{ color: "var(--color-accent)" }}
          >
            Master filter
          </div>
        </div>
        <div className="grid items-center gap-8 md:grid-cols-12">
          <div className="md:col-span-7 md:col-start-1">
            <div className="mx-auto w-full max-w-sm overflow-hidden rounded-[2px] bg-black md:mx-0">
              <video
                src="/images/master_filter.mp4"
                autoPlay
                muted
                loop
                playsInline
                preload="metadata"
                className="h-full w-full object-cover"
              />
            </div>
          </div>
          <div className="md:col-span-5 md:block mx-auto w-full h-full flex flex-col justify-evenly items-start text-3xl gap-4">
            <p className="leading-relaxed text-start border-l-4 border-accent pl-4 my-8">
              Fine-tune tone shaping from a compact filter-focused control perspective.
            </p>
            <p className="leading-relaxed text-start border-l-4 border-accent pl-4 my-8">
           Similar to the envelope, you can toggle corresponding oscillator buttons to adjust multiple oscillator filters at once.
            </p>
            <p className="leading-relaxed text-start border-l-4 border-accent pl-4 my-8">
              You can control the drive of the filter to add some distortion.
            </p>
          </div>
        </div>
      </div>
    </section>
  );
}
