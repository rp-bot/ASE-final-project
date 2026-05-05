"use client";

/**
 * Ambient “3D space” backdrop for the commercial hero: dot field, perspective grid,
 * paper grain, and vignette. CSS-only motion; respects prefers-reduced-motion.
 */
export function HeroBackdrop() {
  return (
    <div className="pointer-events-none absolute inset-0 -z-10 overflow-hidden" aria-hidden>
      <div className="absolute inset-0 hero-stars" />
      <div className="absolute inset-x-0 bottom-0 h-[70%] hero-floor" />
      <div className="absolute inset-0 paper-grain" />
      <div className="absolute inset-0 hero-vignette" />
    </div>
  );
}
