# TypeCheck - Comprehensive JavaScript Type Checking Library

A robust, environment-agnostic type checking library for JavaScript that works in browsers, Node.js, and web workers.

## Features

- ✅ **Universal Module Definition (UMD)** - Works everywhere (AMD, CommonJS, Browser globals)
- ✅ **Exhaustive Type Checks** - Primitives, objects, arrays, and more
- ✅ **Binary Data Support** - TypedArrays, ArrayBuffers, DataView
- ✅ **Schema Validation** - Recursive object validation with custom rules
- ✅ **Modern JavaScript** - Supports BigInt, Symbols, Promises, Generators, Async functions
- ✅ **Zero Dependencies** - Pure JavaScript, no external libraries needed

## Installation

### Node.js (CommonJS)
```javascript
const TypeCheck = require('./types.js');
```

### ES6 Module (after conversion)
```javascript
import * as TypeCheck from './types.js';
```

### Browser (Global)
```html
<script src="types.js"></script>
<script>
  console.log(TypeCheck.isString("hello")); // true
</script>
```

## API Reference

### Primitive Type Checks

#### Null and Undefined
```javascript
TypeCheck.isUndefined(undefined);  // true
TypeCheck.isNull(null);            // true
TypeCheck.isNil(null);             // true (checks both null and undefined)
TypeCheck.isNil(undefined);        // true
```

#### Basic Types
```javascript
TypeCheck.isString("hello");       // true
TypeCheck.isBoolean(true);         // true
TypeCheck.isSymbol(Symbol());      // true
TypeCheck.isBigInt(123n);          // true
```

#### Numeric Types (with nuance)
```javascript
TypeCheck.isNumber(42);            // true
TypeCheck.isNumber(NaN);           // false (excludes NaN)
TypeCheck.isNaN(NaN);              // true
TypeCheck.isFinite(42);            // true
TypeCheck.isFinite(Infinity);      // false
TypeCheck.isInteger(42);           // true
TypeCheck.isInteger(42.5);         // false
TypeCheck.isSafeInteger(42);       // true (within Number.MAX_SAFE_INTEGER)
TypeCheck.isFloat(3.14);           // true (number, not integer, finite)
TypeCheck.isFloat(42);             // false (integers are not floats)
```

### Reference Types

#### Functions and Objects
```javascript
TypeCheck.isFunction(() => {});              // true
TypeCheck.isObject({});                      // true (excludes null)
TypeCheck.isObject(null);                    // false
TypeCheck.isPlainObject({});                 // true
TypeCheck.isPlainObject(new Date());         // false
TypeCheck.isPlainObject(Object.create(null)); // true
```

#### Arrays
```javascript
TypeCheck.isArray([1, 2, 3]);              // true
TypeCheck.isArrayLike([1, 2, 3]);          // true
TypeCheck.isArrayLike({ length: 3 });      // true (array-like object)
TypeCheck.isArrayLike({ 0: 'a', 1: 'b' }); // false (no length property)
```

### Built-in Object Types

```javascript
TypeCheck.isDate(new Date());              // true
TypeCheck.isDate(new Date('invalid'));     // false (invalid dates return false)
TypeCheck.isRegExp(/abc/);                 // true
TypeCheck.isError(new Error());            // true
TypeCheck.isError(new TypeError());        // true
TypeCheck.isPromise(Promise.resolve());    // true
TypeCheck.isPromise({ then: fn, catch: fn }); // true (thenable)
```

### Collections

```javascript
TypeCheck.isMap(new Map());                // true
TypeCheck.isSet(new Set());                // true
TypeCheck.isWeakMap(new WeakMap());        // true
TypeCheck.isWeakSet(new WeakSet());        // true
```

### Binary Data Types

```javascript
TypeCheck.isArrayBuffer(new ArrayBuffer(8));           // true
TypeCheck.isSharedArrayBuffer(new SharedArrayBuffer(8)); // true
TypeCheck.isDataView(new DataView(new ArrayBuffer(8))); // true
TypeCheck.isTypedArray(new Uint8Array());              // true
```

#### Specific TypedArray Checks
```javascript
TypeCheck.isInt8Array(new Int8Array());              // true
TypeCheck.isUint8Array(new Uint8Array());            // true
TypeCheck.isUint8ClampedArray(new Uint8ClampedArray()); // true
TypeCheck.isInt16Array(new Int16Array());            // true
TypeCheck.isUint16Array(new Uint16Array());          // true
TypeCheck.isInt32Array(new Int32Array());            // true
TypeCheck.isUint32Array(new Uint32Array());          // true
TypeCheck.isFloat32Array(new Float32Array());        // true
TypeCheck.isFloat64Array(new Float64Array());        // true
TypeCheck.isBigInt64Array(new BigInt64Array());      // true
TypeCheck.isBigUint64Array(new BigUint64Array());    // true
```

