# ARKium — Architecture

## Overview
ARKium is the desktop sibling to [ARKster](https://github.com/Rae-ARK/ARKster) — a local-first, offline reading companion for folders of novel `.txt`/`.md` files. Where ARKster is Android/Kotlin/Compose, ARKium targets desktop (Windows/Linux/macOS) with a lightweight, non-Electron stack.

## Stack
- **Shell**: [Neutralino.js](https://neutralino.js.org/) — native window, filesystem, OS APIs, no bundled Chromium, small binary footprint.
- **Frontend**: Vue 3 + TypeScript + Vite.
- **Client bridge**: `@neutralinojs/lib` — typed bindings to the native Neutralino runtime over local WebSocket. No separate backend server for core features.

## Why no backend server
Neutralino exposes filesystem, OS, and window APIs directly to frontend JS/TS via `@neutralinojs/lib`. For anything beyond that (heavier parsing, background jobs), Neutralino supports **extensions** — an optional Node.js background process the shell can spawn and talk to over WebSocket — keeping the whole stack JS/TS, no second language needed.

## Relationship to ARKster
ARKium is intended to share design language and core concepts (BookSource-style folder scanning, chapter detection heuristics, reading progress) with ARKster, adapted for desktop file-system conventions and a mouse/keyboard UI. Not a code-shared monorepo (yet) — parallel implementation, aligned UX.

## Status
Early scaffold stage. Vue 3 + Vite frontend initialized, Neutralino client lib installed. Native shell wiring and first working window in progress.
