# Final Presentation Outline (Midterm, 10 Minutes)

Project: **VolumetricSynth**  
Theme: concise, evidence-driven progress update with clear ownership and timeline.

## Slide 1 - Brief Introduction (0:45)
- VolumetricSynth is a JUCE-based plugin for real-time 3D timbre blending.
- Core idea: move from 2D vector synthesis to 3D interpolation across 8 sources.
- Why it matters: better expressive control and cleaner automation for evolving sounds.

**Speaker:** Member A

## Slide 2 - Goals and Scope (1:20)
- **Goal 1:** Build stable core DSP blocks for a volumetric synthesis pipeline.
- **Goal 2:** Integrate modules into a playable plugin path (MIDI -> audio out).
- **Goal 3:** Provide a clear UI/control layer for 3D mix behavior.
- **In scope this semester:** oscillator/filter/envelope/mixer integration, plugin wiring, tests, and core UI state sharing.
- **Out of scope for this phase:** advanced FX suite, large preset ecosystem, deep visual polish.

**Speaker:** Member A

## Slide 3 - Module Overview (1:10)
- `DSP/`: `DualFilter`, `TrilinearMixer8`, oscillation and envelope logic.
- `Plugin/`: processor/editor lifecycle and parameter plumbing.
- `UI/`: mixer gain views and control surfaces tied to DSP state.
- `Tests/`: unit coverage for `DualFilter`, `TrilinearMixer8`, and math/util modules.
- Flow: `MIDI/Input -> Voice/DSP path -> Mixer/Filter/Envelope -> Output + UI feedback`.

**Speaker:** Member B

## Slide 4 - Individual Responsibilities (1:20)
- **Member A (Architecture/Integration):** plugin skeleton, module boundaries, build integration.
- **Member B (DSP Core):** mixer/filter algorithm implementation and tuning.
- **Member C (UI + State Handoff):** editor controls, gain visualization, parameter linking.
- **Member D (Validation/Quality):** unit tests, regression checks, reproducibility support.
- Team understanding check: each member can explain one module outside their primary ownership.

**Speaker:** Member C

## Slide 5 - Progress So Far (Since Jan 20) (1:10)
- Core repository and CMake structure established.
- Foundational DSP modules implemented (`DualFilter`, `TrilinearMixer8`, math helpers).
- Plugin processor/editor path in place for iterative integration.
- Unit tests added for critical DSP/math behavior.
- Current status: strong module-level progress; full end-to-end polish still in progress.

**Speaker:** Member C

## Slide 6 - Remaining Tasks and Risks (1:10)
- Finish full voice/engine integration path and verify stability under load.
- Tighten parameter mapping and edge-case behavior between UI and DSP.
- Expand testing for integration-level scenarios (not only unit modules).
- Risks: integration regressions, time pressure near deadline, performance surprises.
- Mitigation: freeze dates, incremental integration checkpoints, buffer window before submission.

**Speaker:** Member D

## Slide 7 - Comprehensive Timeline (Jan 20 -> May 5) (2:00) [Highest Weight]

| Time Window | Focus | Owner(s) | Risk / Dependency | Done Definition |
| --- | --- | --- | --- | --- |
| **Jan 20 - Feb 2** | Project kickoff, architecture, repo scaffolding | Member A + D | Unclear boundaries | Agreed module map and working build baseline |
| **Feb 3 - Feb 23** | Core DSP module implementation (`DualFilter`, mixer/math foundations) | Member B | Algorithm complexity | Passing module-level tests and stable outputs |
| **Feb 24 - Mar 16** | Plugin integration and parameter plumbing | Member A + C | API mismatch across modules | End-to-end signal path working in plugin shell |
| **Mar 17 - Mar 31** | UI controls/state handoff + test expansion | Member C + D | Thread/state sync issues | UI reflects parameter changes reliably; tests expanded |
| **Apr 1 - Apr 14** | Remaining feature completion and bug fixing pass 1 | Member B + C | Scope creep | All planned core features implemented |
| **Apr 15 - Apr 24** | Integration hardening + bug fixing pass 2 | All members | Hidden regressions | Stable integrated build with known critical bugs resolved |
| **Apr 25 - May 1** | Demo prep, documentation, rehearsal | All members | Late breakages | Demo script finalized; presentation draft complete |
| **May 2 - May 5** | Final buffer, polish, submission | All members | Deadline compression | Final submission package delivered on time |

**Speaker:** Member D

## Slide 8 - Optional Demo + Close (0:35)
- Demo only one clear flow (no deep debugging during talk).
- Show what currently works now; avoid spending more than ~30 seconds.
- Closing line: team is on track because ownership is clear and timeline includes integration buffers.

**Speaker:** Member A (plus 1-line handoff from each member)

---

## Rubric Alignment Checklist
- **Presentation Clarity (25):** one message per slide, short bullets, clear transitions.
- **Student Effort (20):** concrete artifacts (modules, tests, integration milestones).
- **Comprehensive Timeline (25):** full Jan 20 -> May 5 schedule with owners, risks, and done criteria.
- **Overall Team Understanding (20):** each member presents and explains system-level links.
- **Delivery (10):** balanced speaking split and rehearsal-backed pacing.

