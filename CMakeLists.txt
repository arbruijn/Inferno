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
# Dependencies (git submodules in deps/)
# ─────────────────────────────────────────────────────────────────────────────

set(DEPS_DIR ${CMAKE_SOURCE_DIR}/deps)

# fmt must be available before spdlog when using SPDLOG_FMT_EXTERNAL
add_subdirectory(${DEPS_DIR}/fmt)

set(SPDLOG_FMT_EXTERNAL ON CACHE BOOL "" FORCE)
add_subdirectory(${DEPS_DIR}/spdlog)

add_subdirectory(${DEPS_DIR}/ryml)
add_subdirectory(${DEPS_DIR}/magic_enum)

# SLikeNet requires OpenSSL
find_package(OpenSSL REQUIRED)

# Disable SLikeNet samples
set(SLIKENET_ENABLE_SAMPLES OFF CACHE BOOL "" FORCE)
set(SLIKENET_ENABLE_DLL OFF CACHE BOOL "" FORCE)

# OpenSSL 3.x compatibility - set API level for older code
add_compile_definitions(OPENSSL_API_COMPAT=0x10100000L)

add_subdirectory(${DEPS_DIR}/SLikeNet)

# lodepng has no CMakeLists.txt - compile manually from source
set(lodepng_SOURCE_DIR ${DEPS_DIR}/lodepng CACHE PATH "lodepng source directory")

# DirectXMath is header-only
set(directxmath_SOURCE_DIR ${DEPS_DIR}/DirectXMath CACHE PATH "DirectXMath source directory")

# DirectXTK12 source for headers (SimpleMath, Keyboard, etc.)
set(DIRECTXTK12_SOURCE_DIR ${DEPS_DIR}/DirectXTK12 CACHE PATH "DirectXTK12 source directory")

# D3D12 Memory Allocator is header-only (used on Linux and MinGW)
set(d3d12memalloc_SOURCE_DIR ${DEPS_DIR}/D3D12MemoryAllocator CACHE PATH "D3D12MemoryAllocator source directory")

# alpaca is header-only serialization library
set(alpaca_SOURCE_DIR ${DEPS_DIR}/alpaca/include CACHE PATH "alpaca source directory")

# ─────────────────────────────────────────────────────────────────────────────
# Platform detection
# ─────────────────────────────────────────────────────────────────────────────

if(LINUX)
    include(FindPkgConfig)

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

    # System-installed vkd3d for DX12 API
    pkg_check_modules(VKD3D REQUIRED libvkd3d)
    pkg_check_modules(VKD3D_UTILS libvkd3d-utils)
    find_package(Vulkan REQUIRED)

    find_program(DXC_EXECUTABLE dxc)
    if(NOT DXC_EXECUTABLE)
        message(WARNING "DXC not found. Shaders will not be compiled.")
    endif()

    set(PLATFORM_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/platform/linux)

    # Create DirectXTK12 subdirectory symlink for <DirectXTK12/Header.h> includes
    if(NOT EXISTS ${DIRECTXTK12_SOURCE_DIR}/Inc/DirectXTK12)
        file(CREATE_LINK . ${DIRECTXTK12_SOURCE_DIR}/Inc/DirectXTK12 SYMBOLIC)
    endif()

elseif(MINGW)
    add_compile_definitions(_WIN32_WINNT=0x0A00 UNICODE _UNICODE)
    add_link_options(-municode)

    find_package(SDL3 CONFIG REQUIRED)

    # Create DirectXTK12 subdirectory symlink for <DirectXTK12/Header.h> includes
    if(NOT EXISTS ${DIRECTXTK12_SOURCE_DIR}/Inc/DirectXTK12)
        file(CREATE_LINK . ${DIRECTXTK12_SOURCE_DIR}/Inc/DirectXTK12 SYMBOLIC)
    endif()

    # MinGW platform stubs (pix3.h, DirectXTK12 audio/input stubs)
    set(PLATFORM_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/platform/mingw)

    include_directories(${PLATFORM_INCLUDE_DIR})

if(1)
    # Build DirectXTex from submodule
    set(BUILD_TOOLS OFF CACHE BOOL "" FORCE)
    set(BUILD_SAMPLE OFF CACHE BOOL "" FORCE)
    set(BUILD_DX11 OFF CACHE BOOL "" FORCE)
    set(BUILD_DX12 ON CACHE BOOL "" FORCE)
    set(ENABLE_OPENEXR_SUPPORT OFF CACHE BOOL "" FORCE)
    set(ENABLE_LIBJPEG_SUPPORT OFF CACHE BOOL "" FORCE)
    set(ENABLE_LIBPNG_SUPPORT OFF CACHE BOOL "" FORCE)
    set(BUILD_TESTING OFF CACHE BOOL "" FORCE)
    add_subdirectory(${DEPS_DIR}/DirectXTex)
