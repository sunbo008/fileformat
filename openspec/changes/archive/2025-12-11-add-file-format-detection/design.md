# Design: File Format Detection Library

## Context
构建一个跨平台（Windows/Linux/macOS）的 C++ 文件格式识别库，需要在性能、可维护性、可扩展性之间取得平衡。目标用户包括需要在应用中集成文件类型检测功能的 C++ 开发者。

## Goals / Non-Goals

### Goals
- 跨平台支持：Windows (MSVC, MinGW), Linux (GCC, Clang), macOS (Clang)
- 高性能：只读取必要的文件头部数据
- 零外部依赖：纯 C++ 标准库实现
- 易于集成：header-only 或静态/动态库
- 可扩展：易于添加新格式支持
- 完整测试：单元测试覆盖所有支持的格式

### Non-Goals
- 文件内容深度解析（如提取元数据）
- 文件格式转换
- 异步/多线程 API（用户可自行实现）

## Decisions

### 1. 项目结构

```
fileformat/
├── CMakeLists.txt              # 主 CMake 配置
├── cmake/
│   └── FileFormatConfig.cmake.in  # CMake 包配置
├── include/
│   └── fileformat/
│       ├── fileformat.hpp      # 主头文件
│       ├── detector.hpp        # 检测器接口
│       ├── formats.hpp         # 格式定义
│       └── types.hpp           # 类型定义
├── src/
│   ├── CMakeLists.txt
│   ├── detector.cpp
│   └── formats/
│       ├── image.cpp           # PNG, JPEG, BMP, GIF
│       ├── document.cpp        # PDF, DOC, DOCX
│       ├── ebook.cpp           # EPUB, MOBI, AZW3, FB2, DJVU
│       ├── archive.cpp         # ZIP, RAR, 7Z
│       └── media.cpp           # MP3, MP4, WAV
├── tests/
│   ├── CMakeLists.txt
│   ├── test_main.cpp
│   └── test_*.cpp
├── examples/
│   ├── CMakeLists.txt
│   └── detect_file.cpp
└── README.md
```

**理由**: 标准 CMake 项目结构，便于其他项目通过 `find_package()` 或 `add_subdirectory()` 集成。

### 2. 检测策略

采用分层检测策略：

1. **Magic Bytes 快速匹配**：读取文件前 16-32 字节，匹配已知签名
2. **结构验证**：对于某些格式（如 Office 文档），需要额外验证内部结构
3. **延迟读取**：只在需要时读取更多数据

```cpp
// 检测流程伪代码
FileFormat detect(const std::string& path) {
    auto header = read_header(path, 32);  // 只读前32字节
    
    // 快速匹配
    if (auto fmt = match_magic(header)) {
        return *fmt;
    }
    
    // 需要更多数据的格式
    if (is_compound_document(header)) {
        return detect_office_format(path);  // DOC, XLS, PPT
    }
    
    return FileFormat::Unknown;
}
```

**理由**: 最小化 I/O 操作，大多数格式只需读取几十字节。

### 3. 支持的文件格式

| 类别 | 格式 | Magic Bytes | 备注 |
|------|------|-------------|------|
| 图像 | PNG | `89 50 4E 47 0D 0A 1A 0A` | |
| | JPEG | `FF D8 FF` | |
| | BMP | `42 4D` | |
| | GIF | `47 49 46 38` | 87a/89a |
| | WebP | `52 49 46 46 xx xx xx xx 57 45 42 50` | RIFF container |
| 文档 | PDF | `25 50 44 46` | `%PDF` |
| | DOC | `D0 CF 11 E0 A1 B1 1A E1` | OLE compound |
| | DOCX | `50 4B 03 04` + internal check | ZIP + structure |
| | XLSX | `50 4B 03 04` + internal check | ZIP + structure |
| | PPTX | `50 4B 03 04` + internal check | ZIP + structure |
| 电子书 | EPUB | `50 4B 03 04` + mimetype check | ZIP + META-INF/container.xml |
| | MOBI | `42 4F 4F 4B 4D 4F 42 49` | BOOKMOBI header |
| | AZW3 | `42 4F 4F 4B 4D 4F 42 49` + KF8 | Kindle Format 8 |
| | FB2 | `3C 3F 78 6D 6C` + FictionBook | XML-based |
| | DJVU | `41 54 26 54 46 4F 52 4D` | AT&TFORM |
| 压缩 | ZIP | `50 4B 03 04` | |
| | RAR | `52 61 72 21 1A 07` | |
| | 7Z | `37 7A BC AF 27 1C` | |
| | GZIP | `1F 8B` | |
| 媒体 | MP3 | `FF FB` / `49 44 33` | Frame sync / ID3 |
| | MP4 | `xx xx xx xx 66 74 79 70` | ftyp box |
| | WAV | `52 49 46 46 xx xx xx xx 57 41 56 45` | RIFF container |
| 可执行 | EXE/DLL | `4D 5A` | MZ header |
| | ELF | `7F 45 4C 46` | |
| | Mach-O | `FE ED FA CE/CF` | |

