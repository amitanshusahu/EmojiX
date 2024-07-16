#include <gtk/gtk.h>
#include "emoji.h"
#include "gui.h"
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <json-c/json.h>

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

char* get_recent_emojis_file_path() {
    const char *data_home = getenv("XDG_DATA_HOME");
    if (!data_home) {
        data_home = g_strconcat(g_get_home_dir(), "/.local/share", NULL);
    }

    char *dir_path = g_strconcat(data_home, "/emoji_picker", NULL);
    char *file_path = g_strconcat(dir_path, "/recent_emojis.json", NULL);

    // Create the directory if it doesn't exist
    struct stat st = {0};
    if (stat(dir_path, &st) == -1) {
        mkdir(dir_path, 0700);
    }

    g_free(dir_path);
    return file_path;
}

void save_recent_emojis(GList *emojis) {
    char *file_path = get_recent_emojis_file_path();
    json_object *jarray = json_object_new_array();

    for (GList *l = emojis; l != NULL; l = l->next) {
        json_object *jstring = json_object_new_string((char *)l->data);
        json_object_array_add(jarray, jstring);
    }

    FILE *fp = fopen(file_path, "w");
    if (fp != NULL) {
        fprintf(fp, "%s\n", json_object_to_json_string(jarray));
        fclose(fp);
    }

    json_object_put(jarray);
    g_free(file_path);
}

GList* load_recent_emojis() {
    char *file_path = get_recent_emojis_file_path();
    GList *emojis = NULL;
    FILE *fp = fopen(file_path, "r");
    if (fp != NULL) {
        fseek(fp, 0, SEEK_END);
        long fsize = ftell(fp);
        fseek(fp, 0, SEEK_SET);

        char *json_string = malloc(fsize + 1);
        fread(json_string, fsize, 1, fp);
        json_string[fsize] = 0;
        fclose(fp);

        json_object *jarray = json_tokener_parse(json_string);
        if (jarray != NULL) {
            int array_len = json_object_array_length(jarray);
            for (int i = 0; i < array_len; i++) {
                json_object *jstring = json_object_array_get_idx(jarray, i);
                const char *emoji = json_object_get_string(jstring);
                emojis = g_list_append(emojis, g_strdup(emoji));
            }
            json_object_put(jarray);
        }
        free(json_string);
    }
    g_free(file_path);
    return emojis;
}

void add_recent_emoji(GList **emojis, const char *emoji) {
    *emojis = g_list_remove(*emojis, emoji); // Remove if already in list
    *emojis = g_list_prepend(*emojis, g_strdup(emoji)); // Add to the front

    if (g_list_length(*emojis) > MAX_RECENT_EMOJIS) {
        GList *last = g_list_last(*emojis);
        *emojis = g_list_remove_link(*emojis, last);
        g_free(last->data);
        g_list_free(last);
    }
}

void print_recent_emojis() {
    char *file_path = get_recent_emojis_file_path();
    FILE *fp = fopen(file_path, "r");
    if (fp != NULL) {
        fseek(fp, 0, SEEK_END);
        long fsize = ftell(fp);
        fseek(fp, 0, SEEK_SET);

        char *json_string = malloc(fsize + 1);
        fread(json_string, fsize, 1, fp);
        json_string[fsize] = 0;
        fclose(fp);

        json_object *jarray = json_tokener_parse(json_string);
        if (jarray != NULL) {
            int array_len = json_object_array_length(jarray);
            printf("Recently used emojis:\n");
            for (int i = 0; i < array_len; i++) {
                json_object *jstring = json_object_array_get_idx(jarray, i);
                const char *emoji = json_object_get_string(jstring);
                printf("%s\n", emoji);
            }
            json_object_put(jarray);
        } else {
            printf("No recently used emojis found.\n");
        }
        free(json_string);
    } else {
        printf("No recently used emojis file found.\n");
    }
    g_free(file_path);
}