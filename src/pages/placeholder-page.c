#include "placeholder-page.h"

GtkWidget *
arkium_placeholder_page_new(const char *title, const char *tag,
                             const char *message) {
  GtkWidget *status = adw_status_page_new();
  adw_status_page_set_icon_name(ADW_STATUS_PAGE(status), "hourglass-symbolic");
  adw_status_page_set_title(ADW_STATUS_PAGE(status), title);
  adw_status_page_set_description(ADW_STATUS_PAGE(status), message);

  GtkWidget *toolbar_view = adw_toolbar_view_new();
  adw_toolbar_view_add_top_bar(ADW_TOOLBAR_VIEW(toolbar_view),
                                adw_header_bar_new());
  adw_toolbar_view_set_content(ADW_TOOLBAR_VIEW(toolbar_view), status);

  AdwNavigationPage *page =
      ADW_NAVIGATION_PAGE(adw_navigation_page_new(toolbar_view, title));
  adw_navigation_page_set_tag(page, tag);
  return GTK_WIDGET(page);
}
