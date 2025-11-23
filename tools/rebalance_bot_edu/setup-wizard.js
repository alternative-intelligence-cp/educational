#!/usr/bin/env node

/**
 * 🧙‍♂️ Interactive Configuration Wizard for Crypto Trading Bot
 * 
 * This solves the "Uncle Mike Problem" - makes the bot usable by anyone!
 * Guided setup with validation, explanations, and safety checks.
 */

const readline = require('readline');
const fs = require('fs').promises;
const path = require('path');
const chalk = require('chalk'); // We'll add this for colors

// Educational bot configuration wizard
class TradingBotWizard {
    constructor() {
        this.rl = readline.createInterface({
            input: process.stdin,
            output: process.stdout
        });

        this.config = {
            version: "2.0.0",
            mode: "paper", // Always start safe!
            created: new Date().toISOString(),
            rebalancing: {},
            portfolio: {},
            risk: {},
            api: {},
            notifications: {}
        };

        // Supported cryptocurrencies with educational info
        this.supportedTokens = {
            'BTC': {
                name: 'Bitcoin',
                description: 'Digital gold - store of value',
                riskLevel: 'Medium',
                minAllocation: 10,
                maxAllocation: 60
            },
            'ETH': {
                name: 'Ethereum',
                description: 'Smart contract platform',
                riskLevel: 'Medium-High',
                minAllocation: 5,
                maxAllocation: 40
            },
            'ADA': {
                name: 'Cardano',
                description: 'Research-driven blockchain',
                riskLevel: 'High',
                minAllocation: 0,
                maxAllocation: 20
            },
            'DOT': {
                name: 'Polkadot',
                description: 'Multi-chain protocol',
                riskLevel: 'High',
                minAllocation: 0,
                maxAllocation: 20
            },
            'LINK': {
                name: 'Chainlink',
                description: 'Oracle network',
                riskLevel: 'High',
                minAllocation: 0,
                maxAllocation: 15
            },
            'AVAX': {
                name: 'Avalanche',
                description: 'High-throughput blockchain',
                riskLevel: 'High',
                minAllocation: 0,
                maxAllocation: 15
            }
        };

        this.riskProfiles = {
            'conservative': {
                name: 'Conservative',
                description: 'Lower risk, slower growth, more stability',
                maxDeviationPercent: 3,
                rebalanceIntervalMinutes: 60,
                stopLossPercent: 15,
                trailingStopPercent: 10,    // Conservative trailing stop
                takeProfitPercent: 20,      // Take profits at 20% gain
                cashReservePercent: 40,
                recommendedTokens: ['BTC', 'ETH']
            },
            'moderate': {
                name: 'Moderate',
                description: 'Balanced risk/reward, good for most people',
                maxDeviationPercent: 5,
                rebalanceIntervalMinutes: 30,
                stopLossPercent: 20,
                trailingStopPercent: 15,    // Moderate trailing stop
                takeProfitPercent: 30,      // Take profits at 30% gain
                cashReservePercent: 25,
                recommendedTokens: ['BTC', 'ETH', 'ADA']
            },
            'aggressive': {
                name: 'Aggressive',
                description: 'Higher risk, potential for higher returns',
                maxDeviationPercent: 8,
                rebalanceIntervalMinutes: 15,
                stopLossPercent: 25,
                trailingStopPercent: 20,    // Aggressive trailing stop
                takeProfitPercent: 50,      // Let winners run to 50%
                cashReservePercent: 15,
                recommendedTokens: ['BTC', 'ETH', 'ADA', 'DOT', 'LINK']
            }
        };
    }

    // Main wizard flow
    async run() {
        console.log('');
        console.log('🤖 ===== CRYPTO TRADING BOT SETUP WIZARD =====');
        console.log('');
        console.log('Welcome! This wizard will help you configure your trading bot safely.');
        console.log('We\'ll start in PAPER TRADING mode - no real money at risk!');
        console.log('');

        try {
            // Step 1: Choose risk profile
            await this.chooseRiskProfile();

            // Step 2: Select cryptocurrencies  
            await this.selectPortfolio();

            // Step 3: Configure rebalancing
            await this.configureRebalancing();

            // Step 4: Set up notifications (optional)
            await this.configureNotifications();

            // Step 5: Review and save
            await this.reviewAndSave();

            console.log('');
            console.log('🎉 Configuration complete!');
            console.log('');
            console.log('Next steps:');
            console.log('1. Start the bot: npm run start:paper');
            console.log('2. Open dashboard: http://localhost:3000');
            console.log('3. Learn how it works before using real money!');
            console.log('');

        } catch (error) {
            console.error('Configuration failed:', error.message);
        } finally {
            this.rl.close();
        }
    }

