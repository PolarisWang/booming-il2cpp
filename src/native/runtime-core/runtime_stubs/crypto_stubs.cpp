// ABI exports: extern "C" linkage for managed/NativeAOT callability.

// crypto_stubs.cpp — Cryptography stub implementations
// Uses Windows CNG (BCrypt) APIs for native crypto operations.
// These stubs are compiled from source (not part of prebuilt lib)
// to avoid stale-symbol issues with the SDK runtime library.

#include <cstdlib>
#include <cstring>
#include <array>

#include "generated_code_compat.h"
#include "runtime_stubs/stub_common.h"
#include "runtime_stubs/crypto_stubs.h"
#include "runtime_core.h"

namespace chaos::il2cpp::runtime_core {
extern "C" {

// ── BCrypt utilities ───────────────────────────────────────────────
// NTSTATUS helper: returns true if a BCrypt call succeeded.
static inline bool ChaosCngSuccess(int ntstatus) noexcept {
    return ntstatus >= 0;
}

// Default BCrypt allocator (heap-based, no special alignment).
static void* ChaosCngAlloc(size_t size) noexcept {
    return std::malloc(size);
}

static void ChaosCngFree(void* ptr) noexcept {
    std::free(ptr);
}

// ── RandomNumberGenerator ──────────────────────────────────────────
// Fills a buffer with cryptographically strong random bytes via BCryptGenRandom.
void ChaosCngFillRandom(CHAOS_IL2CPP_UINT8* buffer, CHAOS_IL2CPP_INT32 count) noexcept
{
    if (buffer == nullptr || count <= 0) return;
    // BCRYPT_USE_SYSTEM_PREFERRED_RNG (0x00000002) uses the system RNG.
    // This is a blocking call that waits for sufficient entropy.
    ULONG ulCount = static_cast<ULONG>(count > 0 ? count : 0);
    NTSTATUS status = BCryptGenRandom(nullptr, buffer, ulCount,
                                      BCRYPT_USE_SYSTEM_PREFERRED_RNG);
    if (!ChaosCngSuccess(status)) {
        // Fallback: use PAL random generator for best-effort
        chaos::il2cpp::pal::PalRandomFill(buffer, static_cast<size_t>(count));
    }
}

// ── Hash algorithms (SHA1, SHA256, SHA384, SHA512) ─────────────────
// Computes hash of input data using BCrypt hash API.
// Returns true on success, false on failure.

static bool ChaosCngHashOneShot(
    const CHAOS_IL2CPP_UINT8* algorithm,
    const CHAOS_IL2CPP_UINT8* input,
    CHAOS_IL2CPP_INT32 inputLength,
    CHAOS_IL2CPP_UINT8* output,
    CHAOS_IL2CPP_INT32* outputLength)
{
    if (input == nullptr || output == nullptr || outputLength == nullptr)
        return false;

    BCRYPT_ALG_HANDLE hAlg = nullptr;
    NTSTATUS status = BCryptOpenAlgorithmProvider(&hAlg, algorithm, nullptr, 0);
    if (!ChaosCngSuccess(status)) return false;

    // Get hash object size
    ULONG hashObjSize = 0;
    ULONG resultSize = 0;
    status = BCryptGetProperty(hAlg, BCRYPT_OBJECT_LENGTH,
                               reinterpret_cast<PUCHAR>(&hashObjSize), sizeof(hashObjSize),
                               &resultSize, 0);
    if (!ChaosCngSuccess(status)) {
        BCryptCloseAlgorithmProvider(hAlg, 0);
        return false;
    }

    // Get hash length
    ULONG hashLen = 0;
    status = BCryptGetProperty(hAlg, BCRYPT_HASH_LENGTH,
                               reinterpret_cast<PUCHAR>(&hashLen), sizeof(hashLen),
                               &resultSize, 0);
    if (!ChaosCngSuccess(status)) {
        BCryptCloseAlgorithmProvider(hAlg, 0);
        return false;
    }

    if (*outputLength < static_cast<CHAOS_IL2CPP_INT32>(hashLen)) {
        *outputLength = static_cast<CHAOS_IL2CPP_INT32>(hashLen);
        BCryptCloseAlgorithmProvider(hAlg, 0);
        return false;
    }

    // Allocate hash object buffer
    auto hashObj = std::make_unique<CHAOS_IL2CPP_UINT8[]>(hashObjSize);
    if (!hashObj) {
        BCryptCloseAlgorithmProvider(hAlg, 0);
        return false;
    }

    BCRYPT_HASH_HANDLE hHash = nullptr;
    status = BCryptCreateHash(hAlg, &hHash, hashObj.get(),
                              static_cast<ULONG>(hashObjSize), nullptr, 0, 0);
    if (!ChaosCngSuccess(status)) {
        BCryptCloseAlgorithmProvider(hAlg, 0);
        return false;
    }

    status = BCryptHashData(hHash, const_cast<PUCHAR>(input),
                            static_cast<ULONG>(inputLength), 0);
    if (ChaosCngSuccess(status)) {
        status = BCryptFinishHash(hHash, output, hashLen, 0);
        if (ChaosCngSuccess(status)) {
            *outputLength = static_cast<CHAOS_IL2CPP_INT32>(hashLen);
        }
    }

    BCryptDestroyHash(hHash);
    BCryptCloseAlgorithmProvider(hAlg, 0);
    return ChaosCngSuccess(status);
}

CHAOS_IL2CPP_INT32 ChaosSha1Hash(
    const CHAOS_IL2CPP_UINT8* input,
    CHAOS_IL2CPP_INT32 inputLength,
    CHAOS_IL2CPP_UINT8* output,
    CHAOS_IL2CPP_INT32 outputLength) noexcept
{
    CHAOS_IL2CPP_INT32 actualLen = outputLength;
    return ChaosCngHashOneShot(BCRYPT_SHA1_ALGORITHM, input, inputLength,
                               output, &actualLen) ? actualLen : -1;
}

CHAOS_IL2CPP_INT32 ChaosSha256Hash(
    const CHAOS_IL2CPP_UINT8* input,
    CHAOS_IL2CPP_INT32 inputLength,
    CHAOS_IL2CPP_UINT8* output,
    CHAOS_IL2CPP_INT32 outputLength) noexcept
{
    CHAOS_IL2CPP_INT32 actualLen = outputLength;
    return ChaosCngHashOneShot(BCRYPT_SHA256_ALGORITHM, input, inputLength,
                               output, &actualLen) ? actualLen : -1;
}

CHAOS_IL2CPP_INT32 ChaosSha384Hash(
    const CHAOS_IL2CPP_UINT8* input,
    CHAOS_IL2CPP_INT32 inputLength,
    CHAOS_IL2CPP_UINT8* output,
    CHAOS_IL2CPP_INT32 outputLength) noexcept
{
    CHAOS_IL2CPP_INT32 actualLen = outputLength;
    return ChaosCngHashOneShot(BCRYPT_SHA384_ALGORITHM, input, inputLength,
                               output, &actualLen) ? actualLen : -1;
}

CHAOS_IL2CPP_INT32 ChaosSha512Hash(
    const CHAOS_IL2CPP_UINT8* input,
    CHAOS_IL2CPP_INT32 inputLength,
    CHAOS_IL2CPP_UINT8* output,
    CHAOS_IL2CPP_INT32 outputLength) noexcept
{
    CHAOS_IL2CPP_INT32 actualLen = outputLength;
    return ChaosCngHashOneShot(BCRYPT_SHA512_ALGORITHM, input, inputLength,
                               output, &actualLen) ? actualLen : -1;
}

// ── HMAC (keyed-hash) ──────────────────────────────────────────────
CHAOS_IL2CPP_INT32 ChaosHmacSha256(
    const CHAOS_IL2CPP_UINT8* key,
    CHAOS_IL2CPP_INT32 keyLength,
    const CHAOS_IL2CPP_UINT8* input,
    CHAOS_IL2CPP_INT32 inputLength,
    CHAOS_IL2CPP_UINT8* output,
    CHAOS_IL2CPP_INT32 outputLength) noexcept
{
    if (key == nullptr || input == nullptr || output == nullptr)
        return -1;

    BCRYPT_ALG_HANDLE hAlg = nullptr;
    NTSTATUS status = BCryptOpenAlgorithmProvider(&hAlg, BCRYPT_SHA256_ALGORITHM,
                                                   nullptr, BCRYPT_ALG_HANDLE_HMAC_FLAG);
    if (!ChaosCngSuccess(status)) return -1;

    // Get hash length
    ULONG hashLen = 0;
    ULONG resultSize = 0;
    BCryptGetProperty(hAlg, BCRYPT_HASH_LENGTH,
                      reinterpret_cast<PUCHAR>(&hashLen), sizeof(hashLen),
                      &resultSize, 0);

    if (outputLength < static_cast<CHAOS_IL2CPP_INT32>(hashLen)) {
        BCryptCloseAlgorithmProvider(hAlg, 0);
        return -1;
    }

    // Compute HMAC using BCryptHash (one-shot HMAC)
    status = BCryptHash(hAlg, key, static_cast<ULONG>(keyLength),
                        const_cast<PUCHAR>(input), static_cast<ULONG>(inputLength),
                        output, hashLen);
    BCryptCloseAlgorithmProvider(hAlg, 0);

    return ChaosCngSuccess(status) ? static_cast<CHAOS_IL2CPP_INT32>(hashLen) : -1;
}

// ── AES encryption (CBC mode) ──────────────────────────────────────
CHAOS_IL2CPP_INT32 ChaosAesEncryptCbc(
    const CHAOS_IL2CPP_UINT8* key,
    CHAOS_IL2CPP_INT32 keyLength,
    const CHAOS_IL2CPP_UINT8* iv,
    CHAOS_IL2CPP_INT32 ivLength,
    const CHAOS_IL2CPP_UINT8* input,
    CHAOS_IL2CPP_INT32 inputLength,
    CHAOS_IL2CPP_UINT8* output,
    CHAOS_IL2CPP_INT32 outputLength) noexcept
{
    if (key == nullptr || iv == nullptr || input == nullptr || output == nullptr)
        return -1;

    BCRYPT_ALG_HANDLE hAlg = nullptr;
    NTSTATUS status = BCryptOpenAlgorithmProvider(&hAlg, BCRYPT_AES_ALGORITHM,
                                                   nullptr, 0);
    if (!ChaosCngSuccess(status)) return -1;

    // Set CBC chaining mode
    status = BCryptSetProperty(hAlg, BCRYPT_CHAINING_MODE,
                               reinterpret_cast<PUCHAR>(const_cast<PWSTR>(BCRYPT_CHAIN_MODE_CBC)),
                               static_cast<ULONG>((wcslen(BCRYPT_CHAIN_MODE_CBC) + 1) * sizeof(wchar_t)),
                               BCRYPT_SET_PROPERTY_FLAG);
    if (!ChaosCngSuccess(status)) {
        BCryptCloseAlgorithmProvider(hAlg, 0);
        return -1;
    }

    // Get key object size
    ULONG keyObjSize = 0;
    ULONG resultSize = 0;
    BCryptGetProperty(hAlg, BCRYPT_OBJECT_LENGTH,
                      reinterpret_cast<PUCHAR>(&keyObjSize), sizeof(keyObjSize),
                      &resultSize, 0);

    auto keyObj = std::make_unique<CHAOS_IL2CPP_UINT8[]>(keyObjSize);
    if (!keyObj) { BCryptCloseAlgorithmProvider(hAlg, 0); return -1; }

    BCRYPT_KEY_HANDLE hKey = nullptr;
    status = BCryptGenerateSymmetricKey(hAlg, &hKey, keyObj.get(),
                                         static_cast<ULONG>(keyObjSize),
                                         const_cast<PUCHAR>(key),
                                         static_cast<ULONG>(keyLength), 0);
    if (!ChaosCngSuccess(status)) {
        BCryptCloseAlgorithmProvider(hAlg, 0);
        return -1;
    }

    // Determine output size
    ULONG cipherLen = 0;
    status = BCryptEncrypt(hKey, const_cast<PUCHAR>(input),
                           static_cast<ULONG>(inputLength),
                           nullptr, const_cast<PUCHAR>(iv),
                           static_cast<ULONG>(ivLength),
                           nullptr, 0, &cipherLen, BCRYPT_BLOCK_PADDING);
    if (!ChaosCngSuccess(status) || static_cast<CHAOS_IL2CPP_INT32>(cipherLen) > outputLength) {
        BCryptDestroyKey(hKey);
        BCryptCloseAlgorithmProvider(hAlg, 0);
        return -1;
    }

    status = BCryptEncrypt(hKey, const_cast<PUCHAR>(input),
                           static_cast<ULONG>(inputLength),
                           nullptr, const_cast<PUCHAR>(iv),
                           static_cast<ULONG>(ivLength),
                           output, cipherLen, &cipherLen, BCRYPT_BLOCK_PADDING);

    BCryptDestroyKey(hKey);
    BCryptCloseAlgorithmProvider(hAlg, 0);

    return ChaosCngSuccess(status) ? static_cast<CHAOS_IL2CPP_INT32>(cipherLen) : -1;
}

CHAOS_IL2CPP_INT32 ChaosAesDecryptCbc(
    const CHAOS_IL2CPP_UINT8* key,
    CHAOS_IL2CPP_INT32 keyLength,
    const CHAOS_IL2CPP_UINT8* iv,
    CHAOS_IL2CPP_INT32 ivLength,
    const CHAOS_IL2CPP_UINT8* input,
    CHAOS_IL2CPP_INT32 inputLength,
    CHAOS_IL2CPP_UINT8* output,
    CHAOS_IL2CPP_INT32 outputLength) noexcept
{
    if (key == nullptr || iv == nullptr || input == nullptr || output == nullptr)
        return -1;

    BCRYPT_ALG_HANDLE hAlg = nullptr;
    NTSTATUS status = BCryptOpenAlgorithmProvider(&hAlg, BCRYPT_AES_ALGORITHM,
                                                   nullptr, 0);
    if (!ChaosCngSuccess(status)) return -1;

    status = BCryptSetProperty(hAlg, BCRYPT_CHAINING_MODE,
                               reinterpret_cast<PUCHAR>(const_cast<PWSTR>(BCRYPT_CHAIN_MODE_CBC)),
                               static_cast<ULONG>((wcslen(BCRYPT_CHAIN_MODE_CBC) + 1) * sizeof(wchar_t)),
                               BCRYPT_SET_PROPERTY_FLAG);
    if (!ChaosCngSuccess(status)) {
        BCryptCloseAlgorithmProvider(hAlg, 0);
        return -1;
    }

    ULONG keyObjSize = 0;
    ULONG resultSize = 0;
    BCryptGetProperty(hAlg, BCRYPT_OBJECT_LENGTH,
                      reinterpret_cast<PUCHAR>(&keyObjSize), sizeof(keyObjSize),
                      &resultSize, 0);

    auto keyObj = std::make_unique<CHAOS_IL2CPP_UINT8[]>(keyObjSize);
    if (!keyObj) { BCryptCloseAlgorithmProvider(hAlg, 0); return -1; }

    BCRYPT_KEY_HANDLE hKey = nullptr;
    status = BCryptGenerateSymmetricKey(hAlg, &hKey, keyObj.get(),
                                         static_cast<ULONG>(keyObjSize),
                                         const_cast<PUCHAR>(key),
                                         static_cast<ULONG>(keyLength), 0);
    if (!ChaosCngSuccess(status)) {
        BCryptCloseAlgorithmProvider(hAlg, 0);
        return -1;
    }

    ULONG plainLen = 0;
    status = BCryptDecrypt(hKey, const_cast<PUCHAR>(input),
                           static_cast<ULONG>(inputLength),
                           nullptr, const_cast<PUCHAR>(iv),
                           static_cast<ULONG>(ivLength),
                           nullptr, 0, &plainLen, BCRYPT_BLOCK_PADDING);
    if (!ChaosCngSuccess(status) || static_cast<CHAOS_IL2CPP_INT32>(plainLen) > outputLength) {
        BCryptDestroyKey(hKey);
        BCryptCloseAlgorithmProvider(hAlg, 0);
        return -1;
    }

    status = BCryptDecrypt(hKey, const_cast<PUCHAR>(input),
                           static_cast<ULONG>(inputLength),
                           nullptr, const_cast<PUCHAR>(iv),
                           static_cast<ULONG>(ivLength),
                           output, plainLen, &plainLen, BCRYPT_BLOCK_PADDING);

    BCryptDestroyKey(hKey);
    BCryptCloseAlgorithmProvider(hAlg, 0);

    return ChaosCngSuccess(status) ? static_cast<CHAOS_IL2CPP_INT32>(plainLen) : -1;
}

}  // extern "C"
}  // namespace chaos::il2cpp::runtime_core
