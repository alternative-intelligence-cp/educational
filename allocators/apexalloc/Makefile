# ApexAlloc Makefile
# Educational memory allocator

CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -Werror \
         -fPIC \
         -fvisibility=hidden \
         -fno-builtin-malloc -fno-builtin-free \
         -fno-builtin-calloc -fno-builtin-realloc \
         -O2 -march=native

DEBUG_FLAGS = -g -O0 -DDEBUG -fsanitize=address -fsanitize=undefined
LDFLAGS = -shared -pthread
DEBUG_LDFLAGS = $(LDFLAGS) -fsanitize=address -fsanitize=undefined

# Directories
SRC_DIR = src
INC_DIR = include
BUILD_DIR = build
TEST_DIR = tests
BENCH_DIR = benchmarks

# Output
LIB = $(BUILD_DIR)/libapexalloc.so
LIB_DEBUG = $(BUILD_DIR)/libapexalloc_debug.so

# Source files
SRCS = $(wildcard $(SRC_DIR)/*.c)
OBJS = $(SRCS:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)
OBJS_DEBUG = $(SRCS:$(SRC_DIR)/%.c=$(BUILD_DIR)/%_debug.o)

# Test files
TEST_SRCS = $(wildcard $(TEST_DIR)/*.c)
TEST_BINS = $(TEST_SRCS:$(TEST_DIR)/%.c=$(BUILD_DIR)/test_%)

# Benchmark files
BENCH_SRCS = $(wildcard $(BENCH_DIR)/*.c)
BENCH_BINS = $(BENCH_SRCS:$(BENCH_DIR)/%.c=$(BUILD_DIR)/bench_%)

.PHONY: all clean test bench debug help

all: $(BUILD_DIR) $(LIB)

debug: $(BUILD_DIR) $(LIB_DEBUG)

# Create build directory
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Build shared library (release)
$(LIB): $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $^
	@echo "✅ Built release library: $@"

# Build shared library (debug)
$(LIB_DEBUG): $(OBJS_DEBUG)
	$(CC) $(DEBUG_LDFLAGS) -o $@ $^
	@echo "✅ Built debug library: $@"

# Compile object files (release)
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -I$(INC_DIR) -c $< -o $@

# Compile object files (debug)
$(BUILD_DIR)/%_debug.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) $(DEBUG_FLAGS) -I$(INC_DIR) -c $< -o $@

# Build tests
test: $(LIB) $(TEST_BINS)
	@echo "🧪 Running tests..."
	@for test in $(TEST_BINS); do \
		echo "Running $$test..."; \
		LD_PRELOAD=$(LIB) $$test || exit 1; \
	done
	@echo "✅ All tests passed!"

$(BUILD_DIR)/test_%: $(TEST_DIR)/%.c $(LIB)
	$(CC) $(CFLAGS) -I$(INC_DIR) $< -o $@ -L$(BUILD_DIR) -lapexalloc -pthread

# Build benchmarks
bench: $(LIB) $(BENCH_BINS)
	@echo "📊 Running benchmarks..."
	@for bench in $(BENCH_BINS); do \
		echo "Running $$bench..."; \
		LD_PRELOAD=$(LIB) $$bench; \
	done

$(BUILD_DIR)/bench_%: $(BENCH_DIR)/%.c $(LIB)
	$(CC) $(CFLAGS) -I$(INC_DIR) $< -o $@ -L$(BUILD_DIR) -lapexalloc -pthread

# Clean
clean:
	rm -rf $(BUILD_DIR)
	@echo "🧹 Cleaned build directory"

# Help
help:
	@echo "ApexAlloc Build System"
	@echo "====================="
	@echo ""
	@echo "Targets:"
	@echo "  make          - Build release library"
	@echo "  make debug    - Build debug library with sanitizers"
	@echo "  make test     - Build and run tests"
	@echo "  make bench    - Build and run benchmarks"
	@echo "  make clean    - Remove build artifacts"
	@echo "  make help     - Show this help"
	@echo ""
	@echo "Usage with LD_PRELOAD:"
	@echo "  LD_PRELOAD=./build/libapexalloc.so ./your_program"
