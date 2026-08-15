# Future Ideas (unscheduled, not committed to any version)

Ideas discussed and deliberately deferred. Nothing here is a spec — just
a memory aid for "we thought about this, here's the shape of it."

## v2: LAN Sync ("iTunes for novels")

**Problem it solves:** people hoard novels; mobile storage fills up;
desktop storage is cheap. Let desktop be the full library, let mobile
hold a curated offline subset.

**Core idea:** ARKium (desktop) can optionally run a local HTTP server,
LAN-only, no cloud, no accounts — same trust model as the rest of the
project. ARKster (mobile) discovers it and pulls what the user wants
to have offline.

**Open questions to resolve when this is actually scheduled:**

- **Discovery:** zero-config via mDNS/Bonjour
  (`_arkium._tcp.local`-ish). Android: `NsdManager`. Desktop: a Node
  extension under Neutralino. No manual IP entry.
- **Trust/pairing:** LAN is not automatically safe (other devices share
  the subnet). First-time pairing via PIN-on-screen or QR code, then a
  stored token/cert for automatic future syncs. No silent trust of any
  device that finds the server.
- **Sync direction:**
  - Novel content: desktop → mobile (desktop is the master library).
  - Reading progress: bidirectional. v1 conflict rule: most-recently-
    updated-timestamp wins per novel. No real CRDT merge logic needed
    yet.
- **Mobile storage UX:** per-novel "synced offline" vs "available on
  demand" state, with an easy "remove local copy, keep on desktop"
  action — this *is* the iTunes/iPod-equivalent UX, it's the actual
  point of the feature.
- **Server lifecycle:** spin up only when ARKster is actively
  requesting, not always-on — battery/resource conscious, consistent
  with the project's "runs when needed" ethos.
- **Shared protocol:** once both apps have working basic scaffolds,
  this needs its own short spec doc (wire format / API shape) since
  it'll be implemented independently in Kotlin (ARKster) and
  TypeScript (ARKium) and needs to stay in sync between the two.

**Status:** intentionally not started. Get "it just works" basics
solid in both apps first.

## Reading Modes (see ARCHITECTURE.md for the fuller version)

Flow (default) → Multi-pane (opt-in, hover-scroll, needs real UX
validation) → Focused → Immersive. Multi-pane specifically needs
prototyping before it's trusted as anything more than an experiment —
discoverability, trackpad behavior, and keyboard/screen-reader parity
are all unresolved.

### Lifecycle refinement: two-tier wake, user's choice on background scope

Two-tier pattern, similar in spirit to systemd socket activation /
inetd: a near-zero-footprint listener (mDNS advertise + handshake
ping) stays up cheaply; the real sync backend (Node extension running
an HTTP server) only spawns on an actual sync request, then
self-terminates after an idle timeout.

**Deliberately deferred, not decided:** whether Tier 1 needs to
survive when the ARKium window is closed (OS-level autostart/tray
process, so a phone can wake it without the desktop app visibly open)
vs. only running while the app is open.

**Resolution: make it a setting, not an architectural default.**
Some users are fine opening the app before a sync; others want
always-available background sync. Ship "runs while app is open" as
the simpler default, expose "run in background / start on login" as
an opt-in toggle for people who want it. Avoids forcing a
tray-icon/autostart footprint on everyone just to support the subset
who want zero-friction background sync.
