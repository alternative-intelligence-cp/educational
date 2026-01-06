#!/usr/bin/env python3
"""
Map-Based Cipher Key Extractor

Extract text from historical map margins (indexes, legends, place name tables)
for use as cipher keys. Handles OCR, 18th-century typography, and French text.

Usage:
    python map_extractor.py map_image.jpg --output place_names.txt
    python map_extractor.py map_image.jpg --interactive  # Manual correction mode
"""

import argparse
import re
import sys
from pathlib import Path

try:
    from PIL import Image
    import pytesseract
except ImportError:
    print("ERROR: Required libraries not installed!")
    print("Install with: pip install pillow pytesseract")
    print(
        "Also install Tesseract OCR: sudo apt-get install tesseract-ocr tesseract-ocr-fra"
    )
    sys.exit(1)


class MapExtractor:
    """Extract and process text from historical map images."""

    def __init__(self, image_path, language="fra+eng"):
        """
        Initialize extractor.

        Args:
            image_path: Path to map image
            language: Tesseract language codes (fra+eng for French+English)
        """
        self.image_path = Path(image_path)
        self.language = language
        self.image = None

    def load_image(self):
        """Load the map image."""
        print(f"Loading image: {self.image_path}")
        self.image = Image.open(self.image_path)
        print(f"Image size: {self.image.size[0]}x{self.image.size[1]} pixels")
        return self.image

    def extract_region(self, region_name, bbox):
        """
        Extract text from a specific region of the map.

        Args:
            region_name: Descriptive name (e.g., "left_margin")
            bbox: (left, top, right, bottom) pixel coordinates

        Returns:
            Extracted text
        """
        if self.image is None:
            self.load_image()

        print(f"\nExtracting {region_name}...")
        region = self.image.crop(bbox)

        # Enhance for OCR (optional preprocessing)
        # region = region.convert('L')  # Grayscale
        # region = ImageEnhance.Contrast(region).enhance(2.0)  # Increase contrast

        # OCR with custom config for historical text
        custom_config = r"--oem 3 --psm 6"  # LSTM OCR, assume uniform text block
        text = pytesseract.image_to_string(
            region, lang=self.language, config=custom_config
        )

        return text

    def extract_all_text(self):
        """
        Extract all text from the entire map.

        Returns:
            Complete text extraction
        """
        if self.image is None:
            self.load_image()

        print("\nExtracting all text from map...")
        custom_config = r"--oem 3 --psm 6"
        text = pytesseract.image_to_string(
            self.image, lang=self.language, config=custom_config
        )

        return text

    def parse_indexed_list(self, text):
        """
        Parse numbered list of place names from extracted text.

        Handles formats like:
            1. Montreal
            2. Quebec
            ...
        Or:
            Montreal .... 1
            Quebec ...... 2

        Args:
            text: Raw OCR text

        Returns:
            List of (number, place_name) tuples
        """
        entries = []

        # Pattern 1: "1. Place Name" or "1 Place Name"
        pattern1 = re.compile(
            r"^\s*(\d+)[\.\s]+([A-Za-zÀ-ÿ\s\-\']+?)(?:\s*\d+)?$", re.MULTILINE
        )

        # Pattern 2: "Place Name .... 1" or "Place Name 1"
        pattern2 = re.compile(
            r"^([A-Za-zÀ-ÿ\s\-\']+?)\s*[\.]+\s*(\d+)\s*$", re.MULTILINE
        )

        # Pattern 3: Just lines with place names (we'll number them ourselves)
        pattern3 = re.compile(r"^([A-Za-zÀ-ÿ][A-Za-zÀ-ÿ\s\-\']{2,})\s*$", re.MULTILINE)

        # Try pattern 1
        matches = pattern1.findall(text)
        if matches:
            for num, name in matches:
                entries.append((int(num), name.strip()))
            print(f"Found {len(entries)} indexed entries (pattern 1)")
            return sorted(entries, key=lambda x: x[0])

        # Try pattern 2
        matches = pattern2.findall(text)
        if matches:
            for name, num in matches:
                entries.append((int(num), name.strip()))
            print(f"Found {len(entries)} indexed entries (pattern 2)")
            return sorted(entries, key=lambda x: x[0])

        # Fallback: Auto-number all place name lines
        matches = pattern3.findall(text)
        if matches:
            for idx, name in enumerate(matches, start=1):
                entries.append((idx, name.strip()))
            print(f"Found {len(entries)} place names (auto-numbered)")
            return entries

        print("WARNING: No indexed entries found. Raw text:")
        print(text[:500])
        return entries

    def clean_place_name(self, name):
        """
        Clean up OCR errors in place names.

        Common OCR issues in 18th-century text:
            - Long s (ſ) confused with f
            - Ligatures (æ, œ)
            - Accent marks
        """
        # Remove extra whitespace
        name = " ".join(name.split())

        # Fix common OCR errors
        name = name.replace("ſ", "s")  # Long s
        name = name.replace("|", "I")  # Pipe to I
        name = name.replace("0", "O")  # Zero to O in names

        # Remove noise
        name = re.sub(r"[^\w\s\-\'ÀÉÈÊÇàéèêç]", "", name)

        return name.strip()

    def export_word_list(self, entries, output_file):
        """
        Export place names as a numbered word list for cipher testing.

        Args:
            entries: List of (number, place_name) tuples
            output_file: Output file path
        """
        with open(output_file, "w", encoding="utf-8") as f:
            # Header
            f.write("# Place Names Extracted from Historical Map\n")
            f.write(f"# Source: {self.image_path}\n")
            f.write(f"# Total entries: {len(entries)}\n\n")

            # Entries
            for num, name in entries:
                clean_name = self.clean_place_name(name)
                f.write(f"{num}. {clean_name}\n")

        print(f"\nExported {len(entries)} place names to: {output_file}")

    def interactive_correction(self, entries):
        """
        Interactive mode to manually correct OCR errors.

        Args:
            entries: List of (number, place_name) tuples

        Returns:
            Corrected entries
        """
        print("\n" + "=" * 70)
        print("INTERACTIVE CORRECTION MODE")
        print("=" * 70)
        print("Press Enter to accept, or type correction")
        print("Type 'skip' to skip entry, 'quit' to finish\n")

        corrected = []

        for num, name in entries:
            clean = self.clean_place_name(name)
            user_input = input(f"{num}. [{clean}] -> ").strip()

            if user_input.lower() == "quit":
                break
            elif user_input.lower() == "skip":
                continue
            elif user_input == "":
                corrected.append((num, clean))
            else:
                corrected.append((num, user_input))

        return corrected


