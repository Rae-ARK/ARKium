#include "window.h"

/*
 * ArkiumWindow — the application's main window.
 *
 * This is the native-shell replacement for src/App.vue + the
 * HelloWorld.vue scaffold from the Neutralino/Vue build: a header
 * bar and a centered "get started" counter, enough to prove the
 * GTK shell, CSS theming and build pipeline work end to end before
 * the real library/reader UI (see docs/UI_SCREENS.md) is built on
 * top of it.
 */

struct _ArkiumWindow {
  GtkApplicationWindow parent_instance;

  GtkWidget *counter_button;
  guint count;
};

G_DEFINE_FINAL_TYPE(ArkiumWindow, arkium_window, GTK_TYPE_APPLICATION_WINDOW)

static void
arkium_window_update_counter_label(ArkiumWindow *self) {
  g_autofree gchar *label = g_strdup_printf("Count is %u", self->count);
  gtk_button_set_label(GTK_BUTTON(self->counter_button), label);
}

static void
on_counter_clicked(GtkButton *button, gpointer user_data) {
  (void)button;
  ArkiumWindow *self = ARKIUM_WINDOW(user_data);
  self->count++;
  arkium_window_update_counter_label(self);
}

static void
sync_dark_class(ArkiumWindow *self) {
  gboolean prefer_dark = FALSE;
  g_object_get(gtk_settings_get_default(), "gtk-application-prefer-dark-theme",
               &prefer_dark, NULL);

  if (prefer_dark) {
    gtk_widget_add_css_class(GTK_WIDGET(self), "dark");
  } else {
    gtk_widget_remove_css_class(GTK_WIDGET(self), "dark");
  }
}

static void
on_prefer_dark_theme_changed(GObject *settings, GParamSpec *pspec,
                              gpointer user_data) {
  (void)settings;
  (void)pspec;
  sync_dark_class(ARKIUM_WINDOW(user_data));
}

static void
arkium_window_class_init(ArkiumWindowClass *klass) {
  (void)klass;
}

static void
arkium_window_init(ArkiumWindow *self) {
  self->count = 0;

  gtk_window_set_title(GTK_WINDOW(self), "ARKium");
  gtk_window_set_default_size(GTK_WINDOW(self), 900, 640);

  GtkWidget *header_bar = gtk_header_bar_new();
  gtk_window_set_titlebar(GTK_WINDOW(self), header_bar);

  GtkWidget *content = gtk_box_new(GTK_ORIENTATION_VERTICAL, 24);
  gtk_widget_set_valign(content, GTK_ALIGN_CENTER);
  gtk_widget_set_halign(content, GTK_ALIGN_CENTER);
  gtk_widget_set_vexpand(content, TRUE);
  gtk_widget_set_hexpand(content, TRUE);
  gtk_widget_add_css_class(content, "arkium-center");

  GtkWidget *heading = gtk_label_new("Get started");
  gtk_widget_add_css_class(heading, "arkium-heading");

  GtkWidget *subtitle =
      gtk_label_new("Native GTK shell — library and reader UI come next");
  gtk_widget_add_css_class(subtitle, "dim-label");

  self->counter_button = gtk_button_new_with_label("Count is 0");
  gtk_widget_add_css_class(self->counter_button, "arkium-counter");
  gtk_widget_set_halign(self->counter_button, GTK_ALIGN_CENTER);
  g_signal_connect(self->counter_button, "clicked",
                    G_CALLBACK(on_counter_clicked), self);

  gtk_box_append(GTK_BOX(content), heading);
  gtk_box_append(GTK_BOX(content), subtitle);
  gtk_box_append(GTK_BOX(content), self->counter_button);

  gtk_window_set_child(GTK_WINDOW(self), content);

  sync_dark_class(self);
  g_signal_connect(gtk_settings_get_default(),
                    "notify::gtk-application-prefer-dark-theme",
                    G_CALLBACK(on_prefer_dark_theme_changed), self);
}

ArkiumWindow *
arkium_window_new(GtkApplication *app) {
  return g_object_new(ARKIUM_TYPE_WINDOW, "application", app, NULL);
}
