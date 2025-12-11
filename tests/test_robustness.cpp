#include <gtest/gtest.h>

#include <cstdint>
#include <sstream>
#include <vector>

#include "fileformat/fileformat.hpp"

namespace fileformat {
namespace {

class RobustnessTest : public ::testing::Test {};

// 输入验证测试
TEST_F(RobustnessTest, NullPointerInput) {
    auto format = detect(static_cast<const uint8_t*>(nullptr), 100);
    EXPECT_EQ(format, Format::Unknown);
}

TEST_F(RobustnessTest, ZeroSizeBuffer) {
    std::vector<uint8_t> data = {0x89, 0x50, 0x4E, 0x47};
    auto format = detect(data.data(), 0);
    EXPECT_EQ(format, Format::Unknown);
}

TEST_F(RobustnessTest, VerySmallBuffer) {
    std::vector<uint8_t> data = {0x89};
    auto format = detect(data.data(), data.size());
    EXPECT_EQ(format, Format::Unknown);
}

TEST_F(RobustnessTest, EmptyPath) {
    auto format = detect("");
    EXPECT_EQ(format, Format::Unknown);
}

TEST_F(RobustnessTest, NonExistentFile) {
    auto format = detect("/nonexistent/path/to/file.xyz");
    EXPECT_EQ(format, Format::Unknown);
}

// detect_safe 测试
TEST_F(RobustnessTest, DetectSafeEmptyPath) {
    auto result = detect_safe("");
    EXPECT_FALSE(result.is_valid());
    EXPECT_TRUE(result.error);
}

TEST_F(RobustnessTest, DetectSafeNonExistentFile) {
    auto result = detect_safe("/nonexistent/file.bin");
    EXPECT_FALSE(result.is_valid());
    EXPECT_TRUE(result.error);
}

// detect_or_throw 测试
TEST_F(RobustnessTest, DetectOrThrowNonExistentFile) {
    EXPECT_THROW(detect_or_throw("/nonexistent/file.bin"), std::system_error);
}

// 流检测测试
TEST_F(RobustnessTest, DetectFromEmptyStream) {
    std::istringstream empty_stream("");
    auto format = detect(empty_stream);
    EXPECT_EQ(format, Format::Unknown);
}

TEST_F(RobustnessTest, DetectFromValidStream) {
    // PNG magic bytes
    std::string png_data("\x89PNG\r\n\x1a\n", 8);
    std::istringstream stream(png_data);
    auto format = detect(stream);
    EXPECT_EQ(format, Format::PNG);
}

TEST_F(RobustnessTest, StreamPositionPreserved) {
    std::string data("\x89PNG\r\n\x1a\n extra data", 20);
    std::istringstream stream(data);

    // 移动到某个位置
    stream.seekg(0);
    auto original_pos = stream.tellg();

    // 检测格式
    detect(stream);

    // 验证位置被恢复
    EXPECT_EQ(stream.tellg(), original_pos);
}

// 未知格式测试
TEST_F(RobustnessTest, UnknownFormatBytes) {
    std::vector<uint8_t> random_data = {0x12, 0x34, 0x56, 0x78, 0x9A, 0xBC, 0xDE, 0xF0};
    auto format = detect(random_data.data(), random_data.size());
    EXPECT_EQ(format, Format::Unknown);
}

// 格式信息查询测试
TEST_F(RobustnessTest, GetInfoUnknownFormat) {
    auto& info = get_info(Format::Unknown);
    EXPECT_EQ(info.format, Format::Unknown);
    EXPECT_EQ(info.name, "Unknown");
}

TEST_F(RobustnessTest, GetInfoInvalidFormat) {
    // 测试超出范围的枚举值
    auto& info = get_info(static_cast<Format>(9999));
    EXPECT_EQ(info.format, Format::Unknown);
}

TEST_F(RobustnessTest, GetCategoryNameUnknown) {
    auto name = get_category_name(Category::Unknown);
    EXPECT_EQ(name, "unknown");
}

TEST_F(RobustnessTest, GetCategoryNameInvalid) {
    auto name = get_category_name(static_cast<Category>(9999));
    EXPECT_EQ(name, "unknown");
}

// 批量检测测试
TEST_F(RobustnessTest, DetectBatchEmpty) {
    std::vector<std::string> empty_paths;
    auto results = detect_batch(empty_paths);
    EXPECT_TRUE(results.empty());
}

TEST_F(RobustnessTest, DetectBatchWithInvalidPaths) {
    std::vector<std::string> paths = {"/nonexistent1.bin", "/nonexistent2.bin"};
    auto results = detect_batch(paths);
    EXPECT_EQ(results.size(), 2);
    EXPECT_EQ(results[0].second, Format::Unknown);
    EXPECT_EQ(results[1].second, Format::Unknown);
}

// DetectResult 测试
TEST_F(RobustnessTest, DetectResultIsValid) {
    DetectResult valid_result;
    valid_result.format = Format::PNG;
    EXPECT_TRUE(valid_result.is_valid());

    DetectResult invalid_result;
    invalid_result.error = std::make_error_code(std::errc::no_such_file_or_directory);
    EXPECT_FALSE(invalid_result.is_valid());
}

TEST_F(RobustnessTest, DetectResultIsKnown) {
    DetectResult known_result;
    known_result.format = Format::PNG;
    EXPECT_TRUE(known_result.is_known());

    DetectResult unknown_result;
    unknown_result.format = Format::Unknown;
    EXPECT_FALSE(unknown_result.is_known());
}

TEST_F(RobustnessTest, DetectResultImplicitConversion) {
    DetectResult result;
    result.format = Format::JPEG;
    Format format = result;  // 隐式转换
    EXPECT_EQ(format, Format::JPEG);
}

}  // namespace
}  // namespace fileformat

