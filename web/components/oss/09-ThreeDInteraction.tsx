export function ThreeDInteraction() {
  return (
    <section id="3d-interaction" className="relative px-6 py-24 md:py-32">
      <div className="mx-auto max-w-6xl">
        <div className="mb-8 md:mb-10">
          <div
            className="font-mono-accent text-[11px] uppercase tracking-[0.28em]"
            style={{ color: "var(--color-accent)" }}
          >
            3D interaction
          </div>
        </div>
        <div className="grid items-center gap-8 md:grid-cols-12">
          <div className="md:col-span-7 md:col-start-1">
            <div
              className="mx-auto w-full max-w-sm overflow-hidden border bg-black md:mx-0"
              style={{ borderColor: "var(--color-hairline)", borderRadius: "3px" }}
            >
              <video
                src="/images/3d_interaction.mp4"
                autoPlay
                muted
                loop
                playsInline
                preload="metadata"
                className="h-full w-full object-cover"
              />
            </div>
          </div>
          <div className="mx-auto flex h-full w-full flex-col items-start justify-evenly gap-4 md:col-span-5">
            <p
              className="my-4 border-l-4 pl-4 text-start text-xl leading-relaxed md:text-2xl"
              style={{ borderColor: "var(--color-accent-2)", color: "var(--color-ink)" }}
            >
              The 3D view lets you control the scene, cube, and cursor.
            </p>
            <p
              className="my-4 border-l-4 pl-4 text-start text-xl leading-relaxed md:text-2xl"
              style={{ borderColor: "var(--color-accent-2)", color: "var(--color-ink)" }}
            >
              You can drive angular velocity in gravity mode or in zero-G.
            </p>
            <p
              className="my-4 border-l-4 pl-4 text-start text-xl leading-relaxed md:text-2xl"
              style={{ borderColor: "var(--color-accent-2)", color: "var(--color-ink)" }}
            >
              Pick a vertical plane and move the cursor with the joystick.
            </p>
          </div>
        </div>
      </div>
    </section>
  );
}
