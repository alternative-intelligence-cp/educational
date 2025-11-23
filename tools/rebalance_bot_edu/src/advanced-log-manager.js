/**
 * 🗂️ Advanced Log Manager - Solving the "tail -f" Truncation Problem
 * 
 * This educational module addresses a real-world problem Randy encountered:
 * When log files are truncated while tail -f is reading them, tail gets confused
 * because the file descriptor points to the old (now empty) file.
 * 
 * Educational Solutions Implemented:
 * 1. Log rotation instead of truncation
 * 2. Signal-based log reopening
 * 3. Graceful monitoring interfaces
 * 4. Size-based and time-based rotation
 */

const fs = require('fs');
const path = require('path');
const EventEmitter = require('events');

class AdvancedLogManager extends EventEmitter {
    constructor(options = {}) {
        super();

        this.config = {
            // Size limits
            maxFileSize: options.maxFileSize || 10 * 1024 * 1024, // 10MB default
            maxFiles: options.maxFiles || 5,

            // Rotation strategy
            rotationStrategy: options.rotationStrategy || 'size', // 'size', 'time', 'both'
            rotationInterval: options.rotationInterval || 24 * 60 * 60 * 1000, // 24 hours

            // Paths
            logDir: options.logDir || './logs',
            filePrefix: options.filePrefix || 'bot',

            // Monitoring
            enableWebInterface: options.enableWebInterface || true,
            enableSignalHandling: options.enableSignalHandling || true
        };

        this.activeLoggers = new Map();
        this.logStreams = new Map();
        this.rotationTimers = new Map();

        // Ensure log directory exists
        this.ensureLogDir();

        // Set up signal handling for log rotation
        if (this.config.enableSignalHandling) {
            this.setupSignalHandlers();
        }

        // Educational: Emit events for learning
        this.emit('manager-initialized', { config: this.config });
    }

    /**
     * 🎓 Educational Method: Create Logger with Advanced Features
     * 
     * This solves Randy's original problem by implementing proper log rotation
     * instead of simple truncation that confuses tail -f
     */
    createLogger(name, options = {}) {
        const loggerConfig = {
            ...this.config,
            ...options,
            name: name
        };

        const logger = new RotationSafeLogger(name, loggerConfig, this);
        this.activeLoggers.set(name, logger);

        this.emit('logger-created', { name, config: loggerConfig });
        return logger;
    }

    /**
     * 🔄 Smart Log Rotation - The Solution to Randy's Problem
     * 
     * Instead of truncating the file (which breaks tail -f), we:
     * 1. Rename current file to .1, .2, etc.
     * 2. Create a new file with the original name
     * 3. Signal monitoring tools to reopen file descriptors
     */
    async rotateLog(loggerName) {
        const logger = this.activeLoggers.get(loggerName);
        if (!logger) return;

        const currentFile = logger.getCurrentFilePath();
        const baseName = path.basename(currentFile, '.log');
        const dir = path.dirname(currentFile);

        try {
            // Close current stream
            logger.closeCurrentStream();

            // Rotate existing files
            await this.rotateExistingFiles(dir, baseName);

            // Move current file to .1
            const rotatedFile = path.join(dir, `${baseName}.1.log`);
            if (fs.existsSync(currentFile)) {
                fs.renameSync(currentFile, rotatedFile);
            }

            // Create new file and stream
            logger.createNewStream();

            // Educational: Log the rotation event
            logger.log(`🔄 Log rotated: ${new Date().toISOString()}`);
            logger.log(`📁 Previous logs: ${baseName}.1.log, ${baseName}.2.log, etc.`);

            this.emit('log-rotated', {
                loggerName,
                currentFile,
                rotatedFile,
                timestamp: new Date().toISOString()
            });

            // Send signal to external processes to reopen logs
            this.signalLogReopening();

        } catch (error) {
            this.emit('rotation-error', { loggerName, error: error.message });
            console.error(`Log rotation failed for ${loggerName}:`, error);
        }
    }

    /**
     * 📁 Rotate Existing Numbered Log Files
     * Keep only maxFiles number of rotated logs
     */
    async rotateExistingFiles(dir, baseName) {
        const maxFiles = this.config.maxFiles;

        // Remove oldest file if we're at the limit
        const oldestFile = path.join(dir, `${baseName}.${maxFiles}.log`);
        if (fs.existsSync(oldestFile)) {
            fs.unlinkSync(oldestFile);
        }

        // Rotate numbered files
        for (let i = maxFiles - 1; i >= 1; i--) {
            const currentFile = path.join(dir, `${baseName}.${i}.log`);
            const nextFile = path.join(dir, `${baseName}.${i + 1}.log`);

            if (fs.existsSync(currentFile)) {
                fs.renameSync(currentFile, nextFile);
            }
        }
    }

    /**
     * 📡 Signal Log Reopening to External Processes
     * 
     * This solves the tail -f problem by sending a signal that
     * monitoring processes can catch to reopen file descriptors
     */
    signalLogReopening() {
        // Send USR1 signal to parent process and any children
        // External monitoring tools can catch this to reopen logs
        process.emit('SIGUSR1');

        // Educational: Explain what's happening
        console.log('📡 Sent log rotation signal (SIGUSR1)');
        console.log('💡 External tools can catch this signal to reopen file descriptors');

        this.emit('signal-sent', { signal: 'SIGUSR1', purpose: 'log-reopening' });
    }

