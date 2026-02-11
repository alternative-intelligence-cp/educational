# SHAPP Framework - Educational JavaScript Module System

**Date**: February 10-11, 2026  
**Author**: Randy Hoggard  
**Purpose**: Educational framework demonstrating JavaScript module patterns, instance management, and data structures

## Overview

SHAPP (SHared APPlication framework) is a lightweight, educational module system designed to teach:
- JavaScript module patterns (IIFE pattern)
- Instance isolation and memory management
- Handle-based object systems
- Data structures (linked lists)
- Cross-environment compatibility (Node.js, Browser, Web Workers)

This framework was developed as teaching material for new programmers learning JavaScript concepts that translate well to other languages like GML (GameMaker Language).

## Architecture

### Core Components

1. **shapp.js** - Core module registry (58 lines)
   - Global module registration
   - Module retrieval
   - Environment detection

2. **event.js** - Event system module (143 lines)
   - Event handler registration
   - Event dispatching
   - Multiple handlers per event
   - Demonstrates handle-based IDs

3. **uint16List.js** - Linked list with uint16 values (368 lines)
   - Instance isolation pattern
   - Memory management with TypedArray  
   - ID pooling and reuse
   - Linked list operations (push, pop, pad, pull, insert, remove)
   - Sorting and searching
   - **Educational focus**: Shows how game engines manage objects

## Key Educational Concepts

### 1. Instance Isolation Pattern

Each module maintains private state using closures:

```javascript
((args) => {
    // Private module state
    let m_instances = {};
    let m_lastID = -1;
    
    function Constructor() {
        const _instanceID = m_createInstance();
        this.getInstanceID = function() { return _instanceID; }
    }
    
    shapp.add("ModuleName", Object.freeze(Constructor));
})({/* metadata */});
```

**Why this matters**:
- Similar to GML's instance variables
- Each object has its own data
- No global state pollution

### 2. Handle-Based IDs

Instead of direct memory addresses, we use numeric IDs:

```javascript
let handle = events.registerEventHandler('my_event', callback);
// handle is just a number: 0, 1, 2, etc.
```

**Benefits**:
- Safe: Can't accidentally corrupt memory
- Reusable: IDs can be recycled when objects are destroyed
- Portable: Works the same across all environments

**Real-world application**: Game engines (Unity, Unreal, GameMaker) use similar systems

### 3. Memory Management with TypedArrays

The Uint16List uses a `Uint16Array` for memory:

```javascript
memory: new Uint16Array(4096)  // Fixed-size memory block
```

**Structure**:
```
[value0, next0, value1, next1, value2, next2, ...]
 ^       ^      ^       ^      ^       ^
 data    ptr    data    ptr    data    ptr
```

Each node takes 2 slots: value + next pointer

**Educational value**:
- Shows how linked lists work in memory
- Demonstrates pointer concepts without actual pointers
- Similar to how GML manages ds_lists internally

### 4. ID Pooling (Preventing Fragmentation)

When an object is destroyed, its ID goes into a pool:

```javascript
let m_releasedIDs = [];

function m_destroyInstance(id) {
    delete m_instances[id];
    m_releasedIDs.push(id);  // Pool for reuse
}

function m_getID() {
    if(m_releasedIDs.length > 0) {
        return m_releasedIDs.pop();  // Reuse old ID
    }
    return ++m_lastID;  // Generate new ID
}
```

**Why this matters**: Prevents IDs from growing infinitely in long-running applications

## Module Reference

### Uint16List API

#### Constructor
```javascript
let list = new Uint16List();
```

#### Methods

| Method | Description | Returns |
|--------|-------------|---------|
| `push(value)` | Add to end of list | `bool` success |
| `pop()` | Remove from end | `Uint16` value or -1 |
| `pad(value)` | Add to beginning | `bool` success |
| `pull()` | Remove from beginning | `Uint16` value or -1 |
| `insert(afterNode, value)` | Insert after specific node | `bool` success |
| `remove(node)` | Remove specific node | `Uint16` value or -1 |
| `find(value)` | Search for value | `node` ID or -1 |
| `sort()` | Sort list in ascending order | `bool` success |
| `getSize()` | Get number of elements | `number` size |
| `grow(numNodes)` | Increase capacity | `number` new capacity |
| `shrink(numNodes)` | Decrease capacity | `number` new capacity |

