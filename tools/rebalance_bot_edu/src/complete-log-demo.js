#!/usr/bin/env node

/**
 * 🎓 Complete Log Management Demo
 * 
 * This demonstrates the full solution to Randy's log truncation problem
 * with both CLI and web interfaces that handle rotation gracefully
 */

const { AdvancedLogManager } = require('./advanced-log-manager');
const WebLogMonitor = require('./web-log-monitor');
const LogTruncationDemo = require('./log-truncation-demo');

class CompleteDemoSuite {
    constructor() {
        this.logManager = null;
        this.webMonitor = null;
        this.demoRunning = false;
    }

    async runCompleteDemo() {
        console.log('🎓 RANDY\'S LOG TRUNCATION PROBLEM - COMPLETE SOLUTION DEMO');
        console.log('==============================================================');
        console.log('');
        console.log('This demonstrates the full solution to the tail -f problem Randy encountered');
        console.log('');

        try {
            // Part 1: Basic problem demonstration
            console.log('📋 PART 1: Understanding the Problem');
            const basicDemo = new LogTruncationDemo();
            await basicDemo.runDemo();

            console.log('\n' + '='.repeat(60) + '\n');

            // Part 2: Advanced solution with web interface
            console.log('📋 PART 2: Complete Solution Implementation');
            await this.demonstrateAdvancedSolution();

        } catch (error) {
            console.error('Demo error:', error);
        }
    }

    async demonstrateAdvancedSolution() {
        console.log('');
        console.log('🛠️ Setting up Advanced Log Management System...');

        // Create log manager with educational settings
        this.logManager = new AdvancedLogManager({
            logDir: './demo-logs',
            maxFileSize: 1024, // 1KB for quick demo
            maxFiles: 3,
            enableSignalHandling: true
        });

        // Listen to educational events
        this.setupEducationalEventLogging();

        // Create multiple loggers for different components (like Randy's bot had)
        console.log('📝 Creating multiple loggers (like Randy\'s trading bot components)...');
        const tradingLogger = this.logManager.createLogger('trading-bot');
        const rebalanceLogger = this.logManager.createLogger('rebalancer');
        const apiLogger = this.logManager.createLogger('api-client');
        const dbLogger = this.logManager.createLogger('database');

        // Start web monitor
        console.log('📱 Starting web-based log monitor...');
        this.webMonitor = new WebLogMonitor(this.logManager, { port: 3001 });
        await this.webMonitor.start();

        console.log('');
        console.log('✅ SYSTEM READY!');
        console.log('');
        console.log('📱 Web Interface: http://localhost:3001');
        console.log('👀 Watch live log updates without tail -f problems!');
        console.log('');

        // Simulate trading bot activity
        console.log('🤖 Simulating Randy\'s trading bot activity...');
        console.log('   (This will generate lots of logs to trigger rotation)');
        console.log('');

        this.demoRunning = true;
        this.simulateTradingActivity(tradingLogger, rebalanceLogger, apiLogger, dbLogger);

        // Let it run for demonstration
        console.log('🕐 Demo running for 30 seconds...');
        console.log('   Open http://localhost:3001 to see rotation-safe log monitoring!');
        console.log('   Press Ctrl+C to stop');
        console.log('');

        // Handle cleanup on exit
        this.setupCleanupHandlers();

        // Wait for demo duration
        await this.sleep(30000);

        await this.cleanup();
        console.log('');
        console.log('🎯 DEMO COMPLETE!');
        this.showFinalEducationalSummary();
    }

    /**
     * 📚 Setup Educational Event Logging
     */
    setupEducationalEventLogging() {
        this.logManager.on('logger-created', (event) => {
            console.log(`📝 Logger created: ${event.name}`);
        });

        this.logManager.on('log-rotated', (event) => {
            console.log(`🔄 Log rotated: ${event.loggerName} (${event.timestamp})`);
            console.log(`   📁 Previous log saved as: ${event.rotatedFile}`);
        });

        this.logManager.on('signal-sent', (event) => {
            console.log(`📡 Signal sent: ${event.signal} (${event.purpose})`);
        });
    }

