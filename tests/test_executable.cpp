#include <gtest/gtest.h>

#include <cstdint>
#include <vector>

#include "fileformat/fileformat.hpp"

namespace fileformat {
namespace {

class ExecutableFormatTest : public ::testing::Test {
protected:
    // Windows PE: MZ
    const std::vector<uint8_t> exe_magic = {0x4D, 0x5A};

    // ELF: \x7FELF
    const std::vector<uint8_t> elf_magic = {0x7F, 0x45, 0x4C, 0x46};

    // Mach-O 64-bit
    const std::vector<uint8_t> macho_magic = {0xFE, 0xED, 0xFA, 0xCF};

    // Mach-O Fat binary
    const std::vector<uint8_t> macho_fat_magic = {0xCA, 0xFE, 0xBA, 0xBE};
};

TEST_F(ExecutableFormatTest, DetectExe) {
    auto format = detect(exe_magic.data(), exe_magic.size());
    EXPECT_EQ(format, Format::EXE);
}

TEST_F(ExecutableFormatTest, DetectElf) {
    auto format = detect(elf_magic.data(), elf_magic.size());
    EXPECT_EQ(format, Format::ELF);
}

TEST_F(ExecutableFormatTest, DetectMachO) {
    auto format = detect(macho_magic.data(), macho_magic.size());
    EXPECT_EQ(format, Format::MachO);
}

TEST_F(ExecutableFormatTest, DetectMachOFat) {
    auto format = detect(macho_fat_magic.data(), macho_fat_magic.size());
    EXPECT_EQ(format, Format::MachO);
}

TEST_F(ExecutableFormatTest, ExeFormatInfo) {
    auto& info = get_info(Format::EXE);
    EXPECT_EQ(info.format, Format::EXE);
    EXPECT_EQ(info.name, "EXE");
    EXPECT_EQ(info.extension, ".exe");
    EXPECT_EQ(info.category, Category::Executable);
}

TEST_F(ExecutableFormatTest, ElfFormatInfo) {
    auto& info = get_info(Format::ELF);
    EXPECT_EQ(info.format, Format::ELF);
    EXPECT_EQ(info.name, "ELF");
    EXPECT_EQ(info.category, Category::Executable);
}

}  // namespace
}  // namespace fileformat

