#include "fileformat/detector.hpp"

#include <algorithm>
#include <array>
#include <cerrno>
#include <cstring>
#include <fstream>
#include <system_error>

namespace fileformat {

//==============================================================================
// 格式信息表
//==============================================================================

namespace {

// 格式信息静态表
constexpr std::array<FormatInfo, static_cast<size_t>(Format::COUNT_)> kFormatInfoTable = {{
    // Unknown
    {Format::Unknown, "Unknown", "application/octet-stream", "", Category::Unknown},

    // 图像格式
    {Format::PNG, "PNG", "image/png", ".png", Category::Image},
    {Format::JPEG, "JPEG", "image/jpeg", ".jpg", Category::Image},
    {Format::BMP, "BMP", "image/bmp", ".bmp", Category::Image},
    {Format::GIF, "GIF", "image/gif", ".gif", Category::Image},
    {Format::WebP, "WebP", "image/webp", ".webp", Category::Image},
    {Format::TIFF, "TIFF", "image/tiff", ".tiff", Category::Image},

    // 文档格式
    {Format::PDF, "PDF", "application/pdf", ".pdf", Category::Document},
    {Format::DOC, "DOC", "application/msword", ".doc", Category::Document},
    {Format::DOCX, "DOCX", "application/vnd.openxmlformats-officedocument.wordprocessingml.document",
     ".docx", Category::Document},
    {Format::XLS, "XLS", "application/vnd.ms-excel", ".xls", Category::Document},
    {Format::XLSX, "XLSX", "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet",
     ".xlsx", Category::Document},
    {Format::PPT, "PPT", "application/vnd.ms-powerpoint", ".ppt", Category::Document},
    {Format::PPTX, "PPTX",
     "application/vnd.openxmlformats-officedocument.presentationml.presentation", ".pptx",
     Category::Document},

    // 电子书格式
    {Format::EPUB, "EPUB", "application/epub+zip", ".epub", Category::Ebook},
    {Format::MOBI, "MOBI", "application/x-mobipocket-ebook", ".mobi", Category::Ebook},
    {Format::AZW3, "AZW3", "application/vnd.amazon.ebook", ".azw3", Category::Ebook},
    {Format::FB2, "FB2", "application/x-fictionbook+xml", ".fb2", Category::Ebook},
    {Format::DJVU, "DJVU", "image/vnd.djvu", ".djvu", Category::Ebook},

    // 压缩格式
    {Format::ZIP, "ZIP", "application/zip", ".zip", Category::Archive},
    {Format::RAR, "RAR", "application/vnd.rar", ".rar", Category::Archive},
    {Format::SevenZip, "7Z", "application/x-7z-compressed", ".7z", Category::Archive},
    {Format::GZip, "GZIP", "application/gzip", ".gz", Category::Archive},
    {Format::Tar, "TAR", "application/x-tar", ".tar", Category::Archive},

    // 媒体格式
    {Format::MP3, "MP3", "audio/mpeg", ".mp3", Category::Media},
    {Format::MP4, "MP4", "video/mp4", ".mp4", Category::Media},
    {Format::WAV, "WAV", "audio/wav", ".wav", Category::Media},
    {Format::AVI, "AVI", "video/x-msvideo", ".avi", Category::Media},
    {Format::MKV, "MKV", "video/x-matroska", ".mkv", Category::Media},

    // 可执行文件格式
    {Format::EXE, "EXE", "application/vnd.microsoft.portable-executable", ".exe",
     Category::Executable},
    {Format::ELF, "ELF", "application/x-executable", "", Category::Executable},
    {Format::MachO, "Mach-O", "application/x-mach-binary", "", Category::Executable},
}};

// 类别名称表
constexpr std::array<std::string_view, 7> kCategoryNames = {
    "unknown", "image", "document", "ebook", "archive", "media", "executable"};

}  // namespace

//==============================================================================
// 格式信息查询实现
//==============================================================================

const FormatInfo& get_info(Format format) noexcept {
    auto index = static_cast<size_t>(format);
    if (index >= kFormatInfoTable.size()) {
        return kFormatInfoTable[0];  // Unknown
    }
    return kFormatInfoTable[index];
}

std::string_view get_category_name(Category category) noexcept {
    auto index = static_cast<size_t>(category);
    if (index >= kCategoryNames.size()) {
        return kCategoryNames[0];  // unknown
    }
    return kCategoryNames[index];
}

//==============================================================================
// 文件读取工具
//==============================================================================

namespace {

/// 读取文件头部数据
/// @return 成功返回读取的字节数据，失败返回空 vector 和错误码
std::pair<std::vector<uint8_t>, std::error_code> read_file_header(const std::string& path,
                                                                   size_t max_size) {
    std::vector<uint8_t> buffer;

    if (path.empty()) {
        return {buffer, std::make_error_code(std::errc::invalid_argument)};
    }

    std::ifstream file(path, std::ios::binary);
    if (!file) {
        return {buffer, std::make_error_code(std::errc::no_such_file_or_directory)};
    }

    // 获取文件大小
    file.seekg(0, std::ios::end);
    auto file_size = static_cast<size_t>(file.tellg());
    file.seekg(0, std::ios::beg);

    if (file_size == 0) {
        return {buffer, std::error_code{}};  // 空文件，无错误
    }

    // 读取头部
    size_t read_size = std::min(file_size, max_size);
    buffer.resize(read_size);

    file.read(reinterpret_cast<char*>(buffer.data()), static_cast<std::streamsize>(read_size));

    if (!file) {
        buffer.clear();
        return {buffer, std::make_error_code(std::errc::io_error)};
    }

    return {buffer, std::error_code{}};
}

}  // namespace

//==============================================================================
// 核心检测逻辑
//==============================================================================

Format detect(const uint8_t* data, size_t size) noexcept {
    // 输入验证
    if (data == nullptr || size < kMinHeaderSize) {
        return Format::Unknown;
    }

    // 按优先级尝试各类检测器
    // 1. 图像格式（最常见，快速匹配）
    if (auto fmt = detail::detect_image(data, size); fmt != Format::Unknown) {
        return fmt;
    }

    // 2. 压缩格式和基于 ZIP 的格式
    if (auto fmt = detail::detect_archive(data, size); fmt != Format::Unknown) {
        // ZIP 格式需要进一步检查内部结构
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

Format detect(const std::string& path) noexcept {
    auto [buffer, error] = read_file_header(path, kMaxHeaderSize);
    if (error || buffer.empty()) {
        return Format::Unknown;
    }
    return detect(buffer.data(), buffer.size());
}

Format detect(std::istream& stream) noexcept {
    if (!stream) {
        return Format::Unknown;
    }

    // 保存当前位置
    auto original_pos = stream.tellg();

    // 读取头部
    std::vector<uint8_t> buffer(kMaxHeaderSize);
    stream.read(reinterpret_cast<char*>(buffer.data()), static_cast<std::streamsize>(buffer.size()));
    auto bytes_read = static_cast<size_t>(stream.gcount());

    // 恢复位置
    stream.clear();
    stream.seekg(original_pos);

    if (bytes_read < kMinHeaderSize) {
        return Format::Unknown;
    }

    return detect(buffer.data(), bytes_read);
}

//==============================================================================
// 安全检测 API
//==============================================================================

DetectResult detect_safe(const std::string& path) noexcept {
    DetectResult result;

    auto [buffer, error] = read_file_header(path, kMaxHeaderSize);
    result.error = error;

    if (error) {
        return result;
    }

    if (buffer.empty()) {
        result.format = Format::Unknown;
        return result;
    }

    result.format = detect(buffer.data(), buffer.size());
    return result;
}

//==============================================================================
// 异常检测 API
//==============================================================================

Format detect_or_throw(const std::string& path) {
    auto [buffer, error] = read_file_header(path, kMaxHeaderSize);
    if (error) {
        throw std::system_error(error, "Failed to read file: " + path);
    }

    if (buffer.empty()) {
        return Format::Unknown;
    }

    return detect(buffer.data(), buffer.size());
}

//==============================================================================
// 批量检测
//==============================================================================

std::vector<std::pair<std::string, Format>> detect_batch(const std::vector<std::string>& paths) {
    std::vector<std::pair<std::string, Format>> results;
    results.reserve(paths.size());

    for (const auto& path : paths) {
        results.emplace_back(path, detect(path));
    }

    return results;
}

}  // namespace fileformat

