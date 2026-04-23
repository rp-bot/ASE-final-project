"use client";

import { ParallaxLayer } from "./ParallaxCard";

type PluginWindowProps = {
  /** Theme override: commercial gives warm elegant; oss gives technical crisp. */
  variant?: "commercial" | "oss";
};

export function PluginWindow({ variant = "commercial" }: PluginWindowProps) {
  const isOss = variant === "oss";
  return (
    <div
      className="relative aspect-[16/10] w-full overflow-hidden"
      style={{
        background: isOss
          ? "#0f1016"
          : "linear-gradient(160deg, #1d1a14 0%, #0e0b07 100%)",
        borderRadius: isOss ? "4px" : "12px",
        border: isOss
          ? "1px solid color-mix(in srgb, var(--color-ink) 30%, transparent)"
          : "1px solid color-mix(in srgb, var(--color-ink) 18%, transparent)",
        boxShadow: isOss
          ? "0 30px 80px -30px rgba(10, 10, 30, 0.35), 0 0 0 1px rgba(255,255,255,0.04)"
          : "0 50px 140px -40px rgba(50, 20, 0, 0.45)",
      }}
    >
      <ParallaxLayer depth={-0.3} className="absolute inset-0">
        <div
          className="absolute inset-0 opacity-60"
          style={{
            backgroundImage: `
              linear-gradient(to right, rgba(255,255,255,0.04) 1px, transparent 1px),
              linear-gradient(to bottom, rgba(255,255,255,0.04) 1px, transparent 1px)
            `,
            backgroundSize: "48px 48px",
            maskImage:
              "radial-gradient(ellipse at center, black 40%, transparent 85%)",
          }}
        />
      </ParallaxLayer>

      <div className="relative flex h-full flex-col">
        <div
          className="flex items-center gap-2 border-b px-4 py-2.5 text-[10px] uppercase tracking-[0.2em]"
          style={{
            borderColor: "rgba(255,255,255,0.06)",
            color: "rgba(255,255,255,0.42)",
          }}
        >
          <span className="h-2 w-2 rounded-full bg-red-400/70" />
          <span className="h-2 w-2 rounded-full bg-yellow-400/70" />
          <span className="h-2 w-2 rounded-full bg-green-400/70" />
          <span className="ml-3 font-mono-accent">
            vvs.vst3 — Cube 01 · Init Patch
          </span>
        </div>

        <div className="relative flex flex-1 items-center justify-center p-6">
          <ParallaxLayer depth={0.35} className="relative w-[72%]">
            <CubeWireframe variant={variant} />
          </ParallaxLayer>

          <ParallaxLayer depth={0.6} className="pointer-events-none absolute inset-0">
            <div
              className="absolute left-1/2 top-1/2 h-28 w-28 -translate-x-1/2 -translate-y-1/2 rounded-full opacity-80 blur-2xl anim-slow-pulse"
              style={{
                background: `radial-gradient(circle, ${
                  isOss
                    ? "rgba(37, 99, 235, 0.55)"
                    : "rgba(194, 65, 12, 0.55)"
                } 0%, transparent 70%)`,
              }}
            />
          </ParallaxLayer>
        </div>

        <div
          className="flex items-center justify-between border-t px-4 py-2 font-mono-accent text-[10px]"
          style={{
            borderColor: "rgba(255,255,255,0.06)",
            color: "rgba(255,255,255,0.48)",
          }}
        >
          <span>XYZ  0.42 / 0.58 / 0.33</span>
          <span>8 osc · 16 voices · 48 kHz</span>
          <span style={{ color: isOss ? "#a3e635" : "#fbbf24" }}>CPU 7.4%</span>
        </div>
      </div>

      {!isOss && (
        <div
          className="pointer-events-none absolute inset-0 opacity-50"
          style={{
            background:
              "radial-gradient(70% 55% at 85% 0%, rgba(251, 146, 60, 0.22) 0%, transparent 60%), radial-gradient(55% 45% at 10% 100%, rgba(184, 147, 42, 0.20) 0%, transparent 60%)",
          }}
        />
      )}
    </div>
  );
}

function CubeWireframe({ variant }: { variant: "commercial" | "oss" }) {
  const edgeStart = variant === "oss" ? "#60a5fa" : "#fb923c";
  const edgeEnd = variant === "oss" ? "#a3e635" : "#fcd34d";
  const cursorColor = variant === "oss" ? "#f43f5e" : "#fde68a";

  return (
    <svg
      viewBox="0 0 400 260"
      className="w-full"
      fill="none"
      strokeWidth="1.2"
      aria-hidden
    >
      <defs>
        <linearGradient id={`edge-${variant}`} x1="0" y1="0" x2="1" y2="1">
          <stop offset="0%" stopColor={edgeStart} stopOpacity="0.95" />
          <stop offset="100%" stopColor={edgeEnd} stopOpacity="0.95" />
        </linearGradient>
        <radialGradient id={`node-${variant}`} cx="50%" cy="50%" r="50%">
          <stop offset="0%" stopColor={edgeStart} stopOpacity="1" />
          <stop offset="100%" stopColor={edgeStart} stopOpacity="0" />
        </radialGradient>
      </defs>

      <g stroke={`url(#edge-${variant})`} opacity="0.55">
        <path d="M120 70 L300 70 L300 170 L120 170 Z" />
      </g>
      <g stroke={`url(#edge-${variant})`}>
        <path d="M60 100 L340 100 L340 220 L60 220 Z" />
      </g>
      <g stroke={`url(#edge-${variant})`} opacity="0.7">
        <line x1="120" y1="70" x2="60" y2="100" />
        <line x1="300" y1="70" x2="340" y2="100" />
        <line x1="300" y1="170" x2="340" y2="220" />
        <line x1="120" y1="170" x2="60" y2="220" />
      </g>

      {[
        [120, 70], [300, 70], [120, 170], [300, 170],
        [60, 100], [340, 100], [60, 220], [340, 220],
      ].map(([cx, cy], i) => (
        <g key={i}>
          <circle cx={cx} cy={cy} r="10" fill={`url(#node-${variant})`} />
          <circle cx={cx} cy={cy} r="2.5" fill={edgeStart} />
        </g>
      ))}

      <g>
        <circle cx="210" cy="150" r="18" fill={`url(#node-${variant})`} opacity="0.9" />
        <circle cx="210" cy="150" r="4" fill={cursorColor} />
      </g>

      <path
        d="M90 210 Q 180 120, 260 150 T 330 90"
        stroke={edgeEnd}
        strokeDasharray="3 4"
        opacity="0.75"
      />
    </svg>
  );
}
