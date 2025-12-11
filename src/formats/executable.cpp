#include "fileformat/detector.hpp"

#include <cstring>

namespace fileformat {
namespace detail {

namespace {

// Magic bytes 常量
constexpr uint8_t kMzMagic[] = {0x4D, 0x5A};  // "MZ"
constexpr uint8_t kElfMagic[] = {0x7F, 0x45, 0x4C, 0x46};  // "\x7FELF"
constexpr uint8_t kMachoMagic32[] = {0xFE, 0xED, 0xFA, 0xCE};  // Mach-O 32-bit
constexpr uint8_t kMachoMagic64[] = {0xFE, 0xED, 0xFA, 0xCF};  // Mach-O 64-bit
constexpr uint8_t kMachoMagic32Rev[] = {0xCE, 0xFA, 0xED, 0xFE};  // Mach-O 32-bit (reversed)
constexpr uint8_t kMachoMagic64Rev[] = {0xCF, 0xFA, 0xED, 0xFE};  // Mach-O 64-bit (reversed)
constexpr uint8_t kMachoFatMagic[] = {0xCA, 0xFE, 0xBA, 0xBE};  // Mach-O Fat binary

/// 比较内存
inline bool mem_equal(const uint8_t* data, const uint8_t* pattern, size_t len) {
    return std::memcmp(data, pattern, len) == 0;
}

}  // namespace

Format detect_executable(const uint8_t* data, size_t size) noexcept {
    if (data == nullptr || size < 2) {
        return Format::Unknown;
    }

    // Windows PE/COFF: MZ header
    if (size >= 2 && mem_equal(data, kMzMagic, 2)) {
        return Format::EXE;
    }

    // ELF: \x7FELF
    if (size >= 4 && mem_equal(data, kElfMagic, 4)) {
        return Format::ELF;
    }

    // Mach-O (various variants)
    if (size >= 4) {
        if (mem_equal(data, kMachoMagic32, 4) ||
            mem_equal(data, kMachoMagic64, 4) ||
            mem_equal(data, kMachoMagic32Rev, 4) ||
            mem_equal(data, kMachoMagic64Rev, 4) ||
            mem_equal(data, kMachoFatMagic, 4)) {
            return Format::MachO;
        }
    }

    return Format::Unknown;
}

}  // namespace detail
}  // namespace fileformat

