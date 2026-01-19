#!/bin/bash
# Quick setup script for Wave Interference Simulator

echo "=================================="
echo "Wave Interference Simulator Setup"
echo "=================================="
echo ""

# Check Python version
echo "Checking Python installation..."
if ! command -v python3 &> /dev/null; then
    echo "❌ Python 3 not found. Please install Python 3.7 or higher."
    exit 1
fi

PYTHON_VERSION=$(python3 --version 2>&1 | awk '{print $2}')
echo "✓ Found Python $PYTHON_VERSION"
echo ""

# Install dependencies
echo "Installing dependencies..."
echo "(This may take a minute)"
pip install -q -r requirements.txt

if [ $? -eq 0 ]; then
    echo "✓ Dependencies installed successfully"
else
    echo "❌ Failed to install dependencies"
    echo "   Try manually: pip install numpy matplotlib scipy"
    exit 1
fi

echo ""
echo "=================================="
echo "✓ Setup Complete!"
echo "=================================="
echo ""
echo "Quick Start:"
echo "  ./wave_sim_1d.py      - 1D wave interference"
echo "  ./wave_sim_2d.py      - 2D double-slit experiment"
echo "  ./wave_quantum.py     - Quantum wave packets"
echo ""
echo "Or run: python3 wave_sim_1d.py"
echo ""
echo "Read EXPERIMENTS.md for guided tutorials!"
echo "=================================="