### 4. API 设计

```cpp
namespace fileformat {

// 文件格式枚举
enum class Format {
    Unknown,
    // Images
    PNG, JPEG, BMP, GIF, WebP, TIFF,
    // Documents
    PDF, DOC, DOCX, XLS, XLSX, PPT, PPTX,
    // Ebooks
    EPUB, MOBI, AZW3, FB2, DJVU,
    // Archives
    ZIP, RAR, SevenZip, GZip, Tar,
    // Media
    MP3, MP4, WAV, AVI, MKV,
    // Executables
    EXE, ELF, MachO
};

// 格式信息
struct FormatInfo {
    Format format;
    std::string_view name;        // "PNG"
    std::string_view mime_type;   // "image/png"
    std::string_view extension;   // ".png"
    std::string_view category;    // "image"
};

// 主检测函数
Format detect(const std::string& path);
Format detect(const uint8_t* data, size_t size);
Format detect(std::istream& stream);

// 获取格式详细信息
const FormatInfo& get_info(Format format);

// 批量检测
std::vector<std::pair<std::string, Format>> 
detect_batch(const std::vector<std::string>& paths);

} // namespace fileformat
```

**理由**: 简洁的 API，支持多种输入方式（文件路径、内存缓冲区、流）。

### 5. 测试策略

- 使用 Google Test 或 Catch2 作为测试框架
- 每种格式至少 2 个测试文件（正常、边缘情况）
- 提供测试数据生成脚本或二进制测试文件
- 性能基准测试

### 6. CMake 配置

```cmake
cmake_minimum_required(VERSION 3.14)
project(fileformat VERSION 1.0.0 LANGUAGES CXX)

# C++ 标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 选项
option(FILEFORMAT_BUILD_TESTS "Build tests" ON)
option(FILEFORMAT_BUILD_EXAMPLES "Build examples" ON)
option(FILEFORMAT_BUILD_SHARED "Build shared library" OFF)

# 跨平台编译器设置
if(MSVC)
    add_compile_options(/W4 /WX)
else()
    add_compile_options(-Wall -Wextra -Wpedantic -Werror)
endif()
```

### 7. 代码健壮性保证

#### 7.1 错误处理策略

采用返回值 + 异常混合模式：

```cpp
// 方式1：返回 Unknown 表示无法识别（不抛异常）
Format detect(const std::string& path) noexcept;

// 方式2：提供可选的详细结果（包含错误信息）
struct DetectResult {
    Format format;
    std::error_code error;  // 文件不存在、权限不足等
    bool is_valid() const { return !error; }
};
DetectResult detect_safe(const std::string& path) noexcept;

// 方式3：需要明确错误时使用（可能抛异常）
Format detect_or_throw(const std::string& path);  // throws std::system_error
```

**设计原则**：
- 默认 API 不抛异常，返回 `Format::Unknown`
- 提供 `_safe` 后缀版本获取详细错误信息
- 提供 `_or_throw` 版本供需要异常语义的用户使用

#### 7.2 资源管理 (RAII)

```cpp
// 文件句柄自动管理
class FileReader {
    std::ifstream stream_;
public:
    explicit FileReader(const std::string& path);
    ~FileReader() = default;  // ifstream 自动关闭
    
    // 禁止拷贝，允许移动
    FileReader(const FileReader&) = delete;
    FileReader& operator=(const FileReader&) = delete;
    FileReader(FileReader&&) = default;
    FileReader& operator=(FileReader&&) = default;
};
```

**原则**：
- 所有资源使用 RAII 管理
- 禁止裸指针持有资源
- 使用智能指针管理动态内存

#### 7.2.1 内存管理强制约束

**禁止使用**：
```cpp
// ❌ 禁止：手动内存管理
new / new[]
delete / delete[]
malloc / calloc / realloc / free
aligned_alloc / posix_memalign
```

**必须使用**：
```cpp
// ✅ 正确：STL 容器自动管理内存
std::vector<uint8_t> buffer(size);
std::string path;
std::array<uint8_t, 32> header;

// ✅ 正确：智能指针（极少数需要动态分配时）
auto ptr = std::make_unique<LargeStruct>();
auto shared = std::make_shared<SharedResource>();

// ✅ 正确：栈上分配（优先）
std::array<uint8_t, kMaxHeaderSize> local_buffer;
```

**代码示例**：
```cpp
// ❌ 错误写法
class BadDetector {
    uint8_t* buffer_;
public:
    BadDetector(size_t size) : buffer_(new uint8_t[size]) {}
    ~BadDetector() { delete[] buffer_; }
};

// ✅ 正确写法
class GoodDetector {
    std::vector<uint8_t> buffer_;
public:
    explicit GoodDetector(size_t size) : buffer_(size) {}
    // 无需析构函数，vector 自动管理
};
```

