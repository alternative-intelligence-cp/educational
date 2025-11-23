#!/bin/bash
# 🗃️ Randy's Simple File-Based Database - Setup Script
# 🎯 Creates a filesystem-based database with hashing and partitioning

set -e

DB_NAME=${1:-"simpledb"}
DB_ROOT="$HOME/.filedb"
DB_PATH="$DB_ROOT/$DB_NAME"

# Configuration
BUCKET_COUNT=256  # Number of hash buckets (folders)
MAX_FILE_SIZE=1048576  # 1MB max file size before splitting
RECORD_SEPARATOR="\n---\n"

# Colors for output
GREEN='\033[0;32m'
BLUE='\033[0;34m'
YELLOW='\033[1;33m'
NC='\033[0m'

print_status() { echo -e "${BLUE}🗃️${NC} $1"; }
print_success() { echo -e "${GREEN}✅${NC} $1"; }
print_warning() { echo -e "${YELLOW}⚠️${NC} $1"; }

show_help() {
    echo "🗃️ Randy's Simple File-Based Database Setup"
    echo "Usage: $0 [database_name]"
    echo ""
    echo "Creates a simple filesystem database with:"
    echo "  • Hash-based partitioning (256 buckets)"
    echo "  • Automatic file splitting at 1MB"
    echo "  • Simple key-value storage"
    echo "  • Basic indexing support"
    echo ""
    echo "Example:"
    echo "  $0 myapp          # Creates ~/.filedb/myapp/"
    echo "  $0                # Creates ~/.filedb/simpledb/"
}