    // Step 1: Choose risk tolerance
    async chooseRiskProfile() {
        console.log('📊 STEP 1: Choose Your Risk Profile');
        console.log('');

        // Educational section about risk management
        console.log('🧠 EDUCATIONAL: Understanding Risk Management');
        console.log('');
        console.log('📚 Risk Management Tools Explained:');
        console.log('');
        console.log('🛡️  Stop Loss:');
        console.log('   Automatically sells if price drops X% from your buy price');
        console.log('   Protects you from major losses');
        console.log('');
        console.log('📈 Trailing Stop:');
        console.log('   Follows price up but triggers if it drops X% from the PEAK');
        console.log('   Lets you capture gains while protecting profits');
        console.log('   Example: Buy at $100, peaks at $150, trails at 10% = sells if drops to $135');
        console.log('');
        console.log('💰 Take Profit:');
        console.log('   Automatically sells when you hit X% gain');
        console.log('   Locks in profits and prevents giving back gains');
        console.log('   Example: Buy at $100, sell when it hits $130 (30% gain)');
        console.log('');
        console.log('🎯 Why These Matter:');
        console.log('   • Remove emotion from trading decisions');
        console.log('   • Automate profit-taking and loss-prevention');
        console.log('   • Essential for sleeping well with crypto investments!');
        console.log('');
        console.log('='.repeat(60));
        console.log('');

        // Display risk profile options
        Object.entries(this.riskProfiles).forEach(([key, profile], index) => {
            console.log(`${index + 1}. ${profile.name}`);
            console.log(`   ${profile.description}`);
            console.log(`   ⏰ Rebalance: ${profile.rebalanceIntervalMinutes}min`);
            console.log(`   🛡️  Stop Loss: ${profile.stopLossPercent}%`);
            console.log(`   📈 Trailing Stop: ${profile.trailingStopPercent}%`);
            console.log(`   💰 Take Profit: ${profile.takeProfitPercent}%`);
            console.log(`   💵 Cash Reserve: ${profile.cashReservePercent}%`);
            console.log('');
        });

        const choice = await this.askQuestion('Choose your risk profile (1-3): ');
        const riskKeys = Object.keys(this.riskProfiles);
        const selectedRisk = riskKeys[parseInt(choice) - 1];

        if (!selectedRisk) {
            throw new Error('Invalid risk profile selection');
        }

        this.config.risk = { ...this.riskProfiles[selectedRisk], level: selectedRisk };
        console.log(`✅ Selected: ${this.config.risk.name}`);
        console.log('');
    }

    // Step 2: Build portfolio allocation
    async selectPortfolio() {
        console.log('💰 STEP 2: Build Your Portfolio');
        console.log('');
        console.log('Recommended cryptocurrencies for your risk level:');
        console.log('');

        const recommended = this.config.risk.recommendedTokens;

        // Show recommended tokens
        recommended.forEach((token, index) => {
            const info = this.supportedTokens[token];
            console.log(`${index + 1}. ${token} (${info.name})`);
            console.log(`   ${info.description}`);
            console.log(`   Risk: ${info.riskLevel}, Allocation: ${info.minAllocation}-${info.maxAllocation}%`);
            console.log('');
        });

        this.config.portfolio.tokens = {};
        let totalAllocation = 0;

        // Get allocation for each recommended token
        for (const token of recommended) {
            const info = this.supportedTokens[token];
            const remaining = 100 - this.config.risk.cashReservePercent - totalAllocation;

            if (remaining <= 0) break;

            const question = `Enter allocation % for ${token} (${info.name}) [0-${Math.min(remaining, info.maxAllocation)}]: `;
            const allocation = await this.askQuestion(question);
            const percent = parseFloat(allocation);

            if (percent > 0 && percent <= Math.min(remaining, info.maxAllocation)) {
                this.config.portfolio.tokens[token] = {
                    name: info.name,
                    targetPercent: percent,
                    pair: `${token}/USD`
                };
                totalAllocation += percent;
            }
        }

        this.config.portfolio.cashReservePercent = this.config.risk.cashReservePercent;
        this.config.portfolio.totalTokenAllocation = totalAllocation;

        console.log('');
        console.log('📈 Your Portfolio:');
        Object.entries(this.config.portfolio.tokens).forEach(([token, config]) => {
            console.log(`   ${token}: ${config.targetPercent}%`);
        });
        console.log(`   Cash Reserve: ${this.config.portfolio.cashReservePercent}%`);
        console.log('');
    }

