// Premature Optimization - The $800 Tool You Can't Verify
// Complex solutions to simple problems

// "Optimized" string search using custom algorithm
class OptimizedStringMatcher {
    constructor(pattern) {
        this.pattern = pattern;
        this.badCharTable = this.buildBadCharTable(pattern);
        this.goodSuffixTable = this.buildGoodSuffixTable(pattern);
        this.preprocessed = true;
    }

    buildBadCharTable(pattern) {
        const table = new Array(256).fill(-1);
        for (let i = 0; i < pattern.length - 1; i++) {
            table[pattern.charCodeAt(i)] = i;
        }
        return table;
    }

    buildGoodSuffixTable(pattern) {
        const m = pattern.length;
        const table = new Array(m).fill(m);
        const suffix = new Array(m);

        suffix[m - 1] = m;
        let g = m - 1;
        let f = 0;

        for (let i = m - 2; i >= 0; i--) {
            if (i > g && suffix[i + m - 1 - f] < i - g) {
                suffix[i] = suffix[i + m - 1 - f];
            } else {
                if (i < g) g = i;
                f = i;
                while (g >= 0 && pattern[g] === pattern[g + m - 1 - f]) {
                    g--;
                }
                suffix[i] = f - g;
            }
        }

        // More complex logic...
        return table;
    }

    search(text) {
        const results = [];
        const m = this.pattern.length;
        const n = text.length;
        let s = 0;

        while (s <= n - m) {
            let j = m - 1;
            while (j >= 0 && this.pattern[j] === text[s + j]) {
                j--;
            }
            if (j < 0) {
                results.push(s);
                s += (s + m < n) ? m - this.badCharTable[text.charCodeAt(s + m)] : 1;
            } else {
                s += Math.max(
                    this.goodSuffixTable[j],
                    j - this.badCharTable[text.charCodeAt(s + j)]
                );
            }
        }
        return results;
    }
}

// "Optimized" array operations with manual memory management
class OptimizedArrayProcessor {
    constructor(initialSize = 1024) {
        this.buffer = new ArrayBuffer(initialSize * 8);
        this.view = new Float64Array(this.buffer);
        this.length = 0;
        this.capacity = initialSize;
    }

    push(value) {
        if (this.length >= this.capacity) {
            this.resize(this.capacity * 2);
        }
        this.view[this.length++] = value;
    }

    resize(newCapacity) {
        const newBuffer = new ArrayBuffer(newCapacity * 8);
        const newView = new Float64Array(newBuffer);
        newView.set(this.view);
        this.buffer = newBuffer;
        this.view = newView;
        this.capacity = newCapacity;
    }

    sum() {
        // "Optimized" with loop unrolling
        let sum = 0;
        let i = 0;
        const len = this.length;
        const unrollFactor = 4;
        const limit = len - (len % unrollFactor);

        for (; i < limit; i += unrollFactor) {
            sum += this.view[i] + this.view[i + 1] + this.view[i + 2] + this.view[i + 3];
        }

        for (; i < len; i++) {
            sum += this.view[i];
        }

        return sum;
    }

    average() {
        return this.length > 0 ? this.sum() / this.length : 0;
    }
}

// "Optimized" object pool to avoid garbage collection
class ObjectPool {
    constructor(factory, initialSize = 100) {
        this.factory = factory;
        this.available = [];
        this.inUse = new Set();

        for (let i = 0; i < initialSize; i++) {
            this.available.push(factory());
        }
    }

    acquire() {
        let obj;
        if (this.available.length > 0) {
            obj = this.available.pop();
        } else {
            obj = this.factory();
        }
        this.inUse.add(obj);
        return obj;
    }

    release(obj) {
        if (this.inUse.has(obj)) {
            this.inUse.delete(obj);
            this.available.push(obj);
        }
    }
}

