#!/bin/bash
# Test script for Type Defaulting bug
# Runs both vulnerable and safe compilers to demonstrate the difference

set -e  # Exit on error (except where we expect errors)

echo "========================================"
echo "Lab 01: Type Defaulting Bug Test"
echo "========================================"
echo ""

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Test vulnerable compiler
echo "========================================" 
echo "Part 1: Testing VULNERABLE Compiler"
echo "========================================"
echo ""

if [ -f ./vulnerable_compiler ]; then
    echo "Running vulnerable compiler..."
    echo ""
    
    # This should crash
    set +e  # Don't exit on error
    ./vulnerable_compiler test_program.aria
    VULN_EXIT=$?
    set -e
    
    echo ""
    if [ $VULN_EXIT -ne 0 ]; then
        echo -e "${RED}✗ Vulnerable compiler CRASHED (as expected)${NC}"
        echo -e "${RED}  Stack corruption caused segmentation fault${NC}"
        VULN_RESULT="CRASHED"
    else
        echo -e "${YELLOW}⚠ Vulnerable compiler succeeded (unexpected!)${NC}"
        VULN_RESULT="SUCCEEDED"
    fi
else
    echo -e "${YELLOW}⚠ Vulnerable compiler not built yet${NC}"
    echo "  Run: make vulnerable"
    VULN_RESULT="NOT_BUILT"
fi

echo ""
echo ""

# Test safe compiler  
echo "========================================"
echo "Part 2: Testing SAFE Compiler"
echo "========================================"
echo ""

if [ -f ./safe_compiler ]; then
    echo "Running safe compiler..."
    echo ""
    
    # This should fail cleanly with error message
    set +e  # Don't exit on error
    ./safe_compiler test_program.aria
    SAFE_EXIT=$?
    set -e
    
    echo ""
    if [ $SAFE_EXIT -ne 0 ]; then
        echo -e "${GREEN}✓ Safe compiler FAILED CLEANLY (as expected)${NC}"
        echo -e "${GREEN}  Explicit error message, no corruption${NC}"
        SAFE_RESULT="FAILED_CLEANLY"
    else
        echo -e "${YELLOW}⚠ Safe compiler succeeded (should have failed!)${NC}"
        SAFE_RESULT="SUCCEEDED"
    fi
else
    echo -e "${YELLOW}⚠ Safe compiler not built yet${NC}"
    echo "  Run: make safe"
    SAFE_RESULT="NOT_BUILT"
fi

echo ""
echo ""

# Summary
echo "========================================"
echo "Test Summary"
echo "========================================"
echo ""

echo "Vulnerable Compiler: $VULN_RESULT"
echo "Safe Compiler: $SAFE_RESULT"
echo ""

# Overall result
if [ "$VULN_RESULT" = "CRASHED" ] && [ "$SAFE_RESULT" = "FAILED_CLEANLY" ]; then
    echo -e "${GREEN}========================================${NC}"
    echo -e "${GREEN}✓ ALL TESTS PASSED!${NC}"
    echo -e "${GREEN}========================================${NC}"
    echo ""
    echo "The defensive fix works correctly:"
    echo "  - Vulnerable version: Silent default → Stack corruption → Crash"
    echo "  - Safe version: Explicit error → No corruption → Clean failure"
    echo ""
    echo -e "${GREEN}The pattern is clear: Fail explicitly, never guess!${NC}"
    exit 0
else
    echo -e "${YELLOW}========================================${NC}"
    echo -e "${YELLOW}⚠ UNEXPECTED RESULTS${NC}"
    echo -e "${YELLOW}========================================${NC}"
    echo ""
    echo "Expected:"
    echo "  - Vulnerable version should crash (stack corruption)"
    echo "  - Safe version should fail cleanly (explicit error)"
    echo ""
    echo "Actual:"
    echo "  - Vulnerable: $VULN_RESULT"
    echo "  - Safe: $SAFE_RESULT"
    echo ""
    
    if [ "$VULN_RESULT" = "NOT_BUILT" ] || [ "$SAFE_RESULT" = "NOT_BUILT" ]; then
        echo "Make sure to build both compilers:"
        echo "  make vulnerable"
        echo "  make safe"
    fi
    
    exit 1
fi