#### Important Notes

1. **Node IDs vs Values**: 
   - `find()` returns a node ID (handle)
   - Use that ID with `insert()` or `remove()`
   - Don't confuse node IDs with the actual values!

2. **Shrink Safety**:
   - Can only shrink when list is empty
   - Prevents destroying active data
   - Educational: Shows importance of safety checks

3. **Insert Validation**:
   - Validates that target node exists in list
   - Prevents corrupting the linked structure
   - Demonstrates defensive programming

### EventSystem API

#### Constructor
```javascript
let events = new EventSystem();
```

#### Methods

| Method | Description | Returns |
|--------|-------------|---------|
| `registerEventHandler(event, callback)` | Register handler for event type | `handle` ID |
| `unregisterEventHandler(handle)` | Remove handler by ID | `bool` success |
| `raiseEvent(event, data)` | Trigger all handlers for event | `void` |

## Usage Examples

### Basic Event System
```javascript
let events = new EventSystem();

// Register multiple handlers for same event
let h1 = events.registerEventHandler('player_hit', (event, damage) => {
    console.log("Player took " + damage + " damage!");
    return true;
});

let h2 = events.registerEventHandler('player_hit', (event, damage) => {
    console.log("Update health bar!");
    return true;
});

// Trigger event
events.raiseEvent('player_hit', 25);

// Later: remove a handler
events.unregisterEventHandler(h1);
```

### Uint16List - Stack Operations
```javascript
let stack = new Uint16List();

// Push values
stack.push(10);
stack.push(20);
stack.push(30);

// Pop values (LIFO - Last In, First Out)
console.log(stack.pop());  // 30
console.log(stack.pop());  // 20
console.log(stack.pop());  // 10
```

### Uint16List - Queue Operations
```javascript
let queue = new Uint16List();

// Enqueue (add to back)
queue.push(10);
queue.push(20);
queue.push(30);

// Dequeue (remove from front) - FIFO
console.log(queue.pull());  // 10
console.log(queue.pull());  // 20
console.log(queue.pull());  // 30
```

### Uint16List - Sorted List
```javascript
let sorted = new Uint16List();

// Add unsorted values
sorted.push(50);
sorted.push(20);
sorted.push(80);
sorted.push(10);

// Sort in place
sorted.sort();

// Extract sorted values
while(sorted.getSize() > 0) {
    console.log(sorted.pull());  // 10, 20, 50, 80
}
```

### Uint16List - Insert After Specific Value
```javascript
let list = new Uint16List();
list.push(100);
list.push(200);
list.push(300);

// Find node containing 200
let node200 = list.find(200);

// Insert 250 after the 200 node
if(node200 !== -1) {
    list.insert(node200, 250);
}

// List is now: 100, 200, 250, 300
```

## Cross-Environment Support

The framework works in:
- ✅ **Node.js** - Command line applications
- ✅ **Browser** - Web applications
- ✅ **Web Workers** - Background threads

### Running Examples

**Node.js**:
```bash
node main.js
```

**Browser**:
```bash
# Optional: Start local server
./installApache2.sh  # Or use: python3 -m http.server
# Open index.html in browser
```

**Web Workers**:
```javascript
// In worker script
importScripts(["./shapp.js", "./event.js", "./uint16List.js"]);
// Use modules normally
```

## Comparison to GML (GameMaker Language)

| Concept | JavaScript (SHAPP) | GML |
|---------|-------------------|-----|
| Object instances | `new Uint16List()` | `instance_create()` |
| Instance variables | `this.getInstanceID()` | `self.id` |
| Data structures | `Uint16List` | `ds_list` |
| Events | `EventSystem` | Built-in events |
| Handles | Numeric IDs | Instance IDs |

## Learning Path for GML Developers

