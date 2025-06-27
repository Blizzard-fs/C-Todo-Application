#include "app.h"
#include "TodoItemObject.h"
#include <time.h>
#include <gtk/gtk.h>
#include <gio/gio.h>
#include <glib/gstdio.h>

#define TODO_FOLDER ".local/todo/items"

// --- Function Prototypes ---
static void app_reload_list(AppData *app);

static void app_on_new_clicked(GtkButton *button, AppData *app);

static void app_on_edit_clicked(GtkButton *button, AppData *app);

static void app_on_delete_clicked(GtkButton *button, AppData *app);

static void app_on_pin_clicked(GtkButton *button, AppData *app);

static void app_on_back_from_input_clicked(GtkButton *button, AppData *app);

// --- GAction Activation Callbacks ---
static void action_new_activated(GSimpleAction *action, GVariant *p, gpointer user_data) {
    app_on_new_clicked(NULL, user_data);
}

static void action_edit_activated(GSimpleAction *action, GVariant *p, gpointer user_data) {
    app_on_edit_clicked(NULL, user_data);
}

static void action_delete_activated(GSimpleAction *action, GVariant *p, gpointer user_data) {
    app_on_delete_clicked(NULL, user_data);
}

static void action_pin_activated(GSimpleAction *action, GVariant *p, gpointer user_data) {
    app_on_pin_clicked(NULL, user_data);
}


// --- UI Logic ---
static void
setup_list_item(GtkListItemFactory *factory, GtkListItem *list_item) {
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);
    GtkWidget *icon = gtk_image_new();
    GtkWidget *label = gtk_label_new(NULL);
    gtk_widget_set_halign(label, GTK_ALIGN_START);
    gtk_box_append(GTK_BOX(box), icon);
    gtk_box_append(GTK_BOX(box), label);
    gtk_list_item_set_child(list_item, box);
}

static void
bind_list_item(GtkListItemFactory *factory, GtkListItem *list_item) {
    GtkWidget *box = gtk_list_item_get_child(list_item);
    GtkWidget *icon = gtk_widget_get_first_child(box);
    GtkWidget *label = gtk_widget_get_next_sibling(icon);
    TodoItemObject *obj = gtk_list_item_get_item(list_item);
    if (obj) {
        TodoItem *todo = todo_item_object_get_todo(obj);
        if (todo) {
            gtk_label_set_text(GTK_LABEL(label), todo->title ? todo->title : "");
            gtk_image_set_from_icon_name(GTK_IMAGE(icon), todo->pinned ? "emblem-favorite-symbolic" : NULL);
        }
    }
}

static void
app_load_todo_to_view(AppData *app, TodoItem *item) {
    gchar *escaped_title = g_markup_escape_text(item->title, -1);
    gchar *escaped_desc = g_markup_escape_text(item->description ? item->description : "", -1);
    gchar *content = g_strdup_printf(
        "<span size='xx-large' font_weight='bold'>%s</span>\n\n"
        "<b>Due:</b> %04d-%02d-%02d\n"
        "____________________________________\n\n"
        "%s",
        escaped_title,
        g_date_get_year(item->due_date), g_date_get_month(item->due_date), g_date_get_day(item->due_date),
        escaped_desc);
    gtk_label_set_markup(GTK_LABEL(app->ui->view_content), content);
    g_free(escaped_title);
    g_free(escaped_desc);
    g_free(content);
    gtk_stack_set_visible_child_name(GTK_STACK(app->ui->right_stack), "view");
}

static void
app_load_todo_to_input(AppData *app, TodoItem *item) {
    if (item) {
        // Editing existing item
        GtkEntryBuffer *buffer = gtk_entry_get_buffer(GTK_ENTRY(app->ui->entry_title));
        gtk_entry_buffer_set_text(buffer, item->title ? item->title : "", -1);
        GtkTextBuffer *text_buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(app->ui->textview_description));
        gtk_text_buffer_set_text(text_buffer, item->description ? item->description : "", -1);
        if (item->due_date) {
            gtk_calendar_set_year(GTK_CALENDAR(app->ui->calendar_due_date), g_date_get_year(item->due_date));
            gtk_calendar_set_month(GTK_CALENDAR(app->ui->calendar_due_date), g_date_get_month(item->due_date) - 1);
            gtk_calendar_set_day(GTK_CALENDAR(app->ui->calendar_due_date), g_date_get_day(item->due_date));
        }
    } else {
        // Creating new item
        GtkEntryBuffer *buffer = gtk_entry_get_buffer(GTK_ENTRY(app->ui->entry_title));
        gtk_entry_buffer_set_text(buffer, "", -1);
        GtkTextBuffer *text_buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(app->ui->textview_description));
        gtk_text_buffer_set_text(text_buffer, "", -1);
        time_t now = time(NULL);
        struct tm *tm_now = localtime(&now);
        gtk_calendar_set_year(GTK_CALENDAR(app->ui->calendar_due_date), tm_now->tm_year + 1900);
        gtk_calendar_set_month(GTK_CALENDAR(app->ui->calendar_due_date), tm_now->tm_mon);
        gtk_calendar_set_day(GTK_CALENDAR(app->ui->calendar_due_date), tm_now->tm_mday);
    }
    gtk_stack_set_visible_child_name(GTK_STACK(app->ui->right_stack), "input");
}

