#!/usr/bin/env python3
"""
Beale Cipher Testing Tool

Test historical book ciphers by mapping number sequences to word positions
in candidate documents. Includes offset testing to catch historical encoding variations.

Usage:
    python beale_tester.py

Then edit the candidate_text variable with your own document to test.
"""

import re
from cipher_data import CIPHER_ONE_FULL, CIPHER_THREE_FULL


def clean_and_tokenize(text):
    """
    Standardize text: remove punctuation, handle mixed whitespace.
    Returns a list of words.

    This regex finds sequences of alphanumeric characters, preserving apostrophes.
    Mirrors the likely method used in 1820 (counting words, not symbols).

    Args:
        text: Raw document text

    Returns:
        list: Tokenized words
    """
    return re.findall(r"\b[\w']+\b", text)


def decrypt_beale(cipher_sequence, word_list, offset=0):
    """
    Map cipher numbers to the first letter of words at those positions.

    Args:
        cipher_sequence: List of integers (the cipher)
        word_list: List of words (the key document)
        offset: Position adjustment for testing historical counting variations

    Returns:
        str: Decrypted text (sequence of first letters)
    """
    decrypted_chars = []

    for number in cipher_sequence:
        # Convert 1-based counting to 0-based index, apply offset
        target_index = (number - 1) + offset

        if 0 <= target_index < len(word_list):
            # Grab the word and take its first letter
            word = word_list[target_index]
            decrypted_chars.append(word[0].upper())
        else:
            # If the number is larger than the text length, mark as '?'
            decrypted_chars.append("?")

    return "".join(decrypted_chars)


def test_cipher(
    candidate_text,
    cipher_name="Cipher #1",
    cipher_data=None,
    test_length=50,
    offset_range=5,
):
    """
    Test a candidate document against a Beale cipher.

    Args:
        candidate_text: The document to test as a key
        cipher_name: Name for display purposes
        cipher_data: The cipher sequence (defaults to Cipher #1)
        test_length: Number of cipher values to test (default 50)
        offset_range: How many positions +/- to test (default 5)

    Returns:
        dict: Results including any promising matches
    """
    if cipher_data is None:
        cipher_data = CIPHER_ONE_FULL

    # Tokenize the candidate document
    tokens = clean_and_tokenize(candidate_text)

    print(f"\n{'='*60}")
    print(f"TESTING {cipher_name.upper()}")
    print(f"{'='*60}")
    print(f"Candidate Word Count: {len(tokens)}")
    print(f"Required Word Count: {max(cipher_data)}")

    # Quick sanity check
    if len(tokens) < max(cipher_data):
        print(f"⚠️  WARNING: Document too short!")
        print(f"   Document has {len(tokens)} words")
        print(f"   Cipher requires {max(cipher_data)} words")
        print(f"   Will show '?' for out-of-range values\n")

    # Test with various offsets
    results = {}
    print(
        f"\nTesting first {test_length} values with offsets [-{offset_range} to +{offset_range}]:\n"
    )

    for shift in range(-offset_range, offset_range + 1):
        # Use first N values for quick test
        result = decrypt_beale(cipher_data[:test_length], tokens, offset=shift)
        results[shift] = result

        # Highlight potentially meaningful results
        indicator = ""
        if "?" not in result[:10] and has_english_patterns(result[:20]):
            indicator = "  ⭐ POSSIBLE MATCH!"

        print(f"Offset {shift:+3d}: {result}{indicator}")

    return results


def has_english_patterns(text):
    """
    Simple heuristic to detect English-like text.

    Args:
        text: String to analyze

    Returns:
        bool: True if text shows English patterns
    """
    # Common English starting patterns
    patterns = [
        "TH",
        "HE",
        "IN",
        "AN",
        "WH",
        "RE",
        "ER",
        "HA",
        "IT",
        "WA",
        "IH",
        "WE",
        "OF",
        "TO",
        "BE",
        "IS",
        "AS",
        "AT",
        "OR",
    ]

    # Check for at least one common pattern
    for pattern in patterns:
        if pattern in text:
            return True

    # Check vowel ratio (English typically has 35-45% vowels)
    vowels = sum(1 for c in text if c in "AEIOUY")
    if len(text) > 0:
        vowel_ratio = vowels / len(text)
        if 0.25 < vowel_ratio < 0.55:
            return True

    return False


def batch_test(documents_dict):
    """
    Test multiple documents against both ciphers.

    Args:
        documents_dict: Dictionary mapping document names to text content

    Returns:
        dict: Results for all documents
    """
    results = {}

    for doc_name, doc_text in documents_dict.items():
        print(f"\n\n{'#'*70}")
        print(f"# TESTING DOCUMENT: {doc_name}")
        print(f"{'#'*70}")

        results[doc_name] = {
            "cipher_1": test_cipher(doc_text, "Cipher #1", CIPHER_ONE_FULL),
            "cipher_3": test_cipher(doc_text, "Cipher #3", CIPHER_THREE_FULL),
        }

    return results


# =============================================================================
# EXAMPLE USAGE
# =============================================================================

if __name__ == "__main__":
    # Example: Preamble to the U.S. Constitution (too short, but good for demo)
    candidate_text = """
    We the People of the United States, in Order to form a more perfect Union, 
    establish Justice, insure domestic Tranquility, provide for the common defence, 
    promote the general Welfare, and secure the Blessings of Liberty to ourselves 
    and our Posterity, do ordain and establish this Constitution for the United 
    States of America.
    
    Article I
    
    Section 1: All legislative Powers herein granted shall be vested in a Congress 
    of the United States, which shall consist of a Senate and House of Representatives.
    
    Section 2: The House of Representatives shall be composed of Members chosen every 
    second Year by the People of the several States, and the Electors in each State 
    shall have the Qualifications requisite for Electors of the most numerous Branch 
    of the State Legislature.
    """

    print(
        """
    ╔═══════════════════════════════════════════════════════════════════╗
    ║                  BEALE CIPHER TESTING TOOL                        ║
    ║                                                                   ║
    ║  Testing historical book ciphers from Bedford County, VA         ║
    ║  Cipher #1: Location (UNSOLVED)                                  ║
    ║  Cipher #3: Heirs (UNSOLVED)                                     ║
    ╚═══════════════════════════════════════════════════════════════════╝
    """
    )

    # Test Cipher #1
    test_cipher(candidate_text, "Cipher #1 (Location)", CIPHER_ONE_FULL)

    # Test Cipher #3
    test_cipher(candidate_text, "Cipher #3 (Heirs)", CIPHER_THREE_FULL)

    print("\n\n" + "=" * 70)
    print("TO TEST YOUR OWN DOCUMENT:")
    print("=" * 70)
    print("1. Replace the 'candidate_text' variable above with your document")
    print("2. Run this script again: python beale_tester.py")
    print("3. Look for patterns that resemble English text")
    print("4. If you find a match, contact cryptographic historians!")
    print("=" * 70)

    print("\n\nPROMISING CANDIDATES TO TEST:")
    print("  • U.S. Constitution (full text) - ~4500 words")
    print("  • Washington's Farewell Address - ~6000 words")
    print("  • Virginia Constitution of 1776 - ~3000 words")
    print("  • Bill of Rights - ~1200 words")
    print("  • Patrick Henry's 'Give Me Liberty' Speech - ~1200 words")
    print("  • Walter Scott's Ivanhoe (1820 edition)")
    print("\nFind these texts at: https://www.gutenberg.org or National Archives")
