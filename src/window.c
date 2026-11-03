#include "window.h"

#include "app-context.h"
#include "pages/home-page.h"
#include "pages/placeholder-page.h"

/*
 * ArkiumWindow — the application shell (docs/UI_SCREENS.md, "Shared
 * Desktop Principles": persistent navigation).
 *
 *   AdwNavigationSplitView
 *     ├─ sidebar: destination list (Home, Favorites, History, ...)
 *     └─ content: AdwToastOverlay > AdwNavigationView
 *                   [home | placeholder] -> fiction -> chapter
 *
 * Sidebar destinations *replace* the content stack. The Home page is
 * built once and kept alive so its scanned library and list survive
 * switching away and back. While the Chapter Page is showing, the
 * sidebar is hidden: reading is deliberately a separate, chrome-free
 * mode.
 */

typedef struct {
  const char *id;
  const char *label;
  const char *icon;
  const char *placeholder_message; /* NULL for the real Home page */
  gboolean separator_before;
} Destination;

static const Destination DESTINATIONS[] = {
    {"home", "Home", "go-home-symbolic", NULL, FALSE},
    {"favorites", "Favorites", "starred-symbolic",
     "Your favorited novels will appear here. Favoriting already works on "
     "the Fiction page; this view arrives in a later release.",
     FALSE},
    {"history", "History", "document-open-recent-symbolic",
     "A reading history across all novels is planned for a later release.",
     FALSE},
    {"authors", "Authors", "avatar-default-symbolic",
     "Browsing by author needs metadata support, which is not built yet.",
     TRUE},
    {"genres", "Genres", "view-list-symbolic",
     "Browsing by genre needs metadata support, which is not built yet.",
     FALSE},
    {"tags", "Tags", "tag-symbolic",
     "Browsing by tag needs metadata support, which is not built yet.",
     FALSE},
};

struct _ArkiumWindow {
  AdwApplicationWindow parent_instance;

  ArkiumAppContext ctx;
  AdwNavigationSplitView *split_view;
  GtkWidget *sidebar_list;
  GtkWidget *home_page; /* owned ref; reused across sidebar switches */
};

G_DEFINE_FINAL_TYPE(ArkiumWindow, arkium_window, ADW_TYPE_APPLICATION_WINDOW)

static void
show_destination(ArkiumWindow *self, guint index) {
  const Destination *dest = &DESTINATIONS[index];
  GtkWidget *page;

  if (dest->placeholder_message == NULL) {
    page = self->home_page;
  } else {
    page = arkium_placeholder_page_new(dest->label, dest->id,
                                        dest->placeholder_message);
  }

  GPtrArray *pages = g_ptr_array_new();
  g_ptr_array_add(pages, page);
  adw_navigation_view_replace(self->ctx.content_nav,
                               (AdwNavigationPage **)pages->pdata, 1);
  g_ptr_array_free(pages, TRUE);

  adw_navigation_split_view_set_show_content(self->split_view, TRUE);
}

static void
on_sidebar_row_activated(GtkListBox *box, GtkListBoxRow *row,
                          gpointer user_data) {
  (void)box;
  show_destination(ARKIUM_WINDOW(user_data), (guint)gtk_list_box_row_get_index(row));
}

static void
on_visible_page_changed(GObject *object, GParamSpec *pspec, gpointer user_data) {
  (void)pspec;
  ArkiumWindow *self = ARKIUM_WINDOW(user_data);
  AdwNavigationPage *visible =
      adw_navigation_view_get_visible_page(ADW_NAVIGATION_VIEW(object));
  const char *tag = visible ? adw_navigation_page_get_tag(visible) : NULL;
#if ADW_CHECK_VERSION(1, 7, 0)
  adw_navigation_split_view_set_show_sidebar(self->split_view,
                                              g_strcmp0(tag, "chapter") != 0);
#else
  /* show-sidebar needs libadwaita 1.7; on older versions the sidebar
   * simply stays visible while reading. */
  (void)self;
  (void)tag;
#endif
}

static GtkWidget *
build_sidebar(ArkiumWindow *self) {
  GtkWidget *toolbar_view = adw_toolbar_view_new();
  adw_toolbar_view_add_top_bar(
      ADW_TOOLBAR_VIEW(toolbar_view),
      adw_header_bar_new());

  self->sidebar_list = gtk_list_box_new();
  gtk_widget_add_css_class(self->sidebar_list, "navigation-sidebar");
  gtk_list_box_set_selection_mode(GTK_LIST_BOX(self->sidebar_list),
                                   GTK_SELECTION_SINGLE);
  g_signal_connect(self->sidebar_list, "row-activated",
                    G_CALLBACK(on_sidebar_row_activated), self);

  for (guint i = 0; i < G_N_ELEMENTS(DESTINATIONS); i++) {
    const Destination *dest = &DESTINATIONS[i];
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 12);
    gtk_box_append(GTK_BOX(box), gtk_image_new_from_icon_name(dest->icon));
    GtkWidget *label = gtk_label_new(dest->label);
    gtk_label_set_xalign(GTK_LABEL(label), 0.0f);
    gtk_box_append(GTK_BOX(box), label);

    GtkWidget *row = gtk_list_box_row_new();
    gtk_list_box_row_set_child(GTK_LIST_BOX_ROW(row), box);
    if (dest->separator_before) {
      gtk_widget_set_margin_top(row, 12);
    }
    gtk_list_box_append(GTK_LIST_BOX(self->sidebar_list), row);
  }

  GtkWidget *scrolled = gtk_scrolled_window_new();
  gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scrolled), self->sidebar_list);
  adw_toolbar_view_set_content(ADW_TOOLBAR_VIEW(toolbar_view), scrolled);

  return GTK_WIDGET(adw_navigation_page_new(toolbar_view, "ARKium"));
}