1. **Start with event.js**
   - Similar to GML's event system
   - Understand callbacks vs event blocks

2. **Study uint16List.js structure**
   - See how `ds_list` might work internally
   - Learn memory management concepts

3. **Explore the IIFE pattern**
   - Compare to GML's `with` statement
   - Understand scope isolation

4. **Experiment with modifications**
   - Add new methods to Uint16List
   - Create your own module (e.g., Uint16Queue)
   - Try implementing a hash map

## Educational Modifications to Try

### Beginner
1. Add a `clear()` method to Uint16List that removes all elements
2. Add a `contains(value)` method that returns true/false
3. Add a `getAt(index)` method to get value by position

### Intermediate
4. Implement a `reverse()` method to reverse the list order
5. Add a `forEach(callback)` method to iterate over all values
6. Create a Uint16Set module (no duplicates allowed)

### Advanced
7. Implement a doubly-linked list (prev + next pointers)
8. Create a Uint16HashMap with key-value pairs
9. Add memory compaction to `shrink()` method

## Design Decisions Explained

### Why Uint16Array?
- **Educational**: Shows typed memory like real languages
- **Efficient**: Direct memory access, cache-friendly
- **Limited**: 65535 max value teaches constraints

### Why Linked List?
- **Fundamental**: Core data structure in CS
- **Practical**: Used in game engines for entity lists
- **Flexible**: Insertions/deletions without array shifting

### Why Handles Instead of References?
- **Safety**: Can't dereference invalid handles
- **Portable**: Works across languages and systems
- **Game-like**: Mirrors how real engines work

### Why IIFE Pattern?
- **Encapsulation**: Private module state
- **Clean**: No global namespace pollution
- **Standard**: Common in professional JS code

## Common Pitfalls & Solutions

### Pitfall 1: Confusing Node IDs with Values
```javascript
// WRONG
let node = list.find(200);
list.push(node);  // Pushes the node ID, not 200!

// RIGHT
let node = list.find(200);
list.insert(node, 250);  // Use node ID to insert AFTER it
```

### Pitfall 2: Forgetting to Check Return Values
```javascript
// WRONG
let value = list.pop();
console.log(value + 10);  // Could be -1 + 10 = 9 !

// RIGHT
let value = list.pop();
if(value !== -1) {
    console.log(value + 10);
}
```

### Pitfall 3: Modifying During Iteration
```javascript
// WRONG - Will skip elements!
let curr = list.head;
while(curr !== NULL_PTR) {
    if(shouldRemove(curr)) {
        list.remove(curr);  // Modifies during iteration!
    }
    curr = getNext(curr);
}

// RIGHT - Collect first, then remove
let toRemove = [];
// ... collect node IDs ...
toRemove.forEach(node => list.remove(node));
```

## Performance Characteristics

| Operation | Time Complexity | Notes |
|-----------|----------------|-------|
| push() | O(1) | Constant time |
| pop() | O(n) | Must traverse to find prev |
| pad() | O(1) | Add to head |
| pull() | O(1) | Remove from head |
| find() | O(n) | Linear search |
| insert() | O(n) | Must validate node |
| remove() | O(n) | Find predecessor |
| sort() | O(n log n) | Uses JS Array.sort |
| getSize() | O(1) | Cached value |

**Educational note**: This is a singly-linked list. A doubly-linked list would make pop() O(1) too!

## Further Reading

- **Linked Lists**: [Wikipedia - Linked List](https://en.wikipedia.org/wiki/Linked_list)
- **Handles vs Pointers**: [Game Programming Patterns - Data Locality](http://gameprogrammingpatterns.com/data-locality.html)
- **IIFE Pattern**: [MDN - IIFE](https://developer.mozilla.org/en-US/docs/Glossary/IIFE)
- **TypedArrays**: [MDN - TypedArray](https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/TypedArray)

## Credits

Developed as educational material for new programmers, particularly those learning GML who want to understand underlying concepts.

Special thanks to Seiko for inspiring this project through her programming journey! 🎓

---

**License**: Educational use encouraged. See parent repository LICENSE for details.
