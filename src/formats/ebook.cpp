#include "fileformat/detector.hpp"

#include <cstring>

namespace fileformat {
namespace detail {

namespace {

// Magic bytes 常量
constexpr uint8_t kMobiMagic[] = {0x42, 0x4F, 0x4F, 0x4B, 0x4D, 0x4F, 0x42, 0x49};  // "BOOKMOBI"
constexpr uint8_t kDjvuMagic[] = {0x41, 0x54, 0x26, 0x54, 0x46, 0x4F, 0x52, 0x4D};  // "AT&TFORM"
constexpr uint8_t kXmlMagic[] = {0x3C, 0x3F, 0x78, 0x6D, 0x6C};  // "<?xml"

/// 比较内存
inline bool mem_equal(const uint8_t* data, const uint8_t* pattern, size_t len) {
    return std::memcmp(data, pattern, len) == 0;
}

/// 检查 FB2 格式（XML 中包含 FictionBook 根元素）
bool is_fb2(const uint8_t* data, size_t size) {
    // 简单搜索 "FictionBook" 字符串
    std::string_view view(reinterpret_cast<const char*>(data),
                          std::min(size, static_cast<size_t>(1024)));
    return view.find("FictionBook") != std::string_view::npos;
}

/// 检查 MOBI 是否为 AZW3/KF8 格式
bool is_azw3(const uint8_t* data, size_t size) {
    // AZW3 在 EXTH 头中包含特定标记
    // 简化实现：检查是否有 KF8 相关标记
    if (size < 132) {
        return false;
    }
    
    // PDB header 中的 type 和 creator
    // 位置 60-67: type (4 bytes) + creator (4 bytes)
    // MOBI: type='BOOK', creator='MOBI'
    // 检查 EXTH 标志（位于 PalmDOC header 之后）
    // 这是简化检测，真实实现需要完整解析 MOBI 结构
    
    std::string_view view(reinterpret_cast<const char*>(data), size);
    // KF8 标记通常在 EXTH 记录中
    if (view.find("KF8") != std::string_view::npos) {
        return true;
    }
    
    return false;
}

}  // namespace

Format detect_ebook(const uint8_t* data, size_t size) noexcept {
    if (data == nullptr || size < 8) {
        return Format::Unknown;
    }

    // MOBI/AZW3: BOOKMOBI at offset 60 (in PDB header)
    if (size >= 68) {
        if (mem_equal(data + 60, kMobiMagic, 8)) {
            // 区分 MOBI 和 AZW3
            if (is_azw3(data, size)) {
                return Format::AZW3;
            }
            return Format::MOBI;
        }
    }

    // DJVU: AT&TFORM
    if (size >= 8 && mem_equal(data, kDjvuMagic, 8)) {
        return Format::DJVU;
    }

    // FB2: XML with FictionBook
    if (size >= 5 && mem_equal(data, kXmlMagic, 5)) {
        if (is_fb2(data, size)) {
            return Format::FB2;
        }
    }

    // EPUB 检测在 archive.cpp 的 detect_zip_content 中处理

    return Format::Unknown;
}

}  // namespace detail
}  // namespace fileformat

