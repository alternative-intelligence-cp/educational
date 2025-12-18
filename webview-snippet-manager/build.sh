#!/bin/bash

echo "🔨 Building Webview Snippet Manager..."

# Create build directory
mkdir -p build
cd build

# Configure with CMake
echo "📋 Configuring with CMake..."
cmake .. \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_C_COMPILER=gcc-14 \
    -DCMAKE_CXX_COMPILER=g++-14

# Build
echo "🏗️  Compiling..."
make -j$(nproc)

if [ $? -eq 0 ]; then
    echo "✅ Build successful!"
    echo ""
    echo "To run:"
    echo "  cd build"
    echo "  ./snippet-manager"
else
    echo "❌ Build failed"
    exit 1
fi
