((args)=>{//##### QUANTUM.JS ###################################################
    //<===> Class Constants <==================================================>
    const Q_UNKNOWN = 0;
    const Q_ID_ERR = -1;
    const Q_TYPES = {
        Q3: {
            type: "Q3",
            c_min: -1,
            c_max: 1,
            q_neither: 0,    // Q3 doesn't have distinct neither/both
            q_both: 0,
            q_last: -1,
            q_released: [],
            q_instances: {}
        },
        Q9: {
            type: "Q9",
            c_min: -4,
            c_max: 4,
            q_neither: -4,   // NEITHER: Complete rejection
            q_both: 4,       // BOTH: Complete acceptance
            q_last: -1,
            q_released: [],
            q_instances: {}
        }
    };
    
    //<===> Class Functions <==================================================>
    /*int*/function Q_GET_ID(/*number*/q_type) {
        switch(q_type) {
            case 3:
                if(Q_TYPES.Q3.q_released.length > 0) {
                    return Q_TYPES.Q3.q_released.pop();
                }
                return ++Q_TYPES.Q3.q_last;
            case 9:
                if(Q_TYPES.Q9.q_released.length > 0) {
                    return Q_TYPES.Q9.q_released.pop();  // Fixed: was Q3
                }
                return ++Q_TYPES.Q9.q_last;
            default:
                console.error(`ERROR ==> Invalid q_type: ${q_type}! Valid types are 3 (Q3) and 9 (Q9)`);
                return Q_ID_ERR;
        }
    }
    
    /*Q_INSTANCE_DATA*/function Q_INSTANCE_DATA(/*number*/width, /*number*/a=0, /*number*/b=0, /*number*/c=0) {
        // Validate args
        if(typeof a !== 'number' || typeof b !== 'number' || typeof c !== 'number') {
            console.error(`ERROR ==> Invalid types for a, b, or c! Must all be numbers`);
            return null;
        }
        
        // Validate confidence range based on width
        const q_type = (width === 3) ? Q_TYPES.Q3 : Q_TYPES.Q9;
        if(c < q_type.c_min || c > q_type.c_max) {
            console.error(`ERROR ==> Confidence c=${c} out of range for Q${width} (${q_type.c_min} to ${q_type.c_max})`);
            return null;
        }
        
        this.width = width;
        this.a = a;
        this.b = b;
        this.c = c;
    }

    /*int*/function Q_CREATE(/*number*/width, /*number*/a=0, /*number*/b=0, /*number*/c=0) {
        // Validate width
        if(typeof width !== 'number' || (width !== 3 && width !== 9)) {
            console.error(`ERROR ==> Invalid width: ${width}! Must be 3 or 9`);
            return Q_ID_ERR;
        }
        
        // Get ID
        const q_id = Q_GET_ID(width);
        if(q_id === Q_ID_ERR) {
            return Q_ID_ERR;
        }
        
        // Create instance data
        const instance = new Q_INSTANCE_DATA(width, a, b, c);
        if(!instance) {
            return Q_ID_ERR;
        }
        
        // Store in appropriate width store with id as key
        const q_type = (width === 3) ? Q_TYPES.Q3 : Q_TYPES.Q9;
        q_type.q_instances[q_id] = instance;
        
        return q_id;
    }

    /*Q_INSTANCE_DATA*/function Q_GET(/*number*/q_type, /*number*/q_id) {
        // Retrieve the q_instance_data from Q_TYPES[q_type].q_instances[q_id]
        const type_obj = (q_type === 3) ? Q_TYPES.Q3 : (q_type === 9) ? Q_TYPES.Q9 : null;
        if(!type_obj) {
            console.error(`ERROR ==> Invalid q_type: ${q_type}`);
            return null;
        }
        
        const instance = type_obj.q_instances[q_id];
        return instance || null;
    }

    /*boolean*/function Q_DESTROY(/*number*/q_type, /*number*/q_id) {
        // Validate args
        const type_obj = (q_type === 3) ? Q_TYPES.Q3 : (q_type === 9) ? Q_TYPES.Q9 : null;
        if(!type_obj) {
            console.error(`ERROR ==> Invalid q_type: ${q_type}`);
            return false;
        }
        
        // Check if instance exists
        if(!type_obj.q_instances.hasOwnProperty(q_id)) {
            console.error(`ERROR ==> No instance with id ${q_id} for Q${q_type}`);
            return false;
        }
        
        // Return id to appropriate released pool
        type_obj.q_released.push(q_id);
        
        // Destroy instance data
        delete type_obj.q_instances[q_id];
        
        return true;
    }
    
    /*number|null*/function Q_COMPARE(/*Q_INSTANCE_DATA*/q) {
        // Order: unknown(0), neither(-4), both(+4), then highest abs(c)
        
        // Unknown: c === 0 (no clear winner)
        if(q.c === 0) {
            return null;  // Return null for unknown
        }
        
        // For Q9: Check NEITHER and BOTH states
        if(q.width === 9) {
            if(q.c === Q_TYPES.Q9.q_neither) {  // c === -4 (NEITHER)
                return null;  // No clear winner (both rejected)
            }
            if(q.c === Q_TYPES.Q9.q_both) {  // c === +4 (BOTH)
                return null;  // No clear winner (both accepted)
            }
        }
        
        // Return the option with highest confidence
        // Negative c = lean toward A, Positive c = lean toward B
        return (q.c < 0) ? q.a : q.b;
    }
    
    /*void*/function Q_Q(/*function(val)*/op, /*Q_INSTANCE_DATA*/q) {
        // Apply operation to BOTH a and b
        q.a = op(q.a);
        q.b = op(q.b);
    }
    
    /*void*/function Q_QQ(/*function(val)*/op, /*Q_INSTANCE_DATA*/q) {
        // Apply operation to ALL THREE: a, b, and c
        q.a = op(q.a);
        q.b = op(q.b);
        q.c = op(q.c);
    } 

    //<===> Constructor <======================================================>
    function QuantumNumber(/*int*/width, /*number*/a=0, /*number*/b=0, /*number*/c=0){
        // Validate width
        if(width !== 3 && width !== 9) {
            throw new Error(`Invalid width: ${width}. Must be 3 or 9`);
        }
        
        // Create quantum instance and store ID
        this.width = width;
        this.q_id = Q_CREATE(width, a, b, c);
        
        if(this.q_id === Q_ID_ERR) {
            throw new Error(`Failed to create Q${width} instance`);
        }
        
        this.getQID = function(){ return this.q_id; };
    }
    
    //<===> Public Interface <=================================================>
    QuantumNumber.prototype.destroy = function(){
        // Call Q_DESTROY with our stored q_id
        const success = Q_DESTROY(this.width, this.q_id);
        if(success) {
            this.q_id = Q_ID_ERR;  // Mark as destroyed
        }
        return success;
    };
    
    QuantumNumber.prototype.crystallize = function(){
        // Get instance data
        const q = Q_GET(this.width, this.q_id);
        if(!q) {
            console.error(`ERROR ==> Cannot crystallize destroyed instance`);
            return null;
        }
        
        // Check if comparison yields a result (not unknown/neither/both)
        const result = Q_COMPARE(q);
        
        // If comparison returns null (unknown state), cannot crystallize
        if(result === null) {
            return null;
        }
        
        // Return crystallized value (immutable copy)
        return result;
    };
    
    QuantumNumber.prototype.qif = function(/*boolean*/condition, /*number*/a_change=0, /*number*/b_change=0, /*number*/c_change=0){
        // If condition is true, apply changes to quantum state
        if(condition) {
            const q = Q_GET(this.width, this.q_id);
            if(q) {
                q.a += a_change;
                q.b += b_change;
                q.c += c_change;
                
                // Clamp confidence to valid range
                const q_type = (this.width === 3) ? Q_TYPES.Q3 : Q_TYPES.Q9;
                q.c = Math.max(q_type.c_min, Math.min(q_type.c_max, q.c));
            }
        }
        return this;
    };
    
    // Arithmetic operations (apply to both hypotheses)
    QuantumNumber.prototype.add = function(/*number*/addend){
        const q = Q_GET(this.width, this.q_id);
        if(q) {
            Q_Q(v => v + addend, q);
        }
        return this;
    };
    
    QuantumNumber.prototype.sub = function(/*number*/subtrahend){
        const q = Q_GET(this.width, this.q_id);
        if(q) {
            Q_Q(v => v - subtrahend, q);
        }
        return this;
    };
    
    QuantumNumber.prototype.mul = function(/*number*/multiplier){
        const q = Q_GET(this.width, this.q_id);
        if(q) {
            Q_Q(v => v * multiplier, q);
        }
        return this;
    };
    
    QuantumNumber.prototype.div = function(/*number*/divisor){
        if(divisor === 0) {
            console.error(`ERROR ==> Division by zero`);
            return this;
        }
        const q = Q_GET(this.width, this.q_id);
        if(q) {
            Q_Q(v => v / divisor, q);
        }
        return this;
    };
    
    QuantumNumber.prototype.mod = function(/*number*/modulus){
        if(modulus === 0) {
            console.error(`ERROR ==> Modulo by zero`);
            return this;
        }
        const q = Q_GET(this.width, this.q_id);
        if(q) {
            Q_Q(v => v % modulus, q);
        }
        return this;
    };
    
    QuantumNumber.prototype.sqrt = function(){
        const q = Q_GET(this.width, this.q_id);
        if(q) {
            Q_Q(v => Math.sqrt(v), q);
        }
        return this;
    };
    
    // Q-functions: Conditional operations based on hypothesis state
    QuantumNumber.prototype.qor = function(/*function(val)*/condition, /*function(q)*/callback){
        // Apply if EITHER a or b meets condition
        const q = Q_GET(this.width, this.q_id);
        if(q && (condition(q.a) || condition(q.b))) {
            callback(q);
        }
        return this;
    };
    
    QuantumNumber.prototype.qand = function(/*function(val)*/condition, /*function(q)*/callback){
        // Apply only if BOTH a and b meet condition
        const q = Q_GET(this.width, this.q_id);
        if(q && condition(q.a) && condition(q.b)) {
            callback(q);
        }
        return this;
    };
    
    QuantumNumber.prototype.qxor = function(/*function(val)*/condition, /*function(q)*/callback){
        // Apply only if ONE but NOT BOTH meet condition
        const q = Q_GET(this.width, this.q_id);
        if(q) {
            const a_meets = condition(q.a);
            const b_meets = condition(q.b);
            if(a_meets !== b_meets) {  // XOR: one but not both
                callback(q);
            }
        }
        return this;
    };
    
    QuantumNumber.prototype.qnor = function(/*function(val)*/condition, /*function(q)*/callback){
        // Apply only if NEITHER meets condition
        const q = Q_GET(this.width, this.q_id);
        if(q && !condition(q.a) && !condition(q.b)) {
            callback(q);
        }
        return this;
    };
    
    QuantumNumber.prototype.qnand = function(/*function(val)*/condition, /*function(q)*/callback){
        // Apply only if NOT (both meet condition)
        const q = Q_GET(this.width, this.q_id);
        if(q && !(condition(q.a) && condition(q.b))) {
            callback(q);
        }
        return this;
    };
    
    QuantumNumber.prototype.qconf = function(/*function(val)*/condition, /*number*/threshold, /*function(q)*/callback){
        // Apply if condition met AND abs(c) >= threshold (high confidence)
        const q = Q_GET(this.width, this.q_id);
        if(q && (condition(q.a) || condition(q.b)) && Math.abs(q.c) >= threshold) {
            callback(q);
        }
        return this;
    };
    
    QuantumNumber.prototype.qnconf = function(/*function(val)*/condition, /*number*/threshold, /*function(q)*/callback){
        // Apply if condition met AND abs(c) < threshold (low confidence)
        const q = Q_GET(this.width, this.q_id);
        if(q && (condition(q.a) || condition(q.b)) && Math.abs(q.c) < threshold) {
            callback(q);
        }
        return this;
    };
    
    // Getter methods for inspection
    QuantumNumber.prototype.getA = function(){ 
        const q = Q_GET(this.width, this.q_id);
        return q ? q.a : null;
    };
    
    QuantumNumber.prototype.getB = function(){ 
        const q = Q_GET(this.width, this.q_id);
        return q ? q.b : null;
    };
    
    QuantumNumber.prototype.getC = function(){ 
        const q = Q_GET(this.width, this.q_id);
        return q ? q.c : null;
    };
    
    QuantumNumber.prototype.getState = function(){
        const q = Q_GET(this.width, this.q_id);
        return q ? {a: q.a, b: q.b, c: q.c, width: q.width} : null;
    };

    //<===> Export <===========================================================>
    globalThis.QuantumNumber = Object.freeze(QuantumNumber);
})({//##########################################################################
    //build args here if needed, reserved for future use
});