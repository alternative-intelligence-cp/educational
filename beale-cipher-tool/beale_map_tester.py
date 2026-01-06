#!/usr/bin/env python3
"""
Beale Cipher Map Tester

Test Beale ciphers against place names extracted from historical maps.
Handles special considerations for map-based keys.

Usage:
    python beale_map_tester.py --key place_names.txt --cipher 1
    python beale_map_tester.py --key place_names.txt --all
"""

import argparse
import sys
from pathlib import Path
from cipher_data import CIPHER_ONE_FULL, CIPHER_THREE_FULL


def load_map_key(key_file):
    """
    Load place names from map extraction file.

    Args:
        key_file: Path to place names file

    Returns:
        List of place names (indexed from 1)
    """
    place_names = [""]  # Index 0 unused (1-based indexing)

    with open(key_file, "r", encoding="utf-8") as f:
        for line in f:
            line = line.strip()

            # Skip comments and empty lines
            if not line or line.startswith("#"):
                continue

            # Parse "123. Place Name" format
            if "." in line:
                parts = line.split(".", 1)
                if len(parts) == 2:
                    try:
                        num = int(parts[0].strip())
                        name = parts[1].strip()

                        # Extend list if necessary
                        while len(place_names) <= num:
                            place_names.append("")

                        place_names[num] = name
                    except ValueError:
                        continue

    # Remove empty entries and reindex
    clean_names = [name for name in place_names if name]

    print(f"Loaded {len(clean_names)} place names from {key_file}")
    return clean_names


def decrypt_with_map(cipher_sequence, place_names, offset=0, method="first_letter"):
    """
    Decrypt cipher using map place names.

    Args:
        cipher_sequence: List of cipher numbers
        place_names: List of place names (1-indexed)
        offset: Position adjustment
        method: 'first_letter', 'last_letter', or 'all_letters'

    Returns:
        Decrypted text
    """
    decrypted_chars = []

    for number in cipher_sequence:
        # Adjust for 1-based indexing and offset
        index = number + offset

        if 1 <= index < len(place_names):
            name = place_names[index]

            if method == "first_letter":
                decrypted_chars.append(name[0].upper())
            elif method == "last_letter":
                decrypted_chars.append(name[-1].upper())
            elif method == "all_letters":
                # Take all letters, might spell longer words
                decrypted_chars.append(name)
            else:
                decrypted_chars.append(name[0].upper())
        else:
            decrypted_chars.append("?")

    if method == "all_letters":
        return " ".join(decrypted_chars)
    else:
        return "".join(decrypted_chars)


def analyze_result(text, min_length=20):
    """
    Analyze decrypted text for English patterns.

    Args:
        text: Decrypted text to analyze
        min_length: Minimum text length to analyze

    Returns:
        Dictionary of analysis metrics
    """
    if len(text) < min_length:
        return {"score": 0, "reason": "Too short"}

    # Count characteristics
    vowels = sum(1 for c in text if c in "AEIOUY")
    consonants = sum(1 for c in text if c.isalpha() and c not in "AEIOUY")
    unknowns = text.count("?")

    # Common English patterns
    common_digrams = ["TH", "HE", "IN", "ER", "AN", "RE", "ON", "AT", "EN", "ND"]
    digram_count = sum(text.count(dg) for dg in common_digrams)

    # Common words related to treasure/location
    treasure_words = [
        "BURIED",
        "FEET",
        "NORTH",
        "SOUTH",
        "EAST",
        "WEST",
        "DEPTH",
        "IRON",
        "VAULT",
        "LOCATED",
        "COUNTY",
    ]
    treasure_matches = sum(1 for word in treasure_words if word in text)

    # Calculate scores
    total_chars = len(text)
    vowel_ratio = vowels / total_chars if total_chars > 0 else 0
    unknown_ratio = unknowns / total_chars if total_chars > 0 else 0

    # Overall score (0-100)
    score = 0

    # Vowel ratio should be 0.35-0.45 for English
    if 0.30 < vowel_ratio < 0.50:
        score += 30

    # Penalize unknowns
    score -= unknown_ratio * 40

    # Reward common digrams
    score += min(digram_count * 5, 30)

    # Bonus for treasure-related words
    score += treasure_matches * 20

    analysis = {
        "score": max(0, min(100, score)),
        "vowel_ratio": vowel_ratio,
        "unknown_ratio": unknown_ratio,
        "digram_count": digram_count,
        "treasure_matches": treasure_matches,
        "text_sample": text[:50],
    }

    return analysis


