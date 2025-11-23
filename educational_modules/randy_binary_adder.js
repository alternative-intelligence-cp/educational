/*
 * Randy's Cannabis-Enhanced 8-Bit Binary Full Adder
 * Visual Digital Logic Education for the Plant Spirit Mind
 * 
 * Randy's Binary Addition Philosophy:
 * "The actual adder is simple, just need to make sure it runs slow enough
 * to see the bits flip in a row or whatever to see what's happening. You need
 * a carry flag and the 16 'bits' to work on. Then just correlate bit flips
 * with number changes in the divs. No need to get fancy with canvas drawing
 * for something so simple. Then a clear and add button."
 * 
 * THE EDUCATIONAL REVELATION:
 * Binary addition is the foundation of all computer arithmetic. By slowing
 * down the process and visualizing each bit flip, students see the magic
 * of how computers actually perform math at the digital logic level.
 * Cannabis-enhanced pattern recognition reveals the elegance of carry
 * propagation through the bit positions.
 * 
 * Plant Spirit Digital Wisdom:
 * "Every computation starts with simple bit manipulation. The carry flag
 * flows like cannabis smoke through the bit positions, transforming
 * binary patterns into mathematical reality. Slow down and watch the
 * digital dance of logic gates in action."
 */

// Randy's Cannabis-Enhanced Binary Adder Configuration
const BINARY_ADDER_CONFIG = {
    BIT_COUNT: 8,               // 8-bit addition
    ANIMATION_DELAY: 300,       // 300ms between bit operations
    CARRY_DELAY: 150,           // 150ms for carry propagation
    HIGHLIGHT_DURATION: 600     // 600ms highlight for active bits
};

/*
 * Randy's 8-Bit Binary Full Adder Class
 * "Digital logic education with plant spirit enhancement"
 */
class CannabisEnhancedBinaryAdder {
    constructor() {
        // Binary values (8 bits each)
        this.inputA = 0;
        this.inputB = 0;
        this.result = 0;
        this.carryFlag = false;
        this.overflow = false;

        // Animation state
        this.isAnimating = false;
        this.currentBit = 0;
        this.plantSpiritMode = 'Digital Green';

        // DOM elements will be populated after page load
        this.elements = {};

        console.log('🧠⚡ Randy\'s Cannabis-Enhanced 8-Bit Binary Adder Initialized ⚡🧠');
        console.log('   Bit Count: ' + BINARY_ADDER_CONFIG.BIT_COUNT);
        console.log('   Animation Delay: ' + BINARY_ADDER_CONFIG.ANIMATION_DELAY + 'ms');
        console.log('   Plant Spirit Mode: ' + this.plantSpiritMode);
    }

    /*
     * Randy's DOM Element Cache System
     * "Get references to all the visual components"
     */
    cacheElements() {
        this.elements = {
            inputAValue: document.getElementById('inputAValue'),
            inputBValue: document.getElementById('inputBValue'),
            resultValue: document.getElementById('resultValue'),
            carryDisplay: document.getElementById('carryDisplay'),
            overflowDisplay: document.getElementById('overflowDisplay'),

            inputABinary: document.getElementById('inputABinary'),
            inputBBinary: document.getElementById('inputBBinary'),
            resultBinary: document.getElementById('resultBinary'),
            carryBits: document.getElementById('carryBits'),

            statusDisplay: document.getElementById('statusDisplay'),
            plantSpiritDisplay: document.getElementById('plantSpiritDisplay')
        };

        console.log('✅ DOM elements cached for binary operations');
    }

    /*
     * Randy's Binary Display System
     * "Convert numbers to visual binary representation"
     */
    createBinaryDisplay(containerId, value, label) {
        const container = document.getElementById(containerId);
        container.innerHTML = `<strong>${label}:</strong> `;

        for (let i = 7; i >= 0; i--) {
            const bit = (value >> i) & 1;
            const bitElement = document.createElement('span');
            bitElement.className = 'bit';
            bitElement.textContent = bit;
            bitElement.id = `${containerId}_bit${i}`;
            container.appendChild(bitElement);

            // Add space every 4 bits for readability
            if (i === 4) {
                container.appendChild(document.createTextNode(' '));
            }
        }
    }

