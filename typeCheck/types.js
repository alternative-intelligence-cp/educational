/**
 * TypeCheck Module
 * A comprehensive, environment-agnostic type checking library for JavaScript.
 * 
 * Features:
 * - UMD support for Browser, Worker, and Node.js.
 * - Exhaustive primitive and reference type checks.
 * - TypedArray and Binary data introspection.
 * - Recursive Schema Validation.
 */
(function (root, factory) {
    // 1. Universal Module Definition (UMD)
    if (typeof define === 'function' && define.amd) {
        // AMD. Register as an anonymous module.
        define(['exports'], factory);
    } else if (typeof exports === 'object' && typeof exports.nodeName !== 'string') {
        // CommonJS (Node.js)
        factory(exports);
    } else {
        // Browser globals (root is window, self, or globalThis)
        // Ensure we find the global object even in strict mode
        var globalRef = (typeof globalThis !== 'undefined' ? globalThis :
            (typeof self !== 'undefined' ? self :
                (typeof window !== 'undefined' ? window : root)));
        globalRef.TypeCheck = {};
        factory(globalRef.TypeCheck);
    }
})(this, function (exports) {
    "use strict";

    // --- Core Introspection ---

    const toString = Object.prototype.toString;

    /**
     * Gets the internal [[Class]] tag of a value.
     * @param {*} value - The value to check.
     * @returns {string} - The tag, e.g., "Array", "String", "Map".
     */
    function getTag(value) {
        if (value == null) {
            return value === undefined ? "Undefined" : "Null";
        }
        return toString.call(value).slice(8, -1);
    }

    // --- Primitive & Scalar Types ---

    exports.isUndefined = (val) => val === undefined;
    exports.isNull = (val) => val === null;
    exports.isNil = (val) => val == null; // Checks both null and undefined

    exports.isString = (val) => typeof val === 'string';
    exports.isBoolean = (val) => typeof val === 'boolean';
    exports.isSymbol = (val) => typeof val === 'symbol';
    exports.isBigInt = (val) => typeof val === 'bigint';

    // Numeric Nuances
    exports.isNumber = (val) => typeof val === 'number' && !Number.isNaN(val);
    exports.isNaN = (val) => Number.isNaN(val);
    exports.isFinite = (val) => Number.isFinite(val);
    exports.isInteger = (val) => Number.isInteger(val);
    exports.isSafeInteger = (val) => Number.isSafeInteger(val);
    exports.isFloat = (val) => exports.isNumber(val) && !Number.isInteger(val) && Number.isFinite(val);

    // --- Complex Reference Types ---

    exports.isFunction = (val) => typeof val === 'function';

    // Generic Object check (excludes null)
    exports.isObject = (val) => val !== null && typeof val === 'object';

    // Plain Object: Created by {} or new Object()
    exports.isPlainObject = (val) => {
        if (getTag(val) !== 'Object') return false;
        const proto = Object.getPrototypeOf(val);
        return proto === null || proto === Object.prototype;
    };

    // Arrays
    exports.isArray = Array.isArray;
    exports.isArrayLike = (val) => {
        return exports.isObject(val) && !exports.isFunction(val) &&
            exports.isInteger(val.length) && val.length >= 0;
    };

    // --- Built-in Objects ---

    exports.isDate = (val) => getTag(val) === 'Date' && !Number.isNaN(val.getTime());
    exports.isRegExp = (val) => getTag(val) === 'RegExp';
    exports.isError = (val) => getTag(val) === 'Error' || val instanceof Error;

    // Promises
    exports.isPromise = (val) => {
        return !!val && (getTag(val) === 'Promise' ||
            (typeof val.then === 'function' && typeof val.catch === 'function'));
    };

    // Collections
    exports.isMap = (val) => getTag(val) === 'Map';
    exports.isSet = (val) => getTag(val) === 'Set';
    exports.isWeakMap = (val) => getTag(val) === 'WeakMap';
    exports.isWeakSet = (val) => getTag(val) === 'WeakSet';

    // --- Binary Data Types ---

    exports.isArrayBuffer = (val) => getTag(val) === 'ArrayBuffer';
    exports.isSharedArrayBuffer = (val) => getTag(val) === 'SharedArrayBuffer';
    exports.isDataView = (val) => getTag(val) === 'DataView';

    // Typed Arrays (Abstract Check)
    // We safeguard access to Uint8Array in case the environment is very old, 
    // though unlikely given the UMD target.
    var TypedArrayProto = typeof Uint8Array !== 'undefined' ? Object.getPrototypeOf(Uint8Array) : undefined;
    exports.isTypedArray = (val) => TypedArrayProto ? val instanceof TypedArrayProto : false;

    // Specific Typed Arrays
    const typedArrayTags = [
        'Int8Array', 'Uint8Array', 'Uint8ClampedArray',
        'Int16Array', 'Uint16Array',
        'Int32Array', 'Uint32Array',
        'Float32Array', 'Float64Array',
        'BigInt64Array', 'BigUint64Array'
    ];
    typedArrayTags.forEach(function (tag) {
        exports['is' + tag] = (val) => getTag(val) === tag;
    });

    // --- Generator & Async Functions ---

    exports.isGeneratorFunction = (val) => getTag(val) === 'GeneratorFunction';
    exports.isAsyncFunction = (val) => getTag(val) === 'AsyncFunction';
    exports.isAsyncGeneratorFunction = (val) => getTag(val) === 'AsyncGeneratorFunction';

    // Duck Typing for Generator Objects (Iterator + throw/return)
    exports.isGenerator = (val) => exports.isObject(val) &&
        typeof val.next === 'function' &&
        typeof val.throw === 'function';

    // --- Structural Validation (Schema Engine) ---

    /**
     * Helper to capitalize type names (e.g., 'string' -> 'String')
     */
    function capitalize(str) {
        return str.charAt(0).toUpperCase() + str.slice(1);
    }

    /**
     * Validates an object against a schema.
     * Supports recursion and array validation.
     * 
     * @param {Object} schema - The definition of expected types.
     * @param {Object} target - The object to validate.
     * @param {string} [path='root'] - The current path for error reporting.
     * @returns {boolean} - True if valid, false otherwise.
     */
    exports.validate = function validate(schema, target, path) {
        path = path || 'root';

        if (!exports.isObject(target)) {
            console.warn(`TypeCheck: Target at '${path}' is not an object.`);
            return false;
        }

        for (const key in schema) {
            if (Object.prototype.hasOwnProperty.call(schema, key)) {
                const rule = schema[key];
                const value = target[key];
                const currentPath = path + '.' + key;

                // 1. Recursive Object Validation
                // If the rule itself is a plain object and has no 'type' property, assume nested schema
                if (exports.isPlainObject(rule) && !rule.type) {
                    if (!exports.isObject(value)) {
                        console.warn(`TypeCheck: Expected object at '${currentPath}'`);
                        return false;
                    }
                    if (!validate(rule, value, currentPath)) return false;
                    continue;
                }

                // 2. Normalize rule to object format
                // Example: { name: 'string' } -> { name: { type: 'string' } }
                const ruleObj = exports.isString(rule) ? { type: rule } : rule;

                // 3. Handle 'required' (default is true unless optional: true)
                if (value === undefined) {
                    if (ruleObj.optional) continue;
                    console.warn(`TypeCheck: Missing required property '${currentPath}'`);
                    return false;
                }

                // 4. Type Check
                const expectedType = ruleObj.type;

                // Array Validation Strategy
                if (expectedType === 'array') {
                    if (!exports.isArray(value)) {
                        console.warn(`TypeCheck: Expected array at '${currentPath}'`);
                        return false;
                    }
                    if (ruleObj.items) {
                        // items can be a string ('string') or a schema object
                        const isSchema = exports.isPlainObject(ruleObj.items);

                        for (let i = 0; i < value.length; i++) {
                            const item = value[i];
                            const itemPath = `${currentPath}[${i}]`;

                            if (isSchema) {
                                // Recursive validation for array of objects
                                if (!validate(ruleObj.items, item, itemPath)) return false;
                            } else {
                                // Primitive check for array of scalars
                                const checkFn = exports['is' + capitalize(ruleObj.items)];
                                if (checkFn && !checkFn(item)) {
                                    console.warn(`TypeCheck: Invalid item at '${itemPath}'. Expected ${ruleObj.items}.`);
                                    return false;
                                }
                            }
                        }
                    }
                }
                // Standard Type Check
                else if (expectedType) {
                    const checkFn = exports['is' + capitalize(expectedType)];
                    if (!checkFn) {
                        console.warn(`TypeCheck: Unknown type '${expectedType}' in schema.`);
                        return false;
                    }
                    if (!checkFn(value)) {
                        console.warn(`TypeCheck: Invalid type at '${currentPath}'. Expected ${expectedType}, got ${getTag(value)}.`);
                        return false;
                    }
                }

                // 5. Custom Validator (Duck Typing)
                // Allows checking for specific properties or values
                if (ruleObj.validator && typeof ruleObj.validator === 'function') {
                    if (!ruleObj.validator(value)) {
                        console.warn(`TypeCheck: Custom validation failed at '${currentPath}'`);
                        return false;
                    }
                }
            }
        }
        return true;
    };
});