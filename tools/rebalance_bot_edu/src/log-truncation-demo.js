#!/usr/bin/env node

/**
 * 🎓 Log Truncation Problem Demo
 * 
 * This demonstrates the "tail -f" problem Randy encountered and shows the solution
 */

const fs = require('fs');
const path = require('path');
const { spawn } = require('child_process');
const { AdvancedLogManager } = require('./advanced-log-manager');

class LogTruncationDemo {
    constructor() {
        this.demoDir = path.join(__dirname, '../demo-logs');
        this.ensureDemoDir();
    }

    async runDemo() {
        console.log('🎓 LOG TRUNCATION PROBLEM DEMONSTRATION');
        console.log('=====================================');
        console.log('');
        console.log('This demo shows the problem Randy encountered with tail -f and log truncation');
        console.log('');

        // Demo 1: The Problem
        console.log('📋 PART 1: The Problem - Simple Truncation');
        console.log('');
        await this.demonstrateProblem();

        console.log('');
        console.log('📋 PART 2: The Solution - Smart Rotation');
        console.log('');
        await this.demonstrateSolution();

        console.log('');
        console.log('🎯 SUMMARY: Educational Insights');
        this.showEducationalInsights();
    }

    /**
     * 🚨 Demonstrate the Original Problem
     */
    async demonstrateProblem() {
        const problemLogFile = path.join(this.demoDir, 'problem-demo.log');

        console.log('Creating a log file with continuous writes...');

        // Create initial log content
        let logContent = '';
        for (let i = 1; i <= 10; i++) {
            logContent += `${new Date().toISOString()} [INFO] Trading bot message ${i}\n`;
        }
        fs.writeFileSync(problemLogFile, logContent);

        console.log(`📁 Created: ${problemLogFile}`);
        console.log('📊 Initial size:', this.getFileSize(problemLogFile));
        console.log('');

        // Add more content
        console.log('Adding more log entries...');
        for (let i = 11; i <= 20; i++) {
            const newEntry = `${new Date().toISOString()} [INFO] Trading bot message ${i}\n`;
            fs.appendFileSync(problemLogFile, newEntry);
        }

        console.log('📊 Size after additions:', this.getFileSize(problemLogFile));
        console.log('');

        // Now demonstrate the problem - truncation
        console.log('🚨 PROBLEM: Truncating log file (this breaks tail -f)');
        console.log('');
        console.log('If you were running: tail -f problem-demo.log');
        console.log('And we truncate the file...');

        // Truncate the file (this is what Randy's original code was doing)
        fs.writeFileSync(problemLogFile, ''); // This breaks tail -f!

        console.log('❌ File truncated to 0 bytes');
        console.log('❌ tail -f is now confused - it\'s still watching the old file descriptor');
        console.log('❌ New log entries won\'t show up in tail output until you restart it');
        console.log('');

        // Add new content that tail -f won't see
        const newContent = `${new Date().toISOString()} [INFO] This message won't show in tail -f\n`;
        fs.writeFileSync(problemLogFile, newContent);

        console.log('📝 Added new content, but tail -f won\'t see it!');
        console.log('');
    }

    /**
     * ✅ Demonstrate the Solution
     */
    async demonstrateSolution() {
        console.log('🛠️ SOLUTION: Using Advanced Log Manager with Smart Rotation');
        console.log('');

        const logManager = new AdvancedLogManager({
            logDir: this.demoDir,
            maxFileSize: 500, // Small size for demo
            maxFiles: 3
        });

        // Create logger
        const logger = logManager.createLogger('solution-demo');

        console.log('📝 Writing log entries...');

        // Write initial entries
        for (let i = 1; i <= 5; i++) {
            logger.log(`Trading bot successfully processed transaction ${i}`);
            await this.sleep(100); // Small delay for demo
        }

        console.log('📊 Current log file stats:');
        const stats = logManager.getLogStatistics();
        console.log('   File:', stats['solution-demo'].filePath);
        console.log('   Size:', stats['solution-demo'].sizeHuman);
        console.log('');

        // Write more entries to trigger rotation
        console.log('📝 Writing more entries to trigger rotation...');
        for (let i = 6; i <= 15; i++) {
            logger.log(`Trading bot processed rebalance operation ${i} - this is a longer message to increase file size for demonstration purposes`);
            await this.sleep(50);
        }

        console.log('');
        console.log('✅ SOLUTION BENEFITS:');
        console.log('   🔄 Files are rotated, not truncated');
        console.log('   📁 Old logs preserved as .1, .2, .3 etc');
        console.log('   📡 Signals sent to notify monitoring tools');
        console.log('   👀 tail -f continues to work properly');
        console.log('   🧹 Automatic cleanup of old log files');
        console.log('');

        // Show rotation results
        this.showRotationResults();

        await logManager.cleanup();
    }

