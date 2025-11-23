#!/bin/bash
# 🗃️ Randy's File Database Demo
# Demonstrates a simple, effective database using just filesystems

set -e

# Colors
GREEN='\033[0;32m'
BLUE='\033[0;34m'
YELLOW='\033[1;33m'
CYAN='\033[0;36m'
NC='\033[0m'

print_header() { echo -e "${BLUE}━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━${NC}"; }
print_section() { echo -e "${CYAN}📦 $1${NC}"; }
print_success() { echo -e "${GREEN}✅ $1${NC}"; }
print_command() { echo -e "${YELLOW}🚀 $1${NC}"; }

clear
echo "🗃️ Randy's Simple File-Based Database"
echo "======================================"
echo ""
echo "🎯 Proof that simple solutions beat complex frameworks!"
echo "   • Hash-based partitioning for fast lookups"
echo "   • Pure filesystem storage - no SQL needed" 
echo "   • Automatic file splitting for large datasets"
echo "   • Built with basic Unix tools: AWK, sed, bash"
echo ""

print_header
print_section "Creating Database"
print_header

cd /home/randy/WORK_SPACE/work/tools/filedb

print_command "./setup_db.sh demo_db"
./setup_db.sh demo_db

echo ""
print_header
print_section "Loading Sample Data"
print_header

cd /home/randy/.filedb/demo_db

print_command "./examples/sample_data.sh"
./examples/sample_data.sh

echo ""
print_header  
print_section "Database Operations Demo"
print_header

print_command "./bin/db.sh stats"
./bin/db.sh stats

echo ""
print_command "./bin/db.sh list"
./bin/db.sh list

echo ""
print_command "./bin/db.sh get user:1001"
echo "Result: $(./bin/db.sh get user:1001)"

echo ""
print_command "./bin/db.sh search 'Engineer'"
./bin/db.sh search "Engineer"

echo ""
print_command "./bin/db.sh insert 'demo:test' 'Live demo data from Randy!'"
./bin/db.sh insert "demo:test" "Live demo data from Randy!"

echo ""
print_command "./bin/db.sh get demo:test"
echo "Result: $(./bin/db.sh get demo:test)"

echo ""
print_header
print_section "Hash Distribution Analysis"
print_header

echo "🔍 Data distribution across hash buckets:"
occupied_buckets=0
for bucket in data/*/; do
    if ls "$bucket"*.dat >/dev/null 2>&1; then
        bucket_name=$(basename "$bucket")
        file_count=$(ls -1 "$bucket"*.dat | wc -l)
        record_count=$(cat "$bucket"*.dat | grep -c "^KEY:" 2>/dev/null || echo 0)
        if [[ $record_count -gt 0 ]]; then
            echo "  Bucket $bucket_name: $record_count records in $file_count files"
            ((occupied_buckets++))
        fi
    fi
done

echo "  Total buckets with data: $occupied_buckets/256"
load_factor=$((occupied_buckets * 100 / 256))
echo "  Load factor: ${load_factor}%"

echo ""
print_header
print_section "Performance Characteristics"
print_header

echo "📊 Why this simple system works:"
echo "  • O(1) average lookup via hashing"
echo "  • Automatic load balancing across 256 buckets"
echo "  • File-level locking for concurrent access"
echo "  • Human-readable data format for debugging"
echo "  • No external dependencies - pure Unix tools"
echo "  • Scales to millions of records per bucket"

echo ""
print_success "File-based database demo complete!"
echo ""
echo "🏗️ Key Insights:"
echo "  1. Simple hash function distributes data evenly"
echo "  2. Filesystem provides built-in durability & concurrency"
echo "  3. AWK parsing is surprisingly fast for structured data"
echo "  4. 256 buckets handle up to ~25M records efficiently"
echo "  5. Pure text format makes debugging trivial"
echo ""
echo "🎯 Randy's Philosophy: Simple tools, properly applied, beat"
echo "   complex frameworks every time. This database proves it!"

print_header