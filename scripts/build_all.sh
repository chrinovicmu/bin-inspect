#!/bin/bash
# build_all.sh - Build and install binload for multiple architectures using CMake toolchains

set -e
set -o pipefail

# Path to toolchains
TOOLCHAIN_DIR="../cmake/toolchains"

# Output installation folder (optional override)
INSTALL_PREFIX="${PWD}/install"

# Targets and corresponding toolchain files
declare -A TARGETS
TARGETS=(
    ["x86_64"]="x86_64-linux.cmake"
    ["arm64"]="arm64-linux.cmake"
    ["riscv64"]="riscv64-linux.cmake"
)

for TARGET in "${!TARGETS[@]}"; do
    TOOLCHAIN_FILE="${TOOLCHAIN_DIR}/${TARGETS[$TARGET]}"
    BUILD_DIR="build-${TARGET}"

    echo "--------------------------------------------"
    echo "Building and installing for ${TARGET}"
    echo "Using toolchain: ${TOOLCHAIN_FILE}"
    echo "Build directory: ${BUILD_DIR}"
    echo "--------------------------------------------"

    # Check if toolchain file exists
    if [ ! -f "$TOOLCHAIN_FILE" ]; then
        echo "Error: Toolchain file not found: $TOOLCHAIN_FILE"
        exit 1
    fi

    # Create build directory
    mkdir -p "$BUILD_DIR"

    # Configure
    cmake -B "$BUILD_DIR" -DCMAKE_TOOLCHAIN_FILE="$TOOLCHAIN_FILE" -DCMAKE_INSTALL_PREFIX="$INSTALL_PREFIX" ..

    # Build
    cmake --build "$BUILD_DIR" -- -j$(nproc)

    # Install
    cmake --install "$BUILD_DIR"
done

echo "All targets built and installed successfully!"
echo "Installed files are under: $INSTALL_PREFIX"
