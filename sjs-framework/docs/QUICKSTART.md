# SJS Framework - Quick Start Guide

## Installation

### Browser (CDN-style)

```html
<!DOCTYPE html>
<html>
<head>
    <title>My SJS App</title>
    <script src="../core/sjs-full.js"></script>
</head>
<body>
    <div id="app"></div>
    <script>
        // Your app code here
        const pubSub = SJS.modules.get("pubSub");
        pubSub.publish("app:loaded", {});
    </script>
</body>
</html>
```

### Node.js (ESM)

```javascript
// Load SJS (assuming it's in same directory)
import './sjs-full.js';

// Access global SJS
const SJS = globalThis.SJS;
const storage = SJS.modules.get("storage");

await storage.set("test", { value: 42 });
console.log(await storage.get("test"));
```

## Your First App

### Step 1: Create Project Structure

```
my-app/
├── index.html
├── config.json
├── manifest.js
├── main.js
└── sjs-full.js  (copy from core/)
```

### Step 2: Create index.html

```html
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>My SJS App</title>
    <style>
        body { font-family: system-ui; max-width: 800px; margin: 40px auto; padding: 0 20px; }
        #app { min-height: 400px; }
    </style>
    <script>
        // Tell SJS where to find config
        globalThis.configFileSrc = "./config.json";
    </script>
    <script src="./sjs-full.js"></script>
</head>
<body>
    <h1>My SJS Application</h1>
    <div id="app">Loading...</div>
</body>
</html>
```

### Step 3: Create config.json

```json
{
  "loadManifest": "./manifest.js",
  "main": "./main.js",
  "mainArgs": ["Welcome to SJS!"],
  "storage": {
    "driver": "auto",
    "namespace": "myapp"
  },
  "router": {
    "enabled": true,
    "mode": "hash",
    "root": "#app",
    "autoStart": true
  }
}
```

### Step 4: Create manifest.js

```javascript
// manifest.js - Define initialization tasks
export default function buildManifest(SJS, runner) {
    const m = new runner.Manifest("myapp");
    
    // Task 1: Initialize storage
    m.addEntry(new runner.ManifestEntry("init-storage", async () => {
        const storage = SJS.modules.get("storage");
        await storage.set("app_version", "1.0.0");
        console.log("✅ Storage initialized");
    }, []));
    
    // Task 2: Set up event listeners (depends on storage)
    m.addEntry(new runner.ManifestEntry("init-events", () => {
        const pubSub = SJS.modules.get("pubSub");
        
        pubSub.subscribe("user:action", (data) => {
            console.log("User action:", data);
        });
        
        console.log("✅ Events initialized");
    }, ["init-storage"]));
    
    // Task 3: Load initial data (depends on both previous tasks)
    m.addEntry(new runner.ManifestEntry("load-data", async () => {
        const http = SJS.modules.get("http");
        http.configure({ baseURL: "https://jsonplaceholder.typicode.com" });
        
        console.log("✅ Data loader ready");
    }, ["init-storage", "init-events"]));
    
    return m;
}
```

### Step 5: Create main.js

```javascript
// main.js - Main application entry point
export default async function main(SJS, welcomeMessage) {
    console.log("🚀", welcomeMessage);
    
    const pubSub = SJS.modules.get("pubSub");
    const storage = SJS.modules.get("storage");
    const router = SJS.modules.get("router");
    
    // Define routes
    router.template.register("home", `
        <div class="page">
            <h2>Home Page</h2>
            <p>Welcome! Visit count: <%- visits %></p>
            <nav>
                <a href="#/about">About</a> | 
                <a href="#/users/123">User Profile</a>
            </nav>
        </div>
    `);
    
    router.template.register("about", `
        <div class="page">
            <h2>About</h2>
            <p>This is an SJS demo application.</p>
            <a href="#/">Back to Home</a>
        </div>
    `);
    
    router.template.register("user", `
        <div class="page">
            <h2>User Profile</h2>
            <p>User ID: <%- params.id %></p>
            <a href="#/">Back to Home</a>
        </div>
    `);
    
    // Set up routes
    router.setRoutes([
        {
            name: "home",
            path: "/",
            templateName: "home",
            async load(ctx) {
                // Track visits
                const visits = (await storage.get("visits")) || 0;
                await storage.set("visits", visits + 1);
                return { visits: visits + 1 };
            }
        },
        {
            name: "about",
            path: "/about",
            templateName: "about"
        },
        {
            name: "user",
            path: "/users/:id",
            templateName: "user"
        }
    ]);
    
    // Subscribe to router events
    pubSub.subscribe("router:navigate", (data) => {
        console.log("Navigated to:", data.url);
        pubSub.publish("user:action", { type: "navigate", url: data.url });
    });
    
    console.log("✅ Application ready!");
    return true;
}
```

