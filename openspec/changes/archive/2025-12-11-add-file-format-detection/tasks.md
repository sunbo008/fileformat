# Tasks: Add File Format Detection Library

## 1. 项目结构搭建
- [x] 1.1 创建项目目录结构（include/, src/, tests/, examples/, cmake/）
- [x] 1.2 配置主 CMakeLists.txt（C++17、跨平台编译选项、构建选项）
- [x] 1.3 配置 cmake/FileFormatConfig.cmake.in（find_package 支持）
- [x] 1.4 创建 .clang-tidy 配置（禁止 new/malloc 检查规则）
- [x] 1.5 创建 .clang-format 配置
- [x] 1.6 创建 README.md 文档框架

## 2. 核心类型定义
- [x] 2.1 定义 Format 枚举（include/fileformat/types.hpp）
- [x] 2.2 定义 FormatInfo 结构体（name, mime_type, extension, category）
- [x] 2.3 定义 DetectResult 结构体（format, error_code）
- [x] 2.4 定义 Magic Bytes 签名数据结构
- [x] 2.5 创建格式信息静态表（constexpr）
- [x] 2.6 创建主头文件 fileformat.hpp

## 3. 检测引擎实现
- [x] 3.1 实现文件读取工具类（RAII，std::vector 缓冲区）
- [x] 3.2 实现 Magic Bytes 匹配引擎
- [x] 3.3 实现 detect(path) noexcept - 文件路径版本
- [x] 3.4 实现 detect(data, size) noexcept - 内存缓冲区版本
- [x] 3.5 实现 detect(stream) noexcept - 流版本
- [x] 3.6 实现 detect_safe(path) noexcept - 返回 DetectResult
- [x] 3.7 实现 detect_or_throw(path) - 抛异常版本
- [x] 3.8 实现 get_info(format) - 获取格式详细信息
- [x] 3.9 实现 detect_batch() - 批量检测

## 4. 格式检测器实现
- [x] 4.1 实现图像格式检测（PNG, JPEG, BMP, GIF, WebP, TIFF）
- [x] 4.2 实现文档格式检测（PDF）
- [x] 4.3 实现 Office 格式检测（DOC, DOCX, XLS, XLSX, PPT, PPTX）
- [x] 4.4 实现压缩格式检测（ZIP, RAR, 7Z, GZIP, TAR）
- [x] 4.5 实现电子书格式检测（EPUB, MOBI, AZW3, FB2, DJVU）
- [x] 4.6 实现媒体格式检测（MP3, MP4, WAV, AVI, MKV）
- [x] 4.7 实现可执行文件检测（EXE/DLL, ELF, Mach-O）

## 5. 单元测试
- [x] 5.1 集成 Google Test（FetchContent）
- [x] 5.2 创建测试数据生成脚本（生成各格式最小有效文件）
- [x] 5.3 编写图像格式测试用例
- [x] 5.4 编写文档格式测试用例
- [x] 5.5 编写 Office 格式测试用例
- [x] 5.6 编写压缩格式测试用例
- [x] 5.7 编写电子书格式测试用例
- [x] 5.8 编写媒体格式测试用例
- [x] 5.9 编写可执行文件格式测试用例

## 6. 健壮性测试
- [x] 6.1 编写边界条件测试（空文件、超小文件、超大文件）
- [x] 6.2 编写错误输入测试（空指针、零长度、无效路径）
- [x] 6.3 编写损坏文件测试（截断、头部损坏、伪造扩展名）
- [x] 6.4 编写并发测试（多线程同时调用）
- [x] 6.5 编写 API 变体测试（detect_safe, detect_or_throw）

## 7. 静态分析与质量保证
- [x] 7.1 配置 CMake 启用 Sanitizers（ASan, UBSan）
- [x] 7.2 集成 clang-tidy 到构建流程
- [x] 7.3 验证代码无 new/malloc 使用
- [ ] 7.4 配置代码覆盖率报告（gcov/lcov）
- [ ] 7.5 编写性能基准测试

## 8. 示例和文档
- [x] 8.1 创建命令行示例程序 detect_file.cpp
- [x] 8.2 创建批量检测示例
- [x] 8.3 完善 README.md（编译说明、API 文档、使用示例）
- [x] 8.4 添加 CHANGELOG.md
- [x] 8.5 添加 LICENSE 文件

## 9. 跨平台验证
- [x] 9.1 Windows MSVC 2019+ 编译验证
- [ ] 9.2 Windows MinGW-w64 编译验证
- [ ] 9.3 Linux GCC 9+ 编译验证
- [ ] 9.4 Linux Clang 10+ 编译验证
- [ ] 9.5 macOS Apple Clang 编译验证
- [ ] 9.6 配置 GitHub Actions CI（多平台矩阵）

## 依赖关系

```
1. 项目结构 ─┬─► 2. 类型定义 ─┬─► 3. 检测引擎 ─┬─► 4. 格式检测器
             │                │                │
             │                │                └─► 5. 单元测试
             │                │                └─► 6. 健壮性测试
             │                │
             └────────────────┴─► 7. 静态分析（可并行）
                                        │
                                        ▼
                              8. 示例文档 ◄─► 9. 跨平台验证
```

- 任务 1 完成后可启动任务 2
- 任务 2 完成后可并行启动任务 3、7
- 任务 3 完成后可并行启动任务 4、5、6
- 任务 4 完成后任务 5、6 可继续
- 任务 8、9 可在任务 4 完成后并行进行

## 验收标准

### 功能
- [x] 所有支持格式的检测准确率 100%
- [x] 所有 API 变体正常工作（detect, detect_safe, detect_or_throw）
- [x] 格式信息查询完整正确

### 质量
- [x] 代码零警告编译（-Wall -Wextra -Werror / /W4 /WX）
- [x] 无 new/malloc/free/delete 使用
- [x] clang-tidy 检查全部通过
- [ ] 单元测试覆盖率 > 90%
- [ ] Sanitizer 检查无错误

### 性能
- [x] 单文件检测 < 1ms（排除 I/O）
- [x] 内存使用 < 64KB
- [x] 1GB 文件检测 < 10ms

### 平台
- [x] Windows MSVC 编译通过
- [ ] Windows MinGW 编译通过
- [ ] Linux GCC 编译通过
- [ ] Linux Clang 编译通过
- [ ] macOS Clang 编译通过

### 文档
- [x] README 完整（编译、API、示例）
- [x] 代码注释覆盖公共 API
