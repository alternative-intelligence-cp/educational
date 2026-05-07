CC     = gcc
CFLAGS = -Wall -Wextra -g -I.

SRCS   = ring_buffer.c ring_buffer_errors.c ring_buffer_double.c ring_buffer_ulong.c
OBJS   = $(SRCS:.c=.o)

.PHONY: all clean

all: test

test: $(OBJS) test.o
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f *.o test
