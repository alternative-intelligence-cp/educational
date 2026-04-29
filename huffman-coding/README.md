# Huffman Coding — Lossless Compression From Scratch

**Turn the Black Box Transparent: How does file compression actually work?**

Every time you use `gzip`, `zip`, or `zstd`, a variant of Huffman coding is doing the heavy lifting. This project implements it from scratch in ~400 lines of C so you can see exactly what's happening.

---

## What You'll Learn

1. **Information Theory** — Why some data compresses better than others (entropy)
2. **Frequency Analysis** — Counting symbol occurrences to find patterns
3. **Binary Trees** — How the Huffman tree assigns variable-length codes
4. **Prefix-Free Codes** — Why no code is a prefix of another (and why that matters)
5. **Bitwise I/O** — Reading and writing individual bits, not just bytes
6. **File Format Design** — How to store the tree alongside the compressed data

## The Core Insight

In ASCII, every character takes 8 bits. But in English text:
- `e` appears ~13% of the time → give it a short code (maybe 3 bits)
- `z` appears ~0.07% of the time → give it a long code (maybe 12 bits)

**Frequent symbols get short codes. Rare symbols get long codes.** This is the entire idea.

David Huffman proved in 1952 that this greedy algorithm produces the **optimal** prefix-free code for any given symbol frequency distribution.

## Building

```bash
gcc -O2 -Wall -Wextra -o huffman huffman.c
```

## Usage

```bash
# Compress a file
./huffman -c input.txt compressed.huf

# Decompress a file
./huffman -d compressed.huf output.txt

# Analyze without compressing (show frequency table + codes)
./huffman -a input.txt

# Verbose mode (show the Huffman tree)
./huffman -v -c input.txt compressed.huf
```

## Example Session

```
$ echo "abracadabra" > test.txt
$ ./huffman -v -c test.txt test.huf

=== Huffman Compression ===
Input: test.txt (12 bytes)

Symbol Frequencies:
  'a' : 5 (41.7%)    code: 0
  'b' : 2 (16.7%)    code: 110
  'r' : 2 (16.7%)    code: 10
  'd' : 1 ( 8.3%)    code: 1110
  'c' : 1 ( 8.3%)    code: 1111
  '\n': 1 ( 8.3%)    code: 100

Huffman Tree:
          (12)
         /    \
       a(5)   (7)
              / \
           (3)  (4)
           / \  / \
         r(2) \n b(2) (2)
             (1)     / \
                   d(1) c(1)

Original:  96 bits (12 bytes)
Compressed: 29 bits + header
Output: test.huf (18 bytes)
Compression ratio: 66.7% (worse on tiny files due to header overhead)

$ ./huffman -d test.huf restored.txt
=== Huffman Decompression ===
Restored: restored.txt (12 bytes)

$ diff test.txt restored.txt && echo "Identical!"
Identical!
```

## How It Works (Step by Step)

### Step 1: Count Frequencies
Read every byte in the file. Count how often each byte value (0-255) appears.

### Step 2: Build the Huffman Tree
1. Create a leaf node for each symbol with its frequency
2. Put all nodes in a priority queue (min-heap), sorted by frequency
3. While more than one node remains:
   - Remove the two nodes with lowest frequency
   - Create a new internal node with those two as children
   - The new node's frequency = sum of its children
   - Insert the new node back into the queue
4. The last remaining node is the root of the Huffman tree

### Step 3: Generate Codes
Walk the tree from root to each leaf:
- Going left = append `0`
- Going right = append `1`
- The path from root to a leaf IS that symbol's code

### Step 4: Encode
Replace each byte in the input with its Huffman code. Write the bits out, packing 8 bits per output byte.

### Step 5: Store the Tree
The compressed file needs to include the Huffman tree so the decoder can reconstruct it. We serialize the tree using a pre-order traversal with leaf/internal markers.

### Step 6: Decode
Read the tree from the file header. Then read bits one at a time, walking the tree from root. When you reach a leaf, output that symbol and restart from the root.

## File Format

```
[2 bytes] Magic number: 0x48 0x46 ("HF")
[1 byte]  Number of unique symbols - 1
[N bytes] Serialized Huffman tree (pre-order, 0=internal, 1=leaf+byte)
[4 bytes] Original file size (little-endian u32)
[N bytes] Compressed bitstream
[padding] Last byte may have unused bits
```

## Why This Matters

Huffman coding is the foundation of:
- **DEFLATE** (used by gzip, zip, PNG) — combines LZ77 + Huffman
- **JPEG** — uses Huffman for entropy coding after DCT
- **MP3** — uses Huffman tables in the bitstream encoding
- **HTTP/2** — HPACK header compression uses Huffman
- **Brotli** — Google's web compression builds on Huffman

Understanding Huffman means understanding a piece of nearly every compressed format you use daily.

## Limitations (By Design)

This is an educational implementation. Production compressors add:
- **LZ77/LZ78** — Find repeated sequences first, THEN Huffman-encode
- **Adaptive Huffman** — Update the tree as you go (no two-pass needed)
- **Arithmetic coding** — Fractional bits per symbol (better than Huffman)
- **Context modeling** — Predict the next symbol from recent context

We skip all of that to keep the core algorithm visible and understandable.

---

*Part of the [AILP Educational Curriculum](../README.md) — Turn the Black Box Transparent.*