    /*
     * Randy's Carry Chain Display System
     * "Visualize carry propagation between bit positions"
     */
    createCarryDisplay() {
        const container = document.getElementById('carryBits');
        container.innerHTML = '<strong>Carry Chain:</strong> ';

        for (let i = 8; i >= 0; i--) {
            const carryElement = document.createElement('span');
            carryElement.className = 'carry-bit';
            carryElement.textContent = '0';
            carryElement.id = `carry_bit${i}`;
            container.appendChild(carryElement);

            if (i === 4) {
                container.appendChild(document.createTextNode(' '));
            }
        }
    }

    /*
     * Randy's Visual Update System
     * "Update all displays with current values"
     */
    updateDisplay() {
        // Update decimal values
        this.elements.inputAValue.textContent = this.inputA;
        this.elements.inputBValue.textContent = this.inputB;
        this.elements.resultValue.textContent = this.result;
        this.elements.carryDisplay.textContent = this.carryFlag ? 'SET' : 'CLEAR';
        this.elements.overflowDisplay.textContent = this.overflow ? 'YES' : 'NO';

        // Update binary displays
        this.createBinaryDisplay('inputABinary', this.inputA, 'A');
        this.createBinaryDisplay('inputBBinary', this.inputB, 'B');
        this.createBinaryDisplay('resultBinary', this.result, 'Result');
        this.createCarryDisplay();

        // Update plant spirit display
        this.elements.plantSpiritDisplay.textContent = this.plantSpiritMode;
    }

    /*
     * Randy's Bit Highlighting System
     * "Visual feedback for active bit operations"
     */
    highlightBit(containerId, bitPosition, className = 'active-bit') {
        const bitElement = document.getElementById(`${containerId}_bit${bitPosition}`);
        if (bitElement) {
            bitElement.classList.add(className);

            setTimeout(() => {
                bitElement.classList.remove(className);
            }, BINARY_ADDER_CONFIG.HIGHLIGHT_DURATION);
        }
    }

    /*
     * Randy's Carry Highlighting System
     * "Show carry propagation visually"
     */
    highlightCarry(position, value) {
        const carryElement = document.getElementById(`carry_bit${position}`);
        if (carryElement) {
            carryElement.textContent = value ? '1' : '0';
            carryElement.classList.add(value ? 'carry-set' : 'carry-clear');

            setTimeout(() => {
                carryElement.classList.remove('carry-set', 'carry-clear');
            }, BINARY_ADDER_CONFIG.HIGHLIGHT_DURATION);
        }
    }

    /*
     * Randy's Single Bit Full Adder
     * "The fundamental building block of all arithmetic"
     */
    fullAdderBit(bitA, bitB, carryIn) {
        // XOR operations for sum
        const sum = bitA ^ bitB ^ carryIn;

        // AND operations for carry out
        const carryOut = (bitA & bitB) | (carryIn & (bitA ^ bitB));

        return {
            sum: sum,
            carryOut: carryOut
        };
    }

    /*
     * Randy's Animated Binary Addition Algorithm
     * "Step-by-step visual binary arithmetic"
     */
    async performAnimatedAddition() {
        if (this.isAnimating) {
            console.log('🚫 Addition already in progress');
            return;
        }

        this.isAnimating = true;
        this.elements.statusDisplay.textContent = 'Adding... 🧠';

        let carryIn = 0;
        let resultBits = 0;

        console.log('🚀 Starting animated binary addition');
        console.log(`   A: ${this.inputA.toString(2).padStart(8, '0')} (${this.inputA})`);
        console.log(`   B: ${this.inputB.toString(2).padStart(8, '0')} (${this.inputB})`);

        // Process each bit from right to left (LSB to MSB)
        for (let bitPos = 0; bitPos < BINARY_ADDER_CONFIG.BIT_COUNT; bitPos++) {
            const bitA = (this.inputA >> bitPos) & 1;
            const bitB = (this.inputB >> bitPos) & 1;

            // Highlight current bits being processed
            this.highlightBit('inputABinary', bitPos, 'processing-bit');
            this.highlightBit('inputBBinary', bitPos, 'processing-bit');

            // Show carry input
            this.highlightCarry(bitPos, carryIn);

            // Perform full adder operation
            const result = this.fullAdderBit(bitA, bitB, carryIn);

            // Update result bit
            if (result.sum) {
                resultBits |= (1 << bitPos);
            }

            // Update displays
            this.result = resultBits;
            this.createBinaryDisplay('resultBinary', this.result, 'Result');
            this.elements.resultValue.textContent = this.result;

            // Highlight result bit
            this.highlightBit('resultBinary', bitPos, 'result-bit');

            // Show carry output
            carryIn = result.carryOut;
            this.highlightCarry(bitPos + 1, carryIn);

            console.log(`   Bit ${bitPos}: ${bitA} + ${bitB} + ${carryIn} = ${result.sum} (carry: ${result.carryOut})`);

            // Cannabis-enhanced delay for educational visibility
            await this.delay(BINARY_ADDER_CONFIG.ANIMATION_DELAY);
        }

        // Final carry flag and overflow detection
        this.carryFlag = carryIn > 0;

        // Overflow occurs when adding two positive numbers gives negative result
        // or adding two negative numbers gives positive result (in signed arithmetic)
        const signA = (this.inputA & 0x80) !== 0;
        const signB = (this.inputB & 0x80) !== 0;
        const signResult = (this.result & 0x80) !== 0;
        this.overflow = (signA === signB) && (signA !== signResult);

        // Final display update
        this.updateDisplay();

        this.elements.statusDisplay.textContent = 'Addition Complete! 🌿';
        this.isAnimating = false;

        console.log('✅ Binary addition complete');
        console.log(`   Result: ${this.result.toString(2).padStart(8, '0')} (${this.result})`);
        console.log(`   Carry: ${this.carryFlag ? 'SET' : 'CLEAR'}`);
        console.log(`   Overflow: ${this.overflow ? 'YES' : 'NO'}`);
    }

