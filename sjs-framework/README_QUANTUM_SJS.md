# Quantum Types for SJS Framework

Complete Q3/Q9 quantum number implementation integrated with the SJS framework.

## 🎓 Educational Purpose

**This is for LEARNING, not production.**

This integration demonstrates:
- How modules integrate with pub/sub systems
- How optional dependencies work
- How to build framework-aware modules
- **What production frameworks are doing behind the scenes**

**Turn the black box transparent.** See how event systems, logging, and module management actually work. Then decide if you need a giant framework or if you can build something simpler and more focused.

**You don't need a package for everything.** Sometimes understanding > convenience.

---

## Quick Start

### Option 1: Via Manifest

```javascript
// manifest.js
export default function buildManifest(SJS, runner) {
    const m = new runner.Manifest("app");
    
    // Load quantum module
    m.addEntry(new runner.ManifestEntry("quantum", "./modules/quantum.js", []));
    
    // Your app modules that depend on quantum
    m.addEntry(new runner.ManifestEntry("main", "./main.js", ["quantum"]));
    
    return m;
}
```

### Option 2: Direct Load

```javascript
// After SJS boots
const quantum = SJS.modules.get('quantum');

// Create quantum number
const q = quantum.create(9, 10, 20, 0);

// Use it
q.qif(true, 0, 0, 1);
const result = q.crystallize();
```

## Configuration

```javascript
const quantum = SJS.modules.get('quantum');

quantum.configure({
    emitEvents: true,     // Emit pub/sub events (requires pubSub module)
    logOperations: false  // Log operations (requires logger module)
});
```

## Integration Features

### PubSub Events

If `emitEvents: true` and pubSub module is loaded:

**Events emitted**:
- `quantum:create` - When quantum number created
- `quantum:update` - When state updated via qif()
- `quantum:crystallize` - When crystallized
- `quantum:destroy` - When destroyed
- `quantum:reset` - When reset() called

```javascript
const pubSub = SJS.modules.get('pubSub');
const quantum = SJS.modules.get('quantum');

quantum.configure({ emitEvents: true });

pubSub.subscribe('quantum:crystallize', data => {
    console.log(`Crystallized to: ${data.result}`);
});

const q = quantum.create(9, 10, 20, 2);
q.crystallize();  // Triggers event
```

### Logging

If `logOperations: true` and logger module is loaded:

```javascript
quantum.configure({ logOperations: true });

const q = quantum.create(9, 10, 20, 0);  // Logs creation
// All errors logged automatically
```

## API

### Module Methods

```javascript
const quantum = SJS.modules.get('quantum');

// Configure
quantum.configure({ emitEvents: true, logOperations: false });

// Create quantum number
const q = quantum.create(width, a, b, c);

// Get statistics
const stats = quantum.stats();
// Returns: { Q3: {...}, Q9: {...} }

// Reset all instances (testing)
quantum.reset();
```

### QuantumNumber Methods

**Creation**:
```javascript
const q = quantum.create(9, 10, 20, 0);  // Q9 with a=10, b=20, c=0
// OR
const q = new quantum.QuantumNumber(9, 10, 20, 0);
```

**Arithmetic** (apply to both hypotheses):
```javascript
q.add(5).mul(2).div(4);  // Chainable
```

**Conditional**:
```javascript
q.qif(condition, a_change, b_change, c_change);
```

**Q-Functions**:
```javascript
q.qor(condition, callback);    // EITHER meets condition
q.qand(condition, callback);   // BOTH meet condition
q.qxor(condition, callback);   // ONE but NOT BOTH
q.qnor(condition, callback);   // NEITHER meets condition
q.qnand(condition, callback);  // NOT (both meet condition)
q.qconf(cond, threshold, cb);  // High confidence
q.qnconf(cond, threshold, cb); // Low confidence
```

**Query**:
```javascript
q.getA();         // Hypothesis A value
q.getB();         // Hypothesis B value
q.getC();         // Confidence value
q.getState();     // {a, b, c, width}
q.crystallize();  // Single value or null
```

**Lifecycle**:
```javascript
q.destroy();  // Free resources
```

## Example App

```javascript
// main.js
export default async function main(SJS) {
    const quantum = SJS.modules.get('quantum');
    const pubSub = SJS.modules.get('pubSub');
    
    // Configure with SJS integration
    quantum.configure({
        emitEvents: true,
        logOperations: true
    });
    
    // Subscribe to events
    if(pubSub) {
        pubSub.subscribe('quantum:crystallize', data => {
            console.log(`Decision made: ${data.result}`);
        });
    }
    
    // Sensor fusion
    const altitude = quantum.create(9, 100, 105, 0);
    
    // Accumulate evidence
    altitude.qif(true, 0, 0, 1);  // +1 toward B
    altitude.qif(true, 0, 0, 1);  // +2 toward B
    
    // Crystallize
    const final = altitude.crystallize();  // Triggers event
    console.log(`Final altitude: ${final}m`);
    
    // Stats
    console.log('Stats:', quantum.stats());
    
    // Cleanup
    altitude.destroy();
    
    return true;
}
```

## Module Info

```javascript
{
    name: 'quantum',
    version: '1.0.0',
    dependencies: []  // Optional: ['pubSub', 'logger']
}
```

**Optional dependencies** (used if available):
- `pubSub` - For event emission
- `logger` - For operation logging

**No hard dependencies** - Works standalone!

## Integration Benefits

**SJS Framework Integration**:
- ✅ Follows SJS module pattern (name, version, dependencies)
- ✅ Optional pub/sub event integration
- ✅ Optional logger integration
- ✅ Stats API for monitoring
- ✅ Reset API for testing
- ✅ Works in browser and Node.js
- ✅ Manifest-based dependency ordering

**Functional JavaScript Patterns**:
- ✅ True privacy via closures
- ✅ Shared state in closure scope
- ✅ Prototype methods for efficiency
- ✅ ID pooling for memory management
- ✅ No class syntax - pure functional power!

## Files

- `modules/quantum.js` - SJS framework module (625 lines)
- `examples/quantum_example.js` - Full demo with all features
- `README_QUANTUM_SJS.md` - This file

## Running Examples

### Browser

Create manifest and config:

```json
{
  "loadManifest": "./examples/quantum_manifest.js",
  "main": "./examples/quantum_example.js",
  "mainArgs": []
}
```

Load SJS and boot with config.

### Node.js

```bash
node -e "import('./core/sjs-full.js').then(m => m.default.boot('./quantum_config.json'))"
```

## Why This Matters

This demonstrates:
1. **Module portability** - Same quantum logic, different integration
2. **Framework integration** - Uses SJS pub/sub, logger, manifest
3. **Optional dependencies** - Works alone or with other modules
4. **Educational value** - Shows how to build framework-aware modules

## See Also

- [Quantum Types POC](../quantum-types-poc/) - Standalone version
- [Shapp Quantum Module](../shapp-framework/quantum.js) - Shapp framework version
- [SJS Framework](./README.md) - Main framework docs
- [SJS Modules](./modules/) - Other SJS modules
