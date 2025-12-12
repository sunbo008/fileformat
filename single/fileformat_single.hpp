/**
 * @file fileformat_single.hpp
 * @brief 单文件版本的文件格式检测库
 * @version 1.0.0
 * 
 * 使用方法：
 * @code
 * #include "fileformat_single.hpp"
 * 
 * std::vector<uint8_t> data = read_file("test.png");
 * auto fmt = fileformat::detect(data.data(), data.size());
 * if (fmt == fileformat::Format::PNG) {
 *     // 处理 PNG 文件
 * }
 * @endcode
 * 
 * 支持的格式：
 * - 图像: PNG, JPEG, BMP, GIF, WebP, TIFF
 * - 文档: PDF, DOC, DOCX, XLS, XLSX, PPT, PPTX
 * - 电子书: EPUB, MOBI, AZW3, FB2, DJVU
 * - 压缩: ZIP, RAR, 7Z, GZIP, TAR
 * - 媒体: MP3, MP4, WAV, AVI, MKV
 * - 可执行: EXE, ELF, Mach-O
 */

#ifndef FILEFORMAT_SINGLE_HPP
#define FILEFORMAT_SINGLE_HPP

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <string>
#include <string_view>

namespace fileformat {

//==============================================================================
// 类型定义
//==============================================================================

/// 支持的文件格式枚举（内部使用）
enum class Format {
    Unknown = 0,

    // 图像格式
    PNG, JPEG, BMP, GIF, WebP, TIFF,

    // 文档格式
    PDF, DOC, DOCX, XLS, XLSX, PPT, PPTX,

    // 电子书格式
    EPUB, MOBI, AZW3, FB2, DJVU,

    // 压缩格式
    ZIP, RAR, SevenZip, GZip, Tar,

    // 媒体格式
    MP3, MP4, WAV, AVI, MKV,

