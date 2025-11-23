#!/bin/bash
# Randy's File Database Demo - Simple Version

echo "Randy's Simple File-Based Database Demo"
echo "======================================="
echo ""

# Create new demo database
echo "1. Creating new database..."
./setup_db.sh demo_simple

# Load sample data
echo ""
echo "2. Loading sample data..."
cd /home/randy/.filedb/demo_simple
./examples/sample_data.sh

echo ""
echo "3. Database operations:"

echo ""
echo "-- Database Stats --"
./bin/db.sh stats

echo ""
echo "-- List all keys --"
./bin/db.sh list

echo ""
echo "-- Get specific user --"
echo "user:1001 = $(./bin/db.sh get user:1001)"

echo ""
echo "-- Search for Engineers --"
./bin/db.sh search "Engineer"

echo ""
echo "-- Insert new test data --"
./bin/db.sh insert "demo:live" "Live demo from Randy's system!"

echo ""
echo "-- Retrieve test data --"
echo "demo:live = $(./bin/db.sh get demo:live)"

echo ""
echo "-- Final stats --"
./bin/db.sh stats

echo ""
echo "Demo complete! File-based database working perfectly."
echo "Location: /home/randy/.filedb/demo_simple"