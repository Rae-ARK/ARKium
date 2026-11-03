# ARKium Docs

Docs are organized by lifecycle stage, not by topic. When writing or editing
a doc, ask where the *content* currently lives, not where it will end up:

| Directory | Stage | Contains |
|---|---|---|
| [`foundation/`](foundation/INDEX.md) | Permanent | What ARKium is and how it's built — stays true until deliberately changed |
| [`proposal/`](proposal/INDEX.md) | Under consideration | Ideas discussed and deferred, not committed to any version |
| [`implementation/`](implementation/INDEX.md) | In-flight | What's being staged and built right now, release by release |

A typical lifecycle: an idea starts in `proposal/`, gets scheduled and moves
into `implementation/` (usually into `ROADMAP.md`), and once it ships and
stabilizes, its lasting description (not its history) gets folded into
`foundation/`. Each subdirectory has its own `INDEX.md`; keep it current when
adding or removing docs there.