// "Optimized" cache with LRU eviction
class OptimizedLRUCache {
    constructor(capacity) {
        this.capacity = capacity;
        this.cache = new Map();
        this.head = { key: null, value: null, prev: null, next: null };
        this.tail = { key: null, value: null, prev: null, next: null };
        this.head.next = this.tail;
        this.tail.prev = this.head;
    }

    get(key) {
        const node = this.cache.get(key);
        if (!node) return null;

        this.moveToHead(node);
        return node.value;
    }

    set(key, value) {
        let node = this.cache.get(key);

        if (node) {
            node.value = value;
            this.moveToHead(node);
        } else {
            node = { key, value, prev: null, next: null };
            this.cache.set(key, node);
            this.addToHead(node);

            if (this.cache.size > this.capacity) {
                const removed = this.removeTail();
                this.cache.delete(removed.key);
            }
        }
    }

    moveToHead(node) {
        this.removeNode(node);
        this.addToHead(node);
    }

    removeNode(node) {
        node.prev.next = node.next;
        node.next.prev = node.prev;
    }

    addToHead(node) {
        node.prev = this.head;
        node.next = this.head.next;
        this.head.next.prev = node;
        this.head.next = node;
    }

    removeTail() {
        const node = this.tail.prev;
        this.removeNode(node);
        return node;
    }
}

// Demonstration of premature optimization
function demonstratePrematureOptimization() {
    console.log("=== PREMATURE OPTIMIZATION ===\n");
    console.log("Complex solutions to simple problems\n");

    // Test 1: String matching
    console.log("Test 1: String Search");
    console.log("Task: Find 'the' in 'the quick brown fox'\n");

    const text = "the quick brown fox jumps over the lazy dog";
    const pattern = "the";

    const matcher = new OptimizedStringMatcher(pattern);
    const start1 = performance.now();
    const results = matcher.search(text);
    const end1 = performance.now();

    console.log("   Optimized (Boyer-Moore): Found at positions", results);
    console.log(`   Time: ${(end1 - start1).toFixed(6)}ms`);
    console.log("   Code complexity: 80+ lines");
    console.log("   Debuggability: 😰 Good luck");
    console.log("   Edge cases handled: 🤷 Maybe?");
    console.log();

    // Test 2: Array operations
    console.log("Test 2: Array Sum/Average");
    console.log("Task: Calculate average of 100 numbers\n");

    const processor = new OptimizedArrayProcessor();
    for (let i = 1; i <= 100; i++) {
        processor.push(i);
    }

    const start2 = performance.now();
    const avg = processor.average();
    const end2 = performance.now();

    console.log(`   Optimized (Manual memory): Average = ${avg}`);
    console.log(`   Time: ${(end2 - start2).toFixed(6)}ms`);
    console.log("   Code complexity: 60+ lines");
    console.log("   Memory management: Manual (error-prone)");
    console.log("   Debuggability: 😰 ArrayBuffer views?");
    console.log();

    // Test 3: Object pooling
    console.log("Test 3: Object Creation");
    console.log("Task: Create 10 user objects\n");

    const pool = new ObjectPool(() => ({ name: '', age: 0 }), 10);

    const start3 = performance.now();
    const users = [];
    for (let i = 0; i < 10; i++) {
        const user = pool.acquire();
        user.name = `User ${i}`;
        user.age = 20 + i;
        users.push(user);
    }
    const end3 = performance.now();

    console.log(`   Optimized (Object pool): Created ${users.length} objects`);
    console.log(`   Time: ${(end3 - start3).toFixed(6)}ms`);
    console.log("   Code complexity: 40+ lines");
    console.log("   Memory leaks: Likely if you forget to release()");
    console.log("   Debuggability: 😰 Why is this object stale?");
    console.log();

    // Test 4: LRU Cache
    console.log("Test 4: Simple Cache");
    console.log("Task: Store and retrieve 5 key-value pairs\n");

    const cache = new OptimizedLRUCache(100);

    const start4 = performance.now();
    for (let i = 0; i < 5; i++) {
        cache.set(`key${i}`, `value${i}`);
    }
    const value = cache.get('key2');
    const end4 = performance.now();

    console.log(`   Optimized (LRU): Retrieved ${value}`);
    console.log(`   Time: ${(end4 - start4).toFixed(6)}ms`);
    console.log("   Code complexity: 70+ lines");
    console.log("   Doubly linked list: Manual pointer management");
    console.log("   Debuggability: 😰 Why is the list corrupted?");
    console.log();
}

