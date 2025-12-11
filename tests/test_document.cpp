#include <gtest/gtest.h>

#include <cstdint>
#include <vector>

#include "fileformat/fileformat.hpp"

namespace fileformat {
namespace {

class DocumentFormatTest : public ::testing::Test {
protected:
    // PDF: %PDF
    const std::vector<uint8_t> pdf_magic = {0x25, 0x50, 0x44, 0x46, 0x2D, 0x31, 0x2E, 0x34};

    // OLE Compound Document (DOC, XLS, PPT)
    const std::vector<uint8_t> ole_magic = {0xD0, 0xCF, 0x11, 0xE0, 0xA1, 0xB1, 0x1A, 0xE1};
};

TEST_F(DocumentFormatTest, DetectPdf) {
    auto format = detect(pdf_magic.data(), pdf_magic.size());
    EXPECT_EQ(format, Format::PDF);
}

TEST_F(DocumentFormatTest, DetectOleDocument) {
    auto format = detect(ole_magic.data(), ole_magic.size());
    // OLE 文档默认识别为 DOC
    EXPECT_EQ(format, Format::DOC);
}

TEST_F(DocumentFormatTest, PdfFormatInfo) {
    auto& info = get_info(Format::PDF);
    EXPECT_EQ(info.format, Format::PDF);
    EXPECT_EQ(info.name, "PDF");
    EXPECT_EQ(info.mime_type, "application/pdf");
    EXPECT_EQ(info.extension, ".pdf");
    EXPECT_EQ(info.category, Category::Document);
}

TEST_F(DocumentFormatTest, DocxFormatInfo) {
    auto& info = get_info(Format::DOCX);
    EXPECT_EQ(info.format, Format::DOCX);
    EXPECT_EQ(info.name, "DOCX");
    EXPECT_EQ(info.extension, ".docx");
    EXPECT_EQ(info.category, Category::Document);
}

}  // namespace
}  // namespace fileformat

