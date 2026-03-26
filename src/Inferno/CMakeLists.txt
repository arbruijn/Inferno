cmake_minimum_required(VERSION 3.20)

# ─────────────────────────────────────────────────────────────────────────────
# Source file collection
# ─────────────────────────────────────────────────────────────────────────────
file(GLOB_RECURSE INFERNO_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
)

file(GLOB_RECURSE INFERNO_HEADERS
    "${CMAKE_CURRENT_SOURCE_DIR}/*.h"
)

# Exclude platform-specific files on Linux
if(LINUX)
    list(FILTER INFERNO_SOURCES EXCLUDE REGEX "WindowsDialogs\\.cpp$")
    list(FILTER INFERNO_SOURCES EXCLUDE REGEX "Shell\\.cpp$")
    list(FILTER INFERNO_SOURCES EXCLUDE REGEX "SoundSystem\\.cpp$")
    list(FILTER INFERNO_SOURCES EXCLUDE REGEX "Audio/")
endif()

# ─────────────────────────────────────────────────────────────────────────────
# Shader compilation for Linux (DXC → SPIR-V)
# ─────────────────────────────────────────────────────────────────────────────
set(SHADER_DIR ${CMAKE_CURRENT_SOURCE_DIR}/shaders)
set(SHADER_OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR}/shaders/bin)
file(MAKE_DIRECTORY ${SHADER_OUTPUT_DIR})

set(SHADER_SOURCES
    ${SHADER_DIR}/level.hlsl
    ${SHADER_DIR}/levelflat.hlsl
    ${SHADER_DIR}/editor.hlsl
    ${SHADER_DIR}/imgui.hlsl
    ${SHADER_DIR}/sprite.hlsl
    ${SHADER_DIR}/object.hlsl
    ${SHADER_DIR}/depth.hlsl
    ${SHADER_DIR}/DepthObject.hlsl
    ${SHADER_DIR}/DepthCutout.hlsl
    ${SHADER_DIR}/HUD.hlsl
    ${SHADER_DIR}/BloomExtractDownsampleCS.hlsl
    ${SHADER_DIR}/DownsampleBloomCS.hlsl
    ${SHADER_DIR}/UpsampleAndBlurCS.hlsl
    ${SHADER_DIR}/ToneMapCS.hlsl
    ${SHADER_DIR}/BlurCS.hlsl
    ${SHADER_DIR}/ScanlineCS.hlsl
    ${SHADER_DIR}/FillLightGridCS.hlsl
)

set(COMPILED_SHADERS)

