#ifndef CHAOS_IL2CPP_PAL_ENCODING_H_
#define CHAOS_IL2CPP_PAL_ENCODING_H_

// ── String encoding / handle close abstraction ───────────────────────
// Abstracts Win32 MultiByteToWideChar/WideCharToMultiByte for UTF-8↔UTF-16
// conversion, and CloseHandle for handle cleanup.
//
// On Win32: delegates to the real Windows API.
// On POSIX: uses hand-written UTF-8/UTF-16 decoders and no-op stubs.

#include <cstdint>

namespace chaos::il2cpp::pal {

/// Convert UTF-8 to UTF-16. Returns the number of UTF-16 code units written,
/// or the required capacity (excluding null terminator) if buf==null
/// or capacity==0. Returns -1 on invalid input.
int32_t PalUtf8ToUtf16(const uint8_t* utf8, int32_t utf8_len,
                       uint16_t* utf16, int32_t utf16_capacity) noexcept;

/// Convert UTF-16 to UTF-8. Returns the number of UTF-8 bytes written,
/// or the required capacity (excluding null terminator) if buf==null
/// or capacity==0. Returns -1 on invalid input.
int32_t PalUtf16ToUtf8(const uint16_t* utf16, int32_t utf16_len,
                       uint8_t* utf8, int32_t utf8_capacity) noexcept;

/// Close an OS handle.  On Win32: calls CloseHandle.
/// On POSIX: no-op, always returns true.
bool PalCloseHandle(intptr_t handle) noexcept;

}  // namespace chaos::il2cpp::pal

#endif  // CHAOS_IL2CPP_PAL_ENCODING_H_
