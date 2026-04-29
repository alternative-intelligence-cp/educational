/*
 * huffman.c — Huffman Coding: Lossless Compression From Scratch
 *
 * Educational implementation. Every line is here to teach, not to optimize.
 * See README.md for the full explanation of how and why this works.
 *
 * Build:  gcc -O2 -Wall -Wextra -o huffman huffman.c
 * Usage:  ./huffman -c input.txt output.huf    (compress)
 *         ./huffman -d output.huf restored.txt  (decompress)
 *         ./huffman -a input.txt                (analyze only)
 *
 * AILP Educational Curriculum — Turn the Black Box Transparent
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <math.h>

/* -----------------------------------------------------------------------
 * SECTION 1: Data Structures
 *
 * A Huffman tree is a binary tree where:
 * - Leaf nodes hold a symbol (byte value) and its frequency
 * - Internal nodes hold the sum of their children's frequencies
 * - The tree is built bottom-up from the least frequent symbols
 * ----------------------------------------------------------------------- */

#define MAX_SYMBOLS 256   /* One for each possible byte value */
#define MAX_CODE_LEN 32   /* Huffman codes can't exceed tree depth */

typedef struct HuffNode {
    uint32_t freq;           /* How often this symbol (or subtree) appears */
    int16_t  symbol;         /* Byte value for leaves, -1 for internal nodes */
    struct HuffNode *left;   /* Left child  (represents bit 0) */
    struct HuffNode *right;  /* Right child (represents bit 1) */
} HuffNode;

/* A Huffman code: the sequence of bits that represents a symbol */
typedef struct {
    uint8_t  bits[MAX_CODE_LEN]; /* The code bits (0 or 1 per entry) */
    uint8_t  length;             /* How many bits in this code */
} HuffCode;

/* -----------------------------------------------------------------------
 * SECTION 2: Priority Queue (Min-Heap)
 *
 * To build the Huffman tree, we need to repeatedly extract the two nodes
 * with the LOWEST frequency. A min-heap gives us O(log n) extraction.
 *
 * In production you'd use a library. Here we build it from scratch
 * because understanding the heap IS the point.
 * ----------------------------------------------------------------------- */

typedef struct {
    HuffNode *nodes[MAX_SYMBOLS];
    int size;
} MinHeap;

static void heap_swap(MinHeap *h, int i, int j) {
    HuffNode *tmp = h->nodes[i];
    h->nodes[i] = h->nodes[j];
    h->nodes[j] = tmp;
}

static void heap_sift_up(MinHeap *h, int i) {
    while (i > 0) {
        int parent = (i - 1) / 2;
        if (h->nodes[i]->freq < h->nodes[parent]->freq) {
            heap_swap(h, i, parent);
            i = parent;
        } else {
            break;
        }
    }
}

static void heap_sift_down(MinHeap *h, int i) {
    while (1) {
        int smallest = i;
        int left  = 2 * i + 1;
        int right = 2 * i + 2;

        if (left < h->size && h->nodes[left]->freq < h->nodes[smallest]->freq)
            smallest = left;
        if (right < h->size && h->nodes[right]->freq < h->nodes[smallest]->freq)
            smallest = right;

        if (smallest != i) {
            heap_swap(h, i, smallest);
            i = smallest;
        } else {
            break;
        }
    }
}

static void heap_insert(MinHeap *h, HuffNode *node) {
    h->nodes[h->size] = node;
    heap_sift_up(h, h->size);
    h->size++;
}

static HuffNode *heap_extract_min(MinHeap *h) {
    HuffNode *min = h->nodes[0];
    h->size--;
    h->nodes[0] = h->nodes[h->size];
    heap_sift_down(h, 0);
    return min;
}

/* -----------------------------------------------------------------------
 * SECTION 3: Huffman Tree Construction
 *
 * The algorithm:
 * 1. Create a leaf node for each symbol that appears in the input
 * 2. Insert all leaves into the min-heap
 * 3. While heap has more than 1 node:
 *    a. Extract the two nodes with lowest frequency
 *    b. Create a new internal node with those as children
 *    c. Its frequency = sum of children's frequencies
 *    d. Insert the new node back into the heap
 * 4. The last remaining node is the root
 *
 * WHY THIS WORKS: By always combining the two rarest symbols first,
 * we ensure they end up deepest in the tree (longest codes). Frequent
 * symbols get combined last, ending up near the root (shortest codes).
 * ----------------------------------------------------------------------- */

