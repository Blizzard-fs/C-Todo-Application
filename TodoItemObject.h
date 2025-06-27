//
// Created by stefan on 6/27/25.
//
#ifndef TODO_ITEM_OBJECT_H
#define TODO_ITEM_OBJECT_H

#include <glib-object.h>
#include "todo.h"

G_BEGIN_DECLS

#define TODO_ITEM_TYPE_OBJECT (todo_item_object_get_type())
G_DECLARE_FINAL_TYPE(TodoItemObject, todo_item_object, TODO_ITEM, OBJECT, GObject)

TodoItemObject *todo_item_object_new(TodoItem *item);
TodoItem *todo_item_object_get_todo(TodoItemObject *self);

G_END_DECLS

#endif // TODO_ITEM_OBJECT_H
