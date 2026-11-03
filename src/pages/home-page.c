#include "home-page.h"

#include "../data/scanner.h"
#include "fiction-page.h"

typedef struct {
  ArkiumAppContext *ctx;
  GtkWidget *listbox;
  GtkWidget *status_label;
} HomePageData;

static void populate_listbox(HomePageData *data);

static void
on_novel_row_activated(GtkListBox *box, GtkListBoxRow *row,
                        gpointer user_data) {
  (void)box;
  HomePageData *data = user_data;
  int index = gtk_list_box_row_get_index(row);
  if (!data->ctx->library || index < 0 ||
      (guint)index >= data->ctx->library->novels->len) {
    return;
  }
  ArkiumNovel *novel = g_ptr_array_index(data->ctx->library->novels, index);
  GtkWidget *page = arkium_fiction_page_new(data->ctx, novel);
  adw_navigation_view_push(data->ctx->content_nav, ADW_NAVIGATION_PAGE(page));
}

static void
rescan_and_populate(HomePageData *data, const char *root_path) {
  GError *error = NULL;
  ArkiumLibrary *library = arkium_scan_library(root_path, &error);
  if (!library) {
    g_autofree char *message =
        g_strdup_printf("Could not scan %s: %s", root_path, error->message);
    arkium_app_context_show_toast(data->ctx, message);
    g_clear_error(&error);
    return;
  }

  if (data->ctx->library) {
    arkium_library_free(data->ctx->library);
  }
  data->ctx->library = library;
  arkium_store_set_library_root(data->ctx->store, root_path);
  populate_listbox(data);

  g_autofree char *message = g_strdup_printf(
      "Found %u novel%s", library->novels->len,
      library->novels->len == 1 ? "" : "s");
  arkium_app_context_show_toast(data->ctx, message);
}

static void
on_folder_chosen(GObject *source, GAsyncResult *result, gpointer user_data) {
  HomePageData *data = user_data;
  GError *error = NULL;
  GFile *folder =
      gtk_file_dialog_select_folder_finish(GTK_FILE_DIALOG(source), result,
                                            &error);
  if (!folder) {
    if (error &&
        !g_error_matches(error, GTK_DIALOG_ERROR, GTK_DIALOG_ERROR_DISMISSED)) {
      arkium_app_context_show_toast(data->ctx, error->message);
    }
    g_clear_error(&error);
    return;
  }

  g_autofree char *path = g_file_get_path(folder);
  g_object_unref(folder);
  if (path) {
    rescan_and_populate(data, path);
  }
}

static void
on_choose_folder_clicked(GtkButton *button, gpointer user_data) {
  (void)button;
  HomePageData *data = user_data;
  GtkFileDialog *dialog = gtk_file_dialog_new();
  gtk_file_dialog_set_title(dialog, "Choose your library folder");
  gtk_file_dialog_select_folder(dialog, GTK_WINDOW(data->ctx->window), NULL,
                                 on_folder_chosen, data);
  g_object_unref(dialog);
}

static void
clear_listbox(GtkWidget *listbox) {
  GtkWidget *child = gtk_widget_get_first_child(listbox);
  while (child) {
    GtkWidget *next = gtk_widget_get_next_sibling(child);
    gtk_list_box_remove(GTK_LIST_BOX(listbox), child);
    child = next;
  }
}

static void
populate_listbox(HomePageData *data) {
  clear_listbox(data->listbox);

  if (!data->ctx->library || data->ctx->library->novels->len == 0) {
    gtk_label_set_text(GTK_LABEL(data->status_label),
                        data->ctx->library
                            ? "No novels found. Expected subfolders "
                              "containing .txt/.md chapter files."
                            : "No library folder selected yet. Use "
                              "“Open Library Folder” to pick one.");
    gtk_widget_set_visible(data->status_label, TRUE);
    gtk_widget_set_visible(data->listbox, FALSE);
    return;
  }

  gtk_widget_set_visible(data->status_label, FALSE);
  gtk_widget_set_visible(data->listbox, TRUE);

  for (guint i = 0; i < data->ctx->library->novels->len; i++) {
    ArkiumNovel *novel = g_ptr_array_index(data->ctx->library->novels, i);
    AdwActionRow *row = ADW_ACTION_ROW(adw_action_row_new());
    adw_preferences_row_set_title(ADW_PREFERENCES_ROW(row), novel->title);

    g_autofree char *subtitle = g_strdup_printf(
        "%u chapter%s", novel->chapters->len,
        novel->chapters->len == 1 ? "" : "s");
    adw_action_row_set_subtitle(row, subtitle);
    adw_action_row_set_activatable(row, TRUE);

    GtkWidget *chevron = gtk_image_new_from_icon_name("go-next-symbolic");
    adw_action_row_add_suffix(row, chevron);

    gtk_list_box_append(GTK_LIST_BOX(data->listbox), GTK_WIDGET(row));
  }
}

GtkWidget *
arkium_home_page_new(ArkiumAppContext *ctx) {
  HomePageData *data = g_new0(HomePageData, 1);
  data->ctx = ctx;

  GtkWidget *toolbar_view = adw_toolbar_view_new();
  GtkWidget *header = adw_header_bar_new();

  GtkWidget *choose_button = gtk_button_new_with_label("Open Library Folder");
  gtk_widget_add_css_class(choose_button, "suggested-action");
  g_signal_connect(choose_button, "clicked",
                    G_CALLBACK(on_choose_folder_clicked), data);
  adw_header_bar_pack_start(ADW_HEADER_BAR(header), choose_button);
  adw_toolbar_view_add_top_bar(ADW_TOOLBAR_VIEW(toolbar_view), header);

  GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 12);
  gtk_widget_set_margin_top(box, 24);
  gtk_widget_set_margin_bottom(box, 24);
  gtk_widget_set_margin_start(box, 24);
  gtk_widget_set_margin_end(box, 24);

  data->status_label = gtk_label_new(NULL);
  gtk_widget_add_css_class(data->status_label, "dim-label");
  gtk_label_set_wrap(GTK_LABEL(data->status_label), TRUE);
  gtk_box_append(GTK_BOX(box), data->status_label);

  data->listbox = gtk_list_box_new();
  gtk_widget_add_css_class(data->listbox, "boxed-list");
  gtk_list_box_set_selection_mode(GTK_LIST_BOX(data->listbox),
                                   GTK_SELECTION_NONE);
  g_signal_connect(data->listbox, "row-activated",
                    G_CALLBACK(on_novel_row_activated), data);
  gtk_box_append(GTK_BOX(box), data->listbox);

  GtkWidget *clamp = adw_clamp_new();
  adw_clamp_set_maximum_size(ADW_CLAMP(clamp), 760);
  adw_clamp_set_child(ADW_CLAMP(clamp), box);

  GtkWidget *scrolled = gtk_scrolled_window_new();
  gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scrolled), clamp);
  gtk_widget_set_vexpand(scrolled, TRUE);
  adw_toolbar_view_set_content(ADW_TOOLBAR_VIEW(toolbar_view), scrolled);

  AdwNavigationPage *page =
      ADW_NAVIGATION_PAGE(adw_navigation_page_new(toolbar_view, "Home"));
  adw_navigation_page_set_tag(page, "home");
  g_object_set_data_full(G_OBJECT(page), "home-page-data", data, g_free);

  g_autofree char *root = arkium_store_get_library_root(ctx->store);
  if (root) {
    rescan_and_populate(data, root);
  } else {
    populate_listbox(data);
  }

  return GTK_WIDGET(page);
}
