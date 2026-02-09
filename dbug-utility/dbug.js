((args)=>{//### MODULE #########################################################
    //===> MODULE VARS <========================================================
    const _name="dBug";
    const _public={};
    
    // Color constants
    const RED = 'RED';
    const YELLOW = 'YELLOW';
    const GREEN = 'GREEN';
    const WHITE = 'WHITE';
    const BLUE = 'BLUE';
    
    const _default_colors_terminal={
        [RED]: { heading: '\x1b[31m', reset: '\x1b[0m' },
        [YELLOW]: { heading: '\x1b[33m', reset: '\x1b[0m' },
        [GREEN]: { heading: '\x1b[32m', reset: '\x1b[0m' },
        [WHITE]: { heading: '\x1b[37m', reset: '\x1b[0m' },
        [BLUE]: { heading: '\x1b[34m', reset: '\x1b[0m' }
    };
    
    const _default_colors_console={
        [RED]: { heading: 'color: red; font-weight: bold;', reset: 'color: initial;' },
        [YELLOW]: { heading: 'color: #ff9800; font-weight: bold;', reset: 'color: initial;' },
        [GREEN]: { heading: 'color: green; font-weight: bold;', reset: 'color: initial;' },
        [WHITE]: { heading: 'color: white; font-weight: bold;', reset: 'color: initial;' },
        [BLUE]: { heading: 'color: #2196f3; font-weight: bold;', reset: 'color: initial;' }
    };
    
    const _default_tag="_default_";
    const _default_tag_set=["all",_default_tag];
    
    const _env=_getEnv();
    const _default_colors=_getColors();
    const _default_headings={
        FAIL: {
            text: "FAIL ===> ",
            color: RED
        },
        WARN: {
            text: "WARN ===> ",
            color: YELLOW
        },
        PASS: {
            text: "PASS ===> ",
            color: GREEN
        },
        LOGS: {
            text: "LOGS ===> ",
            color: WHITE
        },
        DATA: {
            text: "DATA ===> ",
            color: BLUE
        },
    };
    
    let enabled=[..._default_tag_set];//default tag list to print
    //===> MODULE INIT <========================================================
    Object.assign(_public, {
        print:(message,type="LOGS",tag=_default_tag)=>{
            //check if tag is enabled, print if so, do nothing if not
            for(let t in enabled){
                if(enabled[t] == tag || enabled[t] == "all"){
                    _print(type,message);
                    break;
                }
            }
        },
        enable:(tags)=>{
            //append tag/s to end of enabled 
            if (!Array.isArray(tags)) tags = [tags];
            for(let t in tags){
                let found = false;
                for(let e in enabled){
                    if( tags[t] == enabled[e]){
                        found = true;
                        break;
                    }   
                }
                if(found == false){
                    enabled.push(tags[t]);
                }
            }
        },
        disable:(tags)=>{
            //remove tag/s from enabled
            if (!Array.isArray(tags)) tags = [tags];
            for(let t in tags){
                for(let e in enabled){
                    if( tags[t] == enabled[e]){
                        enabled.splice(e,1);
                        break;
                    }   
                }
            }
        },
        getEnabled:()=>{
            return [...enabled];
        },
        reset:()=>{
            enabled = [..._default_tag_set];
        }
    });

    //===> MODULE FUNCTIONS <===================================================
    function _getEnv(){
        if (
            typeof process !== 'undefined' && 
            process.versions && process.versions.node
        ) {
            return "node";
        } else if (
            typeof window !== 'undefined' && 
            typeof window.document !== 'undefined'
        ) {
            // This is a browser's main thread
            return "browser";
        } else if (
            typeof importScripts !== 'undefined' ||
            typeof WorkerGlobalScope !== 'undefined'
        ) {
            return "worker";
        } else {
            // Other environments (e.g., Deno, Bun, Cloudflare Workers,etc.)
            return "other";
        }        
    }
    
    function _getColors(){
        switch(_env){
            case "node":
                return _default_colors_terminal;
            case "worker":
                return _default_colors_console;
            case "browser":
                return _default_colors_console;
            case "other":
                return _default_colors_terminal;
            default: 
                return _default_colors_terminal;   
        }
    }
    
    function _print(type,message){
        //type specifies which heading to use to style it
        //use appropriate method for environment to print a string
        //with appropriately colored heading prepended to message
        const heading = _default_headings[type] || _default_headings.LOGS;
        const colors = _default_colors[heading.color];
        
        if (_env === "node" || _env === "other") {
            // Terminal color codes
            console.log(`${colors.heading}${heading.text}${colors.reset}${message}`);
        } else {
            // Browser/worker CSS styling
            console.log(`%c${heading.text}%c${message}`, colors.heading, colors.reset);
        }
    }

    //===> MODULE EXPORT <======================================================
    globalThis[_name]=Object.freeze(_public);
    
    // Support Node.js module.exports for CommonJS
    if (typeof module !== 'undefined' && module.exports) {
        module.exports = _public;
    }
})(//### MODULE ARGS ###########################################################
    {
        //custom build args here
    }
);

/*Example Usage
    dbug.disable("all");
    dbug.enable("pets");
    dBug.print("the machine exploded","FAIL","machine");
    dBug.print("the dog farted","WARN","pets")
    //should only print the line about the dog farting as machine tag is a
    //custom tag and is not part of the default set [ "all", "_default_" ]
    //and has not been enabled yet and we disabled all
    dbug.enable("all");
    //should print any tag going forward till all is disabled again
*/