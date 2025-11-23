/**
 * 📱 Web-Based Log Monitor - Rotation-Safe Interface
 * 
 * This provides the web interface Randy mentioned for following logs
 * without the tail -f truncation problems
 */

const express = require('express');
const WebSocket = require('ws');
const fs = require('fs');
const path = require('path');
const { EventEmitter } = require('events');

class WebLogMonitor extends EventEmitter {
    constructor(logManager, options = {}) {
        super();

        this.logManager = logManager;
        this.config = {
            port: options.port || 3000,
            maxHistoryLines: options.maxHistoryLines || 1000,
            updateInterval: options.updateInterval || 1000
        };

        this.app = express();
        this.server = null;
        this.wss = null;
        this.clients = new Set();
        this.logHistory = new Map();
        this.fileWatchers = new Map();

        this.setupRoutes();
        this.setupWebSocket();
    }

    /**
     * 🌐 Setup Express Routes
     */
    setupRoutes() {
        // Serve static files
        this.app.use(express.static(path.join(__dirname, '../web')));

        // Main log monitor page
        this.app.get('/', (req, res) => {
            res.send(this.generateLogMonitorHTML());
        });

        // API endpoint for log statistics
        this.app.get('/api/logs', (req, res) => {
            const stats = this.logManager.getLogStatistics();
            res.json(stats);
        });

        // API endpoint for specific log content
        this.app.get('/api/logs/:name', (req, res) => {
            const logName = req.params.name;
            const lines = this.getLogContent(logName);
            res.json({ name: logName, lines });
        });

        // API endpoint to trigger manual rotation
        this.app.post('/api/logs/:name/rotate', async (req, res) => {
            const logName = req.params.name;
            try {
                await this.logManager.rotateLog(logName);
                res.json({ success: true, message: `Log ${logName} rotated successfully` });
            } catch (error) {
                res.status(500).json({ success: false, error: error.message });
            }
        });
    }

    /**
     * 🔌 Setup WebSocket for Real-time Log Updates
     */
    setupWebSocket() {
        // Listen for log manager events
        this.logManager.on('log-rotated', (event) => {
            this.broadcastToClients('log-rotated', event);
            this.handleLogRotation(event.loggerName);
        });

        this.logManager.on('logger-created', (event) => {
            this.startWatchingLog(event.name);
        });
    }

    /**
     * 🚀 Start the Web Monitor
     */
    async start() {
        return new Promise((resolve, reject) => {
            this.server = this.app.listen(this.config.port, (error) => {
                if (error) {
                    reject(error);
                    return;
                }

                console.log(`📱 Web Log Monitor started on http://localhost:${this.config.port}`);

                // Setup WebSocket server
                this.wss = new WebSocket.Server({ server: this.server });
                this.setupWebSocketHandlers();

                resolve();
            });
        });
    }

    /**
     * 🔌 Setup WebSocket Connection Handlers
     */
    setupWebSocketHandlers() {
        this.wss.on('connection', (ws) => {
            console.log('📱 New web client connected');
            this.clients.add(ws);

            // Send initial log data
            const stats = this.logManager.getLogStatistics();
            ws.send(JSON.stringify({
                type: 'initial-data',
                data: stats
            }));

            // Send recent log history for all logs
            for (const [logName, history] of this.logHistory.entries()) {
                ws.send(JSON.stringify({
                    type: 'log-history',
                    logName,
                    lines: history.slice(-50) // Last 50 lines
                }));
            }

            ws.on('close', () => {
                console.log('📱 Web client disconnected');
                this.clients.delete(ws);
            });

            ws.on('message', (data) => {
                try {
                    const message = JSON.parse(data);
                    this.handleClientMessage(ws, message);
                } catch (error) {
                    console.error('Invalid message from client:', error);
                }
            });
        });
    }

