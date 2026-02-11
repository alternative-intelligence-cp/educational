#!/usr/bin/env node
//determine what environment we are running in
let env;
if (typeof window !== 'undefined' && typeof document !== 'undefined'){
    env="browser"
} else if(typeof process !== 'undefined' && !!process.versions && !!process.versions.node){
    env="node";
} else if(typeof self !== 'undefined'){
    env="worker";
} else {
    throw new Error("invalid environment");
}

//load our files
switch(env){
    case "browser":
        let shappScript= document.createElement("script");
        let eventScript= document.createElement("script");
        let uint16ListScript= document.createElement("script");
        shappScript.type="text/javascript";
        eventScript.type="text/javascript";
        uint16ListScript.type="text/javascript";
        shappScript.src="shapp.js"
        eventScript.src="event.js"
        uint16ListScript.src="uint16List.js"
        document.body.appendChild(shappScript);
        document.body.appendChild(eventScript);
        document.body.appendChild(uint16ListScript);
        let todo=3;
        let done=0;
        shappScript.onload=onDone;
        eventScript.onload=onDone;
        uint16ListScript.onload=onDone;
        function onDone(){ 
            done++;
            if(done == todo){
                main();
            }
        }
        break;
    case "node":
        require("./shapp.js");
        require("./event.js");
        require("./uint16List.js");
        main();
        break;
    case "worker":
        importScripts(["./shapp.js", "./event.js", "./uint16List.js"]);
        main();
        break;
    default:
        throw new Error("invalid environment");
        break;
}

//the main program
function main(){
    //get EventSystem module
    //make sure shapp framework is loaded
    let es_mod = "EventSystem"
    if(!globalThis.shapp){
        console.error("Could not find shapp system!");
        switch(env){
            case "node":
                process.exit(1);
                break;
            default:
                throw new Error("Could not find shapp system");
                break;
        }
    }
    //get the module from shapp
    let EventSystem = globalThis.shapp.get(es_mod);
    if(EventSystem == null ){
        console.error("Could not find module with name " + es_mod);
        switch(env){ 
            case "node":
                process.exit(2);
                break;
            default:
                throw new Error("Could not find module with name " + es_mod);
                break;
        }
    }

    //create new EventSystem
    let events = new EventSystem();

    //create different event handlers for 'my_event' event
    let handle1 = events.registerEventHandler('my_event', (event,data)=>{
        console.log("EVENT ==> " + event + " with data: " + data + " handled by handle1");
            if(env == "browser"){
                document.getElementById("output").innerHTML += "EVENT ==> " + event + " with data: " + '<span class="red_data">' + data + "</span> handled by handle1<br/>";
            }
        return true;
    });
    let handle2 = events.registerEventHandler('my_event',(event,data)=>{
        console.log("EVENT: " + event + " with data: " + data + " handled by handle2");
        if(env == "browser"){
            document.getElementById("output").innerHTML += "EVENT: " + event + " with data: " + '<span class="red_data">' + data + "</span> handled by handle2<br/>";
        }
        return true;
    });
    let handle3 = events.registerEventHandler('my_event',(event,data)=>{
        console.log("!E! >>> " + event + ", data: " + data + " handled by handle3 <<< !E!");
        if(env == "browser"){
            document.getElementById("output").innerHTML += "!E! >>> " + event + " , data: " + '<span class="red_data">' + data + "</span> handled by handle3 <<< !E!<br/>";
        }
        return true;
    });

    //raise event 'my_event'
    events.raiseEvent('my_event',420);

    //-------------------------------------------------------------------
    // Uint16List Example
    //-------------------------------------------------------------------
    if(env == "browser"){
        document.getElementById("output").innerHTML += "<br/>=== Uint16List Demo ===<br/>";
    }
    console.log("\n=== Uint16List Demo ===");

    // Get the Uint16List module
    let Uint16List = globalThis.shapp.get("Uint16List");
    if(Uint16List == null) {
        console.error("Could not find Uint16List module");
        if(env == "node") process.exit(3);
    }

    // Create a new list
    let myList = new Uint16List();
    
    // Push some values (add to end)
    myList.push(100);
    myList.push(200);
    myList.push(300);
    console.log("After push(100,200,300): size = " + myList.getSize());
    if(env == "browser"){
        document.getElementById("output").innerHTML += "After push(100,200,300): size = " + myList.getSize() + "<br/>";
    }

    // Pad (add to beginning)
    myList.pad(50);
    console.log("After pad(50): size = " + myList.getSize());
    if(env == "browser"){
        document.getElementById("output").innerHTML += "After pad(50): size = " + myList.getSize() + "<br/>";
    }

    // Find a value
    let node200 = myList.find(200);
    console.log("Find(200) returned node: " + node200);
    if(env == "browser"){
        document.getElementById("output").innerHTML += "Find(200) returned node: " + '<span class="red_data">' + node200 + "</span><br/>";
    }

    // Insert after node
    if(node200 !== -1) {
        myList.insert(node200, 250);
        console.log("After insert(after 200, value=250): size = " + myList.getSize());
        if(env == "browser"){
            document.getElementById("output").innerHTML += "After insert(after 200, value=250): size = " + myList.getSize() + "<br/>";
        }
    }

    // Pull from front and pop from back
    let frontVal = myList.pull();
    let backVal = myList.pop();
    console.log("Pulled from front: " + frontVal + ", popped from back: " + backVal);
    if(env == "browser"){
        document.getElementById("output").innerHTML += "Pulled from front: " + '<span class="red_data">' + frontVal + "</span>, popped from back: " + '<span class="red_data">' + backVal + "</span><br/>";
    }

    // Sort the list
    myList.sort();
    console.log("After sort(): size = " + myList.getSize());
    if(env == "browser"){
        document.getElementById("output").innerHTML += "After sort(): size = " + myList.getSize() + "<br/>";
    }

    // Display final state by pulling all values
    console.log("List contents (in order):");
    if(env == "browser"){
        document.getElementById("output").innerHTML += "List contents (in order): ";
    }
    let values = [];
    while(myList.getSize() > 0) {
        values.push(myList.pull());
    }
    console.log(values.join(", "));
    if(env == "browser"){
        document.getElementById("output").innerHTML += '<span class="red_data">' + values.join(", ") + "</span><br/>";
    }

    //exit successfully from node, cannot exit main loop in browser
    if(env == "node") { 
        process.exit(0);
    } 
}