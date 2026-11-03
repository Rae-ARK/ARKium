# Getting Started

## Prerequisites
- A C11 toolchain (`gcc` or `clang`) and `make`
- `pkg-config`
- GTK 4 development files (Debian/Ubuntu: `libgtk-4-dev`; Fedora: `gtk4-devel`; Arch: `gtk4`)
- libadwaita development files (Debian/Ubuntu: `libadwaita-1-dev`; Fedora: `libadwaita-devel`; Arch: `libadwaita`) — 1.5 or newer
- SQLite development files (Debian/Ubuntu: `libsqlite3-dev`; Fedora: `sqlite-devel`; Arch: `sqlite`)
- `glib-compile-resources` (ships with `libglib2.0-dev` / `glib2-devel`, usually already pulled in by the GTK dev package)

## Build
```bash
make
```

## Run
```bash
make run
# or directly:
./build/arkium
```

## Install (optional)
Installs the binary, `.desktop` entry and app icon under `PREFIX` (defaults to `/usr/local`):
```bash
sudo make install
```

## Clean
```bash
make clean
```

## Using it
1. Run ARKium and click **Open Library Folder**. Pick a folder whose immediate subfolders are novels (optionally with arc subfolders holding `.txt` / `.md` chapter files). Files are read in place, never copied.
2. Open a novel, then **Start Reading** / **Continue Reading**.
3. In the reader: `←` / `→` switch chapters; scroll position is saved automatically.

Data (library root, favorites, progress) lives in `$XDG_DATA_HOME/arkium/arkium.db` (default `~/.local/share/arkium/arkium.db`).
