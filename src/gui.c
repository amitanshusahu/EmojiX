#include <gtk/gtk.h>
#include "callback.h"
#include "gui.h"
#include "emoji.h"

void activate(GtkApplication *app, gpointer user_data)
{
  GtkWidget *window;
  GtkWidget *parent_grid;
  GtkWidget *emoji_grid;
  GtkWidget *search_entry;
  GtkWidget *emoji_book;
  GtkWidget *scrolled_window;

  // Create a window
  window = gtk_application_window_new(app);
  gtk_window_set_title(GTK_WINDOW(window), NAME);
  gtk_container_set_border_width(GTK_CONTAINER(window), 10);

  // Create a CSS provider and load CSS from a string
  GtkCssProvider *provider = gtk_css_provider_new();
  const gchar *css_data =
      "#search_entry{"
      "}"
      "#emoji_grid button{"
      "margin-top: 5px;"
      "padding: 2px 5px;"
      "}"
      ".notebook tab {"
      "  padding: 5px;"
      "}"
      ".notebook {"
      "  border: none;"
      "}"
      ".scrolled-window {"
      "  padding: 5px;"
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
  search_entry = gtk_entry_new();
  gtk_widget_set_name(search_entry, "search_entry"); // for css

  // Attach search entry to parent_grid
  gtk_grid_attach(GTK_GRID(parent_grid), search_entry, 0, 0, 6, 1);

  // Create a notebook
  emoji_book = gtk_notebook_new();

  // Attach notebook to parent_grid
  gtk_grid_attach(GTK_GRID(parent_grid), emoji_book, 0, 1, 6, 10);

  // Create a scrolled window
  scrolled_window = gtk_scrolled_window_new(NULL, NULL);
  gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
  gtk_widget_set_size_request(scrolled_window, 0, 200); // Set minimum height

  // Create emoji_grid for emojis
  emoji_grid = gtk_grid_new();
  gtk_widget_set_name(emoji_grid, "emoji_grid"); // for css
  gtk_grid_set_row_spacing(GTK_GRID(emoji_grid), 5);
  gtk_grid_set_column_spacing(GTK_GRID(emoji_grid), 5);

  // Add emoji_grid into scrolled window
  gtk_container_add(GTK_CONTAINER(scrolled_window), emoji_grid);

  // Add the scrolled window to the notebook tab
  gtk_notebook_append_page(GTK_NOTEBOOK(emoji_book), scrolled_window, gtk_label_new("all"));


  // Dynamically allocate AppWidgets
  AppWidgets *app_widgets = g_malloc(sizeof(AppWidgets));
  app_widgets->grid = emoji_grid;
  app_widgets->search_entry = search_entry;
  app_widgets->window = window;

  // Connect the "changed" signal of search_entry to the filter_emojis callback
  g_signal_connect(search_entry, "changed", G_CALLBACK(filter_emojis), app_widgets);

  // Initial population of emojis
  filter_emojis(GTK_ENTRY(app_widgets->search_entry), app_widgets);

  // Show all widgets in the window
  gtk_widget_show_all(window);
}