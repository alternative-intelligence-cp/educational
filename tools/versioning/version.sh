#!/bin/bash
# 🔧 Randy's Simple Bash Template Versioning System
# 🎯 Automates version management and build packaging
#
# Features:
# - Automatic version incrementing
# - Template variable replacement [$varName$] 
# - Automated builds with packaging
# - Simple configuration
# - Zero dependencies beyond standard tools

set -e  # Exit on any error

# Get script directory for relative paths
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
WORK_DIR="$(cd "$SCRIPT_DIR/../.." && pwd)"

# Load configuration
if [[ ! -f "$SCRIPT_DIR/version.conf" ]]; then
    echo "❌ Error: version.conf not found!"
    exit 1
fi

source "$SCRIPT_DIR/version.conf"

# Color output for better UX
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Print colored output
print_status() {
    echo -e "${BLUE}🔧${NC} $1"
}

print_success() {
    echo -e "${GREEN}✅${NC} $1"
}

print_warning() {
    echo -e "${YELLOW}⚠️${NC} $1"
}

print_error() {
    echo -e "${RED}❌${NC} $1"
}

# Show help
show_help() {
    echo "🔧 Randy's Bash Template Versioning System"
    echo ""
    echo "Usage: $0 [options]"
    echo ""
    echo "Options:"
    echo "  -h, --help        Show this help message"
    echo "  -m, --major       Increment major version (resets minor and build to 0)"
    echo "  -n, --minor       Increment minor version (resets build to 0)"
    echo "  -b, --build       Increment build version (default)"
    echo "  -s, --show        Show current version without incrementing"
    echo "  -t, --test        Test template replacement without building"
    echo "  -c, --clean       Clean previous builds"
    echo ""
    echo "Template Variables Available:"
    echo "  [\$MAJOR\$]         - Major version number"
    echo "  [\$MINOR\$]         - Minor version number"  
    echo "  [\$BUILD\$]         - Build version number"
    echo "  [\$VERSION\$]       - Full version string (MAJOR.MINOR.BUILD)"
    echo "  [\$DATE\$]          - Current date (YYYY-MM-DD)"
    echo "  [\$DATETIME\$]      - Current date and time"
    echo "  [\$PROJECT_NAME\$]  - Project name from config"
    echo "  [\$AUTHOR_NAME\$]   - Author name from config"
    echo "  [\$COMPANY\$]       - Company from config"
    echo "  [\$COPYRIGHT_YEAR\$] - Copyright year"
    echo ""
    echo "Examples:"
    echo "  $0                # Increment build version and create release"
    echo "  $0 --minor        # Increment minor version (1.0.5 -> 1.1.0)"
    echo "  $0 --major        # Increment major version (1.0.5 -> 2.0.0)"
    echo "  $0 --test         # Test template replacement only"
}

# Show current version
show_version() {
    echo "📋 Current Version Information:"
    echo "  Project: $PROJECT_NAME"
    echo "  Version: $MAJOR.$MINOR.$BUILD"
    echo "  Author: $AUTHOR_NAME"
    echo "  Date: $(date +%Y-%m-%d)"
}

# Update version numbers in config file
update_version_file() {
    local temp_file=$(mktemp)
    
    # Update the version numbers in place
    sed -E "s/^MAJOR=[0-9]+/MAJOR=$MAJOR/" "$SCRIPT_DIR/version.conf" > "$temp_file"
    sed -i -E "s/^MINOR=[0-9]+/MINOR=$MINOR/" "$temp_file"
    sed -i -E "s/^BUILD=[0-9]+/BUILD=$BUILD/" "$temp_file"
    
    mv "$temp_file" "$SCRIPT_DIR/version.conf"
    print_success "Updated version to $MAJOR.$MINOR.$BUILD"
}

# Replace template variables in files
replace_templates() {
    local test_mode=${1:-false}
    local files_processed=0
    
    # Generate dynamic values
    local VERSION="$MAJOR.$MINOR.$BUILD"
    local DATE=$(date +%Y-%m-%d)
    local DATETIME=$(date '+%Y-%m-%d %H:%M:%S')
    
    print_status "Replacing template variables..."
    
    # Find all source files
    cd "$WORK_DIR"
    for ext in $SOURCE_EXTENSIONS; do
        while IFS= read -r -d '' file; do
            # Skip if file matches exclude patterns
            local skip=false
            for pattern in $EXCLUDE_PATTERNS; do
                if [[ "$file" == *"$pattern"* ]]; then
                    skip=true
                    break
                fi
            done
            
            if [[ "$skip" == true ]]; then
                continue
            fi
            
            # Check if file contains template variables
            if grep -q '\[\$[A-Z_]*\$\]' "$file" 2>/dev/null; then
                if [[ "$test_mode" == true ]]; then
                    echo "  📝 Would process: $file"
                    grep -n '\[\$[A-Z_]*\$\]' "$file" | head -3
                else
                    # Create backup
                    cp "$file" "$file.backup"
                    
                    # Perform replacements
                    sed -i \
                        -e "s/\[\\\$MAJOR\\\$\]/$MAJOR/g" \
                        -e "s/\[\\\$MINOR\\\$\]/$MINOR/g" \
                        -e "s/\[\\\$BUILD\\\$\]/$BUILD/g" \
                        -e "s/\[\\\$VERSION\\\$\]/$VERSION/g" \
                        -e "s/\[\\\$DATE\\\$\]/$DATE/g" \
                        -e "s/\[\\\$DATETIME\\\$\]/$DATETIME/g" \
                        -e "s/\[\\\$PROJECT_NAME\\\$\]/$PROJECT_NAME/g" \
                        -e "s/\[\\\$AUTHOR_NAME\\\$\]/$AUTHOR_NAME/g" \
                        -e "s/\[\\\$COMPANY\\\$\]/$COMPANY/g" \
                        -e "s/\[\\\$COPYRIGHT_YEAR\\\$\]/$COPYRIGHT_YEAR/g" \
                        "$file"
                    
                    echo "  ✨ Processed: $file"
                fi
                ((files_processed++))
            fi
        done < <(find . -name "*.$ext" -type f -print0)
    done
    
    if [[ "$test_mode" == true ]]; then
        print_status "Test complete - $files_processed files would be processed"
    else

        print_success "Processed $files_processed files"
    fi
}

