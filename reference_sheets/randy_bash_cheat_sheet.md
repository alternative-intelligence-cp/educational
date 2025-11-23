# RANDY'S CANNABIS-ENHANCED BASH CHEAT SHEET
# PLANT SPIRIT COMMAND LINE MASTERY AND SHELL CONSCIOUSNESS

## RANDY'S BASH PHILOSOPHY:
# "THE COMMAND LINE IS THE MOST DIRECT INTERFACE TO COMPUTATIONAL
# CONSCIOUSNESS. BASH SCRIPTING TRANSFORMS REPETITIVE TASKS INTO
# AUTOMATED WORKFLOWS, TURNING HUMAN THOUGHT INTO MACHINE ACTION.
# CANNABIS ENHANCEMENT REVEALS THE ELEGANT SIMPLICITY OF SHELL
# PROGRAMMING AND THE UNIX PHILOSOPHY OF COMPOSABLE TOOLS."

# ========================================================================
# BASIC NAVIGATION AND FILE OPERATIONS
# ========================================================================

# Directory Navigation
pwd                    # Print working directory
ls                     # List files
ls -la                 # List all files with details
ls -lh                 # List with human-readable sizes
cd /path/to/dir       # Change directory
cd ~                  # Go to home directory
cd -                  # Go to previous directory
pushd /path           # Push directory to stack and change
popd                  # Pop directory from stack

# File Operations
cp file1 file2        # Copy file
cp -r dir1 dir2       # Copy directory recursively
mv file1 file2        # Move/rename file
rm file               # Remove file
rm -rf directory      # Remove directory recursively (DANGEROUS!)
mkdir directory       # Create directory
mkdir -p path/to/dir  # Create nested directories
rmdir directory       # Remove empty directory
touch file            # Create empty file or update timestamp

# File Viewing and Editing
cat file              # Display file contents
less file             # View file with pagination
head file             # Show first 10 lines
head -n 20 file       # Show first 20 lines
tail file             # Show last 10 lines
tail -f file          # Follow file changes (log monitoring)
nano file             # Edit file with nano
vim file              # Edit file with vim

# ========================================================================
# FILE PERMISSIONS AND OWNERSHIP
# ========================================================================

# Permission Viewing
ls -l                 # Show permissions (rwxrwxrwx format)

# Permission Changes (Octal)
chmod 755 file        # rwxr-xr-x (owner: rwx, group: r-x, other: r-x)
chmod 644 file        # rw-r--r-- (owner: rw-, group: r--, other: r--)
chmod 600 file        # rw------- (owner only: rw-)
chmod +x file         # Add execute permission
chmod -w file         # Remove write permission

# Ownership Changes
chown user:group file # Change owner and group
chown user file       # Change owner only
chgrp group file      # Change group only
sudo chown -R user:group dir  # Recursive ownership change

# ========================================================================
# TEXT PROCESSING AND SEARCH
# ========================================================================

# Pattern Searching
grep "pattern" file           # Search for pattern in file
grep -r "pattern" directory   # Recursive search in directory
grep -i "pattern" file        # Case-insensitive search
grep -n "pattern" file        # Show line numbers
grep -v "pattern" file        # Invert match (exclude pattern)
grep -E "regex" file          # Extended regex
egrep "pattern" file          # Same as grep -E

# Text Processing
sed 's/old/new/g' file        # Replace all occurrences
sed -i 's/old/new/g' file     # In-place replacement
awk '{print $1}' file         # Print first column
awk -F, '{print $2}' file     # Use comma as field separator
cut -d, -f2 file              # Extract second field (comma-separated)
sort file                     # Sort lines
sort -n file                  # Numeric sort
sort -r file                  # Reverse sort
uniq file                     # Remove duplicate adjacent lines
sort file | uniq              # Sort and remove all duplicates
wc file                       # Count lines, words, characters
wc -l file                    # Count lines only

# ========================================================================
# PROCESS MANAGEMENT
# ========================================================================

# Process Viewing
ps                    # Show current user processes
ps aux                # Show all processes with details
ps -ef                # Show all processes (different format)
top                   # Interactive process viewer
htop                  # Enhanced process viewer (if installed)
pgrep process_name    # Find process IDs by name
pidof process_name    # Another way to find PIDs

