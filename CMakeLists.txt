cmake_minimum_required(VERSION 3.14)
project(fileformat VERSION 1.0.0 LANGUAGES CXX)

# C++ 标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 构建选项
option(FILEFORMAT_BUILD_TESTS "Build tests" ON)
option(FILEFORMAT_BUILD_EXAMPLES "Build examples" ON)
option(FILEFORMAT_BUILD_SHARED "Build shared library" OFF)
option(FILEFORMAT_ENABLE_SANITIZERS "Enable sanitizers (ASan, UBSan)" OFF)
option(FILEFORMAT_ENABLE_CLANG_TIDY "Enable clang-tidy" OFF)

# 跨平台编译器设置
if(MSVC)
    add_compile_options(/W4 /WX /utf-8)
    add_compile_definitions(_CRT_SECURE_NO_WARNINGS)
else()
    add_compile_options(-Wall -Wextra -Wpedantic -Werror)
endif()

# Sanitizers
if(FILEFORMAT_ENABLE_SANITIZERS AND NOT MSVC)
    add_compile_options(-fsanitize=address,undefined -fno-omit-frame-pointer)
    add_link_options(-fsanitize=address,undefined)
endif()

# clang-tidy
if(FILEFORMAT_ENABLE_CLANG_TIDY)
    find_program(CLANG_TIDY_EXE NAMES clang-tidy)
    if(CLANG_TIDY_EXE)
        set(CMAKE_CXX_CLANG_TIDY "${CLANG_TIDY_EXE}")
    endif()
endif()

# 库源文件
set(FILEFORMAT_SOURCES
    src/detector.cpp
    src/formats/image.cpp
    src/formats/document.cpp
    src/formats/archive.cpp
    src/formats/ebook.cpp
    src/formats/media.cpp
    src/formats/executable.cpp
)

# 创建库
if(FILEFORMAT_BUILD_SHARED)
    add_library(fileformat SHARED ${FILEFORMAT_SOURCES})
    target_compile_definitions(fileformat PRIVATE FILEFORMAT_EXPORTS)
else()
    add_library(fileformat STATIC ${FILEFORMAT_SOURCES})
endif()

# 库别名
add_library(fileformat::fileformat ALIAS fileformat)

# 包含目录
target_include_directories(fileformat
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# 测试
if(FILEFORMAT_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

# 示例
if(FILEFORMAT_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

# 安装配置
include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

install(TARGETS fileformat
    EXPORT fileformat-targets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

install(DIRECTORY include/fileformat
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(EXPORT fileformat-targets
    FILE fileformat-targets.cmake
    NAMESPACE fileformat::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/fileformat
)

configure_package_config_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/FileFormatConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/fileformat-config.cmake
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/fileformat
)

write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/fileformat-config-version.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/fileformat-config.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/fileformat-config-version.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/fileformat
)