static HuffNode *make_leaf(int symbol, uint32_t freq) {
    HuffNode *n = calloc(1, sizeof(HuffNode));
    if (!n) { fprintf(stderr, "Out of memory\n"); exit(1); }
    n->symbol = (int16_t)symbol;
    n->freq = freq;
    return n;
}

static HuffNode *make_internal(HuffNode *left, HuffNode *right) {
    HuffNode *n = calloc(1, sizeof(HuffNode));
    if (!n) { fprintf(stderr, "Out of memory\n"); exit(1); }
    n->symbol = -1;  /* -1 means internal node */
    n->freq = left->freq + right->freq;
    n->left = left;
    n->right = right;
    return n;
}

static HuffNode *build_tree(uint32_t freq[MAX_SYMBOLS], int *num_symbols) {
    MinHeap heap = {.size = 0};
    *num_symbols = 0;

    /* Create leaf nodes for every symbol that appears at least once */
    for (int i = 0; i < MAX_SYMBOLS; i++) {
        if (freq[i] > 0) {
            heap_insert(&heap, make_leaf(i, freq[i]));
            (*num_symbols)++;
        }
    }

    /* Edge case: only one unique symbol (e.g., "aaaaaaa") */
    if (heap.size == 1) {
        /* Need at least two nodes to form a tree */
        HuffNode *only = heap_extract_min(&heap);
        HuffNode *dummy = make_leaf(only->symbol == 0 ? 1 : 0, 0);
        heap_insert(&heap, make_internal(only, dummy));
        return heap.nodes[0];
    }

    /* The core loop: combine the two smallest until one remains */
    while (heap.size > 1) {
        HuffNode *left  = heap_extract_min(&heap);
        HuffNode *right = heap_extract_min(&heap);
        heap_insert(&heap, make_internal(left, right));
    }

    return heap_extract_min(&heap);
}

static void free_tree(HuffNode *node) {
    if (!node) return;
    free_tree(node->left);
    free_tree(node->right);
    free(node);
}

/* -----------------------------------------------------------------------
 * SECTION 4: Code Generation
 *
 * Walk the tree from root to every leaf. The path IS the code:
 * - Going left  = bit 0
 * - Going right = bit 1
 *
 * Because every symbol is a leaf, no code is a prefix of another.
 * This is called a "prefix-free code" and it's what makes decoding
 * unambiguous — you never have to backtrack.
 * ----------------------------------------------------------------------- */

static HuffCode codes[MAX_SYMBOLS];

static void generate_codes(HuffNode *node, uint8_t *path, int depth) {
    if (!node) return;

    if (node->symbol >= 0) {
        /* Leaf node: record the path as this symbol's code */
        codes[node->symbol].length = (uint8_t)depth;
        memcpy(codes[node->symbol].bits, path, (size_t)depth);
        return;
    }

    /* Internal node: recurse left (0) and right (1) */
    path[depth] = 0;
    generate_codes(node->left, path, depth + 1);
    path[depth] = 1;
    generate_codes(node->right, path, depth + 1);
}

/* -----------------------------------------------------------------------
 * SECTION 5: Bitwise I/O
 *
 * Huffman codes are variable-length bit sequences, but files are
 * byte-addressable. We need to pack bits into bytes for writing
 * and unpack them for reading.
 *
 * This is the part most tutorials skip, but it's crucial for
 * understanding how compressed data is actually stored.
 * ----------------------------------------------------------------------- */

typedef struct {
    FILE    *fp;
    uint8_t  buffer;     /* Accumulates bits until we have 8 */
    int      bit_count;  /* How many bits are in the buffer (0-7) */
} BitWriter;

typedef struct {
    FILE    *fp;
    uint8_t  buffer;     /* Current byte being read */
    int      bit_count;  /* How many unread bits remain in buffer */
} BitReader;

static void bw_init(BitWriter *bw, FILE *fp) {
    bw->fp = fp;
    bw->buffer = 0;
    bw->bit_count = 0;
}

static void bw_write_bit(BitWriter *bw, int bit) {
    bw->buffer = (uint8_t)((bw->buffer << 1) | (bit & 1));
    bw->bit_count++;
    if (bw->bit_count == 8) {
        fputc(bw->buffer, bw->fp);
        bw->buffer = 0;
        bw->bit_count = 0;
    }
}

