#pragma once

#include <glib.h>

G_BEGIN_DECLS

/*
 * Plain C data model for the scanned library.
 *
 * Deliberately not GObjects yet: v0.01's Home page is a plain
 * GtkListBox, not a GListModel-backed GtkListView/GtkGridView, so
 * there is nothing today that needs property/signal machinery. See
 * docs/ROADMAP.md v0.02 ("card/grid Home page") for when these
 * should grow into GObjects backing a GListStore.
 */

typedef struct {
  char *path;           /* absolute path to the chapter file; doubles as its stable id */
  char *title;           /* derived display title */
  char *arc;             /* arc/subfolder name, or NULL if the novel has no arcs */
  int parsed_number;     /* leading chapter number found in the filename, or -1 */
} ArkiumChapter;

typedef struct {
  char *path;            /* absolute path to the novel's folder; doubles as its stable id */
  char *title;            /* derived display title */
  GPtrArray *chapters;    /* owned: ArkiumChapter*, in reading order */
} ArkiumNovel;

typedef struct {
  GPtrArray *novels;      /* owned: ArkiumNovel*, sorted by title */
} ArkiumLibrary;

ArkiumChapter *arkium_chapter_new(const char *path, const char *title,
                                   const char *arc, int parsed_number);
void arkium_chapter_free(ArkiumChapter *chapter);

ArkiumNovel *arkium_novel_new(const char *path, const char *title);
void arkium_novel_free(ArkiumNovel *novel);

/* Returns the 0-based position of a chapter within novel->chapters, or -1. */
int arkium_novel_chapter_index(ArkiumNovel *novel, ArkiumChapter *chapter);

void arkium_library_free(ArkiumLibrary *library);

G_END_DECLS
