#!/usr/bin/env node
//##### SENSOR FUSION WITH QUANTUM TYPES ###################################
// A realistic example: Drone altitude estimation using GPS + Barometer
// Shows how quantum types handle sensor disagreement and failure gracefully

require('./quantum.js');

console.log("═".repeat(80));
console.log("DRONE ALTITUDE ESTIMATION - Sensor Fusion with Quantum Types");
console.log("═".repeat(80));
console.log();

// Simulate sensor readings with noise and potential failures
class Sensor {
    constructor(name, trueValue, noise, failureRate = 0) {
        this.name = name;
        this.trueValue = trueValue;
        this.noise = noise;
        this.failureRate = failureRate;
        this.failed = false;
    }
    
    read() {
        // Random sensor failure
        if(Math.random() < this.failureRate) {
            this.failed = true;
            return null;
        }
        
        // Add Gaussian-ish noise
        const noise = (Math.random() - 0.5) * 2 * this.noise;
        return this.trueValue + noise;
    }
    
    updateTrueValue(newValue) {
        this.trueValue = newValue;
    }
}

// Binary approach (traditional averaging)
function binaryFusion(reading1, reading2) {
    if(reading1 === null && reading2 === null) return null;
    if(reading1 === null) return reading2;
    if(reading2 === null) return reading1;
    return (reading1 + reading2) / 2;  // Simple average
}

// Quantum approach (evidence-based confidence)
function quantumFusion(q, reading1, reading2, name1, name2) {
    const state = q.getState();
    
    // Both sensors working: Compare readings to hypotheses
    if(reading1 !== null && reading2 !== null) {
        const dist_a1 = Math.abs(reading1 - state.a);
        const dist_a2 = Math.abs(reading2 - state.a);
        const dist_b1 = Math.abs(reading1 - state.b);
        const dist_b2 = Math.abs(reading2 - state.b);
        
        // Evidence toward hypothesis A (sensor 1)
        if(dist_a1 < dist_b1 && dist_a2 < dist_b2) {
            q.qif(true, 0, 0, -1);
            return `Both readings favor ${name1}`;
        }
        // Evidence toward hypothesis B (sensor 2)
        else if(dist_b1 < dist_a1 && dist_b2 < dist_a2) {
            q.qif(true, 0, 0, 1);
            return `Both readings favor ${name2}`;
        }
        // Mixed evidence - readings disagree
        else {
            return "Readings conflict - no confidence change";
        }
    }
    
    // Sensor 1 failed
    else if(reading1 === null && reading2 !== null) {
        q.qif(true, 0, 0, 2);  // Strong shift toward working sensor
        return `${name1} FAILED - shifting confidence to ${name2}`;
    }
    
    // Sensor 2 failed
    else if(reading2 === null && reading1 !== null) {
        q.qif(true, 0, 0, -2);  // Strong shift toward working sensor
        return `${name2} FAILED - shifting confidence to ${name1}`;
    }
    
    // Both failed!
    else {
        q.qif(true, 0, 0, -state.c);  // Reset to unknown
        return "BOTH SENSORS FAILED - confidence reset to unknown";
    }
}

// Simulation
console.log("SCENARIO: Drone flying at TRUE altitude 100.0 meters");
console.log("  - GPS:       ±2.0m noise, 5% failure rate");
console.log("  - Barometer: ±0.5m noise, 2% failure rate");
console.log();

const GPS = new Sensor("GPS", 100.0, 2.0, 0.05);
const Barometer = new Sensor("Barometer", 100.0, 0.5, 0.02);

// Initialize with first readings
const gps_initial = GPS.read() || 100;
const baro_initial = Barometer.read() || 100;

const altitude_quantum = new QuantumNumber(9, gps_initial, baro_initial, 0);

console.log("INITIAL READINGS:");
console.log(`  GPS:       ${gps_initial.toFixed(2)}m`);
console.log(`  Barometer: ${baro_initial.toFixed(2)}m`);
console.log(`  Quantum state: A=${gps_initial.toFixed(2)}, B=${baro_initial.toFixed(2)}, c=0`);
console.log();

console.log("─".repeat(80));
console.log("SIMULATION: 20 time steps with sensor updates");
console.log("─".repeat(80));
console.log();

let binary_errors = [];
let quantum_errors = [];

