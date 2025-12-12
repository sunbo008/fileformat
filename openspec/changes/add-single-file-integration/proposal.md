# Change: Add Single-File Integration

## Why
用户需要将文件格式检测功能集成到现有程序中，希望有一个单一的 `.cpp` 文件，无需复杂的 CMake 配置，只需包含一个文件即可使用 `detect(data, size)` 接口。

## What Changes
- 生成一个独立的 `fileformat_single.hpp` 单头文件
- 只包含 `detect(const uint8_t* data, size_t size)` 接口
- 合并所有格式检测代码到一个文件
- 保持零外部依赖
- 简化集成：只需 `#include "fileformat_single.hpp"`

## Impact
- Affected specs: `file-format-detection`
- Affected code: 新增 `single/fileformat_single.hpp`






