# 🔐 Beale Cipher Testing Tool

## Overview

An educational tool for testing historical book ciphers, specifically designed for the famous Beale Ciphers. This project demonstrates cryptographic principles, pattern matching algorithms, and historical computational challenges.

**Educational Value:**
- Historical cryptography and book cipher mechanics
- Algorithm design for pattern matching
- Handling historical encoding ambiguities (off-by-one errors, title counting variations)
- Text processing and tokenization strategies

## The Beale Cipher Mystery

The Beale Ciphers are three encrypted messages allegedly describing the location and contents of a buried treasure in Bedford County, Virginia (circa 1820). Only Cipher #2 has been solved using the Declaration of Independence as a key.

**Key Facts:**
- **Cipher #1 (Location)**: 520 numbers, max value 2906 → requires ~3000+ word key
- **Cipher #2 (Contents)**: Solved using Declaration of Independence
- **Cipher #3 (Heirs)**: 618 numbers, max value ~975 → requires ~1000+ word key

## Quick Start

```bash
# Run the tester with a candidate document
python3 beale_tester.py

# The script includes test data by default
# Edit the `candidate_text` variable to test your own documents
```

## How Book Ciphers Work

1. **Encoding**: Each number refers to a word position in a "key" document
2. **Letter Extraction**: Take the first letter of that word
3. **Message**: The sequence of letters forms the hidden message

**Example:**
```
Key Text: "We the People of the United States..."
Cipher: [1, 2, 3, 4, 5]
Decrypt: W-T-P-O-T (first letters of words 1-5)
```

## Historical Challenges

### The "Off-by-One" Problem

19th-century cipher creators were inconsistent about:
- Whether to count titles and preambles
- Zero-indexing vs one-indexing
- How to handle punctuation-attached words

This tool tests **offset variations** (-5 to +5) to catch these historical quirks.

### Edition Variations

For longer texts (novels, books), different printings had different pagination. The solver **must use the same edition** the encoder used, or the word positions won't match.

**Solution Strategy**: Use short, canonical texts:
- Legal documents (Declaration, Constitution)
- Famous speeches (Washington's Farewell Address)
- Religious texts (specific passages from King James Bible)

## Candidate Documents

Based on the 1820s Virginia context, likely keys include:

### High Probability (Cipher #1: 3000+ words)
- U.S. Constitution (full text) - ~4500 words
- Washington's Farewell Address - ~6000 words
- Virginia Constitution of 1776 - ~3000 words
- Walter Scott's *Ivanhoe* (1820 edition, Chapter 1)

### Medium Probability (Cipher #3: 1000+ words)
- Bill of Rights (First 10 Amendments)
- Patrick Henry's "Give Me Liberty" Speech - ~1200 words
- Lord's Prayer + Ten Commandments

## Usage Examples

### Test a Single Document

```python
from beale_tester import test_cipher

# Your candidate text
candidate = """
We the People of the United States, in Order to form
a more perfect Union...
"""

# Test both ciphers
test_cipher(candidate, cipher_name="Cipher #1")
test_cipher(candidate, cipher_name="Cipher #3")
```

### Batch Testing

```python
from beale_tester import batch_test

documents = {
    "Constitution": constitution_text,
    "Bill of Rights": bor_text,
    "Declaration": declaration_text
}

results = batch_test(documents)
print(results)
```

## Understanding Output

```
Total Word Count in Candidate Text: 4543

--- TESTING CIPHER #1 (LOCATION) ---
Offset -5: ?XZQP...  ← Gibberish = wrong key
Offset -4: ?TKMP...  ← Still wrong
Offset  0: IHAVE...  ← Readable English! Possible match!
Offset +1: ?QWER...  ← Wrong again
```

**Success Indicators:**
- ✅ Common English patterns: THE, HAVE, WITH, LOOK
- ✅ Vowel distribution (A, E, I, O appear frequently)
- ❌ Rare consonant clusters: JQ, XZ, PK (usually wrong key)
- ❌ Many `?` symbols (number exceeds document length)

## Technical Details

### Tokenization Strategy

```python
import re

# Matches alphabetic sequences, preserving apostrophes
tokens = re.findall(r"\b[\w']+\b", text)
```

**Why this matters:**
- "self-evident" → one word or two?
- "1776" → counted as a word?
- "We're" → apostrophe handling

The regex approach mirrors likely 1820s manual counting.

### Offset Testing

Tests positions [-5, +5] relative to standard indexing:

```python
for shift in range(-5, 6):
    target_index = (number - 1) + shift  # Convert to 0-based, apply offset
    if 0 <= target_index < len(words):
        letter = words[target_index][0].upper()
```

This catches:
- Title counted/skipped
- Date line included/excluded  
- Preamble treated as body

## Files

- `beale_tester.py` - Main testing script
- `cipher_data.py` - Full cipher sequences (Cipher #1 and #3)
- `candidate_keys.txt` - List of historical documents to test
- `README.md` - This file

## Educational Extensions

### For Students

1. **Frequency Analysis**: Modify the script to count letter frequency in decrypted output
2. **Automated Scoring**: Assign scores based on English letter frequency (E, T, A, O should be common)
3. **Dictionary Validation**: Check if decrypted chunks form valid English words
4. **Key Generation**: Create your own book cipher encoder

### Advanced Projects

1. **ML Classification**: Train a model to detect "English-like" sequences
2. **Genetic Algorithms**: Evolve offset parameters to maximize English-ness
3. **N-gram Analysis**: Use bigram/trigram frequencies (TH, HE, AN are common)
4. **Historical OCR**: Process scanned 1820s documents for key testing

## The Mystery Continues

Despite 200 years of effort, Ciphers #1 and #3 remain unsolved. Theories include:

- **Lost Key**: The document no longer exists or is misidentified
- **Hoax Theory**: The ciphers are random numbers (no solution exists)
- **Multilingual Key**: Uses French, Latin, or mixed-language text
- **Custom Mapping**: Non-standard encoding (not simple first-letter extraction)

## Contributing

Found a promising pattern? Discovered a new candidate document?

Submit your findings:
1. Document the key text (with source and edition)
2. Share offset parameters that produced readable output
3. Include decrypted string samples

**Note**: This tool is for education and historical research. Handle any "treasure location" findings responsibly!

## References

- [Beale Ciphers - Wikipedia](https://en.wikipedia.org/wiki/Beale_ciphers)
- [The Beale Papers (1885 Pamphlet)](http://www.unmuseum.org/bealeppr.htm)
- *The Code Book* by Simon Singh (Chapter on Beale Ciphers)
- [Bedford County, VA Historical Society](https://www.bedfordvahistoricalsociety.org/)

## License

MIT License - Educational use encouraged!

---

**Living near Bedford County, VA?** You have a unique advantage - visit local archives, examine historical documents firsthand, and contribute to solving one of cryptography's enduring mysteries! 🗺️🔍
