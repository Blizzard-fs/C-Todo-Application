//
// Created by stefan on 6/27/25.
//
#ifndef APP_H
#define APP_H

#include "ui.h"
#include "todo.h"
#include <gtk/gtk.h>

typedef struct {
    UIWidgets *ui;
    GList *todos;
    TodoItem *current_todo;
    GListStore *store;
    GtkSingleSelection *selection_model;
} AppData;

void app_init(AppData *app, GtkApplication *app_inst);
void app_cleanup(AppData *app);

#endif