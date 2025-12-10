# SJS Framework - Simple JavaScript Application System

**A lightweight, modular JavaScript framework for building applications in both browser and Node.js environments.**

## 🎯 Overview

SJS (Simple JavaScript System) is an educational framework that demonstrates best practices for:
- **Modular architecture** - Clean separation of concerns
- **Dependency management** - Topological resolution of module dependencies
- **Pub/Sub messaging** - Event-driven communication between components
- **Cross-platform storage** - Unified API for IndexedDB, localStorage, and Node.js fs
- **Structured logging** - Leveled, namespaced logging with multiple sinks
- **HTTP client** - Fetch-based client with retries, caching, and interceptors
- **Client-side routing** - SPA routing with template engine
- **Input handling** - Keyboard/mouse state management
- **Metrics collection** - Performance monitoring and observability
- **Task orchestration** - Async task execution with proper error handling

## 📁 Project Structure

```
sjs-framework/
├── README.md              # This file
├── core/
│   ├── sjs-full.js       # Complete bundle (all modules)
│   ├── sjs-core.js       # Core only (bootstrapper + modules system)
│   └── sjs-minimal.js    # Ultra-minimal (just module system)
├── modules/
│   ├── runner.js         # Dependency resolution and script execution
│   ├── taskManager.js    # Async task orchestration
│   ├── pubSub.js         # Publish/subscribe messaging
│   ├── storage.js        # Cross-platform key/value storage
│   ├── logger.js         # Structured logging
│   ├── debug.js          # Simple debug toggler
│   ├── cache.js          # Memoization with TTL/SWR
│   ├── http.js           # HTTP client
│   ├── input.js          # Keyboard/mouse input
│   ├── metrics.js        # Performance metrics
│   └── router.js         # Client-side routing + templates
├── examples/
│   ├── 01-hello-world/
│   ├── 02-pubsub-demo/
│   ├── 03-storage-demo/
│   ├── 04-router-spa/
│   ├── 05-http-client/
│   └── 06-full-app/
└── docs/
    ├── QUICKSTART.md
    ├── API-REFERENCE.md
    ├── ARCHITECTURE.md
    └── MODULES.md
```

## 🚀 Quick Start

### Option 1: Auto-Boot (Recommended)

```html
<!DOCTYPE html>
<html>
<head>
    <title>SJS App</title>
    <script>
        // Define config before loading SJS
        globalThis.configFileSrc = "./config.json";
    </script>
    <script src="./core/sjs-full.js"></script>
</head>
<body>
    <div id="app"></div>
</body>
</html>
```

**config.json:**
```json
{
  "loadManifest": "./manifest.js",
  "main": "./main.js",
  "mainArgs": ["hello", "world"]
}
```

**manifest.js:**
```javascript
export default function buildManifest(SJS, runner) {
    const m = new runner.Manifest("app");
    m.addEntry(new runner.ManifestEntry("init", () => {
        console.log("App initialized!");
    }, []));
    return m;
}
```

**main.js:**
```javascript
export default async function main(SJS, ...args) {
    console.log("Main started with:", args);
    const pubSub = SJS.modules.get("pubSub");
    pubSub.publish("app:ready", { timestamp: Date.now() });
    return true;
}
```

### Option 2: Manual Boot

```javascript
// After loading sjs-full.js
SJS.boot({
    main: async (SJS) => {
        console.log("App started!");
        const storage = SJS.modules.get("storage");
        await storage.set("initialized", true);
    },
    mainArgs: []
});
```

### Option 3: Direct Module Usage

```javascript
// Load sjs-full.js first, then:
const pubSub = SJS.modules.get("pubSub");
const storage = SJS.modules.get("storage");

// Subscribe to events
pubSub.subscribe("user:login", (user) => {
    console.log("User logged in:", user);
});

// Use storage
await storage.set("user", { id: 1, name: "Ada" });
const user = await storage.get("user");
```

## 🎓 Educational Value

This framework teaches:

1. **Module Pattern** - IIFE-based encapsulation without bundlers
2. **Dependency Injection** - Loose coupling through module registry
3. **Async Patterns** - Promises, async/await, task orchestration
4. **Event-Driven Architecture** - Pub/sub for decoupled communication
5. **Cross-Platform Abstraction** - Unified API across environments
6. **Error Handling** - Comprehensive error recovery strategies
7. **Performance Optimization** - Caching, lazy loading, debouncing
8. **Testing Patterns** - How to structure testable code

## 📚 Core Concepts

### Module System

Every module follows this pattern:

```javascript
(function (modules) {
    if (!modules) {
        console.error("Module system not available");
        return;
    }

    // Module implementation
    const MyModule = {
        doSomething() { /* ... */ }
    };

    // Register module
    modules.add("myModule", Object.freeze(MyModule));
})(globalThis.SJS?.modules || null);
```

### Bootstrapper

The bootstrapper orchestrates application startup:

1. **Load config** - From file or inline object
2. **Load manifest** - Dependency graph of initialization tasks
3. **Run manifest** - Execute tasks in topological order
4. **Execute main** - Call main function with arguments

### Dependency Resolution

The runner module performs topological sort to execute tasks in correct order:

```javascript
const m = new runner.Manifest("app");
m.addEntry(new runner.ManifestEntry("A", fnA, []));
m.addEntry(new runner.ManifestEntry("B", fnB, ["A"])); // B depends on A
m.addEntry(new runner.ManifestEntry("C", fnC, ["A"])); // C depends on A
m.addEntry(new runner.ManifestEntry("D", fnD, ["B", "C"])); // D depends on B and C

// Executes: A, then B and C in parallel, then D
runner.run(m);
```

## 🔧 Configuration

**Complete config.json example:**

