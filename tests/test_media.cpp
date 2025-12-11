#include <gtest/gtest.h>

#include <cstdint>
#include <vector>

#include "fileformat/fileformat.hpp"

namespace fileformat {
namespace {

class MediaFormatTest : public ::testing::Test {
protected:
    // MP3 with ID3 tag
    const std::vector<uint8_t> mp3_id3_magic = {0x49, 0x44, 0x33};  // ID3

    // MP3 frame sync
    const std::vector<uint8_t> mp3_sync_magic = {0xFF, 0xFB};

    // WAV: RIFF....WAVE
    const std::vector<uint8_t> wav_magic = {0x52, 0x49, 0x46, 0x46, 0x00, 0x00,
                                             0x00, 0x00, 0x57, 0x41, 0x56, 0x45};

    // AVI: RIFF....AVI
    const std::vector<uint8_t> avi_magic = {0x52, 0x49, 0x46, 0x46, 0x00, 0x00,
                                             0x00, 0x00, 0x41, 0x56, 0x49, 0x20};

    // MP4: ....ftyp
    const std::vector<uint8_t> mp4_magic = {0x00, 0x00, 0x00, 0x18, 0x66, 0x74, 0x79, 0x70};

    // MKV: EBML header
    const std::vector<uint8_t> mkv_magic = {0x1A, 0x45, 0xDF, 0xA3};
};

TEST_F(MediaFormatTest, DetectMp3WithId3) {
    auto format = detect(mp3_id3_magic.data(), mp3_id3_magic.size());
    EXPECT_EQ(format, Format::MP3);
}

TEST_F(MediaFormatTest, DetectMp3FrameSync) {
    auto format = detect(mp3_sync_magic.data(), mp3_sync_magic.size());
    EXPECT_EQ(format, Format::MP3);
}

TEST_F(MediaFormatTest, DetectWav) {
    auto format = detect(wav_magic.data(), wav_magic.size());
    EXPECT_EQ(format, Format::WAV);
}

TEST_F(MediaFormatTest, DetectAvi) {
    auto format = detect(avi_magic.data(), avi_magic.size());
    EXPECT_EQ(format, Format::AVI);
}

TEST_F(MediaFormatTest, DetectMp4) {
    auto format = detect(mp4_magic.data(), mp4_magic.size());
    EXPECT_EQ(format, Format::MP4);
}

TEST_F(MediaFormatTest, DetectMkv) {
    auto format = detect(mkv_magic.data(), mkv_magic.size());
    EXPECT_EQ(format, Format::MKV);
}

TEST_F(MediaFormatTest, Mp3FormatInfo) {
    auto& info = get_info(Format::MP3);
    EXPECT_EQ(info.format, Format::MP3);
    EXPECT_EQ(info.name, "MP3");
    EXPECT_EQ(info.mime_type, "audio/mpeg");
    EXPECT_EQ(info.extension, ".mp3");
    EXPECT_EQ(info.category, Category::Media);
}

TEST_F(MediaFormatTest, Mp4FormatInfo) {
    auto& info = get_info(Format::MP4);
    EXPECT_EQ(info.format, Format::MP4);
    EXPECT_EQ(info.name, "MP4");
    EXPECT_EQ(info.mime_type, "video/mp4");
}

}  // namespace
}  // namespace fileformat

