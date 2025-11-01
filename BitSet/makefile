# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -O2

# Directories
SRC_DIR = ./src
INC_DIR = ./inc
LIB_DIR = ./lib
OBJ_DIR = ./obj
BIN_DIR = ./bin
TST_DIR = ./tst

# Find all source files
SRCS = $(wildcard $(SRC_DIR)/*.c)

# Generate object files from source files
OBJS = $(SRCS:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

# Include directories
INCLUDES = -I$(INC_DIR)

# Libraries
LIBS = -L$(LIB_DIR) -lm

# Default target
TARGET=$(OBJS) test
all: $(TARGET)

# Compile source files into object files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# Compile test source files into test executables
TEST_SRCS = $(wildcard $(TST_DIR)/src/*.c)
TEST_OBJS = $(TEST_SRCS:$(TST_DIR)/src/%.c=$(TST_DIR)/bin/%)
$(TST_DIR)/bin/%: $(TST_DIR)/src/%.c
	@mkdir -p $(TST_DIR)/bin
	$(CC) $(CFLAGS) $(INCLUDES) $< -o $@ $(OBJS) $(LIBS)

test: $(TEST_OBJS)
	@for test in $(TEST_OBJS); do \
		$$test; \
	done
	

# Clean up build files
clean:
	rm -rf $(OBJ_DIR)/* $(BIN_DIR)/* $(TST_DIR)/bin/*

.PHONY: all clean
