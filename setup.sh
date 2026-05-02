#!/bin/bash
# setup.sh - Install dependencies for Linux Mint

echo "=== OS Kernel Build System Installer ==="
echo "Detected: Linux Mint (Debian-based)"
echo ""
echo "Installing dependencies..."
echo ""

# Update package list
sudo apt-get update

# Install required packages
echo "Installing build tools..."
sudo apt-get install -y \
    build-essential \
    gcc-multilib \
    nasm \
    qemu-system-x86

echo ""

# Verify installations
echo "Verifying installations..."
echo ""

if command -v gcc &> /dev/null; then
    echo "✓ GCC installed"
else
    echo "✗ GCC not found"
fi

if command -v nasm &> /dev/null; then
    echo "✓ NASM installed"
else
    echo "✗ NASM not found"
fi

if command -v qemu-system-i386 &> /dev/null; then
    echo "✓ QEMU installed"
else
    echo "✗ QEMU not found"
fi

echo ""
echo "✓ Setup complete!"
echo ""
echo "To build and run:"
echo "  chmod +x build.sh"
echo "  ./build.sh"
echo ""

