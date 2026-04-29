CC = gcc
CFLAGS = -O2 -Wall -Wextra -pedantic
LDFLAGS = -lm

huffman: huffman.c
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

test: huffman
	@echo "=== Round-trip test: text ==="
	@echo "abracadabra" > /tmp/huff_test.txt
	@./huffman -c /tmp/huff_test.txt /tmp/huff_test.huf
	@./huffman -d /tmp/huff_test.huf /tmp/huff_restored.txt
	@diff /tmp/huff_test.txt /tmp/huff_restored.txt && echo "PASS" || echo "FAIL"
	@echo ""
	@echo "=== Round-trip test: source code ==="
	@./huffman -c huffman.c /tmp/huff_src.huf
	@./huffman -d /tmp/huff_src.huf /tmp/huff_src_restored.c
	@diff huffman.c /tmp/huff_src_restored.c && echo "PASS" || echo "FAIL"
	@echo ""
	@echo "=== Round-trip test: binary ==="
	@./huffman -c huffman /tmp/huff_bin.huf
	@./huffman -d /tmp/huff_bin.huf /tmp/huff_bin_restored
	@diff huffman /tmp/huff_bin_restored && echo "PASS" || echo "FAIL"
	@echo ""
	@echo "=== Analysis demo ==="
	@./huffman -a huffman.c 2>&1 | head -12

clean:
	rm -f huffman

.PHONY: test clean
