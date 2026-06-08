// ABI exports: extern "C" linkage for managed/NativeAOT callability.

// crypto_stubs.cpp — Cryptography stub implementations
// Uses Windows CNG (BCrypt) APIs for native crypto operations.
// All functions receive managed CHAOS_IL2CPP_INTPTR (byte[] pointers)
// and use get_managed_array() to access raw data.

#include <cstdlib>
#include <cstring>
#include <memory>
#include <Windows.h>
#include <bcrypt.h>

#include "generated_code_compat.h"
#include "runtime_stubs/stub_common.h"
#include "runtime_stubs/crypto_stubs.h"
#include "runtime_core.h"

#pragma comment(lib, "bcrypt.lib")

namespace chaos::il2cpp::runtime_core {
extern "C" {

// ── BCrypt NTSTATUS helper ────────────────────────────────────────
static inline bool ChaosCngOk(NTSTATUS status) noexcept {
    return status >= 0;
}

// ── RandomNumberGenerator: Fill(byte[], int, int) ─────────────────
// Fills buffer[offset..offset+count-1] with cryptographically strong random bytes.
void ChaosCngFillRandom(CHAOS_IL2CPP_INTPTR buffer, CHAOS_IL2CPP_INT32 offset, CHAOS_IL2CPP_INT32 count) noexcept
{
    if (buffer == 0 || count <= 0) return;
    auto* arr = get_managed_array_mut(buffer);
    if (arr == nullptr || arr->length <= 0) return;
    auto* data = reinterpret_cast<CHAOS_IL2CPP_UINT8*>(accessor_get_elements(arr));
    if (data == nullptr) return;
    // Clamp to array bounds
    CHAOS_IL2CPP_INT32 avail = static_cast<CHAOS_IL2CPP_INT32>(arr->length) - offset;
    if (avail <= 0) return;
    if (count > avail) count = avail;
    NTSTATUS status = BCryptGenRandom(nullptr, data + offset, static_cast<ULONG>(count),
                                      BCRYPT_USE_SYSTEM_PREFERRED_RNG);
    if (!ChaosCngOk(status)) {
        // Fallback: PAL random
        chaos::il2cpp::pal::PalRandomFill(data + offset, static_cast<size_t>(count));
    }
}

// ── Hash algorithm helper ─────────────────────────────────────────
// Computes a hash over input managed byte[] using BCrypt.
// Returns managed byte[] containing the hash, or 0 on failure.
static CHAOS_IL2CPP_INTPTR ChaosCngHash(
    LPCWSTR algorithm,
    CHAOS_IL2CPP_INTPTR inputData)
{
    if (inputData == 0) return 0;
    auto* inArr = get_managed_array(inputData);
    if (inArr == nullptr || inArr->length <= 0) return 0;
    auto* inData = accessor_get_elements(inArr);
    if (inData == nullptr) return 0;

    BCRYPT_ALG_HANDLE hAlg = nullptr;
    NTSTATUS status = BCryptOpenAlgorithmProvider(&hAlg, algorithm, nullptr, 0);
    if (!ChaosCngOk(status)) return 0;

    // Get hash length
    ULONG hashLen = 0;
    ULONG resultSize = 0;
    BCryptGetProperty(hAlg, BCRYPT_HASH_LENGTH,
        reinterpret_cast<PUCHAR>(&hashLen), sizeof(hashLen), &resultSize, 0);

    // Get hash object size
    ULONG hashObjSize = 0;
    BCryptGetProperty(hAlg, BCRYPT_OBJECT_LENGTH,
        reinterpret_cast<PUCHAR>(&hashObjSize), sizeof(hashObjSize), &resultSize, 0);

    auto hashObj = std::make_unique<UCHAR[]>(hashObjSize);
    auto hashBuf = std::make_unique<UCHAR[]>(hashLen);

    BCRYPT_HASH_HANDLE hHash = nullptr;
    status = BCryptCreateHash(hAlg, &hHash, hashObj.get(), hashObjSize, nullptr, 0, 0);
    if (ChaosCngOk(status)) {
        status = BCryptHashData(hHash, const_cast<PUCHAR>(inData),
            static_cast<ULONG>(inArr->length * sizeof(CHAOS_IL2CPP_INTPTR)), 0);
        if (ChaosCngOk(status)) {
            status = BCryptFinishHash(hHash, hashBuf.get(), hashLen, 0);
        }
        BCryptDestroyHash(hHash);
    }
    BCryptCloseAlgorithmProvider(hAlg, 0);

    if (!ChaosCngOk(status)) return 0;

    // Allocate output managed byte[] and copy hash
    // For now, return 0 as sentinel (managed-side allocation needed)
    // TODO: Allocate managed byte[] of hashLen and copy hashBuf
    // This requires GC allocation which is complex from native code.
    // For test verification, returning 0 (null) causes the test to fail
    // gracefully rather than CHAOS_IL2CPP_FAIL/abort.
    return 0;
}

// ── SHA1/256/384/512 One-shot Hashes ──────────────────────────────
CHAOS_IL2CPP_INTPTR ChaosSha1Hash(CHAOS_IL2CPP_INTPTR data) noexcept
{
    return ChaosCngHash(BCRYPT_SHA1_ALGORITHM, data);
}

CHAOS_IL2CPP_INTPTR ChaosSha256Hash(CHAOS_IL2CPP_INTPTR data) noexcept
{
    return ChaosCngHash(BCRYPT_SHA256_ALGORITHM, data);
}

CHAOS_IL2CPP_INTPTR ChaosSha384Hash(CHAOS_IL2CPP_INTPTR data) noexcept
{
    return ChaosCngHash(BCRYPT_SHA384_ALGORITHM, data);
}

CHAOS_IL2CPP_INTPTR ChaosSha512Hash(CHAOS_IL2CPP_INTPTR data) noexcept
{
    return ChaosCngHash(BCRYPT_SHA512_ALGORITHM, data);
}

// ── HMAC-SHA256 ───────────────────────────────────────────────────
CHAOS_IL2CPP_INTPTR ChaosHmacSha256(
    CHAOS_IL2CPP_INTPTR key,
    CHAOS_IL2CPP_INTPTR data) noexcept
{
    if (key == 0 || data == 0) return 0;
    auto* keyArr = get_managed_array(key);
    auto* dataArr = get_managed_array(data);
    if (keyArr == nullptr || dataArr == nullptr) return 0;
    auto* keyData = accessor_get_elements(keyArr);
    auto* dataBuf = accessor_get_elements(dataArr);
    if (keyData == nullptr || dataBuf == nullptr) return 0;

    BCRYPT_ALG_HANDLE hAlg = nullptr;
    NTSTATUS status = BCryptOpenAlgorithmProvider(&hAlg, BCRYPT_SHA256_ALGORITHM,
        nullptr, BCRYPT_ALG_HANDLE_HMAC_FLAG);
    if (!ChaosCngOk(status)) return 0;

    ULONG hashLen = 0;
    ULONG resultSize = 0;
    BCryptGetProperty(hAlg, BCRYPT_HASH_LENGTH,
        reinterpret_cast<PUCHAR>(&hashLen), sizeof(hashLen), &resultSize, 0);

    auto buf = std::make_unique<UCHAR[]>(hashLen);
    status = BCryptHash(hAlg,
        const_cast<PUCHAR>(keyData), static_cast<ULONG>(keyArr->length * sizeof(CHAOS_IL2CPP_INTPTR)),
        const_cast<PUCHAR>(dataBuf), static_cast<ULONG>(dataArr->length * sizeof(CHAOS_IL2CPP_INTPTR)),
        buf.get(), hashLen);

    BCryptCloseAlgorithmProvider(hAlg, 0);
    if (!ChaosCngOk(status)) return 0;

    // TODO: Allocate managed byte[] and return
    return 0;
}

// ── AES Create (stub — returns null sentinel) ────────────────────
// Aes.Create() returns a new AesCryptoServiceProvider instance.
// Proper implementation requires managed object allocation.
CHAOS_IL2CPP_INTPTR ChaosAesCreate() noexcept
{
    return 0;
}

}  // extern "C"
}  // namespace chaos::il2cpp::runtime_core
