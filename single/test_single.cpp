/**
 * @file test_single.cpp
 * @brief 单文件版本测试程序
 * 
 * 编译方法（无需 CMake）：
 *   Windows MSVC: cl /EHsc /std:c++17 test_single.cpp
 *   GCC/Clang:    g++ -std=c++17 test_single.cpp -o test_single
 * 
 * 使用方法：
 *   test_single                    # 运行内置测试
 *   test_single <file>             # 检测指定文件
 *   test_single <file1> <file2>... # 批量检测
 */

#include "fileformat_single.hpp"
#include <iostream>
#include <fstream>
#include <vector>
#include <cstring>

// 检测文件
void detect_file(const char* path) {
    std::ifstream file(path, std::ios::binary);
    if (!file) {
        std::cerr << path << ": Failed to open file" << std::endl;
        return;
    }

    // 读取文件头部（最多 4KB）
    std::vector<uint8_t> buffer(4096);
    file.read(reinterpret_cast<char*>(buffer.data()), buffer.size());
    size_t bytes_read = static_cast<size_t>(file.gcount());

    if (bytes_read == 0) {
        std::cout << path << ": Empty file" << std::endl;
        return;
    }

    // 检测格式（detect 直接返回 std::string）
    std::string fmt = fileformat::detect(buffer.data(), bytes_read);
    std::cout << path << ": " << fmt << std::endl;
}

// 内置测试
void run_builtin_tests() {
    std::cout << "=== FileFormat Built-in Tests ===" << std::endl;
    std::cout << std::endl;

    auto test = [](const char* name, const std::vector<uint8_t>& data, const char* expected) {
        std::string result = fileformat::detect(data.data(), data.size());
        bool passed = (result == expected);
        std::cout << (passed ? "[PASS]" : "[FAIL]") << " " << name;
        if (!passed) {
            std::cout << " (expected: " << expected << ", got: " << result << ")";
        }
        std::cout << std::endl;
    };

    // 图像格式
    test("PNG",  {0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A}, "PNG");
    test("JPEG", {0xFF, 0xD8, 0xFF, 0xE0}, "JPEG");
    test("BMP",  {0x42, 0x4D}, "BMP");
    test("GIF",  {0x47, 0x49, 0x46, 0x38, 0x39, 0x61}, "GIF");

    // 文档格式
    test("PDF",  {0x25, 0x50, 0x44, 0x46, 0x2D, 0x31, 0x2E, 0x34}, "PDF");
    test("DOC",  {0xD0, 0xCF, 0x11, 0xE0, 0xA1, 0xB1, 0x1A, 0xE1}, "DOC");

    // 压缩格式
    test("ZIP",  {0x50, 0x4B, 0x03, 0x04}, "ZIP");
    test("RAR",  {0x52, 0x61, 0x72, 0x21, 0x1A, 0x07}, "RAR");
    test("7Z",   {0x37, 0x7A, 0xBC, 0xAF, 0x27, 0x1C}, "7Z");
    test("GZIP", {0x1F, 0x8B}, "GZIP");

    // 媒体格式
    test("MP3",  {0x49, 0x44, 0x33}, "MP3");
    test("MP4",  {0x00, 0x00, 0x00, 0x18, 0x66, 0x74, 0x79, 0x70}, "MP4");

    // 可执行文件
    test("EXE",  {0x4D, 0x5A}, "EXE");
    test("ELF",  {0x7F, 0x45, 0x4C, 0x46}, "ELF");

    // 边界条件
    test("Empty", {}, "Unknown");
    test("Unknown", {0x12, 0x34, 0x56, 0x78}, "Unknown");

    std::cout << std::endl;
    std::cout << "=== Tests Complete ===" << std::endl;
}

void print_usage(const char* prog) {
    std::cout << "FileFormat Detector - Single File Version" << std::endl;
    std::cout << std::endl;
    std::cout << "Usage:" << std::endl;
    std::cout << "  " << prog << "                 Run built-in tests" << std::endl;
    std::cout << "  " << prog << " <file>          Detect file format" << std::endl;
    std::cout << "  " << prog << " <f1> <f2> ...   Detect multiple files" << std::endl;
    std::cout << "  " << prog << " --help          Show this help" << std::endl;
}

int main(int argc, char* argv[]) {
    // 无参数：运行内置测试
    if (argc == 1) {
        run_builtin_tests();
        return 0;
    }

    // 帮助
    if (argc == 2 && (std::strcmp(argv[1], "--help") == 0 || std::strcmp(argv[1], "-h") == 0)) {
        print_usage(argv[0]);
        return 0;
    }

    // 检测文件
    for (int i = 1; i < argc; ++i) {
        detect_file(argv[i]);
    }

    return 0;
}