create_hash_function() {
    cat > "$DB_PATH/bin/hash.sh" << 'EOF'
#!/bin/bash
# Simple hash function for key distribution
key="$1"
# Simple hash: sum ASCII values mod bucket count
hash=0
for (( i=0; i<${#key}; i++ )); do
    char="${key:$i:1}"
    ascii=$(printf "%d" "'$char")
    hash=$((hash + ascii))
done
echo $((hash % 256))
EOF
    chmod +x "$DB_PATH/bin/hash.sh"
}

create_db_client() {
    cat > "$DB_PATH/bin/db.sh" << 'EOF'
#!/bin/bash
# 🗃️ Randy's Simple Database Client
# Usage: db.sh <command> [args...]

DB_PATH="$(dirname "$(dirname "$(realpath "$0")")")"
BUCKET_COUNT=256
MAX_FILE_SIZE=1048576
RECORD_SEP="---"

# Get hash bucket for a key
get_bucket() {
    local key="$1"
    local hash=0
    for (( i=0; i<${#key}; i++ )); do
        char="${key:$i:1}"
        ascii=$(printf "%d" "'$char")
        hash=$((hash + ascii))
    done
    printf "%03d" $((hash % BUCKET_COUNT))
}

# Find or create data file in bucket
get_data_file() {
    local bucket="$1"
    local bucket_dir="$DB_PATH/data/$bucket"
    
    # Find file with space or create new one
    local file_num=0
    while true; do
        local file="$bucket_dir/$(printf "%03d" $file_num).dat"
        
        if [[ ! -f "$file" ]] || [[ $(stat -f%z "$file" 2>/dev/null || stat -c%s "$file" 2>/dev/null) -lt $MAX_FILE_SIZE ]]; then
            echo "$file"
            return
        fi
        ((file_num++))
    done
}

# Insert key-value pair
insert() {
    local key="$1"
    local value="$2"
    
    if [[ -z "$key" ]] || [[ -z "$value" ]]; then
        echo "❌ Usage: insert <key> <value>"
        return 1
    fi
    
    local bucket=$(get_bucket "$key")
    local data_file=$(get_data_file "$bucket")
    local timestamp=$(date '+%Y-%m-%d %H:%M:%S')
    
    # Append record
    {
        echo "KEY: $key"
        echo "TIMESTAMP: $timestamp"
        echo "VALUE: $value"
        echo "$RECORD_SEP"
    } >> "$data_file"
    
    # Update index
    echo "$key:$bucket:$(basename "$data_file"):$(date +%s)" >> "$DB_PATH/index/keys.idx"
    
    echo "✅ Inserted: $key -> $(basename $(dirname "$data_file"))/$(basename "$data_file")"
}

# Get value by key
get() {
    local key="$1"
    
    if [[ -z "$key" ]]; then
        echo "❌ Usage: get <key>"
        return 1
    fi
    
    local bucket=$(get_bucket "$key")
    local bucket_dir="$DB_PATH/data/$bucket"
    
    # Search all files in bucket
    for data_file in "$bucket_dir"/*.dat; do
        [[ -f "$data_file" ]] || continue
        
        # Use awk to find the record
        awk -v key="$key" -v RS="---" '
        /^KEY: / {
            if ($0 ~ "KEY: " key "$") {
                print $0
                found = 1
                exit
            }
        }' "$data_file" | while read -r line; do
            if [[ "$line" =~ ^VALUE:\ (.+) ]]; then
                echo "${BASH_REMATCH[1]}"
                return 0
            fi
        done
    done
    
    echo "❌ Key not found: $key"
    return 1
}

# Search for keys matching pattern
search() {
    local pattern="$1"
    
    if [[ -z "$pattern" ]]; then
        echo "❌ Usage: search <pattern>"
        return 1
    fi
    
    echo "🔍 Searching for pattern: $pattern"
    
    for bucket_dir in "$DB_PATH/data"/*; do
        [[ -d "$bucket_dir" ]] || continue
        
        for data_file in "$bucket_dir"/*.dat; do
            [[ -f "$data_file" ]] || continue
            
            awk -v pattern="$pattern" -v RS="---" '
            /^KEY: / {
                key_line = $0
                value_line = ""
                for(i=1; i<=NF; i++) {
                    if($i == "VALUE:") {
                        value_line = substr($0, index($0, "VALUE:") + 7)
                        break
                    }
                }
                if(key_line ~ pattern || value_line ~ pattern) {
                    print "🔑 " substr(key_line, 6) " -> " value_line
                }
            }' "$data_file"
        done
    done
}

# List all keys
list() {
    echo "📋 All keys in database:"
    
    for bucket_dir in "$DB_PATH/data"/*; do
        [[ -d "$bucket_dir" ]] || continue
        
        for data_file in "$bucket_dir"/*.dat; do
            [[ -f "$data_file" ]] || continue
            
            awk -v RS="---" '/^KEY: / { print "🔑 " substr($1, 5) " (" FILENAME ")" }' "$data_file"
        done
    done
}

# Show database stats
stats() {
    echo "📊 Database Statistics:"
    echo "  Database: $(basename "$DB_PATH")"
    echo "  Location: $DB_PATH"
    
    local total_keys=0
    local total_files=0
    local total_size=0
    
    for bucket_dir in "$DB_PATH/data"/*; do
        [[ -d "$bucket_dir" ]] || continue
        
        for data_file in "$bucket_dir"/*.dat; do
            [[ -f "$data_file" ]] || continue
            
            ((total_files++))
            local file_size=$(stat -f%z "$data_file" 2>/dev/null || stat -c%s "$data_file" 2>/dev/null)
            total_size=$((total_size + file_size))
            
            local keys_in_file=$(grep -c "^KEY:" "$data_file" 2>/dev/null || echo 0)
            total_keys=$((total_keys + keys_in_file))
        done
    done
    
    echo "  Total Keys: $total_keys"
    echo "  Total Files: $total_files"
    echo "  Total Size: $((total_size / 1024)) KB"
    echo "  Buckets: $(ls -1d "$DB_PATH/data"/* 2>/dev/null | wc -l)"
}

# Show help
show_help() {
    echo "🗃️ Randy's Simple File Database Client"
    echo "Database: $(basename "$DB_PATH")"
    echo ""
    echo "Commands:"
    echo "  insert <key> <value>  - Insert/update a key-value pair"
    echo "  get <key>            - Retrieve value for key"
    echo "  search <pattern>     - Search keys/values with regex"
    echo "  list                 - List all keys"
    echo "  stats                - Show database statistics"
    echo "  help                 - Show this help"
    echo ""
    echo "Examples:"
    echo "  $0 insert user:1001 'John Doe'"
    echo "  $0 get user:1001"
    echo "  $0 search 'user:.*'"
    echo "  $0 list"
}

# Main command dispatch
case "${1:-help}" in
    insert)
        insert "$2" "$3"
        ;;
    get)
        get "$2"
        ;;
    search)
        search "$2"
        ;;
    list)
        list
        ;;
    stats)
        stats
        ;;
    help|--help|-h)
        show_help
        ;;
    *)
        echo "❌ Unknown command: $1"
        show_help
        exit 1
        ;;
esac
EOF
    chmod +x "$DB_PATH/bin/db.sh"
}

create_example_data() {
    cat > "$DB_PATH/examples/sample_data.sh" << 'EOF'
#!/bin/bash
# Sample data for testing the file database

DB="$(dirname "$0")/../bin/db.sh"

echo "🗃️ Loading sample data..."

# User records
$DB insert "user:1001" "John Doe, Software Engineer, john@example.com"
$DB insert "user:1002" "Jane Smith, Data Scientist, jane@example.com"  
$DB insert "user:1003" "Bob Wilson, DevOps Engineer, bob@example.com"

# Configuration
$DB insert "config:max_users" "1000"
$DB insert "config:debug_mode" "true"
$DB insert "config:api_key" "abc123xyz789"

# Session data  
$DB insert "session:abc123" "user_id:1001,login_time:2025-11-23,ip:192.168.1.100"
$DB insert "session:def456" "user_id:1002,login_time:2025-11-23,ip:192.168.1.101"

# Product catalog
$DB insert "product:laptop" "Brand: Dell, Model: XPS 13, Price: $999"
$DB insert "product:mouse" "Brand: Logitech, Model: MX Master, Price: $79"
$DB insert "product:keyboard" "Brand: Mechanical, Model: Cherry MX, Price: $149"

echo "✅ Sample data loaded! Try:"
echo "  $DB list"
echo "  $DB get user:1001"
echo "  $DB search 'user:.*'"
echo "  $DB stats"
EOF
    chmod +x "$DB_PATH/examples/sample_data.sh"
}

main() {
    echo "🗃️ Randy's Simple File-Based Database Setup"
    echo "=============================================="
    
    if [[ "$1" == "--help" || "$1" == "-h" ]]; then
        show_help
        exit 0
    fi
    
    print_status "Setting up database: $DB_NAME"
    print_status "Location: $DB_PATH"
    
    # Create directory structure
    mkdir -p "$DB_ROOT"
    
    if [[ -d "$DB_PATH" ]]; then
        print_warning "Database already exists, updating structure..."
    fi
    
    # Create main directories
    mkdir -p "$DB_PATH"/{data,index,bin,logs,examples}
    
    # Create hash buckets
    print_status "Creating $BUCKET_COUNT hash buckets..."
    for ((i=0; i<BUCKET_COUNT; i++)); do
        printf -v bucket_name "%03d" $i
        mkdir -p "$DB_PATH/data/$bucket_name"
    done
    
    # Create utility scripts
    print_status "Creating database tools..."
    create_hash_function
    create_db_client
    create_example_data
    
    # Create configuration file
    cat > "$DB_PATH/db.conf" << EOF
# Database Configuration
DB_NAME=$DB_NAME
BUCKET_COUNT=$BUCKET_COUNT
MAX_FILE_SIZE=$MAX_FILE_SIZE
CREATED=$(date)
VERSION=1.0
EOF
    
    # Create README
    cat > "$DB_PATH/README.md" << EOF
# $DB_NAME Database

Simple file-based database created with Randy's system.

## Quick Start

\`\`\`bash
# Insert data
./bin/db.sh insert "mykey" "myvalue"

# Get data  
./bin/db.sh get "mykey"

# Search
./bin/db.sh search "pattern"

# List all
./bin/db.sh list

# Statistics
./bin/db.sh stats
\`\`\`

## Load Sample Data

\`\`\`bash
./examples/sample_data.sh
\`\`\`

## Database Structure

- **data/**: Hash buckets (000-255) containing data files
- **index/**: Simple index files for faster lookups
- **bin/**: Database client tools
- **logs/**: Operation logs (future)

Created: $(date)
EOF
    
    print_success "Database '$DB_NAME' created successfully!"
    echo ""
    echo "📋 Quick start:"
    echo "  cd $DB_PATH"
    echo "  ./examples/sample_data.sh    # Load sample data"
    echo "  ./bin/db.sh list             # List all keys"
    echo "  ./bin/db.sh get user:1001    # Get specific value"
    echo "  ./bin/db.sh stats            # Show statistics"
    echo ""
    echo "🗂️ Database location: $DB_PATH"
}

main "$@"