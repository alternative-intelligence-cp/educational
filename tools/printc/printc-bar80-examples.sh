#!/bin/sh
# printc + BAR80 Integration Examples
# Demonstrates combining printc with BAR80-style separators for polished terminal output

PRINTC="./printc"

# BAR80-style separator variables (80 characters each)
BAR_HASH='################################################################################'
BAR_EQUL='================================================================================'
BAR_DASH='--------------------------------------------------------------------------------'
BAR_ASTR='********************************************************************************'

# Function to center text (simple version for demo)
center_text() {
    text="$1"
    width=78  # 80 minus 2 for border chars
    len=${#text}
    if [ $len -ge $width ]; then
        echo "$text"
        return
    fi
    pad=$(( (width - len) / 2 ))
    printf "%${pad}s%s%${pad}s" "" "$text" ""
}

echo ""
echo "╔════════════════════════════════════════════════════════════════════════════════╗"
echo "║           printc + BAR80 Integration Examples                                  ║"
echo "╚════════════════════════════════════════════════════════════════════════════════╝"
echo ""

# Example 1: Colored headers with bars
echo "Example 1: Colored Section Headers"
echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
$PRINTC -n 13 "$BAR_HASH"
centered=$(center_text "BUILD STATUS REPORT")
$PRINTC -n 13 "#" 16 " $centered " 13 "#"
$PRINTC -n 13 "$BAR_HASH"
echo ""
echo ""

# Example 2: Color-coded status sections
echo "Example 2: Status Indicators with Sections"
echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
$PRINTC -n 10 "$BAR_EQUL"
centered=$(center_text "COMPILATION RESULTS")
$PRINTC -n 10 "=" 16 " $centered " 10 "="
$PRINTC -n 10 "$BAR_EQUL"
$PRINTC -n 11 "  [✓] " 8 "main.c compiled successfully"
$PRINTC -n 11 "  [✓] " 8 "utils.c compiled successfully"
$PRINTC -n 2 "  [✗] " 8 "tests.c compilation failed"
$PRINTC -n 12 "  [!] " 8 "3 warnings generated"
$PRINTC -n 10 "$BAR_EQUL"
echo ""
echo ""

# Example 3: Log levels with colored separators
echo "Example 3: Log Output with Color-Coded Levels"
echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
$PRINTC -n 15 "$BAR_ASTR"
centered=$(center_text "APPLICATION LOGS")
$PRINTC -n 15 "*" 13 " $centered " 15 "*"
$PRINTC -n 15 "$BAR_ASTR"
$PRINTC -n 9 "[DEBUG] " 8 "Initializing database connection pool"
$PRINTC -n 5 "[INFO]  " 16 "Server started on port 8080"
$PRINTC -n 12 "[WARN]  " 8 "Cache hit ratio below 50%"
$PRINTC -n 2 "[ERROR] " 16 "Failed to connect to external API"
$PRINTC -n 15 "$BAR_ASTR"
echo ""
echo ""

# Example 4: Test results with numeric colors (faster typing)
echo "Example 4: Test Suite Results (using numeric color shortcuts)"
echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
$PRINTC -n 7 "$BAR_DASH"
centered=$(center_text "TEST EXECUTION SUMMARY")
$PRINTC -n 7 "-" 16 " $centered " 7 "-"
$PRINTC -n 7 "$BAR_DASH"
$PRINTC -n 11 "✓ " 8 "test_addition          " 9 "(12ms)"
$PRINTC -n 11 "✓ " 8 "test_subtraction       " 9 "(8ms)"
$PRINTC -n 2 "✗ " 8 "test_division          " 9 "(failed: division by zero)"
$PRINTC -n 11 "✓ " 8 "test_multiplication    " 9 "(15ms)"
$PRINTC -n 12 "⊙ " 8 "test_integration       " 9 "(skipped)"
echo ""
$PRINTC -n 8 "  Summary: " 11 "3 passed" 8 ", " 2 "1 failed" 8 ", " 12 "1 skipped" 8 " (35ms total)"
$PRINTC -n 7 "$BAR_DASH"
echo ""
echo ""

# Example 5: Multi-color progress indicator
echo "Example 5: Installation Progress"
echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
$PRINTC -n 13 "$BAR_HASH"
centered=$(center_text "PACKAGE INSTALLATION")
$PRINTC -n 13 "#" 16 " $centered " 13 "#"
$PRINTC -n 13 "$BAR_HASH"
$PRINTC -n 7 "  [1/5] " 11 "✓ " 8 "Downloading packages"
$PRINTC -n 7 "  [2/5] " 11 "✓ " 8 "Verifying checksums"
$PRINTC -n 7 "  [3/5] " 11 "✓ " 8 "Extracting archives"
$PRINTC -n 7 "  [4/5] " 12 "⟳ " 8 "Installing files..."
$PRINTC -n 9 "  [5/5] " 9 "○ " 9 "Pending: Configure system"
$PRINTC -n 13 "$BAR_HASH"
echo ""
echo ""

# Example 6: Diff-style output
echo "Example 6: Diff-Style Code Changes"
echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
$PRINTC -n 6 "$BAR_EQUL"
centered=$(center_text "FILE: config.sh")
$PRINTC -n 6 "=" 16 " $centered " 6 "="
$PRINTC -n 6 "$BAR_EQUL"
$PRINTC -n 8 "  10:  " 16 "export PATH=\"/usr/local/bin:\$PATH\""
$PRINTC -n 2 "  11:- " 9 "export DEBUG=false"
$PRINTC -n 3 "  11:+ " 16 "export DEBUG=true"
$PRINTC -n 8 "  12:  " 16 "source ~/.profile"
$PRINTC -n 6 "$BAR_EQUL"
echo ""
echo ""

# Example 7: Boxed alerts
echo "Example 7: Important Notifications"
echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
$PRINTC -n 12 "╔══════════════════════════════════════════════════════════════════════════════╗"
$PRINTC -n 12 "║" 16 "  ⚠  WARNING: DEPRECATED API                                                  " 12 "║"
$PRINTC -n 12 "║" 8 "  The function 'getConfig()' is deprecated and will be removed in v3.0        " 12 "║"
$PRINTC -n 12 "║" 8 "  Please migrate to 'loadConfiguration()' instead.                            " 12 "║"
$PRINTC -n 12 "╚══════════════════════════════════════════════════════════════════════════════╝"
echo ""

echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
echo ""
$PRINTC -n 13 "💡 TIP: " 8 "Use numeric colors (0-16) for faster typing in scripts!"
echo ""
$PRINTC -n 8 "   Examples: " 11 "printc 11 \"[OK]\"" 8 " is much faster than " 11 "printc LightGreen \"[OK]\""
echo ""
