#include "fiction-page.h"

#include "chapter-page.h"

typedef struct {
  ArkiumAppContext *ctx;
  ArkiumNovel *novel;
} FictionPageData;

static void
on_chapter_row_activated(GtkListBox *box, GtkListBoxRow *row,
                          gpointer user_data) {
  (void)box;
  FictionPageData *data = user_data;
  int index = gtk_list_box_row_get_index(row);
  GtkWidget *page = arkium_chapter_page_new(data->ctx, data->novel, index);
  adw_navigation_view_push(data->ctx->content_nav, ADW_NAVIGATION_PAGE(page));
}

static void
on_start_continue_clicked(GtkButton *button, gpointer user_data) {
  (void)button;
  FictionPageData *data = user_data;

  int index = 0;
  g_autofree char *chapter_path = NULL;
  if (arkium_store_get_progress(data->ctx->store, data->novel->path,
                                 &chapter_path, NULL)) {
    for (guint i = 0; i < data->novel->chapters->len; i++) {
      ArkiumChapter *chapter = g_ptr_array_index(data->novel->chapters, i);
      if (g_strcmp0(chapter->path, chapter_path) == 0) {
        index = (int)i;
        break;
      }
    }
  }

  GtkWidget *page = arkium_chapter_page_new(data->ctx, data->novel, index);
  adw_navigation_view_push(data->ctx->content_nav, ADW_NAVIGATION_PAGE(page));
}

static void
on_favorite_toggled(GtkToggleButton *button, gpointer user_data) {
  FictionPageData *data = user_data;
  arkium_store_set_favorite(data->ctx->store, data->novel->path,
                             gtk_toggle_button_get_active(button));
}

GtkWidget *
arkium_fiction_page_new(ArkiumAppContext *ctx, ArkiumNovel *novel) {
  FictionPageData *data = g_new0(FictionPageData, 1);
  data->ctx = ctx;
  data->novel = novel;

  GtkWidget *toolbar_view = adw_toolbar_view_new();
  GtkWidget *header = adw_header_bar_new();
  adw_toolbar_view_add_top_bar(ADW_TOOLBAR_VIEW(toolbar_view), header);

  GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 16);
  gtk_widget_set_margin_top(box, 24);
  gtk_widget_set_margin_bottom(box, 24);
  gtk_widget_set_margin_start(box, 24);
  gtk_widget_set_margin_end(box, 24);

  GtkWidget *heading = gtk_label_new(novel->title);
  gtk_widget_add_css_class(heading, "title-1");
  gtk_widget_set_halign(heading, GTK_ALIGN_START);
  gtk_label_set_wrap(GTK_LABEL(heading), TRUE);
  gtk_box_append(GTK_BOX(box), heading);

  g_autofree char *chapter_count_text = g_strdup_printf(
      "%u chapter%s", novel->chapters->len,
      novel->chapters->len == 1 ? "" : "s");
  GtkWidget *meta = gtk_label_new(chapter_count_text);
  gtk_widget_add_css_class(meta, "dim-label");
  gtk_widget_set_halign(meta, GTK_ALIGN_START);
  gtk_box_append(GTK_BOX(box), meta);

  GtkWidget *actions = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);

  gboolean has_progress =
      arkium_store_get_progress(ctx->store, novel->path, NULL, NULL);
  GtkWidget *primary_button = gtk_button_new_with_label(
      has_progress ? "Continue Reading" : "Start Reading");
  gtk_widget_add_css_class(primary_button, "suggested-action");
  gtk_widget_add_css_class(primary_button, "pill");
  g_signal_connect(primary_button, "clicked",
                    G_CALLBACK(on_start_continue_clicked), data);
  gtk_box_append(GTK_BOX(actions), primary_button);

  GtkWidget *favorite_button = gtk_toggle_button_new();
  gtk_button_set_icon_name(GTK_BUTTON(favorite_button), "starred-symbolic");
  gtk_widget_set_tooltip_text(favorite_button, "Favorite");
  gtk_widget_add_css_class(favorite_button, "pill");
  gtk_toggle_button_set_active(
      GTK_TOGGLE_BUTTON(favorite_button),
      arkium_store_get_favorite(ctx->store, novel->path));
  g_signal_connect(favorite_button, "toggled",
                    G_CALLBACK(on_favorite_toggled), data);
  gtk_box_append(GTK_BOX(actions), favorite_button);

  gtk_box_append(GTK_BOX(box), actions);

  GtkWidget *chapters_heading = gtk_label_new("Chapters");
  gtk_widget_add_css_class(chapters_heading, "title-4");
  gtk_widget_set_halign(chapters_heading, GTK_ALIGN_START);
  gtk_widget_set_margin_top(chapters_heading, 8);
  gtk_box_append(GTK_BOX(box), chapters_heading);

  GtkWidget *listbox = gtk_list_box_new();
  gtk_widget_add_css_class(listbox, "boxed-list");
  gtk_list_box_set_selection_mode(GTK_LIST_BOX(listbox), GTK_SELECTION_NONE);
  g_signal_connect(listbox, "row-activated",
                    G_CALLBACK(on_chapter_row_activated), data);

  for (guint i = 0; i < novel->chapters->len; i++) {
    ArkiumChapter *chapter = g_ptr_array_index(novel->chapters, i);
    AdwActionRow *row = ADW_ACTION_ROW(adw_action_row_new());
    adw_preferences_row_set_title(ADW_PREFERENCES_ROW(row), chapter->title);
    if (chapter->arc) {
      adw_action_row_set_subtitle(row, chapter->arc);
    }
    gtk_list_box_row_set_activatable(GTK_LIST_BOX_ROW(row), TRUE);
    GtkWidget *chevron = gtk_image_new_from_icon_name("go-next-symbolic");
    adw_action_row_add_suffix(row, chevron);
    gtk_list_box_append(GTK_LIST_BOX(listbox), GTK_WIDGET(row));
  }
  gtk_box_append(GTK_BOX(box), listbox);

  GtkWidget *clamp = adw_clamp_new();
  adw_clamp_set_maximum_size(ADW_CLAMP(clamp), 760);
  adw_clamp_set_child(ADW_CLAMP(clamp), box);

  GtkWidget *scrolled = gtk_scrolled_window_new();
  gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scrolled), clamp);
  gtk_widget_set_vexpand(scrolled, TRUE);
  adw_toolbar_view_set_content(ADW_TOOLBAR_VIEW(toolbar_view), scrolled);

  AdwNavigationPage *page = ADW_NAVIGATION_PAGE(
      adw_navigation_page_new(toolbar_view, novel->title));
  g_object_set_data_full(G_OBJECT(page), "fiction-page-data", data, g_free);

  return GTK_WIDGET(page);
}
