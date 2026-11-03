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

## v0.02 — Library feel
- Card/grid Home with Continue Reading section
- Favorites and History views backed by the existing store
- Chapter list: read/unread/current state, local chapter search
- Rescan action; background scanning via `GTask`

## v0.03 — Reader preferences
- Font family/size, line height, paragraph spacing, reading width, theme (incl. sepia), persisted
- Table-of-contents side panel, `Ctrl+F` in-chapter search, auto-hiding header

## Later
- Global search (`Ctrl+K`), metadata (author, genres, tags, synopsis), covers
- Ideas parked in [`FUTURE_IDEAS.md`](FUTURE_IDEAS.md)