static void
on_selection_changed(GtkSingleSelection *self, GParamSpec *pspec, gpointer user_data) {
    AppData *app = user_data;
    guint position = gtk_single_selection_get_selected(self);
    gboolean has_selection = (position != GTK_INVALID_LIST_POSITION);

    gtk_widget_set_sensitive(app->ui->btn_edit, has_selection);
    gtk_widget_set_sensitive(app->ui->btn_remove, has_selection);
    gtk_widget_set_sensitive(app->ui->btn_pin, has_selection);
    GActionMap *action_map = G_ACTION_MAP(gtk_window_get_application(GTK_WINDOW(app->ui->window)));
    g_simple_action_set_enabled(G_SIMPLE_ACTION(g_action_map_lookup_action(action_map, "edit")), has_selection);
    g_simple_action_set_enabled(G_SIMPLE_ACTION(g_action_map_lookup_action(action_map, "delete")), has_selection);
    g_simple_action_set_enabled(G_SIMPLE_ACTION(g_action_map_lookup_action(action_map, "pin")), has_selection);

    if (has_selection) {
        TodoItemObject *obj = g_list_model_get_item(G_LIST_MODEL(app->store), position);
        if (obj) {
            TodoItem *item = todo_item_object_get_todo(obj);
            app->current_todo = item;
            app_load_todo_to_view(app, item);
            if (item->pinned) gtk_widget_add_css_class(app->ui->btn_pin, "pinned");
            else gtk_widget_remove_css_class(app->ui->btn_pin, "pinned");
            g_object_unref(obj);
        }
    } else {
        app->current_todo = NULL;
        gtk_stack_set_visible_child_name(GTK_STACK(app->ui->right_stack), "placeholder");
        gtk_widget_remove_css_class(app->ui->btn_pin, "pinned");
    }
}


// --- Action Handlers ---
static void app_on_new_clicked(GtkButton *button, AppData *app) {
    gtk_single_selection_set_selected(app->selection_model, GTK_INVALID_LIST_POSITION);
    app->current_todo = NULL;
    app_load_todo_to_input(app, NULL);
}

static void app_on_edit_clicked(GtkButton *button, AppData *app) {
    if (app->current_todo) app_load_todo_to_input(app, app->current_todo);
}

static void
app_on_back_from_input_clicked(GtkButton *button, AppData *app) {
    on_selection_changed(app->selection_model, NULL, app); // Go back to view or placeholder
}

static void app_reload_list(AppData *app) {
    g_list_store_remove_all(app->store);
    for (GList *l = app->todos; l != NULL; l = l->next) {
        g_list_store_append(app->store, todo_item_object_new(l->data));
    }
}

static void
app_on_save_clicked(GtkButton *button, AppData *app) {
    GDateTime *date = gtk_calendar_get_date(GTK_CALENDAR(app->ui->calendar_due_date));
    GtkEntryBuffer *title_buffer = gtk_entry_get_buffer(GTK_ENTRY(app->ui->entry_title));
    const gchar *new_title_const = gtk_entry_buffer_get_text(title_buffer);
    if (!new_title_const || *new_title_const == '\0') {
        g_date_time_unref(date);
        return;
    }

    gchar *saved_title = g_strdup(new_title_const);

    GtkTextBuffer *text_buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(app->ui->textview_description));
    GtkTextIter start, end;
    gtk_text_buffer_get_bounds(text_buffer, &start, &end);
    gchar *description = gtk_text_buffer_get_text(text_buffer, &start, &end, FALSE);
    TodoItem *item = app->current_todo;
    gchar *old_filepath = NULL;

    if (item) {
        if (item->filepath) old_filepath = g_strdup(item->filepath);
    } else {
        item = g_new0(TodoItem, 1);
    }

    gboolean title_changed = (item->title == NULL || g_strcmp0(item->title, saved_title) != 0);
    g_free(item->title);
    item->title = g_strdup(saved_title);
    g_free(item->description);
    item->description = description;
    if (!item->due_date) item->due_date = g_date_new();
    g_date_set_dmy(item->due_date, g_date_time_get_day_of_month(date), g_date_time_get_month(date),
                   g_date_time_get_year(date));

    if (app->current_todo) item->pinned = app->current_todo->pinned;

    if (todo_save_item(TODO_FOLDER, item)) {
        if (old_filepath && title_changed) g_remove(old_filepath);
    }
    g_free(old_filepath);

    g_list_free_full(app->todos, (GDestroyNotify) todo_free_item);
    app->todos = todo_load_all(TODO_FOLDER);
    app_reload_list(app);

    guint i = 0;
    for (GList *l = app->todos; l != NULL; l = l->next, i++) {
        TodoItem *current_item_in_list = l->data;
        if (g_strcmp0(current_item_in_list->title, saved_title) == 0) {
            gtk_single_selection_set_selected(app->selection_model, i);
            break;
        }
    }

    g_date_time_unref(date);
    g_free(saved_title); // was g_free(description) - potential bug, now fixed to free the right pointer
}