static void bw_flush(BitWriter *bw) {
    /* Pad the last byte with zeros on the RIGHT */
    if (bw->bit_count > 0) {
        bw->buffer <<= (8 - bw->bit_count);
        fputc(bw->buffer, bw->fp);
        bw->buffer = 0;
        bw->bit_count = 0;
    }
}

static void br_init(BitReader *br, FILE *fp) {
    br->fp = fp;
    br->buffer = 0;
    br->bit_count = 0;
}

static int br_read_bit(BitReader *br) {
    if (br->bit_count == 0) {
        int c = fgetc(br->fp);
        if (c == EOF) return -1;
        br->buffer = (uint8_t)c;
        br->bit_count = 8;
    }
    br->bit_count--;
    return (br->buffer >> br->bit_count) & 1;
}

/* -----------------------------------------------------------------------
 * SECTION 6: Tree Serialization
 *
 * The decoder needs the Huffman tree to decompress. We serialize it
 * using a pre-order traversal:
 * - Internal node: write bit 0, recurse left then right
 * - Leaf node: write bit 1, then write the 8-bit symbol
 *
 * This is compact and self-describing. The decoder can reconstruct
 * the exact tree by reading in the same order.
 * ----------------------------------------------------------------------- */

static void serialize_tree(HuffNode *node, BitWriter *bw) {
    if (node->symbol >= 0) {
        /* Leaf: marker bit 1, then the byte value */
        bw_write_bit(bw, 1);
        for (int i = 7; i >= 0; i--)
            bw_write_bit(bw, (node->symbol >> i) & 1);
    } else {
        /* Internal: marker bit 0, then recurse */
        bw_write_bit(bw, 0);
        serialize_tree(node->left, bw);
        serialize_tree(node->right, bw);
    }
}

static HuffNode *deserialize_tree(BitReader *br) {
    int bit = br_read_bit(br);
    if (bit < 0) return NULL;

    if (bit == 1) {
        /* Leaf: read 8-bit symbol */
        int symbol = 0;
        for (int i = 0; i < 8; i++) {
            int b = br_read_bit(br);
            if (b < 0) return NULL;
            symbol = (symbol << 1) | b;
        }
        return make_leaf(symbol, 0);
    } else {
        /* Internal: recurse */
        HuffNode *left  = deserialize_tree(br);
        HuffNode *right = deserialize_tree(br);
        return make_internal(left, right);
    }
}

/* -----------------------------------------------------------------------
 * SECTION 7: Compression
 *
 * Two-pass algorithm:
 * Pass 1: Read entire file, count byte frequencies
 * Pass 2: Re-read file, encode each byte using its Huffman code
 *
 * File format:
 * [2B] Magic "HF"
 * [4B] Original file size (little-endian)
 * [tree] Serialized Huffman tree
 * [data] Compressed bitstream
 * ----------------------------------------------------------------------- */

