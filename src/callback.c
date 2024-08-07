#include <gtk/gtk.h>
#include "emoji.h"
#include "gui.h"
#include "callback.h"
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <json-c/json.h>

void copy_to_clipboard(GtkWidget *widget, gpointer data)
{
    const char *emoji = (const char *)data;
    GtkClipboard *clipboard = gtk_clipboard_get(GDK_SELECTION_CLIPBOARD);
    gtk_clipboard_set_text(clipboard, emoji, -1);

    // Add the emoji to recent list
    AppWidgets *app_widgets = g_object_get_data(G_OBJECT(widget), "app_widgets");

    // Get the currently active tab
    GtkNotebook *notebook = GTK_NOTEBOOK(app_widgets->emoji_book);
    int active_tab = gtk_notebook_get_current_page(notebook);

    // Assume that tab 0 is the emoji tab and tab 1 is the recent tab
    if (active_tab == 0)
    {
        // Emoji tab is active
        check_and_add_recent_emoji(&app_widgets->recent_emojis, emoji);
    }
    else if (active_tab == 1)
    {
        // Recent tab is active
        add_recent_emoji(&app_widgets->recent_emojis, emoji);
    }

    save_recent_emojis(app_widgets->recent_emojis);
    populate_recent_emojis(app_widgets);
}

void clear_selected_emojis()
{
    g_list_free_full(selected_emojis, g_free);
    selected_emojis = NULL;
}

void emoji_button_clicked(GtkWidget *button, gpointer data)
{
    const char *emoji = (const char *)data;

    // Check if Shift key is pressed
    GdkModifierType state;
    gdk_window_get_pointer(gtk_widget_get_window(GTK_WIDGET(button)), NULL, NULL, &state);
    gboolean shift_pressed = (state & GDK_SHIFT_MASK);

    if (shift_pressed)
    {
        // Add emoji to selected list
        selected_emojis = g_list_append(selected_emojis, g_strdup(emoji));
        const char *emoji = (const char *)data;
        GtkClipboard *clipboard = gtk_clipboard_get(GDK_SELECTION_CLIPBOARD);

        if (selected_emojis != NULL)
        {
            // Shift key was pressed and multiple emojis are selected
            GString *copy_text = g_string_new("");
            for (GList *iter = selected_emojis; iter != NULL; iter = g_list_next(iter))
            {
                const char *selected_emoji = (const char *)iter->data;
                g_string_append(copy_text, selected_emoji);
                g_string_append_c(copy_text, ' '); // Example: Space separated
            }
            gtk_clipboard_set_text(clipboard, copy_text->str, -1);
            g_string_free(copy_text, TRUE);
        }
    }
    else
    {
        // Single emoji selected, copy to clipboard
        clear_selected_emojis();
        copy_to_clipboard(button, emoji);
    }
}

void filter_emojis(GtkEntry *entry, gpointer data)
{
    AppWidgets *app_widgets = (AppWidgets *)data;
    const char *search_text = gtk_entry_get_text(entry);

    // check active tab if not emoji change to emoji
    GtkNotebook *notebook = GTK_NOTEBOOK(app_widgets->emoji_book);
    if (gtk_notebook_get_current_page(notebook) != 0)
        gtk_notebook_set_current_page(notebook, 0);

    // Remove all current children from the grid
    GList *children, *iter;
    children = gtk_container_get_children(GTK_CONTAINER(app_widgets->grid));
    for (iter = children; iter != NULL; iter = g_list_next(iter))
    {
        gtk_widget_destroy(GTK_WIDGET(iter->data));
    }
    g_list_free(children);

    // Add emojis that match the search text
    int col = 0, row = 0;
    for (size_t i = 0; i < NUM_EMOJIS; ++i)
    {
        if (strstr(emojis[i].keywords, search_text) != NULL)
        {
            GtkWidget *button = gtk_button_new_with_label(emojis[i].emoji);
            g_signal_connect(button, "clicked", G_CALLBACK(emoji_button_clicked), (gpointer)emojis[i].emoji);
            g_object_set_data(G_OBJECT(button), "app_widgets", app_widgets); // set app_widgets data
            gtk_grid_attach(GTK_GRID(app_widgets->grid), button, col, row, 1, 1);
            col++;
            if (col >= 5)
            {
                col = 0;
                row++;
            }
        }
    }
    gtk_widget_show_all(app_widgets->grid);
}

void populate_recent_emojis(AppWidgets *app_widgets)
{
    // Remove all current children from the recent grid
    GList *children, *iter;
    children = gtk_container_get_children(GTK_CONTAINER(app_widgets->recent_grid));
    for (iter = children; iter != NULL; iter = g_list_next(iter))
    {
        gtk_widget_destroy(GTK_WIDGET(iter->data));
    }
    g_list_free(children);

    // Load recent emojis
    app_widgets->recent_emojis = load_recent_emojis();

    // Add recent emojis to the grid
    int col = 0, row = 0;
    for (GList *l = app_widgets->recent_emojis; l != NULL; l = l->next)
    {
        GtkWidget *button = gtk_button_new_with_label((char *)l->data);
        g_signal_connect(button, "clicked", G_CALLBACK(copy_to_clipboard), (gpointer)l->data);
        g_object_set_data(G_OBJECT(button), "app_widgets", app_widgets); // set app_widgets data
        gtk_grid_attach(GTK_GRID(app_widgets->recent_grid), button, col, row, 1, 1);
        col++;
        if (col >= 5)
        {
            col = 0;
            row++;
        }
    }
    gtk_widget_show_all(app_widgets->recent_grid);
}

