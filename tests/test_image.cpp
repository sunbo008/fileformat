#include <gtest/gtest.h>

#include <array>
#include <cstdint>
#include <vector>

#include "fileformat/fileformat.hpp"

namespace fileformat {
namespace {

class ImageFormatTest : public ::testing::Test {
protected:
    // PNG: 89 50 4E 47 0D 0A 1A 0A
    const std::vector<uint8_t> png_magic = {0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A};

    // JPEG: FF D8 FF
    const std::vector<uint8_t> jpeg_magic = {0xFF, 0xD8, 0xFF, 0xE0};

    // BMP: BM
    const std::vector<uint8_t> bmp_magic = {0x42, 0x4D, 0x00, 0x00};

    // GIF89a
    const std::vector<uint8_t> gif_magic = {0x47, 0x49, 0x46, 0x38, 0x39, 0x61};

    // GIF87a
    const std::vector<uint8_t> gif87_magic = {0x47, 0x49, 0x46, 0x38, 0x37, 0x61};

    // WebP: RIFF....WEBP
    const std::vector<uint8_t> webp_magic = {0x52, 0x49, 0x46, 0x46, 0x00, 0x00,
                                              0x00, 0x00, 0x57, 0x45, 0x42, 0x50};

    // TIFF (little-endian): II*\0
    const std::vector<uint8_t> tiff_le_magic = {0x49, 0x49, 0x2A, 0x00};

    // TIFF (big-endian): MM\0*
    const std::vector<uint8_t> tiff_be_magic = {0x4D, 0x4D, 0x00, 0x2A};
};

TEST_F(ImageFormatTest, DetectPng) {
    auto format = detect(png_magic.data(), png_magic.size());
    EXPECT_EQ(format, Format::PNG);
}

TEST_F(ImageFormatTest, DetectJpeg) {
    auto format = detect(jpeg_magic.data(), jpeg_magic.size());
    EXPECT_EQ(format, Format::JPEG);
}

TEST_F(ImageFormatTest, DetectBmp) {
    auto format = detect(bmp_magic.data(), bmp_magic.size());
    EXPECT_EQ(format, Format::BMP);
}

TEST_F(ImageFormatTest, DetectGif89a) {
    auto format = detect(gif_magic.data(), gif_magic.size());
    EXPECT_EQ(format, Format::GIF);
}

TEST_F(ImageFormatTest, DetectGif87a) {
    auto format = detect(gif87_magic.data(), gif87_magic.size());
    EXPECT_EQ(format, Format::GIF);
}

TEST_F(ImageFormatTest, DetectWebp) {
    auto format = detect(webp_magic.data(), webp_magic.size());
    EXPECT_EQ(format, Format::WebP);
}

TEST_F(ImageFormatTest, DetectTiffLittleEndian) {
    auto format = detect(tiff_le_magic.data(), tiff_le_magic.size());
    EXPECT_EQ(format, Format::TIFF);
}

TEST_F(ImageFormatTest, DetectTiffBigEndian) {
    auto format = detect(tiff_be_magic.data(), tiff_be_magic.size());
    EXPECT_EQ(format, Format::TIFF);
}

TEST_F(ImageFormatTest, PngFormatInfo) {
    auto& info = get_info(Format::PNG);
    EXPECT_EQ(info.format, Format::PNG);
    EXPECT_EQ(info.name, "PNG");
    EXPECT_EQ(info.mime_type, "image/png");
    EXPECT_EQ(info.extension, ".png");
    EXPECT_EQ(info.category, Category::Image);
}

TEST_F(ImageFormatTest, JpegFormatInfo) {
    auto& info = get_info(Format::JPEG);
    EXPECT_EQ(info.format, Format::JPEG);
    EXPECT_EQ(info.name, "JPEG");
    EXPECT_EQ(info.mime_type, "image/jpeg");
}

}  // namespace
}  // namespace fileformat

