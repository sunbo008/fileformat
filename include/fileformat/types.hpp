#ifndef FILEFORMAT_TYPES_HPP
#define FILEFORMAT_TYPES_HPP

#include <array>
#include <cstddef>
#include <cstdint>
#include <string_view>
#include <system_error>

namespace fileformat {

/// 支持的文件格式枚举
enum class Format {
    Unknown = 0,

    // 图像格式
    PNG,
    JPEG,
    BMP,
    GIF,
    WebP,
    TIFF,

    // 文档格式
    PDF,
    DOC,
    DOCX,
    XLS,
    XLSX,
    PPT,
    PPTX,

    // 电子书格式
    EPUB,
    MOBI,
    AZW3,
    FB2,
    DJVU,

    // 压缩格式
    ZIP,
    RAR,
    SevenZip,
    GZip,
    Tar,

    // 媒体格式
    MP3,
    MP4,
    WAV,
    AVI,
    MKV,

    // 可执行文件格式
    EXE,
    ELF,
    MachO,

    // 格式数量（用于数组大小）
    COUNT_
};

/// 格式类别
enum class Category {
    Unknown,
    Image,
    Document,
    Ebook,
    Archive,
    Media,
    Executable
};

/// 格式详细信息
struct FormatInfo {
    Format format;
    std::string_view name;       // 如 "PNG"
    std::string_view mime_type;  // 如 "image/png"
    std::string_view extension;  // 如 ".png"
    Category category;
};

/// 检测结果（包含错误信息）
struct DetectResult {
    Format format = Format::Unknown;
    std::error_code error;

    /// 检测是否成功（无错误）
    [[nodiscard]] bool is_valid() const noexcept { return !error; }

    /// 是否识别出格式
    [[nodiscard]] bool is_known() const noexcept { return format != Format::Unknown; }

    /// 隐式转换为 Format
    operator Format() const noexcept { return format; }
};

/// Magic bytes 签名
struct MagicSignature {
    std::array<uint8_t, 16> bytes;  // 签名字节
    std::array<uint8_t, 16> mask;   // 掩码（0xFF 表示必须匹配，0x00 表示忽略）
    size_t length;                  // 有效长度
    size_t offset;                  // 在文件中的偏移
    Format format;                  // 对应格式
};

// 常量
constexpr size_t kMaxHeaderSize = 4096;   // 最大读取头部大小
constexpr size_t kMinHeaderSize = 2;      // 最小有效头部大小
constexpr size_t kDefaultHeaderSize = 64; // 默认读取大小

}  // namespace fileformat

#endif  // FILEFORMAT_TYPES_HPP

