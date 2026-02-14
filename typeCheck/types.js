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
     * Validates an object against a schema with detailed failure reporting.
     * Supports recursion and array validation.
     * 
     * @param {Object} schema - The definition of expected types.
     * @param {Object} target - The object to validate.
     * @param {string} [path='root'] - The current path for error reporting.
     * @returns {Object} - { isValid: boolean, failures: Array<{path, field, reason}> }
     */
    exports.validateDetailed = function validateDetailed(schema, target, path) {
        path = path || 'root';
        const failures = [];

        if (!exports.isObject(target)) {
            failures.push({
                path: path,
                field: path,
                reason: 'Target is not an object'
            });
            return { isValid: false, failures };
        }

        function addFailure(fieldPath, reason) {
            failures.push({
                path: path,
                field: fieldPath,
                reason: reason
            });
        }

        for (const key in schema) {
            if (Object.prototype.hasOwnProperty.call(schema, key)) {
                const rule = schema[key];
                const value = target[key];
                const currentPath = path + '.' + key;

                // 1. Recursive Object Validation
                if (exports.isPlainObject(rule) && !rule.type) {
                    if (!exports.isObject(value)) {
                        addFailure(currentPath, 'Expected object');
                        continue;
                    }
                    const nested = validateDetailed(rule, value, currentPath);
                    if (!nested.isValid) {
                        failures.push(...nested.failures);
                    }
                    continue;
                }

                // 2. Normalize rule to object format
                const ruleObj = exports.isString(rule) ? { type: rule } : rule;

                // 3. Handle 'required' (default is true unless optional: true)
                if (value === undefined) {
                    if (ruleObj.optional) continue;
                    addFailure(currentPath, 'Missing required property');
                    continue;
                }

                // 4. Type Check
                const expectedType = ruleObj.type;

                // Array Validation Strategy
                if (expectedType === 'array') {
                    if (!exports.isArray(value)) {
                        addFailure(currentPath, 'Expected array');
                        continue;
                    }
                    if (ruleObj.items) {
                        const isSchema = exports.isPlainObject(ruleObj.items);

                        for (let i = 0; i < value.length; i++) {
                            const item = value[i];
                            const itemPath = `${currentPath}[${i}]`;

                            if (isSchema) {
                                const itemResult = validateDetailed(ruleObj.items, item, itemPath);
                                if (!itemResult.isValid) {
                                    failures.push(...itemResult.failures);
                                }
                            } else {
                                const checkFn = exports['is' + capitalize(ruleObj.items)];
                                if (checkFn && !checkFn(item)) {
                                    addFailure(itemPath, `Expected ${ruleObj.items}, got ${getTag(item)}`);
                                }
                            }
                        }
                    }
                }
                // Standard Type Check
                else if (expectedType) {
                    const checkFn = exports['is' + capitalize(expectedType)];
                    if (!checkFn) {
                        addFailure(currentPath, `Unknown type '${expectedType}' in schema`);
                        continue;
                    }
                    if (!checkFn(value)) {
                        addFailure(currentPath, `Expected ${expectedType}, got ${getTag(value)}`);
                        continue;
                    }
                }

                // 5. Custom Validator (Duck Typing)
                if (ruleObj.validator && typeof ruleObj.validator === 'function') {
                    const validatorResult = ruleObj.validator(value);
                    
                    // Support both boolean and detailed validator responses
                    if (typeof validatorResult === 'object' && validatorResult.isValid === false) {
                        addFailure(currentPath, validatorResult.reason || 'Custom validation failed');
                    } else if (!validatorResult) {
                        addFailure(currentPath, 'Custom validation failed');
                    }
                }
            }
        }
        
        return {
            isValid: failures.length === 0,
            failures: failures
        };
    };

    /**
     * Validates an object against a schema (backward compatible boolean version).
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
    /**
     * Validates an object against a schema (backward compatible boolean version).
     * Supports recursion and array validation.
     * 
     * @param {Object} schema - The definition of expected types.
     * @param {Object} target - The object to validate.
     * @param {string} [path='root'] - The current path for error reporting.
     * @returns {boolean} - True if valid, false otherwise.
     */
    exports.validate = function validate(schema, target, path) {
        const result = exports.validateDetailed(schema, target, path);
        
        // Log failures for backward compatibility with console.warn behavior
        if (!result.isValid) {
            result.failures.forEach(failure => {
                console.warn(`TypeCheck: ${failure.reason} at '${failure.field}'`);
            });
        }
        
        return result.isValid;
    };

    /**
     * Type registry for custom type definitions.
     * Allows registration of domain-specific types with validation logic.
     */
    const _typeRegistry = {};

    /**
     * Register a custom type definition.
     * 
     * @param {string} name - The name of the type (e.g., 'email', 'username').
     * @param {Object} definition - Type definition with properties:
     *   - raw_type: Base JavaScript type (e.g., 'string', 'number')
     *   - required: Whether the value is required (default: true)
     *   - check: Custom validation function (value) => boolean or { isValid, reason }
     * 
     * @example
     * TypeCheck.registerType('email', {
     *     raw_type: 'string',
     *     required: true,
     *     check: (val) => /^[^\s@]+@[^\s@]+\.[^\s@]+$/.test(val)
     * });
     */
    exports.registerType = function registerType(name, definition) {
        _typeRegistry[name] = definition;
    };

    /**
     * Check if a value matches a registered custom type.
     * 
     * @param {*} value - The value to validate.
     * @param {string} typeName - The name of the registered type.
     * @returns {Object} - { isValid: boolean, failures: Array<{field, reason}> }
     * 
     * @example
     * const result = TypeCheck.checkType('bob@example.com', 'email');
     * if (!result.isValid) {
     *     result.failures.forEach(f => console.error(f.reason));
     * }
     */
    exports.checkType = function checkType(value, typeName) {
        const typeDef = _typeRegistry[typeName];
        
        if (!typeDef) {
            return {
                isValid: false,
                failures: [{ field: typeName, reason: `Unknown type '${typeName}'` }]
            };
        }

        const failures = [];

        // Check 'required' property
        if (value === undefined || value === null) {
            if (typeDef.required !== false) {
                failures.push({
                    field: typeName,
                    reason: 'Value is required but missing'
                });
            }
            return { isValid: failures.length === 0, failures };
        }

        // Check raw_type
        if (typeDef.raw_type) {
            const checkFn = exports['is' + capitalize(typeDef.raw_type)];
            if (checkFn && !checkFn(value)) {
                failures.push({
                    field: typeName,
                    reason: `Expected ${typeDef.raw_type}, got ${getTag(value)}`
                });
                return { isValid: false, failures };
            }
        }

        // Run custom check function
        if (typeDef.check && typeof typeDef.check === 'function') {
            const checkResult = typeDef.check(value);
            
            // Support both boolean and detailed responses
            if (typeof checkResult === 'object') {
                if (checkResult.isValid === false) {
                    failures.push({
                        field: typeName,
                        reason: checkResult.reason || 'Custom validation failed'
                    });
                }
            } else if (!checkResult) {
                failures.push({
                    field: typeName,
                    reason: 'Custom validation failed'
                });
            }
        }

        return {
            isValid: failures.length === 0,
            failures: failures
        };
    };

    /**
     * Simple boolean check for backward compatibility.
     * 
     * @param {*} value - The value to validate.
     * @param {string} typeName - The name of the registered type.
     * @returns {boolean} - True if valid, false otherwise.
     */
    exports.isValidType = function isValidType(value, typeName) {
        return exports.checkType(value, typeName).isValid;
    };
});