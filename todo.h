#ifndef TODO_H
#define TODO_H
#include <gtk/gtk.h>

typedef struct {
    gchar *title;
    gchar *description;
    GDate *due_date;
    gboolean pinned;
    gchar *filepath;
} TodoItem;

GList *todo_load_all(const char *folder);
void todo_free_item(TodoItem *item);
gboolean todo_save_item(const char *folder, TodoItem *item);
gboolean todo_delete_item(TodoItem *item);
gint todo_item_compare(gconstpointer a, gconstpointer b);

#endif //TODO_H
