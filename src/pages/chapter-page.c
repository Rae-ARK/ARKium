#include "chapter-page.h"

#include <math.h>

/*
 * Chapter Page — the reading environment (docs/foundation/UI_SCREENS.md §3).
 *
 * Previous/Next swap the text inside this same page rather than
 * pushing a new page per chapter, so the navigation stack stays
 * [home, fiction, chapter] no matter how far the reader goes and the
 * back button always returns to the Fiction Page.
 *
 * Progress is the vertical scroll fraction in [0, 1]. It is saved
 * (debounced) while scrolling, and flushed when the page is hidden
 * or the chapter changes. Restoring it has to wait for GtkTextView's
 * asynchronous layout, so for a short window after a chapter loads we
 * keep re-applying the target fraction as the scroll range grows.
 */

#define SAVE_DEBOUNCE_MS 750
#define RESTORE_WINDOW_MS 800

typedef struct {
  ArkiumAppContext *ctx;
  ArkiumNovel *novel;
  int index;

  AdwWindowTitle *window_title;
  GtkWidget *percent_label;
  GtkWidget *heading;
  GtkWidget *text_view;
  GtkWidget *scrolled;
  GtkWidget *prev_button;
  GtkWidget *next_button;
  GtkAdjustment *vadj;

  guint save_source;
  guint restore_source;
  gboolean restoring;
  double restore_fraction;
} ChapterPageData;

static double
current_fraction(ChapterPageData *d) {
  double upper = gtk_adjustment_get_upper(d->vadj);
  double page = gtk_adjustment_get_page_size(d->vadj);
  double range = upper - page;
  if (range <= 0.0) {
    return 0.0;
  }
  return CLAMP(gtk_adjustment_get_value(d->vadj) / range, 0.0, 1.0);
}

static void
update_percent_label(ChapterPageData *d) {
  g_autofree char *text =
      g_strdup_printf("%d%%", (int)floor(current_fraction(d) * 100.0));
  gtk_label_set_text(GTK_LABEL(d->percent_label), text);
}

static void
save_progress_now(ChapterPageData *d) {
  if (d->save_source) {
    g_source_remove(d->save_source);
    d->save_source = 0;
  }
  if (d->index < 0 || (guint)d->index >= d->novel->chapters->len) {
    return;
  }
  ArkiumChapter *chapter = g_ptr_array_index(d->novel->chapters, d->index);
  arkium_store_set_progress(d->ctx->store, d->novel->path, chapter->path,
                             current_fraction(d));
}

static gboolean
on_save_timeout(gpointer user_data) {
  ChapterPageData *d = user_data;
  d->save_source = 0;
  save_progress_now(d);
  return G_SOURCE_REMOVE;
}

static void
on_scroll_changed(GtkAdjustment *adj, gpointer user_data) {
  (void)adj;
  ChapterPageData *d = user_data;
  if (d->restoring) {
    return; /* our own programmatic scroll; don't overwrite saved progress */
  }
  update_percent_label(d);
  if (d->save_source) {
    g_source_remove(d->save_source);
  }
  d->save_source = g_timeout_add(SAVE_DEBOUNCE_MS, on_save_timeout, d);
}

static void
apply_restore(ChapterPageData *d) {
  double range =
      gtk_adjustment_get_upper(d->vadj) - gtk_adjustment_get_page_size(d->vadj);
  if (range > 0.0) {
    gtk_adjustment_set_value(d->vadj, d->restore_fraction * range);
  }
}

static void
on_range_changed(GtkAdjustment *adj, gpointer user_data) {
  (void)adj;
  ChapterPageData *d = user_data;
  if (d->restoring) {
    apply_restore(d);
  }
  update_percent_label(d);
}

static gboolean
on_restore_done(gpointer user_data) {
  ChapterPageData *d = user_data;
  d->restore_source = 0;
  d->restoring = FALSE;
  update_percent_label(d);
  return G_SOURCE_REMOVE;
}

static void
update_nav_buttons(ChapterPageData *d) {
  gtk_widget_set_sensitive(d->prev_button, d->index > 0);
  gtk_widget_set_sensitive(d->next_button,
                            (guint)(d->index + 1) < d->novel->chapters->len);
}

