# 测试配置

include(FetchContent)

# 下载 Google Test
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG v1.14.0
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# 测试源文件
set(TEST_SOURCES
    test_main.cpp
    test_image.cpp
    test_document.cpp
    test_archive.cpp
    test_ebook.cpp
    test_media.cpp
    test_executable.cpp
    test_robustness.cpp
    test_api.cpp
)

# 创建测试可执行文件
add_executable(fileformat_tests ${TEST_SOURCES})

target_link_libraries(fileformat_tests
    PRIVATE
        fileformat
        GTest::gtest
        GTest::gtest_main
)

# 启用测试发现
include(GoogleTest)
gtest_discover_tests(fileformat_tests)

# 测试数据目录
set(TEST_DATA_DIR "${CMAKE_CURRENT_SOURCE_DIR}/data")
target_compile_definitions(fileformat_tests PRIVATE
    TEST_DATA_DIR="${TEST_DATA_DIR}"
)

