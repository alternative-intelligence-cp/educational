# remote

SSH/SCP wrapper with session management and config file support

**Version:** 1.0.0  
**Author:** Randy Hoggard  
**Date:** February 7, 2026

## Description

`remote` is a convenience wrapper around SSH and SCP that provides:
- Persistent session storage (remote host, username, port)
- Config file support with chaining
- Simplified copy commands (to/from remote)
- Session management across multiple commands

## Installation

```bash
# Copy to a directory in your PATH
cp remote /usr/local/bin/
chmod +x /usr/local/bin/remote
```

## Usage

```bash
remote [options] command [args]
```

## Options

| Option | Description |
|--------|-------------|
| `--set-remote REMOTE_HOST` | Set remote hostname/IP for session |
| `--set-user USER_NAME` | Set SSH username for session |
| `--set-port PORT` | Set SSH port for session (default: 22) |
| `--set-pass PASSWORD` | Set password for session (NOT SECURE - see Security) |
| `--set-conf PATH_TO_CONF` | Load settings from config file |
| `--clear` | Clear current session settings |
| `--show` | Show current session settings |
| `--help` | Show help message |

## Commands

### cpt - Copy To Remote

Copy files/directories TO the remote server:

```bash
remote cpt LOCAL_SOURCE REMOTE_DESTINATION
```

**Examples:**
```bash
# Copy file to remote home directory
remote cpt ./myfile.txt ~/

# Copy entire directory to remote server
remote cpt ./myproject /var/www/
```

### cpf - Copy From Remote

Copy files/directories FROM the remote server:

```bash
remote cpf REMOTE_SOURCE LOCAL_DESTINATION
```

**Examples:**
```bash
# Copy remote log file to local directory
remote cpf /var/log/app.log ./logs/

# Copy entire remote directory
remote cpf /var/www/backup ./backup/
```

### con - Connect via SSH

Connect to remote server via SSH:

```bash
remote con [remote-host]
```

**Examples:**
```bash
# Connect using session settings
remote con

# Connect with override (ignores session settings)
remote con user@server.com
remote con 192.168.1.100
```

### exec - Execute Command

Execute a command on the remote server:

```bash
remote exec "COMMAND"
```

**Examples:**
```bash
# Check disk space
remote exec "df -h"

# View logs
remote exec "tail -20 /var/log/syslog"

# Restart service (interactive, prompts for sudo password if needed)
remote exec "sudo systemctl restart nginx"

# Multiple commands
remote exec "cd /var/www && ls -la"
```

**Note:** Commands are executed with TTY allocation (`-t` flag), allowing interactive commands like `sudo` to work properly.

## Session Management

Session settings persist across commands until cleared or system reboot. This allows you to configure once and use repeatedly:

```bash
# Set up session
remote --set-remote 192.168.1.100
remote --set-user admin
remote --set-port 2222

# Now use repeatedly without re-specifying
remote con                              # Connect with saved settings
remote cpt ./file.txt /tmp/            # Copy using saved settings
remote cpf /var/log/app.log ./         # Copy from using saved settings

# View current settings
remote --show

# Clear when done
remote --clear
```

**How settings are loaded:**
1. Default config (`~/.remote.conf`) is loaded first (if it exists)
2. Session settings override config file settings
3. Command-line flags override both

This means you can:
- Edit `~/.remote.conf` with your common defaults (e.g., username, port)
- Use `--set-remote` to set the host for this session
- Session persists the merged settings

**Session storage:** `/tmp/remote_session_$USER` (protected with mode 600)

## Config Files

### Default Config

Default config file: `~/.remote.conf`

If it doesn't exist, `remote` creates it on first run with this template:

```bash
# ~/.remote.conf
# Remote SSH/SCP Configuration

REMOTE_HOST=""
USER_NAME=""
PORT="22"
PASSWORD=""
PATH_TO_CONF=""
```

**Important:** The default config is **automatically loaded** every time you run `remote`. 