    /**
     * 📁 Show Rotation Results
     */
    showRotationResults() {
        console.log('📁 LOG ROTATION RESULTS:');
        const files = fs.readdirSync(this.demoDir)
            .filter(f => f.startsWith('solution-demo'))
            .sort();

        files.forEach(file => {
            const filePath = path.join(this.demoDir, file);
            const size = this.getFileSize(filePath);
            console.log(`   📄 ${file}: ${size}`);
        });
        console.log('');
    }

    /**
     * 🎯 Show Educational Insights
     */
    showEducationalInsights() {
        console.log('🎓 EDUCATIONAL INSIGHTS:');
        console.log('');
        console.log('❌ THE PROBLEM (What Randy experienced):');
        console.log('   • Log files get too big and need management');
        console.log('   • Simple truncation breaks tail -f monitoring');
        console.log('   • File descriptor confusion when file is recreated');
        console.log('   • Loss of monitoring capability during truncation');
        console.log('');
        console.log('✅ THE SOLUTION (Advanced Log Management):');
        console.log('   • File rotation instead of truncation');
        console.log('   • Preserved log history (.1, .2, .3 files)');
        console.log('   • Signal-based notification system');
        console.log('   • Graceful handling of monitoring tools');
        console.log('   • Size-based and time-based rotation options');
        console.log('');
        console.log('🛠️ IMPLEMENTATION TECHNIQUES:');
        console.log('   • Event-driven architecture for log management');
        console.log('   • Process signal handling (SIGUSR1, SIGHUP)');
        console.log('   • File system monitoring and statistics');
        console.log('   • Stream management and cleanup');
        console.log('');
        console.log('🏢 REAL-WORLD APPLICATIONS:');
        console.log('   • Production server log management');
        console.log('   • Trading bot operational monitoring');
        console.log('   • Any long-running process with logging needs');
        console.log('   • Systems that need 24/7 monitoring capability');
        console.log('');
        console.log('🎯 RANDY\'S ORIGINAL CONTEXT:');
        console.log('   • Trading bot ran continuously');
        console.log('   • Needed to monitor logs with tail -f');
        console.log('   • Log files grew large quickly');
        console.log('   • Simple truncation broke monitoring workflow');
        console.log('   • This solution would have solved the problem elegantly');
        console.log('');
    }

    /**
     * 🗂️ Ensure Demo Directory Exists
     */
    ensureDemoDir() {
        if (!fs.existsSync(this.demoDir)) {
            fs.mkdirSync(this.demoDir, { recursive: true });
        }
    }

    /**
     * 📏 Get Human-Readable File Size
     */
    getFileSize(filePath) {
        try {
            const stat = fs.statSync(filePath);
            return `${stat.size} bytes`;
        } catch (error) {
            return 'File not found';
        }
    }

    /**
     * ⏱️ Sleep Utility for Demo
     */
    sleep(ms) {
        return new Promise(resolve => setTimeout(resolve, ms));
    }
}

// Run the demo
if (require.main === module) {
    const demo = new LogTruncationDemo();
    demo.runDemo().catch(console.error);
}

module.exports = LogTruncationDemo;