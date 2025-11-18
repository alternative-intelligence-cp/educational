# Makefile for Educational Memory Allocator
# ==========================================

CC = gcc
CFLAGS = -std=c99 -Wall -Wextra -g -O2
TARGET = allocator_demo
SOURCES = improved_allocator.c allocator_test_suite.c

# Default target
all: $(TARGET)

# Build the demo
$(TARGET): $(SOURCES)
	$(CC) $(CFLAGS) -o $(TARGET) $(SOURCES)
	@echo "✅ Educational allocator compiled successfully!"

# Run tests
test: $(TARGET)
	@echo "🚀 Running allocator tests..."
	./$(TARGET)

# Run with memory debugging
debug: $(SOURCES)
	$(CC) $(CFLAGS) -DDEBUG -fsanitize=address -o $(TARGET)_debug $(SOURCES)
	@echo "🐛 Debug version compiled with AddressSanitizer"
	./$(TARGET)_debug

# Clean build artifacts
clean:
	rm -f $(TARGET) $(TARGET)_debug
	@echo "🧹 Cleaned build artifacts"

# Generate documentation (requires doxygen)
docs:
	doxygen Doxyfile 2>/dev/null || echo "📚 Install doxygen to generate documentation"

# Performance benchmark
bench: $(TARGET)
	@echo "⚡ Running performance benchmark..."
	time ./$(TARGET)

# Help
help:
	@echo "Educational Memory Allocator Build System"
	@echo "========================================="
	@echo "Targets:"
	@echo "  all     - Build the allocator demo (default)"
	@echo "  test    - Build and run tests"
	@echo "  debug   - Build with AddressSanitizer and run"
	@echo "  clean   - Remove build artifacts"
	@echo "  docs    - Generate documentation (requires doxygen)"
	@echo "  bench   - Run performance benchmark"
	@echo "  help    - Show this help message"

.PHONY: all test debug clean docs bench help