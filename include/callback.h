#ifndef CALLBACKS_H
#define CALLBACKS_H

#define MAX_RECENT_EMOJIS 40

void copy_to_clipboard(GtkWidget *widget, gpointer data);
void filter_emojis(GtkEntry *entry, gpointer data);


#endif // CALLBACKS_H
