# Quantum Types for Shapp Framework

Complete Q3/Q9 quantum number implementation integrated with the Shapp framework.

## Quick Start

```html
<!DOCTYPE html>
<html>
<head>
    <title>Quantum Types</title>
</head>
<body>
    <!-- Load framework then module -->
    <script src="shapp.js"></script>
    <script src="quantum.js"></script>
    
    <script>
        // Get quantum module
        const quantum = shapp.get('quantum');
        
        // Create Q9 quantum number
        const q = new quantum.QuantumNumber(9, 10, 20, 0);
        
        // Accumulate evidence
        q.qif(true, 0, 0, 1);  // Increase confidence toward B
        
        // Crystallize when confident
        const result = q.crystallize();  // Returns 20
    </script>
</body>
</html>
```

## Loading Order

**IMPORTANT**: Load `shapp.js` BEFORE `quantum.js`

```html
<script src="shapp.js"></script>    <!-- Framework first -->
<script src="quantum.js"></script>  <!-- Module second -->
```

The quantum module will automatically register itself with shapp on load.

## API

### Get Module

```javascript
const quantum = shapp.get('quantum');
```

Returns module object with:
- `QuantumNumber` - Constructor
- `version` - Module version
- `description` - Module description

### Create Quantum Number

```javascript
const q = new quantum.QuantumNumber(width, a, b, c);
```

- `width`: 3 (Q3) or 9 (Q9)
- `a`: Hypothesis A value (default: 0)
- `b`: Hypothesis B value (default: 0)
- `c`: Confidence (-4 to +4 for Q9, -1 to +1 for Q3, default: 0)

### Methods

All methods from the core quantum types POC:

**Arithmetic** (apply to both hypotheses):
- `add(value)`, `sub(value)`, `mul(value)`, `div(value)`, `mod(value)`, `sqrt()`

**Conditional Updates**:
- `qif(condition, a_change, b_change, c_change)` - Apply changes if condition true

**Q-Functions** (conditional operations):
- `qor(condition, callback)` - Execute if EITHER meets condition
- `qand(condition, callback)` - Execute if BOTH meet condition
- `qxor(condition, callback)` - Execute if ONE but NOT BOTH
- `qnor(condition, callback)` - Execute if NEITHER meets condition
- `qnand(condition, callback)` - Execute if NOT (both meet condition)
- `qconf(condition, threshold, callback)` - Execute if high confidence
- `qnconf(condition, threshold, callback)` - Execute if low confidence

**Query**:
- `getA()`, `getB()`, `getC()` - Get individual values
- `getState()` - Get {a, b, c, width} object
- `crystallize()` - Collapse to single value (or null if uncertain)

**Lifecycle**:
- `destroy()` - Free resources, return ID to pool

## Example

```javascript
// Get module
const quantum = shapp.get('quantum');

// Sensor fusion example
const altitude = new quantum.QuantumNumber(9, 100, 105, 0);  // GPS vs Barometer

// Evidence accumulation
altitude.qif(reading_favors_barometer, 0, 0, 1);  // +1 confidence
altitude.qif(reading_favors_barometer, 0, 0, 1);  // +2 confidence

// Crystallize when confident
const final = altitude.crystallize();  // Returns 105 (barometer wins)

// Cleanup
altitude.destroy();
```

## Integration Benefits

**Shapp Framework Integration**:
- ✅ Follows shapp module pattern
- ✅ Checks for shapp presence before loading
- ✅ Auto-registers on load
- ✅ Same IIFE pattern as other shapp modules
- ✅ Works in browser and Node.js

**Functional JavaScript Patterns**:
- ✅ True privacy via closures
- ✅ Shared state in closure scope
- ✅ Prototype methods for efficiency
- ✅ ID pooling for memory management
- ✅ No class syntax - pure functional power!

## Files

- `quantum.js` - Shapp framework module (428 lines)
- `quantum_example.html` - HTML demo with output
- `README_QUANTUM_SHAPP.md` - This file

## Demo

Open `quantum_example.html` in a browser to see:
- Module loading and registration
- Q9 sensor fusion example
- Quantum arithmetic
- Q-function conditional logic
- All output to page

## Why This Matters

This demonstrates:
1. **Module portability** - Same quantum logic, different export pattern
2. **Functional patterns** - Closure-based privacy, no class syntax
3. **Framework integration** - Clean integration with shapp registry
4. **Educational value** - Shows how to adapt modules to frameworks

## See Also

- [Quantum Types POC](../quantum-types-poc/) - Standalone version
- [SJS Quantum Module](../sjs-framework/modules/quantum.js) - SJS framework version
- [Shapp Framework](./README.md) - Main framework docs
