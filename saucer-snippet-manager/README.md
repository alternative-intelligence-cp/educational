# Saucer Snippet Manager

A lightweight desktop code snippet manager built with **Saucer** - exploring C++ webview capabilities.

## What This Is

An experiment to learn [Saucer](https://github.com/saucer/saucer) by building something useful:
- Save and organize code snippets
- Syntax highlighting
- Run snippets and capture output
- All in a tiny native desktop app

## Features

- 📝 Create/Edit/Delete snippets
- 🎨 Syntax highlighting (multiple languages)
- ▶️  Run code and see output
- 💾 Save snippets to disk
- 🔍 Search/Filter snippets
- 🪶 Ultra-lightweight (~250KB binary)

## Architecture

**Backend (C++):**
- File I/O for snippet storage
- Process execution for running code
- JSON parsing for snippet metadata
- All exposed to JavaScript via saucer bridge

**Frontend (HTML/CSS/JS):**
- Monaco Editor for code editing
- Simple UI for snippet management
- Calls C++ functions via `saucer.exposed.*`

## Learning Goals

1. Understand saucer's C++ ↔ JS bridge
2. Learn resource embedding
3. Practice FFI patterns (useful for Nitpick later)
4. Explore cross-platform desktop development
5. Test saucer's performance and limitations

## Building

```bash
mkdir build && cd build
cmake ..
make
./snippet-manager
```

## Project Structure

```
saucer-snippet-manager/
├── CMakeLists.txt           # Build configuration
├── src/
│   ├── main.cpp            # Entry point, saucer setup
│   ├── snippet_manager.hpp # Snippet management logic
│   └── snippet_manager.cpp
├── frontend/
│   ├── index.html          # Main UI
│   ├── style.css           # Styling
│   └── app.js              # Frontend logic
└── snippets/               # Where snippets are saved
```

## Why This Project?

**Educational Value:**
- Hands-on with saucer before potentially using it for Nitpick tools
- Learn webview patterns for future debugger/REPL UIs
- Practice C++ FFI design (directly applicable to Nitpick)
- Small enough to complete in a session, useful enough to keep

**Practical Value:**
- Actually useful tool for daily dev work
- Replace heavy Electron apps with tiny native binary
- Quick way to test code snippets
- Organized snippet library

## Next Steps

After this works, we could:
1. Add Nitpick syntax highlighting (dogfooding!)
2. Use it as a test case for Nitpick's FFI
3. Build Nitpick language bindings for saucer
4. Use similar patterns for Nitpick debugger UI

---

Let's see what saucer can do! 🚀
