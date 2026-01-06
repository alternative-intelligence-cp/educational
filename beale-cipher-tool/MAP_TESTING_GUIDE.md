# Map-Based Cipher Testing - Quick Start Guide

## What You Have

1. **Historical map image** - 1720s French map of North America
2. **Beale ciphers** - Unsolved location and heirs ciphers
3. **Reddit rumor** - Someone got "interesting results" testing a map

## What You Need To Do

### Step 1: Extract Place Names from Map

The map has indexed tables in the margins. Extract these:

```bash
# Install required tools (first time only)
pip3 install pillow pytesseract
sudo apt-get install tesseract-ocr tesseract-ocr-fra

# Extract text from the map
python3 map_extractor.py your_map_image.jpg --output map_places.txt

# Or extract just the left margin
python3 map_extractor.py your_map_image.jpg --region left --output left_margin.txt
```

**Interactive mode** (recommended for accuracy):
```bash
python3 map_extractor.py your_map_image.jpg --interactive --output map_places.txt
```

This lets you manually correct OCR errors as it extracts.

### Step 2: Test Against Beale Ciphers

```bash
# Test Cipher #1 (Location)
python3 beale_map_tester.py --key map_places.txt --cipher 1

# Test Cipher #3 (Heirs)
python3 beale_map_tester.py --key map_places.txt --cipher 3

# Test both ciphers
python3 beale_map_tester.py --key map_places.txt --all
```

### Step 3: Look for Patterns

The tester will show you:
- **Score**: 0-100, higher is more English-like
- **Sample text**: First 60 characters of decrypted result
- **Indicators**: ⭐ symbols for promising results

**What to look for:**
- Scores above 30: Worth investigating
- Common words: THE, HAVE, FEET, BURIED, NORTH, etc.
- Vowel ratio around 35-45%

## Understanding the Output

```
Offset  +0 [ 67.3]: IHAVEBURIEDTHEVAULTTHREEMILESFROMBUFORDSINN ⭐⭐⭐ HIGH SCORE!
Offset  +1 [ 23.1]: XQZPWMKLTHGFDSAPOIUYTREWQASDFGHJKLMNBVCXZ
Offset  -1 [ 45.2]: THEVAULTISBURIEDTHREEMILESFROMTHETAVERN  ⭐⭐ PROMISING
```

- **Offset +0**: Gibberish = wrong
- **Offset +1**: Looks like English! Possible match!
- **Offset -1**: Also promising, test further

## Advanced Usage

### Extract Specific Map Regions

If you know where the index tables are:

```bash
# Custom bounding box: left,top,right,bottom (in pixels)
python3 map_extractor.py map.jpg --region custom --bbox 0,0,200,800
```

### Test Different Extraction Methods

The map might use different encoding schemes:

```bash
# Already built into beale_map_tester.py
# It tests both:
#   - First letter of place name
#   - Last letter of place name
```

### Full Cipher Testing

Test the entire cipher (not just first 100 values):

```bash
python3 beale_map_tester.py --key map_places.txt --cipher 1 --full
```

⚠️ This can be slow but gives complete results.

## Troubleshooting

### "No place names extracted"

The OCR might have failed. Try:

1. **Higher resolution image**: Get better scan
2. **Different region**: Try `--region left` or `--region right`
3. **Manual entry**: Create `map_places.txt` by hand:

```
1. Montreal
2. Quebec
3. Louisiane
4. Nouvelle Orleans
...
```

### "Score is always low"

- Check that place names are correctly numbered (1-based)
- Verify OCR extracted text correctly
- Try different offset ranges
- Map might not be the correct key

### "Max cipher value exceeds place names"

You need more place names! Options:

1. Extract from other map regions
2. Combine multiple maps from same era
3. Include ALL text from map (not just indexes)

## File Formats

### Input: Place Names File

```
# Comments start with #
1. Montreal
2. Quebec
3. Louisiane
4. Nouvelle Orleans
5. Carolina
...
```

### Output: Results

The tester prints results to screen. To save:

```bash
python3 beale_map_tester.py --key map_places.txt --all > results.txt
```

## What If You Find Something?

If you get a high-scoring result:

1. **Document everything**:
   - Which map (exact source, date, edition)
   - Which cipher
   - What offset worked
   - Full decrypted text

2. **Verify it's not coincidence**:
   - Test with full cipher, not just first 100
   - Check if message makes historical sense
   - Compare to known Cipher #2 solution style

3. **Share carefully**:
   - Consider historical/academic channels first
   - Document your methodology
   - Be prepared for skepticism (it's crypto!)

## Next Steps

1. Get the highest resolution scan of the map you can find
2. Extract place names using the tools
3. Test against ciphers
4. If you get promising results, iterate:
   - Refine the place name list
   - Try different map regions
   - Test with full ciphers

Good luck! You might be the one to finally crack these 200-year-old ciphers! 🔐🗺️
