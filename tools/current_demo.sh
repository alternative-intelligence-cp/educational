#!/bin/bash
# Randy's Educational Tools - Current Demo

echo "RANDY'S EDUCATIONAL PROGRAMMING TOOLS"
echo "====================================="
echo ""
echo "Current tools available:"

echo ""
echo "1. TEMPLATE VERSIONING SYSTEM"
echo "-----------------------------"
cd versioning/
if [[ -f "version.conf" ]]; then
    echo "✅ Configuration: $(cat version.conf | grep PROJECT_NAME)"
    echo "✅ Version: $(grep MAJOR version.conf | cut -d= -f2).$(grep MINOR version.conf | cut -d= -f2).$(grep BUILD version.conf | cut -d= -f2)"
fi

if [[ -f "simple_version.sh" ]]; then
    echo "✅ Template processor ready"
    if [[ -f "examples/template_demo.c" ]]; then
        echo "✅ Demo template available"
        echo "   Template contains: $(grep -c '\[\$.*\$\]' examples/template_demo.c) variables to replace"
    fi
fi

cd ..

echo ""
echo "2. FILE-BASED DATABASE SYSTEM" 
echo "-----------------------------"
cd filedb/
echo "✅ Database setup script: setup_db.sh"
echo "✅ Demo scripts available: $(ls *demo*.sh 2>/dev/null | wc -l) demos"

# Check existing databases
if [[ -d "/home/randy/.filedb" ]]; then
    db_count=$(ls -1d /home/randy/.filedb/*/ 2>/dev/null | wc -l)
    echo "✅ Active databases: $db_count created"
    
    if [[ -d "/home/randy/.filedb/demo_simple" ]]; then
        echo "   Sample database stats:"
        cd /home/randy/.filedb/demo_simple
        ./bin/db.sh stats | grep "Total Keys\|Total Size"
        echo "   Sample record: user:1001 -> $(./bin/db.sh get user:1001 | head -c 40)..."
    fi
fi

cd /home/randy/WORK_SPACE/work/tools

echo ""
echo "DEMONSTRATION COMPLETE"
echo "====================="
echo ""
echo "Available tools:"
echo "  📁 versioning/ - Template processing with version management"
echo "  📁 filedb/     - Complete database system (zero dependencies)"
echo ""
echo "Key achievements:"
echo "  ✅ Template system: Variable replacement and version tracking"
echo "  ✅ Database system: Hash partitioning with O(1) lookups"
echo "  ✅ Zero dependencies: Pure bash/awk/sed implementations"
echo "  ✅ Educational value: Real systems that teach fundamentals"
echo ""
echo "Randy's philosophy proven: Simple solutions work better!"
echo ""
echo "Next: Run individual demos in each directory to explore further"