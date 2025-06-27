#include "todo.h"
#include <glib/gstdio.h>
#include <stdio.h>
#include <time.h>

#define TODO_FOLDER ".local/todo/items"

gint
todo_item_compare(gconstpointer a, gconstpointer b)
{
    const TodoItem *item_a = a;
    const TodoItem *item_b = b;

    // Pinned items always come before unpinned ones
    if (item_a->pinned && !item_b->pinned) {
        return -1; // a comes before b
    }
    if (!item_a->pinned && item_b->pinned) {
        return 1; // b comes before a
    }

    // If both have the same pinned status, sort alphabetically by title
    return g_strcmp0(item_a->title, item_b->title);
}

static gchar *
get_todo_filepath(const char *folder, const gchar *title) {
    return g_build_filename(g_get_home_dir(), folder, title, NULL);
}

GList *
todo_load_all(const char *folder) {
    GList *todos = NULL;
    gchar *path = g_build_filename(g_get_home_dir(), folder, NULL);
    GDir *dir = g_dir_open(path, 0, NULL);
    if (!dir) {
        g_free(path);
        return NULL;
    }

    const gchar *filename;
    while ((filename = g_dir_read_name(dir)) != NULL) {
        if (!g_str_has_suffix(filename, ".md"))
            continue;

        gchar *filepath = g_build_filename(path, filename, NULL);
        GError *error = NULL;
        gchar *contents = NULL;
        if (g_file_get_contents(filepath, &contents, NULL, &error)) {
            TodoItem *item = g_new0(TodoItem, 1);
            item->filepath = g_strdup(filepath);

            item->title = g_strdup(filename);
            item->title[strlen(filename) - 3] = '\0';

            item->pinned = FALSE;
            item->description = NULL;
            item->due_date = g_date_new();

            GStrv lines = g_strsplit(contents, "\n", 0);
            for (int i = 0; lines[i] != NULL; i++) {
                if (g_str_has_prefix(lines[i], "Pin:")) {
                    if (g_strcmp0(lines[i] + 4, " 1") == 0)
                        item->pinned = TRUE;
                } else if (g_str_has_prefix(lines[i], "Due:")) {
                    GDate *d = g_date_new();
                    g_date_set_parse(d, lines[i] + 5);
                    if (g_date_valid(d)) {
                        g_date_set_year(item->due_date, g_date_get_year(d));
                        g_date_set_month(item->due_date, g_date_get_month(d));
                        g_date_set_day(item->due_date, g_date_get_day(d));
                    }
                    g_date_free(d);
                } else if (lines[i][0] != '\0' && !g_str_has_prefix(lines[i], "Pin:") && !g_str_has_prefix(
                               lines[i], "Due:")) {
                    if (!item->description)
                        item->description = g_strdup(lines[i]);
                    else {
                        gchar *tmp = g_strdup_printf("%s\n%s", item->description, lines[i]);
                        g_free(item->description);
                        item->description = tmp;
                    }
                }
            }
            g_strfreev(lines);
            g_free(contents);
            todos = g_list_prepend(todos, item);
        } else {
            g_printerr("Error reading %s: %s\n", filepath, error->message);
            g_error_free(error);
        }
        g_free(filepath);
    }
    g_dir_close(dir);
    g_free(path);

    // Sort the list instead of reversing it
    todos = g_list_sort(todos, todo_item_compare);

    return todos;
}

void
todo_free_item(TodoItem *item) {
    if (!item)
        return;
    g_free(item->title);
    g_free(item->description);
    g_date_free(item->due_date);
    g_free(item->filepath);
    g_free(item);
}

gboolean
todo_save_item(const char *folder, TodoItem *item) {
    gchar *dir = g_build_filename(g_get_home_dir(), folder, NULL);
    g_mkdir_with_parents(dir, 0755);
    g_free(dir);

    gchar *filename = g_strdup_printf("%s.md", item->title);
    gchar *filepath = g_build_filename(g_get_home_dir(), folder, filename, NULL);
    g_free(filename);

    FILE *f = fopen(filepath, "w");
    if (!f) {
        g_printerr("Failed to open file for writing: %s\n", filepath);
        g_free(filepath);
        return FALSE;
    }

    fprintf(f, "Pin: %d\n", item->pinned ? 1 : 0);
    fprintf(f, "Due: %04d-%02d-%02d\n", g_date_get_year(item->due_date), g_date_get_month(item->due_date),
            g_date_get_day(item->due_date));
    fprintf(f, "\n%s\n", item->description ? item->description : "");
    fclose(f);

    g_free(filepath);
    return TRUE;
}

gboolean
todo_delete_item(TodoItem *item) {
    if (!item || !item->filepath)
        return FALSE;

    return g_remove(item->filepath) == 0;
}