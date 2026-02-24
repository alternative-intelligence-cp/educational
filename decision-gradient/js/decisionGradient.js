function DecisionGradient(width=100){
    //##### ARG VALIDATION #############################################################################################
    if(typeof width !== 'number' || width <= 1) {
        console.error(`Invalid width: ${width}, must be number greater than 1!`);
        return null;
    }

    //##### PRIVATE MEMBERS ############################################################################################

    // pbw: probability box half-span (fixed). Normalized: pbw/width = 0.5.
    const pbw = width * 0.5;

    // pbc: probability box center in raw units.
    //   Range: [width*0.25 (max A) ... width*0.75 (max B)], starts at width*0.5 (unknown).
    let pbc = width * 0.5;

    // pbm: probability box width multiplier [0..1].
    //   Drives a quadratic factor: factor = pbm² + 0.5·pbm + 0.5
    //     pbm=0.0 → factor=0.50 (narrow: "none"  — width × 0.50)
    //     pbm=0.5 → factor=1.00 (normal: "unkn"  — width × 1.00)
    //     pbm=1.0 → factor=2.00 (wide:   "both"  — width × 2.00)
    //   Amount scaling for tilt ops: step = amount / pbw  (same feel as lean ops).
    let pbm = 0.5;

    //_______ Private helpers __________________________________________________________________________________________

    // Quadratic factor mapping pbm → box-width multiplier.
    function _factor(m){ return m*m + 0.5*m + 0.5; }

    // Compute [lo, hi] of the probability box in normalized [0..1] space.
    // Box can extend beyond [0,1]; overlap functions clamp naturally.
    function _boxRange(){
        const c = pbc / width;              // normalized center
        const hw = 0.5 * _factor(pbm) / 2; // half-width in normalized space
        return { lo: c - hw, hi: c + hw };
    }

    // Clamp x to [min,max].
    function _clamp(x, lo, hi){ return x < lo ? lo : (x > hi ? hi : x); }

    //##### PUBLIC INTERFACE ###########################################################################################

    // ── Lean (shift pbc along A-Unknown-B axis) ───────────────────────────────

    this.leanOptA = function(amount=1){ // move pbc towards A end (width*0.25)
        if(typeof amount !== 'number' || amount <= 0) {
            console.error(`Invalid amount: ${amount}, must be number greater than 0!`);
            return null;
        }
        pbc = _clamp(pbc - amount, width * 0.25, width * 0.75);
    };

    this.leanUnkn = function(amount=1){ // move pbc towards Unknown center (width*0.5)
        if(typeof amount !== 'number' || amount <= 0) {
            console.error(`Invalid amount: ${amount}, must be number greater than 0!`);
            return null;
        }
        const center = width * 0.5;
        if(pbc > center){
            pbc = Math.max(center, pbc - amount);
        } else if(pbc < center){
            pbc = Math.min(center, pbc + amount);
        }
    };

    this.leanOptB = function(amount=1){ // move pbc towards B end (width*0.75)
        if(typeof amount !== 'number' || amount <= 0) {
            console.error(`Invalid amount: ${amount}, must be number greater than 0!`);
            return null;
        }
        pbc = _clamp(pbc + amount, width * 0.25, width * 0.75);
    };

    // ── Tilt (shift pbm along None-Unknown-Both axis) ─────────────────────────
    // step = amount / pbw so a unit amount feels proportional to lean ops.

    this.tiltBoth = function(amount=1){ // move pbm towards 1.0 (widest: "both")
        if(typeof amount !== 'number' || amount <= 0) {
            console.error(`Invalid amount: ${amount}, must be number greater than 0!`);
            return null;
        }
        pbm = _clamp(pbm + amount / pbw, 0, 1);
    };

    this.tiltUnkn = function(amount=1){ // move pbm towards 0.5 (normal: "unknown")
        if(typeof amount !== 'number' || amount <= 0) {
            console.error(`Invalid amount: ${amount}, must be number greater than 0!`);
            return null;
        }
        const step = amount / pbw;
        if(pbm > 0.5){
            pbm = Math.max(0.5, pbm - step);
        } else if(pbm < 0.5){
            pbm = Math.min(0.5, pbm + step);
        }
    };

    this.tiltNone = function(amount=1){ // move pbm towards 0.0 (narrowest: "none")
        if(typeof amount !== 'number' || amount <= 0) {
            console.error(`Invalid amount: ${amount}, must be number greater than 0!`);
            return null;
        }
        pbm = _clamp(pbm - amount / pbw, 0, 1);
    };

    // ── Readers ──────────────────────────────────────────────────────────────

    this.getOptA = function(){ // confidence in A: overlap of box with [0, 0.5], normalized to [0,1]
        const { lo, hi } = _boxRange();
        const overlap = Math.max(0, Math.min(0.5, hi) - Math.max(0, lo));
        return Math.min(1, overlap / 0.5);
    };

    this.getOptB = function(){ // confidence in B: overlap of box with [0.5, 1.0], normalized to [0,1]
        const { lo, hi } = _boxRange();
        const overlap = Math.max(0, Math.min(1.0, hi) - Math.max(0.5, lo));
        return Math.min(1, overlap / 0.5);
    };

    this.getNB = function(){ // current pbm position [0..1]: 0=NONE, 0.5=UNKNOWN, 1=BOTH
        return pbm;
    };

    this.getState = function(){ // raw state snapshot {a, b, nb}
        return { a: this.getOptA(), b: this.getOptB(), nb: this.getNB() };
    };

    this.getDecision = function(){ // highest-probability token: A | B | BOTH | UNKNOWN | NONE
        const a = this.getOptA();
        const b = this.getOptB();
        if(a > 0.25 && b > 0.25)  return 'BOTH';
        if(a > 0.25)               return 'A';
        if(b > 0.25)               return 'B';
        if(a >= 0.125 || b >= 0.125) return 'UNKNOWN';
        return 'NONE';
    };

    this.printState = function(){
        const s = this.getState();
        console.log(`State --> A: ${s.a.toFixed(4)} , B: ${s.b.toFixed(4)} , NB: ${s.nb.toFixed(4)}`);
    };

    this.printDecision = function(){
        console.log(`Decision --> ${this.getDecision()}`);
    };

    //##### IMPLEMENTATION INFO ########################################################################################
    //  saturate descending for decisions/output: 0.000
    //  saturate ascending for decisions/output: 1.000
    //
    //  if pbc is max A (.25) with pbw .5 and pbm is unkn (1.00x) range is 0.00 .. 0.50 : 0.5/0.5 A, 0/0.5 B
    //  if pbc is max A (.25) with pbw .5 and pbm is both (2.00x), range is -0.25 .. 0.75 : 0.5/0.5 A, 0.25/0.5 B
    //  if pbc is max A (.25) with pbw .5 and pbm is none (0.25x), range is .125 ... .375 : 0.25/0.5 A, 0/0.5 B
    //
    //  if pbc is unknown (.5) with pbw .5 and pbm is unkn (1.00x) range is 0.25 .. 0.75 : 0.25/0.5 A, 0.25/0.5 B
    //  if pbc is unknown (.5) with pbw .5 and pbm is both (2.00x), range is 0.00 .. 1.00 : 0.5/0.5 A, 0.5/0.5 B
    //  if pbc is unknown (.5) with pbw .5 and pbm is none (0.25x), range is .375 ... .625: 0.125/0.5 A, 0.125/0.125 B
    //
    //  if pbc is max B (.75) with pbw .5 and pbm is unkn (1.00x) range is 0.50 .. 1.00 : 0/0.5 A, 0.5/0.5 B
    //  if pbc is max B (.75) with pbw .5 and pbm is both (2.00x), range is 0.25 .. 1.25 : 0.25/0.5 A, 0.5/0.5 B
    //  if pbc is max B (.75) with pbw .5 and pbm is none (0.25x), range is .625 ... .875 : 0/0.5 A, 0.25/0.5 B
    //##################################################################################################################
}
module.exports=DecisionGradient;