static void
load_chapter(ChapterPageData *d, int index, double fraction) {
  if (index < 0 || (guint)index >= d->novel->chapters->len) {
    return;
  }

  /* Persist where we were in the chapter we're leaving. */
  if (d->index >= 0 && d->index != index) {
    save_progress_now(d);
  }

  d->index = index;
  ArkiumChapter *chapter = g_ptr_array_index(d->novel->chapters, index);

  adw_window_title_set_title(d->window_title, chapter->title);
  adw_window_title_set_subtitle(d->window_title, d->novel->title);
  gtk_label_set_text(GTK_LABEL(d->heading), chapter->title);

  g_autofree char *contents = NULL;
  gsize length = 0;
  g_autoptr(GError) error = NULL;
  GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(d->text_view));

  if (g_file_get_contents(chapter->path, &contents, &length, &error)) {
    g_autofree char *valid = g_utf8_make_valid(contents, (gssize)length);
    gtk_text_buffer_set_text(buffer, valid, -1);
  } else {
    g_autofree char *msg =
        g_strdup_printf("Could not read this chapter.\n\n%s", error->message);
    gtk_text_buffer_set_text(buffer, msg, -1);
  }

  /* Format the whole buffer with the paragraph-spacing tag. */
  GtkTextIter start, end;
  gtk_text_buffer_get_bounds(buffer, &start, &end);
  gtk_text_buffer_apply_tag_by_name(buffer, "body", &start, &end);

  update_nav_buttons(d);

  /* Restore (or reset to top) once layout catches up. */
  if (d->restore_source) {
    g_source_remove(d->restore_source);
  }
  d->restoring = TRUE;
  d->restore_fraction = fraction;
  gtk_adjustment_set_value(d->vadj, 0.0);
  d->restore_source = g_timeout_add(RESTORE_WINDOW_MS, on_restore_done, d);

  /* Record that this chapter is now the one being read, so
   * "Continue Reading" resumes here even if the user never scrolls. */
  arkium_store_set_progress(d->ctx->store, d->novel->path, chapter->path,
                             fraction);
}

static void
go_prev(ChapterPageData *d) {
  if (d->index > 0) {
    load_chapter(d, d->index - 1, 0.0);
  }
}

static void
go_next(ChapterPageData *d) {
  if ((guint)(d->index + 1) < d->novel->chapters->len) {
    load_chapter(d, d->index + 1, 0.0);
  }
}

static void
on_prev_clicked(GtkButton *button, gpointer user_data) {
  (void)button;
  go_prev(user_data);
}

static void
on_next_clicked(GtkButton *button, gpointer user_data) {
  (void)button;
  go_next(user_data);
}

static gboolean
on_key_pressed(GtkEventControllerKey *controller, guint keyval, guint keycode,
                GdkModifierType state, gpointer user_data) {
  (void)controller;
  (void)keycode;
  ChapterPageData *d = user_data;

  if (state & (GDK_CONTROL_MASK | GDK_ALT_MASK | GDK_SHIFT_MASK |
               GDK_SUPER_MASK)) {
    return GDK_EVENT_PROPAGATE;
  }
  if (keyval == GDK_KEY_Left) {
    go_prev(d);
    return GDK_EVENT_STOP;
  }
  if (keyval == GDK_KEY_Right) {
    go_next(d);
    return GDK_EVENT_STOP;
  }
  return GDK_EVENT_PROPAGATE;
}

static void
on_page_hidden(AdwNavigationPage *page, gpointer user_data) {
  (void)page;
  ChapterPageData *d = user_data;
  /* Flush while the novel is guaranteed alive: the window may free the
   * library on the next rescan. */
  save_progress_now(d);
}

static void
chapter_page_data_free(gpointer data) {
  ChapterPageData *d = data;
  if (d->save_source) {
    g_source_remove(d->save_source);
  }
  if (d->restore_source) {
    g_source_remove(d->restore_source);
  }
  g_free(d);
}

