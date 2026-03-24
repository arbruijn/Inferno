cmake_minimum_required(VERSION 3.20)

file(GLOB_RECURSE CORE_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
file(GLOB_RECURSE CORE_HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/*.h")

add_library(InfernoCore STATIC ${CORE_SOURCES} ${CORE_HEADERS})

target_include_directories(InfernoCore PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(InfernoCore PUBLIC
    fmt::fmt
    spdlog::spdlog
    ryml::ryml
    magic_enum::magic_enum
)

# lodepng
target_include_directories(InfernoCore PRIVATE ${lodepng_SOURCE_DIR})
target_sources(InfernoCore PRIVATE ${lodepng_SOURCE_DIR}/lodepng.cpp)

if(LINUX)
    target_include_directories(InfernoCore SYSTEM PUBLIC
        ${PLATFORM_INCLUDE_DIR}
        ${DIRECTXTK12_SOURCE_DIR}/Inc
        ${directxmath_SOURCE_DIR}/Inc
        ${VKD3D_INCLUDE_DIRS}
    )
    target_link_directories(InfernoCore PUBLIC ${VKD3D_LIBRARY_DIRS})
    target_compile_definitions(InfernoCore PUBLIC PLATFORM_LINUX)
    target_compile_options(InfernoCore PRIVATE -Wno-unused-variable -Wno-unused-function)
else()
    find_package(directxtk12 CONFIG REQUIRED)
    find_package(directxtex CONFIG REQUIRED)
    target_link_libraries(InfernoCore PUBLIC
        Microsoft::DirectXTK12
        Microsoft::DirectXTex
    )
endif()

if(MSVC)
    target_compile_options(InfernoCore PRIVATE /W4)
    target_precompile_headers(InfernoCore PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/../Inferno/pch.h)
endif()
