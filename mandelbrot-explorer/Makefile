# Mandelbrot Explorer — Makefile
# Just run 'make' to build, 'make run' to build and launch.

CC      = gcc
CFLAGS  = -O2 -Wall -Wextra -std=c11
LDFLAGS = -lm
TARGET  = mandelbrot

.PHONY: all clean run

all: $(TARGET)

$(TARGET): mandelbrot.c
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET)
