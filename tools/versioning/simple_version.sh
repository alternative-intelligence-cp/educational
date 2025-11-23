#!/bin/bash
# 🔧 Randy's Simple Template Versioning System (Fixed)
# 🎯 Automates version management and build packaging

set -e

# Get script directory
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
WORK_DIR="$(cd "$SCRIPT_DIR/../.." && pwd)"

# Load configuration
source "$SCRIPT_DIR/version.conf"

# Colors
GREEN='\033[0;32m'
BLUE='\033[0;34m'
YELLOW='\033[1;33m'
NC='\033[0m'

print_status() { echo -e "${BLUE}🔧${NC} $1"; }
print_success() { echo -e "${GREEN}✅${NC} $1"; }
print_warning() { echo -e "${YELLOW}⚠️${NC} $1"; }

show_help() {
    echo "🔧 Randy's Simple Template Versioning System"
    echo "Usage: $0 [--major|--minor|--build|--show|--test]"
    echo ""
    echo "Template Variables:"
    echo "  [MAJOR]    [MINOR]    [BUILD]     [VERSION]"
    echo "  [DATE]     [PROJECT]  [AUTHOR]"
}

show_version() {
    echo "📋 Current Version: $MAJOR.$MINOR.$BUILD"
    echo "📋 Project: $PROJECT_NAME"
    echo "📋 Author: $AUTHOR_NAME"
}

update_version_file() {
    sed -i "s/^MAJOR=.*/MAJOR=$MAJOR/" "$SCRIPT_DIR/version.conf"
    sed -i "s/^MINOR=.*/MINOR=$MINOR/" "$SCRIPT_DIR/version.conf"  
    sed -i "s/^BUILD=.*/BUILD=$BUILD/" "$SCRIPT_DIR/version.conf"
    print_success "Updated to version $MAJOR.$MINOR.$BUILD"
}

process_templates() {
    local test_mode=${1:-false}
    local VERSION="$MAJOR.$MINOR.$BUILD"
    local DATE=$(date +%Y-%m-%d)
    local DATETIME=$(date '+%Y-%m-%d %H:%M:%S')
    
    print_status "Processing template files..."
    
    cd "$WORK_DIR"
    
    # Find files with template variables
    local count=0
    for ext in c h py js md html; do
        find . -name "*.$ext" -type f | while read -r file; do
            if grep -q '\[\$.*\$\]' "$file" 2>/dev/null; then
                if [[ "$test_mode" == "true" ]]; then
                    echo "  📝 Would process: $file"
                else
                    # Create backup
                    cp "$file" "$file.backup"
                    
                    # Simple sed replacements
                    sed -i \
                        -e "s/\[\$MAJOR\$\]/$MAJOR/g" \
                        -e "s/\[\$MINOR\$\]/$MINOR/g" \
                        -e "s/\[\$BUILD\$\]/$BUILD/g" \
                        -e "s/\[\$VERSION\$\]/$VERSION/g" \
                        -e "s/\[\$DATE\$\]/$DATE/g" \
                        -e "s/\[\$DATETIME\$\]/$DATETIME/g" \
                        -e "s/\[\$PROJECT_NAME\$\]/$PROJECT_NAME/g" \
                        -e "s/\[\$AUTHOR_NAME\$\]/$AUTHOR_NAME/g" \
                        -e "s/\[\$COMPANY\$\]/$COMPANY/g" \
                        -e "s/\[\$COPYRIGHT_YEAR\$\]/$COPYRIGHT_YEAR/g" \
                        "$file"
                    
                    echo "  ✨ Processed: $file"
                fi
                count=$((count + 1))
            fi
        done
    done
    
    if [[ "$test_mode" == "true" ]]; then
        print_status "Test mode - no files modified"
    else
        print_success "Template processing complete"
    fi
}

create_release() {
    local VERSION="$MAJOR.$MINOR.$BUILD"
    local BUILD_DIR="$WORK_DIR/builds"
    local PACKAGE_NAME="Educational_Data_Structures-v$VERSION"
    
    print_status "Creating release package..."
    
    mkdir -p "$BUILD_DIR"
    rm -rf "$BUILD_DIR/$PACKAGE_NAME"
    mkdir -p "$BUILD_DIR/$PACKAGE_NAME"
    
    # Copy source files
    cp -r src "$BUILD_DIR/$PACKAGE_NAME/" 2>/dev/null || true
    cp -r examples "$BUILD_DIR/$PACKAGE_NAME/" 2>/dev/null || true
    cp -r docs "$BUILD_DIR/$PACKAGE_NAME/" 2>/dev/null || true
    
    # Copy key files
    for file in README.md LICENSE; do
        [[ -f "$file" ]] && cp "$file" "$BUILD_DIR/$PACKAGE_NAME/"
    done
    
    # Create VERSION file
    cat > "$BUILD_DIR/$PACKAGE_NAME/VERSION" << EOF
Project: $PROJECT_NAME
Version: $VERSION
Date: $(date)
Author: $AUTHOR_NAME
EOF
    
    # Create tarball
    cd "$BUILD_DIR"
    tar -czf "${PACKAGE_NAME}.tar.gz" "$PACKAGE_NAME"
    
    print_success "Created: builds/${PACKAGE_NAME}.tar.gz"
}

restore_files() {
    print_status "Restoring original files..."
    cd "$WORK_DIR"
    find . -name "*.backup" -type f | while read -r backup; do
        mv "$backup" "${backup%.backup}"
    done
    print_success "Files restored"
}

main() {
    echo "🔧 Randy's Simple Template Versioning System"
    echo "=============================================="
    
    case "${1:-build}" in
        --help|-h)
            show_help
            exit 0
            ;;
        --show|-s)
            show_version
            exit 0
            ;;
        --test|-t)
            show_version
            echo ""
            process_templates true
            exit 0
            ;;
        --major|-m)
            MAJOR=$((MAJOR + 1))
            MINOR=0
            BUILD=0
            ;;
        --minor|-n)
            MINOR=$((MINOR + 1))
            BUILD=0
            ;;
        --build|-b|*)
            BUILD=$((BUILD + 1))
            ;;
    esac
    
    show_version
    echo ""
    
    update_version_file
    process_templates false
    create_release
    restore_files
    
    echo ""
    print_success "Release v$MAJOR.$MINOR.$BUILD complete! 🎉"
}

main "$@"