**静态检查配置**（clang-tidy）：
```yaml
# .clang-tidy
Checks: >
  -*,
  cppcoreguidelines-no-malloc,
  cppcoreguidelines-owning-memory,
  modernize-make-unique,
  modernize-make-shared,
  modernize-avoid-c-arrays
```

#### 7.3 输入验证

```cpp
Format detect(const uint8_t* data, size_t size) noexcept {
    // 空指针检查
    if (data == nullptr || size == 0) {
        return Format::Unknown;
    }
    
    // 最小长度检查（至少需要2字节判断）
    if (size < kMinHeaderSize) {
        return Format::Unknown;
    }
    
    // 实际检测逻辑...
}
```

**验证点**：
- 空指针/空路径检查
- 缓冲区大小验证
- 文件存在性检查（detect 内部处理）
- 路径有效性检查（过长、非法字符）

#### 7.4 边界条件处理

| 边界条件 | 处理方式 |
|----------|----------|
| 空文件 (0 bytes) | 返回 `Format::Unknown` |
| 超小文件 (<2 bytes) | 返回 `Format::Unknown` |
| 超大文件 (>4GB) | 正常处理（只读头部） |
| 损坏的文件头 | 返回 `Format::Unknown`，不崩溃 |
| 权限不足 | 返回 `Unknown` 或设置 error_code |
| 符号链接 | 跟随链接读取目标文件 |
| 设备文件 | 返回 `Format::Unknown` |
| 网络路径 | 正常处理（由 OS 处理） |

#### 7.5 静态分析与工具链

```cmake
# 启用静态分析
option(FILEFORMAT_ENABLE_SANITIZERS "Enable sanitizers" OFF)
option(FILEFORMAT_ENABLE_CLANG_TIDY "Enable clang-tidy" OFF)

if(FILEFORMAT_ENABLE_SANITIZERS)
    add_compile_options(-fsanitize=address,undefined)
    add_link_options(-fsanitize=address,undefined)
endif()

if(FILEFORMAT_ENABLE_CLANG_TIDY)
    set(CMAKE_CXX_CLANG_TIDY "clang-tidy;-checks=*,-llvmlibc-*")
endif()
```

**工具使用**：
- **AddressSanitizer (ASan)** - 检测内存越界、use-after-free
- **UndefinedBehaviorSanitizer (UBSan)** - 检测未定义行为
- **clang-tidy** - 静态代码分析
- **Valgrind** - 内存泄漏检测（Linux）
- **MSVC /analyze** - Windows 静态分析

#### 7.6 防御性编程原则

```cpp
// 1. 使用 assert 检查内部不变量（仅 Debug）
assert(header_size <= kMaxHeaderSize && "Header size overflow");

// 2. 使用 static_assert 检查编译期约束
static_assert(sizeof(PngHeader) == 8, "PNG header size mismatch");

// 3. 枚举值边界检查
const FormatInfo& get_info(Format format) {
    auto index = static_cast<size_t>(format);
    if (index >= format_info_table.size()) {
        return format_info_table[0];  // Unknown
    }
    return format_info_table[index];
}

// 4. 整数溢出保护
bool safe_add(size_t a, size_t b, size_t& result) {
    if (a > std::numeric_limits<size_t>::max() - b) {
        return false;
    }
    result = a + b;
    return true;
}
```

#### 7.7 线程安全

```cpp
// 所有公共 API 保证线程安全（无状态设计）
// detect() 函数无全局状态，可并发调用

namespace fileformat {
    // 线程安全：无共享状态
    Format detect(const std::string& path) noexcept;
    
    // 线程安全：返回 const 引用到静态数据
    const FormatInfo& get_info(Format format) noexcept;
}
```

**保证**：
- 所有检测函数无全局可变状态
- 格式信息表使用 `constexpr` 或静态初始化
- 无需用户端同步即可并发调用

## Risks / Trade-offs

| 风险 | 缓解措施 |
|------|----------|
| Office 文档检测复杂 | 对于 OOXML 格式只检测 ZIP 内部结构，不完整解析 |
| EPUB 与 ZIP/DOCX 冲突 | EPUB 通过 mimetype 文件内容区分；检测优先级：EPUB > DOCX > ZIP |
| MOBI/AZW3 区分困难 | 通过 EXTH 记录检测 KF8 标记区分 AZW3 |
| 某些格式 magic bytes 冲突 | 使用更长签名 + 结构验证组合判断 |
| 大文件性能 | 只读取必要字节数，使用内存映射可选优化 |
| 平台特定代码 | 使用预处理宏隔离，最小化平台差异 |

## Open Questions

1. 是否需要支持 header-only 模式？
2. 是否需要提供 C API 以支持其他语言绑定？
3. 测试文件如何管理？（Git LFS / 运行时生成）

