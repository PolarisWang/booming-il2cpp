// crypto_stubs.h — Cryptography stub definitions
// Uses Windows CNG (BCrypt) API via P/Invoke-compatible extern "C" wrappers.
#ifndef CHAOS_IL2CPP_CRYPTO_STUBS_H_
#define CHAOS_IL2CPP_CRYPTO_STUBS_H_

#include <chaos/native_types.h>

namespace chaos { namespace il2cpp { namespace runtime_core {
extern "C" {

// ── RandomNumberGenerator ──
void ChaosCngFillRandom(CHAOS_IL2CPP_UINT8* buffer, CHAOS_IL2CPP_INT32 count) noexcept;

// ── Hash algorithms ──
// Returns hash byte count on success, -1 on failure.
CHAOS_IL2CPP_INT32 ChaosSha1Hash(
    const CHAOS_IL2CPP_UINT8* input, CHAOS_IL2CPP_INT32 inputLength,
    CHAOS_IL2CPP_UINT8* output, CHAOS_IL2CPP_INT32 outputLength) noexcept;

CHAOS_IL2CPP_INT32 ChaosSha256Hash(
    const CHAOS_IL2CPP_UINT8* input, CHAOS_IL2CPP_INT32 inputLength,
    CHAOS_IL2CPP_UINT8* output, CHAOS_IL2CPP_INT32 outputLength) noexcept;

CHAOS_IL2CPP_INT32 ChaosSha384Hash(
    const CHAOS_IL2CPP_UINT8* input, CHAOS_IL2CPP_INT32 inputLength,
    CHAOS_IL2CPP_UINT8* output, CHAOS_IL2CPP_INT32 outputLength) noexcept;

CHAOS_IL2CPP_INT32 ChaosSha512Hash(
    const CHAOS_IL2CPP_UINT8* input, CHAOS_IL2CPP_INT32 inputLength,
    CHAOS_IL2CPP_UINT8* output, CHAOS_IL2CPP_INT32 outputLength) noexcept;

// ── HMAC ──
CHAOS_IL2CPP_INT32 ChaosHmacSha256(
    const CHAOS_IL2CPP_UINT8* key, CHAOS_IL2CPP_INT32 keyLength,
    const CHAOS_IL2CPP_UINT8* input, CHAOS_IL2CPP_INT32 inputLength,
    CHAOS_IL2CPP_UINT8* output, CHAOS_IL2CPP_INT32 outputLength) noexcept;

// ── AES CBC ──
CHAOS_IL2CPP_INT32 ChaosAesEncryptCbc(
    const CHAOS_IL2CPP_UINT8* key, CHAOS_IL2CPP_INT32 keyLength,
    const CHAOS_IL2CPP_UINT8* iv, CHAOS_IL2CPP_INT32 ivLength,
    const CHAOS_IL2CPP_UINT8* input, CHAOS_IL2CPP_INT32 inputLength,
    CHAOS_IL2CPP_UINT8* output, CHAOS_IL2CPP_INT32 outputLength) noexcept;

CHAOS_IL2CPP_INT32 ChaosAesDecryptCbc(
    const CHAOS_IL2CPP_UINT8* key, CHAOS_IL2CPP_INT32 keyLength,
    const CHAOS_IL2CPP_UINT8* iv, CHAOS_IL2CPP_INT32 ivLength,
    const CHAOS_IL2CPP_UINT8* input, CHAOS_IL2CPP_INT32 inputLength,
    CHAOS_IL2CPP_UINT8* output, CHAOS_IL2CPP_INT32 outputLength) noexcept;

}  // extern "C"
}  // namespace runtime_core
}  // namespace il2cpp
}  // namespace chaos

#endif  // CHAOS_IL2CPP_CRYPTO_STUBS_H_
