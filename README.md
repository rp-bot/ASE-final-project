# Volumetric Vector Synthesis Plugin

<!-- [![License](https://img.shields.io/badge/license-TBD-blue.svg)](LICENSE)
[![JUCE](https://img.shields.io/badge/JUCE-8-orange.svg)](https://juce.com/)
[![OpenGL](https://img.shields.io/badge/OpenGL-3.3+-green.svg)](https://www.opengl.org/)
[![Platform](https://img.shields.io/badge/platform-Windows%20%7C%20macOS-lightgrey.svg)]() -->



## Quick Links

- [Latest Release](https://github.com/rp-bot/ASE-final-project/releases/latest)
- [Motivation](#motivation)
- [Applications & Target Users](#applications--target-users)
- [Description](#description)
- [Getting Started](#getting-started)
- [Contributing](#contributing)
- [Credits](#credits)
- [License](#license)

## Motivation

Classic vector synthesis is still mostly limited to a 2D XY surface with 4-source blending.
This project extends that model to a true 3D space, where users can morph across 8 timbral corners in real time.

The core aim is workflow clarity: shape complex timbral motion with direct spatial gestures instead of many disconnected automation lanes.

## Applications & Target Users

Built for creators who need expressive, evolving timbre:
- **Sound designers and composers** (film, TV, games)
- **Electronic producers and live performers**
- **VR/AR audio developers**

## Description

Volumetric Vector Synthesis Plugin is a JUCE-based instrument concept for 3D timbral navigation.
Instead of static patch switching, users move through a cubic sound field and blend eight sound sources continuously.

Planned scope:
- real-time 3D interpolation between 8 oscillators
- trajectory-based movement through timbral space
- interactive 3D UI for cursor and path control

![Volumetric Vector Synthesis Plugin Screenshot](img/plugin_screenshot.png)
## Getting Started

### Prerequisites

- CMake 3.22+
- Git
- A C++17 toolchain
  - **macOS**: Xcode + Command Line Tools
  - **Windows**: Visual Studio 2022 (Desktop development with C++)
  - **Linux**: GCC or Clang, plus OpenGL/X11 development packages

Dependencies (`JUCE` and `glm`) are fetched automatically by CMake.


### Platform-Specific Configure Commands

**macOS (Apple Silicon):**
```bash
cmake -S . -B build -G Xcode -DCMAKE_OSX_ARCHITECTURES=arm64
cmake --build build --config Release
```

**macOS (Intel):**
```bash
cmake -S . -B build -G Xcode -DCMAKE_OSX_ARCHITECTURES=x86_64
cmake --build build --config Release
```

**Windows (Visual Studio 2022):**
```bash
cmake -S . -B build -G "Visual Studio 17 2022" -A x64
cmake --build build --config Release
```

**Linux (Ninja):**
```bash
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

### Optional Build Flags

- `-DUSE_JUCE_DEVELOP=ON` to use JUCE `develop` (helpful for newer macOS/Xcode setups).

### Build Tests

```bash
cmake --build build --target VolumetricSynth_Tests --config Release
```

### Build Outputs

- `VST3` and `Standalone` on all supported platforms
- `AU` on macOS

### How to Use

#### Main Controls

- **Move the camera view**: Hold `Shift` and drag in the 3D view.
- **Spin the cube**: Hold `Option` (macOS) / `Alt` (Windows/Linux) and spin.
  Turn on **Zero G Mode** to keep the cube spinning continuously.
- **Move the 3D cursor**:
  - Use the gizmo to move along a single axis or within a plane.
  - Turn off the gizmo for free movement.
- **2D joystick and XYZ knobs**:
  - Use the 2D joystick to move the cursor on a plane.
  - Use the XYZ knobs for precise cursor position control.

#### Oscillators, Envelopes, and Filters

- There are **8 oscillators**.
- Each oscillator pane has top tabs to switch between oscillator, envelope, and
  filter controls for that oscillator.
- You can also control multiple envelopes or filters at once with the
  **master controls**.
- Use the color selectors to the right of each editor to choose which
  oscillator(s) receive those envelope/filter parameter changes.

#### Safety / Auto-Mute

- If gain clips beyond `+10 dB`, the plugin enters **auto-mute**, which essentially turns off the audio engine and disallows any input.
- Click **Reset Audio Engine** at the top-right to continue using the plugin.

---

## Contributing

We welcome contributions from the community! Please read our detailed [CONTRIBUTING.md](CONTRIBUTING.md) for guidelines on:

- **Branching Strategy**: `main`, `develop`, `feature/*`, `hotfix/*`
- **Commit Conventions**: Conventional Commits format
- **Code Review Process**: Minimum 1 reviewer approval
- **Testing Requirements**: Unit tests for new features
- **Pull Request Template**: What to include in PR descriptions

## Credits

- **Angelina**: UI, parts of I/O, and plumbing between the audio engine and editor parameters
- **Canting**: DSP and UI implementation
- **Griffin**: Wavetable oscillator implementation
- **Lennon**: 3D visualization, envelope, plumbing, and bug fixes
- **Pratham**: architecture setup, UI implementation, bug fixes, and parts of 3D visualization

## License

This project is licensed under the **GNU General Public License v3.0 (GPL-3.0)**.
See [`COPYING`](COPYING) for the full license text.