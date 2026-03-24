cmake_minimum_required(VERSION 3.20)
project(Inferno VERSION 0.4.2 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(INFERNO_USE_VCPKG "Use vcpkg for dependencies" OFF)

if(UNIX AND NOT APPLE)
    set(LINUX TRUE)
endif()

if(INFERNO_USE_VCPKG)
    set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake"
        CACHE STRING "Vcpkg toolchain file")
endif()

# ─────────────────────────────────────────────────────────────────────────────
# Dependencies
# ─────────────────────────────────────────────────────────────────────────────
include(FetchContent)
include(FindPkgConfig)

# fmt must be available before spdlog when using SPDLOG_FMT_EXTERNAL
find_package(fmt QUIET)
if(NOT fmt_FOUND)
    FetchContent_Declare(fmt
        GIT_REPOSITORY https://github.com/fmtlib/fmt.git
        GIT_TAG 10.1.1
        GIT_SHALLOW TRUE)
    FetchContent_MakeAvailable(fmt)
endif()

find_package(spdlog QUIET)
if(NOT spdlog_FOUND)
    FetchContent_Declare(spdlog
        GIT_REPOSITORY https://github.com/gabime/spdlog.git
        GIT_TAG v1.12.0
        GIT_SHALLOW TRUE)
    set(SPDLOG_FMT_EXTERNAL ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(spdlog)
endif()

FetchContent_Declare(ryml
    GIT_REPOSITORY https://github.com/biojppm/rapidyaml.git
    GIT_TAG v0.7.2
    GIT_SHALLOW TRUE)
FetchContent_MakeAvailable(ryml)

FetchContent_Declare(lodepng
    GIT_REPOSITORY https://github.com/lvandeve/lodepng.git
    GIT_TAG master
    GIT_SHALLOW TRUE)
FetchContent_MakeAvailable(lodepng)

FetchContent_Declare(magic_enum
    GIT_REPOSITORY https://github.com/Neargye/magic_enum.git
    GIT_TAG v0.9.2
    GIT_SHALLOW TRUE)
FetchContent_MakeAvailable(magic_enum)

pkg_check_modules(SDL3 REQUIRED sdl3)
# Create imported target for SDL3 since pkg_check_modules doesn't do it
if(NOT TARGET SDL3::SDL3)
    add_library(SDL3::SDL3 INTERFACE IMPORTED)
    set_target_properties(SDL3::SDL3 PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES "${SDL3_INCLUDE_DIRS}"
        INTERFACE_LINK_LIBRARIES "${SDL3_LIBRARIES}"
        INTERFACE_LINK_DIRECTORIES "${SDL3_LIBRARY_DIRS}"
    )
endif()

FetchContent_Declare(directxmath
    GIT_REPOSITORY https://github.com/microsoft/DirectXMath.git
    GIT_TAG oct2024
    GIT_SHALLOW TRUE)
FetchContent_MakeAvailable(directxmath)

if(LINUX)
    # ─────────────────────────────────────────────────────────────────────────
    # System-installed vkd3d for DX12 API
    # ─────────────────────────────────────────────────────────────────────────
    pkg_check_modules(VKD3D REQUIRED libvkd3d)
    pkg_check_modules(VKD3D_UTILS libvkd3d-utils)
    find_package(Vulkan REQUIRED)

    find_program(DXC_EXECUTABLE dxc)
    if(NOT DXC_EXECUTABLE)
        message(WARNING "DXC not found. Shaders will not be compiled.")
    endif()

    set(PLATFORM_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/platform/linux)

    # DirectXTK12 source for SimpleMath, Keyboard, etc.
    FetchContent_Declare(directxtk12
        GIT_REPOSITORY https://github.com/microsoft/DirectXTK12.git
        GIT_TAG sept2023
        GIT_SHALLOW TRUE)
    FetchContent_GetProperties(directxtk12)
    if(NOT directxtk12_POPULATED)
        FetchContent_Populate(directxtk12)
    endif()
    set(DIRECTXTK12_SOURCE_DIR ${directxtk12_SOURCE_DIR})

    # Create DirectXTK12 subdirectory symlink for <DirectXTK12/Header.h> includes
    if(NOT EXISTS ${directxtk12_SOURCE_DIR}/Inc/DirectXTK12)
        file(CREATE_LINK . ${directxtk12_SOURCE_DIR}/Inc/DirectXTK12 SYMBOLIC)
    endif()

    # D3D12 Memory Allocator (header-only on Linux)
    FetchContent_Declare(d3d12memalloc
        GIT_REPOSITORY https://github.com/GPUOpen-LibrariesAndSDKs/D3D12MemoryAllocator.git
        GIT_TAG v3.0.1
        GIT_SHALLOW TRUE)
    FetchContent_GetProperties(d3d12memalloc)
    if(NOT d3d12memalloc_POPULATED)
        FetchContent_Populate(d3d12memalloc)
    endif()
endif()

# ─────────────────────────────────────────────────────────────────────────────
# Compiler flags
# ─────────────────────────────────────────────────────────────────────────────
add_compile_definitions(
    $<$<CONFIG:Debug>:_DEBUG>
    $<$<CONFIG:Release>:NDEBUG>
    NOMINMAX
    _CRT_SECURE_NO_WARNINGS
)

if(LINUX)
    add_compile_definitions(PLATFORM_LINUX)
    # Force-include Windows compatibility header on Linux
    add_compile_options(-include ${CMAKE_SOURCE_DIR}/platform/linux/WinCompat.h)
endif()

if(MSVC)
    add_compile_options(/W4 /permissive- /utf-8)
else()
    add_compile_options(-Wall -Wextra -Wno-unused-parameter -Wno-missing-field-initializers -Wno-changes-meaning)
endif()

# ─────────────────────────────────────────────────────────────────────────────
# Subdirectories
# ─────────────────────────────────────────────────────────────────────────────
add_subdirectory(src/Imgui)
add_subdirectory(src/Inferno.Core)
add_subdirectory(src/Inferno)
