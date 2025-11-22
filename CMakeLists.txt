cmake_minimum_required(VERSION 3.12...3.26)

get_property(is_multi_config GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)
if(NOT is_multi_config AND NOT (CMAKE_BUILD_TYPE OR DEFINED ENV{CMAKE_BUILD_TYPE}))
  set(CMAKE_BUILD_TYPE Release CACHE STRING "Release default")
endif()

project(tinyrenderer LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(iwyu "Run include-what-you-use")
if(iwyu)
  find_program(IWYU_EXE NAMES include-what-you-use REQUIRED)
  set(CMAKE_CXX_INCLUDE_WHAT_YOU_USE ${IWYU_EXE})
endif()

if(CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU|Intel")
  add_compile_options(-Wall)
elseif(MSVC)
  add_compile_options(/W4)
endif()

find_package(OpenMP COMPONENTS CXX)

# --- SimpleMath / DirectXMath ---
# Ожидаем репозитории:
#   external/DirectXMath/Inc/DirectXMath.h
#   external/DirectXTK/Inc/SimpleMath.h
set(DIRECTXMATH_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/external/DirectXMath/Inc")
set(DIRECTXTK_INCLUDE_DIR   "${CMAKE_SOURCE_DIR}/external/DirectXTK/Inc")

if(NOT EXISTS "${DIRECTXMATH_INCLUDE_DIR}/DirectXMath.h")
  message(FATAL_ERROR "Не найден ${DIRECTXMATH_INCLUDE_DIR}/DirectXMath.h. Склонируй https://github.com/microsoft/DirectXMath в external/DirectXMath")
endif()

if(NOT EXISTS "${DIRECTXTK_INCLUDE_DIR}/SimpleMath.h")
  message(FATAL_ERROR "Не найден ${DIRECTXTK_INCLUDE_DIR}/SimpleMath.h. Склонируй https://github.com/microsoft/DirectXTK в external/DirectXTK")
endif()

# На Windows иногда конфликтуют макросы min/max из Windows.h
# Если ты используешь Windows.h — NOMINMAX поможет.
add_compile_definitions(NOMINMAX)

set(SOURCES
        main.cpp
        tgaimage.cpp
        Model.cpp
        Renderer.cpp
        Camera.cpp
        MathTypes.h
        ShaderTypes.h
        IShader.h
        FlatRandomColorShader.cpp
        FlatRandomColorShader.h
)

add_executable(${PROJECT_NAME} ${SOURCES})

# Подключаем заголовки DirectXMath и SimpleMath
target_include_directories(${PROJECT_NAME} PRIVATE
        "${DIRECTXMATH_INCLUDE_DIR}"
        "${DIRECTXTK_INCLUDE_DIR}"
)

# OpenMP (если найден)
target_link_libraries(${PROJECT_NAME} PRIVATE
        $<$<BOOL:${OpenMP_CXX_FOUND}>:OpenMP::OpenMP_CXX>
)

# Чтобы не засорять папку билд-артефактами при генерации
file(GENERATE OUTPUT .gitignore CONTENT "*")
