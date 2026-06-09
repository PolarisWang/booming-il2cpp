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

// ── SHA-3 family: HashData(byte[]) -> byte[] (DirectNativeSymbol) ──
CHAOS_IL2CPP_INTPTR ChaosSha3_256Hash(CHAOS_IL2CPP_INTPTR data) noexcept;
CHAOS_IL2CPP_INTPTR ChaosSha3_384Hash(CHAOS_IL2CPP_INTPTR data) noexcept;
CHAOS_IL2CPP_INTPTR ChaosSha3_512Hash(CHAOS_IL2CPP_INTPTR data) noexcept;

// ── HMAC: HashData(byte[], byte[]) -> byte[] (DirectNativeSymbol) ──
CHAOS_IL2CPP_INTPTR ChaosHmacSha1(CHAOS_IL2CPP_INTPTR key, CHAOS_IL2CPP_INTPTR data) noexcept;
CHAOS_IL2CPP_INTPTR ChaosHmacSha256(CHAOS_IL2CPP_INTPTR key, CHAOS_IL2CPP_INTPTR data) noexcept;
CHAOS_IL2CPP_INTPTR ChaosHmacSha384(CHAOS_IL2CPP_INTPTR key, CHAOS_IL2CPP_INTPTR data) noexcept;
CHAOS_IL2CPP_INTPTR ChaosHmacSha512(CHAOS_IL2CPP_INTPTR key, CHAOS_IL2CPP_INTPTR data) noexcept;
CHAOS_IL2CPP_INTPTR ChaosHmacSha3_256(CHAOS_IL2CPP_INTPTR key, CHAOS_IL2CPP_INTPTR data) noexcept;
CHAOS_IL2CPP_INTPTR ChaosHmacSha3_384(CHAOS_IL2CPP_INTPTR key, CHAOS_IL2CPP_INTPTR data) noexcept;
CHAOS_IL2CPP_INTPTR ChaosHmacSha3_512(CHAOS_IL2CPP_INTPTR key, CHAOS_IL2CPP_INTPTR data) noexcept;

// ── MD5: HashData(byte[]) -> byte[] (DirectNativeSymbol) ──
CHAOS_IL2CPP_INTPTR ChaosMd5Hash(CHAOS_IL2CPP_INTPTR data) noexcept;

// ── HMACMD5: HashData(byte[], byte[]) -> byte[] (DirectNativeSymbol) ──
CHAOS_IL2CPP_INTPTR ChaosHmacMd5(CHAOS_IL2CPP_INTPTR key, CHAOS_IL2CPP_INTPTR data) noexcept;

// ── RNG: GetBytes(int) -> byte[] (DirectNativeSymbol) ──
CHAOS_IL2CPP_INTPTR ChaosCngGetBytes(CHAOS_IL2CPP_INT32 count) noexcept;

// ── RNG: Fill(byte[]) (DirectNativeSymbol) ──
void ChaosCngFill(CHAOS_IL2CPP_INTPTR buffer) noexcept;

// ── AES: Create() -> Aes object (stub) ──
CHAOS_IL2CPP_INTPTR ChaosAesCreate() noexcept;

}  // extern "C"
}  // namespace runtime_core
}  // namespace il2cpp
}  // namespace chaos

#endif  // CHAOS_IL2CPP_CRYPTO_STUBS_H_
