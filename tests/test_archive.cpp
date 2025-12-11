#include <gtest/gtest.h>

#include <cstdint>
#include <vector>

#include "fileformat/fileformat.hpp"

namespace fileformat {
namespace {

class ArchiveFormatTest : public ::testing::Test {
protected:
    // ZIP: PK\x03\x04
    const std::vector<uint8_t> zip_magic = {0x50, 0x4B, 0x03, 0x04};

    // RAR: Rar!\x1A\x07
    const std::vector<uint8_t> rar_magic = {0x52, 0x61, 0x72, 0x21, 0x1A, 0x07};

    // 7-Zip
    const std::vector<uint8_t> sevenzip_magic = {0x37, 0x7A, 0xBC, 0xAF, 0x27, 0x1C};

    // GZIP
    const std::vector<uint8_t> gzip_magic = {0x1F, 0x8B, 0x08};

    // TAR (ustar at offset 257) - simplified test data
    std::vector<uint8_t> tar_magic;

    void SetUp() override {
        // 构造一个最小的 TAR 头
        tar_magic.resize(262, 0);
        // "ustar" at offset 257
        tar_magic[257] = 'u';
        tar_magic[258] = 's';
        tar_magic[259] = 't';
        tar_magic[260] = 'a';
        tar_magic[261] = 'r';
    }
};

TEST_F(ArchiveFormatTest, DetectZip) {
    auto format = detect(zip_magic.data(), zip_magic.size());
    EXPECT_EQ(format, Format::ZIP);
}

TEST_F(ArchiveFormatTest, DetectRar) {
    auto format = detect(rar_magic.data(), rar_magic.size());
    EXPECT_EQ(format, Format::RAR);
}

TEST_F(ArchiveFormatTest, DetectSevenZip) {
    auto format = detect(sevenzip_magic.data(), sevenzip_magic.size());
    EXPECT_EQ(format, Format::SevenZip);
}

TEST_F(ArchiveFormatTest, DetectGzip) {
    auto format = detect(gzip_magic.data(), gzip_magic.size());
    EXPECT_EQ(format, Format::GZip);
}

TEST_F(ArchiveFormatTest, DetectTar) {
    auto format = detect(tar_magic.data(), tar_magic.size());
    EXPECT_EQ(format, Format::Tar);
}

TEST_F(ArchiveFormatTest, ZipFormatInfo) {
    auto& info = get_info(Format::ZIP);
    EXPECT_EQ(info.format, Format::ZIP);
    EXPECT_EQ(info.name, "ZIP");
    EXPECT_EQ(info.mime_type, "application/zip");
    EXPECT_EQ(info.extension, ".zip");
    EXPECT_EQ(info.category, Category::Archive);
}

}  // namespace
}  // namespace fileformat