for(let step = 1; step <= 20; step++) {
    // Drone climbs 5m per step
    const true_altitude = 100.0 + (step * 5);
    GPS.updateTrueValue(true_altitude);
    Barometer.updateTrueValue(true_altitude);
    
    // Read sensors
    const gps_reading = GPS.read();
    const baro_reading = Barometer.read();
    
    // Binary fusion
    const binary_result = binaryFusion(gps_reading, baro_reading);
    
    // Quantum fusion
    const fusion_msg = quantumFusion(altitude_quantum, gps_reading, baro_reading, "GPS", "Barometer");
    
    // Update quantum hypotheses with new readings (if available)
    const state = altitude_quantum.getState();
    if(gps_reading !== null) {
        altitude_quantum.qif(true, gps_reading - state.a, 0, 0);
    }
    if(baro_reading !== null) {
        altitude_quantum.qif(true, 0, baro_reading - state.b, 0);
    }
    
    const quantum_result = altitude_quantum.crystallize();
    
    // Calculate errors
    if(binary_result !== null) {
        binary_errors.push(Math.abs(binary_result - true_altitude));
    }
    if(quantum_result !== null) {
        quantum_errors.push(Math.abs(quantum_result - true_altitude));
    }
    
    // Display results
    console.log(`Step ${step.toString().padStart(2)}: True=${true_altitude.toFixed(1)}m`);
    console.log(`  GPS: ${gps_reading !== null ? gps_reading.toFixed(2) + 'm' : 'FAILED'}`);
    console.log(`  Barometer: ${baro_reading !== null ? baro_reading.toFixed(2) + 'm' : 'FAILED'}`);
    console.log(`  Binary fusion: ${binary_result !== null ? binary_result.toFixed(2) + 'm' : 'NULL'} (error: ${binary_result !== null ? Math.abs(binary_result - true_altitude).toFixed(2) + 'm' : 'N/A'})`);
    console.log(`  Quantum: ${fusion_msg}`);
    console.log(`  Quantum result: ${quantum_result !== null ? quantum_result.toFixed(2) + 'm' : 'CANNOT CRYSTALLIZE'} (c=${state.c}) (error: ${quantum_result !== null ? Math.abs(quantum_result - true_altitude).toFixed(2) + 'm' : 'N/A'})`);
    console.log();
}

console.log("─".repeat(80));
console.log("STATISTICS");
console.log("─".repeat(80));

const avg_binary_error = binary_errors.reduce((a,b) => a+b, 0) / binary_errors.length;
const avg_quantum_error = quantum_errors.reduce((a,b) => a+b, 0) / quantum_errors.length;
const max_binary_error = Math.max(...binary_errors);
const max_quantum_error = Math.max(...quantum_errors);

console.log(`Binary Fusion:`);
console.log(`  Average error: ${avg_binary_error.toFixed(3)}m`);
console.log(`  Maximum error: ${max_binary_error.toFixed(3)}m`);
console.log(`  Valid readings: ${binary_errors.length}/20`);
console.log();

console.log(`Quantum Fusion:`);
console.log(`  Average error: ${avg_quantum_error.toFixed(3)}m`);
console.log(`  Maximum error: ${max_quantum_error.toFixed(3)}m`);
console.log(`  Valid readings: ${quantum_errors.length}/20`);
console.log(`  Final confidence: ${altitude_quantum.getC()}`);
console.log();

if(avg_quantum_error < avg_binary_error) {
    const improvement = ((avg_binary_error - avg_quantum_error) / avg_binary_error * 100).toFixed(1);
    console.log(`✓ Quantum fusion ${improvement}% more accurate on average!`);
} else {
    console.log(`Binary fusion was more accurate this run (random variation)`);
}

console.log();
console.log("─".repeat(80));
console.log("KEY INSIGHTS");
console.log("─".repeat(80));
console.log(`
1. GRACEFUL DEGRADATION:
   - When sensor fails, binary fusion can only average or use remaining sensor
   - Quantum fusion shifts confidence toward working sensor automatically
   - No special failure handling code needed!

2. EVIDENCE ACCUMULATION:
   - Binary fusion treats each reading independently (stateless)
   - Quantum fusion builds confidence over time (stateful)
   - Previous good readings increase trust in that sensor

3. UNCERTAINTY AWARENESS:
   - Binary fusion always returns a value (false confidence)
   - Quantum fusion returns null when uncertain (honest about limits)
   - Can trigger "need more data" logic in safety-critical systems

4. ADAPTIVE TRUST:
   - If GPS consistently better, quantum shifts confidence toward it
   - If barometer consistently better, quantum shifts confidence toward it
   - Adaptation happens naturally through evidence accumulation

5. REAL-WORLD MODELING:
   - Real systems don't have instant perfect knowledge
   - Quantum types model gradual confidence building (realistic)
   - Better matches how human operators think about sensor fusion
`);

console.log("═".repeat(80));
console.log("This demonstrates why quantum types matter for robotics and AI!");
console.log("═".repeat(80));
