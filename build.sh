#!/bin/bash
# build.sh - Automatic build and run script for Linux Mint

set -e  # Exit on error

echo "=== OS Kernel Build & Run ==="
echo ""

# Check if dependencies are installed
if ! command -v gcc &> /dev/null; then
    echo "✗ GCC not found. Running setup..."
    chmod +x setup.sh
    ./setup.sh
fi

if ! command -v nasm &> /dev/null; then
    echo "✗ NASM not found. Installing..."
    sudo apt-get install -y nasm
fi

if ! command -v qemu-system-i386 &> /dev/null; then
    echo "✗ QEMU not found. Installing..."
    sudo apt-get install -y qemu-system-x86
fi

echo "✓ Dependencies ready"
echo ""

# Build
echo "Building..."
make clean > /dev/null 2>&1 || true
make all

echo "✓ Build complete"
echo ""

# Run in QEMU
echo "Launching QEMU..."
make run
