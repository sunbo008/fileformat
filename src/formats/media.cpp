#include "fileformat/detector.hpp"

#include <cstring>

namespace fileformat {
namespace detail {

namespace {

// Magic bytes 常量
constexpr uint8_t kId3Magic[] = {0x49, 0x44, 0x33};  // "ID3"
constexpr uint8_t kMp3SyncMagic[] = {0xFF, 0xFB};    // MP3 frame sync
constexpr uint8_t kMp3SyncMagic2[] = {0xFF, 0xFA};   // MP3 frame sync (variant)
constexpr uint8_t kMp3SyncMagic3[] = {0xFF, 0xF3};   // MP3 frame sync (variant)
constexpr uint8_t kMp3SyncMagic4[] = {0xFF, 0xF2};   // MP3 frame sync (variant)
constexpr uint8_t kRiffMagic[] = {0x52, 0x49, 0x46, 0x46};  // "RIFF"
constexpr uint8_t kWaveMagic[] = {0x57, 0x41, 0x56, 0x45};  // "WAVE"
constexpr uint8_t kAviMagic[] = {0x41, 0x56, 0x49, 0x20};   // "AVI "
constexpr uint8_t kFtypMagic[] = {0x66, 0x74, 0x79, 0x70};  // "ftyp"
constexpr uint8_t kMkvMagic[] = {0x1A, 0x45, 0xDF, 0xA3};   // EBML header

/// 比较内存
inline bool mem_equal(const uint8_t* data, const uint8_t* pattern, size_t len) {
    return std::memcmp(data, pattern, len) == 0;
}

/// 检查 MP3 帧同步
bool is_mp3_frame_sync(const uint8_t* data) {
    return mem_equal(data, kMp3SyncMagic, 2) ||
           mem_equal(data, kMp3SyncMagic2, 2) ||
           mem_equal(data, kMp3SyncMagic3, 2) ||
           mem_equal(data, kMp3SyncMagic4, 2);
}

}  // namespace

Format detect_media(const uint8_t* data, size_t size) noexcept {
    if (data == nullptr || size < 4) {
        return Format::Unknown;
    }

    // MP3: ID3 tag 或帧同步
    if (size >= 3 && mem_equal(data, kId3Magic, 3)) {
        return Format::MP3;
    }
    if (size >= 2 && is_mp3_frame_sync(data)) {
        return Format::MP3;
    }

    // RIFF 容器（WAV, AVI）
    if (size >= 12 && mem_equal(data, kRiffMagic, 4)) {
        if (mem_equal(data + 8, kWaveMagic, 4)) {
            return Format::WAV;
        }
        if (mem_equal(data + 8, kAviMagic, 4)) {
            return Format::AVI;
        }
    }

    // MP4/M4A/MOV: ftyp box
    if (size >= 8) {
        // ftyp 在偏移 4 处
        if (mem_equal(data + 4, kFtypMagic, 4)) {
            return Format::MP4;
        }
    }

    // MKV/WebM: EBML header
    if (size >= 4 && mem_equal(data, kMkvMagic, 4)) {
        return Format::MKV;
    }

    return Format::Unknown;
}

}  // namespace detail
}  // namespace fileformat

