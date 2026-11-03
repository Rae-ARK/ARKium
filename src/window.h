#pragma once

#include <adwaita.h>

G_BEGIN_DECLS

#define ARKIUM_TYPE_WINDOW (arkium_window_get_type())

G_DECLARE_FINAL_TYPE(ArkiumWindow, arkium_window, ARKIUM, WINDOW, AdwApplicationWindow)

ArkiumWindow *arkium_window_new(AdwApplication *app);

G_END_DECLS
