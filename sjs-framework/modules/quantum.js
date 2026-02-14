/**
 * SJS Quantum Module - Q3 and Q9 Quantum Number Types
 * 
 * Implements quantum types that maintain TWO hypotheses simultaneously with
 * a confidence metric. Models uncertainty and gradual evidence accumulation.
 * 
 * Perfect for:
 * - Sensor fusion (conflicting readings)
 * - AI decision-making (multiple hypotheses)
 * - Scientific measurement (uncertainty tracking)
 * - Cognitive modeling (gradient thinking vs binary)
 * - Graceful degradation (sensor failures)
 * 
 * Features:
 * - Q3: 3 confidence states (-1, 0, +1)
 * - Q9: 9 confidence states (-4 to +4)
 * - Superposition: Operations apply to both hypotheses
 * - Crystallization: Collapse to single value when confident
 * - Q-functions: Conditional operations based on hypothesis state
 * - Evidence accumulation: Build confidence gradually
 * 
 * Integration:
 * - Uses SJS pub/sub for quantum state events (optional)
 * - Uses SJS logger for debugging (optional)
 * 
 * @module quantum
 * @version 1.0.0
 */

(function (global) {
    'use strict';

    const quantum = {
        name: 'quantum',
        version: '1.0.0',
        dependencies: [] // Optional: ['pubSub', 'logger']
    };

    //<===> Class Constants <==================================================>
    const Q_UNKNOWN = 0;
    const Q_ID_ERR = -1;
    const Q_TYPES = {
        Q3: {
            type: "Q3",
            c_min: -1,
            c_max: 1,
            q_neither: 0,
            q_both: 0,
            q_last: -1,
            q_released: [],
            q_instances: {}
        },
        Q9: {
            type: "Q9",
            c_min: -4,
            c_max: 4,
            q_neither: -4,
            q_both: 4,
            q_last: -1,
            q_released: [],
            q_instances: {}
        }
    };
    
    // Configuration
    let config = {
        emitEvents: false,    // Emit pub/sub events for quantum state changes
        logOperations: false  // Log operations for debugging
    };

    //<===> Internal Functions <==================================================>
    function Q_GET_ID(q_type) {
        switch(q_type) {
            case 3:
                if(Q_TYPES.Q3.q_released.length > 0) {
                    return Q_TYPES.Q3.q_released.pop();
                }
                return ++Q_TYPES.Q3.q_last;
            case 9:
                if(Q_TYPES.Q9.q_released.length > 0) {
                    return Q_TYPES.Q9.q_released.pop();
                }
                return ++Q_TYPES.Q9.q_last;
            default:
                log('error', `Invalid q_type: ${q_type}`);
                return Q_ID_ERR;
        }
    }
    
    function Q_INSTANCE_DATA(width, a=0, b=0, c=0) {
        if(typeof a !== 'number' || typeof b !== 'number' || typeof c !== 'number') {
            log('error', 'Invalid types for a, b, or c');
            return null;
        }
        
        const q_type = (width === 3) ? Q_TYPES.Q3 : Q_TYPES.Q9;
        if(c < q_type.c_min || c > q_type.c_max) {
            log('error', `Confidence c=${c} out of range for Q${width}`);
            return null;
        }
        
        this.width = width;
        this.a = a;
        this.b = b;
        this.c = c;
    }

    function Q_CREATE(width, a=0, b=0, c=0) {
        if(typeof width !== 'number' || (width !== 3 && width !== 9)) {
            log('error', `Invalid width: ${width}`);
            return Q_ID_ERR;
        }
        
        const q_id = Q_GET_ID(width);
        if(q_id === Q_ID_ERR) {
            return Q_ID_ERR;
        }
        
        const instance = new Q_INSTANCE_DATA(width, a, b, c);
        if(!instance) {
            return Q_ID_ERR;
        }
        
        const q_type = (width === 3) ? Q_TYPES.Q3 : Q_TYPES.Q9;
        q_type.q_instances[q_id] = instance;
        
        emit('quantum:create', { width, q_id, a, b, c });
        return q_id;
    }

    function Q_GET(q_type, q_id) {
        const type_obj = (q_type === 3) ? Q_TYPES.Q3 : (q_type === 9) ? Q_TYPES.Q9 : null;
        if(!type_obj) {
            log('error', `Invalid q_type: ${q_type}`);
            return null;
        }
        return type_obj.q_instances[q_id] || null;
    }

    function Q_DESTROY(q_type, q_id) {
        const type_obj = (q_type === 3) ? Q_TYPES.Q3 : (q_type === 9) ? Q_TYPES.Q9 : null;
        if(!type_obj) {
            log('error', `Invalid q_type: ${q_type}`);
            return false;
        }
        
        if(!type_obj.q_instances.hasOwnProperty(q_id)) {
            log('error', `No instance with id ${q_id} for Q${q_type}`);
            return false;
        }
        
        type_obj.q_released.push(q_id);
        delete type_obj.q_instances[q_id];
        
        emit('quantum:destroy', { q_type, q_id });
        return true;
    }
    
    function Q_COMPARE(q) {
        if(q.c === 0) return null;
        
        if(q.width === 9) {
            if(q.c === Q_TYPES.Q9.q_neither || q.c === Q_TYPES.Q9.q_both) {
                return null;
            }
        }
        
        return (q.c < 0) ? q.a : q.b;
    }
    
    function Q_Q(op, q) {
        q.a = op(q.a);
        q.b = op(q.b);
    }
    
    function Q_QQ(op, q) {
        q.a = op(q.a);
        q.b = op(q.b);
        q.c = op(q.c);
    }

    //<===> Helper Functions <==================================================>
    function log(level, message) {
        if(config.logOperations) {
            // Try to use SJS logger if available
            const SJS = global.SJS;
            if(SJS && SJS.modules) {
                const logger = SJS.modules.get('logger');
                if(logger && logger[level]) {
                    logger[level](`[quantum] ${message}`);
                    return;
                }
            }
            // Fallback to console
            console[level](`[quantum] ${message}`);
        }
    }

    function emit(event, data) {
        if(config.emitEvents) {
            // Try to use SJS pubSub if available
            const SJS = global.SJS;
            if(SJS && SJS.modules) {
                const pubSub = SJS.modules.get('pubSub');
                if(pubSub && pubSub.publish) {
                    pubSub.publish(event, data);
                }
            }
        }
    }

    //<===> QuantumNumber Constructor <========================================>
    function QuantumNumber(width, a=0, b=0, c=0) {
        if(width !== 3 && width !== 9) {
            throw new Error(`Invalid width: ${width}. Must be 3 or 9`);
        }
        
        this.width = width;
        this.q_id = Q_CREATE(width, a, b, c);
        
        if(this.q_id === Q_ID_ERR) {
            throw new Error(`Failed to create Q${width} instance`);
        }
        
        this.getQID = function(){ return this.q_id; };
    }
    
    //<===> QuantumNumber Prototype Methods <===================================>
    QuantumNumber.prototype.destroy = function() {
        const success = Q_DESTROY(this.width, this.q_id);
        if(success) this.q_id = Q_ID_ERR;
        return success;
    };
    
    QuantumNumber.prototype.crystallize = function() {
        const q = Q_GET(this.width, this.q_id);
        if(!q) {
            log('error', 'Cannot crystallize destroyed instance');
            return null;
        }
        const result = Q_COMPARE(q);
        emit('quantum:crystallize', { width: this.width, q_id: this.q_id, result });
        return result;
    };
    
    QuantumNumber.prototype.qif = function(condition, a_change=0, b_change=0, c_change=0) {
        if(condition) {
            const q = Q_GET(this.width, this.q_id);
            if(q) {
                q.a += a_change;
                q.b += b_change;
                q.c += c_change;
                const q_type = (this.width === 3) ? Q_TYPES.Q3 : Q_TYPES.Q9;
                q.c = Math.max(q_type.c_min, Math.min(q_type.c_max, q.c));
                emit('quantum:update', { width: this.width, q_id: this.q_id, state: this.getState() });
            }
        }
        return this;
    };
    
    QuantumNumber.prototype.add = function(addend) {
        const q = Q_GET(this.width, this.q_id);
        if(q) Q_Q(v => v + addend, q);
        return this;
    };
    
    QuantumNumber.prototype.sub = function(subtrahend) {
        const q = Q_GET(this.width, this.q_id);
        if(q) Q_Q(v => v - subtrahend, q);
        return this;
    };
    
    QuantumNumber.prototype.mul = function(multiplier) {
        const q = Q_GET(this.width, this.q_id);
        if(q) Q_Q(v => v * multiplier, q);
        return this;
    };
    
    QuantumNumber.prototype.div = function(divisor) {
        if(divisor === 0) {
            log('error', 'Division by zero');
            return this;
        }
        const q = Q_GET(this.width, this.q_id);
        if(q) Q_Q(v => v / divisor, q);
        return this;
    };
    
    QuantumNumber.prototype.mod = function(modulus) {
        if(modulus === 0) {
            log('error', 'Modulo by zero');
            return this;
        }
        const q = Q_GET(this.width, this.q_id);
        if(q) Q_Q(v => v % modulus, q);
        return this;
    };
    
    QuantumNumber.prototype.sqrt = function() {
        const q = Q_GET(this.width, this.q_id);
        if(q) Q_Q(v => Math.sqrt(v), q);
        return this;
    };
    
    QuantumNumber.prototype.qor = function(condition, callback) {
        const q = Q_GET(this.width, this.q_id);
        if(q && (condition(q.a) || condition(q.b))) callback(q);
        return this;
    };
    
    QuantumNumber.prototype.qand = function(condition, callback) {
        const q = Q_GET(this.width, this.q_id);
        if(q && condition(q.a) && condition(q.b)) callback(q);
        return this;
    };
    
    QuantumNumber.prototype.qxor = function(condition, callback) {
        const q = Q_GET(this.width, this.q_id);
        if(q) {
            const a_meets = condition(q.a);
            const b_meets = condition(q.b);
            if(a_meets !== b_meets) callback(q);
        }
        return this;
    };
    
    QuantumNumber.prototype.qnor = function(condition, callback) {
        const q = Q_GET(this.width, this.q_id);
        if(q && !condition(q.a) && !condition(q.b)) callback(q);
        return this;
    };
    
    QuantumNumber.prototype.qnand = function(condition, callback) {
        const q = Q_GET(this.width, this.q_id);
        if(q && !(condition(q.a) && condition(q.b))) callback(q);
        return this;
    };
    
    QuantumNumber.prototype.qconf = function(condition, threshold, callback) {
        const q = Q_GET(this.width, this.q_id);
        if(q && (condition(q.a) || condition(q.b)) && Math.abs(q.c) >= threshold) {
            callback(q);
        }
        return this;
    };
    
    QuantumNumber.prototype.qnconf = function(condition, threshold, callback) {
        const q = Q_GET(this.width, this.q_id);
        if(q && (condition(q.a) || condition(q.b)) && Math.abs(q.c) < threshold) {
            callback(q);
        }
        return this;
    };
    
    QuantumNumber.prototype.getA = function() { 
        const q = Q_GET(this.width, this.q_id);
        return q ? q.a : null;
    };
    
    QuantumNumber.prototype.getB = function() { 
        const q = Q_GET(this.width, this.q_id);
        return q ? q.b : null;
    };
    
    QuantumNumber.prototype.getC = function() { 
        const q = Q_GET(this.width, this.q_id);
        return q ? q.c : null;
    };
    
    QuantumNumber.prototype.getState = function() {
        const q = Q_GET(this.width, this.q_id);
        return q ? {a: q.a, b: q.b, c: q.c, width: q.width} : null;
    };

    //<===> Public API <========================================================>
    /**
     * Configure the quantum module
     * @param {Object} options - Configuration options
     * @param {boolean} options.emitEvents - Emit pub/sub events
     * @param {boolean} options.logOperations - Log operations for debugging
     */
    quantum.configure = function(options = {}) {
        Object.assign(config, options);
        return quantum;
    };

    /**
     * Create a new quantum number
     * @param {number} width - 3 or 9
     * @param {number} a - Hypothesis A value
     * @param {number} b - Hypothesis B value
     * @param {number} c - Confidence value
     * @returns {QuantumNumber}
     */
    quantum.create = function(width, a, b, c) {
        return new QuantumNumber(width, a, b, c);
    };

    /**
     * Get statistics about quantum instances
     * @returns {Object} Statistics
     */
    quantum.stats = function() {
        return {
            Q3: {
                total: Q_TYPES.Q3.q_last + 1,
                active: Object.keys(Q_TYPES.Q3.q_instances).length,
                pooled: Q_TYPES.Q3.q_released.length
            },
            Q9: {
                total: Q_TYPES.Q9.q_last + 1,
                active: Object.keys(Q_TYPES.Q9.q_instances).length,
                pooled: Q_TYPES.Q9.q_released.length
            }
        };
    };

    /**
     * Reset all quantum instances (useful for testing)
     */
    quantum.reset = function() {
        Q_TYPES.Q3.q_instances = {};
        Q_TYPES.Q3.q_released = [];
        Q_TYPES.Q3.q_last = -1;
        Q_TYPES.Q9.q_instances = {};
        Q_TYPES.Q9.q_released = [];
        Q_TYPES.Q9.q_last = -1;
        emit('quantum:reset', {});
    };

    // Export QuantumNumber constructor
    quantum.QuantumNumber = QuantumNumber;

    //<===> Export Module <=====================================================>
    // For SJS framework (ES modules or global)
    if (typeof module !== 'undefined' && module.exports) {
        module.exports = quantum;
    } else if (typeof define === 'function' && define.amd) {
        define([], function() { return quantum; });
    } else {
        global.quantumModule = quantum;
    }

    return quantum;

})(typeof globalThis !== 'undefined' ? globalThis : typeof window !== 'undefined' ? window : this);
