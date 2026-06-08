// pal_random.h — Cryptographically secure random byte generation
#pragma once

#include <cstddef>
#include <cstdint>

namespace chaos::il2cpp::pal {

/// Fill buffer with cryptographically secure random bytes.
/// On Win32: uses BCryptGenRandom.
/// On POSIX: reads /dev/urandom.
/// Returns true on success, false on failure (buffer contents undefined on failure).
bool PalRandomBytes(void* buffer, size_t count) noexcept;

}  // namespace chaos::il2cpp::pal
