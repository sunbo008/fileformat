# 示例程序

# 单文件检测示例
add_executable(detect_file detect_file.cpp)
target_link_libraries(detect_file PRIVATE fileformat)

# 批量检测示例
add_executable(detect_batch detect_batch.cpp)
target_link_libraries(detect_batch PRIVATE fileformat)