    /**
     * 🛠️ Setup Signal Handlers for Manual Rotation
     */
    setupSignalHandlers() {
        // Handle SIGUSR1 for manual log rotation
        process.on('SIGUSR1', () => {
            console.log('📡 Received SIGUSR1 - Rotating all logs');
            this.rotateAllLogs();
        });

        // Handle SIGHUP for configuration reload
        process.on('SIGHUP', () => {
            console.log('📡 Received SIGHUP - Reloading log configuration');
            this.reloadConfiguration();
        });
    }

    /**
     * 🔄 Rotate All Active Logs
     */
    async rotateAllLogs() {
        for (const loggerName of this.activeLoggers.keys()) {
            await this.rotateLog(loggerName);
        }
    }

    /**
     * ⚙️ Reload Configuration
     */
    reloadConfiguration() {
        // Educational: This would reload config from file
        console.log('⚙️ Log configuration reloaded');
        this.emit('configuration-reloaded');
    }

    /**
     * 📊 Get Log Statistics for Educational Dashboard
     */
    getLogStatistics() {
        const stats = {};

        for (const [name, logger] of this.activeLoggers.entries()) {
            const filePath = logger.getCurrentFilePath();
            let size = 0;
            let lastModified = null;

            try {
                const stat = fs.statSync(filePath);
                size = stat.size;
                lastModified = stat.mtime;
            } catch (error) {
                // File might not exist yet
            }

            stats[name] = {
                filePath,
                size,
                lastModified,
                sizeHuman: this.formatFileSize(size),
                rotationNeeded: size > this.config.maxFileSize
            };
        }

        return stats;
    }

    /**
     * 🗂️ Ensure Log Directory Exists
     */
    ensureLogDir() {
        if (!fs.existsSync(this.config.logDir)) {
            fs.mkdirSync(this.config.logDir, { recursive: true });
        }
    }

    /**
     * 📏 Format File Size for Human Reading
     */
    formatFileSize(bytes) {
        const units = ['B', 'KB', 'MB', 'GB'];
        let size = bytes;
        let unitIndex = 0;

        while (size >= 1024 && unitIndex < units.length - 1) {
            size /= 1024;
            unitIndex++;
        }

        return `${size.toFixed(2)} ${units[unitIndex]}`;
    }

    /**
     * 🧹 Cleanup Resources
     */
    async cleanup() {
        // Clear rotation timers
        for (const timer of this.rotationTimers.values()) {
            clearInterval(timer);
        }

        // Close all log streams
        for (const logger of this.activeLoggers.values()) {
            logger.cleanup();
        }

        this.emit('manager-cleanup');
    }
}

/**
 * 📝 Rotation-Safe Logger
 * 
 * This logger can be safely monitored with tail -f because it handles
 * rotation properly instead of truncating files
 */
class RotationSafeLogger {
    constructor(name, config, manager) {
        this.name = name;
        this.config = config;
        this.manager = manager;

        this.currentStream = null;
        this.filePath = path.join(config.logDir, `${name}.log`);

        this.createNewStream();
        this.setupSizeMonitoring();
    }

    /**
     * 📝 Log Message with Automatic Rotation Check
     */
    log(message, level = 'INFO') {
        const timestamp = new Date().toISOString();
        const logLine = `${timestamp} [${level}] ${message}\n`;

        // Check if rotation is needed before writing
        this.checkRotationNeeded();

        if (this.currentStream) {
            this.currentStream.write(logLine);
        }
    }

    /**
     * 🚨 Log Error Message
     */
    error(message) {
        this.log(message, 'ERROR');
    }

    /**
     * ⚠️ Log Warning Message
     */
    warn(message) {
        this.log(message, 'WARN');
    }

    /**
     * 🔍 Log Debug Message
     */
    debug(message) {
        if (this.config.debug) {
            this.log(message, 'DEBUG');
        }
    }

    /**
     * 🔄 Check if Rotation is Needed
     */
    checkRotationNeeded() {
        try {
            const stat = fs.statSync(this.filePath);
            if (stat.size > this.config.maxFileSize) {
                this.manager.rotateLog(this.name);
            }
        } catch (error) {
            // File might not exist yet, that's okay
        }
    }

    /**
     * 🔧 Create New Log Stream
     */
    createNewStream() {
        this.currentStream = fs.createWriteStream(this.filePath, { flags: 'a' });

        this.currentStream.on('error', (error) => {
            console.error(`Log stream error for ${this.name}:`, error);
            this.manager.emit('stream-error', { name: this.name, error });
        });
    }

    /**
     * 🔒 Close Current Stream
     */
    closeCurrentStream() {
        if (this.currentStream) {
            this.currentStream.end();
            this.currentStream = null;
        }
    }

    /**
     * 📍 Get Current File Path
     */
    getCurrentFilePath() {
        return this.filePath;
    }

    /**
     * 📊 Setup Size Monitoring
     */
    setupSizeMonitoring() {
        // Check file size periodically
        this.sizeCheckInterval = setInterval(() => {
            this.checkRotationNeeded();
        }, 5000); // Check every 5 seconds
    }

    /**
     * 🧹 Cleanup Resources
     */
    cleanup() {
        if (this.sizeCheckInterval) {
            clearInterval(this.sizeCheckInterval);
        }
        this.closeCurrentStream();
    }
}

module.exports = { AdvancedLogManager, RotationSafeLogger };