char *get_recent_emojis_file_path()
{
    const char *data_home = getenv("XDG_DATA_HOME");
    if (!data_home)
    {
        data_home = g_strconcat(g_get_home_dir(), "/.local/share", NULL);
    }

    char *dir_path = g_strconcat(data_home, "/emojix", NULL);
    char *file_path = g_strconcat(dir_path, "/recent_emojis.json", NULL);

    // Create the directory if it doesn't exist
    struct stat st = {0};
    if (stat(dir_path, &st) == -1)
    {
        mkdir(dir_path, 0700);
    }

    g_free(dir_path);
    return file_path;
}

void save_recent_emojis(GList *emojis)
{
    char *file_path = get_recent_emojis_file_path();
    json_object *jarray = json_object_new_array();

    for (GList *l = emojis; l != NULL; l = l->next)
    {
        json_object *jstring = json_object_new_string((char *)l->data);
        json_object_array_add(jarray, jstring);
    }

    FILE *fp = fopen(file_path, "w");
    if (fp != NULL)
    {
        fprintf(fp, "%s\n", json_object_to_json_string(jarray));
        fclose(fp);
    }

    json_object_put(jarray);
    g_free(file_path);
}

GList *load_recent_emojis()
{
    char *file_path = get_recent_emojis_file_path();
    GList *emojis = NULL;
    FILE *fp = fopen(file_path, "r");
    if (fp != NULL)
    {
        fseek(fp, 0, SEEK_END);
        long fsize = ftell(fp);
        fseek(fp, 0, SEEK_SET);

        char *json_string = malloc(fsize + 1);
        fread(json_string, fsize, 1, fp);
        json_string[fsize] = 0;
        fclose(fp);

        json_object *jarray = json_tokener_parse(json_string);
        if (jarray != NULL)
        {
            int array_len = json_object_array_length(jarray);
            for (int i = 0; i < array_len; i++)
            {
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

void add_recent_emoji(GList **emojis, const char *emoji)
{
    *emojis = g_list_remove(*emojis, emoji);            // Remove if already in list
    *emojis = g_list_prepend(*emojis, g_strdup(emoji)); // Add to the front

    if (g_list_length(*emojis) > MAX_RECENT_EMOJIS)
    {
        GList *last = g_list_last(*emojis);
        *emojis = g_list_remove_link(*emojis, last);
        g_free(last->data);
        g_list_free(last);
    }
}

void check_and_add_recent_emoji(GList **emojis, const char *emoji)
{
    GList *existing = g_list_find_custom(*emojis, emoji, (GCompareFunc)strcmp);
    if (existing)
    {
        *emojis = g_list_remove_link(*emojis, existing);
        g_free(existing->data);
        g_list_free(existing);
    }
    *emojis = g_list_prepend(*emojis, g_strdup(emoji)); // Add to the front

    if (g_list_length(*emojis) > MAX_RECENT_EMOJIS)
    {
        GList *last = g_list_last(*emojis);
        *emojis = g_list_remove_link(*emojis, last);
        g_free(last->data);
        g_list_free(last);
    }
}

// window positon persistance

char *get_window_position_file_path()
{
    const char *data_home = getenv("XDG_DATA_HOME");
    if (!data_home)
    {
        data_home = g_strconcat(g_get_home_dir(), "/.local/share", NULL);
    }

    char *dir_path = g_strconcat(data_home, "/emojix", NULL);
    char *file_path = g_strconcat(dir_path, "/window_position.json", NULL);

    // Create the directory if it doesn't exist
    struct stat st = {0};
    if (stat(dir_path, &st) == -1)
    {
        mkdir(dir_path, 0700);
    }

    g_free(dir_path);
    return file_path;
}

// Function to save window position to JSON file
void save_window_position(GtkWindow *window)
{
    char *file_path = get_window_position_file_path();
    gint x, y;
    gtk_window_get_position(window, &x, &y);

    // Create JSON object and add window position
    json_object *jobj = json_object_new_object();
    json_object_object_add(jobj, "x", json_object_new_int(x));
    json_object_object_add(jobj, "y", json_object_new_int(y));

    // Save JSON object to file
    FILE *file = fopen(file_path, "w");
    if (file)
    {
        fputs(json_object_to_json_string_ext(jobj, JSON_C_TO_STRING_PRETTY), file);
        fclose(file);
    }

    // Free JSON object
    json_object_put(jobj);
}

// Function to get window position from JSON file
void get_window_position_from_config(GtkWindow *window)
{
    char *file_path = get_window_position_file_path();
    // Open the configuration file
    FILE *file = fopen(file_path, "r");
    if (file)
    {
        // Parse the JSON data from the file
        struct json_object *jobj = json_object_from_file(file_path);
        if (jobj)
        {
            struct json_object *jx, *jy;
            // Extract x and y coordinates from the JSON object
            if (json_object_object_get_ex(jobj, "x", &jx) &&
                json_object_object_get_ex(jobj, "y", &jy))
            {
                gint x = json_object_get_int(jx);
                gint y = json_object_get_int(jy);
                // Set the window position
                gtk_window_move(window, x, y);
            }
            // Free the JSON object
            json_object_put(jobj);
        }
        fclose(file);
    }
    else
    {
        // Handle the case where the file does not exist or cannot be opened
        g_print("Window position file not found or could not be opened.\n");
    }
}

gboolean on_configure_event(GtkWidget *widget, GdkEventConfigure *event, gpointer user_data) {
    save_window_position(GTK_WINDOW(widget));

    return FALSE; // Return FALSE to propagate the event further
}