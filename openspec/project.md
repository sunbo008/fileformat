# Project Context

## Purpose
FileFormat 是一个高性能、跨平台的 C++ 文件格式识别库。通过分析文件内部数据（magic bytes、文件头结构）而非文件扩展名来准确识别文件类型。

## Tech Stack
- C++17
- CMake 3.14+
- Google Test (测试框架)

## Supported Platforms
- Windows (MSVC 2019+, MinGW-w64)
- Linux (GCC 9+, Clang 10+)
- macOS (Apple Clang)

## Project Conventions

### Code Style
- 使用 snake_case 命名函数和变量
- 使用 PascalCase 命名类型和枚举
- 使用 4 空格缩进
- 头文件使用 `.hpp` 扩展名
- 实现文件使用 `.cpp` 扩展名

### Architecture Patterns
- 单一职责：每个格式检测器专注于一类格式
- 延迟加载：只读取必要的文件头部数据
- 接口分离：公共 API 与内部实现分离

### Testing Strategy
- 使用 Google Test 框架
- 每种格式至少两个测试用例
- 包含边界条件测试（空文件、损坏文件）
- 性能基准测试

### Git Workflow
- main 分支保护
- 功能分支命名：feature/xxx
- 提交信息格式：`type(scope): description`

## Domain Context
文件格式识别依赖于 Magic Bytes（文件签名），不同格式有独特的字节序列。某些格式（如 Office 文档）需要深入分析内部结构。

## Important Constraints
- 零外部依赖（仅 C++ 标准库）
- 最小化文件 I/O 操作
- 内存使用不超过 64KB
- **禁止手动内存管理**：不使用 new/delete/malloc/free，一律使用 STL 容器和智能指针

## External Dependencies
- Google Test (仅测试，FetchContent 自动下载)
