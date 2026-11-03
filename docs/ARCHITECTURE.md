# ARKium — Architecture

## Overview
ARKium is the desktop sibling to [ARKster](https://github.com/Rae-ARK/ARKster) — a local-first, offline reading companion for folders of novel `.txt`/`.md` files. Where ARKster is Android/Kotlin/Compose, ARKium targets desktop (Windows/Linux/macOS) with a lightweight, non-Electron stack.

## Stack
- **Language**: C (C11).
- **Toolkit**: [GTK 4](https://www.gtk.org/) — native window, widgets, filesystem/OS access via GLib/GIO, no bundled web runtime, no second language for the shell.
- **Build**: a plain `Makefile` driving `cc`/`pkg-config`/`glib-compile-resources`; see [`GETTING_STARTED.md`](GETTING_STARTED.md).
- **Resources**: static assets (CSS, icons) are compiled into the binary via `GResource` (`data/arkium.gresource.xml`), not loaded from loose files at runtime.

## Why no backend server
GTK/GLib/GIO expose filesystem, OS, and window APIs directly in-process — no local WebSocket bridge needed, and no separate runtime to spawn. For anything genuinely heavy (large-folder scanning, background indexing), that's a `GTask`/thread-pool job inside the same process rather than a second language or an out-of-process extension.

## Relationship to ARKster
ARKium is intended to share design language and core concepts (BookSource-style folder scanning, chapter detection heuristics, reading progress) with ARKster, adapted for desktop file-system conventions and a mouse/keyboard UI. Not a code-shared monorepo (yet) — parallel implementation, aligned UX.

## Status
Early scaffold stage. GTK 4 shell initialized with a first working window and build pipeline (see [`GETTING_STARTED.md`](GETTING_STARTED.md)). Library/reader UI (see [`UI_SCREENS.md`](UI_SCREENS.md)) not yet implemented.

## Relationship to ARKster — Sibling, Not a Fork

ARKster (Android) and ARKium (Desktop) are **separate repos on purpose**. Different interaction models, different resource budgets, different filesystem access patterns — a stretched mobile UI on desktop (or vice versa) is worse than either done natively. They share the underlying novel/library *concepts* (arcs, chapters, reading progress, folder-based sources), not a UI or a codebase.

> Android: "I want to read something."
> Desktop: "I want to manage and read my entire library."

## Reading Modes (design notes, not locked spec)

Early idea worth prototyping once a basic window is running — desktop has room for a genuinely spatial reader instead of a tall mobile page:

- **Flow** — default. Single-column vertical reading, works everywhere, no learning curve.
- **Multi-pane (opt-in)** — chapters as side-by-side containers (≈2 on 16:9, ≈3 on ultrawide via intrinsic CSS `grid-template-columns: repeat(auto-fit, minmax(...))`, not hard breakpoints). Chapter under the cursor receives scroll focus.
- **Focused** — one chapter, generous margins, comfortable typography.
- **Immersive** — minimal/hidden chrome.

Multi-pane hover-scroll is **not the default** — it's a novel interaction pattern with real discoverability, trackpad, and accessibility risk (keyboard-only/screen-reader users need a fully parallel focus model). Ship Flow first, validate multi-pane as an opt-in mode once real usage data exists.

Text width should stay comfortable regardless of viewport — extra horizontal space adds more chapter columns, not longer lines.

## Library Folder Model

Same conceptual shape as ARKster, adapted for desktop's lack of sandboxing:

- **One root library folder**, granted via a plain OS folder picker (no SAF needed on desktop — that's an Android sandboxing requirement, not a design goal in itself).
- Same expected structure underneath: novel subfolders → chapter files, optionally grouped into arc subfolders.
- Files are read/indexed in place — never copied — matching ARKster's approach.
- Chapter-detection heuristics (leading numbers, `Chapter N` prefixes, filename tokenization fallback) are reimplemented independently in C, not shared as a library (ARKster and ARKium are separate codebases by design — see "Relationship to ARKster" above), but should behave identically so a folder that works in one works unmodified in the other.

**Desktop-only affordances (not required for v1, no sandbox constraint forcing single-root):**
- Multiple root folders (e.g. separate "Library" and "Imports" locations, or an external drive) — deferred, not blocked by anything architectural.
- Drag-and-drop a folder onto the window as an alternative to the picker dialog.

**Why this matters beyond consistency:** a shared on-disk folder/novel/arc/chapter shape is what makes the deferred v2 LAN sync idea (see `FUTURE_IDEAS.md`) tractable later — "sync this novel" becomes "copy these files," not a format-translation problem between two apps.