    // Step 3: Configure rebalancing strategy  
    async configureRebalancing() {
        console.log('⚖️  STEP 3: Rebalancing Strategy & Risk Management');
        console.log('');
        console.log('🧠 EDUCATIONAL: Advanced Risk Management');
        console.log('The bot will automatically maintain your portfolio allocation AND protect your investments!');
        console.log('');
        console.log(`📊 Default settings (based on ${this.config.risk.name} profile):`);
        console.log('');

        this.config.rebalancing = {
            intervalMinutes: this.config.risk.rebalanceIntervalMinutes,
            maxDeviationPercent: this.config.risk.maxDeviationPercent,
            stopLossPercent: this.config.risk.stopLossPercent,
            trailingStopPercent: this.config.risk.trailingStopPercent,
            takeProfitPercent: this.config.risk.takeProfitPercent,
            strategy: 'percentage_of_portfolio'
        };

        console.log(`⏰ Rebalance Frequency: ${this.config.rebalancing.intervalMinutes} minutes`);
        console.log(`📏 Rebalance Trigger: ${this.config.rebalancing.maxDeviationPercent}% deviation from target`);
        console.log(`🛡️  Stop Loss: ${this.config.rebalancing.stopLossPercent}% (sells if token drops from buy price)`);
        console.log(`📈 Trailing Stop: ${this.config.rebalancing.trailingStopPercent}% (follows price up, protects profits)`);
        console.log(`💰 Take Profit: ${this.config.rebalancing.takeProfitPercent}% (automatic profit taking)`);
        console.log('');
        console.log('🎯 How Trailing Stops Work:');
        console.log('   • Buy BTC at $50,000');
        console.log('   • Price rises to $60,000 (20% gain!)');
        console.log('   • Trailing stop (15%) follows: triggers if drops below $51,000');
        console.log('   • Protects most of your $10,000 profit!');
        console.log('');

        const customize = await this.askQuestion('Use these settings? (y/n): ');

        if (customize.toLowerCase() === 'n') {
            console.log('');
            console.log('🔧 Customizing Risk Management:');
            console.log('');

            // Allow customization
            const interval = await this.askQuestion(`Rebalance interval (minutes) [${this.config.rebalancing.intervalMinutes}]: `);
            if (interval) this.config.rebalancing.intervalMinutes = parseInt(interval);

            const deviation = await this.askQuestion(`Max deviation before rebalance (%) [${this.config.rebalancing.maxDeviationPercent}]: `);
            if (deviation) this.config.rebalancing.maxDeviationPercent = parseFloat(deviation);

            const stopLoss = await this.askQuestion(`Stop loss percentage (%) [${this.config.rebalancing.stopLossPercent}]: `);
            if (stopLoss) this.config.rebalancing.stopLossPercent = parseFloat(stopLoss);

            const trailingStop = await this.askQuestion(`Trailing stop percentage (%) [${this.config.rebalancing.trailingStopPercent}]: `);
            if (trailingStop) this.config.rebalancing.trailingStopPercent = parseFloat(trailingStop);

            const takeProfit = await this.askQuestion(`Take profit percentage (%) [${this.config.rebalancing.takeProfitPercent}]: `);
            if (takeProfit) this.config.rebalancing.takeProfitPercent = parseFloat(takeProfit);
        }

        console.log('✅ Advanced risk management configured');
        console.log('');
    }

    // Step 4: Optional notifications
    async configureNotifications() {
        console.log('📬 STEP 4: Notifications (Optional)');
        console.log('');
        console.log('Get alerted when trades happen or problems occur.');
        console.log('');

        const wantNotifications = await this.askQuestion('Enable notifications? (y/n): ');

        if (wantNotifications.toLowerCase() === 'y') {
            const email = await this.askQuestion('Email address (optional): ');
            const discord = await this.askQuestion('Discord webhook URL (optional): ');

            this.config.notifications = {
                enabled: true,
                email: email || null,
                discord: discord || null,
                events: ['trade_executed', 'stop_loss_triggered', 'error']
            };
        } else {
            this.config.notifications = {
                enabled: false,
                events: []
            };
        }

        console.log('✅ Notifications configured');
        console.log('');
    }

