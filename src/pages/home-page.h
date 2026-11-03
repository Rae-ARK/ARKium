#pragma once

#include "../app-context.h"

G_BEGIN_DECLS

/* The "home" tagged page: library-folder picker + flat novel list.
 * Loads the persisted library root (if any) and scans it immediately. */
GtkWidget *arkium_home_page_new(ArkiumAppContext *ctx);

G_END_DECLS
