#include <gtest/gtest.h>

#include <cstdint>
#include <sstream>
#include <vector>

#include "fileformat/fileformat.hpp"

namespace fileformat {
namespace {

class ApiTest : public ::testing::Test {};

// Format 枚举测试
TEST_F(ApiTest, FormatEnumValues) {
    EXPECT_EQ(static_cast<int>(Format::Unknown), 0);
    EXPECT_NE(static_cast<int>(Format::PNG), 0);
    EXPECT_NE(static_cast<int>(Format::JPEG), 0);
}

// Category 枚举测试
TEST_F(ApiTest, CategoryEnumValues) {
    EXPECT_EQ(static_cast<int>(Category::Unknown), 0);
    EXPECT_EQ(static_cast<int>(Category::Image), 1);
    EXPECT_EQ(static_cast<int>(Category::Document), 2);
}

// FormatInfo 完整性测试
TEST_F(ApiTest, AllFormatsHaveInfo) {
    // 测试所有格式都有有效的 FormatInfo
    for (int i = 0; i < static_cast<int>(Format::COUNT_); ++i) {
        auto format = static_cast<Format>(i);
        auto& info = get_info(format);

        // 每个格式应该有名称
        EXPECT_FALSE(info.name.empty()) << "Format " << i << " has empty name";

        // 每个格式应该有 MIME 类型
        EXPECT_FALSE(info.mime_type.empty()) << "Format " << i << " has empty mime_type";

        // 格式值应该匹配
        EXPECT_EQ(info.format, format) << "Format " << i << " has mismatched format field";
    }
}

// 类别名称测试
TEST_F(ApiTest, CategoryNames) {
    EXPECT_EQ(get_category_name(Category::Unknown), "unknown");
    EXPECT_EQ(get_category_name(Category::Image), "image");
    EXPECT_EQ(get_category_name(Category::Document), "document");
    EXPECT_EQ(get_category_name(Category::Ebook), "ebook");
    EXPECT_EQ(get_category_name(Category::Archive), "archive");
    EXPECT_EQ(get_category_name(Category::Media), "media");
    EXPECT_EQ(get_category_name(Category::Executable), "executable");
}

// 图像格式类别测试
TEST_F(ApiTest, ImageFormatsHaveImageCategory) {
    EXPECT_EQ(get_info(Format::PNG).category, Category::Image);
    EXPECT_EQ(get_info(Format::JPEG).category, Category::Image);
    EXPECT_EQ(get_info(Format::BMP).category, Category::Image);
    EXPECT_EQ(get_info(Format::GIF).category, Category::Image);
    EXPECT_EQ(get_info(Format::WebP).category, Category::Image);
    EXPECT_EQ(get_info(Format::TIFF).category, Category::Image);
}

// 文档格式类别测试
TEST_F(ApiTest, DocumentFormatsHaveDocumentCategory) {
    EXPECT_EQ(get_info(Format::PDF).category, Category::Document);
    EXPECT_EQ(get_info(Format::DOC).category, Category::Document);
    EXPECT_EQ(get_info(Format::DOCX).category, Category::Document);
    EXPECT_EQ(get_info(Format::XLS).category, Category::Document);
    EXPECT_EQ(get_info(Format::XLSX).category, Category::Document);
}

// 电子书格式类别测试
TEST_F(ApiTest, EbookFormatsHaveEbookCategory) {
    EXPECT_EQ(get_info(Format::EPUB).category, Category::Ebook);
    EXPECT_EQ(get_info(Format::MOBI).category, Category::Ebook);
    EXPECT_EQ(get_info(Format::AZW3).category, Category::Ebook);
    EXPECT_EQ(get_info(Format::FB2).category, Category::Ebook);
    EXPECT_EQ(get_info(Format::DJVU).category, Category::Ebook);
}

// 压缩格式类别测试
TEST_F(ApiTest, ArchiveFormatsHaveArchiveCategory) {
    EXPECT_EQ(get_info(Format::ZIP).category, Category::Archive);
    EXPECT_EQ(get_info(Format::RAR).category, Category::Archive);
    EXPECT_EQ(get_info(Format::SevenZip).category, Category::Archive);
    EXPECT_EQ(get_info(Format::GZip).category, Category::Archive);
    EXPECT_EQ(get_info(Format::Tar).category, Category::Archive);
}

// 媒体格式类别测试
TEST_F(ApiTest, MediaFormatsHaveMediaCategory) {
    EXPECT_EQ(get_info(Format::MP3).category, Category::Media);
    EXPECT_EQ(get_info(Format::MP4).category, Category::Media);
    EXPECT_EQ(get_info(Format::WAV).category, Category::Media);
    EXPECT_EQ(get_info(Format::AVI).category, Category::Media);
    EXPECT_EQ(get_info(Format::MKV).category, Category::Media);
}

// 可执行文件格式类别测试
TEST_F(ApiTest, ExecutableFormatsHaveExecutableCategory) {
    EXPECT_EQ(get_info(Format::EXE).category, Category::Executable);
    EXPECT_EQ(get_info(Format::ELF).category, Category::Executable);
    EXPECT_EQ(get_info(Format::MachO).category, Category::Executable);
}

// MIME 类型测试
TEST_F(ApiTest, CommonMimeTypes) {
    EXPECT_EQ(get_info(Format::PNG).mime_type, "image/png");
    EXPECT_EQ(get_info(Format::JPEG).mime_type, "image/jpeg");
    EXPECT_EQ(get_info(Format::PDF).mime_type, "application/pdf");
    EXPECT_EQ(get_info(Format::ZIP).mime_type, "application/zip");
    EXPECT_EQ(get_info(Format::MP3).mime_type, "audio/mpeg");
    EXPECT_EQ(get_info(Format::MP4).mime_type, "video/mp4");
}

// 文件扩展名测试
TEST_F(ApiTest, CommonExtensions) {
    EXPECT_EQ(get_info(Format::PNG).extension, ".png");
    EXPECT_EQ(get_info(Format::JPEG).extension, ".jpg");
    EXPECT_EQ(get_info(Format::PDF).extension, ".pdf");
    EXPECT_EQ(get_info(Format::ZIP).extension, ".zip");
    EXPECT_EQ(get_info(Format::MP3).extension, ".mp3");
}

// 多重检测一致性测试
TEST_F(ApiTest, ConsistentDetection) {
    std::vector<uint8_t> png_data = {0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A};

    // 多次检测应该返回相同结果
    auto format1 = detect(png_data.data(), png_data.size());
    auto format2 = detect(png_data.data(), png_data.size());
    auto format3 = detect(png_data.data(), png_data.size());

    EXPECT_EQ(format1, format2);
    EXPECT_EQ(format2, format3);
    EXPECT_EQ(format1, Format::PNG);
}

// 流检测与缓冲区检测一致性测试
TEST_F(ApiTest, StreamAndBufferConsistency) {
    std::vector<uint8_t> jpeg_data = {0xFF, 0xD8, 0xFF, 0xE0};
    std::string jpeg_str(jpeg_data.begin(), jpeg_data.end());
    std::istringstream stream(jpeg_str);

    auto buffer_format = detect(jpeg_data.data(), jpeg_data.size());
    auto stream_format = detect(stream);

    EXPECT_EQ(buffer_format, stream_format);
    EXPECT_EQ(buffer_format, Format::JPEG);
}

}  // namespace
}  // namespace fileformat