    /**
     * 📨 Handle Client Messages
     */
    handleClientMessage(ws, message) {
        switch (message.type) {
            case 'subscribe-log':
                // Client wants to subscribe to specific log
                ws.logSubscriptions = ws.logSubscriptions || new Set();
                ws.logSubscriptions.add(message.logName);
                break;

            case 'unsubscribe-log':
                if (ws.logSubscriptions) {
                    ws.logSubscriptions.delete(message.logName);
                }
                break;

            case 'request-rotation':
                // Client requested manual log rotation
                this.logManager.rotateLog(message.logName);
                break;
        }
    }

    /**
     * 👀 Start Watching a Log File
     * This handles the rotation gracefully unlike tail -f
     */
    startWatchingLog(logName) {
        const logger = this.logManager.activeLoggers.get(logName);
        if (!logger) return;

        const filePath = logger.getCurrentFilePath();
        this.logHistory.set(logName, []);

        // Read existing content
        this.readExistingLogContent(logName, filePath);

        // Watch for new content
        this.watchLogFile(logName, filePath);
    }

    /**
     * 📖 Read Existing Log Content
     */
    readExistingLogContent(logName, filePath) {
        try {
            if (fs.existsSync(filePath)) {
                const content = fs.readFileSync(filePath, 'utf8');
                const lines = content.split('\n').filter(line => line.trim());
                this.logHistory.set(logName, lines.slice(-this.config.maxHistoryLines));
            }
        } catch (error) {
            console.error(`Error reading log content for ${logName}:`, error);
        }
    }

    /**
     * 👁️ Watch Log File for Changes
     */
    watchLogFile(logName, filePath) {
        let lastSize = 0;

        // Get initial file size
        try {
            const stat = fs.statSync(filePath);
            lastSize = stat.size;
        } catch (error) {
            // File doesn't exist yet
        }

        const watcher = setInterval(() => {
            this.checkForLogUpdates(logName, filePath, lastSize);

            try {
                const stat = fs.statSync(filePath);
                lastSize = stat.size;
            } catch (error) {
                // File might have been rotated
                lastSize = 0;
            }
        }, this.config.updateInterval);

        this.fileWatchers.set(logName, watcher);
    }

    /**
     * 🔍 Check for Log Updates
     */
    checkForLogUpdates(logName, filePath, lastSize) {
        try {
            if (!fs.existsSync(filePath)) {
                // File might have been rotated
                return;
            }

            const stat = fs.statSync(filePath);
            if (stat.size <= lastSize) {
                // No new content (or file was truncated/rotated)
                return;
            }

            // Read new content
            const fd = fs.openSync(filePath, 'r');
            const buffer = Buffer.alloc(stat.size - lastSize);
            fs.readSync(fd, buffer, 0, buffer.length, lastSize);
            fs.closeSync(fd);

            const newContent = buffer.toString('utf8');
            const newLines = newContent.split('\n').filter(line => line.trim());

            if (newLines.length > 0) {
                // Add to history
                const history = this.logHistory.get(logName) || [];
                history.push(...newLines);

                // Keep only recent lines
                if (history.length > this.config.maxHistoryLines) {
                    history.splice(0, history.length - this.config.maxHistoryLines);
                }

                this.logHistory.set(logName, history);

                // Broadcast to web clients
                this.broadcastToClients('new-log-lines', {
                    logName,
                    lines: newLines,
                    timestamp: new Date().toISOString()
                });
            }

        } catch (error) {
            console.error(`Error checking log updates for ${logName}:`, error);
        }
    }

    /**
     * 🔄 Handle Log Rotation
     * This is the key to solving Randy's problem - graceful rotation handling
     */
    handleLogRotation(logName) {
        console.log(`🔄 Handling log rotation for ${logName}`);

        // Stop watching the old file
        if (this.fileWatchers.has(logName)) {
            clearInterval(this.fileWatchers.get(logName));
        }

        // Start watching the new file
        const logger = this.logManager.activeLoggers.get(logName);
        if (logger) {
            const newFilePath = logger.getCurrentFilePath();
            this.watchLogFile(logName, newFilePath);

            // Notify clients about rotation
            this.broadcastToClients('log-rotated', {
                logName,
                newFilePath,
                message: `Log ${logName} was rotated - monitoring continues seamlessly`
            });
        }
    }

