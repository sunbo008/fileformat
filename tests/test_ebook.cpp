#include <gtest/gtest.h>

#include <cstdint>
#include <vector>

#include "fileformat/fileformat.hpp"

namespace fileformat {
namespace {

class EbookFormatTest : public ::testing::Test {
protected:
    // DJVU: AT&TFORM
    const std::vector<uint8_t> djvu_magic = {0x41, 0x54, 0x26, 0x54, 0x46, 0x4F, 0x52, 0x4D};

    // MOBI: 需要在偏移 60 处有 BOOKMOBI
    std::vector<uint8_t> mobi_magic;

    // FB2: XML with FictionBook
    const std::vector<uint8_t> fb2_magic = {
        0x3C, 0x3F, 0x78, 0x6D, 0x6C,  // <?xml
        0x20, 0x76, 0x65, 0x72, 0x73, 0x69, 0x6F, 0x6E,  // version
        0x3D, 0x22, 0x31, 0x2E, 0x30, 0x22, 0x3F, 0x3E,  // ="1.0"?>
        0x3C, 0x46, 0x69, 0x63, 0x74, 0x69, 0x6F, 0x6E,  // <Fiction
        0x42, 0x6F, 0x6F, 0x6B  // Book
    };

    void SetUp() override {
        // 构造 MOBI 头（BOOKMOBI 在偏移 60）
        mobi_magic.resize(68, 0);
        mobi_magic[60] = 'B';
        mobi_magic[61] = 'O';
        mobi_magic[62] = 'O';
        mobi_magic[63] = 'K';
        mobi_magic[64] = 'M';
        mobi_magic[65] = 'O';
        mobi_magic[66] = 'B';
        mobi_magic[67] = 'I';
    }
};

TEST_F(EbookFormatTest, DetectDjvu) {
    auto format = detect(djvu_magic.data(), djvu_magic.size());
    EXPECT_EQ(format, Format::DJVU);
}

TEST_F(EbookFormatTest, DetectMobi) {
    auto format = detect(mobi_magic.data(), mobi_magic.size());
    EXPECT_EQ(format, Format::MOBI);
}

TEST_F(EbookFormatTest, DetectFb2) {
    auto format = detect(fb2_magic.data(), fb2_magic.size());
    EXPECT_EQ(format, Format::FB2);
}

TEST_F(EbookFormatTest, EpubFormatInfo) {
    auto& info = get_info(Format::EPUB);
    EXPECT_EQ(info.format, Format::EPUB);
    EXPECT_EQ(info.name, "EPUB");
    EXPECT_EQ(info.mime_type, "application/epub+zip");
    EXPECT_EQ(info.extension, ".epub");
    EXPECT_EQ(info.category, Category::Ebook);
}

TEST_F(EbookFormatTest, MobiFormatInfo) {
    auto& info = get_info(Format::MOBI);
    EXPECT_EQ(info.format, Format::MOBI);
    EXPECT_EQ(info.name, "MOBI");
    EXPECT_EQ(info.category, Category::Ebook);
}

}  // namespace
}  // namespace fileformat

