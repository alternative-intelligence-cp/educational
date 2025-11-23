#!/bin/bash
# 🔧 Randy's Template Demo - Super Simple Version

echo "🔧 Randy's Template Versioning Demo"
echo "===================================="

# Load version
source ./tools/versioning/version.conf

VERSION="$MAJOR.$MINOR.$BUILD"
DATE=$(date +%Y-%m-%d)

echo "📋 Project: $PROJECT_NAME"
echo "📋 Version: $VERSION"
echo "📋 Date: $DATE"
echo ""

# Increment build
BUILD=$((BUILD + 1))
echo "🚀 Incrementing to build $BUILD"

# Update config file
sed -i "s/^BUILD=.*/BUILD=$BUILD/" ./tools/versioning/version.conf

# Process our demo files
echo "🔧 Processing templates..."

if [[ -f "examples/template_demo.c" ]]; then
    cp examples/template_demo.c examples/template_demo.c.backup
    
    sed -i \
        -e "s/\[\$PROJECT_NAME\$\]/$PROJECT_NAME/g" \
        -e "s/\[\$VERSION\$\]/$MAJOR.$MINOR.$BUILD/g" \
        -e "s/\[\$MAJOR\$\]/$MAJOR/g" \
        -e "s/\[\$MINOR\$\]/$MINOR/g" \
        -e "s/\[\$BUILD\$\]/$BUILD/g" \
        -e "s/\[\$DATE\$\]/$DATE/g" \
        -e "s/\[\$AUTHOR_NAME\$\]/$AUTHOR_NAME/g" \
        -e "s/\[\$COMPANY\$\]/$COMPANY/g" \
        -e "s/\[\$COPYRIGHT_YEAR\$\]/$COPYRIGHT_YEAR/g" \
        examples/template_demo.c
    
    echo "✨ Processed: examples/template_demo.c"
fi

echo ""
echo "📄 Result:"
head -n 15 examples/template_demo.c

# Restore original
if [[ -f "examples/template_demo.c.backup" ]]; then
    mv examples/template_demo.c.backup examples/template_demo.c
    echo ""
    echo "🔄 Original file restored"
fi

echo ""
echo "✅ Demo complete! Version is now $MAJOR.$MINOR.$BUILD"