static void
app_on_pin_clicked(GtkButton *button, AppData *app) {
    if (!app->current_todo) return;
    gchar *saved_title = g_strdup(app->current_todo->title);

    app->current_todo->pinned = !app->current_todo->pinned;
    todo_save_item(TODO_FOLDER, app->current_todo);
    g_list_free_full(app->todos, (GDestroyNotify) todo_free_item);
    app->todos = todo_load_all(TODO_FOLDER);
    app_reload_list(app);

    guint i = 0;
    for (GList *l = app->todos; l != NULL; l = l->next, i++) {
        TodoItem *current_item_in_list = l->data;
        if (g_strcmp0(current_item_in_list->title, saved_title) == 0) {
            gtk_single_selection_set_selected(app->selection_model, i);
            break;
        }
    }
    g_free(saved_title);
}

static void
app_on_delete_clicked(GtkButton *button, AppData *app) {
    if (!app->current_todo) return;
    if (todo_delete_item(app->current_todo)) {
        gtk_single_selection_set_selected(app->selection_model, GTK_INVALID_LIST_POSITION);
        g_list_free_full(app->todos, (GDestroyNotify) todo_free_item);
        app->todos = todo_load_all(TODO_FOLDER);
        app_reload_list(app);
    }
}

void
app_init(AppData *app, GtkApplication *app_inst) {
    g_mkdir_with_parents(g_build_filename(g_get_home_dir(), TODO_FOLDER, NULL), 0700);

    const GActionEntry app_entries[] = {
        {"new", action_new_activated, NULL, NULL, NULL},
        {"edit", action_edit_activated, NULL, NULL, NULL},
        {"delete", action_delete_activated, NULL, NULL, NULL},
        {"pin", action_pin_activated, NULL, NULL, NULL}
    };
    g_action_map_add_action_entries(G_ACTION_MAP(app_inst), app_entries, G_N_ELEMENTS(app_entries), app);
    gtk_application_set_accels_for_action(app_inst, "app.new", (const char *[]){"<Primary><Shift>A", NULL});
    gtk_application_set_accels_for_action(app_inst, "app.edit", (const char *[]){"<Primary><Shift>E", NULL});
    gtk_application_set_accels_for_action(app_inst, "app.delete", (const char *[]){"<Primary><Shift>D", NULL});
    gtk_application_set_accels_for_action(app_inst, "app.pin", (const char *[]){"<Primary><Shift>P", NULL});

    app->ui = ui_widgets_new(app_inst, "builder.ui");
    GtkListItemFactory *factory = gtk_signal_list_item_factory_new();
    g_signal_connect(factory, "setup", G_CALLBACK(setup_list_item), NULL);
    g_signal_connect(factory, "bind", G_CALLBACK(bind_list_item), NULL);
    gtk_list_view_set_factory(GTK_LIST_VIEW(app->ui->list_view), factory);
    g_object_unref(factory);

    app->store = g_list_store_new(TODO_ITEM_TYPE_OBJECT);
    app->selection_model = gtk_single_selection_new(G_LIST_MODEL(app->store));
    gtk_list_view_set_model(GTK_LIST_VIEW(app->ui->list_view), GTK_SELECTION_MODEL(app->selection_model));

    app->todos = todo_load_all(TODO_FOLDER);
    app_reload_list(app);

    g_signal_connect(app->selection_model, "notify::selected", G_CALLBACK(on_selection_changed), app);
    g_signal_connect(app->ui->btn_new, "clicked", G_CALLBACK(app_on_new_clicked), app);
    g_signal_connect(app->ui->btn_edit, "clicked", G_CALLBACK(app_on_edit_clicked), app);
    g_signal_connect(app->ui->btn_remove, "clicked", G_CALLBACK(app_on_delete_clicked), app);
    g_signal_connect(app->ui->btn_pin, "clicked", G_CALLBACK(app_on_pin_clicked), app);
    g_signal_connect(app->ui->btn_save, "clicked", G_CALLBACK(app_on_save_clicked), app);
    g_signal_connect(app->ui->btn_back_from_input, "clicked", G_CALLBACK(app_on_back_from_input_clicked), app);

    on_selection_changed(app->selection_model, NULL, app);
    gtk_window_present(GTK_WINDOW(app->ui->window));
}

void
app_cleanup(AppData *app) {
    if (app->todos) g_list_free_full(app->todos, (GDestroyNotify) todo_free_item);
    if (app->selection_model) g_object_unref(app->selection_model);
    ui_widgets_free(app->ui);
}
