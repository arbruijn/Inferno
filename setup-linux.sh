#!/bin/bash
# setup-linux.sh - Setup script for building Inferno on Linux
set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BUILD_DIR="${SCRIPT_DIR}/build"

echo "=== Inferno Linux Build Setup ==="

# Check for required tools
MISSING=""
command -v cmake >/dev/null 2>&1 || MISSING="${MISSING} cmake"
command -v meson >/dev/null 2>&1 || MISSING="${MISSING} meson"
command -v ninja >/dev/null 2>&1 || MISSING="${MISSING} ninja-build"
command -v pkg-config >/dev/null 2>&1 || MISSING="${MISSING} pkg-config"
command -v git >/dev/null 2>&1 || MISSING="${MISSING} git"

if [ -n "${MISSING}" ]; then
    echo "ERROR: Missing required tools:${MISSING}"
    echo "Install with: sudo apt install${MISSING}"
    exit 1
fi

# Check for required libraries
echo "Checking dependencies..."
for pkg in sdl3 libvkd3d libvkd3d-utils vulkan; do
    if ! pkg-config --exists $pkg 2>/dev/null; then
        echo "ERROR: Missing library: $pkg"
        echo "Install the -dev package for $pkg"
        exit 1
    fi
done

# Check for DXC (DirectX Shader Compiler)
if ! command -v dxc >/dev/null 2>&1; then
    echo "WARNING: DXC (DirectX Shader Compiler) not found."
    echo "Shaders will not be compiled. Install from:"
    echo "  https://github.com/microsoft/DirectXShaderCompiler/releases"
    echo ""
fi

# Create build directory
mkdir -p "${BUILD_DIR}"
cd "${BUILD_DIR}"

echo ""
echo "=== Configuring CMake ==="
cmake "${SCRIPT_DIR}" \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
    -DINFERNO_USE_VCPKG=OFF

echo ""
echo "=== Building ==="
cmake --build . -j$(nproc)

echo ""
echo "=== Build complete ==="
echo "Run: ${BUILD_DIR}/src/Inferno/Inferno"
