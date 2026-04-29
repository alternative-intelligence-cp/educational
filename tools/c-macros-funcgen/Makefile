CC      := gcc
# Note: -Wpedantic is intentionally omitted.  The Layer 2/3 macros generate
# function definitions that GCC allows inside other functions as an extension.
# All other warnings are still treated as errors.
CFLAGS  := -Wall -Wextra -Werror -std=c11 -g
TARGET  := macro_funcgen_demo
TEST_TARGET := tests/test_macro_funcgen

.PHONY: all build run test clean

all: build run

build: $(TARGET)

test: $(TEST_TARGET)
	./$(TEST_TARGET)

$(TARGET): macro_funcgen_demo.o
	$(CC) $(CFLAGS) -o $@ $^

macro_funcgen_demo.o: macro_funcgen_demo.c macro_funcgen.h
	$(CC) $(CFLAGS) -c $< -o $@

tests/test_macro_funcgen.o: tests/test_macro_funcgen.c macro_funcgen.h
	$(CC) $(CFLAGS) -I. -c $< -o $@

$(TEST_TARGET): tests/test_macro_funcgen.o
	$(CC) $(CFLAGS) -o $@ $^

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f *.o tests/*.o $(TARGET) $(TEST_TARGET)
