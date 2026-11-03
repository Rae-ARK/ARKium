# ARKium UI Screen Specifications

ARKium is the desktop/web-oriented counterpart to ARKster. Its UI should retain ARKster's novel-centric information architecture while being designed specifically for large screens, keyboard/mouse interaction, persistent navigation, and higher information density.

# ARKium UI Screen Specifications
ARKium is the desktop/web-oriented counterpart to ARKster. Its UI should retain ARKster's novel-centric information architecture while being designed specifically for large screens, keyboard/mouse interaction, persistent navigation, and higher information density.
The following screens define the initial core experience:
1. Home Page
2. Fiction Page
3. Chapter Page
The UI should not attempt to reproduce Royal Road's desktop layout directly. Royal Road is used as an information architecture reference, while ARKium should use a desktop-native information hierarchy.
---
# 1. Home Page
## Purpose
The Home Page is the user's primary library and discovery surface.
It should immediately answer:
- What was I reading?
- What should I read next?
- What novels are available?
- What has recently changed?
- Where is my library?
The page should prioritize the user's own reading activity over generic discovery.
## Layout
```text
┌──────────────────────────────────────────────────────────────────────────────┐
│ ARKium                  Search novels, authors, tags...             ⚙  ◐  │
├───────────────┬──────────────────────────────────────────────────────────────┤
│               │                                                              │
│  HOME         │  Continue Reading                                            │
│               │                                                              │
│  Library      │  ┌────────────┐ ┌────────────┐ ┌────────────┐               │
│  Favorites    │  │            │ │            │ │            │               │
│  History      │  │   COVER    │ │   COVER    │ │   COVER    │               │
│               │  │            │ │            │ │            │               │
│  Authors      │  └────────────┘ └────────────┘ └────────────┘               │
│  Genres       │    Novel Title     Novel Title     Novel Title              │
│  Tags         │    Ch. 18          Ch. 07          Ch. 42                   │
│               │    64%              31%              89%                     │
│               │                                                              │
│               │  Recently Updated                                            │
│               │                                                              │
│               │  ┌──────────────────────────────────────────────────────┐    │
│               │  │ COVER │ Novel Title                         Updated  │    │
│               │  │       │ Author                              2h ago   │    │
│               │  ├──────────────────────────────────────────────────────┤    │
│               │  │ COVER │ Another Novel                       Updated  │    │
│               │  │       │ Author                              5h ago   │    │
│               │  └──────────────────────────────────────────────────────┘    │
│               │                                                              │
│               │  Recently Added                                              │
│               │                                                              │
│               │  [Cover] [Cover] [Cover] [Cover] [Cover]                    │
│               │                                                              │
└───────────────┴──────────────────────────────────────────────────────────────┘
````
## Navigation
The left sidebar remains persistent on desktop.
### Primary navigation
* Home
* Library
* Favorites
* History
### Discovery navigation
* Authors
* Genres
* Tags
### Utility
* Settings
* Theme
* Application information
The sidebar should be collapsible.
Collapsed:
```text
┌────┐
│ AR │
├────┤
│ ⌂  │
│ ▣  │
│ ♥  │
│ ◷  │
│    │
│ A  │
│ #  │
│ T  │
└────┘
```
Expanded:
```text
┌───────────────┐
│ ARKium        │
├───────────────┤
│ Home          │
│ Library       │
│ Favorites     │
│ History       │
│               │
│ Authors       │
│ Genres        │
│ Tags          │
└───────────────┘
```
## Header
The header should contain:
* ARKium branding
* Global search
* Settings
* Theme control
* User/application state if required
Search should support:
* Fiction titles
* Authors
* Tags
* Genres
* Chapter titles
Keyboard shortcut:
```text
Ctrl + K
```
opens global search.
## Continue Reading
This is the highest-priority content section.
Each card should display:
* Cover
* Fiction title
* Author
* Current chapter
* Reading progress
* Last-read time
* Continue action
Example:
```text
┌────────────────────┐
│                    │
│       COVER        │
│                    │
├────────────────────┤
│ Summoned By        │
│ Mistake            │
│                    │
│ Chapter 18         │
│ ███████████░░ 72%  │
│                    │
│ [ Continue ]       │
└────────────────────┘
```
The progress indicator should represent the user's reading position within the fiction or chapter, depending on the implementation.
## Recently Updated
This section should favor information density over large cards.
Each entry:
* Cover thumbnail
* Fiction title
* Author
* Latest chapter
* Update time
* Reading state
Clicking the entry opens the Fiction Page.
## Recently Added
A horizontal card grid.
Desktop should allow more items to be visible simultaneously than the Android application.
Cards should remain visually compact enough that a large library does not become an endless wall of giant rectangles.
---
# 2. Fiction Page
## Purpose
The Fiction Page is the central information and navigation page for a novel.
It should provide enough information for the user to decide whether to read while also providing immediate access to chapters.
## Layout
```text
┌──────────────────────────────────────────────────────────────────────────────┐
│ ARKium                  Search novels, authors, tags...             ⚙  ◐  │
├───────────────┬──────────────────────────────────────────────────────────────┤
│               │                                                              │
│  HOME         │  ← Back                                                      │
│  Library      │                                                              │
│  Favorites    │  ┌──────────────┐  Novel Title                               │
│  History      │  │              │  Author                                    │
│               │  │    COVER     │  ★ Favorite                                │
│  Authors      │  │              │                                            │
│  Genres       │  └──────────────┘  Genres · Tags                             │
│  Tags         │                                                              │
│               │  [ Continue Reading ]  [ Start Reading ]                     │
│               │                                                              │
│               │  Synopsis                                                    │
│               │  ──────────────────────────────────────────────────────────  │
│               │  Fiction description goes here. The description may span     │
│               │  multiple paragraphs and should remain readable at desktop  │
│               │  widths without becoming excessively wide.                  │
│               │                                                              │
│               │  Statistics                                                  │
│               │  Chapters     Words       Status       Updated               │
│               │  124          420k        Ongoing      2h ago                │
│               │                                                              │
│               │  Chapters                                                    │
│               │  ┌──────────────────────────────┬────────────────────────┐  │
│               │  │ Arc 1                        │ Search chapters...     │  │
│               │  ├──────────────────────────────┴────────────────────────┤  │
│               │  │ Chapter 001                         12 Jan 2026       │  │
│               │  │ Chapter 002                         13 Jan 2026       │  │
│               │  │ Chapter 003                         15 Jan 2026       │  │
│               │  │ Chapter 004                         18 Jan 2026       │  │
│               │  ├───────────────────────────────────────────────────────┤  │
│               │  │ Arc 2                                                     │
│               │  ├───────────────────────────────────────────────────────┤  │
│               │  │ Chapter 005                         20 Jan 2026       │  │
│               │  │ Chapter 006                         22 Jan 2026       │  │
│               │  └───────────────────────────────────────────────────────┘  │
│               │                                                              │
└───────────────┴──────────────────────────────────────────────────────────────┘
```
## Fiction Header
The header should contain:
* Cover
* Title
* Author
* Genres
* Tags
* Status
* Favorite control
* Continue Reading button
* Start Reading button
The primary action should be visually dominant.
### Primary actions
```text
[ Continue Reading ]
```
when reading progress exists.
Otherwise:
```text
[ Start Reading ]
```
Secondary action:
```text
[ ♡ Favorite ]
```
## Metadata
Display important metadata in a compact row.
Example:
```text
Chapters     Words      Status       Updated
124          420k       Ongoing      2h ago
```
Additional metadata can include:
* Publication date
* Last chapter
* Language
* Local library path
* Tags
* Genres
Metadata should not overwhelm the synopsis.
## Synopsis
The synopsis should be readable without forcing the user into a modal.
Use a constrained text width.
```text
┌───────────────────────────────────────────────┐
│ Synopsis                                      │
│                                               │
│ Long-form fiction description...              │
│                                               │
│ More text...                                  │
└───────────────────────────────────────────────┘
```
If the synopsis exceeds the display limit:
```text
Read more
```
expands it inline.
## Chapter List
The chapter list is the most important section after the fiction header.
Desktop should use available horizontal space.
Each chapter row should contain:
* Chapter number
* Chapter title
* Arc
* Publication/update date
* Reading state
Example:
```text
001  The Beginning of Something Terrible     Jan 12
002  A Very Bad Decision                      Jan 13
003  The Consequences                         Jan 15
```
Read chapters may be visually subdued.
Current chapter should have a clear active state.
Unread chapters should remain visually prominent.
## Chapter Search
Chapter search should filter locally without navigating away.
Search examples:
```text
chapter 42
the forest
return
```
Search should update the visible chapter list immediately.
---
# 3. Chapter Page
## Purpose
The Chapter Page is the reading environment.
Unlike the Home and Fiction pages, this screen should minimize navigation clutter and maximize reading comfort.
The reader should feel like a dedicated application rather than a webpage.
## Layout
```text
┌──────────────────────────────────────────────────────────────────────────────┐
│ ← Fiction Title                 Chapter 18                    ☰   Aa   ⚙   │
├──────────────────────────────────────────────────────────────────────────────┤
│                                                                              │
│                                                                              │
│                         Chapter 18                                          │
│                         The Long Road                                       │
│                                                                              │
│                 ───────────────────────────────────                         │
│                                                                              │
│                 The road stretched beyond the horizon.                      │
│                                                                              │
│                 Nobody knew where it ended.                                 │
│                                                                              │
│                 Rae looked toward the distant mountains and                  │
│                 wondered whether turning back would have                     │
│                 been the wiser decision.                                     │
│                                                                              │
│                 ...                                                          │
│                                                                              │
│                                                                              │
│                 ───────────────────────────────────                         │
│                                                                              │
│                         [ Previous ]  [ Next ]                               │
│                                                                              │
└──────────────────────────────────────────────────────────────────────────────┘
```
## Reader Header
The header should be minimal.
Left:
```text
← Fiction Title
```
Center:
```text
Chapter 18
```
Right:
```text
☰   Aa   ⚙
```
Controls:
* Chapter navigation
* Table of contents
* Reader preferences
* Additional reader options
The header may automatically hide while reading.
## Reading Area
The reading area should have a constrained maximum width.
Do not allow text to stretch across the entire desktop monitor.
Recommended structure:
```text
┌─────────────────────────────────────────────────────────────┐
│                                                             │
│              ┌───────────────────────────────┐              │
│              │                               │              │
│              │        Chapter content        │              │
│              │                               │              │
│              │        readable width         │              │
│              │                               │              │
│              └───────────────────────────────┘              │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```
The exact reading width should be configurable.
Recommended default:
```text
max-width: 760px - 900px
```
The reader should prioritize typography over filling the screen.
## Reader Preferences
Reader preferences should be available without leaving the chapter.
Options:
* Font family
* Font size
* Line height
* Paragraph spacing
* Reading width
* Theme
* Text alignment
* Page margins
Example panel:
```text
┌─────────────────────────────┐
│ Reader Preferences          │
├─────────────────────────────┤
│ Font                        │
│ [ System                  ▼]│
│                             │
│ Size                        │
│ −          18px          +  │
│                             │
│ Line Height                 │
│ ─────────●──────────────    │
│                             │
│ Reading Width               │
│ ───────────●────────────    │
│                             │
│ Theme                       │
│ ○ Light  ● Dark  ○ Sepia    │
└─────────────────────────────┘
```
Preferences should persist between chapters and sessions.
## Chapter Navigation
At the bottom:
```text
┌──────────────────────────────────────────────┐
│                                              │
│  [ ← Previous ]              [ Next → ]      │
│                                              │
└──────────────────────────────────────────────┘
```
The next chapter button should be the primary action.
Keyboard shortcuts:
```text
←    Previous chapter
→    Next chapter
Esc  Close reader controls
Ctrl + F    Search chapter
```
Additional shortcuts may be added later.
## Progress
Reading progress should be represented unobtrusively.
Possible implementation:
```text
Chapter 18                         72%
──────────────────────────────────────────────
```
The progress indicator should not permanently occupy significant screen space.
## Table of Contents
A collapsible right-side panel can provide chapter navigation.
```text
┌───────────────────────────────────────┬──────────────────────┐
│                                       │ Chapters             │
│                                       ├──────────────────────┤
│          Chapter content              │ Arc 1                │
│                                       │ 001                  │
│                                       │ 002                  │
│                                       │ 003                  │
│                                       │                      │
│                                       │ Arc 2                │
│                                       │ 004                  │
│                                       │ 005                  │
│                                       │ 006                  │
│                                       │                      │
└───────────────────────────────────────┴──────────────────────┘
```
The current chapter should be highlighted.
The panel should be independently scrollable.
---
# Shared Desktop Principles
## 1. Persistent Navigation
Home, Library, Favorites, and History should remain accessible without requiring repeated browser-style back navigation.
## 2. Information Density
Desktop has space.
Use it.
Do not simply scale the mobile interface until every card becomes an enormous rectangle containing three words.
## 3. Resizable Layout
The application should accommodate:
* 1280×720
* 1366×768
* 1920×1080
* 2560×1440
* ultrawide displays
The layout should reflow rather than simply stretch.
## 4. Keyboard First
Desktop users should be able to navigate common actions without touching the mouse.
Initial shortcuts:
```text
Ctrl + K       Global search
Ctrl + F       Search current context
Esc            Close panel/dialog
←              Previous chapter
→              Next chapter
Space          Page/reading movement
Home           Beginning
End            End
```
## 5. Reading Is a Separate Mode
The Chapter Page should deliberately reduce application chrome.
The Home/Fiction pages are information-dense.
The Chapter Page is reading-focused.
These should feel like different modes of the same application.
## 6. Consistent Fiction Identity
A fiction should look recognizably the same across:
```text
Home
  ↓
Fiction Page
  ↓
Chapter Page
```
Cover, title, author, and fiction metadata should maintain consistent visual identity.
## 7. Desktop Is Not Android Enlarged
ARKster and ARKium should share:
* Fiction concepts
* Chapter concepts
* Library concepts
* Reading progress
* Metadata
* Visual identity
* Terminology
They should not necessarily share:
* Navigation structure
* Component dimensions
* Information density
* Interaction patterns
* Reader controls
* Sidebar behavior
* Desktop keyboard interactions
The goal is platform consistency, not pixel consistency.
```
Current ARKium already has a `spec/screens` area, so this fits naturally as screen-spec documentation in that project. :contentReference[oaicite:0]{index=0}
```