// The "In The Marks" Approach - Floating Point Financial Disaster

class PaymentProcessor {
    constructor() {
        this.total = 0.0;  // "It's just money, floats are fine"
        this.transactions = [];
    }

    processTransaction(amount) {
        // 2.9% processing fee (like Stripe)
        const fee = amount * 0.029;
        const rounded = Math.round(fee * 100) / 100;  // Round to cents
        this.total += rounded;
        this.transactions.push({ amount, fee: rounded });
        return rounded;
    }

    getTotal() {
        return this.total;
    }

    audit() {
        // Show where error accumulates
        let computedTotal = 0;
        for (const tx of this.transactions) {
            computedTotal += tx.fee;
        }
        return {
            storedTotal: this.total,
            computedTotal: computedTotal,
            match: Math.abs(this.total - computedTotal) < 0.01
        };
    }
}

// Demo function
function runDemo(transactionCount = 10000, transactionAmount = 10.00) {
    console.log("=== THE 'CLOSE ENOUGH' APPROACH ===\n");
    console.log(`Processing ${transactionCount.toLocaleString()} transactions of $${transactionAmount.toFixed(2)} each\n`);

    const processor = new PaymentProcessor();
    const startTime = performance.now();

    for (let i = 0; i < transactionCount; i++) {
        processor.processTransaction(transactionAmount);
    }

    const endTime = performance.now();
    const expectedTotal = transactionCount * transactionAmount * 0.029;
    const actualTotal = processor.getTotal();
    const loss = expectedTotal - actualTotal;

    console.log("Expected total: $" + expectedTotal.toFixed(2));
    console.log("Actual total:   $" + actualTotal.toFixed(2));
    console.log("Money lost:     $" + Math.abs(loss).toFixed(2));
    console.log("Error rate:     " + (Math.abs(loss / expectedTotal) * 100).toFixed(4) + "%");
    console.log("Time:           " + (endTime - startTime).toFixed(2) + "ms");

    const audit = processor.audit();
    console.log("\nAudit Check:");
    console.log("  Stored total:   $" + audit.storedTotal.toFixed(2));
    console.log("  Computed total: $" + audit.computedTotal.toFixed(2));
    console.log("  Match:          " + (audit.match ? "✓" : "✗"));

    // Show first few transactions to demonstrate rounding
    console.log("\nFirst 5 transactions:");
    for (let i = 0; i < Math.min(5, processor.transactions.length); i++) {
        const tx = processor.transactions[i];
        const exact = tx.amount * 0.029;
        console.log(`  $${tx.amount.toFixed(2)} × 0.029 = $${exact.toFixed(4)} → $${tx.fee.toFixed(2)} (lost $${(exact - tx.fee).toFixed(4)})`);
    }

    // Scale demonstration
    console.log("\n=== SCALING DEMONSTRATION ===");
    console.log("At different scales:");
    const scales = [100, 1000, 10000, 100000, 1000000];
    for (const count of scales) {
        const p = new PaymentProcessor();
        for (let i = 0; i < count; i++) {
            p.processTransaction(transactionAmount);
        }
        const exp = count * transactionAmount * 0.029;
        const act = p.getTotal();
        const l = exp - act;
        console.log(`  ${count.toLocaleString().padStart(10)} transactions: Lost $${Math.abs(l).toFixed(2).padStart(8)}`);
    }

    console.log("\n💀 This is the compound error problem.");
    console.log("💀 'Close enough' on each transaction adds up to real money lost.");
    console.log("💀 Like being 1/16\" off on each wall - by wall 16, you're inches off.");
}

// Run if called directly
if (typeof window === 'undefined') {
    runDemo();
}

// Export for browser use
if (typeof module !== 'undefined') {
    module.exports = { PaymentProcessor, runDemo };
}
