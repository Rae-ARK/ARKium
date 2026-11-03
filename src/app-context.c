#include "app-context.h"

void
arkium_app_context_show_toast(ArkiumAppContext *ctx, const char *message) {
  if (!ctx || !ctx->toast_overlay) {
    return;
  }
  AdwToast *toast = adw_toast_new(message);
  adw_toast_set_timeout(toast, 4);
  adw_toast_overlay_add_toast(ctx->toast_overlay, toast);
}