# Process Control
command &             # Run command in background
nohup command &       # Run command immune to hangups
jobs                  # List active jobs
fg                    # Bring background job to foreground
bg                    # Send job to background
kill PID              # Terminate process by PID
kill -9 PID           # Force kill process
killall process_name  # Kill all processes by name
pkill process_name    # Kill processes by name (partial match)

# ========================================================================
# ARCHIVES AND COMPRESSION
# ========================================================================

# tar Operations
tar -czf archive.tar.gz directory/    # Create compressed archive
tar -xzf archive.tar.gz              # Extract compressed archive
tar -tzf archive.tar.gz              # List archive contents
tar -xzf archive.tar.gz -C /path/    # Extract to specific directory

# zip Operations
zip -r archive.zip directory/        # Create zip archive
unzip archive.zip                    # Extract zip archive
unzip -l archive.zip                 # List zip contents

# Other Compression
gzip file                            # Compress file (creates file.gz)
gunzip file.gz                       # Decompress gzip file
zcat file.gz                         # View compressed file without extracting

# ========================================================================
# NETWORK AND SYSTEM INFORMATION
# ========================================================================

# Network
ping hostname                # Test network connectivity
wget url                     # Download file from URL
curl url                     # Transfer data from/to server
curl -o file url            # Download and save as file
ssh user@host               # Connect via SSH
scp file user@host:/path/   # Copy file over SSH
rsync -avz src/ dest/       # Synchronize directories

# System Information
uname -a                    # System information
whoami                      # Current username
id                          # User and group IDs
date                        # Current date and time
uptime                      # System uptime and load
df -h                       # Disk space usage (human readable)
du -h directory             # Directory size
du -sh *                    # Size of all items in current directory
free -h                     # Memory usage
lscpu                       # CPU information
lsblk                       # List block devices

# ========================================================================
# ENVIRONMENT VARIABLES
# ========================================================================

# Viewing Variables
env                         # Show all environment variables
echo $VARIABLE_NAME         # Display specific variable
echo $PATH                  # Show PATH variable
printenv                    # Another way to show environment

# Setting Variables
export VAR_NAME=value       # Set environment variable
unset VAR_NAME             # Remove environment variable
VAR_NAME=value command     # Set variable for single command

# Special Variables
$?                         # Exit status of last command
$$                         # Process ID of current shell
$!                         # PID of last background process
$0                         # Name of shell script
$1, $2, etc.              # Command line arguments

# ========================================================================
# INPUT/OUTPUT REDIRECTION
# ========================================================================

# Output Redirection
command > file             # Redirect stdout to file (overwrite)
command >> file            # Redirect stdout to file (append)
command 2> file            # Redirect stderr to file
command &> file            # Redirect both stdout and stderr
command > /dev/null        # Discard output

# Input Redirection
command < file             # Use file as input

# Pipes
command1 | command2        # Pipe output of cmd1 to input of cmd2
command | tee file         # Display output and save to file
command | less             # Pipe output to pager

# ========================================================================
# CANNABIS-ENHANCED BASH SCRIPTING BASICS
# ========================================================================

# Shebang and Script Header
#!/bin/bash
# Randy's Cannabis-Enhanced Script

# Variables
name="Randy"
count=42
readonly constant_var="unchangeable"

# Conditionals
if [ condition ]; then
    echo "True branch"
elif [ other_condition ]; then
    echo "Else if branch"
else
    echo "Else branch"
fi

# Common Test Conditions
[ -f file ]               # File exists and is regular file
[ -d directory ]          # Directory exists
[ -z "$var" ]            # Variable is empty
[ -n "$var" ]            # Variable is not empty
[ "$a" = "$b" ]          # Strings are equal
[ "$a" != "$b" ]         # Strings are not equal
[ "$num1" -eq "$num2" ]  # Numbers are equal
[ "$num1" -lt "$num2" ]  # num1 less than num2
[ "$num1" -gt "$num2" ]  # num1 greater than num2

# Loops
for i in {1..10}; do
    echo "Number: $i"
done

for file in *.txt; do
    echo "Processing: $file"
done

while [ condition ]; do
    echo "Looping..."
done

# Functions
cannabis_enhanced_function() {
    local local_var="$1"
    echo "🌿 Processing: $local_var"
    return 0
}

