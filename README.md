# C GTK To-Do Application

A full-featured, graphical to-do list application built with C and the GTK4 toolkit. This project demonstrates a modern, event-driven C application with a responsive user interface, file-based data persistence, and a clean, maintainable structure.

## Features

This application was built from the ground up to include a robust set of features for managing daily tasks:

-   **Master-Detail UI**: A professional, two-pane layout where the to-do list is always visible on the left, and the details of the selected item are shown on the right.
-   **Full CRUD Functionality**:
    -   **Create**: Add new to-do items with a title, a multi-line description, and a due date selected from a calendar widget.
    -   **Read**: View a formatted, read-only display of any to-do item.
    -   **Update**: Edit any aspect of an existing to-do item. Renaming an item intelligently cleans up the old file.
    -   **Delete**: Permanently remove to-do items.
-   **Pinning System**:
    -   Pin important items to keep them at the top of the list, visually marked with a star icon.
    -   The "Pin" button provides clear visual feedback, highlighting when a selected item is pinned.
-   **Data Persistence**:
    -   To-do items are saved as individual Markdown (`.md`) files in `~/.local/todo/items/`.
    -   The application automatically creates and loads a custom stylesheet from `~/.config/stefantomatodo/style.css` for easy customization.
-   **Keyboard Shortcuts**: A full set of accelerators for power users:
    -   `Ctrl+Shift+A`: Add New Item
    -   `Ctrl+Shift+E`: Edit Selected Item
    -   `Ctrl+Shift+D`: Delete Selected Item
    -   `Ctrl+Shift+P`: Pin/Unpin Selected Item

## Building and Running

The project is built using CMake.

### Dependencies

You must have the GTK4 development libraries installed. On a Debian/Ubuntu-based system, you can install them with:
```sh
sudo apt-get install libgtk-4-dev
```

On an Arch-based system:

```sh
sudo pacman -S gtk4
```

### Build Instructions

1.  **Clone the repository:**

    ```sh
    git clone git@github.com:Blizzard-fs/C-Todo-Application.git
    cd C-Todo-Application
    ```

2.  **Configure and build with CMake:**

    ```sh
    cmake -B cmake-build-debug -S .
    cmake --build cmake-build-debug
    ```

    (These are the commands that an IDE like CLion runs in the background).

3.  **Run the application:**

    ```sh
    ./cmake-build-debug/todo
    ```

## Usage

  - Use the **New**, **Edit**, **Delete**, and **Pin** buttons to manage the selected to-do item.
  - Select an item from the list on the left to see its formatted content on the right.
  - Click **Edit** to modify the selected item, and **Save** or **Back** when you are done.
  - Use the keyboard shortcuts listed above for faster navigation.
