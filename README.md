# Paper Drums

A small JUCE/C++ VST3 drum instrument made from paper foley. The editor is drawn like a warm, hand-sketched sheet of paper. Click a drawing or send MIDI and the matching part glows while the sound plays.

## What is included

- Real VST3 instrument source for Windows.
- Optional Standalone target for quick testing.
- Five paper-based pads:
  - **KICK** — paper handling, pitched down for body.
  - **SNARE** — paper handling, shorter and brighter.
  - **HAT** — pencil on paper, short and pitched up.
  - **TEAR** — torn paper.
  - **CRUMPLE** — crumpled paper.
- MIDI notes: C1 / 36 Kick, D1 / 38 Snare, F#1 / 42 Hat, A#1 / 46 Tear, C#2 / 49 Crumple.
- The four WAV assets are CC0 / public-domain-equivalent releases. See `CREDITS.md`.

## Build on Windows

Install:

1. Visual Studio 2022 with **Desktop development with C++**.
2. CMake 3.22 or newer.
3. Git, because CMake fetches JUCE 8.0.8 at configure time.

From **x64 Native Tools Command Prompt for VS 2022**:

```bat
cd PaperDrums
cmake -B build -G "Visual Studio 17 2022" -A x64
cmake --build build --config Release
```

The VST3 is copied after the build. The exact folder is printed by CMake and is normally under:

```text
build\PaperDrums_artefacts\Release\VST3\Paper Drums.vst3
```

If your DAW does not see it, copy that bundle to:

```text
C:\Program Files\Common Files\VST3
```

Then rescan plugins in the DAW. The Standalone executable is useful for checking the design and MIDI mapping without opening a DAW.

## Build automatically on GitHub

This repository includes `.github/workflows/build-windows.yml`. Push the project contents to GitHub, open the **Actions** tab, wait for **Build Paper Drums VST3 for Windows** to finish, and download the `PaperDrums-VST3-Windows` artifact. The Persian step-by-step guide is in `GITHUB_BUILD_FA.md`.

If you push a tag such as `v0.1.0`, the workflow also attaches the Windows VST3 ZIP to a GitHub Release.

## Offline JUCE build

If the build machine cannot access GitHub, download JUCE 8.0.8 separately and configure with:

```bat
cmake -B build -G "Visual Studio 17 2022" -A x64 -DJUCE_SOURCE_DIR=C:/path/to/JUCE
cmake --build build --config Release
```

## Design notes

The audio engine uses a small pool of polyphonic voices. Each pad is a short, pitched and time-limited slice of the source recording, so long foley recordings behave like playable drum hits. Missing or silent source files do not crash the plugin; the plugin simply produces silence for that pad.

The first version intentionally keeps the interface simple: there are no hidden menus or dense controls. The page is the instrument.
