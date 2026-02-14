/**
 * SJS Framework - Quantum Types Example
 * 
 * This example shows how to use the quantum module with SJS framework
 * 
 * To run:
 * 1. Create a manifest that loads the quantum module
 * 2. Configure SJS to use this as main
 * 3. Open in browser or run with Node.js
 */

export default async function main(SJS) {
    console.log('═'.repeat(80));
    console.log('SJS Framework - Quantum Types Demo');
    console.log('═'.repeat(80));
    console.log();

    // Get quantum module
    const quantum = SJS.modules.get('quantum');
    if(!quantum) {
        console.error('ERROR: Quantum module not loaded!');
        return false;
    }

    console.log(`✓ Quantum module loaded (v${quantum.version})`);
    console.log();

    // Configure quantum to use SJS pub/sub
    quantum.configure({
        emitEvents: true,
        logOperations: false
    });

    // Subscribe to quantum events (if pubSub available)
    const pubSub = SJS.modules.get('pubSub');
    if(pubSub) {
        pubSub.subscribe('quantum:create', data => {
            console.log(`[Event] Quantum created: Q${data.width} (id: ${data.q_id})`);
        });
        
        pubSub.subscribe('quantum:crystallize', data => {
            console.log(`[Event] Quantum crystallized: ${data.result}`);
        });
    }

    console.log('─'.repeat(80));
    console.log('EXAMPLE 1: Sensor Fusion (GPS + Barometer)');
    console.log('─'.repeat(80));
    
    // Create Q9 for sensor fusion
    const altitude = quantum.create(9, 100, 102, 0);
    console.log(`Initial: A=${altitude.getA()}m (GPS), B=${altitude.getB()}m (Baro), C=${altitude.getC()} (unknown)`);
    
    // Simulate evidence accumulation
    console.log('Reading 1: Closer to barometer...');
    altitude.qif(true, 0, 0, 1);
    console.log(`Confidence: ${altitude.getC()}`);
    
    console.log('Reading 2: Still favoring barometer...');
    altitude.qif(true, 0, 0, 1);
    console.log(`Confidence: ${altitude.getC()}`);
    
    const finalAltitude = altitude.crystallize();
    console.log(`✓ Final altitude: ${finalAltitude}m (crystallized)`);
    console.log();

    console.log('─'.repeat(80));
    console.log('EXAMPLE 2: Quantum Arithmetic');
    console.log('─'.repeat(80));
    
    const uncertain = quantum.create(3, 8, 12, 0);
    console.log(`Initial: A=${uncertain.getA()}, B=${uncertain.getB()}`);
    console.log('Computing: ((value + 3) * 2) / 4');
    
    uncertain.add(3).mul(2).div(4);
    console.log(`Result: A=${uncertain.getA()}, B=${uncertain.getB()}`);
    console.log('Both hypotheses evolved through same operations!');
    console.log();

    console.log('─'.repeat(80));
    console.log('EXAMPLE 3: Q-Functions (Conditional Logic)');
    console.log('─'.repeat(80));
    
    const qfunc = quantum.create(9, -5, 15, 1);
    console.log(`Initial: A=${qfunc.getA()}, B=${qfunc.getB()}`);
    
    console.log('Using qor() - execute if EITHER hypothesis is positive...');
    qfunc.qor(
        v => v > 0,
        q => {
            q.a *= 2;
            q.b *= 2;
            console.log('  → Callback executed: doubled both values');
        }
    );
    console.log(`After qor: A=${qfunc.getA()}, B=${qfunc.getB()}`);
    
    console.log('Using qand() - execute if BOTH hypotheses are positive...');
    qfunc.qand(
        v => v > 0,
        q => {
            console.log('  → Callback would execute here, but A is negative');
        }
    );
    console.log('Callback NOT executed (A is negative)');
    console.log();

    console.log('─'.repeat(80));
    console.log('EXAMPLE 4: Statistics');
    console.log('─'.repeat(80));
    
    const stats = quantum.stats();
    console.log('Quantum Instance Statistics:');
    console.log(`Q3: ${stats.Q3.active} active, ${stats.Q3.pooled} pooled, ${stats.Q3.total} total created`);
    console.log(`Q9: ${stats.Q9.active} active, ${stats.Q9.pooled} pooled, ${stats.Q9.total} total created`);
    console.log();

    // Cleanup
    console.log('Cleaning up instances...');
    altitude.destroy();
    uncertain.destroy();
    qfunc.destroy();
    
    const finalStats = quantum.stats();
    console.log(`After cleanup: Q3 active=${finalStats.Q3.active}, Q9 active=${finalStats.Q9.active}`);
    console.log(`IDs returned to pool for reuse!`);
    console.log();

    console.log('═'.repeat(80));
    console.log('✓ SJS Quantum Types Demo Complete');
    console.log('═'.repeat(80));

    return true;
}
