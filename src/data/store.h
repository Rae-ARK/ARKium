#pragma once

#include <glib.h>

G_BEGIN_DECLS

/*
 * Local SQLite-backed store for everything that needs to survive a
 * restart: the chosen library root, favorites, and reading progress.
 * Lives at $XDG_DATA_HOME/arkium/arkium.db (falls back to
 * ~/.local/share/arkium/arkium.db).
 *
 * Novels and chapters are keyed by their absolute filesystem path —
 * matching docs/foundation/ARCHITECTURE.md's "read/indexed in place, never
 * copied" model, this needs no separate id scheme.
 */

typedef struct _ArkiumStore ArkiumStore;

ArkiumStore *arkium_store_open(GError **error);
void arkium_store_close(ArkiumStore *store);

/* Caller frees the result with g_free(). Returns NULL if unset. */
char *arkium_store_get_library_root(ArkiumStore *store);
gboolean arkium_store_set_library_root(ArkiumStore *store, const char *path);

gboolean arkium_store_get_favorite(ArkiumStore *store, const char *novel_path);
gboolean arkium_store_set_favorite(ArkiumStore *store, const char *novel_path,
                                    gboolean favorite);

/* Records that chapter_path (within novel_path) was last read, with
 * scroll_fraction in [0, 1] marking position within that chapter. */
gboolean arkium_store_set_progress(ArkiumStore *store, const char *novel_path,
                                    const char *chapter_path,
                                    double scroll_fraction);

/* Caller frees *out_chapter_path with g_free(). Returns FALSE (and
 * leaves outputs untouched) if there is no recorded progress for
 * novel_path. */
gboolean arkium_store_get_progress(ArkiumStore *store, const char *novel_path,
                                    char **out_chapter_path,
                                    double *out_scroll_fraction);

G_END_DECLS