if(DXC_EXECUTABLE AND (LINUX OR MINGW))
    foreach(SHADER ${SHADER_SOURCES})
        get_filename_component(SHADER_NAME ${SHADER} NAME_WE)

        if(EXISTS ${SHADER})
            if(SHADER_NAME MATCHES "CS$")
                # Compute shader
                set(OUTPUT_FILE ${SHADER_OUTPUT_DIR}/${SHADER_NAME}.bin)
                add_custom_command(
                    OUTPUT ${OUTPUT_FILE}
                    COMMAND ${DXC_EXECUTABLE}
                        -spirv -T cs_6_0 -E main
                        -I ${SHADER_DIR}
                        -Fo ${OUTPUT_FILE} ${SHADER}
                    DEPENDS ${SHADER} ${SHADER_DIR}/*.hlsli
                    COMMENT "Compiling compute shader ${SHADER_NAME}"
                )
                list(APPEND COMPILED_SHADERS ${OUTPUT_FILE})
            else()
                # Vertex shader
                set(VS_OUTPUT ${SHADER_OUTPUT_DIR}/${SHADER_NAME}.vs.bin)
                add_custom_command(
                    OUTPUT ${VS_OUTPUT}
                    COMMAND ${DXC_EXECUTABLE}
                        -spirv -T vs_6_0 -E vsmain
                        -I ${SHADER_DIR}
                        -Fo ${VS_OUTPUT} ${SHADER}
                    DEPENDS ${SHADER} ${SHADER_DIR}/*.hlsli
                    COMMENT "Compiling vertex shader ${SHADER_NAME}"
                )
                list(APPEND COMPILED_SHADERS ${VS_OUTPUT})

                # Pixel shader
                set(PS_OUTPUT ${SHADER_OUTPUT_DIR}/${SHADER_NAME}.ps.bin)
                add_custom_command(
                    OUTPUT ${PS_OUTPUT}
                    COMMAND ${DXC_EXECUTABLE}
                        -spirv -T ps_6_0 -E psmain
                        -I ${SHADER_DIR}
                        -Fo ${PS_OUTPUT} ${SHADER}
                    DEPENDS ${SHADER} ${SHADER_DIR}/*.hlsli
                    COMMENT "Compiling pixel shader ${SHADER_NAME}"
                )
                list(APPEND COMPILED_SHADERS ${PS_OUTPUT})
            endif()
        endif()
    endforeach()
endif()

if(COMPILED_SHADERS)
    add_custom_target(CompileShaders ALL DEPENDS ${COMPILED_SHADERS})
endif()

add_library(zip STATIC zip/zip.c)

add_library(pcm STATIC Audio/pcm/stb_vorbis.c)

# ─────────────────────────────────────────────────────────────────────────────
# Executable
# ─────────────────────────────────────────────────────────────────────────────
add_executable(Inferno
    ${INFERNO_SOURCES}
    ${INFERNO_HEADERS}
)

if(COMPILED_SHADERS)
    add_dependencies(Inferno CompileShaders)
endif()

target_include_directories(Inferno PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../vendor
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/..
)

target_precompile_headers(Inferno PRIVATE pch.h)

find_package(OpenMP REQUIRED)
target_link_libraries(Inferno PRIVATE OpenMP::OpenMP_CXX)
target_link_libraries(Inferno PRIVATE zip pcm winmm x3daudio1_7 xaudio2_9
	${CMAKE_SOURCE_DIR}/platform/mingw/lib/dxcompiler.dll)
target_link_options(Inferno PRIVATE -fopenmp)

# ─────────────────────────────────────────────────────────────────────────────
# Link libraries
# ─────────────────────────────────────────────────────────────────────────────
target_link_libraries(Inferno PRIVATE
    InfernoCore
    InfernoImgui
    fmt::fmt
    spdlog::spdlog
    ryml::ryml
    magic_enum::magic_enum
    SLikeNetLibStatic
    iphlpapi
)

# SLikeNet platform dependencies
if(WIN32)
    target_link_libraries(Inferno PRIVATE ws2_32)
elseif(UNIX)
    # Linux socket library (may be needed)
    find_library(SOCKET_LIB socket)
    if(SOCKET_LIB)
        target_link_libraries(Inferno PRIVATE ${SOCKET_LIB})
    endif()
    target_link_libraries(Inferno PRIVATE pthread)
endif()

# SLinkNet requires OpenSSL
target_link_libraries(Inferno PRIVATE OpenSSL::Crypto OpenSSL::SSL)

# SLikeNet include directory
target_include_directories(Inferno PRIVATE ${SLikeNet_SOURCE_DIR}/Source ${SLikeNet_SOURCE_DIR}/Source/include)

# lodepng
target_include_directories(Inferno PRIVATE ${lodepng_SOURCE_DIR})

# alpaca header-only
target_include_directories(Inferno PRIVATE ${alpaca_SOURCE_DIR})

if(LINUX)
    target_include_directories(Inferno SYSTEM PRIVATE
        ${PLATFORM_INCLUDE_DIR}
        ${DIRECTXTK12_SOURCE_DIR}/Inc
        ${directxmath_SOURCE_DIR}/Inc
        ${d3d12memalloc_SOURCE_DIR}/include
        ${VKD3D_INCLUDE_DIRS}
    )

    target_compile_definitions(Inferno PRIVATE
        PLATFORM_LINUX
    )

    target_link_libraries(Inferno PRIVATE
        SDL3::SDL3
        Vulkan::Vulkan
        ${VKD3D_LIBRARIES}
        ${VKD3D_UTILS_LIBRARIES}
        dl
        pthread
    )

    target_link_directories(Inferno PRIVATE
        ${VKD3D_LIBRARY_DIRS}
        ${VKD3D_UTILS_LIBRARY_DIRS}
    )

    # Add Linux platform source files
    target_sources(Inferno PRIVATE
        ${CMAKE_SOURCE_DIR}/platform/linux/WinTypes.cpp
    )

    target_compile_options(Inferno PRIVATE
        -Wno-unused-variable
        -Wno-unused-function
        -Wno-missing-field-initializers
    )

elseif(MINGW)
    target_include_directories(Inferno SYSTEM PRIVATE
        ${PLATFORM_INCLUDE_DIR}
        ${DIRECTXTK12_SOURCE_DIR}/Inc
        ${directxmath_SOURCE_DIR}/Inc
        ${d3d12memalloc_SOURCE_DIR}/include
    )

    target_link_libraries(Inferno PRIVATE
        SDL3::SDL3
        DirectXTex
        DirectXTK12Lib
        D3D12MALib
        d3d12
        dxgi
        dwmapi
        shlwapi
    )

    target_compile_options(Inferno PRIVATE
        -Wno-unused-variable
        -Wno-unused-function
        -Wno-missing-field-initializers
    )

else()
    find_package(d3d12-memory-allocator CONFIG REQUIRED)
    find_package(directx-headers CONFIG REQUIRED)

    target_link_libraries(Inferno PRIVATE
        Microsoft::DirectXTK12
        Microsoft::DirectXTex
        Microsoft::DirectX-Guids
        d3d12-memory-allocator::D3D12MemoryAllocator
        d3d12.lib
        dxgi.lib
        d3dcompiler.lib
        dxguid.lib
        dwmapi.lib
        shlwapi.lib
        comctl32.lib
    )
endif()

# ─────────────────────────────────────────────────────────────────────────────
# Install
# ─────────────────────────────────────────────────────────────────────────────
install(TARGETS Inferno RUNTIME DESTINATION bin)

if((LINUX OR MINGW) AND DXC_EXECUTABLE AND COMPILED_SHADERS)
    install(DIRECTORY ${SHADER_OUTPUT_DIR}/
        DESTINATION bin/shaders/bin
        FILES_MATCHING PATTERN "*.bin")
endif()
