# SJS Framework - Architecture Overview

## Design Philosophy

SJS follows these principles:

1. **Simplicity** - Easy to understand, minimal abstractions
2. **Modularity** - Each module is independent and composable
3. **Education** - Code is written to teach, not just to work
4. **Zero Dependencies** - Pure JavaScript, no npm packages
5. **Cross-Platform** - Works in browsers and Node.js
6. **Progressive Enhancement** - Use what you need, ignore the rest

## Architecture Diagram

```
┌─────────────────────────────────────────────────────────┐
│                    Application Code                      │
│                  (main.js, manifest.js)                  │
└───────────────────────┬─────────────────────────────────┘
                        │
                        ▼
┌─────────────────────────────────────────────────────────┐
│                  Module Registry (SJS.modules)           │
│  ┌──────────┬──────────┬──────────┬──────────┬────────┐ │
│  │  runner  │  pubSub  │  storage │  logger  │  ...   │ │
│  └──────────┴──────────┴──────────┴──────────┴────────┘ │
└───────────────────────┬─────────────────────────────────┘
                        │
                        ▼
┌─────────────────────────────────────────────────────────┐
│                   Platform Layer                         │
│  ┌─────────────┬──────────────┬──────────────┬────────┐ │
│  │ Browser DOM │  Node.js FS  │  IndexedDB   │  Fetch │ │
│  └─────────────┴──────────────┴──────────────┴────────┘ │
└─────────────────────────────────────────────────────────┘
```

## Module System

### IIFE Pattern

Each module uses an Immediately Invoked Function Expression (IIFE) to:
- Create isolated scope
- Prevent global pollution
- Control what gets exposed

```javascript
(function (modules) {
    // Module implementation here
    const MyModule = {
        method1() { },
        method2() { }
    };
    
    // Register with module registry
    modules.add("myModule", Object.freeze(MyModule));
})(globalThis.SJS?.modules || null);
```

### Why This Pattern?

1. **No bundler needed** - Works directly in browsers
2. **Clear dependencies** - Explicit module registry lookup
3. **Immutable exports** - `Object.freeze()` prevents tampering
4. **Safe failure** - Null check prevents errors if core isn't loaded

## Core Modules

### 1. Bootstrapper

**Responsibility:** Application initialization

```
┌──────────────────────────────────────────────────┐
│ 1. Load config.json                              │
│ 2. Load manifest.js (dependency graph)           │
│ 3. Run manifest (execute tasks in order)         │
│ 4. Execute main(...mainArgs)                     │
│ 5. Application ready                             │
└──────────────────────────────────────────────────┘
```

**Key Functions:**
- `loadJSON(path)` - Loads JSON via fetch/fs/import
- `loadModule(path)` - Loads JS modules with fallbacks
- `boot(config)` - Orchestrates startup sequence

### 2. Module Registry

**Responsibility:** Module storage and retrieval

```javascript
// Internal storage
const modules = {};

// Public API
{
    get(id) { /* Returns module or null */ },
    add(id, module) { /* Registers module */ },
    list() { /* Returns all module IDs */ }
}
```

**Safety Features:**
- Duplicate ID prevention
- Null checks
- Descriptive error messages

### 3. Runner

**Responsibility:** Dependency resolution and execution

**Algorithm:** Topological sort with concurrent execution

```
Graph: A ─┬─→ B ─→ D
           └─→ C ─→ D

Execution:
1. A runs (no dependencies)
2. B and C run in parallel (both depend only on A)
3. D runs (after B and C complete)
```

**Implementation:**
```javascript
// Kahn's algorithm for topological sort
while (readyNodes.length > 0 || inFlight.size > 0) {
    // Launch all nodes with no pending dependencies
    for (const node of readyNodes) {
        inFlight.add(runNode(node));
    }
    
    // Wait for any to complete
    const completed = await Promise.race(inFlight);
    
    // Update dependency counts for nodes that depended on completed
    for (const dependent of graph[completed]) {
        dependencyCount[dependent]--;
        if (dependencyCount[dependent] === 0) {
            readyNodes.push(dependent);
        }
    }
}
```

### 4. PubSub

**Responsibility:** Event-driven communication

**Pattern:** Observer pattern with topic-based routing

```javascript
// Internal structure
const topics = {
    "user:login": [
        { cb: handler1, once: false, token: "tok-1" },
        { cb: handler2, once: true, token: "tok-2" }
    ]
};

// Publishing flow
publish(topic, payload) →
    1. Look up topic subscribers
    2. For each subscriber:
        a. Call handler(payload, meta)
        b. If once=true, unsubscribe
    3. Return number of handlers called
```