```json
{
  "loadManifest": "./manifest.js",
  "main": "./main.js",
  "mainArgs": [],
  "storage": {
    "driver": "auto",
    "namespace": "app",
    "idbName": "SJS_KV",
    "idbStore": "kv",
    "localStoragePrefix": "sjs:",
    "filePath": "./sjs_store.json",
    "broadcast": true
  },
  "router": {
    "enabled": true,
    "mode": "history",
    "basePath": "",
    "root": "#app",
    "autoStart": true,
    "linksSelector": "a[href]",
    "linkActiveClass": "active",
    "scrollBehavior": "auto"
  },
  "input": {
    "enabled": true,
    "capture": false,
    "preventDefault": false,
    "ignoreEditable": true
  },
  "metrics": {
    "enabled": true,
    "persist": true,
    "exportIntervalMs": 60000,
    "autoLongTask": true
  }
}
```

## 📦 Available Modules

| Module | Purpose | Key Features |
|--------|---------|--------------|
| **runner** | Script execution | Dependency resolution, async support |
| **taskManager** | Task orchestration | Sequential async execution |
| **pubSub** | Messaging | Subscribe, publish, once, async support |
| **storage** | Persistence | IndexedDB/localStorage/fs, TTL, namespaces |
| **logger** | Logging | Levels, namespaces, sinks, child loggers |
| **debug** | Debug toggle | Colorized output, on/off toggle |
| **cache** | Memoization | TTL, SWR, de-duplication |
| **http** | HTTP client | Retries, timeout, caching, interceptors |
| **input** | Input handling | Keyboard/mouse state, events |
| **metrics** | Performance metrics | Counters, gauges, histograms, timers |
| **router** | Client-side routing | History/hash, templates, hooks |
| **errorReporter** | Error tracking | Auto-capture, breadcrumbs, aggregation, reporting |
| **state** | State management | Reactive store, computed props, time-travel, persistence |

## 🎯 Use Cases

### 1. Error Tracking & Reporting

```javascript
const errorReporter = SJS.modules.get("errorReporter");

errorReporter.configure({
    autoCapture: true,
    httpEndpoint: "https://errors.example.com/api"
});

// Errors are automatically caught and reported
// Add context for debugging
errorReporter.setUser({ id: 123, email: "user@example.com" });
errorReporter.addBreadcrumb("ui", "Button clicked", { id: "submit" });
```

### 2. Reactive State Management

```javascript
const state = SJS.modules.get("state");

const store = state.createStore("app", { count: 0 }, { persist: true });

store.subscribe("count", (newCount) => {
    document.getElementById("counter").textContent = newCount;
});

store.set("count", 5);  // UI updates automatically
store.undo();           // Time-travel back to 0
```

### 3. Single Page Application (SPA)

```javascript
const router = SJS.modules.get("router");

router.setRoutes([
    { name: "home", path: "/", template: "<h1>Home</h1>" },
    { name: "about", path: "/about", template: "<h1>About</h1>" },
    { name: "user", path: "/user/:id", template: "<h1>User <%- params.id %></h1>" }
]);

router.start();
```

### 4. Data Fetching with Caching

```javascript
const http = SJS.modules.get("http");
const cache = SJS.modules.get("cache");

http.configure({ baseURL: "https://api.example.com", cacheGet: true });

const fetchUser = cache.memoize("user", async (id) => {
    return await http.get(`/users/${id}`);
}, { ttlMs: 60000 });

// First call hits API, subsequent calls use cache
const user1 = await fetchUser(123);
const user2 = await fetchUser(123); // From cache
```

### 5. Event-Driven Communication

```javascript
const pubSub = SJS.modules.get("pubSub");

// Module A
pubSub.subscribe("user:logout", () => {
    console.log("Clearing user data...");
});

// Module B
pubSub.subscribe("user:logout", () => {
    console.log("Redirecting to login...");
});

// Trigger from anywhere
pubSub.publish("user:logout");
```

### 6. Performance Monitoring

```javascript
const metrics = SJS.modules.get("metrics");

metrics.configure({ enabled: true, persist: true });

// Track counters
metrics.counterInc("page_views");

// Track timing
const timer = metrics.timeStart("api_call");
await fetchData();
metrics.timeEnd(timer);

// Export metrics
const snapshot = metrics.snapshot();
console.log(snapshot);
```

## 🐛 Common Issues & Solutions

### Issue: Module not found
**Solution:** Ensure the module is loaded before accessing it:
```javascript
const mod = SJS.modules.get("myModule");
if (!mod) {
    console.error("Module not loaded yet");
}
```

### Issue: Config file not loading
**Solution:** Check file path and CORS headers (browser) or file permissions (Node.js)

### Issue: Router not working
**Solution:** Ensure router is started and routes are registered:
```javascript
router.setRoutes([...]); // First register routes
router.start();           // Then start
```

## 📖 Further Reading

- [Quick Start Guide](./docs/QUICKSTART.md)
- [API Reference](./docs/API-REFERENCE.md)
- [Architecture Overview](./docs/ARCHITECTURE.md)
- [Module Documentation](./docs/MODULES.md)

## 🤝 Contributing

This is an educational project. Contributions that improve clarity, add examples, or fix bugs are welcome!

## 📄 License

MIT License - Free to use for learning and teaching.

## 🎓 Learning Path

1. **Start with core** - Understand module system and bootstrapper
2. **Add pubSub** - Learn event-driven patterns
3. **Add storage** - Persist data across sessions
4. **Add router** - Build single-page apps
5. **Add http** - Fetch external data
6. **Add metrics** - Monitor performance

Each module builds on the previous, teaching progressively advanced concepts.

---

**Happy Learning! 🚀**