    /**
     * 📡 Broadcast Message to All Clients
     */
    broadcastToClients(type, data) {
        const message = JSON.stringify({ type, data });

        this.clients.forEach(client => {
            if (client.readyState === WebSocket.OPEN) {
                try {
                    client.send(message);
                } catch (error) {
                    console.error('Error sending to client:', error);
                    this.clients.delete(client);
                }
            }
        });
    }

    /**
     * 📄 Get Log Content
     */
    getLogContent(logName) {
        return this.logHistory.get(logName) || [];
    }

    /**
     * 🌐 Generate HTML for Log Monitor Interface
     */
    generateLogMonitorHTML() {
        return `
<!DOCTYPE html>
<html>
<head>
    <title>🗂️ Advanced Log Monitor - Rotation Safe</title>
    <style>
        body { font-family: monospace; margin: 20px; background: #1a1a1a; color: #00ff00; }
        .header { background: #333; padding: 10px; border-radius: 5px; margin-bottom: 20px; }
        .log-container { border: 1px solid #555; margin-bottom: 20px; border-radius: 5px; }
        .log-header { background: #444; padding: 10px; display: flex; justify-content: space-between; align-items: center; }
        .log-content { height: 300px; overflow-y: auto; padding: 10px; background: #2a2a2a; }
        .log-line { margin: 2px 0; }
        .timestamp { color: #888; }
        .level-info { color: #00ff00; }
        .level-warn { color: #ffaa00; }
        .level-error { color: #ff0000; }
        .status-indicator { display: inline-block; width: 10px; height: 10px; border-radius: 50%; margin-right: 5px; }
        .status-active { background: #00ff00; }
        .status-rotated { background: #ffaa00; }
        .button { background: #555; color: white; border: none; padding: 5px 10px; cursor: pointer; border-radius: 3px; }
        .button:hover { background: #666; }
        .educational-note { background: #003366; border-left: 4px solid #0066cc; padding: 10px; margin: 10px 0; }
    </style>
</head>
<body>
    <div class="header">
        <h1>🗂️ Advanced Log Monitor - Rotation Safe</h1>
        <p>📚 Educational Demo: Solving the "tail -f" truncation problem</p>
        <div class="educational-note">
            <strong>🎓 Educational Context:</strong> This interface solves Randy's original problem where 
            log truncation would break tail -f monitoring. Here, log rotation is handled gracefully 
            with no interruption to monitoring capability.
        </div>
    </div>
    
    <div id="logs-container"></div>
    
    <script>
        class LogMonitor {
            constructor() {
                this.ws = new WebSocket(\`ws://\${window.location.host}\`);
                this.setupWebSocket();
                this.logs = new Map();
            }
            
            setupWebSocket() {
                this.ws.onopen = () => {
                    console.log('Connected to log monitor');
                };
                
                this.ws.onmessage = (event) => {
                    const message = JSON.parse(event.data);
                    this.handleMessage(message);
                };
                
                this.ws.onclose = () => {
                    console.log('Disconnected from log monitor');
                    setTimeout(() => this.reconnect(), 3000);
                };
            }
            
            reconnect() {
                this.ws = new WebSocket(\`ws://\${window.location.host}\`);
                this.setupWebSocket();
            }
            
            handleMessage(message) {
                switch (message.type) {
                    case 'initial-data':
                        this.initializeLogs(message.data);
                        break;
                    case 'log-history':
                        this.displayLogHistory(message.logName, message.lines);
                        break;
                    case 'new-log-lines':
                        this.appendLogLines(message.data.logName, message.data.lines);
                        break;
                    case 'log-rotated':
                        this.handleLogRotation(message.data);
                        break;
                }
            }
            
            initializeLogs(stats) {
                const container = document.getElementById('logs-container');
                container.innerHTML = '';
                
                Object.keys(stats).forEach(logName => {
                    this.createLogDisplay(logName);
                });
            }
            
            createLogDisplay(logName) {
                const container = document.getElementById('logs-container');
                
                const logDiv = document.createElement('div');
                logDiv.className = 'log-container';
                logDiv.id = \`log-\${logName}\`;
                
                logDiv.innerHTML = \`
                    <div class="log-header">
                        <span>
                            <span class="status-indicator status-active"></span>
                            📄 Log: \${logName}
                        </span>
                        <button class="button" onclick="monitor.rotateLog('\${logName}')">
                            🔄 Rotate
                        </button>
                    </div>
                    <div class="log-content" id="content-\${logName}"></div>
                \`;
                
                container.appendChild(logDiv);
                this.logs.set(logName, []);
            }
            
            displayLogHistory(logName, lines) {
                this.logs.set(logName, lines);
                this.renderLogContent(logName);
            }
            
            appendLogLines(logName, newLines) {
                const currentLines = this.logs.get(logName) || [];
                currentLines.push(...newLines);
                
                // Keep only recent lines
                if (currentLines.length > 1000) {
                    currentLines.splice(0, currentLines.length - 1000);
                }
                
                this.logs.set(logName, currentLines);
                this.renderLogContent(logName);
                this.scrollToBottom(logName);
            }
            
            renderLogContent(logName) {
                const contentDiv = document.getElementById(\`content-\${logName}\`);
                if (!contentDiv) return;
                
                const lines = this.logs.get(logName) || [];
                contentDiv.innerHTML = lines.map(line => {
                    const levelClass = this.getLogLevelClass(line);
                    return \`<div class="log-line \${levelClass}">\${this.escapeHtml(line)}</div>\`;
                }).join('');
            }
            
            getLogLevelClass(line) {
                if (line.includes('[ERROR]')) return 'level-error';
                if (line.includes('[WARN]')) return 'level-warn';
                return 'level-info';
            }
            
            escapeHtml(text) {
                const div = document.createElement('div');
                div.textContent = text;
                return div.innerHTML;
            }
            
            scrollToBottom(logName) {
                const contentDiv = document.getElementById(\`content-\${logName}\`);
                if (contentDiv) {
                    contentDiv.scrollTop = contentDiv.scrollHeight;
                }
            }
            
            handleLogRotation(data) {
                console.log('Log rotated:', data);
                
                // Update status indicator temporarily
                const indicator = document.querySelector(\`#log-\${data.logName} .status-indicator\`);
                if (indicator) {
                    indicator.className = 'status-indicator status-rotated';
                    setTimeout(() => {
                        indicator.className = 'status-indicator status-active';
                    }, 3000);
                }
                
                // Add rotation message to log
                this.appendLogLines(data.logName, [
                    \`🔄 [\${new Date().toISOString()}] Log rotated - monitoring continues seamlessly\`
                ]);
            }
            
            rotateLog(logName) {
                this.ws.send(JSON.stringify({
                    type: 'request-rotation',
                    logName: logName
                }));
            }
        }
        
        const monitor = new LogMonitor();
    </script>
</body>
</html>`;
    }

    /**
     * 🛑 Stop the Web Monitor
     */
    async stop() {
        // Clear all file watchers
        for (const watcher of this.fileWatchers.values()) {
            clearInterval(watcher);
        }

        // Close WebSocket connections
        if (this.wss) {
            this.wss.close();
        }

        // Close HTTP server
        if (this.server) {
            await new Promise(resolve => {
                this.server.close(resolve);
            });
        }
    }
}

module.exports = WebLogMonitor;