#include "fileformat/detector.hpp"

#include <cstring>

namespace fileformat {
namespace detail {

namespace {

// Magic bytes 常量
constexpr uint8_t kPngMagic[] = {0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A};
constexpr uint8_t kJpegMagic[] = {0xFF, 0xD8, 0xFF};
constexpr uint8_t kBmpMagic[] = {0x42, 0x4D};  // "BM"
constexpr uint8_t kGif87Magic[] = {0x47, 0x49, 0x46, 0x38, 0x37, 0x61};  // "GIF87a"
constexpr uint8_t kGif89Magic[] = {0x47, 0x49, 0x46, 0x38, 0x39, 0x61};  // "GIF89a"
constexpr uint8_t kRiffMagic[] = {0x52, 0x49, 0x46, 0x46};  // "RIFF"
constexpr uint8_t kWebpMagic[] = {0x57, 0x45, 0x42, 0x50};  // "WEBP"
constexpr uint8_t kTiffLeMagic[] = {0x49, 0x49, 0x2A, 0x00};  // Little-endian
constexpr uint8_t kTiffBeMagic[] = {0x4D, 0x4D, 0x00, 0x2A};  // Big-endian

/// 比较内存
inline bool mem_equal(const uint8_t* data, const uint8_t* pattern, size_t len) {
    return std::memcmp(data, pattern, len) == 0;
}

}  // namespace

Format detect_image(const uint8_t* data, size_t size) noexcept {
    if (data == nullptr || size < 2) {
        return Format::Unknown;
    }

    // PNG: 89 50 4E 47 0D 0A 1A 0A
    if (size >= 8 && mem_equal(data, kPngMagic, 8)) {
        return Format::PNG;
    }

    // JPEG: FF D8 FF
    if (size >= 3 && mem_equal(data, kJpegMagic, 3)) {
        return Format::JPEG;
    }

    // BMP: 42 4D ("BM")
    if (size >= 2 && mem_equal(data, kBmpMagic, 2)) {
        return Format::BMP;
    }

    // GIF: GIF87a 或 GIF89a
    if (size >= 6) {
        if (mem_equal(data, kGif87Magic, 6) || mem_equal(data, kGif89Magic, 6)) {
            return Format::GIF;
        }
    }

    // RIFF 容器（WebP, WAV, AVI）
    if (size >= 12 && mem_equal(data, kRiffMagic, 4)) {
        // WebP: RIFF....WEBP
        if (mem_equal(data + 8, kWebpMagic, 4)) {
            return Format::WebP;
        }
        // WAV 和 AVI 在 media.cpp 中处理
    }

    // TIFF: II*\0 (little-endian) 或 MM\0* (big-endian)
    if (size >= 4) {
        if (mem_equal(data, kTiffLeMagic, 4) || mem_equal(data, kTiffBeMagic, 4)) {
            return Format::TIFF;
        }
    }

    return Format::Unknown;
}

}  // namespace detail
}  // namespace fileformat

