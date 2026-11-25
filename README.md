# Text Editor

A lightweight Windows text editor written in C++17 with Qt Widgets. It supports creating, editing, and saving UTF-8 plain-text (`.txt`) files.

## Features

- Create, open, save, and save a copy of a text file.
- Edit Unicode text, including Cyrillic.
- Preserve the UTF-8 BOM and the file's line-ending style for files with consistent LF or CRLF endings.
- Get a prompt to save unsaved changes before creating a new document, opening another file, or closing the editor.

## Requirements

- Visual Studio with the **Desktop development with C++** workload and CMake support.
- Qt 6.5 or later with the **MSVC 2022 64-bit** kit.

## Build and run

1. Set the `Qt6_ROOT` user environment variable to the Qt kit directory, for example `C:\Qt\6.12.1\msvc2022_64`. Restart Visual Studio if it was open.
2. In Visual Studio, select **File → Open → Folder** and open the folder containing `CMakeLists.txt`.
3. Select the **Windows x64 Debug** preset and wait for CMake to configure the project.
4. Select **TextEditor.exe** as the startup item and press **F5**.

## Keyboard shortcuts

| Shortcut | Action |
| --- | --- |
| `Ctrl + N` | New document |
| `Ctrl + O` | Open a text file |
| `Ctrl + S` | Save |
| `Ctrl + Shift + S` | Save As |

## Project files

| File | Purpose |
| --- | --- |
| `main.cpp` | Editor window, file operations, and keyboard shortcuts |
| `CMakeLists.txt` | Build rules and Qt dependencies |
| `CMakePresets.json` | Windows x64 Debug build configuration |
