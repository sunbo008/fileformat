# FileFormat 快速入门指南

本指南帮助你在 5 分钟内开始使用 FileFormat 库。

---

## 第一步：获取源码

```bash
git clone https://github.com/your-username/fileformat.git
cd fileformat
```

---

## 第二步：编译库

### Windows (Visual Studio)

```bash
cmake -B build
cmake --build build --config Release
```

### Linux / macOS

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

---

## 第三步：运行示例

```bash
# Windows
build\examples\Release\detect_file.exe README.md

# Linux / macOS
./build/examples/detect_file README.md
```

---

## 第四步：在你的项目中使用

### 方法 A：复制到项目中

1. 将 `include/` 和 `src/` 目录复制到你的项目
2. 在 CMakeLists.txt 中添加：

```cmake
add_subdirectory(fileformat)
target_link_libraries(你的目标 PRIVATE fileformat::fileformat)
```

### 方法 B：作为子模块

```bash
git submodule add https://github.com/your-username/fileformat.git external/fileformat
```

```cmake
add_subdirectory(external/fileformat)
target_link_libraries(你的目标 PRIVATE fileformat::fileformat)
```

---

## 第五步：编写代码

### 最简单的例子

```cpp
#include <fileformat/fileformat.hpp>
#include <iostream>

int main() {
    auto format = fileformat::detect("test.pdf");
    std::cout << fileformat::get_info(format).name << std::endl;
    return 0;
}
```

### 带错误处理的例子

```cpp
#include <fileformat/fileformat.hpp>
#include <iostream>

int main() {
    auto result = fileformat::detect_safe("test.pdf");
    
    if (!result.is_valid()) {
        std::cerr << "错误: " << result.error.message() << std::endl;
        return 1;
    }
    
    if (!result.is_known()) {
        std::cout << "未知格式" << std::endl;
        return 0;
    }
    
    auto& info = fileformat::get_info(result.format);
    std::cout << "格式: " << info.name << std::endl;
    std::cout << "MIME: " << info.mime_type << std::endl;
    
    return 0;
}
```

---

## 常用 API 速查

| 函数 | 用途 | 示例 |
|------|------|------|
| `detect(path)` | 检测文件 | `detect("file.pdf")` |
| `detect(data, size)` | 检测内存 | `detect(buf, len)` |
| `detect_safe(path)` | 带错误信息 | `detect_safe("file.pdf")` |
| `get_info(format)` | 获取格式信息 | `get_info(Format::PNG)` |
| `detect_batch(paths)` | 批量检测 | `detect_batch(files)` |

---

## 支持的格式

- **图像**: PNG, JPEG, BMP, GIF, WebP, TIFF
- **文档**: PDF, DOC, DOCX, XLS, XLSX, PPT, PPTX
- **电子书**: EPUB, MOBI, AZW3, FB2, DJVU
- **压缩**: ZIP, RAR, 7Z, GZIP, TAR
- **媒体**: MP3, MP4, WAV, AVI, MKV
- **可执行**: EXE, ELF, Mach-O

---

## 下一步

- 阅读 [README.md](../README.md) 了解完整功能
- 阅读 [API.md](API.md) 了解详细 API
- 查看 `examples/` 目录的示例代码

---

## 遇到问题？

1. 确保使用 C++17 或更高版本
2. 确保 CMake 版本 >= 3.14
3. 查看 [FAQ](../README.md#faq) 部分
4. 提交 [Issue](https://github.com/your-username/fileformat/issues)

