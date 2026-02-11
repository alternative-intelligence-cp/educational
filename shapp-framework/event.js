#!/usr/bin/env node
((args)=>{//##### EVENT SYSTEM CLASS ###########################################
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
    
    /*number*/function m_getID(){
        if(m_releasedIDs.length > 0){
            return m_releasedIDs.pop();
        } else {
            return ++m_lastID;
        }
    }

    //***** INSTANCE STORAGE ***************************************************
    let m_instances={};

    /*number*/function m_createInstance(){
        let instanceID= m_getID();
        m_instances[instanceID]={
            lastID:-1,
            releasedIDs:[],
            events:{}
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

    //***** CONSTRUCTOR ********************************************************
    /*EventSystem*/function EventSystem(){
        const _instanceID=m_createInstance();
        this.getInstanceID= function(){ return _instanceID; }
    }

    //***** PROTOTYPE/STATIC ***************************************************
    /*number*/EventSystem.prototype.getID=function(){
        let instanceID = this.getInstanceID();
        let members = m_instances.hasOwnProperty(instanceID) ?
            m_instances[instanceID] : null;
        if(members == null){
            console.error("Instance with ID: " + instanceID + " not found!");
            return -1;
        } 
        if(members.releasedIDs.length > 0){
            return members.releasedIDs.pop();
        } else {
            return ++members.lastID;
        }
    };

    /*string*/EventSystem.prototype.getHandle=function(/*string*/event){
        return event + "_" + this.getID();
    };

    /*string*/EventSystem.prototype.registerEventHandler=function(/*string*/event,/*function*/handler){
        let instanceID = this.getInstanceID();
        let members = m_instances[instanceID] || null;
        if(members == null){
            console.error("Instance with ID: " + instanceID + " not found!");
            return null;
        } 
        if(!members.events.hasOwnProperty(event)){
            members.events[event]={};
        }
        let handle = this.getHandle(event);
        members.events[event][handle]=handler;
        return handle;
    };

    /*bool*/EventSystem.prototype.removeEventHandler=function(/*string*/handle){
        let instanceID = this.getInstanceID();
        let members = m_instances[instanceID] || null;
        if(members == null){
            console.error("Instance with ID: " + instanceID + " not found!");
            return false;
        } 
        // Extract event name (handle format: "event_name_id")
        let lastUnderscore = handle.lastIndexOf("_");
        let event = handle.substring(0, lastUnderscore);
        if(members.events.hasOwnProperty(event)){
            if(members.events[event].hasOwnProperty(handle)){
                delete members.events[event][handle];
                let id = parseInt(handle.substring(lastUnderscore+1,handle.length-1));
                members.releasedIDs.push(id);
                return true;
            }
        }
        return false;
    };

    /*bool*/EventSystem.prototype.raiseEvent=function(/*string*/event,/*any*/dat){
        let instanceID = this.getInstanceID();
        let members = m_instances[instanceID] || null;
        if(members == null){
            console.error("Instance with ID: " + instanceID + " not found!");
            return false;
        }
        if(members.events.hasOwnProperty(event)){             
            for(let h in members.events[event]){
                let handler = members.events[event][h];
                handler(event,dat);
            }
            return true;
        }
        return false;
    };

    //***** EXPORT *************************************************************
    shapp.add("EventSystem",Object.freeze(EventSystem));

})({//##### MODULE BUILD ARGS ##################################################
    Author: "Randy Hoggard",
    Version: "1.0.0",
    Date: "February 10, 2026",
    Dependencies: {
        //example: require("./example.js")
    }
});

