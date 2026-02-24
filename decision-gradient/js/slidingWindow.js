//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ SlidingWindow @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
((args)=>{    
    
    //+++++ CONSTRUCTOR ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    function SlidingWindow(width=100){  
        
        //----- ARG VALIDATION -----------------------------------------------------------------------------------------
        if(typeof width !== 'number' || width <= 1) {
            console.error(`Invalid width: ${width}, must be number greater than 1!`);
            return null;
        }

        //----- PRIVATE MEMBERS ----------------------------------------------------------------------------------------
        
        //_______ Constants ____________________________________________________________________________________________
        
        const MAX = width;

        //_______ Variables ____________________________________________________________________________________________
        
        let a = MAX * .5;
        let b = a;

        //_______ Functions ____________________________________________________________________________________________
        
            //reserved

        //----- PUBLIC INTERFACE ---------------------------------------------------------------------------------------
        
        //_______ Properties ___________________________________________________________________________________________    
        
            //reserved

        //_______ Methods ______________________________________________________________________________________________
        
        this.shiftA=function(amount=1){ //..............................................................................
            if(typeof amount !== 'number' || amount <= 0) {
                console.error(`Invalid amount: ${amount}, must be number greater than 0!`);
                return null;
            }
            if(a + amount > MAX){
                a=MAX;
                b=0;
            } else {
                a += amount;
                b -= amount;
            }
        };

        this.shiftB=function(amount=1){ //..............................................................................
            if(typeof amount !== 'number' || amount <= 0) {
                console.error(`Invalid amount: ${amount}, must be number greater than 0!`);
                return null;
            }
            if(b + amount > MAX){
                b=MAX;
                a=0;
            } else {
                b += amount;
                a -= amount;
            }
        };

        this.getA=function(){ //........................................................................................
            return a/MAX;
        };

        this.getB=function(){ //........................................................................................
            return b/MAX;
        };

        this.getState=function(){ //....................................................................................
            return {a:this.getA(),b:this.getB()};
        };

        this.printState=function(){ //..................................................................................
            let state=this.getState();
            console.log(`State --> A: ${state.a} , B: ${state.b}`);
        };

        this.getBest=function(){ //.....................................................................................
            return ((a == b) ? 0 : ((a > b) ? 1 : 2)); 
        };

        this.getBestSemantic=function(){ //.............................................................................
            let best=this.getBest();
            switch(best){
                case 0:
                    return "UNKNOWN";
                case 1:
                    return "A";
                case 2:
                    return "B";
                default:
                    console.error(`Invalid value returned by getBest: ${best}`);
                    return null;
            }
        };

    }
    
    //++++++ EXPORT ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    module.exports=Object.freeze(SlidingWindow);

})({//################################################ BUILD ARGS ######################################################

    //build args go here, access via args.[nameHere]

});
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ End SlidingWindow @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ 