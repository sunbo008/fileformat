# file-format-detection Specification

## Purpose
TBD - created by archiving change add-file-format-detection. Update Purpose after archive.
## Requirements
### Requirement: Core Format Detection
系统 SHALL 通过分析文件内部数据（magic bytes、文件头结构）识别文件格式，不依赖文件扩展名。

#### Scenario: Detect PNG file
- **GIVEN** 一个 PNG 格式的文件（无论其扩展名是什么）
- **WHEN** 调用 detect() 函数
- **THEN** 返回 Format::PNG

#### Scenario: Detect file with wrong extension
- **GIVEN** 一个实际为 JPEG 格式但扩展名为 .png 的文件
- **WHEN** 调用 detect() 函数
- **THEN** 返回 Format::JPEG（基于文件内容而非扩展名）

#### Scenario: Unknown format
- **GIVEN** 一个不在支持列表中的文件格式
- **WHEN** 调用 detect() 函数
- **THEN** 返回 Format::Unknown

### Requirement: Multiple Input Sources
系统 SHALL 支持多种输入方式进行格式检测。

#### Scenario: Detect from file path
- **GIVEN** 一个有效的文件路径字符串
- **WHEN** 调用 detect(const std::string& path)
- **THEN** 返回正确的文件格式

#### Scenario: Detect from memory buffer
- **GIVEN** 文件数据的内存缓冲区和大小
- **WHEN** 调用 detect(const uint8_t* data, size_t size)
- **THEN** 返回正确的文件格式

#### Scenario: Detect from stream
- **GIVEN** 一个可读的 std::istream
- **WHEN** 调用 detect(std::istream& stream)
- **THEN** 返回正确的文件格式，且流位置被重置

### Requirement: Image Format Support
系统 SHALL 支持检测常见图像格式。

#### Scenario: Detect PNG
- **GIVEN** 一个以 `89 50 4E 47 0D 0A 1A 0A` 开头的文件
- **WHEN** 调用 detect()
- **THEN** 返回 Format::PNG

#### Scenario: Detect JPEG
- **GIVEN** 一个以 `FF D8 FF` 开头的文件
- **WHEN** 调用 detect()
- **THEN** 返回 Format::JPEG

#### Scenario: Detect BMP
- **GIVEN** 一个以 `42 4D` 开头的文件
- **WHEN** 调用 detect()
- **THEN** 返回 Format::BMP

#### Scenario: Detect GIF
- **GIVEN** 一个以 `47 49 46 38` 开头的文件
- **WHEN** 调用 detect()
- **THEN** 返回 Format::GIF

### Requirement: Document Format Support
系统 SHALL 支持检测常见文档格式。

#### Scenario: Detect PDF
- **GIVEN** 一个以 `25 50 44 46` (%PDF) 开头的文件
- **WHEN** 调用 detect()
- **THEN** 返回 Format::PDF

#### Scenario: Detect legacy DOC
- **GIVEN** 一个以 OLE Compound Document 签名开头且内部包含 Word 文档流的文件
- **WHEN** 调用 detect()
- **THEN** 返回 Format::DOC

#### Scenario: Detect DOCX
- **GIVEN** 一个 ZIP 格式文件，内部包含 [Content_Types].xml 和 word/ 目录
- **WHEN** 调用 detect()
- **THEN** 返回 Format::DOCX

### Requirement: Ebook Format Support
系统 SHALL 支持检测常见电子书格式。

#### Scenario: Detect EPUB
- **GIVEN** 一个 ZIP 格式文件，首个文件为 mimetype 且内容为 "application/epub+zip"
- **WHEN** 调用 detect()
- **THEN** 返回 Format::EPUB

#### Scenario: Detect MOBI
- **GIVEN** 一个以 `42 4F 4F 4B 4D 4F 42 49` (BOOKMOBI) 开头的文件
- **WHEN** 调用 detect()
- **THEN** 返回 Format::MOBI

#### Scenario: Detect AZW3
- **GIVEN** 一个 MOBI 格式文件，EXTH 记录中包含 KF8 标记
- **WHEN** 调用 detect()
- **THEN** 返回 Format::AZW3

#### Scenario: Detect FB2
- **GIVEN** 一个 XML 文件，根元素为 FictionBook
- **WHEN** 调用 detect()
- **THEN** 返回 Format::FB2

#### Scenario: Detect DJVU
- **GIVEN** 一个以 `41 54 26 54 46 4F 52 4D` (AT&TFORM) 开头的 DJVU 文件
- **WHEN** 调用 detect()
- **THEN** 返回 Format::DJVU

#### Scenario: Distinguish EPUB from regular ZIP
- **GIVEN** 一个普通 ZIP 文件（无 EPUB mimetype）
- **WHEN** 调用 detect()
- **THEN** 返回 Format::ZIP 而非 Format::EPUB

### Requirement: Archive Format Support
系统 SHALL 支持检测常见压缩格式。

#### Scenario: Detect ZIP
- **GIVEN** 一个以 `50 4B 03 04` 开头的普通 ZIP 文件
- **WHEN** 调用 detect()
- **THEN** 返回 Format::ZIP

#### Scenario: Detect RAR
- **GIVEN** 一个以 `52 61 72 21 1A 07` 开头的文件
- **WHEN** 调用 detect()
- **THEN** 返回 Format::RAR

#### Scenario: Detect 7Z
- **GIVEN** 一个以 `37 7A BC AF 27 1C` 开头的文件
- **WHEN** 调用 detect()
- **THEN** 返回 Format::SevenZip

### Requirement: Media Format Support
系统 SHALL 支持检测常见媒体格式。

#### Scenario: Detect MP3 with ID3 tag
- **GIVEN** 一个以 `49 44 33` (ID3) 开头的 MP3 文件
- **WHEN** 调用 detect()
- **THEN** 返回 Format::MP3

#### Scenario: Detect MP3 without ID3 tag
- **GIVEN** 一个以帧同步字节 `FF FB` 开头的 MP3 文件
- **WHEN** 调用 detect()
- **THEN** 返回 Format::MP3

#### Scenario: Detect MP4
- **GIVEN** 一个包含 ftyp box 的 MP4 文件
- **WHEN** 调用 detect()
- **THEN** 返回 Format::MP4

### Requirement: Format Information Query
系统 SHALL 提供查询格式详细信息的能力。

#### Scenario: Get format info
- **GIVEN** 一个有效的 Format 枚举值
- **WHEN** 调用 get_info(format)
- **THEN** 返回包含 name, mime_type, extension, category 的 FormatInfo 结构

#### Scenario: Get PNG info
- **GIVEN** Format::PNG
- **WHEN** 调用 get_info(Format::PNG)
- **THEN** 返回 name="PNG", mime_type="image/png", extension=".png", category="image"

### Requirement: Performance
系统 SHALL 满足以下性能要求。

#### Scenario: Fast detection
- **GIVEN** 一个 1GB 的 PNG 文件
- **WHEN** 调用 detect()
- **THEN** 检测时间应小于 10ms（因为只读取文件头部）

#### Scenario: Minimal memory usage
- **GIVEN** 任意大小的文件
- **WHEN** 调用 detect(path)
- **THEN** 内存使用不超过 64KB（只加载必要的头部数据）

### Requirement: Error Handling
系统 SHALL 安全地处理错误情况。

#### Scenario: Non-existent file
- **GIVEN** 一个不存在的文件路径
- **WHEN** 调用 detect()
- **THEN** 返回 Format::Unknown 或抛出明确的异常

#### Scenario: Empty file
- **GIVEN** 一个空文件（0 字节）
- **WHEN** 调用 detect()
- **THEN** 返回 Format::Unknown

#### Scenario: Permission denied
- **GIVEN** 一个无读取权限的文件
- **WHEN** 调用 detect()
- **THEN** 返回 Format::Unknown 或抛出明确的异常

