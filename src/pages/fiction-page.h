#pragma once

#include "../app-context.h"
#include "../data/novel.h"

G_BEGIN_DECLS

/* The Fiction Page: novel header, favorite toggle, Start/Continue
 * Reading, and the flat chapter list. novel must outlive the page
 * (owned by ctx->library). */
GtkWidget *arkium_fiction_page_new(ArkiumAppContext *ctx, ArkiumNovel *novel);

G_END_DECLS
