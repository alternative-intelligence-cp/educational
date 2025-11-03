# Minimal Makefile for LibAI Agent Playground
CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -O2

INCLUDES = -I../s/LibAI_OS -I../s/LibAI_OS/kernel -I../s/LibAI_OS/libraries -I../src/allocator

# Sources from the original ShaggyMem project
SHAGGY_SOURCES = \
    ../src/allocator/sm_consciousness_enhancements_simple.c \
    ../src/allocator/sm_allocator.c \
    ../src/allocator/sm_allocator_sysmem.c

all: minimal_client

minimal_client: minimal_client.c libai_wrapper.c $(SHAGGY_SOURCES)
	$(CC) $(CFLAGS) $(INCLUDES) minimal_client.c libai_wrapper.c $(SHAGGY_SOURCES) -o minimal_client

clean:
	rm -f minimal_client

.PHONY: all clean
