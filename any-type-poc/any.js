/**
 * any.js — JavaScript proof-of-concept for ARIA's `any` type
 *
 * In the ARIA compiler, `any` is a fat pointer: {ptr, i64} — a data pointer
 * plus the size of the boxed value. You write to the same memory and reinterpret
 * it as different types via turbofish methods: .get::<T>(), .set::<T>(val),
 * .resolve::<T>().
 *
 * This POC mirrors that design using an 8-byte ArrayBuffer as the backing store
 * for numeric types (same memory, different DataView accessors) and separate
 * slots for object, string, and array views. It's intentionally educational —
 * NOT production code.
 *
 * WHY an ArrayBuffer?
 *   Because it lets us do the same thing the compiler does: store raw bytes
 *   and reinterpret them through a different lens. Write 511 as u32, read it
 *   back as u8 and you get 255 (truncation). Read it as i8 and you get -1
 *   (two's complement). That's the whole point of `any` — same memory,
 *   different interpretation.
 *
 * Usage:
 *   const box = new Any('u32', 511);
 *   box.get();              // 511
 *   box.setType('u8');
 *   box.get();              // 255  (only the low byte)
 *   box.setType('i8');
 *   box.get();              // -1   (two's complement of 0xFF)
 *
 * @module any-type-poc
 * @see https://github.com/alternative-intelligence-cp/aria
 */

'use strict';

// ---------------------------------------------------------------------------
// Supported type tags — mirrors a subset of ARIA's type system
// ---------------------------------------------------------------------------
const NUMERIC_TYPES = new Set([
    'i8', 'i16', 'i32', 'i64',
    'u8', 'u16', 'u32', 'u64',
    'f32', 'f64',
]);

const COMPOSITE_TYPES = new Set(['obj', 'str', 'arr']);

const ALL_TYPES = new Set([...NUMERIC_TYPES, ...COMPOSITE_TYPES]);

// Map type tag → DataView getter/setter method names + byte widths
const NUMERIC_ACCESSORS = {
    i8:  { get: 'getInt8',       set: 'setInt8',       bytes: 1, bigint: false },
    i16: { get: 'getInt16',      set: 'setInt16',      bytes: 2, bigint: false },
    i32: { get: 'getInt32',      set: 'setInt32',      bytes: 4, bigint: false },
    i64: { get: 'getBigInt64',   set: 'setBigInt64',   bytes: 8, bigint: true  },
    u8:  { get: 'getUint8',      set: 'setUint8',      bytes: 1, bigint: false },
    u16: { get: 'getUint16',     set: 'setUint16',     bytes: 2, bigint: false },
    u32: { get: 'getUint32',     set: 'setUint32',     bytes: 4, bigint: false },
    u64: { get: 'getBigUint64',  set: 'setBigUint64',  bytes: 8, bigint: true  },
    f32: { get: 'getFloat32',    set: 'setFloat32',    bytes: 4, bigint: false },
    f64: { get: 'getFloat64',    set: 'setFloat64',    bytes: 8, bigint: false },
};

// ---------------------------------------------------------------------------
// Any — type-erased container
// ---------------------------------------------------------------------------
class Any {
    #buffer;    // 8-byte ArrayBuffer (shared backing store for all numeric views)
    #view;      // DataView over #buffer
    #obj;       // object slot
    #str;       // string slot
    #arr;       // array slot
    #type;      // current type tag (string)
    #resolved;  // true after resolve() — box is frozen

    /**
     * @param {string} [type='obj']  Initial type tag (i8..u64, f32, f64, obj, str, arr)
     * @param {*}      [value=null]  Initial value to store
     */
    constructor(type = 'obj', value = null) {
        this.#buffer   = new ArrayBuffer(8);
        this.#view     = new DataView(this.#buffer);
        this.#obj      = {};
        this.#str      = '';
        this.#arr      = [];
        this.#type     = 'obj';
        this.#resolved = false;

        if (type !== null) {
            this.setType(type);
        }
        if (value !== null) {
            this.set(value);
        }
    }

    // -- Type management ----------------------------------------------------

    /**
     * Change the active type lens. Does NOT clear memory — the raw bytes stay
     * so you can observe reinterpretation effects.
     *
     * @param  {string} newType  One of ALL_TYPES
     * @return {boolean} true on success
     */
    setType(newType) {
        this.#guardResolved('setType');
        if (!ALL_TYPES.has(newType)) {
            throw new TypeError(`Any.setType: unknown type '${newType}'. Valid: ${[...ALL_TYPES].join(', ')}`);
        }
        this.#type = newType;
        return true;
    }

    /**
     * @return {string} Current type tag
     */
    getType() {
        return this.#type;
    }

    // -- Read ---------------------------------------------------------------

