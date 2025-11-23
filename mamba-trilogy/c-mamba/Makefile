# 🔥 High-Performance C Mamba Makefile 🔥
# Optimized build configuration for maximum performance

CC = gcc
CFLAGS = -std=c99 -Wall -Wextra -O3 -march=native -ffast-math
LDFLAGS = -lm -lpthread

# Debug build
DEBUG_CFLAGS = -std=c99 -Wall -Wextra -g -O0 -DDEBUG
DEBUG_LDFLAGS = -lm -lpthread

# Profiling build  
PROFILE_CFLAGS = -std=c99 -Wall -Wextra -O3 -march=native -g -pg
PROFILE_LDFLAGS = -lm -lpthread -pg

# Source files
SOURCES = high_performance_mamba.c
EXECUTABLE = mamba_performance_test

# Default target
all: $(EXECUTABLE)

# Performance build (default)
$(EXECUTABLE): $(SOURCES)
	@echo "🔥 Building high-performance C Mamba..."
	$(CC) $(CFLAGS) $(SOURCES) -o $(EXECUTABLE) $(LDFLAGS)
	@echo "✅ Build complete: $(EXECUTABLE)"

# Debug build
debug: $(SOURCES)
	@echo "🐛 Building debug version..."
	$(CC) $(DEBUG_CFLAGS) $(SOURCES) -o $(EXECUTABLE)_debug $(DEBUG_LDFLAGS)
	@echo "✅ Debug build complete: $(EXECUTABLE)_debug"

# Profile build
profile: $(SOURCES)
	@echo "📊 Building profile version..."
	$(CC) $(PROFILE_CFLAGS) $(SOURCES) -o $(EXECUTABLE)_profile $(PROFILE_LDFLAGS)
	@echo "✅ Profile build complete: $(EXECUTABLE)_profile"

# Run performance test
test: $(EXECUTABLE)
	@echo "🚀 Running performance tests..."
	./$(EXECUTABLE)

# Run debug test
test-debug: debug
	@echo "🐛 Running debug tests..."
	./$(EXECUTABLE)_debug

# Run with profiler
profile-run: profile
	@echo "📊 Running with profiler..."
	./$(EXECUTABLE)_profile
	gprof $(EXECUTABLE)_profile gmon.out > profile_analysis.txt
	@echo "📋 Profile results saved to: profile_analysis.txt"

# Memory check with valgrind
memcheck: debug
	@echo "🔍 Running memory check..."
	valgrind --leak-check=full --show-leak-kinds=all ./$(EXECUTABLE)_debug

# Performance analysis with perf
perf-analysis: $(EXECUTABLE)
	@echo "⚡ Running performance analysis..."
	perf record -g ./$(EXECUTABLE)
	perf report

# Clean build artifacts
clean:
	@echo "🧹 Cleaning build artifacts..."
	rm -f $(EXECUTABLE) $(EXECUTABLE)_debug $(EXECUTABLE)_profile
	rm -f gmon.out profile_analysis.txt perf.data*
	@echo "✅ Clean complete"

# Show compiler and system info
info:
	@echo "🔧 Compiler Information:"
	@echo "========================"
	@$(CC) --version
	@echo ""
	@echo "🖥️  System Information:"
	@echo "======================"
	@uname -a
	@echo ""
	@echo "🎯 CPU Features:"
	@echo "==============="
	@cat /proc/cpuinfo | grep "flags" | head -1
	@echo ""
	@echo "💾 Memory Information:"
	@echo "====================="
	@cat /proc/meminfo | grep "MemTotal"

# Benchmark against Python
benchmark-comparison: $(EXECUTABLE)
	@echo "📊 Running C vs Python benchmark..."
	@echo "C Performance:"
	@echo "=============="
	./$(EXECUTABLE)
	@echo ""
	@echo "Python Performance:"
	@echo "=================="
	cd ../python-mamba && python -c "from educational_mamba import *; import time; \
	config = MambaConfig(d_model=128, d_state=16); \
	mamba = EducationalMamba(config); \
	x = np.random.randn(4, 100, 128) * 0.1; \
	start = time.time(); \
	y = mamba.forward(x); \
	print(f'  Medium batch: {time.time()-start:.3f}s ({400/(time.time()-start):.0f} tok/s)')"

# Help target
help:
	@echo "🧠⚡ High-Performance C Mamba Build System ⚡🧠"
	@echo "=============================================="
	@echo ""
	@echo "Targets:"
	@echo "  all              - Build optimized executable (default)"
	@echo "  debug            - Build debug version with symbols"
	@echo "  profile          - Build version with profiling enabled"
	@echo "  test             - Run performance tests"
	@echo "  test-debug       - Run debug version"
	@echo "  profile-run      - Run with profiling analysis"
	@echo "  memcheck         - Run memory leak detection"
	@echo "  perf-analysis    - Run performance profiling with perf"
	@echo "  benchmark-comparison - Compare C vs Python performance"
	@echo "  clean            - Remove build artifacts"
	@echo "  info             - Show compiler and system information"
	@echo "  help             - Show this help message"
	@echo ""
	@echo "Examples:"
	@echo "  make              # Build optimized version"
	@echo "  make test         # Build and run tests"
	@echo "  make debug test-debug  # Build and test debug version"
	@echo "  make benchmark-comparison  # Compare with Python"

.PHONY: all debug profile test test-debug profile-run memcheck perf-analysis clean info benchmark-comparison help