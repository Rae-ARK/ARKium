# Android Reference: ARKster & ARKarium

A snapshot of what the two Android codebases do, taken so ARKium can borrow ideas on purpose instead of by accident. **This is a reference, not a spec.** It describes other projects' code as of the snapshot date; re-check upstream before relying on any detail.

- Snapshot date: 2026-11-03
- ARKster: `Rae-ARK/ARKster`, v0.2, commit `5cf252a`
- ARKarium: `Rae-ARK/arkarium`, commit `b4fcc2f` (last commit 2026-10-14)

## The three projects

| | ARKster | ARKarium | ARKium |
|---|---|---|---|
| Platform | Android (Kotlin, Compose) | Android (Kotlin, Compose) | Desktop (C, GTK 4, libadwaita) |
| Audience | Anyone with a folder of novels | Readers of Rae ARK's novels | Anyone with a folder of novels |
| Role | General-purpose local library + reader | Author-to-reader distribution and reader for one author's novels | General-purpose local library + reader |
| License | GPLv3 | Proprietary, all rights reserved | GPLv3 |
| Storage | Room + DataStore | Room + DataStore | SQLite |
| Sync | None (by design) | Manifest + SHA-256 diff from one fixed relay | None |
| Tests | None found | Scanner, theme, settings view-model | None |
| Read-event stats (streak, weekly count) | No | Yes | No |

ARKster and ARKium are the general readers. ARKarium is a separate app for readers of Rae ARK's novels; its README describes it as built around direct distribution from the author to the reader. ARKarium's own docs say its scanner logic (sort tiers, rescan safety, author pages) was carried over from ARKster. Its idea doc also mentions a possible desktop client derived from ARKium — that's ARKarium's own future aspiration, not something ARKium commits to becoming. ARKium's scope conclusion (no author-distribution features) follows from this; see [`ARCHITECTURE.md`](ARCHITECTURE.md#decision-arkium-is-a-general-purpose-reader).

