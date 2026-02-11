#!/usr/bin/env node
((args)=>{//##### UINT16 LIST CLASS #############################################
    if(typeof shapp != 'object'){
        console.error("ERROR! Could not find shapp system!");    
        if (typeof window !== 'undefined' && typeof document !== 'undefined'){
            return 1;
        } else if(typeof process !== 'undefined' && !!process.versions && !!process.versions.node){
            process.exit(1)
        } else if(typeof self !== 'undefined'){
            return 1;
        } else {
            console.error("ERROR: Invalid environment!")
            return 1;
        }   
    }

    //***** META ***************************************************************
    let m_lastID=-1;
    let m_releasedIDs= [];
    const NULL_PTR = 0xFFFF; // Sentinel for null pointer
    const NODE_SIZE = 2;     // 1 block for value, 1 block for next ptr
    
    /*number*/function m_getID(){
        if(m_releasedIDs.length > 0){
            return m_releasedIDs.pop();
        } else {
            return ++m_lastID;
        }
    }

    //***** INSTANCE STORAGE ***************************************************
    let m_instances={};
    const m_array_size=4096;
    /*number*/function m_createInstance(){
        let instanceID= m_getID();
        m_instances[instanceID]={
            lastID:-1,
            releasedIDs:[],
            memory:new Uint16Array(m_array_size),
            // usedMeta and freeMeta removed/ignored in favor of head/tail/releasedIDs logic
            size:0,
            head: NULL_PTR,
            tail: NULL_PTR
        };
        return instanceID;
    }

    /*bool*/function m_destroyInstance(/*number*/instanceID){
        if(m_instances.hasOwnProperty(instanceID)){
            delete m_instances[instanceID];
            m_releasedIDs.push(instanceID);
            return true;
        }
        return false;
    }

    //Helper to allocate a node within an instance
    function m_allocNode(members, value) {
        let nodeID;
        if(members.releasedIDs.length > 0){
            nodeID = members.releasedIDs.pop();
        } else {
            nodeID = ++members.lastID;
        }
        
        // Check if we need to resize strictly (simplistic check for this toy example)
        if (nodeID * NODE_SIZE + 1 >= members.memory.length) {
            // Auto-grow if possible, or return failure
            // Using the prototype's grow logic manually here for safety
            const oldMem = members.memory;
            const newMem = new Uint16Array(oldMem.length + 1024); // Grow by block
            newMem.set(oldMem);
            members.memory = newMem;
        }

        members.memory[nodeID * NODE_SIZE] = value;
        members.memory[nodeID * NODE_SIZE + 1] = NULL_PTR;
        return nodeID;
    }

    function m_freeNode(members, nodeID) {
        members.releasedIDs.push(nodeID);
    }

    //***** CONSTRUCTOR ********************************************************
    /*Uint16List*/function Uint16List(){
        const _instanceID=m_createInstance();
        this.getInstanceID= function(){ return _instanceID; }
    }
    

    //***** PROTOTYPE/STATIC ***************************************************
    /*number*/Uint16List.prototype.getID=function(){
        let instanceID = this.getInstanceID();
        let members = m_instances.hasOwnProperty(instanceID) ?
            m_instances[instanceID] : null;
        if(members == null){
            console.error("Instance with ID: " + instanceID + " not found!");
            return -1;
        } 
        return instanceID;
    };

    /*bool*/Uint16List.prototype.push=function(/*Uint16*/num){
        let instanceID = this.getInstanceID();
        let members = m_instances.hasOwnProperty(instanceID) ?
            m_instances[instanceID] : null;
        if(members == null){ return false; }
        
        let newNodeIndex = m_allocNode(members, num);
        
        if (members.head === NULL_PTR) {
            members.head = newNodeIndex;
            members.tail = newNodeIndex;
        } else {
            // Link old tail to new node
            members.memory[members.tail * NODE_SIZE + 1] = newNodeIndex;
            members.tail = newNodeIndex;
        }
        members.size++;
        return true;
    }

    /*Uint16*/Uint16List.prototype.pop=function(){
        let instanceID = this.getInstanceID();
        let members = m_instances.hasOwnProperty(instanceID) ?
            m_instances[instanceID] : null;
        if(members == null || members.size === 0){ return -1; }
        
        let valueToReturn = members.memory[members.tail * NODE_SIZE];
        let nodeToRemove = members.tail;

        if (members.head === members.tail) {
            members.head = NULL_PTR;
            members.tail = NULL_PTR;
        } else {
            // Singly linked list: traverse to find node before tail
            let curr = members.head;
            while(members.memory[curr * NODE_SIZE + 1] !== members.tail) {
                curr = members.memory[curr * NODE_SIZE + 1];
            }
            members.tail = curr;
            members.memory[curr * NODE_SIZE + 1] = NULL_PTR;
        }

        m_freeNode(members, nodeToRemove);
        members.size--;
        return valueToReturn;
    }

    /*bool*/Uint16List.prototype.pad=function(/*Uint16*/num){
        let instanceID = this.getInstanceID();
        let members = m_instances.hasOwnProperty(instanceID) ?
            m_instances[instanceID] : null;
        if(members == null){ return false; }

        let newNodeIndex = m_allocNode(members, num);
        
        if (members.head === NULL_PTR) {
            members.head = newNodeIndex;
            members.tail = newNodeIndex;
        } else {
            members.memory[newNodeIndex * NODE_SIZE + 1] = members.head;
            members.head = newNodeIndex;
        }
        members.size++;
        return true;
    }

    /*Uint16*/Uint16List.prototype.pull=function(){
        let instanceID = this.getInstanceID();
        let members = m_instances.hasOwnProperty(instanceID) ?
            m_instances[instanceID] : null;
        if(members == null || members.size === 0){ return -1; }

        let nodeToRemove = members.head;
        let valueToReturn = members.memory[nodeToRemove * NODE_SIZE];

        members.head = members.memory[nodeToRemove * NODE_SIZE + 1];
        if (members.head === NULL_PTR) {
            members.tail = NULL_PTR; // List became empty
        }

        m_freeNode(members, nodeToRemove);
        members.size--;
        return valueToReturn;
    }

    /*bool*/Uint16List.prototype.insert=function(/*listNode*/after,/*Uint16*/num){
        let instanceID = this.getInstanceID();
        let members = m_instances.hasOwnProperty(instanceID) ?
            m_instances[instanceID] : null;
        if(members == null){ return false; }

        // Validate that 'after' is a valid node in the list
        let isValid = false;
        let curr = members.head;
        while(curr !== NULL_PTR) {
            if(curr === after) {
                isValid = true;
                break;
            }
            curr = members.memory[curr * NODE_SIZE + 1];
        }
        if(!isValid) { return false; }

        let newNodeIndex = m_allocNode(members, num);
        
        // Point new node to whatever 'after' was pointing to
        members.memory[newNodeIndex * NODE_SIZE + 1] = members.memory[after * NODE_SIZE + 1];
        
        // Point 'after' to new node
        members.memory[after * NODE_SIZE + 1] = newNodeIndex;

        // If we inserted after tail, update tail
        if (after === members.tail) {
            members.tail = newNodeIndex;
        }

        members.size++;
        return true;
    }

    /*Uint16*/Uint16List.prototype.remove=function(/*listNode*/target){
        let instanceID = this.getInstanceID();
        let members = m_instances.hasOwnProperty(instanceID) ?
            m_instances[instanceID] : null;
        if(members == null || members.size === 0){ return -1; }

        // If removing head
        if (target === members.head) {
            return this.pull();
        }

        // Find predecessor
        let prev = members.head;
        while(prev !== NULL_PTR && members.memory[prev * NODE_SIZE + 1] !== target) {
            prev = members.memory[prev * NODE_SIZE + 1];
        }

        // Target not found in chain
        if (prev === NULL_PTR) return -1;

        let valueToReturn = members.memory[target * NODE_SIZE];

        // Unlink
        members.memory[prev * NODE_SIZE + 1] = members.memory[target * NODE_SIZE + 1];

        // If removing tail, update tail
        if (target === members.tail) {
            members.tail = prev;
        }

        m_freeNode(members, target);
        members.size--;
        return valueToReturn;
    }

    /*bool*/Uint16List.prototype.sort=function(){
        let instanceID = this.getInstanceID();
        let members = m_instances.hasOwnProperty(instanceID) ?
            m_instances[instanceID] : null;
        if(members == null){ return false; }
        if(members.size < 2) return true;

        // Optimization: Extract values to array, sort, then write back.
        // Much faster than manipulating pointers for simple integer types.
        let values = new Uint16Array(members.size);
        let curr = members.head;
        let i = 0;
        while(curr !== NULL_PTR) {
            values[i++] = members.memory[curr * NODE_SIZE];
            curr = members.memory[curr * NODE_SIZE + 1];
        }

        values.sort();

        // Write back
        curr = members.head;
        i = 0;
        while(curr !== NULL_PTR) {
            members.memory[curr * NODE_SIZE] = values[i++];
            curr = members.memory[curr * NODE_SIZE + 1];
        }
        return true;
    }

    /*listNode*/Uint16List.prototype.find=function(/*Uint16*/target){
        let instanceID = this.getInstanceID();
        let members = m_instances.hasOwnProperty(instanceID) ?
            m_instances[instanceID] : null;
        if(members == null){ return -1; }

        let curr = members.head;
        while(curr !== NULL_PTR) {
            if (members.memory[curr * NODE_SIZE] === target) {
                return curr;
            }
            curr = members.memory[curr * NODE_SIZE + 1];
        }
        return -1;
    }

    /*num*/Uint16List.prototype.getSize=function(){
        let instanceID = this.getInstanceID();
        let members = m_instances.hasOwnProperty(instanceID) ?
            m_instances[instanceID] : null;
        if(members == null){ return -1; }
        return members.size;
    }

    /*num*/Uint16List.prototype.grow=function(/*num*/numVals){
        let instanceID = this.getInstanceID();
        let members = m_instances.hasOwnProperty(instanceID) ?
            m_instances[instanceID] : null;
        if(members == null){ return -1; }

        // numVals interpreted as number of NODES to add capacity for
        let additionalRawSize = numVals * NODE_SIZE;
        let newMemory = new Uint16Array(members.memory.length + additionalRawSize);
        newMemory.set(members.memory);
        members.memory = newMemory;
        
        return members.memory.length / NODE_SIZE; // Return new capacity in nodes
    }

    /*num*/Uint16List.prototype.shrink=function(/*num*/numVals){
        let instanceID = this.getInstanceID();
        let members = m_instances.hasOwnProperty(instanceID) ?
            m_instances[instanceID] : null;
        if(members == null){ return -1; }

        // Safety: Don't shrink if any nodes are in use
        // In a production allocator, we'd compact memory to reclaim fragmented space
        // For this educational example, we take the simple/safe approach
        if(members.size > 0) {
            console.warn("Cannot shrink: list is not empty. Clear the list first.");
            return -1;
        }
        
        // Also check if there are any allocated-but-not-linked nodes (edge case)
        // This can happen if nodes were allocated then freed without being inserted
        let actualUsedSlots = members.lastID + 1 - members.releasedIDs.length;
        if(actualUsedSlots > 0) {
            console.warn("Cannot shrink: allocated nodes still exist in memory.");
            return -1;
        }

        let removeRawSize = numVals * NODE_SIZE;
        if(members.memory.length - removeRawSize < 0) {
            console.warn("Cannot shrink: requested reduction exceeds memory size.");
            return -1;
        }

        let newMemory = members.memory.slice(0, members.memory.length - removeRawSize);
        members.memory = newMemory;
        
        return members.memory.length / NODE_SIZE;
    }
    
    //***** EXPORT *************************************************************
    shapp.add("Uint16List",Object.freeze(Uint16List));

})({//##### MODULE BUILD ARGS ##################################################
    Author: "Randy Hoggard",
    Version: "1.0.0",
    Date: "February 10, 2026",
    Dependencies: {
        //example: require("./example.js")
    }
});