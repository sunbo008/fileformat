#ifndef FILEFORMAT_FILEFORMAT_HPP
#define FILEFORMAT_FILEFORMAT_HPP

/// @file fileformat.hpp
/// @brief FileFormat 库主头文件
///
/// 这是一个高性能、跨平台的文件格式检测库。
/// 通过分析文件内部数据（magic bytes）识别文件类型，不依赖文件扩展名。
///
/// @example
/// @code
/// #include <fileformat/fileformat.hpp>
/// #include <iostream>
///
/// int main() {
///     // 检测单个文件
///     auto format = fileformat::detect("document.pdf");
///     auto info = fileformat::get_info(format);
///     std::cout << "Format: " << info.name << std::endl;
///     std::cout << "MIME: " << info.mime_type << std::endl;
///
///     // 安全检测（获取错误信息）
///     auto result = fileformat::detect_safe("unknown.bin");
///     if (result.is_valid()) {
///         std::cout << "Detected: " << fileformat::get_info(result.format).name;
///     } else {
///         std::cout << "Error: " << result.error.message();
///     }
///
///     return 0;
/// }
/// @endcode

#include "fileformat/detector.hpp"
#include "fileformat/types.hpp"

/// @namespace fileformat
/// @brief FileFormat 库命名空间

#endif  // FILEFORMAT_FILEFORMAT_HPP