That conclusion is about ARKarium's *relay* sync specifically (author → reader). It does not rule out ARKium acting as a sync/backup hub for either Android app's *existing* library — see [`FUTURE_IDEAS.md`](../proposal/FUTURE_IDEAS.md#v2-lan-sync-itunes-for-novels). Worth being blunt about: none of that exists yet, and neither does much of what the Android apps' own docs plan for themselves. As of this snapshot: ARKster's README states it has no syncing at all; ARKarium's own planning docs (`docs/arkarium/FUTURE_PLANS.md`) list its stated "killer feature," character-aware TTS, as having no code anywhere in `app/src`, and multi-relay fallback / background sync / auth are all "explicitly punted" in its sync design doc. Treat everything below as a snapshot of two early, actively-shifting codebases, not a stable target.

## Folder conventions: Android vs ARKium

ARKium's goal (see ARCHITECTURE.md, "Library Folder Model") is that a library folder works unmodified across the apps. Checked against the code, it does not yet. Every convention in the "Android" column below is present in both ARKster and ARKarium. "Differs" means the same folder produces a different result in ARKium today.

| Convention | Android | ARKium | Status |
|---|---|---|---|
| One root folder; subfolders are novels | Yes | Yes | Match |
| Chapter files are `.txt` / `.md` | Yes | Yes | Match |
| Leading number in filename, e.g. `001 Title.txt` | Yes | Yes (also `Chapter N`) | Match |
| Underscore/dash tokenizing for titles | Yes | Yes | Match |
| Arc subfolders | Recognized by name pattern: `Arc`, `Volume`, `Book`, `Part` + optional number, plus CJK equivalents | Every subfolder is treated as an arc | Differs (unverified impact on real libraries) |
| Arc ordering | By the numeric arc index in the folder name | Plain string comparison, so `Arc 10` sorts before `Arc 2` | **Differs** |
| Sort tiers: `~` prefix = bonus, `!` prefix = closing | Yes; marker is stripped from the displayed title, optional number after it still orders within the tier | No | **Differs** |
| Legacy tier keywords (`interlude`, `side story`, `omake`, `afterword`, `author's note`, ...) | Yes, used only when no marker is present | No | **Differs** |
| Text encoding | UTF-8, falling back to ISO-8859-1 | `g_utf8_make_valid` (invalid bytes become U+FFFD) | Differs: Latin-1 files show replacement characters |
| Cover images (`cover.{jpg,png,webp}`, per-novel and per-arc) | Yes | No | Missing |
| Per-novel `metadata.json` | Yes (optional) | No | Missing |
| `authors/<id>.json` at library root, with avatar | Yes | No | Missing |
| Manual chapter title / position overrides | Yes, persisted separately so rescans never erase them | No | Missing |

Sort tiers matter most. Without them an author's afterword or interlude lands wherever its filename sorts, and the order differs between devices.

## Data model comparison

Both Android apps keep separate tables for novels, authors, arcs, chapters (with `number` and `sort_tier`), chapter overrides, reading progress (a `position` plus a `position_type`, default `PERCENTAGE`), and scan fingerprints. ARKarium additionally has chapter read events (chapter, novel, calendar day, timestamp) and synced files.

ARKium's store has three tables: `settings`, `favorites`, and `progress` (one row per novel: last chapter path + scroll fraction). The novel and chapter lists are rebuilt in memory on every scan and keyed by absolute path, which is why progress survives a rescan today.

What is missing for the features on the roadmap:

- **Per-chapter read state** (ARKarium only; ARKster has no equivalent). ARKarium records a read event when a chapter is left past a **0.9 scroll threshold** (deliberately not 1.0, since readers often skip a final author's note). ARKium would need something similar to show read/unread in the chapter list.
- **Reading stats** (ARKarium only). Its Home shows a read-every-day streak and chapters read in the last 7 days, computed from read events using local calendar days (not UTC, and day-key arithmetic that is safe across DST and month ends).
- **Scan fingerprints** (both apps): per-novel last-modified/size/file-count so an incremental rescan can skip unchanged novels.

## Lessons from ARKster's bug log (`docs/bugs.md` upstream)

- **Bug 2, afterword/interlude ordering:** the origin of sort tiers. Explicit filename markers are authoritative; keyword guessing is only a fallback for old libraries.
- **Bug 3, covers:** the reader should fall back arc cover, then fiction cover, then placeholder.
- **Bug 4, rescans making novels disappear:** a rescan must reconcile (upsert what changed, remove only what is genuinely gone), never wipe and rebuild, and user overrides must survive. ARKium's current in-memory rescan avoids this by not persisting the library, but will hit it the moment a library table or overrides exist.

## UI ideas worth borrowing

Home:
- Continue Reading cards (cover, chapter, progress, Continue action)
- A featured section
- A reading-stats card: streak and chapters this week (ARKarium only)

Novel page:
- Tabbed "All Chapters" and per-arc views
- Chapter search
- Zebra striping
- Bonus / Extra tags on tiered chapters
- Author page linked from the novel and chapter pages

Reader:
- A Reader Preferences control: font size (12 to 28, default 18), line spacing, Light / Sepia / Dark
- Tap to toggle controls (immersive mode)
- "About the author" card at the end of a chapter
- Author-note callout (ARKarium only)
- Keep screen on while reading aloud (ARKarium only, tied to text-to-speech)

ARKium's reader already uses 18px as its fixed default, which matches Android's default.

## ARKarium-only features

These serve ARKarium's purpose (delivering one author's novels to his readers) and are **out of scope for ARKium**. They are listed for reference, and because a few ideas might translate to a general reader.

- **Relay sync:** fetch a manifest, diff SHA-256 hashes, download only changed files, delete stale ones, then run the normal folder scan. Single fixed relay, resolved from a fiction name. Multi-relay fallback, auth and background sync are designed but not built. Known robustness issues are listed in ARKarium's `docs/arkarium/NEXT_FIXES.md`. (This is the author→reader mechanism specifically — ARKium's own planned desktop↔phone *backup* sync, in `FUTURE_IDEAS.md`, is a different feature and targets ARKarium's library too, not just ARKster's.)
- **New-chapter notifications:** a background worker that checks synced novels.
- **Text-to-speech:** described in ARKarium's docs as the intended flagship feature, with character-aware multi-voice reading planned. Per its docs this is in progress, not finished.
- **Because the content format is plain files**, much of this stays compatible with ARKium's folder model if ARKium ever wants to consume it.

## Licensing note

ARKium and ARKster are GPLv3; ARKarium is proprietary. The same author owns all three, so reusing one's own code is a legal non-issue. Ideas and file conventions can be adopted freely. The practical consequence for code is that anything ported from ARKarium into ARKium is published under GPLv3, so do that deliberately. ARKster is GPLv3 already, so it is the natural source for ported code.

## Suggested adoption order

A proposal, not a commitment. It goes from cheapest and most correctness-critical to most involved.

1. **Scanner parity:** numeric arc ordering, `~` / `!` sort tiers with the legacy keyword fallback, encoding fallback. Add a small scanner test fixture (a temp folder tree plus `g_test`) so parity stays checkable.
2. **Per-chapter read events** with the 0.9 threshold, then read/unread styling in the chapter list.
3. **Covers and metadata files:** `cover.*`, `metadata.json`, `authors/`. Prerequisite for the Authors page and card-style Home.
4. **Reader preferences** (already planned as v0.03).

Not on the list: relay sync, new-chapter notifications and character-aware TTS. Those are ARKarium's job (see ARCHITECTURE.md).
