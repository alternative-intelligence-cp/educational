#!/usr/bin/env node

/**
 * 🧙‍♂️ Demo of Configuration Wizard for Crypto Trading Bot
 * 
 * This is a simplified version to demonstrate the concept
 * Shows how we solve the "Uncle Mike Problem" - unusable configurations
 */

const readline = require('readline');
const fs = require('fs').promises;
const path = require('path');

// Demo of the educational configuration wizard
class TradingBotWizardDemo {
    constructor() {
        this.rl = readline.createInterface({
            input: process.stdin,
            output: process.stdout
        });

        this.config = {
            version: "2.0.0-educational",
            mode: "paper", // Always start safe!
            created: new Date().toISOString(),
            portfolio: {},
            risk: {},
            rebalancing: {}
        };

        // Educational crypto info
        this.cryptos = {
            'BTC': { name: 'Bitcoin', risk: 'Medium', min: 10, max: 60 },
            'ETH': { name: 'Ethereum', risk: 'Medium-High', min: 5, max: 40 },
            'ADA': { name: 'Cardano', risk: 'High', min: 0, max: 20 }
        };

        this.riskProfiles = {
            conservative: {
                name: 'Conservative (Safe)',
                cash: 40,
                rebalance: 60,
                stopLoss: 15,
                trailingStop: 10,
                takeProfit: 20
            },
            moderate: {
                name: 'Moderate (Balanced)',
                cash: 25,
                rebalance: 30,
                stopLoss: 20,
                trailingStop: 15,
                takeProfit: 30
            },
            aggressive: {
                name: 'Aggressive (Higher Risk)',
                cash: 15,
                rebalance: 15,
                stopLoss: 25,
                trailingStop: 20,
                takeProfit: 50
            }
        };
    }

    async run() {
        try {
            await this.showIntro();
            await this.selectRiskProfile();
            await this.buildPortfolio();
            await this.finalizeConfig();
            await this.showResults();
        } catch (error) {
            console.log('\n❌ Setup cancelled:', error.message);
        } finally {
            this.rl.close();
        }
    }

    async showIntro() {
        console.log('\n' + '='.repeat(60));
        console.log('🤖  EDUCATIONAL CRYPTO TRADING BOT - SETUP WIZARD');
        console.log('='.repeat(60));
        console.log('\n📚 This wizard solves the "Uncle Mike Problem":');
        console.log('   ❌ Old config: {"name":{"name":"name","targetPercent":5}}');
        console.log('   ✅ New config: Interactive, validated, educational!');
        console.log('\n🛡️  We start in PAPER TRADING mode - no real money at risk!');
        console.log('\n🎯 You\'ll learn:');
        console.log('   • Portfolio allocation strategies');
        console.log('   • Risk management principles');
        console.log('   • Cryptocurrency rebalancing theory');
        console.log('   • How trading bots actually work');

        await this.askQuestion('\n📖 Press Enter to start learning...');
    }

    async selectRiskProfile() {
        console.log('\n📊 STEP 1: Choose Your Risk Profile');
        console.log('\nEducational Note: This determines how aggressive your rebalancing will be.');
        console.log('Higher risk = more frequent trading, potentially higher returns, but more volatility.\n');

        Object.entries(this.riskProfiles).forEach(([key, profile], index) => {
            console.log(`${index + 1}. ${profile.name}`);
            console.log(`   💰 Cash Reserve: ${profile.cash}%`);
            console.log(`   ⏰ Rebalance: Every ${profile.rebalance} minutes`);
            console.log(`   🛡️  Stop Loss: ${profile.stopLoss}%`);
            console.log(`   📈 Trailing Stop: ${profile.trailingStop}%`);
            console.log(`   💎 Take Profit: ${profile.takeProfit}%`);
            console.log('');
        });

        const choice = await this.askQuestion('Select your risk profile (1-3): ');
        const profiles = Object.values(this.riskProfiles);
        const selected = profiles[parseInt(choice) - 1];

        if (!selected) throw new Error('Invalid selection');

        this.config.risk = selected;
        console.log(`\n✅ Selected: ${selected.name}`);
        console.log(`\n💡 Educational Insight: ${selected.name} profiles are good for:`);

        if (choice === '1') {
            console.log('   • Learning the basics without high volatility');
            console.log('   • Long-term holding with occasional rebalancing');
            console.log('   • Preserving capital while getting small gains');
        } else if (choice === '2') {
            console.log('   • Balanced approach to growth and stability');
            console.log('   • Good for most people new to crypto trading');
            console.log('   • Reasonable returns with manageable risk');
        } else {
            console.log('   • Experienced traders comfortable with volatility');
            console.log('   • Maximizing potential returns (and potential losses)');
            console.log('   • Active trading and frequent rebalancing');
        }
    }