static int compress(const char *inpath, const char *outpath, int verbose) {
    /* Pass 1: Count frequencies */
    FILE *fin = fopen(inpath, "rb");
    if (!fin) { perror(inpath); return 1; }

    uint32_t freq[MAX_SYMBOLS] = {0};
    uint32_t file_size = 0;
    int c;
    while ((c = fgetc(fin)) != EOF) {
        freq[c]++;
        file_size++;
    }

    if (file_size == 0) {
        fprintf(stderr, "Error: empty file\n");
        fclose(fin);
        return 1;
    }

    /* Build tree and generate codes */
    int num_symbols = 0;
    HuffNode *root = build_tree(freq, &num_symbols);
    uint8_t path[MAX_CODE_LEN];
    memset(codes, 0, sizeof(codes));
    generate_codes(root, path, 0);

    /* Calculate compressed size (in bits) for reporting */
    uint64_t total_bits = 0;
    for (int i = 0; i < MAX_SYMBOLS; i++) {
        if (freq[i] > 0) {
            total_bits += (uint64_t)freq[i] * codes[i].length;
        }
    }

    if (verbose) {
        printf("\n=== Huffman Compression ===\n");
        printf("Input: %s (%u bytes)\n\n", inpath, file_size);

        /* Print frequency table sorted by frequency (descending) */
        typedef struct { int sym; uint32_t f; } SymFreq;
        SymFreq sorted[MAX_SYMBOLS];
        int count = 0;
        for (int i = 0; i < MAX_SYMBOLS; i++) {
            if (freq[i] > 0) {
                sorted[count].sym = i;
                sorted[count].f = freq[i];
                count++;
            }
        }
        /* Simple insertion sort (fine for <=256 elements) */
        for (int i = 1; i < count; i++) {
            SymFreq key = sorted[i];
            int j = i - 1;
            while (j >= 0 && sorted[j].f < key.f) {
                sorted[j + 1] = sorted[j];
                j--;
            }
            sorted[j + 1] = key;
        }

        printf("Symbol Frequencies:\n");
        for (int i = 0; i < count; i++) {
            int s = sorted[i].sym;
            float pct = 100.0f * (float)sorted[i].f / (float)file_size;

            /* Pretty-print the symbol */
            if (s >= 33 && s <= 126)
                printf("  '%c' ", s);
            else if (s == ' ')
                printf("  ' ' ");
            else if (s == '\n')
                printf("  '\\n'");
            else if (s == '\t')
                printf("  '\\t'");
            else if (s == '\r')
                printf("  '\\r'");
            else
                printf("  0x%02X", s);

            printf(": %5u (%5.1f%%)    code: ", sorted[i].f, pct);
            for (int b = 0; b < codes[s].length; b++)
                putchar('0' + codes[s].bits[b]);
            putchar('\n');
        }

        printf("\nOriginal:   %lu bits (%u bytes)\n",
               (unsigned long)file_size * 8, file_size);
        printf("Compressed: %lu bits (data only)\n", (unsigned long)total_bits);
    }

    /* Pass 2: Write compressed file */
    FILE *fout = fopen(outpath, "wb");
    if (!fout) { perror(outpath); fclose(fin); free_tree(root); return 1; }

    /* Magic number */
    fputc('H', fout);
    fputc('F', fout);

    /* Original file size (4 bytes, little-endian) */
    fputc((int)(file_size & 0xFF), fout);
    fputc((int)((file_size >> 8) & 0xFF), fout);
    fputc((int)((file_size >> 16) & 0xFF), fout);
    fputc((int)((file_size >> 24) & 0xFF), fout);

    /* Serialize the Huffman tree */
    BitWriter bw;
    bw_init(&bw, fout);
    serialize_tree(root, &bw);

    /* Encode the data */
    rewind(fin);
    while ((c = fgetc(fin)) != EOF) {
        HuffCode *code = &codes[c];
        for (int i = 0; i < code->length; i++) {
            bw_write_bit(&bw, code->bits[i]);
        }
    }
    bw_flush(&bw);

    long out_size = ftell(fout);
    fclose(fin);
    fclose(fout);

    if (verbose) {
        printf("Output: %s (%ld bytes)\n", outpath, out_size);
        if (file_size > 0) {
            float ratio = 100.0f * (float)out_size / (float)file_size;
            printf("Compression ratio: %.1f%%", ratio);
            if (ratio > 100.0f)
                printf(" (larger — small files have header overhead)");
            putchar('\n');
        }
    } else {
        printf("Compressed %s → %s (%u → %ld bytes)\n",
               inpath, outpath, file_size, out_size);
    }

    free_tree(root);
    return 0;
}

/* -----------------------------------------------------------------------
 * SECTION 8: Decompression
 *
 * Read the tree from the header, then walk it bit by bit:
 * - Start at root
 * - Read a bit: 0=go left, 1=go right
 * - When you reach a leaf, output that symbol, restart from root
 * - Stop after `file_size` symbols have been output
 * ----------------------------------------------------------------------- */