    /**
     * Read the stored value through the current type lens.
     *
     * For composite types (obj, str, arr) an optional index/key narrows the read:
     *   box.setType('obj'); box.get('name')   → obj_mem['name']
     *   box.setType('str'); box.get(0)        → first character
     *   box.setType('arr'); box.get(2)        → third element
     *
     * @param  {string|number} [key] Optional index/key for composite types
     * @return {*} The value
     */
    get(key) {
        this.#guardResolved('get');

        // Numeric path — one DataView call
        if (NUMERIC_TYPES.has(this.#type)) {
            const acc = NUMERIC_ACCESSORS[this.#type];
            return this.#view[acc.get](0, true);  // little-endian
        }

        // Composite path
        switch (this.#type) {
            case 'obj':
                return (key !== undefined) ? this.#obj[key] : this.#obj;
            case 'str':
                if (typeof key === 'number') {
                    this.#boundsCheck(key, this.#str.length, 'str');
                    return this.#str[key];
                }
                return this.#str;
            case 'arr':
                if (typeof key === 'number') {
                    this.#boundsCheck(key, this.#arr.length, 'arr');
                    return this.#arr[key];
                }
                return this.#arr;
        }
    }

    // -- Write --------------------------------------------------------------

    /**
     * Write a value through the current type lens.
     *
     * For composites an optional key/index targets a single element:
     *   box.setType('obj'); box.set(10, 'age')    → obj['age'] = 10
     *   box.setType('str'); box.set('o', 1)       → replace char at index 1
     *   box.setType('arr'); box.set(99, 2)        → arr[2] = 99
     *
     * @param  {*}             value  The value to store
     * @param  {string|number} [key]  Optional index/key for composite types
     * @return {boolean} true on success
     */
    set(value, key) {
        this.#guardResolved('set');

        // Numeric path
        if (NUMERIC_TYPES.has(this.#type)) {
            const acc = NUMERIC_ACCESSORS[this.#type];
            // Validate BigInt requirement for 64-bit types
            if (acc.bigint && typeof value !== 'bigint') {
                throw new TypeError(
                    `Any.set: type '${this.#type}' requires a BigInt value (e.g. 42n), got ${typeof value}`
                );
            }
            if (!acc.bigint && typeof value !== 'number') {
                throw new TypeError(
                    `Any.set: type '${this.#type}' requires a number value, got ${typeof value}`
                );
            }
            this.#view[acc.set](0, value, true);  // little-endian
            return true;
        }

        // Composite path
        switch (this.#type) {
            case 'obj':
                if (key !== undefined) {
                    this.#obj[key] = value;
                } else {
                    if (typeof value !== 'object' || value === null) {
                        throw new TypeError('Any.set: obj view requires an object value');
                    }
                    this.#obj = value;
                }
                return true;

            case 'str':
                if (typeof key === 'number') {
                    this.#boundsCheck(key, this.#str.length, 'str');
                    this.#str = this.#str.slice(0, key) + value + this.#str.slice(key + 1);
                } else {
                    if (typeof value !== 'string') {
                        throw new TypeError('Any.set: str view requires a string value');
                    }
                    this.#str = value;
                }
                return true;

            case 'arr':
                if (typeof key === 'number') {
                    this.#boundsCheck(key, this.#arr.length, 'arr');
                    this.#arr[key] = value;
                } else {
                    if (!Array.isArray(value)) {
                        throw new TypeError('Any.set: arr view requires an array value');
                    }
                    this.#arr = value;
                }
                return true;
        }
    }

    // -- Resolve (consume) --------------------------------------------------

    /**
     * Freeze the box into a concrete typed value and return it.
     *
     * In ARIA, resolve::<T>() returns a typed pointer and the any box becomes
     * unusable (consuming semantics). We simulate this by returning the final
     * value and marking the box as resolved — further get/set/setType calls
     * will throw.
     *
     * @return {*} The resolved value
     */
    resolve() {
        this.#guardResolved('resolve');
        this.#resolved = true;

        if (NUMERIC_TYPES.has(this.#type)) {
            return this.#view[NUMERIC_ACCESSORS[this.#type].get](0, true);
        }

        switch (this.#type) {
            case 'obj': return structuredClone(this.#obj);
            case 'str': return this.#str;
            case 'arr': return [...this.#arr];
        }
    }

    /**
     * @return {boolean} Whether resolve() has been called
     */
    isResolved() {
        return this.#resolved;
    }

    // -- Display ------------------------------------------------------------

    /**
     * @return {string} Human-readable representation
     */
    toString() {
        if (this.#resolved) return '[Any: resolved]';
        const val = this.get();
        return `[Any<${this.#type}>: ${typeof val === 'object' ? JSON.stringify(val) : val}]`;
    }

    // -- Raw buffer access --------------------------------------------------

    /**
     * Dump the raw 8 bytes as a hex string — useful for seeing what's actually
     * in memory when you switch type lenses.
     *
     * @return {string} e.g. "ff 01 00 00 00 00 00 00"
     */
    rawHex() {
        return [...new Uint8Array(this.#buffer)]
            .map(b => b.toString(16).padStart(2, '0'))
            .join(' ');
    }

    // -- Private helpers ----------------------------------------------------

    #guardResolved(method) {
        if (this.#resolved) {
            throw new Error(
                `Any.${method}(): box is resolved — cannot reuse after resolve(). ` +
                'In ARIA this is a compile-time error (consuming semantics).'
            );
        }
    }

    #boundsCheck(index, length, typeName) {
        if (index < 0 || index >= length) {
            throw new RangeError(
                `Any.get/set: index ${index} out of bounds for ${typeName} (length ${length})`
            );
        }
    }
}

// ---------------------------------------------------------------------------
// Export for both Node.js (CommonJS / ESM) and browser
// ---------------------------------------------------------------------------
if (typeof module !== 'undefined' && module.exports) {
    module.exports = { Any, NUMERIC_TYPES, COMPOSITE_TYPES, ALL_TYPES };
}
