#include <gtk/gtk.h>
#include <glib/gstdio.h>
#include "app.h"

static void
load_custom_css(void)
{
    const char *app_config_dir_name = "stefantomatodo";
    gchar *config_dir_path = g_build_filename(g_get_user_config_dir(), app_config_dir_name, NULL);
    gchar *css_path = g_build_filename(config_dir_path, "style.css", NULL);

    // Check if the CSS file exists
    if (!g_file_test(css_path, G_FILE_TEST_EXISTS)) {
        g_print("style.css not found. Creating default stylesheet at %s\n", css_path);

        const char *default_css =
            "/* Default stylesheet for the To-Do application */\n\n"
            "/* Add some padding to the main button bar and list view */\n"
            "box#left_pane {\n"
            "    padding: 6px;\n"
            "}\n\n"
            "/* Style for the 'Pin' button when an item is pinned */\n"
            "button.pinned {\n"
            "    background-color: white;\n"
            "}\n"
            "button.pinned label {\n"
            "    color: black;\n"
            "}\n"
            "button.pinned image {\n"
            "    color: black;\n"
            "}\n\n"
            "/* Add some padding and a subtle background to the read-only view */\n"
            "box#view_box {\n"
            "    background-color: @theme_bg_color;\n"
            "    padding: 12px;\n"
            "    border-radius: 6px;\n"
            "}\n";

        g_mkdir_with_parents(config_dir_path, 0755);

        g_file_set_contents(css_path, default_css, -1, NULL);
    }

    GtkCssProvider *provider = gtk_css_provider_new();
    gtk_css_provider_load_from_path(provider, css_path);
    gtk_style_context_add_provider_for_display(gdk_display_get_default(),
                                               GTK_STYLE_PROVIDER(provider),
                                               GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
    g_object_unref(provider);
    g_free(config_dir_path);
    g_free(css_path);
}


static void
activate(GtkApplication *app_inst, gpointer user_data) {
    load_custom_css();

    AppData *app = g_new0(AppData, 1);
    app_init(app, app_inst);
    g_object_set_data_full(G_OBJECT(app_inst), "app_data", app, (GDestroyNotify) app_cleanup);
}

int
main(int argc, char **argv) {
    GtkApplication *app = gtk_application_new("nl.stefantoma.todo", G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);

    int status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);
    return status;
}