def auto_detect_margins(image):
    """
    Attempt to auto-detect margin regions in the map.

    This is a placeholder - would need computer vision techniques
    to properly detect text regions.

    Args:
        image: PIL Image object

    Returns:
        Dictionary of region bounding boxes
    """
    width, height = image.size

    # Rough estimates for typical map layouts
    # These would need to be adjusted based on actual map
    return {
        "left_margin": (0, 0, int(width * 0.15), height),
        "right_margin": (int(width * 0.85), 0, width, height),
        "top_title": (0, 0, width, int(height * 0.1)),
        "full_map": (0, 0, width, height),
    }


def main():
    parser = argparse.ArgumentParser(
        description="Extract place names from historical map for cipher testing"
    )
    parser.add_argument("image", help="Path to map image file")
    parser.add_argument(
        "-o", "--output", default="place_names.txt", help="Output file for place names"
    )
    parser.add_argument(
        "-r",
        "--region",
        choices=["left", "right", "top", "full", "custom"],
        default="full",
        help="Region to extract",
    )
    parser.add_argument("--bbox", help="Custom bounding box: left,top,right,bottom")
    parser.add_argument(
        "-i", "--interactive", action="store_true", help="Interactive correction mode"
    )
    parser.add_argument(
        "-l", "--language", default="fra+eng", help="Tesseract language codes"
    )

    args = parser.parse_args()

    # Initialize extractor
    extractor = MapExtractor(args.image, language=args.language)

    # Determine region
    if args.region == "custom" and args.bbox:
        bbox = tuple(map(int, args.bbox.split(",")))
        text = extractor.extract_region("custom", bbox)
    else:
        # Auto-detect or use predefined regions
        extractor.load_image()
        regions = auto_detect_margins(extractor.image)

        if args.region == "full":
            text = extractor.extract_all_text()
        elif args.region == "left":
            text = extractor.extract_region("left_margin", regions["left_margin"])
        elif args.region == "right":
            text = extractor.extract_region("right_margin", regions["right_margin"])
        elif args.region == "top":
            text = extractor.extract_region("top_title", regions["top_title"])

    # Parse indexed list
    entries = extractor.parse_indexed_list(text)

    if not entries:
        print("\nERROR: No place names extracted!")
        print("Try adjusting the region or using --interactive mode")
        return 1

    # Interactive correction if requested
    if args.interactive:
        entries = extractor.interactive_correction(entries)

    # Export
    extractor.export_word_list(entries, args.output)

    print("\n" + "=" * 70)
    print("NEXT STEPS:")
    print("=" * 70)
    print(f"1. Review {args.output} for accuracy")
    print(f"2. Run: python beale_map_tester.py --key {args.output}")
    print("3. Look for English patterns in the output")

    return 0


if __name__ == "__main__":
    sys.exit(main())