GtkWidget *
arkium_chapter_page_new(ArkiumAppContext *ctx, ArkiumNovel *novel,
                         int chapter_index) {
  ChapterPageData *d = g_new0(ChapterPageData, 1);
  d->ctx = ctx;
  d->novel = novel;
  d->index = -1;

  /* Header: back button comes for free from AdwNavigationView. */
  GtkWidget *toolbar_view = adw_toolbar_view_new();
  GtkWidget *header = adw_header_bar_new();
  d->window_title = ADW_WINDOW_TITLE(adw_window_title_new("", ""));
  adw_header_bar_set_title_widget(ADW_HEADER_BAR(header),
                                   GTK_WIDGET(d->window_title));
  d->percent_label = gtk_label_new("0%");
  gtk_widget_add_css_class(d->percent_label, "dim-label");
  gtk_widget_add_css_class(d->percent_label, "numeric");
  adw_header_bar_pack_end(ADW_HEADER_BAR(header), d->percent_label);
  adw_toolbar_view_add_top_bar(ADW_TOOLBAR_VIEW(toolbar_view), header);

  /* Reading column. */
  GtkWidget *column = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
  gtk_widget_set_margin_top(column, 32);
  gtk_widget_set_margin_bottom(column, 48);
  gtk_widget_set_margin_start(column, 24);
  gtk_widget_set_margin_end(column, 24);

  d->heading = gtk_label_new("");
  gtk_widget_add_css_class(d->heading, "title-1");
  gtk_widget_add_css_class(d->heading, "arkium-reader-heading");
  gtk_label_set_wrap(GTK_LABEL(d->heading), TRUE);
  gtk_label_set_xalign(GTK_LABEL(d->heading), 0.0f);
  gtk_box_append(GTK_BOX(column), d->heading);

  GtkWidget *rule = gtk_separator_new(GTK_ORIENTATION_HORIZONTAL);
  gtk_widget_set_margin_top(rule, 16);
  gtk_widget_set_margin_bottom(rule, 16);
  gtk_box_append(GTK_BOX(column), rule);

  d->text_view = gtk_text_view_new();
  gtk_text_view_set_editable(GTK_TEXT_VIEW(d->text_view), FALSE);
  gtk_text_view_set_cursor_visible(GTK_TEXT_VIEW(d->text_view), FALSE);
  gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(d->text_view), GTK_WRAP_WORD_CHAR);
  gtk_widget_add_css_class(d->text_view, "arkium-reader");
  gtk_widget_set_focusable(d->text_view, FALSE);

  GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(d->text_view));
  gtk_text_buffer_create_tag(buffer, "body", "pixels-below-lines", 10,
                              "pixels-inside-wrap", 4, NULL);
  gtk_box_append(GTK_BOX(column), d->text_view);

  GtkWidget *nav = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 12);
  gtk_widget_set_margin_top(nav, 40);
  gtk_widget_set_halign(nav, GTK_ALIGN_CENTER);

  d->prev_button = gtk_button_new_with_label("← Previous");
  gtk_widget_add_css_class(d->prev_button, "pill");
  g_signal_connect(d->prev_button, "clicked", G_CALLBACK(on_prev_clicked), d);
  gtk_box_append(GTK_BOX(nav), d->prev_button);

  d->next_button = gtk_button_new_with_label("Next →");
  gtk_widget_add_css_class(d->next_button, "pill");
  gtk_widget_add_css_class(d->next_button, "suggested-action");
  g_signal_connect(d->next_button, "clicked", G_CALLBACK(on_next_clicked), d);
  gtk_box_append(GTK_BOX(nav), d->next_button);
  gtk_box_append(GTK_BOX(column), nav);

  GtkWidget *clamp = adw_clamp_new();
  adw_clamp_set_maximum_size(ADW_CLAMP(clamp), 820);
  adw_clamp_set_tightening_threshold(ADW_CLAMP(clamp), 700);
  adw_clamp_set_child(ADW_CLAMP(clamp), column);

  d->scrolled = gtk_scrolled_window_new();
  gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(d->scrolled),
                                  GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
  gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(d->scrolled), clamp);
  gtk_widget_set_vexpand(d->scrolled, TRUE);
  adw_toolbar_view_set_content(ADW_TOOLBAR_VIEW(toolbar_view), d->scrolled);

  d->vadj = gtk_scrolled_window_get_vadjustment(GTK_SCROLLED_WINDOW(d->scrolled));
  g_signal_connect(d->vadj, "value-changed", G_CALLBACK(on_scroll_changed), d);
  g_signal_connect(d->vadj, "changed", G_CALLBACK(on_range_changed), d);

  AdwNavigationPage *page =
      ADW_NAVIGATION_PAGE(adw_navigation_page_new(toolbar_view, "Chapter"));
  adw_navigation_page_set_tag(page, "chapter");
  g_object_set_data_full(G_OBJECT(page), "chapter-page-data", d,
                          chapter_page_data_free);
  g_signal_connect(page, "hidden", G_CALLBACK(on_page_hidden), d);

  /* Arrow keys: capture phase so they win over the scrolled window. */
  GtkEventController *keys = gtk_event_controller_key_new();
  gtk_event_controller_set_propagation_phase(keys, GTK_PHASE_CAPTURE);
  g_signal_connect(keys, "key-pressed", G_CALLBACK(on_key_pressed), d);
  gtk_widget_add_controller(GTK_WIDGET(page), keys);

  /* Resume at the saved scroll position if we're reopening the chapter
   * the store last recorded for this novel. */
  double fraction = 0.0;
  if (chapter_index >= 0 && (guint)chapter_index < novel->chapters->len) {
    ArkiumChapter *chapter = g_ptr_array_index(novel->chapters, chapter_index);
    g_autofree char *saved_path = NULL;
    double saved_fraction = 0.0;
    if (arkium_store_get_progress(ctx->store, novel->path, &saved_path,
                                   &saved_fraction) &&
        g_strcmp0(saved_path, chapter->path) == 0) {
      fraction = saved_fraction;
    }
  } else {
    chapter_index = 0;
  }
  if (novel->chapters->len == 0) {
    gtk_label_set_text(GTK_LABEL(d->heading), "No chapters");
    gtk_widget_set_sensitive(d->prev_button, FALSE);
    gtk_widget_set_sensitive(d->next_button, FALSE);
  } else {
    load_chapter(d, chapter_index, fraction);
  }

  return GTK_WIDGET(page);
}
