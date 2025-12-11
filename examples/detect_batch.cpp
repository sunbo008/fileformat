/// @file detect_batch.cpp
/// @brief 批量文件格式检测示例

#include <fileformat/fileformat.hpp>

#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <file1> [file2] [file3] ...\n";
        std::cerr << "Example: " << argv[0] << " image.png doc.pdf archive.zip\n";
        return 1;
    }

    // 收集所有文件路径
    std::vector<std::string> paths;
    for (int i = 1; i < argc; ++i) {
        paths.emplace_back(argv[i]);
    }

    // 批量检测
    auto results = fileformat::detect_batch(paths);

    // 输出结果表格
    std::cout << std::left;
    std::cout << std::setw(40) << "File" << std::setw(12) << "Format" << std::setw(12) << "Category"
              << "MIME Type\n";
    std::cout << std::string(80, '-') << "\n";

    for (const auto& [path, format] : results) {
        auto& info = fileformat::get_info(format);
        std::cout << std::setw(40) << path << std::setw(12) << info.name << std::setw(12)
                  << fileformat::get_category_name(info.category) << info.mime_type << "\n";
    }

    // 统计
    std::cout << "\n=== Statistics ===\n";
    int known_count = 0;
    int unknown_count = 0;

    for (const auto& [path, format] : results) {
        if (format != fileformat::Format::Unknown) {
            ++known_count;
        } else {
            ++unknown_count;
        }
    }

    std::cout << "Total files: " << results.size() << "\n";
    std::cout << "Recognized: " << known_count << "\n";
    std::cout << "Unknown: " << unknown_count << "\n";

    return 0;
}

