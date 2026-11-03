#include <adwaita.h>

#include "window.h"

#define ARKIUM_APP_ID "io.github.raeark.Arkium"
#define ARKIUM_RESOURCE_CSS "/io/github/raeark/Arkium/style.css"

static void
load_css(void) {
  GtkCssProvider *provider = gtk_css_provider_new();
  gtk_css_provider_load_from_resource(provider, ARKIUM_RESOURCE_CSS);

  gtk_style_context_add_provider_for_display(
      gdk_display_get_default(), GTK_STYLE_PROVIDER(provider),
      GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);

  g_object_unref(provider);
}

static void
on_activate(GApplication *app, gpointer user_data) {
  (void)user_data;

  load_css();

  ArkiumWindow *window = arkium_window_new(ADW_APPLICATION(app));
  gtk_window_present(GTK_WINDOW(window));
}

int
main(int argc, char *argv[]) {
  g_autoptr(AdwApplication) app =
      adw_application_new(ARKIUM_APP_ID, G_APPLICATION_DEFAULT_FLAGS);

  g_signal_connect(app, "activate", G_CALLBACK(on_activate), NULL);

  return g_application_run(G_APPLICATION(app), argc, argv);
}
