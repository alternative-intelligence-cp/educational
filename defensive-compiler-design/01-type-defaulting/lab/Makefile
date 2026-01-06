# Makefile for Type Defaulting Lab
# Builds both vulnerable and safe compiler versions

CXX = g++
CXXFLAGS = -std=c++11 -Wall -Wextra
CXXFLAGS_DEBUG = $(CXXFLAGS) -g -O0
CXXFLAGS_RELEASE = $(CXXFLAGS) -O2

.PHONY: all vulnerable safe clean test help

# Default target
all: vulnerable safe

# Vulnerable compiler (with bug)
vulnerable: vulnerable_compiler.cpp
	@echo "Building vulnerable compiler (with type defaulting bug)..."
	$(CXX) $(CXXFLAGS_RELEASE) -o vulnerable_compiler vulnerable_compiler.cpp
	@echo "✓ Built: ./vulnerable_compiler"
	@echo ""

# Safe compiler (with fix)
safe: safe_compiler.cpp
	@echo "Building safe compiler (with defensive fix)..."
	$(CXX) $(CXXFLAGS_RELEASE) -o safe_compiler safe_compiler.cpp
	@echo "✓ Built: ./safe_compiler"
	@echo ""

# Debug versions (for GDB)
vulnerable-debug: vulnerable_compiler.cpp
	@echo "Building vulnerable compiler (debug mode)..."
	$(CXX) $(CXXFLAGS_DEBUG) -o vulnerable_compiler vulnerable_compiler.cpp
	@echo "✓ Built: ./vulnerable_compiler (with debug symbols)"
	@echo "Run with: gdb ./vulnerable_compiler"
	@echo ""

safe-debug: safe_compiler.cpp
	@echo "Building safe compiler (debug mode)..."
	$(CXX) $(CXXFLAGS_DEBUG) -o safe_compiler safe_compiler.cpp
	@echo "✓ Built: ./safe_compiler (with debug symbols)"
	@echo ""

# Run tests
test: vulnerable safe
	@echo "Running test suite..."
	@chmod +x test.sh
	@./test.sh

# Clean build artifacts
clean:
	@echo "Cleaning build artifacts..."
	@rm -f vulnerable_compiler safe_compiler
	@echo "✓ Clean"

# Help message
help:
	@echo "Type Defaulting Lab - Makefile"
	@echo ""
	@echo "Available targets:"
	@echo "  make vulnerable       - Build vulnerable compiler (with bug)"
	@echo "  make safe             - Build safe compiler (with fix)"
	@echo "  make all              - Build both versions (default)"
	@echo "  make vulnerable-debug - Build vulnerable with debug symbols"
	@echo "  make safe-debug       - Build safe with debug symbols"
	@echo "  make test             - Build and run test suite"
	@echo "  make clean            - Remove build artifacts"
	@echo "  make help             - Show this message"
	@echo ""
	@echo "Quick start:"
	@echo "  1. make all"
	@echo "  2. make test"
	@echo "  3. See the difference!"
	@echo ""
	@echo "For debugging:"
	@echo "  make vulnerable-debug"
	@echo "  gdb ./vulnerable_compiler"
	@echo ""