static int decompress(const char *inpath, const char *outpath, int verbose) {
    FILE *fin = fopen(inpath, "rb");
    if (!fin) { perror(inpath); return 1; }

    /* Read magic number */
    int m1 = fgetc(fin), m2 = fgetc(fin);
    if (m1 != 'H' || m2 != 'F') {
        fprintf(stderr, "Error: %s is not a Huffman-compressed file\n", inpath);
        fclose(fin);
        return 1;
    }

    /* Read original file size */
    uint32_t file_size = 0;
    file_size |= (uint32_t)(fgetc(fin) & 0xFF);
    file_size |= (uint32_t)(fgetc(fin) & 0xFF) << 8;
    file_size |= (uint32_t)(fgetc(fin) & 0xFF) << 16;
    file_size |= (uint32_t)(fgetc(fin) & 0xFF) << 24;

    /* Reconstruct the Huffman tree */
    BitReader br;
    br_init(&br, fin);
    HuffNode *root = deserialize_tree(&br);
    if (!root) {
        fprintf(stderr, "Error: corrupt Huffman tree\n");
        fclose(fin);
        return 1;
    }

    /* Decode the data */
    FILE *fout = fopen(outpath, "wb");
    if (!fout) { perror(outpath); fclose(fin); free_tree(root); return 1; }

    if (verbose) {
        printf("\n=== Huffman Decompression ===\n");
        printf("Input: %s\n", inpath);
        printf("Original size: %u bytes\n", file_size);
    }

    uint32_t decoded = 0;
    HuffNode *current = root;

    while (decoded < file_size) {
        /* Single-symbol tree edge case */
        if (root->left == NULL && root->right == NULL) {
            fputc(root->symbol, fout);
            decoded++;
            continue;
        }

        int bit = br_read_bit(&br);
        if (bit < 0) {
            fprintf(stderr, "Error: unexpected end of compressed data "
                           "(%u/%u bytes decoded)\n", decoded, file_size);
            break;
        }

        current = (bit == 0) ? current->left : current->right;

        if (current->symbol >= 0) {
            fputc(current->symbol, fout);
            decoded++;
            current = root;  /* Restart from top for next symbol */
        }
    }

    fclose(fin);
    fclose(fout);
    free_tree(root);

    if (verbose) {
        printf("Restored: %s (%u bytes)\n", outpath, decoded);
    } else {
        printf("Decompressed %s → %s (%u bytes)\n", inpath, outpath, decoded);
    }

    return 0;
}

/* -----------------------------------------------------------------------
 * SECTION 9: Analysis Mode
 *
 * Show the frequency table and Huffman codes without actually compressing.
 * Useful for understanding how the algorithm would handle different inputs.
 * ----------------------------------------------------------------------- */

static int analyze(const char *inpath) {
    FILE *fin = fopen(inpath, "rb");
    if (!fin) { perror(inpath); return 1; }

    uint32_t freq[MAX_SYMBOLS] = {0};
    uint32_t file_size = 0;
    int c;
    while ((c = fgetc(fin)) != EOF) {
        freq[c]++;
        file_size++;
    }
    fclose(fin);

    if (file_size == 0) {
        printf("Empty file.\n");
        return 0;
    }

    int num_symbols = 0;
    HuffNode *root = build_tree(freq, &num_symbols);
    uint8_t path[MAX_CODE_LEN];
    memset(codes, 0, sizeof(codes));
    generate_codes(root, path, 0);

    /* Calculate entropy (theoretical minimum bits per symbol) */
    double entropy = 0.0;
    for (int i = 0; i < MAX_SYMBOLS; i++) {
        if (freq[i] > 0) {
            double p = (double)freq[i] / file_size;
            entropy -= p * (log(p) / log(2.0));  /* -p * log2(p) */
        }
    }

    /* Calculate actual average bits per symbol */
    uint64_t total_bits = 0;
    for (int i = 0; i < MAX_SYMBOLS; i++) {
        if (freq[i] > 0)
            total_bits += (uint64_t)freq[i] * codes[i].length;
    }
    double avg_bits = (double)total_bits / file_size;

    printf("\n=== Huffman Analysis ===\n");
    printf("File: %s (%u bytes, %d unique symbols)\n\n", inpath, file_size, num_symbols);

    printf("Information Theory:\n");
    printf("  Entropy (theoretical minimum): %.3f bits/symbol\n", entropy);
    printf("  Huffman average:               %.3f bits/symbol\n", avg_bits);
    printf("  Overhead vs optimal:           %.3f bits/symbol\n", avg_bits - entropy);
    printf("  Fixed-width (8 bits/byte):     8.000 bits/symbol\n");
    printf("  Theoretical compression:       %.1f%%\n", 100.0 * entropy / 8.0);
    printf("  Huffman compression:           %.1f%%\n", 100.0 * avg_bits / 8.0);

    printf("\nSymbol Table (sorted by frequency):\n");
    printf("  %-8s %8s %7s  %s\n", "Symbol", "Count", "%", "Huffman Code");
    printf("  %-8s %8s %7s  %s\n", "------", "-----", "---", "------------");

    /* Sort by frequency descending */
    int order[MAX_SYMBOLS];
    int count = 0;
    for (int i = 0; i < MAX_SYMBOLS; i++) {
        if (freq[i] > 0) order[count++] = i;
    }
    for (int i = 1; i < count; i++) {
        int key = order[i];
        int j = i - 1;
        while (j >= 0 && freq[order[j]] < freq[key]) {
            order[j + 1] = order[j];
            j--;
        }
        order[j + 1] = key;
    }

    for (int i = 0; i < count; i++) {
        int s = order[i];
        float pct = 100.0f * (float)freq[s] / (float)file_size;

        char sym_str[8];
        if (s >= 33 && s <= 126)
            snprintf(sym_str, sizeof(sym_str), "'%c'", s);
        else if (s == ' ')
            snprintf(sym_str, sizeof(sym_str), "' '");
        else if (s == '\n')
            snprintf(sym_str, sizeof(sym_str), "'\\n'");
        else if (s == '\t')
            snprintf(sym_str, sizeof(sym_str), "'\\t'");
        else if (s == '\r')
            snprintf(sym_str, sizeof(sym_str), "'\\r'");
        else if (s == 0)
            snprintf(sym_str, sizeof(sym_str), "NUL");
        else
            snprintf(sym_str, sizeof(sym_str), "0x%02X", s);

        printf("  %-8s %8u %6.1f%%  ", sym_str, freq[s], pct);
        for (int b = 0; b < codes[s].length; b++)
            putchar('0' + codes[s].bits[b]);
        printf(" (%d bits)\n", codes[s].length);
    }

    free_tree(root);
    return 0;
}

