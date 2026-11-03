#pragma once

#include <adwaita.h>

#include "data/novel.h"
#include "data/store.h"

G_BEGIN_DECLS

/*
 * Everything a page constructor needs: the content navigation stack
 * to push/pop pages on, the persistence layer, and the currently
 * scanned library (NULL until a folder has been chosen). Owned by
 * ArkiumWindow and passed down by pointer rather than threaded
 * through widget hierarchies or globals.
 */
typedef struct {
  AdwApplicationWindow *window;
  AdwNavigationView *content_nav;
  AdwToastOverlay *toast_overlay;
  ArkiumStore *store;
  ArkiumLibrary *library; /* owned by the window; replaced on rescan */
} ArkiumAppContext;

void arkium_app_context_show_toast(ArkiumAppContext *ctx, const char *message);

G_END_DECLS
