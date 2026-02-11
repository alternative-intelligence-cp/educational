#!/usr/bin/env node
((args)=>{//##### shapp -- Shaggy's App System #################################
    if(typeof globalThis.shapp == 'object') {
        console.warn("Not loading app.js, it already exists! ");
        return 1;
    };

    //***** Private Members ****************************************************
    let modules={};
    let public_interface={
        /*any*/get:(/*string*/name)=>{
            if(modules.hasOwnProperty(name)){
                return modules[name];
            }
            console.error("Module with name " + name + " not found!");
            return null;
        },
        /*bool*/add:(/*string*/name,/*any*/module)=>{
            if(!modules.hasOwnProperty(name)){
                modules[name]=module;
                return true;
            }
            return false;
        },
        /*bool*/remove:(/*string*/name)=>{
            if(modules.hasOwnProperty(name)){
                delete modules[name];
                return true;
            }
            return false;
        },
        /*string*/list:()=>{
            let out=""
            for(let mod in modules){
                out += mod + ", "
            }
            if( out.length > 0){
                out = out.slice(0, -2);  // Remove trailing ", "
            }
            return out;
        }
    }

    //***** EXPORT *************************************************************
    const mod = Object.freeze(public_interface);
    globalThis.shapp=mod;
    Object.freeze(globalThis.shapp);
    
})({//##### BUILD ARGUMENTS ####################################################
    Author: "Randy Hoggard",
    Version: "1.0.0",
    Date: "February 10, 2026",
    Dependencies: {
        //Example:require("./example.js")
    }
});