#ifndef GUI_H
#define GUI_H

#include <gtk/gtk.h>

#define NAME "EmojiX"

typedef struct {
  GtkWidget *grid;
  GtkWidget *search_entry;
  GtkWidget *window;
} AppWidgets;

void activate (GtkApplication *app, gpointer user_data);

#endif