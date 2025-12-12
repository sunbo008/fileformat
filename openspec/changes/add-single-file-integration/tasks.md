# Tasks: Add Single-File Integration

## 1. 生成单文件
- [ ] 1.1 创建 single/fileformat_single.hpp
- [ ] 1.2 合并 Format 枚举和必要类型
- [ ] 1.3 合并所有格式检测代码（image, document, archive, ebook, media, executable）
- [ ] 1.4 只暴露 detect(data, size) 接口

## 2. 验证
- [ ] 2.1 编译测试（无 CMake，直接 g++/clang++/MSVC）
- [ ] 2.2 功能验证

## 验收标准
- [ ] 单文件可独立编译
- [ ] detect(data, size) 接口正常工作
- [ ] 支持所有已实现的格式






