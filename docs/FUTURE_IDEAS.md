# Future Ideas (unscheduled, not committed to any version)

Ideas discussed and deliberately deferred. Nothing here is a spec — just
a memory aid for "we thought about this, here's the shape of it."

## v2: LAN Sync ("iTunes for novels")

**Problem it solves:** people hoard novels; mobile storage fills up;
desktop storage is cheap. Let desktop be the full library, let mobile
hold a curated offline subset. This applies to **both** Android
siblings, not just ARKster: ARKarium carries over ARKster's storage
and folder model (see `ANDROID_REFERENCE.md`), so a novel ARKarium
pulled from the author's relay sits on disk the same way one ARKster
indexed from a hand-assembled folder does. ARKium doesn't need a
separate code path per app — it's the same backup/sync target either
way, and a library built or synced on either phone app can be backed
up to desktop and restored from it.

**Core idea:** ARKium (desktop) can optionally run a local HTTP server,
LAN-only, no cloud, no accounts — same trust model as the rest of the
project. ARKster and ARKarium (mobile) discover it and pull what the
user wants to have offline, or push a library up to desktop as a
backup.

**Status reality check:** neither Android app supports this today.
ARKster's own README says "no accounts, no syncing" as of v0.2.
ARKarium has a *different* sync (author-to-reader, via its relay) but
nothing resembling a phone-to-desktop backup channel, and its own
planning docs list its stated flagship feature (character-aware TTS)
as entirely unbuilt — so treat "the Android side of this ecosystem" as
early and the plan below as a direction, not a schedule.

**Open questions to resolve when this is actually scheduled:**

- **Discovery:** zero-config via mDNS/Bonjour
  (`_arkium._tcp.local`-ish). Android: `NsdManager`. Desktop: Avahi
  (`libavahi-client`), the standard Linux mDNS stack. No manual IP
  entry.
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
  it'll be implemented independently in Kotlin (ARKster) and C
  (ARKium) and needs to stay in sync between the two.

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
ping) stays up cheaply; the real sync backend (a small `libsoup`
HTTP server in-process, or a spawned helper binary) only spawns on
an actual sync request, then self-terminates after an idle timeout.

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

## Relation to ARKarium's sync

The LAN-sync idea above now targets **both** Android siblings, ARKium↔ARKster and ARKium↔ARKarium — see the status note above for why that's one feature, not two.

ARKarium separately has its own, already-working sync: it fetches a manifest from the author's relay, diffs SHA-256 hashes, downloads only what changed, then runs the normal folder scan. That one is author-to-reader (content flows from a publisher to people reading that author's novels), where LAN sync is desktop-to-phone (content flows between one person's own devices, as a backup). The two are complementary, not the same feature: ARKarium keeps doing its own relay sync to *get* an author's novels in the first place; ARKium's LAN sync is about *backing up and offloading* whatever is already in a phone's library, regardless of how it got there. The relay-sync mechanism itself (manifest/hash-diff from a publisher) stays ARKarium's feature and out of scope for ARKium (see [`ARCHITECTURE.md`](ARCHITECTURE.md#decision-arkium-is-a-general-purpose-reader)).

Worth remembering for LAN sync:
- Both approaches lean on the same property: the on-disk format is plain files in a shared folder shape, so syncing is file copying. That is another reason to keep ARKium's scanner in step with the Android scanners.
- Reading progress sync (bidirectional, last-write-wins in the sketch above) has no equivalent in ARKarium's sync today.
- ARKarium's design is summarized in [`ANDROID_REFERENCE.md`](ANDROID_REFERENCE.md) if anything in it is useful as prior art for the LAN protocol spec.