    /**
     * 🤖 Simulate Trading Bot Activity
     */
    simulateTradingActivity(tradingLogger, rebalanceLogger, apiLogger, dbLogger) {
        const activities = [
            { logger: tradingLogger, message: 'Portfolio rebalance completed successfully' },
            { logger: rebalanceLogger, message: 'BTC allocation adjusted from 45% to 50%' },
            { logger: apiLogger, message: 'Kraken WebSocket connection established' },
            { logger: dbLogger, message: 'State data persisted to database' },
            { logger: tradingLogger, message: 'Stop loss triggered for ETH position' },
            { logger: rebalanceLogger, message: 'Take profit executed for ADA at 30% gain' },
            { logger: apiLogger, message: 'Market data received: BTC $50,234.56' },
            { logger: dbLogger, message: 'Configuration backup created' },
            { logger: tradingLogger, message: 'Trailing stop adjusted for BTC position' },
            { logger: rebalanceLogger, message: 'Portfolio deviation 6% - rebalancing triggered' }
        ];

        const interval = setInterval(() => {
            if (!this.demoRunning) {
                clearInterval(interval);
                return;
            }

            // Pick random activity
            const activity = activities[Math.floor(Math.random() * activities.length)];
            const timestamp = new Date().toISOString();
            activity.logger.log(`${activity.message} (${Math.random().toString(36).substr(2, 9)})`);

            // Occasionally log errors to show different log levels
            if (Math.random() < 0.1) {
                activity.logger.error('Temporary API timeout - retrying connection');
            }

            // Occasionally log warnings
            if (Math.random() < 0.15) {
                activity.logger.warn('High volatility detected - adjusting position sizes');
            }

        }, 200); // Log every 200ms to quickly trigger rotation
    }

    /**
     * 🧹 Setup Cleanup Handlers
     */
    setupCleanupHandlers() {
        process.on('SIGINT', () => {
            console.log('\n🛑 Demo interrupted by user');
            this.cleanup().then(() => process.exit(0));
        });

        process.on('SIGTERM', () => {
            console.log('\n🛑 Demo terminated');
            this.cleanup().then(() => process.exit(0));
        });
    }

    /**
     * 🧹 Cleanup Resources
     */
    async cleanup() {
        this.demoRunning = false;

        if (this.webMonitor) {
            console.log('📱 Stopping web monitor...');
            await this.webMonitor.stop();
        }

        if (this.logManager) {
            console.log('🗂️ Cleaning up log manager...');
            await this.logManager.cleanup();
        }

        console.log('✅ Cleanup complete');
    }

    /**
     * 🎯 Show Final Educational Summary
     */
    showFinalEducationalSummary() {
        console.log('🎓 EDUCATIONAL SUMMARY: Randy\'s Problem Solved!');
        console.log('================================================');
        console.log('');
        console.log('❌ ORIGINAL PROBLEM (What Randy experienced):');
        console.log('   • Trading bot generated continuous logs');
        console.log('   • tail -f used to monitor log activity');
        console.log('   • Simple log truncation broke tail -f monitoring');
        console.log('   • Lost ability to monitor bot during truncation');
        console.log('   • Incomplete logManager.js suggests you started solving this');
        console.log('');
        console.log('✅ COMPLETE SOLUTION IMPLEMENTED:');
        console.log('');
        console.log('🔄 Smart Log Rotation:');
        console.log('   • Files rotated (renamed) instead of truncated');
        console.log('   • History preserved as .1, .2, .3 files');
        console.log('   • No interruption to monitoring tools');
        console.log('');
        console.log('📱 Web-Based Monitor:');
        console.log('   • Real-time log updates via WebSocket');
        console.log('   • Graceful handling of log rotation');
        console.log('   • Multiple log streams in single interface');
        console.log('   • No dependency on external tools like tail');
        console.log('');
        console.log('📡 Signal-Based Communication:');
        console.log('   • SIGUSR1 signals notify external tools of rotation');
        console.log('   • Graceful coordination between components');
        console.log('   • Professional-grade log management');
        console.log('');
        console.log('🎯 EDUCATIONAL VALUE:');
        console.log('   • Real-world problem solving approach');
        console.log('   • File system and process management');
        console.log('   • WebSocket-based real-time interfaces');
        console.log('   • Event-driven architecture patterns');
        console.log('   • Production-quality log management');
        console.log('');
        console.log('💡 RANDY\'S CONTEXT:');
        console.log('   • Your empty logManager.js shows you were working on this');
        console.log('   • The problem was real and impacted your workflow');
        console.log('   • This solution would have solved it elegantly');
        console.log('   • Educational value: teaches production log management');
        console.log('');
        console.log('🏆 RESULT: Crisis-time problem becomes educational gold!');
        console.log('');
    }

    /**
     * ⏱️ Sleep Utility
     */
    sleep(ms) {
        return new Promise(resolve => setTimeout(resolve, ms));
    }
}

// Run the complete demo
if (require.main === module) {
    const demo = new CompleteDemoSuite();
    demo.runCompleteDemo().catch(console.error);
}

module.exports = CompleteDemoSuite;