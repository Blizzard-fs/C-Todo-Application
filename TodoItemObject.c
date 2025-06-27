#include "TodoItemObject.h"

struct _TodoItemObject
{
    GObject parent_instance;
    TodoItem *todo;
};

G_DEFINE_TYPE(TodoItemObject, todo_item_object, G_TYPE_OBJECT)

static void todo_item_object_finalize(GObject *object)
{
    TodoItemObject *self = (TodoItemObject *)object;
    // We don't free the TodoItem here because ownership remains outside
    self->todo = NULL;
    G_OBJECT_CLASS(todo_item_object_parent_class)->finalize(object);
}

static void todo_item_object_class_init(TodoItemObjectClass *klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS(klass);
    object_class->finalize = todo_item_object_finalize;
}

static void todo_item_object_init(TodoItemObject *self)
{
    self->todo = NULL;
}

TodoItemObject *todo_item_object_new(TodoItem *item)
{
    TodoItemObject *obj = g_object_new(TODO_ITEM_TYPE_OBJECT, NULL);
    obj->todo = item;
    return obj;
}

TodoItem *todo_item_object_get_todo(TodoItemObject *self)
{
    return self->todo;
}
