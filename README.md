# ARKium

The desktop sibling to [**ARKster**](https://github.com/Rae-ARK/ARKster) — a local-first, offline reading companion for your own folder of novel `.txt`/`.md` files. No accounts, no cloud sync, no Play Store or app store dependency.

ARKster's Android successor is ARKarium; see [`docs/ANDROID_REFERENCE.md`](docs/ANDROID_REFERENCE.md) for how the three projects compare.

## Stack
- C (C11)
- [GTK 4](https://www.gtk.org/) + libadwaita — native Linux toolkit, no bundled web runtime
- SQLite for favorites and reading progress

## Status
🚧 MVP v0.01 — you can pick a library folder, browse novels and chapters, read, favorite, and resume where you left off. Previous SvelteKit/Capacitor and Neutralino.js/Vue attempts were scrapped in favor of a native GTK shell. See [`docs/ARCHITECTURE.md`](docs/ARCHITECTURE.md) for design details and [`docs/ROADMAP.md`](docs/ROADMAP.md) for what is built and what is next.

## Getting started
See [`docs/GETTING_STARTED.md`](docs/GETTING_STARTED.md).

## License
See [GPLv3 LICENSE](LICENSE).
