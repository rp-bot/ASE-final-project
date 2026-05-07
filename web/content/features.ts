export type Feature = {
  title: string;
  body: string;
  accent: "cyan" | "magenta";
};

/** Studio / commercial — what the musician hears and feels. No implementation talk. */
export const commercialFeatures: Feature[] = [
  {
    title: "Eight timbres in one instrument",
    body:
      "Place a sound source at each corner of the cube. As the cursor moves, trilinear blending mixes all 8 sources into one continuous tone, so every position feels playable and musical.",
    accent: "cyan",
  },
  {
    title: "Motion that feels alive",
    body:
      "Draw a path and let it run. The tone shifts smoothly through the cube without stepping or clicks, so movement sounds expressive instead of robotic.",
    accent: "magenta",
  },
  {
    title: "Sixteen voices of polyphony",
    body:
      "Play lush chords, dense pads, and fast runs at the same time. You keep musical detail and dynamics without the instrument feeling fragile in a real session.",
    accent: "cyan",
  },
  {
    title: "Dual filter architecture",
    body:
      "Shape tone quickly with two filters per voice and familiar curves (lowpass, highpass, bandpass, notch). Modulate cutoff and resonance from your movement in the cube.",
    accent: "magenta",
  },
  {
    title: "An interface that responds",
    body:
      "The visuals track what you hear. Corners light up as you move, the center reacts to intensity, and paths show exactly where your sound is going.",
    accent: "cyan",
  },
  {
    title: "Ready for spatial control",
    body:
      "Map MIDI or other XYZ-style control and perform timbre like a physical gesture. It turns sound design into performance, not menu diving.",
    accent: "magenta",
  },
];

/** Open-source — the implementation facts a dev / power user wants to see. */
export const ossFeatures: Feature[] = [
  {
    title: "Eight oscillators. One cube.",
    body:
      "Eight wavetable voices at the vertices of a unit cube. A 3D cursor computes per-vertex gains via trilinear interpolation — unity-gain across the entire space, no hot-spots.",
    accent: "cyan",
  },
  {
    title: "Trajectory-ready control path",
    body:
      "Trajectory mode is already wired through the lock-free GUI state into the audio-thread parameter snapshot, so movement automation has a clear integration path.",
    accent: "magenta",
  },
  {
    title: "16-voice polyphonic engine",
    body:
      "Built on JUCE's Synthesiser with a custom 8-to-1 voice mixer. The current shipping path is scalar and tuned for correctness and stable real-time playback.",
    accent: "cyan",
  },
  {
    title: "per oscillator and master filter and envelope",
    body:
      "Per oscillator filter and envelope allow you to adjust the filter and envelope parameters for each voice individually. Master filter and envelope allow you to adjust the filter and envelope parameters for all voices at once.",
    accent: "magenta",
  },
  {
    title: "Hybrid 2D/3D cube and cursor rendering",
    body:
      "The plugin editor runs an OpenGL 3.3+ context for real 3D rendering of the cube and cursor, while UI controls remain on JUCE's native vector path. No Z-ordering issues—3D visuals and 2D knobs coexist seamlessly in the interface.",
    accent: "cyan",
  },
  {
    title: "Spinning cube and interactive motion",
    body:
      "The 3D cube spins freely on its axis and responds to user input; you can control the cube’s angular velocity and move the cursor throughout 3D space, directly shaping sound with physical gestures.",
    accent: "cyan",
  },
  // {
  //   // title: "Readable DSP, in the open",
  //   // body:
  //   //   "C++17 throughout, organized under /Source/DSP. Lock-free audio ↔ GUI via juce::AbstractFifo. Contribution friendly — issues tagged `good-first-issue` kept fresh.",
  //   // accent: "magenta",
  // },
];

/** Studio / commercial — three results-focused highlights, paired with diagrams. */
export type CommercialHighlight = {
  label: string;
  title: string;
  body: string;
};

export const commercialHighlights: CommercialHighlight[] = [
  {
    label: "Seamless blending",
    title: "Eight timbres become one.",
    body:
      "Every point inside the cube is a sound. Trilinear blending mixes the 8 corner sources smoothly, so you move without seams, jumps, or clicks.",
  },
  {
    label: "Organic motion",
    title: "Movement that doesn't feel automated.",
    body:
      "Draw a path through the cube and the cursor flows like a melodic phrase. Transitions stay smooth, so automation feels performed instead of programmed.",
  },
  {
    label: "Effortless polyphony",
    title: "Sixteen voices. No compromises.",
    body:
      "Chords, pads, and runs all stay full and clear across sixteen voices. You get width and depth without losing control of the session.",
  },
];

/** Open-source — the real algorithms. Shown with code in /Internals. */
export type Algorithm = {
  name: string;
  subtitle: string;
  body: string;
};

export const algorithms: Algorithm[] = [
  {
    name: "Trilinear Interpolation",
    subtitle: "8-source unity-gain mixing",
    body:
      "Each vertex's gain is the volume of the opposite rectangular prism: three-stage linear interpolation (X -> Y -> Z), unity-sum across the cube. The mixer runs allocation-free on the audio thread.",
  },
  {
    name: "Lock-free GUI -> audio state",
    subtitle: "Atomic cursor + trajectory transport",
    body:
      "Cursor XYZ and trajectory-active state are stored in atomics and read safely by the audio thread via ParameterSnapshot. No locks in the render path.",
  },
  {
    name: "Coverage with unit tests",
    subtitle: "Math + mixer behavior validated",
    body:
      "Unit tests cover trilinear weight math and mixer behavior (including unity-sum and edge/corner cases), making DSP iteration safer as features land.",
  },
];

export const specs = [
  { label: "Formats", value: "VST3 · AU" },
  { label: "Platforms", value: "Windows 10+ · macOS 12+ · Linux" },
  { label: "Polyphony", value: "16 voices" },
  { label: "Oscillators per voice", value: "8 (wavetable)" },
  { label: "Filters", value: "lowpass, with resonance control" },
  { label: "Graphics", value: "OpenGL 3.3+ (hybrid render)" },
  {label:"LFO", value: "Coming soon..."},
];

export type AudioSample = {
  title: string;
  file: string;
};

export const audioSamples: AudioSample[] = [
  {
    title: "Sample 01",
    file: "/audio/s1.mp3",
  },
  {
    title: "Sample 02",
    file: "/audio/s2.mp3",
  },
  {
    title: "Sample 03",
    file: "/audio/s3.mp3",
  },
  {
    title: "Sample 04",
    file: "/audio/s4.mp3",
  },
];
