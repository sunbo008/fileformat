# FileFormat API 详细文档

本文档提供 FileFormat 库的完整 API 参考。

---

## 目录

1. [命名空间](#命名空间)
2. [枚举类型](#枚举类型)
3. [结构体](#结构体)
4. [常量](#常量)
5. [检测函数](#检测函数)
6. [信息查询函数](#信息查询函数)
7. [内部函数](#内部函数)
8. [错误码](#错误码)

---

## 命名空间

所有公共 API 都在 `fileformat` 命名空间下：

```cpp
namespace fileformat {
    // 所有类型和函数
}
```

内部实现细节在 `fileformat::detail` 命名空间下：

```cpp
namespace fileformat::detail {
    // 内部实现，不建议直接使用
}
```

---

## 枚举类型

### `Format` - 文件格式枚举

```cpp
enum class Format {
    Unknown = 0,    // 未知格式
    
    // === 图像格式 ===
    PNG,            // PNG 图像
    JPEG,           // JPEG 图像
    BMP,            // BMP 位图
    GIF,            // GIF 动画/图像
    WebP,           // WebP 图像
    TIFF,           // TIFF 图像
    
    // === 文档格式 ===
    PDF,            // PDF 文档
    DOC,            // Word 97-2003 文档
    DOCX,           // Word 2007+ 文档
    XLS,            // Excel 97-2003 工作簿
    XLSX,           // Excel 2007+ 工作簿
    PPT,            // PowerPoint 97-2003 演示文稿
    PPTX,           // PowerPoint 2007+ 演示文稿
    
    // === 电子书格式 ===
    EPUB,           // EPUB 电子书
    MOBI,           // Mobipocket 电子书
    AZW3,           // Kindle Format 8
    FB2,            // FictionBook 2.0
    DJVU,           // DjVu 文档
    
    // === 压缩格式 ===
    ZIP,            // ZIP 压缩档案
    RAR,            // RAR 压缩档案
    SevenZip,       // 7-Zip 压缩档案
    GZip,           // GZIP 压缩
    Tar,            // TAR 档案
    
    // === 媒体格式 ===
    MP3,            // MP3 音频
    MP4,            // MP4 视频/音频
    WAV,            // WAV 音频
    AVI,            // AVI 视频
    MKV,            // Matroska 视频
    
    // === 可执行文件 ===
    EXE,            // Windows PE 可执行文件
    ELF,            // Linux ELF 可执行文件
    MachO,          // macOS Mach-O 可执行文件
    
    COUNT_          // 内部使用，表示枚举数量
};
```

**使用示例：**

```cpp
fileformat::Format fmt = fileformat::Format::PNG;

// 比较
if (fmt == fileformat::Format::PNG) {
    // ...
}

// 转换为整数
int value = static_cast<int>(fmt);

// 遍历所有格式
for (int i = 0; i < static_cast<int>(fileformat::Format::COUNT_); ++i) {
    auto format = static_cast<fileformat::Format>(i);
    auto& info = fileformat::get_info(format);
    std::cout << info.name << std::endl;
}
```

---

### `Category` - 格式类别枚举

```cpp
enum class Category {
    Unknown,        // 未知类别
    Image,          // 图像
    Document,       // 文档
    Ebook,          // 电子书
    Archive,        // 压缩档案
    Media,          // 媒体（音频/视频）
    Executable      // 可执行文件
};
```

**使用示例：**

```cpp
auto& info = fileformat::get_info(fileformat::Format::PNG);

switch (info.category) {
    case fileformat::Category::Image:
        std::cout << "这是图像文件" << std::endl;
        break;
    case fileformat::Category::Document:
        std::cout << "这是文档文件" << std::endl;
        break;
    // ...
}
```

---

## 结构体

### `FormatInfo` - 格式详细信息

```cpp
struct FormatInfo {
    Format format;              // 格式枚举值
    std::string_view name;      // 格式名称（如 "PNG"）
    std::string_view mime_type; // MIME 类型（如 "image/png"）
    std::string_view extension; // 文件扩展名（如 ".png"）
    Category category;          // 格式类别
};
```

**成员说明：**

| 成员 | 类型 | 说明 |
|------|------|------|
| `format` | `Format` | 对应的格式枚举值 |
| `name` | `std::string_view` | 人类可读的格式名称 |
| `mime_type` | `std::string_view` | IANA 注册的 MIME 类型 |
| `extension` | `std::string_view` | 标准文件扩展名（带点号）|
| `category` | `Category` | 格式所属的类别 |

**使用示例：**

```cpp
auto& info = fileformat::get_info(fileformat::Format::PDF);

std::cout << "格式名称: " << info.name << std::endl;        // "PDF"
std::cout << "MIME 类型: " << info.mime_type << std::endl;  // "application/pdf"
std::cout << "扩展名: " << info.extension << std::endl;     // ".pdf"

// 用于 HTTP Content-Type
std::cout << "Content-Type: " << info.mime_type << std::endl;

// 用于下载文件名
std::cout << "filename=document" << info.extension << std::endl;
```

---

### `DetectResult` - 检测结果

```cpp
struct DetectResult {
    Format format = Format::Unknown;  // 检测到的格式
    std::error_code error;            // 错误码（如果有）
    
    // === 成员函数 ===
    
    /// 检测是否成功（无错误发生）
    [[nodiscard]] bool is_valid() const noexcept;
    
    /// 是否识别出已知格式（非 Unknown）
    [[nodiscard]] bool is_known() const noexcept;
    
    /// 隐式转换为 Format
    operator Format() const noexcept;
};
```

**成员函数说明：**

#### `is_valid()`

```cpp
[[nodiscard]] bool is_valid() const noexcept;
```

- **返回值**：`true` 如果没有错误发生，`false` 如果有错误
- **说明**：即使返回 `true`，格式也可能是 `Unknown`

#### `is_known()`

```cpp
[[nodiscard]] bool is_known() const noexcept;
```

- **返回值**：`true` 如果识别出已知格式，`false` 如果是 `Unknown`
- **说明**：不检查是否有错误

#### `operator Format()`

```cpp
operator Format() const noexcept;
```

- **说明**：允许将 `DetectResult` 隐式转换为 `Format`

**使用示例：**

```cpp
auto result = fileformat::detect_safe("file.bin");

// 完整检查
if (result.is_valid()) {
    if (result.is_known()) {
        std::cout << "检测成功: " << fileformat::get_info(result.format).name;
    } else {
        std::cout << "未知格式";
    }
} else {
    std::cout << "错误: " << result.error.message();
}

// 简化使用（隐式转换）
fileformat::Format fmt = result;

// 直接比较
if (result == fileformat::Format::PNG) {
    // ...
}
```

---

### `MagicSignature` - Magic Bytes 签名（内部使用）

```cpp
struct MagicSignature {
    std::array<uint8_t, 16> bytes;  // 签名字节
    std::array<uint8_t, 16> mask;   // 掩码
    size_t length;                  // 有效长度
    size_t offset;                  // 在文件中的偏移
    Format format;                  // 对应格式
};
```

**说明**：此结构体主要供内部使用，用于定义格式的 magic bytes 签名。

---

## 常量

```cpp
namespace fileformat {
    constexpr size_t kMaxHeaderSize = 4096;   // 最大读取头部大小
    constexpr size_t kMinHeaderSize = 2;      // 最小有效头部大小
    constexpr size_t kDefaultHeaderSize = 64; // 默认读取大小
}
```

| 常量 | 值 | 说明 |
|------|-----|------|
| `kMaxHeaderSize` | 4096 | 读取文件头部的最大字节数 |
| `kMinHeaderSize` | 2 | 能够检测的最小数据量 |
| `kDefaultHeaderSize` | 64 | 默认读取的头部大小 |

---

## 检测函数

### `detect()` - 基本检测

#### 重载 1：文件路径

```cpp
[[nodiscard]] Format detect(const std::string& path) noexcept;
```

**参数：**
- `path` - 要检测的文件路径

**返回值：**
- 检测到的格式，如果无法检测则返回 `Format::Unknown`

**异常：**
- 不抛出异常（`noexcept`）

**说明：**
- 文件不存在、无权限、读取错误等情况都返回 `Format::Unknown`
- 自动处理路径编码（支持 UTF-8）

**示例：**

```cpp
auto format = fileformat::detect("C:/Users/test/document.pdf");
auto format2 = fileformat::detect("/home/user/image.png");
auto format3 = fileformat::detect("相对路径/文件.zip");
```

---

#### 重载 2：内存缓冲区

```cpp
[[nodiscard]] Format detect(const uint8_t* data, size_t size) noexcept;
```

**参数：**
- `data` - 指向数据的指针
- `size` - 数据大小（字节）

**返回值：**
- 检测到的格式，如果无法检测则返回 `Format::Unknown`

**异常：**
- 不抛出异常（`noexcept`）

**说明：**
- `data` 为 `nullptr` 或 `size` < 2 时返回 `Format::Unknown`
- 不需要传入完整文件，只需头部数据即可

**示例：**

```cpp
// 从 vector 检测
std::vector<uint8_t> data = {0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A};
auto format = fileformat::detect(data.data(), data.size());

// 从数组检测
uint8_t arr[] = {0xFF, 0xD8, 0xFF, 0xE0};
auto format2 = fileformat::detect(arr, sizeof(arr));

// 从 string 检测
std::string str = "\x89PNG\r\n\x1a\n";
auto format3 = fileformat::detect(
    reinterpret_cast<const uint8_t*>(str.data()), 
    str.size()
);
```

---

#### 重载 3：输入流

```cpp
[[nodiscard]] Format detect(std::istream& stream) noexcept;
```

**参数：**
- `stream` - 输入流引用

**返回值：**
- 检测到的格式，如果无法检测则返回 `Format::Unknown`

**异常：**
- 不抛出异常（`noexcept`）

**说明：**
- 流位置会被自动恢复到调用前的位置
- 流的错误状态会被清除后恢复
- 支持任何派生自 `std::istream` 的流

**示例：**

```cpp
// 文件流
std::ifstream file("image.png", std::ios::binary);
auto format = fileformat::detect(file);
// file 的位置已恢复，可以继续读取

// 字符串流
std::istringstream iss("\x89PNG\r\n\x1a\n...");
auto format2 = fileformat::detect(iss);

// 自定义流
MyNetworkStream stream;
auto format3 = fileformat::detect(stream);
```

---

### `detect_safe()` - 安全检测

```cpp
[[nodiscard]] DetectResult detect_safe(const std::string& path) noexcept;
```

**参数：**
- `path` - 要检测的文件路径

**返回值：**
- `DetectResult` 结构体，包含格式和错误信息

**异常：**
- 不抛出异常（`noexcept`）

**说明：**
- 与 `detect()` 不同，此函数提供详细的错误信息
- 错误码使用标准 `std::errc` 枚举

**常见错误码：**

| 错误码 | 说明 |
|--------|------|
| `std::errc::no_such_file_or_directory` | 文件不存在 |
| `std::errc::permission_denied` | 权限不足 |
| `std::errc::invalid_argument` | 无效参数（如空路径）|
| `std::errc::io_error` | I/O 错误 |

**示例：**

```cpp
auto result = fileformat::detect_safe("file.bin");

if (!result.is_valid()) {
    // 处理错误
    if (result.error == std::errc::no_such_file_or_directory) {
        std::cerr << "文件不存在: file.bin" << std::endl;
    } else if (result.error == std::errc::permission_denied) {
        std::cerr << "权限不足" << std::endl;
    } else {
        std::cerr << "错误: " << result.error.message() << std::endl;
    }
    return;
}

if (!result.is_known()) {
    std::cout << "无法识别的文件格式" << std::endl;
    return;
}

std::cout << "检测到: " << fileformat::get_info(result.format).name << std::endl;
```

---

### `detect_or_throw()` - 异常检测

```cpp
[[nodiscard]] Format detect_or_throw(const std::string& path);
```

**参数：**
- `path` - 要检测的文件路径

**返回值：**
- 检测到的格式（可能是 `Format::Unknown`）

**异常：**
- `std::system_error` - 当文件无法读取时抛出

**说明：**
- 只有 I/O 错误会抛出异常
- 无法识别的格式不会抛出异常，而是返回 `Format::Unknown`

**示例：**

```cpp
try {
    auto format = fileformat::detect_or_throw("file.bin");
    
    if (format == fileformat::Format::Unknown) {
        std::cout << "未知格式" << std::endl;
    } else {
        std::cout << "格式: " << fileformat::get_info(format).name << std::endl;
    }
    
} catch (const std::system_error& e) {
    std::cerr << "检测失败: " << e.what() << std::endl;
    std::cerr << "错误码: " << e.code().value() << std::endl;
    std::cerr << "错误类别: " << e.code().category().name() << std::endl;
}
```

---

### `detect_batch()` - 批量检测

```cpp
[[nodiscard]] std::vector<std::pair<std::string, Format>> 
detect_batch(const std::vector<std::string>& paths);
```

**参数：**
- `paths` - 文件路径列表

**返回值：**
- 路径和格式的配对列表，顺序与输入相同

**异常：**
- 可能抛出 `std::bad_alloc`（内存分配失败）

**说明：**
- 内部使用 `detect()` 检测每个文件
- 单个文件的错误不影响其他文件
- 错误的文件返回 `Format::Unknown`

**示例：**

```cpp
std::vector<std::string> files = {
    "image1.png",
    "image2.jpg",
    "document.pdf",
    "nonexistent.xyz"
};

auto results = fileformat::detect_batch(files);

for (const auto& [path, format] : results) {
    auto& info = fileformat::get_info(format);
    std::cout << path << " -> " << info.name << std::endl;
}

// 输出:
// image1.png -> PNG
// image2.jpg -> JPEG
// document.pdf -> PDF
// nonexistent.xyz -> Unknown
```

---

## 信息查询函数

### `get_info()` - 获取格式信息

```cpp
[[nodiscard]] const FormatInfo& get_info(Format format) noexcept;
```

**参数：**
- `format` - 格式枚举值

**返回值：**
- 格式信息的常量引用

**异常：**
- 不抛出异常（`noexcept`）

**说明：**
- 始终返回有效引用，不会返回空
- 无效的格式值返回 `Unknown` 的信息
- 返回的引用在程序生命周期内有效

**示例：**

```cpp
// 正常使用
auto& png_info = fileformat::get_info(fileformat::Format::PNG);
std::cout << png_info.name;       // "PNG"
std::cout << png_info.mime_type;  // "image/png"

// 无效格式
auto& unknown = fileformat::get_info(static_cast<fileformat::Format>(999));
std::cout << unknown.name;  // "Unknown"

// 配合检测使用
auto format = fileformat::detect("file.bin");
auto& info = fileformat::get_info(format);
```

---

### `get_category_name()` - 获取类别名称

```cpp
[[nodiscard]] std::string_view get_category_name(Category category) noexcept;
```

**参数：**
- `category` - 类别枚举值

**返回值：**
- 类别名称的字符串视图

**异常：**
- 不抛出异常（`noexcept`）

**类别名称映射：**

| Category | 返回值 |
|----------|--------|
| `Unknown` | "unknown" |
| `Image` | "image" |
| `Document` | "document" |
| `Ebook` | "ebook" |
| `Archive` | "archive" |
| `Media` | "media" |
| `Executable` | "executable" |

**示例：**

```cpp
auto& info = fileformat::get_info(fileformat::Format::PNG);
auto category_name = fileformat::get_category_name(info.category);
std::cout << category_name;  // "image"

// 用于分类显示
for (int i = 0; i < static_cast<int>(fileformat::Format::COUNT_); ++i) {
    auto format = static_cast<fileformat::Format>(i);
    auto& info = fileformat::get_info(format);
    auto cat = fileformat::get_category_name(info.category);
    std::cout << cat << "/" << info.name << std::endl;
}
```

---

## 内部函数

以下函数在 `fileformat::detail` 命名空间中，供高级用户使用：

```cpp
namespace fileformat::detail {
    // 各类型检测器
    Format detect_image(const uint8_t* data, size_t size) noexcept;
    Format detect_document(const uint8_t* data, size_t size) noexcept;
    Format detect_archive(const uint8_t* data, size_t size) noexcept;
    Format detect_ebook(const uint8_t* data, size_t size) noexcept;
    Format detect_media(const uint8_t* data, size_t size) noexcept;
    Format detect_executable(const uint8_t* data, size_t size) noexcept;
    
    // ZIP 内部结构分析
    Format detect_zip_content(const uint8_t* data, size_t size) noexcept;
}
```

**注意**：这些函数是内部实现细节，API 可能在未来版本中更改。

---

## 错误码

`detect_safe()` 返回的 `std::error_code` 使用标准 `std::errc` 枚举：

| 错误码 | 数值 | 说明 |
|--------|------|------|
| (无错误) | 0 | 检测成功 |
| `invalid_argument` | 22 | 无效参数（空路径等）|
| `no_such_file_or_directory` | 2 | 文件不存在 |
| `permission_denied` | 13 | 权限不足 |
| `io_error` | 5 | I/O 读取错误 |

**检查错误码示例：**

```cpp
auto result = fileformat::detect_safe("file.bin");

if (result.error) {
    // 方式 1：与 errc 比较
    if (result.error == std::errc::no_such_file_or_directory) {
        // ...
    }
    
    // 方式 2：获取错误消息
    std::string msg = result.error.message();
    
    // 方式 3：获取错误码值
    int code = result.error.value();
    
    // 方式 4：获取错误类别
    const std::error_category& cat = result.error.category();
}
```

---

## 线程安全

所有公共 API 都是线程安全的：

- 无全局可变状态
- 格式信息表是只读的 `constexpr` 数组
- 每次检测使用独立的局部变量

**并发使用示例：**

```cpp
#include <thread>
#include <vector>
#include <atomic>

std::atomic<int> png_count{0};
std::atomic<int> jpeg_count{0};

void worker(const std::vector<std::string>& files) {
    for (const auto& file : files) {
        auto format = fileformat::detect(file);
        if (format == fileformat::Format::PNG) ++png_count;
        if (format == fileformat::Format::JPEG) ++jpeg_count;
    }
}

int main() {
    std::vector<std::string> all_files = get_all_files();
    
    // 分割文件列表
    auto mid = all_files.begin() + all_files.size() / 2;
    std::vector<std::string> files1(all_files.begin(), mid);
    std::vector<std::string> files2(mid, all_files.end());
    
    // 并发检测
    std::thread t1(worker, std::ref(files1));
    std::thread t2(worker, std::ref(files2));
    
    t1.join();
    t2.join();
    
    std::cout << "PNG: " << png_count << std::endl;
    std::cout << "JPEG: " << jpeg_count << std::endl;
}
```