    async buildPortfolio() {
        console.log('\n💰 STEP 2: Build Your Portfolio');
        console.log('\nEducational Note: Portfolio allocation is the foundation of successful investing.');
        console.log('We\'ll help you build a diversified portfolio based on your risk tolerance.\n');

        console.log('Available cryptocurrencies:');
        Object.entries(this.cryptos).forEach(([symbol, info], index) => {
            console.log(`${index + 1}. ${symbol} (${info.name})`);
            console.log(`   Risk Level: ${info.risk}`);
            console.log(`   Recommended Range: ${info.min}-${info.max}%`);
            console.log('');
        });

        this.config.portfolio.tokens = {};
        let totalAllocation = 0;
        const maxCrypto = 100 - this.config.risk.cash;

        for (const [symbol, info] of Object.entries(this.cryptos)) {
            const remaining = maxCrypto - totalAllocation;
            if (remaining <= 0) break;

            const max = Math.min(remaining, info.max);
            const allocation = await this.askQuestion(
                `Enter allocation % for ${symbol} (${info.name}) [0-${max}]: `
            );

            const percent = parseFloat(allocation);
            if (percent > 0 && percent <= max) {
                this.config.portfolio.tokens[symbol] = {
                    name: info.name,
                    targetPercent: percent,
                    pair: `${symbol}/USD`
                };
                totalAllocation += percent;
                console.log(`   ✅ Added ${symbol}: ${percent}%`);
            }
        }

        this.config.portfolio.cashReservePercent = this.config.risk.cash;
        this.config.portfolio.totalAllocation = totalAllocation;

        console.log('\n📈 Your Portfolio Allocation:');
        Object.entries(this.config.portfolio.tokens).forEach(([symbol, config]) => {
            console.log(`   📊 ${symbol}: ${config.targetPercent}%`);
        });
        console.log(`   💵 Cash Reserve: ${this.config.portfolio.cashReservePercent}%`);
        console.log(`   📈 Total Crypto: ${totalAllocation}%`);

        console.log('\n💡 Educational Insight: Why cash reserves matter:');
        console.log('   • Provides liquidity for rebalancing opportunities');
        console.log('   • Reduces overall portfolio volatility');
        console.log('   • Allows buying during market dips');
        console.log('   • Essential for proper risk management');
    }

    async finalizeConfig() {
        console.log('\n⚖️  STEP 3: Finalize Advanced Trading Settings');
        console.log('\nBased on your risk profile, here are your comprehensive risk management parameters:');
        console.log('');

        this.config.rebalancing = {
            intervalMinutes: this.config.risk.rebalance,
            maxDeviationPercent: 5, // Standard for most strategies
            stopLossPercent: this.config.risk.stopLoss,
            trailingStopPercent: this.config.risk.trailingStop,
            takeProfitPercent: this.config.risk.takeProfit,
            strategy: 'percentage_of_portfolio'
        };

        console.log(`⏰ Rebalance Frequency: Every ${this.config.rebalancing.intervalMinutes} minutes`);
        console.log(`📏 Rebalance Trigger: When allocation drifts 5% from target`);
        console.log(`🛡️  Stop Loss: Sell if any token drops ${this.config.rebalancing.stopLossPercent}%`);
        console.log(`📈 Trailing Stop: Follows price up, triggers on ${this.config.rebalancing.trailingStopPercent}% drop from peak`);
        console.log(`💎 Take Profit: Automatic profit taking at ${this.config.rebalancing.takeProfitPercent}% gain`);
        console.log('');

        console.log('💡 Educational Insight: Advanced risk management in action:');
        console.log('   🎯 Example with BTC at $50,000:');
        console.log(`   • Stop Loss triggers if drops to $${(50000 * (1 - this.config.rebalancing.stopLossPercent / 100)).toLocaleString()}`);
        console.log(`   • Take Profit triggers if rises to $${(50000 * (1 + this.config.rebalancing.takeProfitPercent / 100)).toLocaleString()}`);
        console.log(`   • Trailing Stop follows price up, protects profits!`);
        console.log('');
        console.log('💡 How rebalancing works:');
        console.log('   1. Monitor your actual vs target allocation percentages');
        console.log('   2. When a token becomes overweight (above target %), sell some');
        console.log('   3. When a token becomes underweight (below target %), buy more');
        console.log('   4. This automatically "buys low, sells high" over time');
        console.log('   5. Maintains your desired risk profile as markets change');

        const accept = await this.askQuestion('\nAccept these settings? (y/n): ');
        if (accept.toLowerCase() !== 'y') {
            throw new Error('Settings not accepted');
        }
    }

