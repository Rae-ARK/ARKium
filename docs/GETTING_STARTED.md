# Getting Started

## Prerequisites
- A C11 toolchain (`gcc` or `clang`) and `make`
- `pkg-config`
- GTK 4 development files (Debian/Ubuntu: `libgtk-4-dev`; Fedora: `gtk4-devel`; Arch: `gtk4`)
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
