CC      := gcc
CFLAGS  := -Wall -Wextra -Wpedantic -Werror -std=c11 -g
TARGET  := lesson_demo
TEST_TARGET := tests/test_lesson_string

.PHONY: all build run test clean

all: build run

build: $(TARGET)

test: $(TEST_TARGET)
	./$(TEST_TARGET)

$(TARGET): lesson_demo.o lesson_string.o
	$(CC) $(CFLAGS) -o $@ $^

lesson_demo.o: lesson_demo.c lesson_string.h
	$(CC) $(CFLAGS) -c $< -o $@

lesson_string.o: lesson_string.c lesson_string.h
	$(CC) $(CFLAGS) -c $< -o $@

tests/test_lesson_string.o: tests/test_lesson_string.c lesson_string.h
	$(CC) $(CFLAGS) -I. -c $< -o $@

$(TEST_TARGET): tests/test_lesson_string.o lesson_string.o
	$(CC) $(CFLAGS) -o $@ $^

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f *.o tests/*.o $(TARGET) $(TEST_TARGET)
