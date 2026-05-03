"use client";

import Image from "next/image";

type HeroPluginScreenshotProps = {
  variant?: "commercial" | "oss";
};

export function HeroPluginScreenshot({ variant = "commercial" }: HeroPluginScreenshotProps) {
  const isOss = variant === "oss";
  return (
    <div
      className="relative aspect-16/10 w-full overflow-hidden"
      style={{
        borderRadius: isOss ? "4px" : "12px",
        border: isOss
          ? "1px solid color-mix(in srgb, var(--color-ink) 30%, transparent)"
          : "1px solid color-mix(in srgb, var(--color-ink) 18%, transparent)",
        boxShadow: isOss
          ? "0 30px 80px -30px rgba(10, 10, 30, 0.35), 0 0 0 1px rgba(255,255,255,0.04)"
          : "0 50px 140px -40px rgba(50, 20, 0, 0.45)",
      }}
    >
      <Image
        src="/images/hero-plugin.png"
        alt="Volumetric Synth Editor plugin interface with the wavetable cube and module controls"
        fill
        className="object-cover object-top-left"
        sizes="(max-width: 1024px) 100vw, 45vw"
        priority
      />
    </div>
  );
}
