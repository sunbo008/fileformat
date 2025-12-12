## ADDED Requirements

### Requirement: Single-File Integration
系统 SHALL 提供单文件版本，便于快速集成到其他项目。

#### Scenario: Include single header
- **GIVEN** 用户只需要 detect(data, size) 接口
- **WHEN** 包含 `fileformat_single.hpp`
- **THEN** 可以直接使用 `fileformat::detect(data, size)` 函数

#### Scenario: Compile without CMake
- **GIVEN** 单文件 `fileformat_single.hpp`
- **WHEN** 使用任意 C++17 编译器直接编译
- **THEN** 无需 CMake 配置即可编译成功

#### Scenario: Detect from buffer
- **GIVEN** 内存中的文件数据
- **WHEN** 调用 `fileformat::detect(data.data(), data.size())`
- **THEN** 返回正确的 Format 枚举值






