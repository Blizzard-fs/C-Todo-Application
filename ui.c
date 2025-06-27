#include "ui.h"

UIWidgets *
ui_widgets_new(GtkApplication *app, const char *ui_file) {
    GError *error = NULL;
    GtkBuilder *builder = gtk_builder_new();

    if (!gtk_builder_add_from_file(builder, ui_file, &error)) {
        g_printerr("Error loading UI file: %s\n", error->message);
        g_error_free(error);
        return NULL;
    }

    UIWidgets *ui = g_new0(UIWidgets, 1);
    ui->window = GTK_WIDGET(gtk_builder_get_object(builder, "window"));
    gtk_window_set_application(GTK_WINDOW(ui->window), app);

    ui->btn_new = GTK_WIDGET(gtk_builder_get_object(builder, "btn_new"));
    ui->btn_edit = GTK_WIDGET(gtk_builder_get_object(builder, "btn_edit"));
    ui->btn_remove = GTK_WIDGET(gtk_builder_get_object(builder, "btn_remove"));
    ui->btn_pin = GTK_WIDGET(gtk_builder_get_object(builder, "btn_pin"));
    ui->list_view = GTK_WIDGET(gtk_builder_get_object(builder, "list_view"));
    ui->right_stack = GTK_WIDGET(gtk_builder_get_object(builder, "right_stack"));

    // Input View
    ui->input_box = GTK_WIDGET(gtk_builder_get_object(builder, "input_box"));
    ui->entry_title = GTK_WIDGET(gtk_builder_get_object(builder, "entry_title"));
    ui->textview_description = GTK_WIDGET(gtk_builder_get_object(builder, "textview_description"));
    ui->calendar_due_date = GTK_WIDGET(gtk_builder_get_object(builder, "calendar_due_date"));
    ui->btn_save = GTK_WIDGET(gtk_builder_get_object(builder, "btn_save"));
    ui->btn_back_from_input = GTK_WIDGET(gtk_builder_get_object(builder, "btn_back_from_input"));

    // Read-only View
    ui->view_box = GTK_WIDGET(gtk_builder_get_object(builder, "view_box"));
    ui->view_content = GTK_WIDGET(gtk_builder_get_object(builder, "view_content"));

    g_object_unref(builder);
    return ui;
}

void
ui_widgets_free(UIWidgets *ui) {
    g_free(ui);
}