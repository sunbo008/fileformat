# FileFormat 开发指南

本文档面向想要贡献代码或了解内部实现的开发者。

---

## 开发环境设置

### 必需工具

| 工具 | 最低版本 | 说明 |
|------|----------|------|
| CMake | 3.14 | 构建系统 |
| C++ 编译器 | C++17 | MSVC 2019+ / GCC 9+ / Clang 10+ |
| Git | 2.0 | 版本控制 |

### 可选工具

| 工具 | 用途 |
|------|------|
| clang-format | 代码格式化 |
| clang-tidy | 静态分析 |
| Valgrind | 内存检查 (Linux) |
| lcov | 代码覆盖率 |

### 设置开发环境

```bash
# 克隆仓库
git clone https://github.com/your-username/fileformat.git
cd fileformat

# 创建开发构建
cmake -B build \
    -DFILEFORMAT_BUILD_TESTS=ON \
    -DFILEFORMAT_BUILD_EXAMPLES=ON \
    -DFILEFORMAT_ENABLE_SANITIZERS=ON

# 编译
cmake --build build

# 运行测试
cd build && ctest --output-on-failure
```

---

## 项目结构

```
fileformat/
├── CMakeLists.txt              # 主构建配置
├── .clang-tidy                 # 静态分析配置
├── .clang-format               # 代码格式配置
├── README.md                   # 项目说明
├── LICENSE                     # 许可证
├── CHANGELOG.md                # 变更日志
│
├── cmake/
│   └── FileFormatConfig.cmake.in  # CMake 包配置模板
│
├── include/fileformat/        # 公共头文件
│   ├── fileformat.hpp         # 主头文件（包含所有）
│   ├── types.hpp              # 类型定义
│   └── detector.hpp           # API 声明
│
├── src/                       # 源文件
│   ├── detector.cpp           # 核心检测逻辑
│   └── formats/               # 格式检测器
│       ├── image.cpp          # 图像格式
│       ├── document.cpp       # 文档格式
│       ├── archive.cpp        # 压缩格式
│       ├── ebook.cpp          # 电子书格式
│       ├── media.cpp          # 媒体格式
│       └── executable.cpp     # 可执行文件
│
├── tests/                     # 单元测试
│   ├── CMakeLists.txt
│   ├── test_main.cpp
│   ├── test_image.cpp
│   ├── test_document.cpp
│   ├── test_archive.cpp
│   ├── test_ebook.cpp
│   ├── test_media.cpp
│   ├── test_executable.cpp
│   ├── test_robustness.cpp    # 健壮性测试
│   └── test_api.cpp           # API 测试
│
├── examples/                  # 示例程序
│   ├── CMakeLists.txt
│   ├── detect_file.cpp
│   └── detect_batch.cpp
│
└── docs/                      # 文档
    ├── API.md
    ├── QUICKSTART.md
    └── DEVELOPMENT.md
```

---

## 代码规范

### 格式化

使用 `.clang-format` 配置自动格式化：

```bash
# 格式化单个文件
clang-format -i src/detector.cpp

# 格式化所有源文件
find src include -name "*.cpp" -o -name "*.hpp" | xargs clang-format -i
```

### 命名规范

| 类型 | 规范 | 示例 |
|------|------|------|
| 命名空间 | snake_case | `fileformat`, `detail` |
| 类/结构体 | PascalCase | `FormatInfo`, `DetectResult` |
| 函数 | snake_case | `detect`, `get_info` |
| 变量 | snake_case | `file_path`, `header_size` |
| 常量 | kCamelCase | `kMaxHeaderSize` |
| 枚举值 | PascalCase | `Format::PNG` |
| 宏 | UPPER_CASE | `FILEFORMAT_EXPORTS` |

### 内存管理规则

**禁止使用：**

```cpp
// ❌ 禁止
new / new[]
delete / delete[]
malloc / calloc / realloc / free
```

**必须使用：**

```cpp
// ✅ 使用 STL 容器
std::vector<uint8_t> buffer;
std::string path;
std::array<uint8_t, 16> magic;

// ✅ 使用智能指针（如必要）
auto ptr = std::make_unique<LargeObject>();
```

### 静态分析

运行 clang-tidy 检查：

```bash
cmake -B build -DFILEFORMAT_ENABLE_CLANG_TIDY=ON
cmake --build build
```

关键检查项：
- `cppcoreguidelines-no-malloc` - 禁止 malloc
- `cppcoreguidelines-owning-memory` - 所有权检查
- `modernize-*` - 现代 C++ 用法

---

## 添加新格式

### 步骤 1：更新枚举

在 `include/fileformat/types.hpp` 中添加新格式：

```cpp
enum class Format {
    // ... existing formats ...
    
    NewFormat,  // 添加在 COUNT_ 之前
    
    COUNT_
};
```

### 步骤 2：添加格式信息

在 `src/detector.cpp` 的 `kFormatInfoTable` 中添加：

```cpp
constexpr std::array<FormatInfo, static_cast<size_t>(Format::COUNT_)> kFormatInfoTable = {{
    // ... existing entries ...
    
    {Format::NewFormat, "NewFormat", "application/x-newformat", ".new", Category::Document},
}};
```

