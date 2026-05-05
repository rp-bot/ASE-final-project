export function ThreeDInteraction() {
  return (
    <section id="3d-interaction" className="relative px-6 py-12 md:py-16">
      <div className="mx-auto max-w-6xl">
        <div className="mb-6 md:mb-8">
          <div
            className="font-mono-accent text-[10px] uppercase tracking-[0.24em]"
            style={{ color: "var(--color-accent)" }}
          >
            3D interaction
          </div>
        </div>
        <div className="grid items-center gap-8 md:grid-cols-12">
          <div className="md:col-span-7 md:col-start-1">
            <div className="mx-auto w-full max-w-sm overflow-hidden rounded-[2px] bg-black md:mx-0">
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
          <div className="md:col-span-5 md:block mx-auto w-full h-full flex flex-col justify-evenly items-start text-3xl gap-4">
            <p className="leading-relaxed text-start border-l-4 border-accent pl-4 my-8">
              The 3D view allows you to control the Scene, the Cube and the Cursor.
            </p>
            <p className="leading-relaxed text-start border-l-4 border-accent pl-4 my-8">
              You can control the Angular velocity of the 3D cube either in gravity mode or in zero G. 
            </p>
            <p className="leading-relaxed text-start border-l-4 border-accent pl-4 my-8">
              Find a plane in the vertical space, and use the joystick to move the cursor on the plane.
            </p>
          </div>
        </div>
      </div>
    </section>
  );
}
