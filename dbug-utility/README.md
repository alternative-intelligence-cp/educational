# dBug - Tag-Based Debug Logging Utility

A lightweight, environment-aware debug logging module with tag-based filtering and color formatting.

## Features

- **Environment Detection**: Automatically detects Node.js, Browser, Web Worker, or other JavaScript environments
- **Color-Coded Messages**: Five message types with distinct colors
  - `FAIL` - Red (critical errors)
  - `WARN` - Yellow (warnings)
  - `PASS` - Green (success messages)
  - `LOGS` - White (general logging)
  - `DATA` - Blue (data dumps/debugging)
- **Tag-Based Filtering**: Enable/disable debug output by category tags
- **Universal Module**: Works in Node.js (CommonJS), browsers, and workers
- **Zero Dependencies**: Pure JavaScript implementation

## Installation

```bash
# Copy dbug.js to your project
cp dbug.js /path/to/your/project/
```

## Quick Start

### Node.js
```javascript
const dBug = require('./dbug.js');

dBug.print("Server started on port 3000", "PASS");
dBug.print("Database connection timeout", "FAIL");
```

### Browser
```html
<script src="dbug.js"></script>
<script>
  dBug.print("Page loaded successfully", "PASS");
  dBug.print("API call failed", "FAIL");
</script>
```

## API Reference

### `print(message, type, tag)`

Prints a colored debug message (if enabled).

**Parameters:**
- `message` (string): The message to print
- `type` (string): Message type - one of: `"FAIL"`, `"WARN"`, `"PASS"`, `"LOGS"`, `"DATA"`
- `tag` (string, optional): Category tag for filtering (defaults to `"_default_"`)

**Examples:**
```javascript
dBug.print("User logged in", "PASS");
dBug.print("Invalid credentials", "FAIL", "authentication");
dBug.print('{"userId": 123, "role": "admin"}', "DATA", "session");
```

### `enable(tags)`

Enables debug output for specific tag(s).

**Parameters:**
- `tags` (string | array): Single tag or array of tags to enable

**Special Tags:**
- `"all"` - Prints all messages regardless of tag
- `"_default_"` - Prints messages with no tag specified

**Examples:**
```javascript
dBug.enable("database");              // Enable single tag
dBug.enable(["api", "cache"]);        // Enable multiple tags
dBug.enable("all");                   // Enable everything
```

### `disable(tags)`

Disables debug output for specific tag(s).

**Parameters:**
- `tags` (string | array): Single tag or array of tags to disable

**Examples:**
```javascript
dBug.disable("verbose");
dBug.disable(["performance", "metrics"]);
dBug.disable("all");  // Disable everything
```

### `reset()`

Restores the enabled tags back to default state (`["all", "_default_"]`).

**Use Case:**
If you've disabled tags and want to return to showing all output without remembering what to re-enable.

**Example:**
```javascript
dBug.disable("all");
dBug.enable("section_a");
// ... do some work ...
dBug.reset();  // Back to showing everything
```

### `getEnabled()`

Returns array of currently enabled tags (for diagnostics).

**Returns:**
- `array` - Copy of enabled tags list

**Example:**
```javascript
const active = dBug.getEnabled();
console.log(`Active tags: ${active.join(', ')}`);
// Output: Active tags: all, _default_, database, api
```

## Usage Patterns

### Pattern 1: Default Behavior
By default, all messages print (all tags enabled):

```javascript
dBug.print("Application starting", "LOGS");
dBug.print("Config loaded", "PASS");
// Both print
```

### Pattern 2: Selective Filtering (Main Use Case)
Disable all output, then enable only what you need:

```javascript
dBug.disable("all");
dBug.enable(["database", "api"]);

dBug.print("DB connected", "PASS", "database");      // Prints
dBug.print("Cache miss", "WARN", "cache");           // Doesn't print
dBug.print("API ready", "PASS", "api");              // Prints
```

### Pattern 3: Reset When Done
Return to default behavior after selective debugging:

```javascript
dBug.disable("all");
dBug.enable("section_a");
// ... debug section A ...
dBug.reset();  // Back to normal
```

### Pattern 4: Development vs Production
Toggle verbosity based on environment:

```javascript
if (process.env.NODE_ENV === 'production') {
  dBug.disable("all");
  dBug.enable(["critical", "errors"]);
} else {
  // Development: print everything
  dBug.enable("all");
}
```

### Pattern 5: Modular Debugging
Each module maintains its own debug tag:

```javascript
// database.js
function query(sql) {
  dBug.print(`Executing: ${sql}`, "LOGS", "database");
  // ...
}

// api.js
function handleRequest(req) {
  dBug.print(`${req.method} ${req.url}`, "LOGS", "api");
  // ...
}
```

## Color Formatting

### Terminal (Node.js)
Uses ANSI escape codes:
```
FAIL ===> Database connection failed
WARN ===> Slow query detected
PASS ===> Migration successful
LOGS ===> Server listening on port 3000
DATA ===> {"user": "alice", "id": 42}
```

### Browser Console
Uses CSS styling with `console.log('%c...')`:
- Same heading format with browser-native color rendering
- Inspectable objects remain interactive

## Environment Detection

The module automatically detects:

1. **Node.js**: `process.versions.node` exists
2. **Browser**: `window.document` exists
3. **Web Worker**: `importScripts` or `WorkerGlobalScope` exists
4. **Other**: Deno, Bun, Cloudflare Workers, etc.

Terminal ANSI codes used for Node.js and "other" environments.
Browser CSS styling used for browsers and workers.

## Implementation Details

### Module Pattern
- IIFE (Immediately Invoked Function Expression) for encapsulation
- Frozen public API prevents modification
- No global pollution (exports cleanly)

### Color Constants
```javascript
const RED = 0;
const YELLOW = 1;
const GREEN = 2;
const WHITE = 3;
const BLUE = 4;
```

### Default Tag Set
```javascript
["all", "_default_"]
```

All messages print by default unless filtering is configured.

## Educational Value

This module demonstrates:

1. **Environment Detection**: Cross-platform JavaScript compatibility
2. **Module Pattern**: Encapsulation and clean exports  
3. **Color Formatting**: Platform-specific output styling
4. **Tag Systems**: Flexible categorization and filtering
5. **Defensive Programming**: Protecting critical tags, frozen API
6. **Practical Debugging**: Real-world development tool patterns

## Performance Considerations

- **Minimal Overhead**: Simple tag lookup and string concatenation
- **Production Readiness**: Disable all tags to eliminate debug overhead
- **No External Dependencies**: Won't bloat bundle size
- **Frozen API**: V8 can optimize property access

## Examples in Action

See `dbug_example.js` for comprehensive demonstrations:
- Default all-enabled behavior
- Selective tag filtering  
- Dynamic enable/disable
- Multi-tag workflows
- Diagnostic tag inspection

## Contributing

This module is part of the educational repository. Feel free to:
- Add new message types
- Extend color schemes
- Implement log levels
- Add file output support
- Create browser DevTools integration

## License

Public domain - use freely for education and development.

## Author

Created as an educational example of environment-aware JavaScript module development.

---

**Pro Tip**: Use tags to match your architecture layers (database, cache, api, frontend, auth, etc.) for surgical debugging during development.