### 步骤 3：实现检测逻辑

在相应的格式文件中添加检测代码（如 `src/formats/document.cpp`）：

```cpp
namespace {
    // Magic bytes
    constexpr uint8_t kNewFormatMagic[] = {0x4E, 0x45, 0x57, 0x46};
}

Format detect_document(const uint8_t* data, size_t size) noexcept {
    // ... existing checks ...
    
    // NewFormat: NEWF
    if (size >= 4 && mem_equal(data, kNewFormatMagic, 4)) {
        return Format::NewFormat;
    }
    
    return Format::Unknown;
}
```

### 步骤 4：添加测试

在 `tests/test_document.cpp` 中添加测试：

```cpp
TEST_F(DocumentFormatTest, DetectNewFormat) {
    std::vector<uint8_t> magic = {0x4E, 0x45, 0x57, 0x46};
    auto format = detect(magic.data(), magic.size());
    EXPECT_EQ(format, Format::NewFormat);
}

TEST_F(DocumentFormatTest, NewFormatInfo) {
    auto& info = get_info(Format::NewFormat);
    EXPECT_EQ(info.format, Format::NewFormat);
    EXPECT_EQ(info.name, "NewFormat");
    EXPECT_EQ(info.mime_type, "application/x-newformat");
}
```

### 步骤 5：更新文档

- 更新 `README.md` 的格式列表
- 更新 `CHANGELOG.md`

---

## 测试

### 运行所有测试

```bash
cd build
ctest --output-on-failure
```

### 运行特定测试

```bash
# 运行图像测试
./build/tests/fileformat_tests --gtest_filter="ImageFormatTest.*"

# 运行健壮性测试
./build/tests/fileformat_tests --gtest_filter="RobustnessTest.*"
```

### 测试覆盖率

```bash
# 配置覆盖率构建
cmake -B build-cov \
    -DCMAKE_BUILD_TYPE=Debug \
    -DCMAKE_CXX_FLAGS="--coverage"

cmake --build build-cov
cd build-cov && ctest

# 生成报告
lcov --capture --directory . --output-file coverage.info
genhtml coverage.info --output-directory coverage_report
```

### Sanitizer 测试

```bash
# AddressSanitizer + UndefinedBehaviorSanitizer
cmake -B build-san -DFILEFORMAT_ENABLE_SANITIZERS=ON
cmake --build build-san
./build-san/tests/fileformat_tests
```

---

## 调试

### 调试检测逻辑

```cpp
// 添加调试输出
Format detect(const uint8_t* data, size_t size) noexcept {
    #ifndef NDEBUG
    std::cerr << "detect() called with " << size << " bytes" << std::endl;
    std::cerr << "First 16 bytes: ";
    for (size_t i = 0; i < std::min(size, size_t(16)); ++i) {
        std::cerr << std::hex << std::setw(2) << std::setfill('0') 
                  << static_cast<int>(data[i]) << " ";
    }
    std::cerr << std::endl;
    #endif
    
    // ... detection logic ...
}
```

### 使用 GDB/LLDB

```bash
# GDB
gdb ./build/tests/fileformat_tests
(gdb) break fileformat::detect
(gdb) run --gtest_filter="ImageFormatTest.DetectPng"

# LLDB
lldb ./build/tests/fileformat_tests
(lldb) breakpoint set -n fileformat::detect
(lldb) run --gtest_filter="ImageFormatTest.DetectPng"
```

---

## 性能分析

### 基准测试

```cpp
#include <chrono>

void benchmark_detect() {
    std::vector<uint8_t> data(4096);
    // 填充测试数据...
    
    auto start = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < 100000; ++i) {
        fileformat::detect(data.data(), data.size());
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    
    std::cout << "100000 detections: " << duration.count() << " μs" << std::endl;
    std::cout << "Per detection: " << duration.count() / 100000.0 << " μs" << std::endl;
}
```

### 使用 perf (Linux)

```bash
perf record ./build/tests/fileformat_tests
perf report
```

---

## 发布流程

1. 更新版本号（`CMakeLists.txt` 中的 `project(fileformat VERSION x.y.z)`）
2. 更新 `CHANGELOG.md`
3. 运行完整测试套件
4. 创建 Git tag: `git tag -a v1.0.0 -m "Release v1.0.0"`
5. 推送 tag: `git push origin v1.0.0`

---

## 常见问题

### Q: 为什么检测结果不正确？

1. 检查 magic bytes 是否正确
2. 检查偏移量是否正确
3. 检查是否有格式冲突（如 ZIP 和 DOCX）
4. 添加调试输出查看实际数据

### Q: 如何处理格式冲突？

某些格式共享相同的 magic bytes（如 DOCX 和 ZIP）。解决方法：

1. 使用更长的签名
2. 检查文件内部结构
3. 调整检测优先级

### Q: 如何提高性能？

1. 减少不必要的 I/O
2. 使用更高效的匹配算法
3. 避免重复检测
4. 使用内存映射（大文件）