def test_cipher_with_map(cipher_data, cipher_name, place_names, test_length=100):
    """
    Test a cipher against map place names with comprehensive analysis.

    Args:
        cipher_data: Cipher sequence
        cipher_name: Display name
        place_names: Place name list
        test_length: How many cipher values to test
    """
    print(f"\n{'='*80}")
    print(f"TESTING {cipher_name.upper()}")
    print(f"{'='*80}")
    print(f"Cipher length: {len(cipher_data)}")
    print(f"Place names available: {len(place_names)}")
    print(f"Max cipher value: {max(cipher_data)}")

    if max(cipher_data) >= len(place_names):
        print(
            f"⚠️  WARNING: Cipher requires index {max(cipher_data)}, but only {len(place_names)} names available"
        )

    print(f"\nTesting first {test_length} values with various offsets and methods...")

    results = []

    # Test different methods
    for method in ["first_letter", "last_letter"]:
        print(f"\n--- Method: {method.upper().replace('_', ' ')} ---")

        for offset in range(-5, 6):
            decrypted = decrypt_with_map(
                cipher_data[:test_length], place_names, offset, method
            )
            analysis = analyze_result(decrypted)

            # Store result
            results.append(
                {
                    "method": method,
                    "offset": offset,
                    "text": decrypted,
                    "analysis": analysis,
                }
            )

            # Display
            indicator = ""
            if analysis["score"] > 50:
                indicator = "  ⭐⭐⭐ HIGH SCORE!"
            elif analysis["score"] > 30:
                indicator = "  ⭐⭐ PROMISING"
            elif analysis["score"] > 15:
                indicator = "  ⭐ POSSIBLE"

            print(
                f"Offset {offset:+3d} [{analysis['score']:5.1f}]: {decrypted[:60]}{indicator}"
            )

    # Show top results
    print(f"\n{'='*80}")
    print("TOP 5 RESULTS:")
    print(f"{'='*80}")

    sorted_results = sorted(results, key=lambda x: x["analysis"]["score"], reverse=True)

    for i, result in enumerate(sorted_results[:5], 1):
        print(f"\n#{i} - Score: {result['analysis']['score']:.1f}")
        print(f"   Method: {result['method']}, Offset: {result['offset']:+d}")
        print(f"   Text: {result['text'][:80]}")
        if result["analysis"]["treasure_matches"] > 0:
            print(
                f"   ⭐ Found {result['analysis']['treasure_matches']} treasure-related words!"
            )

    return sorted_results


def main():
    parser = argparse.ArgumentParser(
        description="Test Beale ciphers against map place names"
    )
    parser.add_argument(
        "--key", required=True, help="Place names file from map extraction"
    )
    parser.add_argument(
        "--cipher", type=int, choices=[1, 3], help="Which cipher to test (1 or 3)"
    )
    parser.add_argument("--all", action="store_true", help="Test all ciphers")
    parser.add_argument(
        "--length", type=int, default=100, help="Number of cipher values to test"
    )
    parser.add_argument(
        "--full", action="store_true", help="Test full cipher (can be slow)"
    )

    args = parser.parse_args()

    # Load place names
    try:
        place_names = load_map_key(args.key)
    except FileNotFoundError:
        print(f"ERROR: Key file not found: {args.key}")
        return 1

    if not place_names:
        print("ERROR: No place names loaded from key file")
        return 1

    # Determine test length
    test_length = args.length
    if args.full:
        test_length = 999999  # Effectively unlimited

    # Test specified ciphers
    if args.all or args.cipher == 1:
        test_cipher_with_map(
            CIPHER_ONE_FULL, "Cipher #1 (Location)", place_names, test_length
        )

    if args.all or args.cipher == 3:
        test_cipher_with_map(
            CIPHER_THREE_FULL, "Cipher #3 (Heirs)", place_names, test_length
        )

    print("\n" + "=" * 80)
    print("ANALYSIS COMPLETE")
    print("=" * 80)
    print("\nIf you found high-scoring results, try:")
    print("  1. Extract more text from the map margins")
    print("  2. Manually correct OCR errors in the place names file")
    print("  3. Test with full cipher length (--full)")
    print("  4. Document your findings!")

    return 0


if __name__ == "__main__":
    sys.exit(main())
