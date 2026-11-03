#pragma once

#include <gtk/gtk.h>

G_BEGIN_DECLS

#define ARKIUM_TYPE_WINDOW (arkium_window_get_type())

G_DECLARE_FINAL_TYPE(ArkiumWindow, arkium_window, ARKIUM, WINDOW, GtkApplicationWindow)

ArkiumWindow *arkium_window_new(GtkApplication *app);

G_END_DECLS
