"use client";

import { audioSamples } from "@/content/features";
import { useEffect, useMemo, useRef, useState } from "react";
import type WaveSurfer from "wavesurfer.js";

type TrackState = {
  playing: boolean;
  currentTime: number;
  duration: number;
};

const defaultTrackState: TrackState = {
  playing: false,
  currentTime: 0,
  duration: 0,
};

function hexToRgba(hex: string, alpha: number): string {
  const clean = hex.trim().replace("#", "");
  const full = clean.length === 3 ? clean.split("").map((c) => c + c).join("") : clean;
  const valid = /^[0-9a-fA-F]{6}$/.test(full);

  if (!valid) {
    return `rgba(127, 127, 127, ${alpha})`;
  }

  const r = Number.parseInt(full.slice(0, 2), 16);
  const g = Number.parseInt(full.slice(2, 4), 16);
  const b = Number.parseInt(full.slice(4, 6), 16);
  return `rgba(${r}, ${g}, ${b}, ${alpha})`;
}

function parseCssColor(color: string): { r: number; g: number; b: number } | null {
  const trimmed = color.trim();

  if (trimmed.startsWith("#")) {
    const clean = trimmed.replace("#", "");
    const full = clean.length === 3 ? clean.split("").map((c) => c + c).join("") : clean;
    if (!/^[0-9a-fA-F]{6}$/.test(full)) {
      return null;
    }
    return {
      r: Number.parseInt(full.slice(0, 2), 16),
      g: Number.parseInt(full.slice(2, 4), 16),
      b: Number.parseInt(full.slice(4, 6), 16),
    };
  }

  const rgbMatch = trimmed.match(
    /^rgba?\(\s*(\d{1,3})[\s,]+(\d{1,3})[\s,]+(\d{1,3})(?:[\s,/]+[\d.]+)?\s*\)$/i,
  );
  if (!rgbMatch) {
    return null;
  }

  return {
    r: Number.parseInt(rgbMatch[1], 10),
    g: Number.parseInt(rgbMatch[2], 10),
    b: Number.parseInt(rgbMatch[3], 10),
  };
}

function luminance({ r, g, b }: { r: number; g: number; b: number }): number {
  return 0.2126 * r + 0.7152 * g + 0.0722 * b;
}

function formatTime(seconds: number): string {
  if (!Number.isFinite(seconds) || seconds < 0) {
    return "0:00";
  }

  const rounded = Math.floor(seconds);
  const mins = Math.floor(rounded / 60);
  const secs = rounded % 60;
  return `${mins}:${secs.toString().padStart(2, "0")}`;
}

