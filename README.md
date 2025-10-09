# Binspect

**Binspect** is a lightweight, cross-platform binary inspection tool and library written in modern C++. It allows you to parse and analyze executable files, inspect their sections and symbols, and retrieve architecture and entry point information.

> **Note:** Currently supports Linux only. Windows and macOS support is under development.

---

## Features

- Supports multiple binary formats:  
  - **ELF** (Linux, Unix-like)  
  - **COFF** (Common Object File Format)  
  - **PE** (Windows Portable Executable)  
  - **Mach-O** (macOS)
- Supports multiple architectures:  
  - **x86 / x86_64**  
  - **ARM / AArch64**  
  - **RISC-V**
- Inspect sections (`.text`, `.data`, etc.) and symbols (functions, objects, labels).  
- Retrieve entry point, architecture, and human-readable binary type.  
- Portable C++17 code, currently supporting Linux (Windows and macOS support under development).  
- Uses GNU BFD library for parsing binaries.

---

## Installation

### Using CMake

1. Clone the repository:

```bash
git clone https://github.com/chrinovicmu/binspect.git
cd binspect
```

2. Build for the detected architecture:

```bash
./scripts/build_all.sh
```

This script will automatically select the correct toolchain based on your system.

You can also manually build for specific architectures using the CMake toolchain files:

```bash
# Example for x86_64
cmake -B build-x86_64 -DCMAKE_TOOLCHAIN_FILE=cmake/toolchains/x86_64-linux.cmake .
cmake --build build-x86_64
```

---

## Usage

```bash
./binspect <path_to_binary>
```

Example:

```bash
./binspect /bin/ls
```

Sample output:

```
Loaded binary: /bin/ls
Filename: /bin/ls
Type: ELF
Architecture: x86_64 (64-bit)
Entry: 0x401000
Sections: 12
Symbols: 123
```

---

## Example in C++

You can also use Binspect as a library in your C++ projects:

```cpp
Binary bin;
if (load_binary("test_binary", &bin, Binary::BIN_TYPE_AUTO) == 0) {
    bin.print_info();
    bin.print_sections();
    bin.print_symbols();
}
```

---

## Project Structure

```
.
├── cmake/
│   └── toolchains/        # Toolchain files for cross-compilation
├── include/               # Header files
├── src/                   # Source files
├── scripts/               # Build scripts
├── tests/                 # Example/test loaders
├── CMakeLists.txt
└── README.md
```

---

## Dependencies

- GNU BFD library (binutils-dev on Linux)
- C++17 compatible compiler
- CMake 3.15+

---

## License

MIT License – see LICENSE
