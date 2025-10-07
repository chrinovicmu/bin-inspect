
cmake_minimum_required(VERSION 3.24)

project(binload
    VERSION 1.0
    DESCRIPTION "Cross-platform C++ binary loader and symbol extractor (ELF, PE, COFF, Mach-O)"
    LANGUAGES C CXX
)

# ---------------------------
# Options
# ---------------------------
option(BUILD_TESTS "Build demo/test binaries" ON)
option(INSTALL_BINARIES "Install demo binaries to bin/" ON)

# ---------------------------
# C++ Standard
# ---------------------------
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ---------------------------
# Source and include directories
# ---------------------------
set(SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")
set(INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include")
set(TEST_DIR "${CMAKE_CURRENT_SOURCE_DIR}/tests")

file(GLOB SRC_FILES "${SRC_DIR}/*.cpp")
file(GLOB TEST_FILES "${TEST_DIR}/*.cpp")

include_directories(${INCLUDE_DIR})

# ---------------------------
# Library target
# ---------------------------
add_library(binload STATIC ${SRC_FILES})

# Link libbfd (required)
find_library(BFD_LIB bfd)
if(NOT BFD_LIB)
    message(FATAL_ERROR "libbfd not found. Install binutils development libraries.")
endif()

target_link_libraries(binload PRIVATE ${BFD_LIB})

# ---------------------------
# Demo / test executables
# ---------------------------
if(BUILD_TESTS)
    foreach(TEST_SRC ${TEST_FILES})
        get_filename_component(TEST_NAME ${TEST_SRC} NAME_WE)
        add_executable(${TEST_NAME} ${TEST_SRC})
        target_link_libraries(${TEST_NAME} PRIVATE binload)
        target_include_directories(${TEST_NAME} PRIVATE ${INCLUDE_DIR})

        # Install binaries if desired
        if(INSTALL_BINARIES)
            install(TARGETS ${TEST_NAME}
                RUNTIME DESTINATION bin
            )
        endif()
    endforeach()
endif()

# ---------------------------
# Install library and headers
# ---------------------------
install(TARGETS binload
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)

install(DIRECTORY ${INCLUDE_DIR}/
    DESTINATION include
    FILES_MATCHING PATTERN "*.h"
)

# ---------------------------
# Optional docs folder
# ---------------------------
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/docs")
    message(STATUS "Docs folder detected, consider adding Doxygen or Sphinx integration")
endif()

# ---------------------------
# Build output directories
# ---------------------------
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# ---------------------------
# Summary
# ---------------------------
message(STATUS "Project: ${PROJECT_NAME} v${PROJECT_VERSION}")
message(STATUS "C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "Include dir: ${INCLUDE_DIR}")
message(STATUS "Source dir: ${SRC_DIR}")