**Features:**
- Wildcard topics (using patterns)
- One-time subscriptions
- Async/sync publishing
- Metadata (topic, token, timestamp)

### 5. Storage

**Responsibility:** Persistent key-value storage

**Driver Selection:**
```
Auto-detect environment:
  ↓
  Browser? → IndexedDB available? → Use IndexedDB
             ↓ No
             localStorage? → Use localStorage
  ↓
  Node.js? → Use fs (JSON file)
  ↓
  Fallback → Memory (not persistent)
```

**API Surface:**
```javascript
{
    configure(opts),
    driverName(),
    use(namespace),  // Returns namespaced view
    get(key),
    set(key, value, { ttlMs }),
    has(key),
    remove(key),
    clear(),
    keys(),
    entries(),
    export(),
    import(obj)
}
```

**TTL Implementation:**
```javascript
// Storage format
{
    value: actualValue,
    expiresAt: timestamp || null
}

// On get()
if (expiresAt && Date.now() > expiresAt) {
    remove(key);  // Auto-cleanup expired
    return null;
}
```

### 6. Logger

**Responsibility:** Structured logging with levels

**Log Levels (Numeric):**
```
DEBUG (10) - Verbose debugging
INFO  (20) - Informational
WARN  (30) - Warnings
ERROR (40) - Errors
OFF   (99) - Disabled
```

**Enable Patterns:**
```javascript
logger.enable("app:*");        // All app namespaces
logger.enable("app:auth,api"); // Specific namespaces
logger.enable("*");            // Everything
logger.disable("*");           // Nothing
```

**Sinks:**
1. **Console** - Standard console.log/warn/error
2. **Memory** - Buffer for testing/debugging
3. **PubSub** - Publish to event bus
4. **Custom** - User-defined handlers

### 7. Cache

**Responsibility:** Memoization with TTL and SWR

**Cache Entry Structure:**
```javascript
{
    value: cachedValue,
    createdAt: timestamp,
    expiresAt: timestamp,
    swrUntil: timestamp,  // Stale-while-revalidate deadline
    inFlight: Promise | null  // Deduplication
}
```

**Memoization Flow:**
```
Request for key
  ↓
  In cache?
    ↓ Yes
    Fresh? (before expiresAt)
      ↓ Yes → Return cached value
      ↓ No
      Within SWR? (before swrUntil)
        ↓ Yes → Return stale + trigger background refresh
        ↓ No → Re-fetch (blocking)
    ↓ No → Fetch and cache
```

**Concurrency Deduplication:**
```javascript
// Multiple simultaneous calls to same key
const promise = fn();  // Only ONE network call
inFlightMap.set(key, promise);

// All callers get same promise
return inFlightMap.get(key);
```

### 8. HTTP

**Responsibility:** Fetch wrapper with retries and caching

**Request Flow:**
```
request(opts)
  ↓
  Run before interceptors
  ↓
  If GET and cache enabled → Check cache
    ↓ Hit → Return cached
    ↓ Miss
  ↓
  Retry loop (1 + retries):
    ↓
    Create AbortController (for timeout)
    ↓
    Call fetch()
      ↓ Success → Parse response → Run after interceptors → Return
      ↓ Network error → Retry if retryable status
      ↓ Timeout → Abort → Retry if attempts remain
  ↓
  Out of retries → Throw error
```

**Retry Strategy:**
```javascript
// Exponential backoff with jitter
delay = baseDelay * (backoffFactor ** attempt) + randomJitter
// Example: 300ms, 600ms±jitter, 1200ms±jitter
```

### 9. Router

**Responsibility:** Client-side routing with templates

**Route Matching:**
```javascript
// Route patterns
"/"              → Exact match
"/users/:id"     → Named param (:id becomes params.id)
"/posts/:slug?"  → Optional param
"/files/*"       → Wildcard (matches everything after)

// Match priority
1. Exact matches
2. Param routes (by specificity)
3. Wildcard routes (last)
```

**Navigation Lifecycle:**
```
navigate(url)
  ↓
  Before hooks (can cancel)
  ↓
  Match route
  ↓
  Load data (async load() function)
  ↓
  BeforeEnter hook (can redirect)
  ↓
  Render template with data
  ↓
  AfterEnter hook
  ↓
  Update browser history
  ↓
  Publish "router:navigate" event
```

**Template Engine:**
```html
<!-- Evaluate: <% code %> -->
<% if (user.admin) { %>
    Admin panel
<% } %>

<!-- Interpolate: <%= expr %> -->
<p>Welcome, <%= user.name %></p>

<!-- Escape: <%- expr %> -->
<p>Comment: <%- userInput %></p>  <!-- HTML-escaped -->
```

