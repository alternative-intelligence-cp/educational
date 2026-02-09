# foreach - Apply a Command to Each Element

A portable POSIX shell utility for applying a command to each element from arguments or stdin.

## Features

- ✅ **Portable**: Pure POSIX sh
- ✅ **Flexible Input**: Arrays or piped/redirected stdin
- ✅ **Selectors**: Head, tail, or range processing
- ✅ **Search Mode**: Generate input via filesystem search (many filters)
- ✅ **Fail Fast**: Stop on first error when desired
- ✅ **Verbose Mode**: See each element as it runs

## Installation

```bash
# Copy to your PATH
sudo cp foreach /usr/local/bin/
sudo chmod +x /usr/local/bin/foreach

# Or run locally
chmod +x foreach
./foreach --help
```

## Usage

```text
foreach [options] COMMAND ARRAY...
some_command | foreach [options] COMMAND
foreach [options] COMMAND < <(command_producing_lines)
foreach [options] COMMAND <<EOF
  line1
  line2
EOF
foreach [options] COMMAND --search PATH [search options]
```

### Placeholders

- `$_THIS_` — current element
- `$_THIS_I` — iteration count (1-based)

**IMPORTANT**: Always quote the COMMAND argument to prevent shell expansion!
- ✅  Good: `foreach 'echo $_THIS_'` or `foreach "echo \$_THIS_"`
- ❌  Bad:  `foreach echo $_THIS_` (expands $_THIS_ before foreach runs)

## Options

- `-v, --verbose` — print each element before running COMMAND
- `-f, --fail` — stop on first non-zero exit code
- `-H, --head COUNT` — process first COUNT elements
- `-t, --tail COUNT` — process last COUNT elements
- `-r, --range FIRST LAST` — process elements from FIRST to LAST (1-based)
- `-s, --search PATH` — generate input by searching PATH
  - `-s-is TYPE` — include only TYPE: dir, file, link, exe
  - `-s-not TYPE` — exclude TYPE: dir, file, link, exe
  - `-s-name NAME` — match name (case-sensitive)
  - `-s-iname NAME` — match name (case-insensitive)
  - `-s-not-name NAME` — exclude name (case-sensitive)
  - `-s-not-iname NAME` — exclude name (case-insensitive)
  - `-s-path PATTERN` — match path (case-sensitive)
  - `-s-ipath PATTERN` — match path (case-insensitive)
  - `-s-not-path PATTERN` — exclude path (case-sensitive)
  - `-s-regex PATTERN` — match regex against full path (GNU find)
  - `-s-iregex PATTERN` — match regex (case-insensitive, GNU find)
  - `-s-ext EXT` — match extension (e.g. log, .log)
  - `-s-not-ext EXT` — exclude extension
  - `-s-starts PREFIX` — match name prefix
  - `-s-ends SUFFIX` — match name suffix
  - `-s-contains TEXT` — match name containing text
  - `-s-istarts PREFIX` — case-insensitive prefix
  - `-s-iends SUFFIX` — case-insensitive suffix
  - `-s-icontains TEXT` — case-insensitive contains
  - `-s-prune PATH` — exclude PATH subtree
  - `-s-owner OWNER` — match file owner
  - `-s-group GROUP` — match file group
  - `-s-uid UID` — match numeric user ID
  - `-s-gid GID` — match numeric group ID
  - `-s-when MIN MAX` — match by modification date range
  - `-s-newer FILE` — match files newer than FILE
  - `-s-older FILE` — match files older than FILE
  - `-s-maxdepth N` — max directory depth
  - `-s-mindepth N` — min directory depth
  - `-s-size SIZE` — match size (find syntax, e.g. +1M)
  - `-s-mtime DAYS` — match mtime in days (e.g. -7, +3)
  - `-s-atime DAYS` — match atime in days (e.g. -7, +3)
  - `-s-ctime DAYS` — match ctime in days (e.g. -7, +3)
  - `-s-links N` — match number of hard links
  - `-s-perm MODE` — match permissions (find syntax)
  - `-s-empty` — match empty files/dirs
  - `-s-hidden` — match hidden files/dirs
  - `-s-nohidden` — exclude hidden files/dirs
  - `-s-readable` — match readable files
  - `-s-writable` — match writable files
  - `-s-executable` — match executable files
  - `-s-follow` — follow symlinks
  - `-s-xdev` — stay on one filesystem
  - `-s-depth` — depth-first traversal
- `-h, --help` — show help

> **Compatibility**: `-h COUNT` is accepted as a head alias when COUNT is numeric.
> **Tip**: Use `-` to leave MIN or MAX open-ended in `-s-when`.

## Examples

```bash
# Run a command for each arg
foreach "echo $_THIS_" a b c

# Read from stdin
printf '%s\n' one two three | foreach "echo $_THIS_I: $_THIS_"

# Copy files, stop on first failure
foreach -f "cp $_THIS_ /tmp/" file1 file2 file3

# Only process a range
foreach -r 2 4 "echo $_THIS_" one two three four five

# Search files and process results
foreach --search /var/log -s-name "*.log" "echo $_THIS_"

# Search for executables updated this month
foreach --search /usr/bin -s-is exe -s-when "2026-02-01" - "echo $_THIS_"

# Exclude hidden paths and limit depth
foreach --search . -s-nohidden -s-maxdepth 2 "echo $_THIS_"

# Use extension and prune
foreach --search . -s-ext sh -s-prune ./node_modules "echo $_THIS_"

# Files newer than a reference file
foreach --search . -s-newer ./README.md "echo $_THIS_"
```

## Notes

- Quote COMMAND if it contains spaces or shell operators.
- Use `--` to stop option parsing if COMMAND starts with `-`.

## License

MIT (same as the educational repository).

## Version

1.0.0 — February 2026
