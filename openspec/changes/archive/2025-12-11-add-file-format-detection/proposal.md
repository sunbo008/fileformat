# Change: Add File Format Detection Library

## Why
需要一个高性能、跨平台的 C++ 文件格式识别库，通过分析文件内部数据（magic bytes、文件头结构）而非文件扩展名来准确识别文件类型。这对于安全扫描、文件管理、内容分析等场景至关重要。

## What Changes
- 创建基于 CMake 的跨平台 C++ 工程结构
- 实现核心文件格式检测引擎
- 支持常见文件格式：PNG, JPEG, BMP, GIF, PDF, DOC, DOCX, ZIP, RAR, MP3, MP4 等
- 提供简洁的 C++ API 接口
- 包含完整的单元测试框架
- 优化性能，支持大文件快速检测

## Impact
- Affected specs: `file-format-detection`, `build-system`
- Affected code: 新项目，创建完整工程结构
  - `src/` - 核心库源码
  - `include/` - 公共头文件
  - `tests/` - 单元测试
  - `examples/` - 使用示例
  - `CMakeLists.txt` - 构建配置

