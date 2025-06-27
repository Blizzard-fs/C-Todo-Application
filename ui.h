#ifndef UI_H
#define UI_H

#include <gtk/gtk.h>

typedef struct {
    GtkWidget *window;
    GtkWidget *btn_new;
    GtkWidget *btn_edit;
    GtkWidget *btn_remove;
    GtkWidget *btn_pin;
    GtkWidget *list_view;
    GtkWidget *right_stack;

    // Input View Widgets
    GtkWidget *input_box;
    GtkWidget *entry_title;
    GtkWidget *textview_description;
    GtkWidget *calendar_due_date;
    GtkWidget *btn_save;
    GtkWidget *btn_back_from_input;

    // Read-only View Widgets
    GtkWidget *view_box;
    GtkWidget *view_content;

} UIWidgets;

UIWidgets *ui_widgets_new(GtkApplication *app, const char *ui_file);
void ui_widgets_free(UIWidgets *ui);

#endif