static void
arkium_window_dispose(GObject *object) {
  ArkiumWindow *self = ARKIUM_WINDOW(object);

  g_clear_object(&self->home_page);
  if (self->ctx.library) {
    arkium_library_free(self->ctx.library);
    self->ctx.library = NULL;
  }
  if (self->ctx.store) {
    arkium_store_close(self->ctx.store);
    self->ctx.store = NULL;
  }

  G_OBJECT_CLASS(arkium_window_parent_class)->dispose(object);
}

static void
arkium_window_class_init(ArkiumWindowClass *klass) {
  G_OBJECT_CLASS(klass)->dispose = arkium_window_dispose;
}

static void
arkium_window_init(ArkiumWindow *self) {
  gtk_window_set_title(GTK_WINDOW(self), "ARKium");
  gtk_window_set_default_size(GTK_WINDOW(self), 1100, 720);
  gtk_widget_set_size_request(GTK_WIDGET(self), 360, 400);

  self->ctx.window = ADW_APPLICATION_WINDOW(self);

  g_autoptr(GError) error = NULL;
  self->ctx.store = arkium_store_open(&error);
  if (!self->ctx.store) {
    GtkWidget *status = adw_status_page_new();
    adw_status_page_set_icon_name(ADW_STATUS_PAGE(status), "dialog-error-symbolic");
    adw_status_page_set_title(ADW_STATUS_PAGE(status), "Could not open the local database");
    adw_status_page_set_description(ADW_STATUS_PAGE(status), error->message);
    GtkWidget *tv = adw_toolbar_view_new();
    adw_toolbar_view_add_top_bar(ADW_TOOLBAR_VIEW(tv), adw_header_bar_new());
    adw_toolbar_view_set_content(ADW_TOOLBAR_VIEW(tv), status);
    adw_application_window_set_content(ADW_APPLICATION_WINDOW(self), tv);
    return;
  }

  self->ctx.content_nav = ADW_NAVIGATION_VIEW(adw_navigation_view_new());
  self->ctx.toast_overlay = ADW_TOAST_OVERLAY(adw_toast_overlay_new());
  adw_toast_overlay_set_child(self->ctx.toast_overlay,
                               GTK_WIDGET(self->ctx.content_nav));

  self->split_view = ADW_NAVIGATION_SPLIT_VIEW(adw_navigation_split_view_new());
  adw_navigation_split_view_set_min_sidebar_width(self->split_view, 180);
  adw_navigation_split_view_set_max_sidebar_width(self->split_view, 240);
  adw_navigation_split_view_set_sidebar(
      self->split_view, ADW_NAVIGATION_PAGE(build_sidebar(self)));
  adw_navigation_split_view_set_content(
      self->split_view,
      ADW_NAVIGATION_PAGE(adw_navigation_page_new(
          GTK_WIDGET(self->ctx.toast_overlay), "ARKium")));

  /* Collapse to a single pane on narrow windows. */
  AdwBreakpoint *bp =
      adw_breakpoint_new(adw_breakpoint_condition_parse("max-width: 640sp"));
  GValue collapsed = G_VALUE_INIT;
  g_value_init(&collapsed, G_TYPE_BOOLEAN);
  g_value_set_boolean(&collapsed, TRUE);
  adw_breakpoint_add_setter(bp, G_OBJECT(self->split_view), "collapsed",
                             &collapsed);
  adw_application_window_add_breakpoint(ADW_APPLICATION_WINDOW(self), bp);

  adw_application_window_set_content(ADW_APPLICATION_WINDOW(self),
                                      GTK_WIDGET(self->split_view));

  g_signal_connect(self->ctx.content_nav, "notify::visible-page",
                    G_CALLBACK(on_visible_page_changed), self);

  /* Home is built once (this also loads + scans the saved library). */
  self->home_page = g_object_ref_sink(arkium_home_page_new(&self->ctx));
  show_destination(self, 0);
  gtk_list_box_select_row(GTK_LIST_BOX(self->sidebar_list),
                           gtk_list_box_get_row_at_index(
                               GTK_LIST_BOX(self->sidebar_list), 0));
}

ArkiumWindow *
arkium_window_new(AdwApplication *app) {
  return g_object_new(ARKIUM_TYPE_WINDOW, "application", app, NULL);
}