endif()

    # ─────────────────────────────────────────────────────────────────────────
    # Download Windows DXC from GitHub releases for shader compilation
    # ─────────────────────────────────────────────────────────────────────────
    set(DXC_VERSION "1.8.2505.1")
    set(DXC_ZIP_NAME "dxc_2025_07_14.zip")
    set(DXC_DOWNLOAD_URL "https://github.com/microsoft/DirectXShaderCompiler/releases/download/v${DXC_VERSION}/${DXC_ZIP_NAME}")
    set(DXC_CACHE_DIR "${CMAKE_SOURCE_DIR}/deps/dxc")
    set(DXC_EXE_PATH "${DXC_CACHE_DIR}/bin/x64/dxc.exe")

    if(NOT EXISTS "${DXC_EXE_PATH}")
        message(STATUS "Downloading DXC ${DXC_VERSION}...")
        file(DOWNLOAD "${DXC_DOWNLOAD_URL}" "${CMAKE_BINARY_DIR}/${DXC_ZIP_NAME}"
            SHOW_PROGRESS
            STATUS DOWNLOAD_STATUS
        )
        list(GET DOWNLOAD_STATUS 0 DOWNLOAD_ERROR)
        list(GET DOWNLOAD_STATUS 1 DOWNLOAD_MSG)
        if(NOT DOWNLOAD_ERROR EQUAL 0)
            message(FATAL_ERROR "Failed to download DXC: ${DOWNLOAD_MSG}")
        endif()
        file(ARCHIVE_EXTRACT INPUT "${CMAKE_BINARY_DIR}/${DXC_ZIP_NAME}" DESTINATION "${DXC_CACHE_DIR}")
        file(REMOVE "${CMAKE_BINARY_DIR}/${DXC_ZIP_NAME}")
        message(STATUS "DXC extracted to ${DXC_CACHE_DIR}")
    endif()

    # DXC is run through wine (via winetst wrapper); set the command prefix
    find_program(WINE_EXECUTABLE wine)
    if(NOT WINE_EXECUTABLE)
        find_program(WINE_EXECUTABLE winetst)
    endif()
    if(WINE_EXECUTABLE)
        set(DXC_WINETST "${WINE_EXECUTABLE}" "${DXC_EXE_PATH}")
    else()
        set(DXC_WINETST "${DXC_EXE_PATH}")
    endif()
    message(STATUS "Using DXC: ${DXC_EXE_PATH}")

    # ─────────────────────────────────────────────────────────────────────────
    # Compile DirectXTK12 shaders
    # ─────────────────────────────────────────────────────────────────────────
    set(DIRECTXTK12_SRC ${DIRECTXTK12_SOURCE_DIR}/Src)
    include(${CMAKE_SOURCE_DIR}/cmake/DirectXTK12Shaders.cmake)

    add_custom_target(DirectXTK12Shaders DEPENDS ${DIRECTXTK12_SHADER_INCS})

    # Build needed DirectXTK12 source files for MinGW
    add_library(DirectXTK12Lib STATIC
        ${DIRECTXTK12_SRC}/BinaryReader.cpp
        ${DIRECTXTK12_SRC}/BufferHelpers.cpp
        ${DIRECTXTK12_SRC}/CommonStates.cpp
        ${DIRECTXTK12_SRC}/DDSTextureLoader.cpp
        ${DIRECTXTK12_SRC}/DescriptorHeap.cpp
        ${DIRECTXTK12_SRC}/DirectXHelpers.cpp
        ${DIRECTXTK12_SRC}/EffectCommon.cpp
        ${DIRECTXTK12_SRC}/EffectFactory.cpp
        ${DIRECTXTK12_SRC}/EffectPipelineStateDescription.cpp
        ${DIRECTXTK12_SRC}/EffectTextureFactory.cpp
        ${DIRECTXTK12_SRC}/GeometricPrimitive.cpp
        ${DIRECTXTK12_SRC}/Geometry.cpp
        ${DIRECTXTK12_SRC}/GraphicsMemory.cpp
        ${DIRECTXTK12_SRC}/LinearAllocator.cpp
        ${DIRECTXTK12_SRC}/Model.cpp
        ${DIRECTXTK12_SRC}/ModelLoadSDKMESH.cpp
        ${DIRECTXTK12_SRC}/PrimitiveBatch.cpp
        ${DIRECTXTK12_SRC}/ResourceUploadBatch.cpp
        ${DIRECTXTK12_SRC}/SimpleMath.cpp
        ${DIRECTXTK12_SRC}/SpriteBatch.cpp
        ${DIRECTXTK12_SRC}/SpriteFont.cpp
        ${DIRECTXTK12_SRC}/VertexTypes.cpp
    )
    add_dependencies(DirectXTK12Lib DirectXTK12Shaders)
    target_include_directories(DirectXTK12Lib SYSTEM PUBLIC
        ${DIRECTXTK12_SOURCE_DIR}/Inc
        ${directxmath_SOURCE_DIR}/Inc
        ${PLATFORM_INCLUDE_DIR}
    )
    target_include_directories(DirectXTK12Lib SYSTEM PRIVATE
        ${DIRECTXTK12_SHADER_OUTPUT_DIR}
    )
    target_compile_definitions(DirectXTK12Lib PRIVATE _WIN32)
    target_compile_options(DirectXTK12Lib PRIVATE -w)

    # D3D12MemoryAllocator source
    add_library(D3D12MALib STATIC ${d3d12memalloc_SOURCE_DIR}/src/D3D12MemAlloc.cpp)
    target_include_directories(D3D12MALib SYSTEM PUBLIC
        ${d3d12memalloc_SOURCE_DIR}/include
        ${directxmath_SOURCE_DIR}/Inc
        ${PLATFORM_INCLUDE_DIR}
    )
    target_compile_definitions(D3D12MALib PRIVATE _WIN32)
    target_compile_options(D3D12MALib PRIVATE -w)

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
    add_compile_options(-Wall -Wextra -Wno-unused-parameter -Wno-missing-field-initializers
	-Wno-switch -Wno-sign-compare -Wno-narrowing -Wno-ignored-qualifiers
	-Wno-implicit-const-int-float-conversion -Wno-unused-private-field
	-Wno-unknown-pragmas -Wno-comment -Wno-nontrivial-memcall
        -Wno-missing-braces)
    if(NOT MINGW)
        add_compile_options(-Wno-changes-meaning)
    endif()
endif()

# ─────────────────────────────────────────────────────────────────────────────
# Subdirectories
# ─────────────────────────────────────────────────────────────────────────────
add_subdirectory(src/Imgui)
add_subdirectory(src/Inferno.Core)
add_subdirectory(src/Inferno)
