# printc - Portable Colored Terminal Output

A lightweight, portable shell script that simplifies colored terminal output without requiring bash-specific features or external dependencies.

## Features

- ✅ **Portable**: Pure POSIX sh - works on any Unix-like system
- ✅ **Simple**: Clean color/text pair syntax
- ✅ **16 Colors**: Standard and bright ANSI colors
- ✅ **Case Insensitive**: `Red`, `red`, `RED` all work
- ✅ **Error Handling**: Clear error messages for invalid inputs
- ✅ **Auto Reset**: Colors reset automatically after output
- ✅ **Self-Documenting**: Built-in help and color demonstration

## Installation

```bash
# Copy to your PATH
sudo cp printc /usr/local/bin/
sudo chmod +x /usr/local/bin/printc

# Or use from current directory
chmod +x printc
./printc --help
```

## Quick Start

```bash
# Simple colored output
printc Red "Error: " White "File not found"

# Multiple colors with newline
printc -n Green "Success! " Blue "Operation completed."

# Mix and match
printc Yellow "Warning: " LightGray "Deprecated function"
```

## Usage

```
printc [FLAGS] [COLOR "text" ...]
```

### Flags

- `-n, --newline` - Append newline at the end of output
- `-c, --colors` - List all available colors (with color preview)
- `-h, --help` - Show help message

### Available Colors

**Standard Colors (30-37):**
- Black, Red, Green, Yellow, Blue, Magenta, Cyan, LightGray

**Bright Colors (90-97):**
- DarkGray, LightRed, LightGreen, LightYellow, LightBlue, LightMagenta, LightCyan, White

Colors are **case insensitive**: `red`, `Red`, `RED` all work the same.

## Examples

### Basic Usage

```bash
# Simple error message
printc Red "Error: " White "File not found"

# Success message with newline
printc -n Green "✓ " White "Build completed successfully"

# Multiple color pairs
printc Cyan "INFO: " LightGray "Processing file " Yellow "data.txt"
```

### Script Integration

```bash
#!/bin/sh

# Status messages
printc -n Blue "[INFO] " White "Starting process..."
printc -n Green "[DONE]"

# Error handling
if [ $? -ne 0 ]; then
    printc -n Red "[ERROR] " White "Process failed"
    exit 1
fi

# Warnings
printc -n Yellow "[WARN] " LightGray "Deprecated API usage detected"
```

### Build Output

```bash
# Compiler-style output
printc -n Cyan "Compiling " White "main.c"
printc -n Green "✓ " White "Compiled successfully"
printc -n Yellow "Warning: " LightGray "Unused variable 'x' at line 42"
```

### Log Colorization

```bash
# Log level indicators
printc -n DarkGray "[DEBUG] " LightGray "Entering function foo()"
printc -n Blue "[INFO] " White "Server started on port 8080"
printc -n Yellow "[WARN] " White "Connection timeout, retrying..."
printc -n Red "[ERROR] " White "Failed to connect to database"
```

## How It Works

The script:
1. Parses color/text pairs from arguments
2. Converts color names to ANSI escape codes (e.g., `Red` → `\033[31m`)
3. Outputs formatted text using `printf`
4. Automatically resets colors at the end

**ANSI Escape Sequence Format:**
```
\033[COLORm  - Set color
\033[0m      - Reset color
```

## Comparison to Alternatives

### vs. `echo -e`
```bash
# Traditional approach (bash-specific)
echo -e "\033[31mError:\033[0m \033[37mFile not found\033[0m"

# With printc (portable, readable)
printc Red "Error: " White "File not found"
```

### vs. tput
```bash
# Using tput (requires terminfo database)
echo "$(tput setaf 1)Error:$(tput sgr0) $(tput setaf 7)File not found$(tput sgr0)"

# With printc (simpler, no dependencies)
printc Red "Error: " White "File not found"
```

### vs. Specialized Libraries
- **No dependencies**: Works with just POSIX sh
- **Instant startup**: No compilation or package installation
- **Universal**: Works in any POSIX environment

## Error Handling

The script handles errors gracefully:

```bash
# Invalid color name
$ printc InvalidColor "test"
Error: Invalid color name 'InvalidColor'

# Missing text argument
$ printc Red
Error: Missing text argument for color 'Red'
```

All errors are printed to stderr and exit with code 1.

## Technical Details

### Portability

- Uses `#!/bin/sh` (POSIX sh, not bash)
- Case conversion with `tr` (no bash 4.0+ `${var,,}` expansion)
- Portable ANSI escape sequences
- Standard `printf` (no `echo -e` extension)
- Compatible with: sh, bash, dash, ash, zsh, ksh

### Color Code Reference

| Color | Standard | Bright |
|-------|----------|--------|
| Black | 30 | 90 (DarkGray) |
| Red | 31 | 91 (LightRed) |
| Green | 32 | 92 (LightGreen) |
| Yellow | 33 | 93 (LightYellow) |
| Blue | 34 | 94 (LightBlue) |
| Magenta | 35 | 95 (LightMagenta) |
| Cyan | 36 | 96 (LightCyan) |
| White | 37 (LightGray) | 97 (White) |

## Limitations

- **No background colors**: Only foreground colors supported
- **No styles**: No bold, italic, underline (keeps it simple)
- **16 colors only**: No 256-color or RGB support
- **No nested colors**: Each text segment is one color

These are intentional design decisions to keep the tool simple and portable.

## Use Cases

- **Build Scripts**: Colorize compiler output, test results
- **CI/CD Pipelines**: Visual status indicators in logs
- **Shell Scripts**: User-friendly status messages
- **Log Analysis**: Color-code log levels when viewing files
- **Documentation**: Create colorful examples in shell tutorials

## Related Tools

In the same repository:
- **dbug**: JavaScript debug tool with color tagging
- **custom_printf**: Educational implementation of printf

## License

MIT License - Feel free to use, modify, and distribute.

## Contributing

Found a bug? Have a feature idea?
- The script is intentionally minimal and portable
- Suggestions for improved error messages welcome
- Keep it POSIX sh compatible

## Author

Created as a companion to the Advanced Bash Mastery Guide.

## Version

Version 1.0 - February 2026
