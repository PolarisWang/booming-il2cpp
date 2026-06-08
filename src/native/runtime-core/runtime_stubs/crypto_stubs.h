// crypto_stubs.h — Cryptography stub definitions
// Uses Windows CNG (BCrypt) API via native wrappers.
// Functions receive managed CHAOS_IL2CPP_INTPTR for byte[] arrays.
#ifndef CHAOS_IL2CPP_CRYPTO_STUBS_H_
#define CHAOS_IL2CPP_CRYPTO_STUBS_H_

#include <chaos/native_types.h>

namespace chaos { namespace il2cpp { namespace runtime_core {
extern "C" {

// ── RandomNumberGenerator: Fill(byte[], int, int) ──
void ChaosCngFillRandom(CHAOS_IL2CPP_INTPTR buffer, CHAOS_IL2CPP_INT32 offset, CHAOS_IL2CPP_INT32 count) noexcept;

// ── SHA hash: HashData(byte[]) -> byte[] (DirectNativeSymbol) ──
// Returns managed byte[] containing the hash, or 0 on failure.
CHAOS_IL2CPP_INTPTR ChaosSha1Hash(CHAOS_IL2CPP_INTPTR data) noexcept;
CHAOS_IL2CPP_INTPTR ChaosSha256Hash(CHAOS_IL2CPP_INTPTR data) noexcept;
CHAOS_IL2CPP_INTPTR ChaosSha384Hash(CHAOS_IL2CPP_INTPTR data) noexcept;
CHAOS_IL2CPP_INTPTR ChaosSha512Hash(CHAOS_IL2CPP_INTPTR data) noexcept;

// ── HMAC-SHA256: HashData(byte[], byte[]) -> byte[] ──
CHAOS_IL2CPP_INTPTR ChaosHmacSha256(CHAOS_IL2CPP_INTPTR key, CHAOS_IL2CPP_INTPTR data) noexcept;

// ── AES: Create() -> Aes object (stub) ──
CHAOS_IL2CPP_INTPTR ChaosAesCreate() noexcept;

}  // extern "C"
}  // namespace runtime_core
}  // namespace il2cpp
}  // namespace chaos

#endif  // CHAOS_IL2CPP_CRYPTO_STUBS_H_