## Data Flow Patterns

### 1. One-Way Data Flow

```
User Action
  ↓
  Publish Event (pubSub)
  ↓
  Business Logic (event handler)
  ↓
  Update Storage (storage.set)
  ↓
  Publish "data:changed" Event
  ↓
  UI Updates (subscriber renders)
```

### 2. Request/Response Flow

```
Component A needs data
  ↓
  Check cache (cache.get)
    ↓ Hit → Use cached
    ↓ Miss
  ↓
  Call HTTP (http.get)
  ↓
  Store in cache (cache.set)
  ↓
  Publish "data:loaded" event
  ↓
  Components B, C update (subscribers)
```

### 3. Cross-Tab Synchronization

```
Tab 1: storage.set("key", value)
  ↓
  BroadcastChannel OR localStorage event
  ↓
  Tab 2: Receives "storage:set" event
  ↓
  Tab 2: Updates UI to reflect change
```

## Error Handling Strategies

### 1. Fail Safely

```javascript
const mod = SJS.modules.get("mayNotExist");
if (!mod) {
    console.warn("Optional module not loaded");
    return; // Graceful degradation
}
```

### 2. Retry with Backoff

```javascript
// HTTP module retries failed requests
for (let attempt = 0; attempt <= retries; attempt++) {
    try {
        return await fetch(url);
    } catch (err) {
        if (attempt === retries) throw err;
        await sleep(delay * (backoffFactor ** attempt));
    }
}
```

### 3. Catch and Publish

```javascript
try {
    await riskyOperation();
} catch (error) {
    pubSub.publish("error:occurred", { error, context });
    // Centralized error handling
}
```

## Performance Considerations

### Memory Management

- **Weak references not used** - Educational clarity over optimization
- **LRU cache** - Simple eviction in cache module (max entries)
- **Cleanup on unload** - Modules clean up on window/process exit

### Async Optimization

- **Parallel execution** - Runner executes independent tasks concurrently
- **Request deduplication** - Cache module prevents duplicate in-flight requests
- **Lazy loading** - Modules loaded only when needed

### Bundle Size

- **Full bundle:** ~40KB (minified, ~12KB gzipped)
- **Core only:** ~8KB (bootstrapper + module system)
- **Modular:** Import individual modules if needed

## Testing Strategies

### Unit Testing

```javascript
// Test module in isolation
describe("pubSub", () => {
    it("should call subscribers", () => {
        let called = false;
        pubSub.subscribe("test", () => { called = true; });
        pubSub.publish("test");
        assert(called);
    });
});
```

### Integration Testing

```javascript
// Test module interactions
it("should cache HTTP responses", async () => {
    http.configure({ cacheGet: true });
    cache.configure({ ttlMs: 5000 });
    
    const res1 = await http.get("/data");
    const res2 = await http.get("/data");
    
    // Second request should be cached
    assert(res1 === res2);
});
```

### E2E Testing

```javascript
// Test full application flow
it("should persist counter across reloads", async () => {
    await storage.set("counter", 5);
    
    // Simulate reload
    location.reload();
    
    const counter = await storage.get("counter");
    assert(counter === 5);
});
```

## Extension Points

### Custom Module

```javascript
(function (modules) {
    const MyCustomModule = {
        doSomething() {
            const pubSub = modules.get("pubSub");
            pubSub.publish("custom:event", {});
        }
    };
    
    modules.add("custom", Object.freeze(MyCustomModule));
})(globalThis.SJS?.modules);
```

### Custom Storage Driver

```javascript
class RedisDriver {
    async get(key) { /* ... */ }
    async set(key, value) { /* ... */ }
    // ... implement full interface
}

storage.configure({ driver: new RedisDriver() });
```

### Custom Logger Sink

```javascript
function remoteSink(record) {
    fetch("/logs", {
        method: "POST",
        body: JSON.stringify(record)
    });
}

logger.addSink("remote", remoteSink);
```

## Learning Outcomes

After studying this architecture, you should understand:

1. ✅ **Module patterns** - IIFE, namespacing, encapsulation
2. ✅ **Async patterns** - Promises, async/await, concurrency
3. ✅ **Event-driven design** - Pub/sub, loose coupling
4. ✅ **Cross-platform abstractions** - Driver patterns, feature detection
5. ✅ **Dependency management** - Topological sort, DAG traversal
6. ✅ **Caching strategies** - TTL, SWR, LRU eviction
7. ✅ **Error handling** - Retry logic, graceful degradation
8. ✅ **Performance optimization** - Deduplication, parallel execution

---

**This architecture prioritizes learning over production optimization.**  
Every choice is made to teach a concept clearly.
