# Roadmap

Scope is deliberately small per release. [`UI_SCREENS.md`](UI_SCREENS.md) describes the long-term target; this file says what is actually built.

## v0.01 — MVP (current)
Done:
- libadwaita shell: persistent sidebar + `AdwNavigationView` content stack, collapses on narrow windows
- Library folder picker, persisted; scan of novels / arcs / `.txt` + `.md` chapters (read in place)
- Chapter ordering and titles from filename heuristic (leading number, `Chapter N`, fallback tokenizing)
- **Home**: flat novel list
- **Fiction**: title, chapter count, Start/Continue Reading, favorite toggle, flat chapter list with arc subtitles
- **Chapter**: constrained-width reader, Previous/Next, `←`/`→` keys, scroll-fraction progress saved and restored
- SQLite store for library root, favorites, progress
- Sidebar entries for Favorites, History, Authors, Genres, Tags as honest placeholders

Known gaps / limitations:
- Sidebar stays visible while reading on libadwaita < 1.7 (hiding it needs `show-sidebar`)
- Progress is per novel (last chapter + scroll fraction); no per-chapter read state yet
- No rescan on file changes; reopen the folder to refresh
- Scanning runs on the main thread (fine for small libraries)
- Folder-format parity with the Android apps is incomplete: arc folders sort as plain strings, no `~`/`!` sort tiers, lossy text-encoding handling, no covers or metadata files (details in [`ARCHITECTURE.md`](ARCHITECTURE.md#parity-status) and [`ANDROID_REFERENCE.md`](ANDROID_REFERENCE.md))

## v0.02 — Parity and library feel
Proposed order; cheapest and most correctness-critical first. Rationale for each item is in [`ANDROID_REFERENCE.md`](ANDROID_REFERENCE.md).

1. **Scanner parity**
   - numeric arc ordering
   - `~` / `!` sort tiers, plus the legacy keyword fallback
   - UTF-8 → ISO-8859-1 encoding fallback in the reader
   - a scanner test fixture (temp folder tree + `g_test`) so parity can be checked
2. **Rescan action and background scanning** via `GTask`
3. **Per-chapter read events** (counted as read past a 0.9 scroll fraction) with read/unread/current styling and local chapter search in the chapter list
4. **Favorites and History views** backed by the existing store
5. **Card/grid Home** with a Continue Reading section; this means moving `ArkiumNovel` from plain structs to GObjects backing a `GListStore`

## v0.03 — Reader preferences
- Font family/size, line height, paragraph spacing, reading width, theme (incl. sepia), persisted
- Table-of-contents side panel, `Ctrl+F` in-chapter search, auto-hiding header

## v0.04 — Metadata
- Covers (`cover.*`, per-arc fallback to per-novel to placeholder)
- Per-novel `metadata.json` and `authors/<id>.json`, same shape as the Android apps
- Authors / Genres / Tags sidebar pages become real
- Reading stats card on Home (streak, chapters this week), built on the v0.02 read events

## Later
- Global search (`Ctrl+K`), including chapter titles
- Manual chapter title/position overrides that survive rescans
- Ideas parked in [`FUTURE_IDEAS.md`](FUTURE_IDEAS.md)

## Open decisions
- **Standalone reader, or desktop client of the ARKarium ecosystem?** Decides whether sync, new-chapter awareness and text-to-speech ever land in ARKium. See [`ARCHITECTURE.md`](ARCHITECTURE.md#open-decision-what-is-arkium-relative-to-arkarium). Nothing in v0.02 to v0.04 depends on the answer.
