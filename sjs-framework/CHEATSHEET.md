# SJS Framework - Quick Reference Cheat Sheet

## 🚀 Setup

```html
<script>globalThis.configFileSrc = "./config.json";</script>
<script src="./sjs-full.js"></script>
```

## 📦 Module Access

```javascript
const SJS = globalThis.SJS;
const pubSub = SJS.modules.get("pubSub");
const storage = SJS.modules.get("storage");
const logger = SJS.modules.get("logger");
const http = SJS.modules.get("http");
const router = SJS.modules.get("router");
const cache = SJS.modules.get("cache");
const input = SJS.modules.get("input");
const metrics = SJS.modules.get("metrics");
const errorReporter = SJS.modules.get("errorReporter");
const state = SJS.modules.get("state");
```

## 🚨 Error Reporter

```javascript
// Configure
errorReporter.configure({
    enabled: true,
    autoCapture: true,        // Auto-catch errors
    sampleRate: 1.0,          // Report 100% of errors
    persistErrors: true,
    httpEndpoint: "https://errors.example.com/api",
    dedupe: true
});

// Set context
errorReporter.setUser({ id: 123, email: "user@example.com" });
errorReporter.setContext("appVersion", "1.0.0");

// Add breadcrumbs (user actions)
errorReporter.addBreadcrumb("ui", "Button clicked", { id: "submit" });
errorReporter.addBreadcrumb("api", "Fetching data", { endpoint: "/users" });

// Manually capture
try {
    throw new Error("Oops!");
} catch (error) {
    errorReporter.captureError(error, { extra: "context" });
}

// Capture messages
errorReporter.captureMessage("User logged in", "info");
errorReporter.captureMessage("Warning!", "warning");

// View errors
const errors = errorReporter.getErrors(10);
const breadcrumbs = errorReporter.getBreadcrumbs();

// Clear
errorReporter.clearErrors();
errorReporter.clearBreadcrumbs();

// Start/stop auto-capture
errorReporter.start();
errorReporter.stop();
```

## 🔄 State Management

```javascript
// Create store
const store = state.createStore("app", {
    user: null,
    count: 0,
    settings: { theme: "light" }
}, {
    persist: true,
    emitPubSub: true,
    historySize: 50
});

// Get state
const user = store.get("user");
const count = store.get("count");
const theme = store.get("settings.theme");
const all = store.get();  // Entire state

// Set state
store.set("count", 5);
store.set("settings.theme", "dark");

// Update with function
store.update("count", (n) => n + 1);

// Commit mutation (batch updates)
store.commit((state) => {
    state.count++;
    state.user = { id: 1, name: "Ada" };
});

// Define actions
store.actions.increment = (store) => {
    store.update("count", (n) => n + 1);
};

store.actions.login = async (store, user) => {
    store.commit((state) => {
        state.user = user;
    });
};

// Dispatch actions
store.dispatch("increment");
await store.dispatch("login", { id: 1, name: "Ada" });

// Subscribe to changes
const unsub = store.subscribe("count", (newVal, oldVal) => {
    console.log(`Count: ${oldVal} → ${newVal}`);
});

// Watch all changes
const unwatch = store.watch((state, path, oldVal, newVal) => {
    console.log(`Changed ${path}`);
});

// Computed properties
store.computed("fullName", (state) => {
    return `${state.user.first} ${state.user.last}`;
}, ["user"]);

console.log(store.fullName);  // Access computed

// Time-travel
store.undo();
store.redo();
console.log(store.canUndo());
console.log(store.canRedo());

// Reset to initial state
store.reset();

// Cleanup
unsub();
unwatch();
store.destroy();

// Get store by name
const retrieved = state.getStore("app");
```

## 🔔 PubSub

```javascript
// Subscribe
const token = pubSub.subscribe("topic", (data) => {
    console.log(data);
});

// Subscribe once
pubSub.once("topic", (data) => { });

// Publish
pubSub.publish("topic", { key: "value" });

// Unsubscribe
pubSub.unsubscribe(token);

// Count subscribers
pubSub.count("topic");

// List all topics
pubSub.list();
```

## 💾 Storage

