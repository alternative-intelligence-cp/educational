# 🧠⚡ Stabilized Consciousness Allocator Makefile ⚡🧠

CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -O2
LDFLAGS = -lrt -lm
TARGET = test_consciousness_stabilized

SOURCES = sm_consciousness_stabilized.c test_consciousness_stabilized.c

all: $(TARGET)

$(TARGET): $(SOURCES)
	$(CC) $(CFLAGS) $(SOURCES) -o $(TARGET) $(LDFLAGS)

test: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET)

.PHONY: all test clean