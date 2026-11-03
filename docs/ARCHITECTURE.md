# ARKium — Architecture

## Overview
ARKium is the desktop sibling to [ARKster](https://github.com/Rae-ARK/ARKster) — a local-first, offline reading companion for folders of novel `.txt`/`.md` files. Where ARKster is Android/Kotlin/Compose, ARKium targets desktop (Windows/Linux/macOS) with a lightweight, non-Electron stack.

ARKster's successor, ARKarium (`Rae-ARK/arkarium`), adds author-to-reader sync and is the other Android codebase ARKium is compared against. See [`ANDROID_REFERENCE.md`](ANDROID_REFERENCE.md).

## Stack
- **Language**: C (C11).
- **Toolkit**: [GTK 4](https://www.gtk.org/) + [libadwaita](https://gnome.pages.gitlab.gnome.org/libadwaita/) — native window, widgets, filesystem/OS access via GLib/GIO, no bundled web runtime, no second language for the shell.
- **Storage**: SQLite (library root, favorites, reading progress) at `$XDG_DATA_HOME/arkium/arkium.db`.
- **Build**: a plain `Makefile` driving `cc`/`pkg-config`/`glib-compile-resources`; see [`GETTING_STARTED.md`](GETTING_STARTED.md).
- **Resources**: static assets (CSS, icons) are compiled into the binary via `GResource` (`data/arkium.gresource.xml`), not loaded from loose files at runtime.

## Code layout
- `src/main.c`, `src/window.c` — application and window shell (sidebar + navigation stack).
- `src/app-context.{h,c}` — shared context (window, navigation view, toast overlay, store, current library) passed to page constructors.
- `src/data/` — `novel` (plain C model), `scanner` (folder scan + chapter-detection heuristics), `store` (SQLite).
- `src/pages/` — one file pair per page: home, fiction, chapter, placeholder.

Each `.h` is the public interface of its `.c`; page constructors return `AdwNavigationPage`s and are wired together in `window.c`.

## Why no backend server
GTK/GLib/GIO expose filesystem, OS, and window APIs directly in-process — no local WebSocket bridge needed, and no separate runtime to spawn. For anything genuinely heavy (large-folder scanning, background indexing), that's a `GTask`/thread-pool job inside the same process rather than a second language or an out-of-process extension.

## Relationship to ARKster and ARKarium — Sibling, Not a Fork

ARKster (Android) and ARKium (Desktop) are **separate repos on purpose**. Different interaction models, different resource budgets, different filesystem access patterns — a stretched mobile UI on desktop (or vice versa) is worse than either done natively. They share the underlying novel/library *concepts* (arcs, chapters, reading progress, folder-based sources), not a UI or a codebase. Chapter detection and other heuristics are reimplemented independently in C, not shared as a library.

> Android: "I want to read something."
> Desktop: "I want to manage and read my entire library."

ARKarium is ARKster's successor on Android. Its idea doc (`docs/arkarium/IDEA.md`) lists the planned clients as Android, a desktop "ARKium-derived client", possibly iOS, and an optional web companion, all consuming the same plain-file content format.

### Open decision: what is ARKium relative to ARKarium?
Not decided yet. The answer changes what ARKium needs to build:

- **A. Standalone folder reader** (current scope). Stays a local library manager for a user's own files. Needs only folder-format parity with Android (see below). No sync, no relay.
- **B. Desktop client of the ARKarium ecosystem.** Additionally needs manifest + hash-diff sync from the relay, new-chapter awareness, and eventually text-to-speech. Much larger scope, and it makes licensing of ported ARKarium code a deliberate choice (ARKarium is proprietary, ARKium is GPLv3).

Either way, folder-format parity is the shared first step, because both Android apps already agree on it.

## Status
MVP v0.01: libadwaita shell (sidebar + navigation stack), folder scan, SQLite-backed favorites/progress, and basic Home, Fiction and Chapter pages (see [`GETTING_STARTED.md`](GETTING_STARTED.md), [`ROADMAP.md`](ROADMAP.md) and [`UI_SCREENS.md`](UI_SCREENS.md)). Most of the richer UI in the screen spec is not built yet.

## Reading Modes (design notes, not locked spec)

Early idea worth prototyping now that a basic window is running — desktop has room for a genuinely spatial reader instead of a tall mobile page. (Written during the earlier web-stack attempt; the CSS-grid wording below needs translating to GTK layout equivalents when prototyped.)

- **Flow** — default. Single-column vertical reading, works everywhere, no learning curve.
- **Multi-pane (opt-in)** — chapters as side-by-side containers (≈2 on 16:9, ≈3 on ultrawide via intrinsic CSS `grid-template-columns: repeat(auto-fit, minmax(...))`, not hard breakpoints). Chapter under the cursor receives scroll focus.
- **Focused** — one chapter, generous margins, comfortable typography.
- **Immersive** — minimal/hidden chrome.

Multi-pane hover-scroll is **not the default** — it's a novel interaction pattern with real discoverability, trackpad, and accessibility risk (keyboard-only/screen-reader users need a fully parallel focus model). Ship Flow first, validate multi-pane as an opt-in mode once real usage data exists.

Text width should stay comfortable regardless of viewport — extra horizontal space adds more chapter columns, not longer lines.

## Library Folder Model

Same conceptual shape as ARKster and ARKarium, adapted for desktop's lack of sandboxing:

- **One root library folder**, granted via a plain OS folder picker (no SAF needed on desktop — that's an Android sandboxing requirement, not a design goal in itself).
- Same expected structure underneath: novel subfolders → chapter files, optionally grouped into arc subfolders.
- Files are read/indexed in place — never copied — matching ARKster's approach.
- Chapter-detection heuristics are reimplemented independently in C, and **should behave identically** so a folder that works in one app works unmodified in the other.

### Parity status
That goal is **not yet met**. Checked against the Android scanners, ARKium currently differs on:

- **Arc ordering:** arc folders sort as plain strings (`Arc 10` before `Arc 2`); Android orders by the numeric index in the name.
- **Sort tiers:** Android treats a `~` filename prefix as bonus content and `!` as closing content (afterwords, author's notes), always sorted after regular chapters, with a keyword fallback for unmarked legacy files. ARKium has no tiers.
- **Encoding:** Android falls back from UTF-8 to ISO-8859-1; ARKium replaces invalid bytes with U+FFFD.
- **Metadata:** ARKium ignores covers, per-novel `metadata.json` and the `authors/` folder.

The full comparison table is in [`ANDROID_REFERENCE.md`](ANDROID_REFERENCE.md); the fix order is in [`ROADMAP.md`](ROADMAP.md).

**Desktop-only affordances (not required for v1, no sandbox constraint forcing single-root):**
- Multiple root folders (e.g. separate "Library" and "Imports" locations, or an external drive) — deferred, not blocked by anything architectural.
- Drag-and-drop a folder onto the window as an alternative to the picker dialog.

**Why this matters beyond consistency:** a shared on-disk folder/novel/arc/chapter shape is what makes the deferred v2 LAN sync idea (see `FUTURE_IDEAS.md`) tractable later — "sync this novel" becomes "copy these files," not a format-translation problem between two apps.