```javascript
// Set
await storage.set("key", { data: 123 });

// Set with TTL (expires in 1 hour)
await storage.set("key", value, { ttlMs: 3600000 });

// Get
const value = await storage.get("key");

// Has
const exists = await storage.has("key");

// Remove
await storage.remove("key");

// Clear all
await storage.clear();

// List keys
const keys = await storage.keys();

// Namespaces
const userStore = storage.use("user");
await userStore.set("profile", { id: 1 });

// Export/Import
const data = await storage.export();
await storage.import(data);
```

## 🌐 HTTP

```javascript
// Configure
http.configure({
    baseURL: "https://api.example.com",
    headers: { "Authorization": "Bearer token" },
    timeoutMs: 10000,
    retries: 2,
    cacheGet: true
});

// GET
const data = await http.get("/users");
const user = await http.get("/users/123");

// POST
const created = await http.post("/users", {
    data: { name: "Ada", email: "ada@example.com" }
});

// PUT
const updated = await http.put("/users/123", {
    data: { name: "Ada Lovelace" }
});

// DELETE
await http.del("/users/123");

// With query params
const results = await http.get("/search", {
    params: { q: "javascript", limit: 10 }
});

// Custom headers
const data = await http.get("/protected", {
    headers: { "X-Custom": "value" }
});
```

## 🗺️ Router

```javascript
// Configure
router.configure({
    mode: "history",  // or "hash" or "memory"
    root: "#app",
    basePath: ""
});

// Register templates
router.template.register("home", `
    <h1>Home</h1>
    <p>Welcome, <%- name %></p>
`);

// Set routes
router.setRoutes([
    {
        name: "home",
        path: "/",
        templateName: "home",
        load: () => ({ name: "User" })
    },
    {
        name: "user",
        path: "/users/:id",
        template: "<h1>User <%- params.id %></h1>"
    }
]);

// Start
await router.start();

// Navigate
router.navigate("/users/123");
router.navigate("/", { replace: true });

// Build URL
const url = router.build("user", { id: 123 });

// Back/Forward
router.back();
router.forward();
```

## 💨 Cache

```javascript
// Configure
cache.configure({
    ttlMs: 60000,     // 1 minute
    swrMs: 30000,     // Serve stale for 30s
    maxEntries: 1000
});

// Memoize function
const getUser = cache.memoize("users", async (id) => {
    return await http.get(`/users/${id}`);
}, { ttlMs: 120000 });

// Use memoized function
const user = await getUser(123);  // Fetches
const same = await getUser(123);  // From cache

// Manual cache operations
await cache.set("group", "key", value);
const val = await cache.get("group", "key");
await cache.del("group", "key");
await cache.clear("group");

// Stats
const stats = cache.stats();
// { size, hits, misses, sets, inflight }
```

## 📝 Logger

```javascript
// Create logger
const log = logger.create("app", { level: "info" });

// Log levels
log.debug("Debug message", { data: 123 });
log.info("Info message", { user: "ada" });
log.warn("Warning", { count: 0 });
log.error("Error occurred", { code: 500 });

// Enable/disable namespaces
logger.enable("app:*");
logger.disable("*");

// Child logger
const child = log.child({ requestId: "abc123" });
child.info("Request started");  // Includes requestId

// Timing
log.time("operation");
// ... do work ...
log.timeEnd("operation");  // Logs duration

// Get buffered logs
const logs = logger.getBuffered("app");
```

## 📊 Metrics

```javascript
// Configure
metrics.configure({
    enabled: true,
    persist: true,
    exportIntervalMs: 60000
});

// Start collecting
metrics.start();

// Counters
metrics.counterInc("page_views");
metrics.counterInc("api_calls", 1, { endpoint: "/users" });

// Gauges
metrics.gaugeSet("active_users", 42);
metrics.gaugeInc("queue_size");
metrics.gaugeDec("queue_size");

// Histograms
metrics.histObserve("response_time", 245);

// Timers
const timer = metrics.timeStart("db_query");
// ... query ...
const duration = metrics.timeEnd(timer);

// Async timing
const result = await metrics.timeAsync("fetch_data", async () => {
    return await http.get("/data");
});

// Snapshot
const snapshot = metrics.snapshot();
console.log(snapshot);

// Reset
metrics.reset();
```

