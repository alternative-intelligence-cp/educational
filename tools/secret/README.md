# secret - Steganography + Encryption Vault

A simple vault that stores files in an encrypted archive, with optional steganographic key hiding via `steghide`.

## Features

- ✅ AES-256-CBC with PBKDF2
- ✅ Optional additional password (`--strong`)
- ✅ Steganographic key storage (if `steghide` + cover file present)
- ✅ Peek and export secrets without full extract
- ✅ Home-based vault directories by default

## Installation

```bash
# Copy to your PATH
sudo cp secret /usr/local/bin/
sudo chmod +x /usr/local/bin/secret

# Or run locally
chmod +x secret
./secret --help
```

## Default Vault Location

By default, the vault lives in:

```
$HOME/.secret-vault/
```

You can override the location with:

```bash
secret --home /path/to/vault --init
```

Or keep it beside the script (useful for demos):

```bash
secret --local --init
```

## Usage

```text
secret COMMAND [OPTIONS]
```

### Commands

- `--init` — initialize the vault
- `--pack` — pack files from `inbox/` into the encrypted archive
- `--unpack` — unpack archive into `outbox/`
- `--peek PATH` — view one file without unpacking everything
- `--list` — list files inside the archive
- `--status` — show vault status and configuration
- `--help` — show help

### Options

- `--strong PASSWORD` — additional password layer
- `--export VAR_NAME` — export peeked file to env var (for `eval`)
- `--home PATH` — use a custom vault location
- `--local` — store vault beside script
- `--iter N` — PBKDF2 iterations (default: 200000)
- `--shred` — secure-delete temp files (requires `shred`)

## Quick Start

```bash
# Initialize
secret --init

# Add files to inbox
cp my_api_key.txt ~/.secret-vault/inbox/

# Pack
secret --pack --strong "mypass"

# List
secret --list --strong "mypass"

# Peek a file
secret --peek my_api_key.txt --strong "mypass"

# Export to env var (askpass use case)
eval $(secret --peek ssh_pass.txt --export SSH_ASKPASS_PASS --strong "mypass")

# Check status
secret --status
```

## Steghide Key Hiding (Optional)

If `steghide` is installed and a cover file exists at:

```text
$HOME/.secret-vault/.internal/.terry_bird.dat
```

the key will be hidden in the cover file, and `secret.txt` will be removed after embedding.

If `steghide` or the cover file are missing, the key remains in:

```text
$HOME/.secret-vault/.internal/secret.txt
```

> **Tip**: Use any steghide-compatible cover file (JPEG, BMP, WAV, AU).

## Vault Layout

```text
.secret-vault/
├── inbox/                   # Drop files here before packing
├── outbox/                  # Unpacked files appear here
└── .internal/
    ├── .terry_bird.cnf      # Fingerprint file (auto-generated if missing)
    ├── .terry_bird.dat      # Steghide cover file (user-provided)
    ├── .terry_bird.tar.gz   # Encrypted archive
    └── secret.txt           # Key (only if not embedded)
```

## Notes

- If you used `--strong` while packing, you must use it for unpack/peek/list.
- Backup `.internal/` if you care about recovery.
- The vault creates folders automatically if missing.

## License

MIT (same as the educational repository).

## Version

1.0.0 — February 2026
