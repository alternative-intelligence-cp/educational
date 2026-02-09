// dbug.js Example - Tag-based Debug Logging Utility
// This demonstrates environment detection, color formatting, and tag filtering

const dBug = require('./dbug.js');

console.log('='.repeat(60));
console.log('EXAMPLE 1: Default Behavior (all tags enabled)');
console.log('='.repeat(60));

dBug.print("Application initialized successfully", "PASS");
dBug.print("Loading configuration from disk", "LOGS");
dBug.print("User authentication token expired", "WARN");
dBug.print("Database connection failed - timeout", "FAIL");
dBug.print('{"user": "alice", "age": 30, "role": "admin"}', "DATA");

console.log('\n' + '='.repeat(60));
console.log('EXAMPLE 2: Selective Tag Filtering');
console.log('='.repeat(60));

// Disable all, then enable only specific tags
dBug.disable("all");
dBug.enable(["database", "api", "cache"]);

dBug.print("DB: Connected to PostgreSQL on localhost:5432", "PASS", "database");
dBug.print("API: Starting Express server on port 3000", "LOGS", "api");
dBug.print("UI: Rendering homepage component", "LOGS", "frontend");  // Won't print
dBug.print("CACHE: Redis connection established", "PASS", "cache");
dBug.print("DB: Query took 1.2s (slow query warning)", "WARN", "database");
dBug.print("AUTH: Failed login attempt from 192.168.1.50", "FAIL", "security");  // Won't print

console.log('\n' + '='.repeat(60));
console.log('EXAMPLE 3: Reset to Defaults');
console.log('='.repeat(60));

// Reset back to default behavior (all enabled)
dBug.reset();

dBug.print("Frontend module loaded", "PASS", "frontend");
dBug.print("Security middleware initialized", "PASS", "security");
dBug.print('{"status":"healthy","uptime":3600}', "DATA", "monitoring");

console.log('\n' + '='.repeat(60));
console.log('EXAMPLE 4: Diagnostic - Show Active Tags');
console.log('='.repeat(60));

const activeTags = dBug.getEnabled();
console.log(`Currently enabled tags: [${activeTags.join(', ')}]`);

console.log('\n' + '='.repeat(60));
console.log('EXAMPLE 5: Complex Filtering Workflow');
console.log('='.repeat(60));

// Disable all tags, enable only network and performance
dBug.disable("all");
dBug.enable(["network", "performance"]);

dBug.print("HTTP GET /api/users - 200 OK (45ms)", "PASS", "network");
dBug.print("Cache miss - fetching from database", "WARN", "performance");
dBug.print("SQL query executed in 23ms", "LOGS", "database");  // Won't print
dBug.print("Memory usage: 245 MB (threshold: 512 MB)", "DATA", "performance");
dBug.print("WebSocket connection dropped", "FAIL", "network");

console.log('\n' + '='.repeat(60));
console.log('EXAMPLE 6: Using Reset');
console.log('='.repeat(60));

// Forgot what tags you enabled? Reset to defaults
dBug.reset();
dBug.print("Reset complete - all tags enabled again", "PASS");

console.log('\n' + '='.repeat(60));
console.log('EXAMPLE COMPLETE');
console.log('='.repeat(60));
