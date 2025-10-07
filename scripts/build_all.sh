#!/usr/bin/env bash
set -e

# Detect the project root relative to this script
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(dirname "$SCRIPT_DIR")"
TOOLCHAIN_DIR="$PROJECT_ROOT/cmake/toolchains"

# Detect host architecture
HOST_ARCH=$(uname -m)

case "$HOST_ARCH" in
x86_64)
    TARGET="x86_64"
    TOOLCHAIN_FILE="$TOOLCHAIN_DIR/x86_64-linux.cmake"
    ;;
aarch64 | arm64)
    TARGET="arm64"
    TOOLCHAIN_FILE="$TOOLCHAIN_DIR/arm64-linux.cmake"
    ;;
riscv64)
    TARGET="riscv64"
    TOOLCHAIN_FILE="$TOOLCHAIN_DIR/riscv64-linux.cmake"
    ;;
*)
    echo "Unsupported architecture: $HOST_ARCH"
    exit 1
    ;;
esac

BUILD_DIR="$PROJECT_ROOT/build-$TARGET"

echo "--------------------------------------------"
echo "Building for detected architecture: $HOST_ARCH"
echo "Using toolchain: $TOOLCHAIN_FILE"
echo "Build directory: $BUILD_DIR"
echo "--------------------------------------------"

if [ ! -f "$TOOLCHAIN_FILE" ]; then
    echo "Error: Toolchain file not found: $TOOLCHAIN_FILE"
    exit 1
fi

cmake -B "$BUILD_DIR" -DCMAKE_TOOLCHAIN_FILE="$TOOLCHAIN_FILE" "$PROJECT_ROOT"
cmake --build "$BUILD_DIR" --parallel

echo
echo "✅ Build completed successfully for $TARGET"
