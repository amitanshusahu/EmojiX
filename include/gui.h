#ifndef GUI_H
#define GUI_H

#include <gtk/gtk.h>

#define NAME "EmojiX"

typedef struct {
    GtkWidget *window;
    GtkWidget *grid;
    GtkWidget *recent_grid; // Add this for recent emojis grid
    GtkWidget *search_entry;
    GtkWidget *emoji_book;
    GList *recent_emojis;   // Add this for storing recent emojis
} AppWidgets;

void activate (GtkApplication *app, gpointer user_data);

#endif