#include <gtk/gtk.h>
#include "callback.h"
#include "gui.h"
#include "emoji.h"
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <json-c/json.h>

void activate(GtkApplication *app, gpointer user_data)
{
  GtkWidget *window;
  GtkWidget *parent_grid;
  GtkWidget *emoji_grid;
  GtkWidget *search_entry;
  GtkWidget *emoji_book;
  GtkWidget *scrolled_window;
  GtkWidget *recent_grid;
  GtkWidget *recent_scrolled_window;

  // Create a window
  window = gtk_application_window_new(app);
  gtk_window_set_title(GTK_WINDOW(window), NAME);
  gtk_container_set_border_width(GTK_CONTAINER(window), 10);
  gtk_window_set_decorated(GTK_WINDOW(window), FALSE);        // no window decorations
  gtk_window_set_resizable(GTK_WINDOW(window), FALSE);        // no window resizing
  gtk_window_set_skip_taskbar_hint(GTK_WINDOW(window), TRUE); // Skip taskbar
  gtk_window_set_keep_above(GTK_WINDOW(window), TRUE);        // Keep window above everyting

  // Create a CSS provider and load CSS from a string
  GtkCssProvider *provider = gtk_css_provider_new();
  const gchar *css_data =
      "#search_entry{"
      "}"
      "#emoji_grid button{"
      "  padding: 5px 5px;"
      "  margin: 2px;"
      "  min-width: 0;"
      "  font-size: 18px;"
      "}"
      "#notebook tab {"
      "  margin: 0px;"
      "  padding: 0px;"
      "  min-width: 20px;"
      "  min-height: 20px;"
      "  font-size: 12px;"
      "}"
      "#notebook {"
      "  border: none;"
      "  min-width: 0;"
      "}"
      "#scrolled-window {"
      "  margin: 1px;"
      "}";
  gtk_css_provider_load_from_data(provider, css_data, -1, NULL);
  GdkScreen *screen = gdk_screen_get_default();
  gtk_style_context_add_provider_for_screen(screen, GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_USER);

  // Create a parent_grid container
  parent_grid = gtk_grid_new();
  gtk_grid_set_row_spacing(GTK_GRID(parent_grid), 10);

  // Add parent_grid to window
  gtk_container_add(GTK_CONTAINER(window), parent_grid);

  // Create a search entry
  search_entry = gtk_search_entry_new();
  gtk_widget_set_hexpand(search_entry, TRUE); // to fill
  gtk_widget_set_size_request(search_entry, 193, -1);
  gtk_widget_set_name(search_entry, "search_entry"); // for css

  // Attach search entry to parent_grid
  gtk_grid_attach(GTK_GRID(parent_grid), search_entry, 0, 0, 1, 1);

  // Create a notebook
  emoji_book = gtk_notebook_new();
  gtk_widget_set_name(emoji_book, "notebook"); // for css

  // Attach notebook to parent_grid
  gtk_grid_attach(GTK_GRID(parent_grid), emoji_book, 0, 1, 1, 10);

  // Create a scrolled window
  scrolled_window = gtk_scrolled_window_new(NULL, NULL);
  gtk_widget_set_name(scrolled_window, "scrolled-window"); // for css
  gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
  gtk_widget_set_size_request(scrolled_window, 0, 200); // Set minimum height

  // Create emoji_grid for emojis
  emoji_grid = gtk_grid_new();
  gtk_widget_set_name(emoji_grid, "emoji_grid"); // for css

  // Add emoji_grid into scrolled window
  gtk_container_add(GTK_CONTAINER(scrolled_window), emoji_grid);

  // Add the scrolled window to the notebook tab
  // tab -1 all
  gtk_notebook_append_page(GTK_NOTEBOOK(emoji_book), scrolled_window, gtk_label_new("😊"));

  // Create a scrolled window for recent emojis
  recent_scrolled_window = gtk_scrolled_window_new(NULL, NULL);
  gtk_widget_set_name(recent_scrolled_window, "scrolled-window"); // for css
  gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(recent_scrolled_window), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
  gtk_widget_set_size_request(recent_scrolled_window, 0, 200); // Set minimum height

  // Create recent_grid for recent emojis
  recent_grid = gtk_grid_new();
  gtk_widget_set_name(recent_grid, "emoji_grid"); // for css

  // Add recent_grid into recent scrolled window
  gtk_container_add(GTK_CONTAINER(recent_scrolled_window), recent_grid);

  // Add the recent scrolled window to the notebook tab
  // tab - recent
  gtk_notebook_append_page(GTK_NOTEBOOK(emoji_book), recent_scrolled_window, gtk_label_new("Recent"));

  // Dynamically allocate AppWidgets
  AppWidgets *app_widgets = g_malloc(sizeof(AppWidgets));
  app_widgets->grid = emoji_grid;
  app_widgets->recent_grid = recent_grid; // add recent_grid to app_widgets
  app_widgets->search_entry = search_entry;
  app_widgets->emoji_book = emoji_book;
  app_widgets->window = window;

  // Connect the "changed" signal of search_entry to the filter_emojis callback
  g_signal_connect(search_entry, "changed", G_CALLBACK(filter_emojis), app_widgets);

  // Initial population of emojis
  filter_emojis(GTK_ENTRY(app_widgets->search_entry), app_widgets);
  populate_recent_emojis(app_widgets); // populate recent emojis

  // Show all widgets in the window
  gtk_widget_show_all(window);
}