#!/bin/bash
# Simple test script for versioning system

echo "🔧 Testing Randy's Versioning System"

# Test basic functionality
source /home/randy/WORK_SPACE/work/tools/versioning/version.conf

echo "Current version: $MAJOR.$MINOR.$BUILD"
echo "Project: $PROJECT_NAME"

# Test template replacement on a simple file
echo "Testing template replacement..."
echo "Version [$VERSION$] built on [$DATE$]" > test_template.txt

# Apply template replacement
VERSION="$MAJOR.$MINOR.$BUILD"
DATE=$(date +%Y-%m-%d)

sed -i \
    -e "s/\[\\\$VERSION\\\$\]/$VERSION/g" \
    -e "s/\[\\\$DATE\\\$\]/$DATE/g" \
    test_template.txt

echo "Result:"
cat test_template.txt
rm test_template.txt

echo "✅ Basic test complete!"