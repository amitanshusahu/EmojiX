#ifndef CALLBACKS_H
#define CALLBACKS_H

#include "gui.h"

#define MAX_RECENT_EMOJIS 20

void copy_to_clipboard(GtkWidget *widget, gpointer data);
void filter_emojis(GtkEntry *entry, gpointer data);
void populate_recent_emojis(AppWidgets *app_widgets);
char* get_recent_emojis_file_path();
void save_recent_emojis(GList *emojis);
GList* load_recent_emojis();
void add_recent_emoji(GList **emojis, const char *emoji);
void check_and_add_recent_emoji(GList **emojis, const char *emoji);
void emoji_button_clicked(GtkWidget *button, gpointer data);
void clear_selected_emojis();
void get_window_position_from_config(GtkWindow *window);
void save_window_position(GtkWindow *window);
char *get_window_position_file_path();
gboolean on_configure_event(GtkWidget *widget, GdkEventConfigure *event, gpointer user_data);

#endif // CALLBACKS_H