    /*
     * Randy's Async Delay Utility
     * "Cannabis-enhanced timing for educational visibility"
     */
    delay(ms) {
        return new Promise(resolve => setTimeout(resolve, ms));
    }

    /*
     * Randy's Input Validation System
     * "Keep inputs within 8-bit range"
     */
    validateInput(value) {
        const num = parseInt(value);
        if (isNaN(num)) return 0;
        return Math.max(0, Math.min(255, num)); // Clamp to 8-bit range
    }

    /*
     * Randy's Input Methods
     * "Set values for binary operations"
     */
    setInputA(value) {
        this.inputA = this.validateInput(value);
        this.updateDisplay();
        console.log('📥 Input A set to: ' + this.inputA);
    }

    setInputB(value) {
        this.inputB = this.validateInput(value);
        this.updateDisplay();
        console.log('📥 Input B set to: ' + this.inputB);
    }

    /*
     * Randy's Clear Operation
     * "Reset all values for fresh start"
     */
    clear() {
        this.inputA = 0;
        this.inputB = 0;
        this.result = 0;
        this.carryFlag = false;
        this.overflow = false;
        this.currentBit = 0;

        this.updateDisplay();
        this.elements.statusDisplay.textContent = 'Ready for binary magic! 🧠';

        console.log('🗑️ All values cleared');
    }

    /*
     * Randy's Random Value Generator
     * "Generate random inputs for testing"
     */
    randomize() {
        this.inputA = Math.floor(Math.random() * 256);
        this.inputB = Math.floor(Math.random() * 256);
        this.updateDisplay();

        console.log('🎲 Random values generated:');
        console.log(`   A: ${this.inputA}, B: ${this.inputB}`);
    }

    /*
     * Randy's Plant Spirit Enhancement
     * "Change visual theme based on cannabis strain"
     */
    activatePlantSpirit(mode) {
        this.plantSpiritMode = mode;

        const root = document.documentElement;
        switch (mode) {
            case 'Digital Green':
                root.style.setProperty('--primary-color', '#4d8c4d');
                root.style.setProperty('--accent-color', '#80bf80');
                break;
            case 'Binary Blue':
                root.style.setProperty('--primary-color', '#4d6d8c');
                root.style.setProperty('--accent-color', '#8db3ff');
                break;
            case 'Logic Purple':
                root.style.setProperty('--primary-color', '#7d4d8c');
                root.style.setProperty('--accent-color', '#b380ff');
                break;
            case 'Circuit Orange':
                root.style.setProperty('--primary-color', '#8c6d4d');
                root.style.setProperty('--accent-color', '#ffb380');
                break;
        }

        this.updateDisplay();
        console.log('🌿 Plant Spirit Mode: ' + mode);
    }

