#include <gtk/gtk.h>
#include "callback.h"
#include "gui.h"
#include "emoji.h"
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <json-c/json.h>


int main(int argc, char **argv) {
    GtkApplication *app;
    int status;

    app = gtk_application_new("org.amitanshu.emojix", G_APPLICATION_FLAGS_NONE);
    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
    status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);

    return status;
}