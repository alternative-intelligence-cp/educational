#!/bin/sh
# Demo script for printc - shows various use cases

PRINTC="./printc"

echo "=== printc Demo ==="
echo ""

# Show available colors
echo "1. Available Colors:"
$PRINTC --colors
echo ""
echo ""

# Basic usage
echo "2. Basic Error Messages:"
$PRINTC -n Red "ERROR: " White "Connection failed"
$PRINTC -n Yellow "WARNING: " LightGray "Rate limit approaching"
$PRINTC -n Green "SUCCESS: " White "Database backup completed"
echo ""

# Build output simulation
echo "3. Build Output Simulation:"
$PRINTC -n Cyan "[ BUILD ] " White "Compiling src/main.c"
$PRINTC -n Cyan "[ BUILD ] " White "Compiling src/utils.c"
$PRINTC -n Green "[ DONE  ] " White "Build completed in 2.3s"
echo ""

# Log levels
echo "4. Log Level Indicators:"
$PRINTC -n DarkGray "[DEBUG] " LightGray "Variable x = 42"
$PRINTC -n Blue "[INFO]  " White "Server listening on port 8080"
$PRINTC -n Yellow "[WARN]  " White "Disk usage at 85%"
$PRINTC -n Red "[ERROR] " White "Failed to authenticate user"
echo ""

# Test results
echo "5. Test Results:"
$PRINTC -n Green "✓ " White "test_addition passed"
$PRINTC -n Green "✓ " White "test_subtraction passed"
$PRINTC -n Red "✗ " White "test_division failed"
$PRINTC -n Green "✓ " White "test_multiplication passed"
echo ""
$PRINTC -n LightGray "Tests: " Green "3 passed" LightGray ", " Red "1 failed" LightGray ", 4 total"
echo ""

# Complex multi-color line
echo "6. Multi-Color Output:"
$PRINTC -n Cyan "[" Yellow "2026-02-07 14:23:45" Cyan "] " \
             Blue "INFO" LightGray " - " \
             White "Request from " Magenta "192.168.1.100" White " processed in " \
             LightGreen "42ms"
echo ""

# Color mixing
echo "7. Syntax Highlighting Simulation:"
$PRINTC -n Blue "func " Cyan "main" White "() " Yellow "int32 " White "{"
$PRINTC -n White "    " Magenta "return " LightYellow "0" White ";"
$PRINTC -n White "}"
echo ""

echo ""
echo "=== Demo Complete ==="