/* -----------------------------------------------------------------------
 * SECTION 10: Main — Command Line Interface
 * ----------------------------------------------------------------------- */

static void usage(const char *prog) {
    fprintf(stderr,
        "Huffman Coding — Lossless Compression From Scratch\n"
        "\n"
        "Usage:\n"
        "  %s -c [-v] <input> <output.huf>   Compress\n"
        "  %s -d [-v] <input.huf> <output>    Decompress\n"
        "  %s -a <input>                      Analyze (show codes)\n"
        "\n"
        "Options:\n"
        "  -c    Compress mode\n"
        "  -d    Decompress mode\n"
        "  -a    Analysis mode (no output file)\n"
        "  -v    Verbose output\n"
        "\n"
        "Part of the AILP Educational Curriculum.\n",
        prog, prog, prog);
}

int main(int argc, char **argv) {
    if (argc < 3) {
        usage(argv[0]);
        return 1;
    }

    int mode = 0;    /* 0=unknown, 'c'=compress, 'd'=decompress, 'a'=analyze */
    int verbose = 0;
    int argstart = 1;

    /* Parse flags */
    for (int i = 1; i < argc; i++) {
        if (argv[i][0] == '-') {
            for (int j = 1; argv[i][j]; j++) {
                switch (argv[i][j]) {
                    case 'c': mode = 'c'; break;
                    case 'd': mode = 'd'; break;
                    case 'a': mode = 'a'; break;
                    case 'v': verbose = 1; break;
                    default:
                        fprintf(stderr, "Unknown option: -%c\n", argv[i][j]);
                        usage(argv[0]);
                        return 1;
                }
            }
            argstart = i + 1;
        } else {
            break;
        }
    }

    if (mode == 0) {
        fprintf(stderr, "Error: specify -c (compress), -d (decompress), or -a (analyze)\n");
        usage(argv[0]);
        return 1;
    }

    if (mode == 'a') {
        if (argstart >= argc) {
            fprintf(stderr, "Error: missing input file\n");
            return 1;
        }
        return analyze(argv[argstart]);
    }

    if (argstart + 1 >= argc) {
        fprintf(stderr, "Error: need both input and output file\n");
        return 1;
    }

    const char *infile  = argv[argstart];
    const char *outfile = argv[argstart + 1];

    if (mode == 'c')
        return compress(infile, outfile, verbose);
    else
        return decompress(infile, outfile, verbose);
}