# Restore backup files
restore_backups() {
    print_status "Restoring original files..."
    cd "$WORK_DIR"
    find . -name "*.backup" -type f | while read -r backup; do
        original="${backup%.backup}"
        mv "$backup" "$original"
        echo "  🔄 Restored: $original"
    done
}

# Create build package
create_build_package() {
    local VERSION="$MAJOR.$MINOR.$BUILD"
    local build_dir="$WORK_DIR/$BUILDS_DIR"
    local package_name="${PROJECT_NAME// /_}-v$VERSION"
    local package_dir="$build_dir/$package_name"
    
    print_status "Creating build package..."
    
    # Create builds directory
    mkdir -p "$build_dir"
    
    # Clean old build with same version
    if [[ -d "$package_dir" ]]; then
        rm -rf "$package_dir"
    fi
    
    # Create package directory
    mkdir -p "$package_dir"
    
    # Copy source files
    for dir in $SOURCE_DIRS; do
        if [[ -d "$WORK_DIR/$dir" ]]; then
            cp -r "$WORK_DIR/$dir" "$package_dir/"
        fi
    done
    
    # Copy important files
    for file in README.md LICENSE VERSION CHANGELOG.md; do
        if [[ -f "$WORK_DIR/$file" ]]; then
            cp "$WORK_DIR/$file" "$package_dir/"
        fi
    done
    
    # Create version info file
    cat > "$package_dir/VERSION" << EOF
Project: $PROJECT_NAME
Version: $VERSION
Build Date: $(date '+%Y-%m-%d %H:%M:%S')
Author: $AUTHOR_NAME
Built with: Randy's Bash Template Versioning System
EOF
    
    # Create tarball
    cd "$build_dir"
    tar -czf "${package_name}.tar.gz" "$package_name"
    
    print_success "Created package: $build_dir/${package_name}.tar.gz"
    return 0
}

# Run build command
run_build() {
    print_status "Running build command: $BUILD_COMMAND"
    
    cd "$WORK_DIR"
    
    # Check if custom build script exists
    if [[ -f "$BUILD_COMMAND" ]]; then
        bash "$BUILD_COMMAND"
    elif command -v "$BUILD_COMMAND" &> /dev/null; then
        $BUILD_COMMAND
    else
        print_warning "Build command '$BUILD_COMMAND' not found - skipping build step"
        return 1
    fi
    
    print_success "Build completed successfully"
}

# Clean previous builds
clean_builds() {
    local build_dir="$WORK_DIR/$BUILDS_DIR"
    
    if [[ -d "$build_dir" ]]; then
        print_status "Cleaning previous builds..."
        rm -rf "$build_dir"/*
        print_success "Cleaned build directory"
    else
        print_warning "No builds directory to clean"
    fi
}

# Main execution logic
main() {
    local increment_type="build"
    local show_only=false
    local test_only=false
    local clean_only=false
    
    # Parse arguments
    while [[ $# -gt 0 ]]; do
        case $1 in
            -h|--help)
                show_help
                exit 0
                ;;
            -m|--major)
                increment_type="major"
                shift
                ;;
            -n|--minor)
                increment_type="minor"
                shift
                ;;
            -b|--build)
                increment_type="build"
                shift
                ;;
            -s|--show)
                show_only=true
                shift
                ;;
            -t|--test)
                test_only=true
                shift
                ;;
            -c|--clean)
                clean_only=true
                shift
                ;;
            *)
                print_error "Unknown option: $1"
                show_help
                exit 1
                ;;
        esac
    done
    
    # Show header
    echo "🔧 Randy's Bash Template Versioning System"
    echo "=============================================="
    
    # Handle clean operation
    if [[ "$clean_only" == true ]]; then
        clean_builds
        exit 0
    fi
    
    # Show current version
    show_version
    echo ""
    
    # Handle show-only operation
    if [[ "$show_only" == true ]]; then
        exit 0
    fi
    
    # Handle test-only operation
    if [[ "$test_only" == true ]]; then
        replace_templates true
        exit 0
    fi
    
    # Increment version
    case $increment_type in
        major)
            MAJOR=$((MAJOR + 1))
            MINOR=0
            BUILD=0
            ;;
        minor)
            MINOR=$((MINOR + 1))
            BUILD=0
            ;;
        build)
            BUILD=$((BUILD + 1))
            ;;
    esac
    
    print_status "Incrementing $increment_type version..."
    
    # Update version file
    update_version_file
    
    # Replace templates in source
    replace_templates false
    
    # Create build package
    if create_build_package; then
        # Run build if successful
        run_build
    fi
    
    # Restore original files
    restore_backups
    
    echo ""
    print_success "Version $MAJOR.$MINOR.$BUILD release complete! 🎉"
    
    # Show final build info
    local VERSION="$MAJOR.$MINOR.$BUILD"
    local package_name="${PROJECT_NAME// /_}-v$VERSION"
    echo ""
    echo "📦 Build artifacts:"
    echo "  • Package: $BUILDS_DIR/${package_name}.tar.gz"
    echo "  • Version: $VERSION"
    echo "  • Date: $(date)"
}

# Run main function with all arguments
main "$@"