#include "fileformat/detector.hpp"

#include <cstring>

namespace fileformat {
namespace detail {

namespace {

// Magic bytes 常量
constexpr uint8_t kPdfMagic[] = {0x25, 0x50, 0x44, 0x46};  // "%PDF"
constexpr uint8_t kOleMagic[] = {0xD0, 0xCF, 0x11, 0xE0, 0xA1, 0xB1, 0x1A, 0xE1};

/// 比较内存
inline bool mem_equal(const uint8_t* data, const uint8_t* pattern, size_t len) {
    return std::memcmp(data, pattern, len) == 0;
}

/// 检测 OLE Compound Document 类型（DOC, XLS, PPT）
Format detect_ole_type(const uint8_t* data, size_t size) {
    // OLE 文件的具体类型需要解析内部结构
    // 简化实现：返回 DOC（最常见的 OLE 文档类型）
    // 完整实现需要解析 FAT 和目录条目
    (void)data;
    (void)size;
    return Format::DOC;
}

}  // namespace

Format detect_document(const uint8_t* data, size_t size) noexcept {
    if (data == nullptr || size < 4) {
        return Format::Unknown;
    }

    // PDF: %PDF
    if (size >= 4 && mem_equal(data, kPdfMagic, 4)) {
        return Format::PDF;
    }

    // OLE Compound Document (DOC, XLS, PPT)
    if (size >= 8 && mem_equal(data, kOleMagic, 8)) {
        return detect_ole_type(data, size);
    }

    return Format::Unknown;
}

}  // namespace detail
}  // namespace fileformat