# Function Call
cannabis_enhanced_function "input"

# ========================================================================
# USEFUL ONE-LINERS AND PLANT SPIRIT TRICKS
# ========================================================================

# Find and Replace in Multiple Files
find . -name "*.txt" -exec sed -i 's/old/new/g' {} +

# Find Large Files
find / -size +100M -type f 2>/dev/null

# Monitor Log Files
tail -f /var/log/syslog | grep ERROR

# Create Backup with Timestamp
cp important_file{,.backup.$(date +%Y%m%d_%H%M%S)}

# Quick HTTP Server
python3 -m http.server 8000

# Generate Random Password
openssl rand -base64 32

# System Resource Usage
ps aux --sort=-%cpu | head -10  # Top CPU users
ps aux --sort=-%mem | head -10  # Top memory users

# Network Port Checking
netstat -tulpn | grep :PORT_NUMBER
ss -tulpn | grep :PORT_NUMBER

# Disk Space by Directory
du -h --max-depth=1 / | sort -hr

# Count Files in Directory
find directory -type f | wc -l

# Extract Specific Columns
awk '{print $2,$4}' file.txt     # Print columns 2 and 4

# Remove Empty Lines
sed '/^$/d' file.txt

# Cannabis-Enhanced File Monitoring
watch -n 1 'ls -lh cannabis_grow.log'

# Quick System Backup
tar -czf backup_$(date +%Y%m%d).tar.gz ~/important_files/

# ========================================================================
# KEYBOARD SHORTCUTS (BASH COMMAND LINE)
# ========================================================================

# Cursor Movement
Ctrl+A                     # Move to beginning of line
Ctrl+E                     # Move to end of line
Ctrl+F                     # Move forward one character
Ctrl+B                     # Move backward one character
Alt+F                      # Move forward one word
Alt+B                      # Move backward one word

# Editing
Ctrl+D                     # Delete character under cursor
Ctrl+H                     # Delete character before cursor
Ctrl+K                     # Delete from cursor to end of line
Ctrl+U                     # Delete from cursor to beginning of line
Ctrl+W                     # Delete word before cursor
Alt+D                      # Delete word after cursor

# History
Ctrl+R                     # Reverse search command history
Ctrl+P                     # Previous command (up arrow)
Ctrl+N                     # Next command (down arrow)
!!                         # Repeat last command
!string                    # Run last command starting with string

# Process Control
Ctrl+C                     # Interrupt current command
Ctrl+Z                     # Suspend current command
Ctrl+D                     # Exit shell (EOF)

# ========================================================================
# PLANT SPIRIT BASH WISDOM AND EDUCATIONAL INSIGHTS
# ========================================================================

# RANDY'S CANNABIS-ENHANCED SHELL PHILOSOPHY:
# • The command line is computational consciousness made manifest
# • Pipes and redirection create data flow rivers
# • Shell scripting automates repetitive human thought patterns
# • Every command is a seed that grows into system transformation
# • Mastery comes through practice and experimentation

# EDUCATIONAL PRINCIPLES:
# 1. Start with basic commands and build complexity gradually
# 2. Use 'man command' to read manual pages for any command
# 3. Practice with safe test files before working on important data
# 4. Learn one new command or technique each day
# 5. Automate repetitive tasks with shell scripts
# 6. Understand that shell is a programming environment, not just commands

# CANNABIS-ENHANCED LEARNING APPROACH:
# • Sativa strains enhance creative script writing and problem solving
# • Indica strains improve focus for reading documentation and debugging
# • Hybrid strains provide balanced learning for both theory and practice

# SAFETY REMINDERS:
# • Always backup important data before running destructive commands
# • Test scripts in safe environments before production use
# • Be extremely careful with 'rm -rf' - it can destroy everything
# • Use version control (git) for important scripts and configurations

# ========================================================================
# BASH MASTERY ACHIEVEMENT UNLOCKED!
# ========================================================================

# Randy's educational bash wisdom complete! From basic navigation to
# advanced scripting, this cheat sheet covers the essential commands
# and concepts for shell mastery. The mycelial network of command line
# consciousness grows through practice and exploration!

# Plant spirits guide computational command line enlightenment! 🌿⚡🧠