    // 可执行文件格式
    EXE, ELF, MachO
};

/// 将格式枚举转换为字符串
[[nodiscard]] inline std::string format_to_string(Format fmt) noexcept {
    switch (fmt) {
        case Format::Unknown:   return "Unknown";
        // 图像
        case Format::PNG:       return "PNG";
        case Format::JPEG:      return "JPEG";
        case Format::BMP:       return "BMP";
        case Format::GIF:       return "GIF";
        case Format::WebP:      return "WebP";
        case Format::TIFF:      return "TIFF";
        // 文档
        case Format::PDF:       return "PDF";
        case Format::DOC:       return "DOC";
        case Format::DOCX:      return "DOCX";
        case Format::XLS:       return "XLS";
        case Format::XLSX:      return "XLSX";
        case Format::PPT:       return "PPT";
        case Format::PPTX:      return "PPTX";
        // 电子书
        case Format::EPUB:      return "EPUB";
        case Format::MOBI:      return "MOBI";
        case Format::AZW3:      return "AZW3";
        case Format::FB2:       return "FB2";
        case Format::DJVU:      return "DJVU";
        // 压缩
        case Format::ZIP:       return "ZIP";
        case Format::RAR:       return "RAR";
        case Format::SevenZip:  return "7Z";
        case Format::GZip:      return "GZIP";
        case Format::Tar:       return "TAR";
        // 媒体
        case Format::MP3:       return "MP3";
        case Format::MP4:       return "MP4";
        case Format::WAV:       return "WAV";
        case Format::AVI:       return "AVI";
        case Format::MKV:       return "MKV";
        // 可执行
        case Format::EXE:       return "EXE";
        case Format::ELF:       return "ELF";
        case Format::MachO:     return "MACHO";
        default:                return "Unknown";
    }
}

//==============================================================================
// 内部工具函数
//==============================================================================

namespace detail {

/// 比较内存
inline bool mem_equal(const uint8_t* data, const uint8_t* pattern, size_t len) {
    return std::memcmp(data, pattern, len) == 0;
}

//------------------------------------------------------------------------------
// 图像格式检测
//------------------------------------------------------------------------------

inline Format detect_image(const uint8_t* data, size_t size) noexcept {
    if (data == nullptr || size < 2) {
        return Format::Unknown;
    }

    // PNG: 89 50 4E 47 0D 0A 1A 0A
    constexpr uint8_t kPngMagic[] = {0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A};
    if (size >= 8 && mem_equal(data, kPngMagic, 8)) {
        return Format::PNG;
    }

    // JPEG: FF D8 FF
    constexpr uint8_t kJpegMagic[] = {0xFF, 0xD8, 0xFF};
    if (size >= 3 && mem_equal(data, kJpegMagic, 3)) {
        return Format::JPEG;
    }

    // BMP: 42 4D ("BM")
    constexpr uint8_t kBmpMagic[] = {0x42, 0x4D};
    if (size >= 2 && mem_equal(data, kBmpMagic, 2)) {
        return Format::BMP;
    }

    // GIF: GIF87a 或 GIF89a
    constexpr uint8_t kGif87Magic[] = {0x47, 0x49, 0x46, 0x38, 0x37, 0x61};
    constexpr uint8_t kGif89Magic[] = {0x47, 0x49, 0x46, 0x38, 0x39, 0x61};
    if (size >= 6) {
        if (mem_equal(data, kGif87Magic, 6) || mem_equal(data, kGif89Magic, 6)) {
            return Format::GIF;
        }
    }

    // RIFF 容器（WebP）
    constexpr uint8_t kRiffMagic[] = {0x52, 0x49, 0x46, 0x46};
    constexpr uint8_t kWebpMagic[] = {0x57, 0x45, 0x42, 0x50};
    if (size >= 12 && mem_equal(data, kRiffMagic, 4)) {
        if (mem_equal(data + 8, kWebpMagic, 4)) {
            return Format::WebP;
        }
    }

    // TIFF: II*\0 (little-endian) 或 MM\0* (big-endian)
    constexpr uint8_t kTiffLeMagic[] = {0x49, 0x49, 0x2A, 0x00};
    constexpr uint8_t kTiffBeMagic[] = {0x4D, 0x4D, 0x00, 0x2A};
    if (size >= 4) {
        if (mem_equal(data, kTiffLeMagic, 4) || mem_equal(data, kTiffBeMagic, 4)) {
            return Format::TIFF;
        }
    }

    return Format::Unknown;
}

//------------------------------------------------------------------------------
// OLE2 复合文档类型检测（区分 DOC/XLS/PPT）
//------------------------------------------------------------------------------

// 在数据中搜索 UTF-16LE 编码的字符串（OLE2 流名称使用 UTF-16LE）
inline bool find_utf16le_string(const uint8_t* data, size_t size, const char* str) noexcept {
    size_t str_len = 0;
    while (str[str_len] != '\0') str_len++;
    
    if (size < str_len * 2) return false;
    
    // 将 ASCII 字符串转换为 UTF-16LE 模式进行搜索
    for (size_t i = 0; i + str_len * 2 <= size; ++i) {
        bool match = true;
        for (size_t j = 0; j < str_len && match; ++j) {
            // UTF-16LE: 低字节是字符，高字节是 0（对于 ASCII）
            if (data[i + j * 2] != static_cast<uint8_t>(str[j]) ||
                data[i + j * 2 + 1] != 0) {
                match = false;
            }
        }
        if (match) return true;
    }
    return false;
}

inline Format detect_ole2_type(const uint8_t* data, size_t size) noexcept {
    // OLE2 内部流名称（UTF-16LE 编码）:
    // - Word: "WordDocument"
    // - Excel: "Workbook" 或 "Book"
    // - PowerPoint: "PowerPoint Document"
    
    // 搜索范围：整个文件头部（最多 kMaxHeaderSize）
    
    // PowerPoint（优先检测，因为名称最独特）
    if (find_utf16le_string(data, size, "PowerPoint Document") ||
        find_utf16le_string(data, size, "PP40")) {
        return Format::PPT;
    }
    
    // Excel: "Workbook" 流名称
    if (find_utf16le_string(data, size, "Workbook") ||
        find_utf16le_string(data, size, "Book")) {
        return Format::XLS;
    }
    
    // Word: "WordDocument" 流名称
    if (find_utf16le_string(data, size, "WordDocument")) {
        return Format::DOC;
    }
    
    // 默认返回 DOC（最常见）
    return Format::DOC;
}

//------------------------------------------------------------------------------
// 文档格式检测
//------------------------------------------------------------------------------

inline Format detect_document(const uint8_t* data, size_t size) noexcept {
    if (data == nullptr || size < 4) {
        return Format::Unknown;
    }

    // PDF: %PDF
    constexpr uint8_t kPdfMagic[] = {0x25, 0x50, 0x44, 0x46};
    if (size >= 4 && mem_equal(data, kPdfMagic, 4)) {
        return Format::PDF;
    }

    // OLE Compound Document (DOC, XLS, PPT)
    constexpr uint8_t kOleMagic[] = {0xD0, 0xCF, 0x11, 0xE0, 0xA1, 0xB1, 0x1A, 0xE1};
    if (size >= 8 && mem_equal(data, kOleMagic, 8)) {
        return detect_ole2_type(data, size);
    }

    return Format::Unknown;
}

//------------------------------------------------------------------------------
// 压缩格式检测
//------------------------------------------------------------------------------

inline Format detect_zip_content(const uint8_t* data, size_t size) noexcept {
    if (data == nullptr || size < 30) {
        return Format::Unknown;
    }

    // 在整个数据中搜索特征字符串
    std::string_view data_view(reinterpret_cast<const char*>(data), size);

    // EPUB: 检查 mimetype 文件内容
    if (data_view.find("application/epub+zip") != std::string_view::npos) {
        return Format::EPUB;
    }
    // EPUB 备用检测
    if (data_view.find("mimetype") != std::string_view::npos && 
        data_view.find("META-INF/") != std::string_view::npos) {
        return Format::EPUB;
    }

    // Office Open XML: 直接检查特征目录（不需要先验证 Office 特征）
    // PPTX: ppt/ 目录
    if (data_view.find("ppt/") != std::string_view::npos) {
        return Format::PPTX;
    }
    
    // XLSX: xl/ 目录（Excel 特有）
    if (data_view.find("xl/") != std::string_view::npos) {
        return Format::XLSX;
    }
    
    // DOCX: word/ 目录
    if (data_view.find("word/") != std::string_view::npos) {
        return Format::DOCX;
    }

    return Format::Unknown;
}

inline Format detect_archive(const uint8_t* data, size_t size) noexcept {
    if (data == nullptr || size < 2) {
        return Format::Unknown;
    }

    // ZIP: PK\x03\x04
    constexpr uint8_t kZipMagic[] = {0x50, 0x4B, 0x03, 0x04};
    constexpr uint8_t kZipEmptyMagic[] = {0x50, 0x4B, 0x05, 0x06};
    constexpr uint8_t kZipSpannedMagic[] = {0x50, 0x4B, 0x07, 0x08};
    if (size >= 4) {
        if (mem_equal(data, kZipMagic, 4) || mem_equal(data, kZipEmptyMagic, 4) ||
            mem_equal(data, kZipSpannedMagic, 4)) {
            return Format::ZIP;
        }
    }

    // RAR: Rar!\x1A\x07
    constexpr uint8_t kRarMagic[] = {0x52, 0x61, 0x72, 0x21, 0x1A, 0x07};
    if (size >= 6 && mem_equal(data, kRarMagic, 6)) {
        return Format::RAR;
    }

    // 7-Zip
    constexpr uint8_t kSevenZipMagic[] = {0x37, 0x7A, 0xBC, 0xAF, 0x27, 0x1C};
    if (size >= 6 && mem_equal(data, kSevenZipMagic, 6)) {
        return Format::SevenZip;
    }

    // GZIP
    constexpr uint8_t kGzipMagic[] = {0x1F, 0x8B};
    if (size >= 2 && mem_equal(data, kGzipMagic, 2)) {
        return Format::GZip;
    }

    // TAR: "ustar" at offset 257
    constexpr uint8_t kTarUstarMagic[] = {0x75, 0x73, 0x74, 0x61, 0x72};
    if (size >= 262 && mem_equal(data + 257, kTarUstarMagic, 5)) {
        return Format::Tar;
    }

    return Format::Unknown;
}

//------------------------------------------------------------------------------
// 电子书格式检测
//------------------------------------------------------------------------------

inline Format detect_ebook(const uint8_t* data, size_t size) noexcept {
    if (data == nullptr || size < 8) {
        return Format::Unknown;
    }

    // MOBI: BOOKMOBI at offset 60
    constexpr uint8_t kMobiMagic[] = {0x42, 0x4F, 0x4F, 0x4B, 0x4D, 0x4F, 0x42, 0x49};
    if (size >= 68 && mem_equal(data + 60, kMobiMagic, 8)) {
        // 检查是否为 AZW3
        std::string_view view(reinterpret_cast<const char*>(data), size);
        if (view.find("KF8") != std::string_view::npos) {
            return Format::AZW3;
        }
        return Format::MOBI;
    }

    // DJVU: AT&TFORM
    constexpr uint8_t kDjvuMagic[] = {0x41, 0x54, 0x26, 0x54, 0x46, 0x4F, 0x52, 0x4D};
    if (size >= 8 && mem_equal(data, kDjvuMagic, 8)) {
        return Format::DJVU;
    }

    // FB2: XML with FictionBook
    constexpr uint8_t kXmlMagic[] = {0x3C, 0x3F, 0x78, 0x6D, 0x6C};
    if (size >= 5 && mem_equal(data, kXmlMagic, 5)) {
        std::string_view view(reinterpret_cast<const char*>(data),
                              std::min(size, static_cast<size_t>(1024)));
        if (view.find("FictionBook") != std::string_view::npos) {
            return Format::FB2;
        }
    }

    return Format::Unknown;
}

//------------------------------------------------------------------------------
// 媒体格式检测
//------------------------------------------------------------------------------

inline Format detect_media(const uint8_t* data, size_t size) noexcept {
    if (data == nullptr || size < 2) {
        return Format::Unknown;
    }

    // MP3: ID3 tag 或帧同步
    constexpr uint8_t kId3Magic[] = {0x49, 0x44, 0x33};
    if (size >= 3 && mem_equal(data, kId3Magic, 3)) {
        return Format::MP3;
    }

    // MP3 frame sync
    if (size >= 2 && data[0] == 0xFF && 
        (data[1] == 0xFB || data[1] == 0xFA || data[1] == 0xF3 || data[1] == 0xF2)) {
        return Format::MP3;
    }

    // RIFF 容器（WAV, AVI）
    constexpr uint8_t kRiffMagic[] = {0x52, 0x49, 0x46, 0x46};
    constexpr uint8_t kWaveMagic[] = {0x57, 0x41, 0x56, 0x45};
    constexpr uint8_t kAviMagic[] = {0x41, 0x56, 0x49, 0x20};
    if (size >= 12 && mem_equal(data, kRiffMagic, 4)) {
        if (mem_equal(data + 8, kWaveMagic, 4)) {
            return Format::WAV;
        }
        if (mem_equal(data + 8, kAviMagic, 4)) {
            return Format::AVI;
        }
    }

    // MP4: ftyp box
    constexpr uint8_t kFtypMagic[] = {0x66, 0x74, 0x79, 0x70};
    if (size >= 8 && mem_equal(data + 4, kFtypMagic, 4)) {
        return Format::MP4;
    }

    // MKV: EBML header
    constexpr uint8_t kMkvMagic[] = {0x1A, 0x45, 0xDF, 0xA3};
    if (size >= 4 && mem_equal(data, kMkvMagic, 4)) {
        return Format::MKV;
    }

    return Format::Unknown;
}

//------------------------------------------------------------------------------
// 可执行文件格式检测
//------------------------------------------------------------------------------

inline Format detect_executable(const uint8_t* data, size_t size) noexcept {
    if (data == nullptr || size < 2) {
        return Format::Unknown;
    }

    // Windows PE: MZ
    constexpr uint8_t kMzMagic[] = {0x4D, 0x5A};
    if (size >= 2 && mem_equal(data, kMzMagic, 2)) {
        return Format::EXE;
    }

    // ELF: \x7FELF
    constexpr uint8_t kElfMagic[] = {0x7F, 0x45, 0x4C, 0x46};
    if (size >= 4 && mem_equal(data, kElfMagic, 4)) {
        return Format::ELF;
    }

    // Mach-O
    constexpr uint8_t kMachoMagic32[] = {0xFE, 0xED, 0xFA, 0xCE};
    constexpr uint8_t kMachoMagic64[] = {0xFE, 0xED, 0xFA, 0xCF};
    constexpr uint8_t kMachoMagic32Rev[] = {0xCE, 0xFA, 0xED, 0xFE};
    constexpr uint8_t kMachoMagic64Rev[] = {0xCF, 0xFA, 0xED, 0xFE};
    constexpr uint8_t kMachoFatMagic[] = {0xCA, 0xFE, 0xBA, 0xBE};
    if (size >= 4) {
        if (mem_equal(data, kMachoMagic32, 4) ||
            mem_equal(data, kMachoMagic64, 4) ||
            mem_equal(data, kMachoMagic32Rev, 4) ||
            mem_equal(data, kMachoMagic64Rev, 4) ||
            mem_equal(data, kMachoFatMagic, 4)) {
            return Format::MachO;
        }
    }

    return Format::Unknown;
}

}  // namespace detail

//==============================================================================
// 公共 API
//==============================================================================

/**
 * @brief 检测文件格式（内部实现，返回枚举）
 */
[[nodiscard]] inline Format detect_format(const uint8_t* data, size_t size) noexcept {
    // 输入验证
    if (data == nullptr || size < 2) {
        return Format::Unknown;
    }

    // 1. 图像格式
    if (auto fmt = detail::detect_image(data, size); fmt != Format::Unknown) {
        return fmt;
    }

    // 2. 压缩格式和基于 ZIP 的格式
    if (auto fmt = detail::detect_archive(data, size); fmt != Format::Unknown) {
        if (fmt == Format::ZIP) {
            auto content_fmt = detail::detect_zip_content(data, size);
            if (content_fmt != Format::Unknown) {
                return content_fmt;
            }
        }
        return fmt;
    }

    // 3. 文档格式
    if (auto fmt = detail::detect_document(data, size); fmt != Format::Unknown) {
        return fmt;
    }

    // 4. 电子书格式
    if (auto fmt = detail::detect_ebook(data, size); fmt != Format::Unknown) {
        return fmt;
    }

    // 5. 媒体格式
    if (auto fmt = detail::detect_media(data, size); fmt != Format::Unknown) {
        return fmt;
    }

    // 6. 可执行文件格式
    if (auto fmt = detail::detect_executable(data, size); fmt != Format::Unknown) {
        return fmt;
    }

    return Format::Unknown;
}

/**
 * @brief 检测文件格式（通过内存缓冲区）
 * @param data 文件数据指针
 * @param size 数据大小
 * @return 检测到的格式名称字符串，无法识别时返回 "Unknown"
 * @note 不抛异常，空指针或零大小返回 "Unknown"
 */
[[nodiscard]] inline std::string detect(const uint8_t* data, size_t size) noexcept {
    return format_to_string(detect_format(data, size));
}

}  // namespace fileformat

#endif  // FILEFORMAT_SINGLE_HPP