#### Scenario: Corrupted file header
- **GIVEN** 一个文件头部被损坏的文件
- **WHEN** 调用 detect()
- **THEN** 返回 Format::Unknown 或能识别的最接近格式，不会崩溃

### Requirement: Input Validation
系统 SHALL 对所有输入进行安全验证。

#### Scenario: Null pointer input
- **GIVEN** 调用 detect(nullptr, 100)
- **WHEN** 传入空指针
- **THEN** 返回 Format::Unknown，不崩溃

#### Scenario: Zero size buffer
- **GIVEN** 调用 detect(data, 0)
- **WHEN** 传入零长度缓冲区
- **THEN** 返回 Format::Unknown

#### Scenario: Very small file
- **GIVEN** 一个只有 1 字节的文件
- **WHEN** 调用 detect()
- **THEN** 返回 Format::Unknown

#### Scenario: Empty path string
- **GIVEN** 调用 detect("")
- **WHEN** 传入空路径
- **THEN** 返回 Format::Unknown

#### Scenario: Path with invalid characters
- **GIVEN** 包含非法字符的路径（如 Windows 上的 <, >, |）
- **WHEN** 调用 detect()
- **THEN** 返回 Format::Unknown 或设置相应错误码

### Requirement: Thread Safety
系统 SHALL 保证所有公共 API 的线程安全。

#### Scenario: Concurrent detection
- **GIVEN** 多个线程同时调用 detect()
- **WHEN** 并发检测不同文件
- **THEN** 每个调用返回正确结果，无数据竞争

#### Scenario: Concurrent format info query
- **GIVEN** 多个线程同时调用 get_info()
- **WHEN** 并发查询格式信息
- **THEN** 每个调用返回正确的 FormatInfo 引用

### Requirement: Memory Management Policy
系统 SHALL 完全使用 STL 容器和智能指针管理内存，禁止手动内存管理。

#### Scenario: No raw new/delete
- **GIVEN** 整个代码库
- **WHEN** 静态分析检查
- **THEN** 不存在 new、new[]、delete、delete[] 关键字

#### Scenario: No malloc/free
- **GIVEN** 整个代码库
- **WHEN** 静态分析检查
- **THEN** 不存在 malloc、calloc、realloc、free 函数调用

#### Scenario: Use STL containers
- **GIVEN** 需要动态大小缓冲区
- **WHEN** 实现检测功能
- **THEN** 使用 std::vector、std::string 等 STL 容器

#### Scenario: Use smart pointers when needed
- **GIVEN** 极少数需要动态分配的场景
- **WHEN** 需要堆上对象
- **THEN** 使用 std::unique_ptr 或 std::make_unique

### Requirement: Resource Safety
系统 SHALL 安全管理所有资源，无泄漏。

#### Scenario: File handle cleanup on success
- **GIVEN** 成功检测一个文件
- **WHEN** detect() 返回后
- **THEN** 文件句柄已关闭

#### Scenario: File handle cleanup on error
- **GIVEN** 检测过程中发生错误
- **WHEN** detect() 返回后
- **THEN** 文件句柄已关闭，无资源泄漏

#### Scenario: Memory cleanup
- **GIVEN** 连续调用 detect() 1000 次
- **WHEN** 所有调用完成后
- **THEN** 内存使用回到初始水平，无泄漏

### Requirement: Detailed Error Reporting
系统 SHALL 提供详细错误信息的 API 变体。

#### Scenario: Get error code for missing file
- **GIVEN** 一个不存在的文件路径
- **WHEN** 调用 detect_safe()
- **THEN** 返回 DetectResult，其 error 为 std::errc::no_such_file_or_directory

#### Scenario: Get error code for permission denied
- **GIVEN** 一个无读取权限的文件
- **WHEN** 调用 detect_safe()
- **THEN** 返回 DetectResult，其 error 为 std::errc::permission_denied

#### Scenario: Throw on error
- **GIVEN** 一个不存在的文件路径
- **WHEN** 调用 detect_or_throw()
- **THEN** 抛出 std::system_error 异常

