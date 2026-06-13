CC = gcc
CFLAGS = -Wall -Wextra -g -O0

SRCS = main.c green_thread.c context_switch.S
OBJS = $(SRCS:.c=.o)
OBJS := $(OBJS:.S=.o)

TARGET = green_threads_demo

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

%.o: %.S
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)
