cmake_minimum_required(VERSION 3.20)

set(IMGUI_SOURCES
    imgui.cpp
    imgui_demo.cpp
    imgui_draw.cpp
    imgui_tables.cpp
    imgui_widgets.cpp
)

if(LINUX)
    list(APPEND IMGUI_SOURCES imgui_impl_sdl3.cpp)
else()
    list(APPEND IMGUI_SOURCES imgui_impl.cpp)
endif()

add_library(InfernoImgui STATIC ${IMGUI_SOURCES})

target_include_directories(InfernoImgui PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(InfernoImgui PUBLIC
    fmt::fmt
    spdlog::spdlog
)

if(LINUX)
    target_include_directories(InfernoImgui SYSTEM PUBLIC
        ${PLATFORM_INCLUDE_DIR}
        ${DIRECTXTK12_SOURCE_DIR}/Inc
        ${directxmath_SOURCE_DIR}/Inc
        ${VKD3D_INCLUDE_DIRS}
    )
    target_link_directories(InfernoImgui PUBLIC ${VKD3D_LIBRARY_DIRS})
    target_compile_definitions(InfernoImgui PUBLIC
        IMGUI_IMPL_WIN32_DISABLE_GAMEPAD
    )
endif()

if(MSVC)
    target_compile_options(InfernoImgui PRIVATE /W4)
else()
    target_compile_options(InfernoImgui PRIVATE -w)
endif()