export function AudioSamples() {
  const waveformRefs = useRef<Array<HTMLDivElement | null>>([]);
  const wavesurferRefs = useRef<Array<WaveSurfer | null>>([]);
  const [tracks, setTracks] = useState<TrackState[]>(
    () => audioSamples.map(() => ({ ...defaultTrackState })),
  );

  useEffect(() => {
    let mounted = true;

    async function initWaveforms() {
      const { default: WaveSurferFactory } = await import("wavesurfer.js");
      if (!mounted) {
        return;
      }
      const computed = window.getComputedStyle(document.documentElement);
      const bg3 = computed.getPropertyValue("--color-bg-3");
      const bgRgb = parseCssColor(bg3);
      const darkTheme = bgRgb ? luminance(bgRgb) < 128 : false;

      const waveColor = darkTheme ? "rgba(255, 255, 255, 0.62)" : "rgba(0, 0, 0, 0.88)";
      const progressColor = darkTheme ? "rgba(255, 255, 255, 0.3)" : "rgba(0, 0, 0, 0.24)";
      const cursorColor = darkTheme ? "rgba(255, 255, 255, 0.9)" : "var(--color-accent-2)";

      audioSamples.forEach((sample, index) => {
        const container = waveformRefs.current[index];
        if (!container) {
          return;
        }

        const wave = WaveSurferFactory.create({
          container,
          url: sample.file,
          height: 80,
          barWidth: 3,
          barGap: 2,
          barRadius: 2,
          normalize: true,
          cursorWidth: 2,
          cursorColor,
          waveColor,
          progressColor,
        });

        wavesurferRefs.current[index] = wave;

        wave.on("ready", () => {
          setTracks((prev) => {
            const next = [...prev];
            next[index] = {
              ...next[index],
              duration: wave.getDuration(),
            };
            return next;
          });
        });

        wave.on("timeupdate", (currentTime: number) => {
          setTracks((prev) => {
            const next = [...prev];
            next[index] = {
              ...next[index],
              currentTime,
            };
            return next;
          });
        });

        wave.on("play", () => {
          wavesurferRefs.current.forEach((instance, instanceIndex) => {
            if (instance && instanceIndex !== index) {
              instance.pause();
            }
          });

          setTracks((prev) =>
            prev.map((item, itemIndex) => ({
              ...item,
              playing: itemIndex === index,
            })),
          );
        });

        wave.on("pause", () => {
          setTracks((prev) => {
            const next = [...prev];
            next[index] = {
              ...next[index],
              playing: false,
            };
            return next;
          });
        });

        wave.on("finish", () => {
          setTracks((prev) => {
            const next = [...prev];
            next[index] = {
              ...next[index],
              playing: false,
              currentTime: next[index].duration,
            };
            return next;
          });
        });
      });
    }

    initWaveforms();

    return () => {
      mounted = false;
      wavesurferRefs.current.forEach((instance) => instance?.destroy());
      wavesurferRefs.current = [];
    };
  }, []);

  const cards = useMemo(
    () =>
      audioSamples.map((sample, index) => ({
        ...sample,
        state: tracks[index] ?? defaultTrackState,
      })),
    [tracks],
  );

  return (
    <section id="audio" className="relative px-6 py-20 md:py-24">
      <div className="mx-auto max-w-6xl">
        <div className="mb-10 md:mb-12">
          <div
            className="font-mono-accent text-[10px] uppercase tracking-[0.24em]"
            style={{ color: "var(--color-accent)" }}
          >
            Audio samples
          </div>
          <h2
            className="mt-4 font-display"
            style={{
              fontSize: "clamp(2rem, 4vw, 3rem)",
              lineHeight: 1.02,
              fontWeight: 400,
              letterSpacing: "-0.01em",
            }}
          >
            Hear the cube in motion.
          </h2>
          <p
            className="mt-4 max-w-2xl font-mono-accent text-sm leading-relaxed"
            style={{ color: "var(--color-ink-soft)" }}
          >
            Play each clip, scrub through the waveform, and explore how timbre
            shifts as the cursor moves through 3D space.
          </p>
        </div>

        <div className="grid gap-5">
          {cards.map((sample, index) => (
            <article
              key={sample.file}
              className="border p-5"
              style={{
                borderColor: "var(--color-hairline)",
                background: "var(--color-bg-2)",
                borderRadius: "4px",
              }}
            >
              <div className="mb-3 flex items-center gap-4">
                <div>
                  <div className="flex items-center gap-3">
                    <h3
                      style={{
                        fontFamily: "var(--font-space-grotesk)",
                        fontWeight: 600,
                        fontSize: "1rem",
                        color: "var(--color-ink)",
                      }}
                    >
                      {sample.title}
                    </h3>
                    <button
                      type="button"
                      onClick={() => wavesurferRefs.current[index]?.playPause()}
                      className="min-w-[86px] border px-3 py-1.5 font-mono-accent text-[11px] uppercase tracking-[0.16em] transition-colors hover:border-accent hover:text-accent"
                      style={{
                        borderColor: "var(--color-hairline)",
                        color: "var(--color-ink)",
                      }}
                    >
                      {sample.state.playing ? "Pause" : "Play"}
                    </button>
                  </div>
                </div>
              </div>

              <div
                className="rounded-[3px] border px-2 py-2"
                style={{
                  borderColor: "var(--color-hairline)",
                  background: "var(--color-bg-3)",
                }}
              >
                <div
                  ref={(element) => {
                    waveformRefs.current[index] = element;
                  }}
                />
              </div>

              <div
                className="mt-3 flex items-center justify-between font-mono-accent text-[11px]"
                style={{ color: "var(--color-ink-soft)" }}
              >
                <span>{formatTime(sample.state.currentTime)}</span>
                <span>{formatTime(sample.state.duration)}</span>
              </div>
            </article>
          ))}
        </div>
      </div>
    </section>
  );
}
