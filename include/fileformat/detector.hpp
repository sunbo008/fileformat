#ifndef FILEFORMAT_DETECTOR_HPP
#define FILEFORMAT_DETECTOR_HPP

#include <cstddef>
#include <cstdint>
#include <istream>
#include <string>
#include <utility>
#include <vector>

#include "fileformat/types.hpp"

namespace fileformat {

//==============================================================================
// 主检测 API
//==============================================================================

/// 检测文件格式（通过文件路径）
/// @param path 文件路径
/// @return 检测到的格式，无法识别时返回 Format::Unknown
/// @note 不抛异常，文件不存在或无法读取时返回 Format::Unknown
[[nodiscard]] Format detect(const std::string& path) noexcept;

/// 检测文件格式（通过内存缓冲区）
/// @param data 文件数据指针
/// @param size 数据大小
/// @return 检测到的格式
/// @note 不抛异常，空指针或零大小返回 Format::Unknown
[[nodiscard]] Format detect(const uint8_t* data, size_t size) noexcept;

/// 检测文件格式（通过输入流）
/// @param stream 输入流
/// @return 检测到的格式
/// @note 不抛异常，流位置会被重置到调用前的位置
[[nodiscard]] Format detect(std::istream& stream) noexcept;

//==============================================================================
// 安全检测 API（返回详细错误）
//==============================================================================

/// 安全检测文件格式（返回详细结果）
/// @param path 文件路径
/// @return DetectResult 包含格式和错误信息
[[nodiscard]] DetectResult detect_safe(const std::string& path) noexcept;

//==============================================================================
// 异常检测 API
//==============================================================================

/// 检测文件格式（可能抛出异常）
/// @param path 文件路径
/// @return 检测到的格式
/// @throws std::system_error 文件不存在或无法读取时抛出
[[nodiscard]] Format detect_or_throw(const std::string& path);

//==============================================================================
// 批量检测 API
//==============================================================================

/// 批量检测多个文件
/// @param paths 文件路径列表
/// @return 文件路径与格式的配对列表
[[nodiscard]] std::vector<std::pair<std::string, Format>> detect_batch(
    const std::vector<std::string>& paths);

//==============================================================================
// 格式信息查询
//==============================================================================

/// 获取格式详细信息
/// @param format 格式枚举值
/// @return 格式信息的常量引用
/// @note 始终返回有效引用，未知格式返回 Unknown 的信息
[[nodiscard]] const FormatInfo& get_info(Format format) noexcept;

/// 获取格式的类别名称字符串
/// @param category 类别枚举值
/// @return 类别名称
[[nodiscard]] std::string_view get_category_name(Category category) noexcept;

//==============================================================================
// 内部检测器（供高级用户使用）
//==============================================================================

namespace detail {

/// 检测图像格式
[[nodiscard]] Format detect_image(const uint8_t* data, size_t size) noexcept;

/// 检测文档格式
[[nodiscard]] Format detect_document(const uint8_t* data, size_t size) noexcept;

/// 检测压缩格式
[[nodiscard]] Format detect_archive(const uint8_t* data, size_t size) noexcept;

/// 检测电子书格式
[[nodiscard]] Format detect_ebook(const uint8_t* data, size_t size) noexcept;

/// 检测媒体格式
[[nodiscard]] Format detect_media(const uint8_t* data, size_t size) noexcept;

/// 检测可执行文件格式
[[nodiscard]] Format detect_executable(const uint8_t* data, size_t size) noexcept;

/// 检测 ZIP 内部结构（区分 DOCX/XLSX/PPTX/EPUB/普通ZIP）
[[nodiscard]] Format detect_zip_content(const uint8_t* data, size_t size) noexcept;

}  // namespace detail

}  // namespace fileformat

#endif  // FILEFORMAT_DETECTOR_HPP