    // Step 5: Review configuration and save
    async reviewAndSave() {
        console.log('📋 STEP 5: Review Your Configuration');
        console.log('');
        console.log('='.repeat(50));
        console.log('TRADING BOT CONFIGURATION');
        console.log('='.repeat(50));
        console.log('');
        console.log(`🛡️  Mode: ${this.config.mode.toUpperCase()} (Safe for learning!)`);
        console.log(`📊 Risk Level: ${this.config.risk.name}`);
        console.log(`💰 Cash Reserve: ${this.config.portfolio.cashReservePercent}%`);
        console.log('');
        console.log('Portfolio Allocation:');
        Object.entries(this.config.portfolio.tokens).forEach(([token, config]) => {
            console.log(`   📈 ${token} (${config.name}): ${config.targetPercent}%`);
        });
        console.log('');
        console.log('Risk Management & Rebalancing:');
        console.log(`   ⏰ Frequency: ${this.config.rebalancing.intervalMinutes} minutes`);
        console.log(`   📏 Trigger: ${this.config.rebalancing.maxDeviationPercent}% deviation`);
        console.log(`   🛡️  Stop Loss: ${this.config.rebalancing.stopLossPercent}% (protection from losses)`);
        console.log(`   📈 Trailing Stop: ${this.config.rebalancing.trailingStopPercent}% (protects profits)`);
        console.log(`   💰 Take Profit: ${this.config.rebalancing.takeProfitPercent}% (automatic profit taking)`);
        console.log('');
        console.log('🎯 What This Means:');
        console.log('   • Your bot rebalances portfolio every few minutes');
        console.log('   • Stop losses protect from big crashes');
        console.log('   • Trailing stops let winners run while protecting gains');
        console.log('   • Take profits lock in gains at your target level');
        console.log('   • You sleep better knowing risks are managed automatically!');
        console.log('');
        console.log('='.repeat(50));
        console.log('');

        const confirm = await this.askQuestion('Save this configuration? (y/n): ');

        if (confirm.toLowerCase() === 'y') {
            await this.saveConfiguration();
            console.log('✅ Configuration saved!');
        } else {
            throw new Error('Configuration cancelled by user');
        }
    }

    // Save configuration to file
    async saveConfiguration() {
        const configDir = path.join(__dirname, 'config');
        const configPath = path.join(configDir, 'trading-bot.json');
        const backupPath = path.join(configDir, `trading-bot-backup-${Date.now()}.json`);

        // Ensure config directory exists
        await fs.mkdir(configDir, { recursive: true });

        // Backup existing config if it exists
        try {
            const existing = await fs.readFile(configPath, 'utf8');
            await fs.writeFile(backupPath, existing);
            console.log(`💾 Backed up existing config to: ${backupPath}`);
        } catch (err) {
            // No existing config, that's fine
        }

        // Save new configuration
        const configJson = JSON.stringify(this.config, null, 2);
        await fs.writeFile(configPath, configJson);

        // Also save a human-readable summary
        const summaryPath = path.join(configDir, 'configuration-summary.txt');
        const summary = this.generateSummary();
        await fs.writeFile(summaryPath, summary);
    }

    // Generate human-readable summary
    generateSummary() {
        const tokens = Object.entries(this.config.portfolio.tokens)
            .map(([token, config]) => `${token}: ${config.targetPercent}%`)
            .join(', ');

        return `
TRADING BOT CONFIGURATION SUMMARY
Generated: ${new Date().toLocaleString()}

Risk Profile: ${this.config.risk.name}
Portfolio: ${tokens}
Cash Reserve: ${this.config.portfolio.cashReservePercent}%

Advanced Risk Management:
- Frequency: ${this.config.rebalancing.intervalMinutes} minutes
- Trigger: ${this.config.rebalancing.maxDeviationPercent}% deviation  
- Stop Loss: ${this.config.rebalancing.stopLossPercent}% (protection from losses)
- Trailing Stop: ${this.config.rebalancing.trailingStopPercent}% (protects profits)
- Take Profit: ${this.config.rebalancing.takeProfitPercent}% (automatic profit taking)

Mode: ${this.config.mode.toUpperCase()} (${this.config.mode === 'paper' ? 'No real money' : 'LIVE TRADING'})

TRAILING STOPS & TAKE PROFITS EXPLAINED:
- Trailing stops follow price up but trigger on drops from peak
- Take profits automatically lock in gains at your target level  
- These features remove emotion from trading decisions
- Essential for managing risk in volatile crypto markets

IMPORTANT: This bot is for educational purposes. 
Start with paper trading to learn how it works!
Never risk more than you can afford to lose.

Randy's original bot (from rebalanceBot_0.2.2) enhanced with educational features!
`;
    }

    // Utility: Ask question and wait for answer
    askQuestion(question) {
        return new Promise((resolve) => {
            this.rl.question(question, (answer) => {
                resolve(answer.trim());
            });
        });
    }
}

// Run the wizard
if (require.main === module) {
    const wizard = new TradingBotWizard();
    wizard.run().catch(console.error);
}

module.exports = TradingBotWizard;