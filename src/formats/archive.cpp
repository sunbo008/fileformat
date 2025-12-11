#include "fileformat/detector.hpp"

#include <algorithm>
#include <cstring>

namespace fileformat {
namespace detail {

namespace {

// Magic bytes 常量
constexpr uint8_t kZipMagic[] = {0x50, 0x4B, 0x03, 0x04};
constexpr uint8_t kZipEmptyMagic[] = {0x50, 0x4B, 0x05, 0x06};  // 空 ZIP
constexpr uint8_t kZipSpannedMagic[] = {0x50, 0x4B, 0x07, 0x08};  // 分卷 ZIP
constexpr uint8_t kRarMagic[] = {0x52, 0x61, 0x72, 0x21, 0x1A, 0x07};  // "Rar!"
constexpr uint8_t kSevenZipMagic[] = {0x37, 0x7A, 0xBC, 0xAF, 0x27, 0x1C};  // 7z
constexpr uint8_t kGzipMagic[] = {0x1F, 0x8B};
constexpr uint8_t kTarUstarMagic[] = {0x75, 0x73, 0x74, 0x61, 0x72};  // "ustar" at offset 257

/// 比较内存
inline bool mem_equal(const uint8_t* data, const uint8_t* pattern, size_t len) {
    return std::memcmp(data, pattern, len) == 0;
}

}  // namespace

Format detect_archive(const uint8_t* data, size_t size) noexcept {
    if (data == nullptr || size < 2) {
        return Format::Unknown;
    }

    // ZIP: PK\x03\x04 或 PK\x05\x06 (空) 或 PK\x07\x08 (分卷)
    if (size >= 4) {
        if (mem_equal(data, kZipMagic, 4) || mem_equal(data, kZipEmptyMagic, 4) ||
            mem_equal(data, kZipSpannedMagic, 4)) {
            return Format::ZIP;
        }
    }

    // RAR: Rar!\x1A\x07
    if (size >= 6 && mem_equal(data, kRarMagic, 6)) {
        return Format::RAR;
    }

    // 7-Zip: 37 7A BC AF 27 1C
    if (size >= 6 && mem_equal(data, kSevenZipMagic, 6)) {
        return Format::SevenZip;
    }

    // GZIP: 1F 8B
    if (size >= 2 && mem_equal(data, kGzipMagic, 2)) {
        return Format::GZip;
    }

    // TAR: "ustar" at offset 257
    if (size >= 262 && mem_equal(data + 257, kTarUstarMagic, 5)) {
        return Format::Tar;
    }

    return Format::Unknown;
}

/// 检测 ZIP 内部结构以区分 DOCX/XLSX/PPTX/EPUB/普通ZIP
Format detect_zip_content(const uint8_t* data, size_t size) noexcept {
    if (data == nullptr || size < 30) {
        return Format::Unknown;
    }

    // ZIP 本地文件头结构：
    // 0-3:   签名 (PK\x03\x04)
    // 4-5:   版本
    // 6-7:   标志
    // 8-9:   压缩方法
    // ...
    // 26-27: 文件名长度
    // 28-29: 扩展字段长度
    // 30+:   文件名

    // 获取第一个文件的文件名
    if (size < 30) {
        return Format::Unknown;
    }

    uint16_t filename_len = static_cast<uint16_t>(data[26]) |
                            (static_cast<uint16_t>(data[27]) << 8);
    uint16_t extra_len = static_cast<uint16_t>(data[28]) |
                         (static_cast<uint16_t>(data[29]) << 8);

    if (size < 30 + filename_len) {
        return Format::Unknown;
    }

    // 提取文件名
    std::string_view filename(reinterpret_cast<const char*>(data + 30), filename_len);

    // EPUB: 第一个文件是 "mimetype"
    if (filename == "mimetype") {
        // 检查 mimetype 内容
        size_t content_offset = 30 + filename_len + extra_len;
        if (size > content_offset + 20) {
            std::string_view content(reinterpret_cast<const char*>(data + content_offset), 20);
            if (content.find("application/epub") != std::string_view::npos) {
                return Format::EPUB;
            }
        }
        return Format::EPUB;  // 假设是 EPUB
    }

    // Office Open XML: [Content_Types].xml
    if (filename == "[Content_Types].xml") {
        // 需要进一步检查是 DOCX, XLSX 还是 PPTX
        // 简化：查找特定目录
        // 完整实现需要解析 ZIP 中央目录
        
        // 在数据中搜索特征字符串
        std::string_view data_view(reinterpret_cast<const char*>(data),
                                   std::min(size, static_cast<size_t>(4096)));
        
        if (data_view.find("word/") != std::string_view::npos) {
            return Format::DOCX;
        }
        if (data_view.find("xl/") != std::string_view::npos) {
            return Format::XLSX;
        }
        if (data_view.find("ppt/") != std::string_view::npos) {
            return Format::PPTX;
        }
        
        // 默认返回 DOCX
        return Format::DOCX;
    }

    // 其他以 _rels/ 或 docProps/ 开头的也可能是 Office 文档
    if (filename.substr(0, 6) == "_rels/" || filename.substr(0, 9) == "docProps/") {
        return Format::DOCX;  // 简化处理
    }

    return Format::Unknown;  // 普通 ZIP
}

}  // namespace detail
}  // namespace fileformat

