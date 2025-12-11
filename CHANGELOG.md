# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [1.0.0] - 2024-12-11

### Added
- Initial release
- Core file format detection engine
- Support for 30+ file formats:
  - Images: PNG, JPEG, BMP, GIF, WebP, TIFF
  - Documents: PDF, DOC, DOCX, XLS, XLSX, PPT, PPTX
  - Ebooks: EPUB, MOBI, AZW3, FB2, DJVU
  - Archives: ZIP, RAR, 7Z, GZIP, TAR
  - Media: MP3, MP4, WAV, AVI, MKV
  - Executables: EXE/DLL, ELF, Mach-O
- Multiple detection APIs:
  - `detect()` - basic detection (noexcept)
  - `detect_safe()` - returns detailed error info
  - `detect_or_throw()` - exception-based
  - `detect_batch()` - batch detection
- Format information query via `get_info()`
- Cross-platform support: Windows, Linux, macOS
- Comprehensive unit tests
- Example programs