### Modern JavaScript Features

#### Generators and Async Functions
```javascript
function* gen() { yield 1; }
async function asyncFn() { return 42; }
async function* asyncGen() { yield 1; }

TypeCheck.isGeneratorFunction(gen);              // true
TypeCheck.isAsyncFunction(asyncFn);              // true
TypeCheck.isAsyncGeneratorFunction(asyncGen);    // true
TypeCheck.isGenerator(gen());                    // true (generator object)
```

## Schema Validation

The most powerful feature - validate complex object structures with nested schemas.

### Basic Schema Validation

```javascript
const userSchema = {
  name: 'string',
  age: 'number',
  email: 'string'
};

const user = {
  name: 'Alice',
  age: 30,
  email: 'alice@example.com'
};

TypeCheck.validate(userSchema, user); // true
```

### Optional Fields

```javascript
const schema = {
  name: 'string',
  nickname: { type: 'string', optional: true }
};

TypeCheck.validate(schema, { name: 'Bob' }); // true (nickname is optional)
```

### Nested Object Validation

```javascript
const schema = {
  user: {
    name: 'string',
    address: {
      street: 'string',
      city: 'string',
      zipCode: 'number'
    }
  }
};

const data = {
  user: {
    name: 'Charlie',
    address: {
      street: '123 Main St',
      city: 'Springfield',
      zipCode: 12345
    }
  }
};

TypeCheck.validate(schema, data); // true
```

### Array Validation

#### Array of Primitives
```javascript
const schema = {
  tags: { type: 'array', items: 'string' }
};

TypeCheck.validate(schema, { tags: ['js', 'node', 'web'] }); // true
TypeCheck.validate(schema, { tags: ['js', 123] });           // false
```

#### Array of Objects
```javascript
const schema = {
  users: {
    type: 'array',
    items: {
      name: 'string',
      age: 'number'
    }
  }
};

const data = {
  users: [
    { name: 'Alice', age: 30 },
    { name: 'Bob', age: 25 }
  ]
};

TypeCheck.validate(schema, data); // true
```

### Custom Validators

```javascript
const schema = {
  email: {
    type: 'string',
    validator: (val) => val.includes('@')
  },
  age: {
    type: 'number',
    validator: (val) => val >= 18 && val <= 120
  }
};

const user = {
  email: 'user@example.com',
  age: 25
};

TypeCheck.validate(schema, user); // true
```

### Complex Real-World Example

```javascript
const apiResponseSchema = {
  status: 'number',
  data: {
    users: {
      type: 'array',
      items: {
        id: 'number',
        name: 'string',
        email: 'string',
        isActive: 'boolean',
        roles: { type: 'array', items: 'string' },
        metadata: {
          createdAt: 'string',
          updatedAt: 'string',
          loginCount: 'number'
        }
      }
    },
    pagination: {
      page: 'number',
      totalPages: 'number',
      hasNext: 'boolean'
    }
  },
  errors: { type: 'array', items: 'string', optional: true }
};

const response = {
  status: 200,
  data: {
    users: [
      {
        id: 1,
        name: 'Alice',
        email: 'alice@example.com',
        isActive: true,
        roles: ['admin', 'user'],
        metadata: {
          createdAt: '2025-01-01',
          updatedAt: '2025-12-17',
          loginCount: 42
        }
      }
    ],
    pagination: {
      page: 1,
      totalPages: 5,
      hasNext: true
    }
  }
};

TypeCheck.validate(apiResponseSchema, response); // true
```

## How It Works

### Internal Type Detection

TypeCheck uses `Object.prototype.toString.call()` to get the internal `[[Class]]` tag of values, which is more reliable than `typeof` for many types:

```javascript
typeof []              // "object" ❌ Not specific enough
typeof new Date()      // "object" ❌ Not specific enough
typeof null            // "object" ❌ Known JavaScript quirk

// TypeCheck uses:
Object.prototype.toString.call([])        // "[object Array]" ✅
Object.prototype.toString.call(new Date()) // "[object Date]" ✅
Object.prototype.toString.call(null)      // "[object Null]" ✅
```

### Schema Validation Algorithm

1. **Recursive Descent**: Validates nested objects by recursing into child schemas
2. **Type Normalization**: Converts shorthand (`'string'`) to full format (`{ type: 'string' }`)
3. **Optional Handling**: Skips undefined values if marked optional
4. **Array Validation**: Validates each item against primitive types or nested schemas
5. **Custom Validators**: Allows duck-typing and business logic checks
6. **Path Tracking**: Reports exact location of validation failures

### Browser Compatibility

