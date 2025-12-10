# SJS Framework - Educational JavaScript Application System

## 📚 Complete Documentation Index

### Getting Started
1. **[README](./README.md)** - Project overview and features
2. **[Quick Start Guide](./docs/QUICKSTART.md)** - Build your first app in 10 minutes
3. **[Architecture Overview](./docs/ARCHITECTURE.md)** - How it all works

### Core Framework
- **[sjs-full.js](./core/sjs-full.js)** - Complete framework (all modules included)

### Examples (Progressive Learning)
1. **[Hello World](./examples/01-hello-world/)** - Basic pub/sub and storage
2. More examples coming soon...

### Module Documentation

| Module | Purpose | Key Features |
|--------|---------|--------------|
| **Bootstrapper** | App initialization | Config loading, manifest execution |
| **Module Registry** | Module management | Register, retrieve, list modules |
| **Runner** | Dependency resolution | Topological sort, parallel execution |
| **Task Manager** | Task orchestration | Sequential async execution |
| **PubSub** | Event messaging | Subscribe, publish, once, async |
| **Storage** | Persistence | IndexedDB/localStorage/fs, TTL, namespaces |
| **Logger** | Structured logging | Levels, patterns, sinks, child loggers |
| **Debug** | Debug toggle | Colorized output, on/off |
| **Cache** | Memoization | TTL, SWR, deduplication |
| **HTTP** | HTTP client | Retries, timeout, caching, interceptors |
| **Input** | Input handling | Keyboard/mouse state, events |
| **Metrics** | Performance metrics | Counters, gauges, histograms, timers |
| **Router** | Client-side routing | History/hash, templates, hooks |
| **ErrorReporter** | Error tracking | Auto-capture, breadcrumbs, deduplication, HTTP reporting |
| **State** | Reactive state management | Stores, computed props, watchers, time-travel, persistence |

## 🎯 Learning Path

### Beginner (1-2 hours)
1. Read [README](./README.md)
2. Follow [Quick Start](./docs/QUICKSTART.md)
3. Build [Hello World example](./examples/01-hello-world/)
4. Understand module pattern and pub/sub

### Intermediate (3-5 hours)
1. Read [Architecture](./docs/ARCHITECTURE.md)
2. Study runner's dependency resolution
3. Explore storage drivers
4. Build a todo app with routing

### Advanced (5-10 hours)
1. Study cache module (SWR, deduplication)
2. Implement custom storage driver
3. Build a full SPA with all modules
4. Add custom module to framework

## 🔧 Quick Reference

### Minimal Setup
```html
<script>globalThis.configFileSrc = "./config.json";</script>
<script src="./sjs-full.js"></script>
```

### Module Access
```javascript
const pubSub = SJS.modules.get("pubSub");
const storage = SJS.modules.get("storage");
```

### Common Patterns
```javascript
// Pub/Sub
pubSub.subscribe("topic", handler);
pubSub.publish("topic", data);

// Storage
await storage.set("key", value);
const val = await storage.get("key");

// HTTP with cache
http.configure({ cacheGet: true });
const data = await http.get("/api/data");
```

## 📖 Documentation Files

### Core Docs
- `README.md` - Main documentation (you are here)
- `docs/QUICKSTART.md` - Tutorial for beginners
- `docs/ARCHITECTURE.md` - Deep dive into design
- `docs/API-REFERENCE.md` - Complete API documentation (TODO)
- `docs/MODULES.md` - Module-by-module guide (TODO)

### Examples
- `examples/01-hello-world/` - Basic counter app
- `examples/02-pubsub-demo/` - Event-driven patterns (TODO)
- `examples/03-storage-demo/` - Persistence patterns (TODO)
- `examples/04-router-spa/` - Single-page app (TODO)
- `examples/05-http-client/` - API integration (TODO)
- `examples/06-full-app/` - Complete application (TODO)

## 🎓 Educational Goals

This framework teaches:

### JavaScript Fundamentals
- ✅ IIFE pattern for encapsulation
- ✅ Closures and scope
- ✅ Object.freeze() for immutability
- ✅ Null-safe navigation (?. operator)

### Async Programming
- ✅ Promises and Promise.all/race
- ✅ async/await patterns
- ✅ Error handling in async code
- ✅ Concurrency management

### Design Patterns
- ✅ Module pattern
- ✅ Observer (pub/sub)
- ✅ Singleton (module registry)
- ✅ Strategy (storage drivers)
- ✅ Decorator (HTTP interceptors)

### Architecture
- ✅ Dependency injection
- ✅ Event-driven architecture
- ✅ Separation of concerns
- ✅ Cross-platform abstraction

### Web Platform APIs
- ✅ IndexedDB
- ✅ LocalStorage
- ✅ Fetch API
- ✅ History API
- ✅ BroadcastChannel

## 🚀 Feature Highlights

### Zero Dependencies
No npm install needed. Copy one file and go.

### Cross-Platform
Works identically in:
- ✅ Modern browsers (Chrome, Firefox, Safari, Edge)
- ✅ Node.js (v14+)
- ✅ Web Workers
- ✅ Service Workers

### Progressive Enhancement
Use only what you need:
```javascript
// Minimal - just pub/sub
const pubSub = SJS.modules.get("pubSub");

// Or use everything
const { storage, http, router, cache } = SJS.modules;
```

### Debuggable
Every module logs what it's doing:
```javascript
const debug = SJS.modules.get("debug");
debug.debugOn(); // See everything
```

## 🐛 Common Issues

### Module not found
```javascript
// ❌ Wrong
const mod = SJS.modules.get("typo");
mod.method(); // TypeError

// ✅ Right
const mod = SJS.modules.get("module");
if (mod) {
    mod.method();
}
```

### Async timing
```javascript
// ❌ Wrong
storage.set("key", "value");
const val = storage.get("key"); // Promise, not value

// ✅ Right
await storage.set("key", "value");
const val = await storage.get("key");
```

### Router not starting
```javascript
// ❌ Wrong
router.start(); // No routes!

// ✅ Right
router.setRoutes([...]);
router.start();
```

## 🤝 Contributing

This is an educational project. Contributions welcome:
- 📝 Better documentation
- 🎓 More examples
- 🐛 Bug fixes
- 💡 Teaching improvements

## 📄 License

MIT License - Free to learn, teach, and use.

## 🎯 Next Steps

1. **[Start the tutorial](./docs/QUICKSTART.md)** →
2. Build the hello world example
3. Read the architecture guide
4. Create your own app!

---

**Built for education. Designed for understanding. Ready for production.** 🚀
