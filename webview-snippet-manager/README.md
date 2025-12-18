# Webview Snippet Manager

A lightweight desktop code snippet manager built with C++ and webview.

## Why This Project?

This demonstrates how to build native desktop apps with web UI using the mature `webview/webview` library:

- **Native performance** - C++ backend with file I/O
- **Web UI** - HTML/CSS/JS for familiar, rapid prototyping
- **Cross-platform** - Single codebase for Windows, macOS, Linux
- **Tiny binaries** - Uses OS native webview (~250KB on Linux)
- **Simple C++ ↔ JS bridge** - Clean API, no complexity

## What We Learned

This replaced an earlier attempt with the `saucer` library which:
- Required GCC 14+ (bleeding edge)
- Had compilation errors even with correct compiler
- Was too immature for production use

The `webview/webview` library is:
- Mature (2017+), battle-tested
- C++11 compatible (no bleeding edge requirements)
- Simple, clean API
- Actually compiles and works

## Features

- Create, read, update, delete code snippets
- Store snippets in JSON files
- Search by title, language, or tags
- Syntax-aware editor
- Fast native performance

## Building

```bash
./build.sh
cd build
./snippet-manager
```

## Dependencies

### Linux
```bash
sudo apt-get install libgtk-3-dev libwebkit2gtk-4.1-dev
```

### macOS
- Built-in WebKit framework

### Windows
- WebView2 runtime (built into Windows 11)

## Architecture

**Backend (C++):**
- `src/snippet_manager.cpp` - CRUD operations, file storage
- `src/main.cpp` - Webview integration, JS bridge

**Frontend (HTML/CSS/JS):**
- `frontend/index.html` - UI structure
- `frontend/style.css` - Modern dark theme
- `frontend/app.js` - UI logic, calls C++ via bridge

## Learning Goals

1. Understand native webview architecture
2. Learn C++ ↔ JavaScript interop patterns
3. Build practical desktop tools for Aria compiler (debugger, REPL)
4. Avoid heavyweight frameworks (Electron = 100MB+, this = <1MB)
