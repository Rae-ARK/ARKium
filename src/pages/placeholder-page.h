#pragma once

#include <adwaita.h>

G_BEGIN_DECLS

/* A plain "not built yet" page for sidebar destinations (Favorites,
 * History, ...) that exist structurally per docs/UI_SCREENS.md but
 * have no real content until a later roadmap stage. Honest filler,
 * not a stub pretending to be finished. */
GtkWidget *arkium_placeholder_page_new(const char *title, const char *tag,
                                        const char *message);

G_END_DECLS
