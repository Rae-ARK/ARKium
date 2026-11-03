#pragma once

#include "../app-context.h"
#include "../data/novel.h"

G_BEGIN_DECLS

/* The Chapter Page: the reading environment. Loads chapter text from
 * disk, tracks scroll position as reading progress, and navigates to
 * adjacent chapters via buttons or the Left/Right arrow keys. */
GtkWidget *arkium_chapter_page_new(ArkiumAppContext *ctx, ArkiumNovel *novel,
                                    int chapter_index);

G_END_DECLS