// The real problem: complexity without benefit
function demonstrateComplexityWithoutBenefit() {
    console.log("\n" + "=".repeat(60) + "\n");
    console.log("=== THE REAL PROBLEM ===\n");

    console.log("You spent days implementing these optimizations:");
    console.log("  - Boyer-Moore string search: 2 days");
    console.log("  - Manual memory management: 1 day");
    console.log("  - Object pooling: 1 day");
    console.log("  - LRU cache: 2 days");
    console.log("  - Total: 6 days (48 hours)\n");

    console.log("The performance gain on your actual data:");
    console.log("  - Boyer-Moore: 0.001ms faster on 100-char strings");
    console.log("  - ArrayBuffer: 0.002ms faster on 100-item arrays");
    console.log("  - Object pool: 0.001ms faster for 10 objects");
    console.log("  - LRU cache: 0.001ms faster for 5 items");
    console.log("  - Total savings: 0.005ms per operation\n");

    console.log("Your actual usage:");
    console.log("  - Operations per day: ~1,000");
    console.log("  - Time saved per day: 5ms");
    console.log("  - Time saved per year: 1.8 seconds\n");

    console.log("Cost-benefit analysis:");
    console.log("  - Time invested: 48 hours");
    console.log("  - Time saved (yearly): 1.8 seconds");
    console.log("  - Break-even: ~950 years\n");

    console.log("💀 You optimized for problems you don't have");
    console.log("💀 You created complexity you can't debug");
    console.log("💀 You bought the $800 laser for cutting 10 rafters");
}

// Edge case failures
function demonstrateEdgeCaseFailures() {
    console.log("\n" + "=".repeat(60) + "\n");
    console.log("=== EDGE CASE FAILURES ===\n");

    console.log("The optimized code breaks in production:\n");

    // Empty string
    try {
        const matcher = new OptimizedStringMatcher("");
        console.log("✅ Empty pattern handled... or did it?");
    } catch (e) {
        console.log("❌ Empty pattern crashed:", e.message);
    }

    // Unicode
    try {
        const matcher = new OptimizedStringMatcher("café");
        const results = matcher.search("café au lait");
        console.log(`   Results: ${results.length} matches (should be 1)`);
        if (results.length !== 1) {
            console.log("   ⚠️  Unicode handling is broken!");
        }
    } catch (e) {
        console.log("❌ Unicode crashed:", e.message);
    }

    // Negative numbers in array
    const processor = new OptimizedArrayProcessor();
    processor.push(-5);
    processor.push(10);
    const avg = processor.average();
    console.log(`   Average of [-5, 10]: ${avg} (should be 2.5)`);
    if (Math.abs(avg - 2.5) > 0.001) {
        console.log("   ⚠️  Negative number handling is broken!");
    }

    console.log("\n💀 Complex code = More ways to fail");
    console.log("💀 Simple code = Obvious edge cases");
}

// Run demonstrations
if (typeof module === 'undefined' || require.main === module) {
    demonstratePrematureOptimization();
    demonstrateComplexityWithoutBenefit();
    demonstrateEdgeCaseFailures();

    console.log("\n" + "=".repeat(60));
    console.log("\n💡 See good.js for simple solutions that actually work\n");
}

if (typeof module !== 'undefined') {
    module.exports = {
        OptimizedStringMatcher,
        OptimizedArrayProcessor,
        ObjectPool,
        OptimizedLRUCache,
        demonstratePrematureOptimization
    };
}
