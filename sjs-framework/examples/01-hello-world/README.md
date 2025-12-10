# Hello World Example

A simple counter app demonstrating:

- **Pub/Sub messaging** - Buttons publish events, subscribers update UI
- **Persistent storage** - Counter value survives page reloads
- **Structured logging** - All actions are logged
- **Event-driven architecture** - Decoupled components

## Running

1. Open `index.html` in a browser
2. Click buttons to modify counter
3. Reload page - counter value persists!
4. Open multiple tabs - they sync via storage events

## Key Concepts

### Pub/Sub Pattern

```javascript
// Publisher
pubSub.publish("counter:changed", { value: 5 });

// Subscriber
pubSub.subscribe("counter:changed", (data) => {
    console.log("New value:", data.value);
});
```

### Storage API

```javascript
// Save
await storage.set("counter", 42);

// Load
const value = await storage.get("counter");
```

### Logger

```javascript
const log = logger.create("app");
log.info("Something happened", { data: 123 });
```

## Challenge

Try adding:
- Undo/Redo functionality
- Step size selector (increment by 1, 5, or 10)
- History of all changes
- Export/import counter value