## 🎮 Input

```javascript
// Configure
input.configure({
    trackMouse: true,
    trackPointer: true,
    ignoreEditable: true
});

// Start tracking
input.start();

// Query state
const keys = input.keysDown();        // ["w", "a", "shift"]
const isW = input.isKeyDown("w");     // true/false
const mouse = input.getMouse();        // { x, y, dx, dy }
const btns = input.buttonsDown();      // [0, 2]

// Listen via pubSub
pubSub.subscribe("input:keydown", (e) => {
    console.log("Key pressed:", e.key);
});

pubSub.subscribe("input:mousemove", (e) => {
    console.log("Mouse at:", e.x, e.y);
});

// Clear state
input.clear();

// Stop tracking
input.stop();
```

## 🏃 Runner

```javascript
const runner = SJS.modules.get("runner");
const m = new runner.Manifest("app");

// Add entries with dependencies
m.addEntry(new runner.ManifestEntry("A", fnA, []));
m.addEntry(new runner.ManifestEntry("B", fnB, ["A"]));
m.addEntry(new runner.ManifestEntry("C", fnC, ["A"]));
m.addEntry(new runner.ManifestEntry("D", fnD, ["B", "C"]));

// Run (executes in dependency order)
const summary = await runner.run(m);
console.log(summary);
```

## 🎯 Task Manager

```javascript
const taskManager = SJS.modules.get("taskManager");

const tasks = [
    new taskManager.Task("task1", async () => {
        return await doSomething();
    }),
    new taskManager.Task("task2", async () => {
        return await doSomethingElse();
    })
];

// Execute sequentially
const results = await taskManager.asyncManager(tasks);
// [{ id: "task1", value: ... }, { id: "task2", value: ... }]
```

## ⚙️ Config Examples

### Minimal
```json
{
  "main": "./main.js",
  "mainArgs": []
}
```

### Full
```json
{
  "loadManifest": "./manifest.js",
  "main": "./main.js",
  "mainArgs": ["arg1", "arg2"],
  "storage": {
    "driver": "auto",
    "namespace": "app",
    "broadcast": true
  },
  "router": {
    "enabled": true,
    "mode": "history",
    "root": "#app",
    "autoStart": true
  },
  "input": {
    "enabled": true,
    "trackMouse": true
  },
  "metrics": {
    "enabled": true,
    "persist": true,
    "exportIntervalMs": 60000
  }
}
```

## 🎨 Common Patterns

### Event-Driven Update
```javascript
// Publisher
function updateUser(user) {
    pubSub.publish("user:updated", user);
}

// Subscriber
pubSub.subscribe("user:updated", async (user) => {
    await storage.set("currentUser", user);
    renderUI(user);
});
```

### Cached API Calls
```javascript
const fetchUser = cache.memoize("users", async (id) => {
    return await http.get(`/users/${id}`);
}, { ttlMs: 300000 });

const user = await fetchUser(123);
```

### Router with Data Loading
```javascript
router.setRoutes([{
    name: "user",
    path: "/users/:id",
    templateName: "userTpl",
    async load(ctx) {
        const user = await http.get(`/users/${ctx.params.id}`);
        return { user };
    }
}]);
```

### Cross-Tab Sync
```javascript
storage.configure({ broadcast: true });

// Tab 1
await storage.set("theme", "dark");

// Tab 2 automatically receives via pubSub
pubSub.subscribe("storage:set", (e) => {
    if (e.key === "theme") {
        applyTheme(e.value);
    }
});
```

## 🐛 Debug

```javascript
const debug = SJS.modules.get("debug");

debug.debugOn();
debug.debugger("info", "Message", { data: 123 });
debug.debugger("warning", "Warning!");
debug.debugger("err", "Error!", { code: 500 });
debug.debugOff();
```

## 📏 Size Reference

- Full bundle: ~40KB (minified: ~12KB gzipped)
- Load time: < 50ms
- Memory: ~2-5MB (depends on usage)

---

**Copy this file for quick reference while coding!** 📋