    async showResults() {
        console.log('\n' + '='.repeat(60));
        console.log('🎉 CONFIGURATION COMPLETE!');
        console.log('='.repeat(60));

        console.log('\n📋 Your Trading Bot Configuration:');
        console.log(`   🛡️  Mode: ${this.config.mode.toUpperCase()} (Safe for learning!)`);
        console.log(`   📊 Risk: ${this.config.risk.name}`);
        console.log(`   💰 Cash: ${this.config.portfolio.cashReservePercent}%`);
        console.log('');

        console.log('   Portfolio:');
        Object.entries(this.config.portfolio.tokens).forEach(([symbol, config]) => {
            console.log(`     📈 ${symbol} (${config.name}): ${config.targetPercent}%`);
        });

        console.log('\n   Trading Parameters:');
        console.log(`     ⏰ Rebalance: Every ${this.config.rebalancing.intervalMinutes} minutes`);
        console.log(`     📏 Trigger: 5% deviation from target`);
        console.log(`     🛡️  Stop Loss: ${this.config.rebalancing.stopLossPercent}%`);
        console.log(`     📈 Trailing Stop: ${this.config.rebalancing.trailingStopPercent}%`);
        console.log(`     💎 Take Profit: ${this.config.rebalancing.takeProfitPercent}%`);

        console.log('\n💾 Saving configuration...');
        await this.saveConfig();

        console.log('\n🚀 Next Steps:');
        console.log('   1. Study how the bot works in paper trading mode');
        console.log('   2. Watch it make simulated trades with live market data');
        console.log('   3. Learn about portfolio theory and risk management');
        console.log('   4. Understand trailing stops and take profit strategies');
        console.log('   5. Only consider live trading after you understand everything!');

        console.log('\n🎓 Educational Value:');
        console.log('   ✅ You now understand portfolio allocation theory');
        console.log('   ✅ You know how automated rebalancing works');
        console.log('   ✅ You\'ve configured sophisticated risk management');
        console.log('   ✅ You understand trailing stops and profit taking');
        console.log('   ✅ You can explain these concepts to Uncle Mike! 😄');

        console.log('\n⚠️  IMPORTANT REMINDER:');
        console.log('   This is EDUCATIONAL software. Start with paper trading.');
        console.log('   Learn how it works before risking real money.');
        console.log('   Never invest more than you can afford to lose!');
        console.log('');
    }

    async saveConfig() {
        const configDir = path.join(__dirname, 'config');
        const configPath = path.join(configDir, 'educational-trading-config.json');

        try {
            await fs.mkdir(configDir, { recursive: true });
            const configJson = JSON.stringify(this.config, null, 2);
            await fs.writeFile(configPath, configJson);
            console.log(`   ✅ Saved to: ${configPath}`);

            // Also save human-readable summary
            const summaryPath = path.join(configDir, 'configuration-summary.txt');
            await fs.writeFile(summaryPath, this.generateSummary());
            console.log(`   📄 Summary: ${summaryPath}`);

        } catch (error) {
            console.log(`   ❌ Error saving: ${error.message}`);
        }
    }

    generateSummary() {
        const timestamp = new Date().toLocaleString();
        const tokens = Object.entries(this.config.portfolio.tokens)
            .map(([symbol, config]) => `${symbol}: ${config.targetPercent}%`)
            .join(', ');

        return `EDUCATIONAL CRYPTO TRADING BOT CONFIGURATION
Generated: ${timestamp}

PORTFOLIO ALLOCATION:
${tokens}
Cash Reserve: ${this.config.portfolio.cashReservePercent}%

RISK PROFILE: ${this.config.risk.name}
Rebalance Frequency: ${this.config.rebalancing.intervalMinutes} minutes

ADVANCED RISK MANAGEMENT:
Stop Loss: ${this.config.rebalancing.stopLossPercent}% (protection from major losses)
Trailing Stop: ${this.config.rebalancing.trailingStopPercent}% (protects profits, follows price up)
Take Profit: ${this.config.rebalancing.takeProfitPercent}% (automatic profit taking)

MODE: ${this.config.mode.toUpperCase()}
${this.config.mode === 'paper' ? '(No real money - safe for learning!)' : '⚠️ LIVE TRADING MODE ⚠️'}

EDUCATIONAL NOTES:
- This configuration implements portfolio rebalancing theory
- Advanced risk management prevents emotion-driven trading decisions  
- Trailing stops are the secret weapon of professional traders
- Take profits force disciplined profit-taking behavior
- Based on Randy's original rebalanceBot with educational enhancements
- Rebalancing automatically "buys low, sells high" over time  
- Cash reserves provide liquidity and reduce volatility
- Stop losses limit downside risk on individual positions
- Paper trading mode lets you learn without financial risk

NEXT STEPS:
1. Run the bot in paper trading mode: npm run start:paper
2. Watch the dashboard: http://localhost:3000
3. Learn how each trade decision is made
4. Study the educational materials provided
5. Only consider live trading after full understanding

Remember: This is educational software. Learn first, trade later!
`;
    }

    askQuestion(question) {
        return new Promise((resolve) => {
            this.rl.question(question, (answer) => {
                resolve(answer.trim());
            });
        });
    }
}

// Run the demo wizard
if (require.main === module) {
    console.log('🎓 Educational Crypto Trading Bot - Configuration Wizard Demo');
    console.log('This demonstrates how to solve the "Uncle Mike Problem"');
    console.log('Making complex software accessible through guided setup\n');

    const wizard = new TradingBotWizardDemo();
    wizard.run().catch(console.error);
}

module.exports = TradingBotWizardDemo;