### Step 6: Run Your App

1. Open `index.html` in a browser
2. Open developer console to see initialization logs
3. Navigate between pages using the links
4. Check Application Storage (DevTools) to see persisted data

## Common Patterns

### Pattern 1: Pub/Sub Communication

```javascript
const pubSub = SJS.modules.get("pubSub");

// Subscriber (Module A)
pubSub.subscribe("data:updated", (newData) => {
    console.log("Data changed:", newData);
});

// Publisher (Module B)
function updateData(data) {
    // Do something with data
    pubSub.publish("data:updated", data);
}
```

### Pattern 2: Async Task Management

```javascript
const taskManager = SJS.modules.get("taskManager");

const tasks = [
    new taskManager.Task("fetch-users", async () => {
        const res = await fetch("/api/users");
        return await res.json();
    }),
    new taskManager.Task("fetch-posts", async () => {
        const res = await fetch("/api/posts");
        return await res.json();
    })
];

const results = await taskManager.asyncManager(tasks);
// results = [{ id: "fetch-users", value: [...] }, { id: "fetch-posts", value: [...] }]
```

### Pattern 3: HTTP with Caching

```javascript
const http = SJS.modules.get("http");
const cache = SJS.modules.get("cache");

// Configure HTTP client with caching
http.configure({
    baseURL: "https://api.example.com",
    cacheGet: true,
    cacheTtlMs: 60000  // Cache for 1 minute
});

// First call hits network
const user1 = await http.get("/users/1");

// Second call uses cache (if within 1 minute)
const user2 = await http.get("/users/1");
```

### Pattern 4: Namespaced Storage

```javascript
const storage = SJS.modules.get("storage");

// Create isolated storage namespaces
const userStore = storage.use("user");
const appStore = storage.use("app");

await userStore.set("profile", { name: "Ada" });
await appStore.set("theme", "dark");

// These are independent
console.log(await userStore.keys());  // ["profile"]
console.log(await appStore.keys());   // ["theme"]
```

### Pattern 5: Performance Metrics

```javascript
const metrics = SJS.modules.get("metrics");

metrics.configure({ enabled: true });

// Track page loads
metrics.counterInc("page_loads");

// Time operations
const timer = metrics.timeStart("api_call");
await fetch("/api/data");
const duration = metrics.timeEnd(timer);
console.log(`API call took ${duration}ms`);

// Or use async wrapper
const data = await metrics.timeAsync("fetch_users", async () => {
    return await http.get("/users");
});
```

## Next Steps

1. ✅ Build your first app (you just did!)
2. 📖 Read [API Reference](./API-REFERENCE.md) for detailed module docs
3. 🏗️ Explore [examples/](../examples/) for more complex apps
4. 🎯 Try adding authentication, real-time updates, or offline support
5. 🧪 Learn testing patterns in [ARCHITECTURE.md](./ARCHITECTURE.md)

## Troubleshooting

### App not loading

**Check:** Console for errors, network tab for failed requests

```javascript
// Add debug logging
const debug = SJS.modules.get("debug");
debug.debugOn();
```

### Storage not persisting

**Check:** Browser privacy settings, available quota

```javascript
const storage = SJS.modules.get("storage");
console.log("Driver:", storage.driverName());
// Should be "idb" (best) or "localStorage" (fallback)
```

### Router not navigating

**Check:** Routes are registered before calling `start()`

```javascript
router.setRoutes([...]);  // First
router.start();            // Then
```

## Getting Help

- Check the examples directory for working code
- Read module source code (it's educational!)
- Console logs show what's happening at each step

---

Ready to build something awesome! 🚀