The library uses:
- `Object.prototype.toString` - Universal (ES3+)
- `Number.isNaN` - Polyfillable (ES6+)
- `Number.isFinite` - Polyfillable (ES6+)
- `Number.isInteger` - Polyfillable (ES6+)
- `BigInt` - Modern browsers (ES2020+), gracefully degrades
- `Symbol` - Modern browsers (ES6+), gracefully degrades

## Use Cases

### API Response Validation
```javascript
fetch('/api/users')
  .then(res => res.json())
  .then(data => {
    if (TypeCheck.validate(userSchema, data)) {
      // Safe to use data
      renderUsers(data.users);
    } else {
      throw new Error('Invalid API response structure');
    }
  });
```

### Configuration Validation
```javascript
function initApp(config) {
  const configSchema = {
    apiEndpoint: 'string',
    timeout: 'number',
    retries: 'number',
    features: {
      analytics: 'boolean',
      logging: { type: 'boolean', optional: true }
    }
  };

  if (!TypeCheck.validate(configSchema, config)) {
    throw new Error('Invalid configuration');
  }

  // Proceed with valid config
  startApp(config);
}
```

### Runtime Type Guards (TypeScript-style)
```javascript
function processData(input) {
  if (TypeCheck.isString(input)) {
    return input.toUpperCase();
  } else if (TypeCheck.isNumber(input)) {
    return input * 2;
  } else if (TypeCheck.isArray(input)) {
    return input.map(processData);
  } else {
    throw new TypeError('Unsupported type');
  }
}
```

### Form Validation
```javascript
const formSchema = {
  username: {
    type: 'string',
    validator: (val) => val.length >= 3 && val.length <= 20
  },
  email: {
    type: 'string',
    validator: (val) => /^[^\s@]+@[^\s@]+\.[^\s@]+$/.test(val)
  },
  age: {
    type: 'number',
    validator: (val) => val >= 13 && val <= 120
  },
  terms: {
    type: 'boolean',
    validator: (val) => val === true
  }
};

function handleSubmit(formData) {
  if (TypeCheck.validate(formSchema, formData)) {
    submitToServer(formData);
  } else {
    showValidationErrors();
  }
}
```

## Educational Value

This library demonstrates:

1. **UMD Pattern**: How to write modules that work in any environment
2. **Duck Typing**: Using structural checks (`then` + `catch` for Promises)
3. **Prototype Chain**: Understanding `Object.getPrototypeOf()` for plain objects
4. **Type Coercion Pitfalls**: Why `typeof null === "object"` and how to handle it
5. **IEEE 754 Floats**: Distinguishing `NaN`, `Infinity`, and safe integers
6. **Recursive Algorithms**: Schema validation with nested structures
7. **Function Composition**: Building complex checks from simple primitives

## Common Pitfalls Avoided

```javascript
// ❌ Common mistakes TypeCheck handles correctly:

typeof null === 'object'           // true (bug in JavaScript)
TypeCheck.isObject(null)           // false ✅

typeof NaN === 'number'            // true (technically correct but confusing)
TypeCheck.isNumber(NaN)            // false ✅

typeof [] === 'object'             // true (not specific)
TypeCheck.isArray([])              // true ✅

new Date('invalid') instanceof Date // true (but date is invalid!)
TypeCheck.isDate(new Date('invalid')) // false ✅
```

## Testing

```javascript
// Simple test runner
function test(name, fn) {
  try {
    fn();
    console.log(`✓ ${name}`);
  } catch (e) {
    console.error(`✗ ${name}: ${e.message}`);
  }
}

// Test suite
test('isString detects strings', () => {
  if (!TypeCheck.isString('hello')) throw new Error('Failed');
  if (TypeCheck.isString(123)) throw new Error('Failed');
});

test('isNumber excludes NaN', () => {
  if (TypeCheck.isNumber(NaN)) throw new Error('Failed');
  if (!TypeCheck.isNumber(42)) throw new Error('Failed');
});

test('validate handles nested schemas', () => {
  const schema = {
    user: { name: 'string', age: 'number' }
  };
  const data = {
    user: { name: 'Alice', age: 30 }
  };
  if (!TypeCheck.validate(schema, data)) throw new Error('Failed');
});
```

## Performance Considerations

- Type checks are O(1) - simple comparisons
- Schema validation is O(n) where n = number of properties (recursive)
- Array validation is O(n*m) where n = array length, m = item complexity
- For high-performance scenarios, cache validation results for static schemas

## License

This is educational code - use freely for learning and projects!

## Contributing

Found a bug? Want to add a feature? This is an educational repository, so contributions are welcome! Make sure to:

1. Test your changes thoroughly
2. Add examples to the README
3. Keep the code readable and well-commented
4. Maintain backward compatibility

## Credits

Created as an educational resource for understanding JavaScript type systems and validation patterns.
