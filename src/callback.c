#include <gtk/gtk.h>
#include "emoji.h"
#include "gui.h"

void copy_to_clipboard(GtkWidget *widget, gpointer data) {
    const char *emoji = (const char *)data;
    GtkClipboard *clipboard = gtk_clipboard_get(GDK_SELECTION_CLIPBOARD);
    gtk_clipboard_set_text(clipboard, emoji, -1);
}

void filter_emojis(GtkEntry *entry, gpointer data) {
    AppWidgets *app_widgets = (AppWidgets *)data;
    const char *search_text = gtk_entry_get_text(entry);

    // Remove all current children from the grid
    GList *children, *iter;
    children = gtk_container_get_children(GTK_CONTAINER(app_widgets->grid));
    for (iter = children; iter != NULL; iter = g_list_next(iter)) {
        gtk_widget_destroy(GTK_WIDGET(iter->data));
    }
    g_list_free(children);

    // Add emojis that match the search text
    int col = 0, row = 0;
    for (size_t i = 0; i < NUM_EMOJIS; ++i) {
        if (strstr(emojis[i].keywords, search_text) != NULL) {
            GtkWidget *button = gtk_button_new_with_label(emojis[i].emoji);
            g_signal_connect(button, "clicked", G_CALLBACK(copy_to_clipboard), (gpointer)emojis[i].emoji);
            gtk_grid_attach(GTK_GRID(app_widgets->grid), button, col, row, 1, 1);
            col++;
            if (col >= 5) {
                col = 0;
                row++;
            }
        }
    }
    gtk_widget_show_all(app_widgets->grid);
}