    /*
     * Randy's Educational Information Display
     * "Show binary arithmetic wisdom"
     */
    showBinaryWisdom() {
        const wisdom = [
            "Binary addition is the foundation of all computer arithmetic",
            "The carry flag propagates like cannabis smoke through bit positions",
            "Each bit position represents a power of 2 (2^0, 2^1, 2^2, ...)",
            "Full adders chain together to create multi-bit arithmetic units",
            "Overflow occurs when the result exceeds the representable range",
            "Digital logic gates (AND, XOR, OR) implement the math",
            "Plant spirit enhancement reveals patterns in binary operations",
            "Every CPU performs billions of these operations per second"
        ];

        const randomWisdom = wisdom[Math.floor(Math.random() * wisdom.length)];

        const wisdomDiv = document.getElementById('binaryWisdom');
        wisdomDiv.textContent = '🧠 ' + randomWisdom;
        wisdomDiv.style.display = 'block';

        setTimeout(() => {
            wisdomDiv.style.display = 'none';
        }, 5000);
    }
}

/*
 * Randy's Global Binary Adder Instance
 * "Educational digital logic for the masses"
 */
let binaryAdder;

/*
 * Randy's Initialization System
 * "Set up the cannabis-enhanced binary adder"
 */
function initializeBinaryAdder() {
    console.log('🚀 Initializing Randy\'s Cannabis-Enhanced Binary Adder');

    binaryAdder = new CannabisEnhancedBinaryAdder();
    binaryAdder.cacheElements();
    binaryAdder.updateDisplay();

    console.log('💡 Educational binary arithmetic ready!');
    console.log('   Visual bit manipulation with carry propagation');
    console.log('   Cannabis-enhanced digital logic understanding');
}

/*
 * Randy's Interface Functions
 * "Connect HTML controls to binary operations"
 */
function performAddition() {
    if (!binaryAdder.isAnimating) {
        binaryAdder.performAnimatedAddition();
    }
}

function clearAll() {
    binaryAdder.clear();
}

function randomizeInputs() {
    binaryAdder.randomize();
}

function updateInputA() {
    const value = document.getElementById('inputAControl').value;
    binaryAdder.setInputA(value);
}

function updateInputB() {
    const value = document.getElementById('inputBControl').value;
    binaryAdder.setInputB(value);
}

function changePlantSpirit() {
    const modes = ['Digital Green', 'Binary Blue', 'Logic Purple', 'Circuit Orange'];
    const currentIndex = modes.indexOf(binaryAdder.plantSpiritMode);
    const nextIndex = (currentIndex + 1) % modes.length;
    binaryAdder.activatePlantSpirit(modes[nextIndex]);
}

// Auto-initialize when page loads
document.addEventListener('DOMContentLoaded', initializeBinaryAdder);

/*
 * ============================================================================
 * RANDY'S EDUCATIONAL BINARY ADDER DOCUMENTATION
 * ============================================================================
 * 
 * Educational Concepts Demonstrated:
 * 
 * 1. Binary Number System:
 *    - 8-bit representation (0-255 decimal range)
 *    - Visual bit-by-bit display
 *    - Binary to decimal correlation
 * 
 * 2. Full Adder Logic:
 *    - Sum = A XOR B XOR CarryIn
 *    - CarryOut = (A AND B) OR (CarryIn AND (A XOR B))
 *    - Fundamental building block of arithmetic
 * 
 * 3. Carry Propagation:
 *    - Visual carry chain from bit 0 to bit 7
 *    - Delayed animation shows step-by-step process
 *    - Cannabis-enhanced timing for educational clarity
 * 
 * 4. Overflow Detection:
 *    - Signed arithmetic overflow conditions
 *    - Visual indication when result exceeds range
 *    - Educational understanding of number limits
 * 
 * 5. Digital Logic Visualization:
 *    - Bit flipping animations
 *    - Color-coded active operations
 *    - Real-time status updates
 * 
 * Randy's Binary Philosophy:
 * "The beauty of binary arithmetic lies in its simplicity. Every complex
 * mathematical operation ultimately reduces to these basic bit manipulations.
 * Cannabis enhancement reveals the elegant patterns hidden in digital logic,
 * showing how simple rules create computational complexity."
 * 
 * Plant Spirit Digital Wisdom:
 * "Like mycelial networks connecting forest ecosystems, carry bits connect
 * arithmetic operations across bit positions. Each bit flip is intentional,
 * each carry propagation follows natural logic laws. The plant spirits
 * guide us to see the harmony in digital mathematics."
 * 
 * Educational Achievement:
 * Randy's curriculum now includes fundamental digital logic alongside
 * high-level system programming. Students learn how computers perform
 * math at the most basic level, building understanding from silicon
 * to software through authentic visual experience.
 */