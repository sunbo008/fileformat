/// @file detect_file.cpp
/// @brief 单文件格式检测示例

#include <fileformat/fileformat.hpp>

#include <iostream>
#include <string>

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <file_path>\n";
        std::cerr << "Example: " << argv[0] << " document.pdf\n";
        return 1;
    }

    std::string path = argv[1];

    // 方式 1: 简单检测
    std::cout << "=== Simple Detection ===\n";
    auto format = fileformat::detect(path);
    auto& info = fileformat::get_info(format);

    std::cout << "File: " << path << "\n";
    std::cout << "Format: " << info.name << "\n";
    std::cout << "MIME Type: " << info.mime_type << "\n";
    std::cout << "Extension: " << info.extension << "\n";
    std::cout << "Category: " << fileformat::get_category_name(info.category) << "\n";

    // 方式 2: 安全检测（获取错误信息）
    std::cout << "\n=== Safe Detection ===\n";
    auto result = fileformat::detect_safe(path);

    if (result.is_valid()) {
        if (result.is_known()) {
            auto& safe_info = fileformat::get_info(result.format);
            std::cout << "Detected: " << safe_info.name << "\n";
        } else {
            std::cout << "Format: Unknown\n";
        }
    } else {
        std::cout << "Error: " << result.error.message() << "\n";
    }

    // 方式 3: 异常检测（演示 try-catch）
    std::cout << "\n=== Exception-based Detection ===\n";
    try {
        auto throw_format = fileformat::detect_or_throw(path);
        auto& throw_info = fileformat::get_info(throw_format);
        std::cout << "Detected: " << throw_info.name << "\n";
    } catch (const std::system_error& e) {
        std::cout << "Exception: " << e.what() << "\n";
    }

    return 0;
}