**Quick start:**
```bash
# Edit the default config with your common settings
nano ~/.remote.conf
# Set: USER_NAME="yourname"
# Set: PORT="22"

# Now just specify the host for each connection
remote --set-remote 192.168.1.100
remote con  # Uses USER_NAME and PORT from config file
```

### Config File Format

Config files use shell-style variable assignments:

```bash
REMOTE_HOST="192.168.1.100"
USER_NAME="admin"
PORT="22"
PASSWORD=""              # Leave empty - NOT RECOMMENDED
PATH_TO_CONF=""          # Chain to another config (optional)
```

### Config Chaining

Use `PATH_TO_CONF` to load a base config, then override specific settings:

**Example:**

`~/.remote.conf` (base settings):
```bash
USER_NAME="randy"
PORT="22"
PATH_TO_CONF=""
```

`~/.remote_prod.conf` (production overrides):
```bash
REMOTE_HOST="prod.example.com"
USER_NAME="deploy"
PORT="2222"
PATH_TO_CONF="/home/randy/.remote.conf"  # Load base first
```

When you load the prod config:
```bash
remote --set-conf ~/.remote_prod.conf
```

It will:
1. Load base config (`~/.remote.conf`)
2. Override with prod config values
3. Save merged settings to session

## Security

### Passwords (NOT RECOMMENDED)

Config files and sessions can store passwords but this is **NOT SECURE**:
- Passwords stored in **PLAIN TEXT**
- Readable by processes with access to /tmp
- No encryption protection

### Better Options

1. **SSH Keys (RECOMMENDED)**: Set up passwordless SSH
   ```bash
   ssh-keygen -t ed25519
   ssh-copy-id user@remote-host
   ```

2. **Manual Entry**: Let SSH prompt for password each time

3. **Session-only**: Use `--set-pass` for temporary storage (cleared on `--clear` or reboot)

### File Permissions

Session files are created with mode 600 (owner read/write only) to provide basic protection.

## Examples

### Quick Start

```bash
# Configuration
remote --set-remote server.example.com
remote --set-user deploy

# Daily usage
remote con                          # Connect
remote cpt ./app.zip /tmp/          # Upload
remote cpf /var/log/app.log ./      # Download
```

### Development Workflow

```bash
# Set up development server
remote --set-conf ~/.remote_dev.conf

# Deploy code
remote cpt ./build/app.jar /opt/myapp/

# Connect and restart service
remote con
# (now in SSH session)
> sudo systemctl restart myapp
> exit

# Fetch logs
remote cpf /var/log/myapp.log ./logs/
```

### Multiple Servers

```bash
# Production
remote --set-conf ~/.remote_prod.conf
remote cpf /var/log/app.log ./prod.log
remote --clear

# Staging
remote --set-conf ~/.remote_staging.conf
remote cpf /var/log/app.log ./staging.log
remote --clear
```

## Dependencies

- `ssh` - OpenSSH client
- `scp` - Secure copy (usually included with OpenSSH)
- `printc` (optional) - Color output support

If `printc` is not available, `remote` falls back to plain text output.

## Troubleshooting

### "Remote host not set" error

You haven't configured a remote host. Use:
```bash
remote --set-remote YOUR_HOST
```

Or load a config file:
```bash
remote --set-conf ~/.remote.conf
```

### Connection refused

- Check remote host is accessible: `ping YOUR_HOST`
- Verify SSH port: `remote --show` (check port setting)
- Ensure SSH service running on remote

### Permission denied

- Verify username: `remote --show`
- Check SSH key permissions (should be 600 for private keys)
- Ensure user has access to remote paths

### Files not copying

- Check source path exists (for cpt/cpf)
- Verify remote path permissions
- Use absolute paths or ensure relative paths are correct

## Notes

- Session settings persist in `/tmp/` and survive across terminal sessions
- Cleared automatically on system reboot
- Use `--clear` to manually clear session when switching contexts
- The `con` command with an argument bypasses all session settings

## License

Alternative Intelligence Liberation Platform  
Open source - free to use and modify

---

For more tools, visit: https://github.com/randyhoggard/educational
