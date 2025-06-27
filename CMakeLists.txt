cmake_minimum_required(VERSION 3.16)
project(todo C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK4 REQUIRED gtk4)

add_executable(todo main.c
        ui.h
        ui.c
        todo.c
        todo.h
        app.c
        app.h
        TodoItemObject.c
        TodoItemObject.h)

target_include_directories(todo PRIVATE ${GTK4_INCLUDE_DIRS})
target_link_libraries(todo PRIVATE ${GTK4_LIBRARIES})
target_compile_options(todo PRIVATE ${GTK4_CFLAGS_OTHER})
