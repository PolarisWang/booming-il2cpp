// ABI exports: extern "C" linkage for managed/NativeAOT callability.

// crypto_stubs.cpp — Cryptography stub implementations
// Uses Windows CNG (BCrypt) APIs for native crypto operations.
// On Linux, stubs are provided — the prebuilt libchaos_runtime_core.a
// contains the real Windows BCrypt implementations.
// All functions receive managed CHAOS_IL2CPP_INTPTR (byte[] pointers)
// and use get_managed_array() to access raw data.

#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <memory>

#include "generated_code_compat.h"
#include "runtime_stubs/stub_common.h"
#include "runtime_stubs/crypto_stubs.h"
#include "runtime_core.h"
#include <chaos/pal/pal_random.h>

#ifdef _WIN32
#include <Windows.h>
#include <bcrypt.h>
#pragma comment(lib, "bcrypt.lib")
#endif

namespace chaos::il2cpp::runtime_core {
extern "C" {

#ifdef _WIN32

// SHA-3 BCrypt algorithm identifiers (not available in older Windows SDKs)
#ifndef BCRYPT_SHA3_256_ALGORITHM
#define BCRYPT_SHA3_256_ALGORITHM L"SHA3_256"
#endif
#ifndef BCRYPT_SHA3_384_ALGORITHM
#define BCRYPT_SHA3_384_ALGORITHM L"SHA3_384"
#endif
#ifndef BCRYPT_SHA3_512_ALGORITHM
#define BCRYPT_SHA3_512_ALGORITHM L"SHA3_512"
#endif

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
        chaos::il2cpp::pal::PalRandomBytes(data + offset, static_cast<size_t>(count));
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
    if (inArr == nullptr) return 0;
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
        status = BCryptHashData(hHash, (PUCHAR)(inData),
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

// ── SHA-3 family one-shot hashes ──────────────────────────────────────
CHAOS_IL2CPP_INTPTR ChaosSha3_256Hash(CHAOS_IL2CPP_INTPTR data) noexcept
{
    return ChaosCngHash(BCRYPT_SHA3_256_ALGORITHM, data);
}

CHAOS_IL2CPP_INTPTR ChaosSha3_384Hash(CHAOS_IL2CPP_INTPTR data) noexcept
{
    return ChaosCngHash(BCRYPT_SHA3_384_ALGORITHM, data);
}

CHAOS_IL2CPP_INTPTR ChaosSha3_512Hash(CHAOS_IL2CPP_INTPTR data) noexcept
{
    return ChaosCngHash(BCRYPT_SHA3_512_ALGORITHM, data);
}

// ── HMAC helper (BCrypt) ──────────────────────────────────────────────
static CHAOS_IL2CPP_INTPTR ChaosCngHmac(
    LPCWSTR algorithm,
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
    NTSTATUS status = BCryptOpenAlgorithmProvider(&hAlg, algorithm,
        nullptr, BCRYPT_ALG_HANDLE_HMAC_FLAG);
    if (!ChaosCngOk(status)) return 0;

    ULONG hashLen = 0;
    ULONG resultSize = 0;
    BCryptGetProperty(hAlg, BCRYPT_HASH_LENGTH,
        reinterpret_cast<PUCHAR>(&hashLen), sizeof(hashLen), &resultSize, 0);

    auto buf = std::make_unique<UCHAR[]>(hashLen);
    status = BCryptHash(hAlg,
        static_cast<PUCHAR>(const_cast<void*>(static_cast<const void*>(keyData))),
        static_cast<ULONG>(keyArr->length * sizeof(CHAOS_IL2CPP_INTPTR)),
        static_cast<PUCHAR>(const_cast<void*>(static_cast<const void*>(dataBuf))),
        static_cast<ULONG>(dataArr->length * sizeof(CHAOS_IL2CPP_INTPTR)),
        buf.get(), hashLen);

    BCryptCloseAlgorithmProvider(hAlg, 0);
    if (!ChaosCngOk(status)) return 0;

    // TODO: Allocate managed byte[] and return
    return 0;
}

// ── HMAC-SHA1/256/384/512 ─────────────────────────────────────────────
CHAOS_IL2CPP_INTPTR ChaosHmacSha1(CHAOS_IL2CPP_INTPTR key, CHAOS_IL2CPP_INTPTR data) noexcept
{
    return ChaosCngHmac(BCRYPT_SHA1_ALGORITHM, key, data);
}

CHAOS_IL2CPP_INTPTR ChaosHmacSha256(
    CHAOS_IL2CPP_INTPTR key,
    CHAOS_IL2CPP_INTPTR data) noexcept
{
    return ChaosCngHmac(BCRYPT_SHA256_ALGORITHM, key, data);
}

CHAOS_IL2CPP_INTPTR ChaosHmacSha384(CHAOS_IL2CPP_INTPTR key, CHAOS_IL2CPP_INTPTR data) noexcept
{
    return ChaosCngHmac(BCRYPT_SHA384_ALGORITHM, key, data);
}

CHAOS_IL2CPP_INTPTR ChaosHmacSha512(CHAOS_IL2CPP_INTPTR key, CHAOS_IL2CPP_INTPTR data) noexcept
{
    return ChaosCngHmac(BCRYPT_SHA512_ALGORITHM, key, data);
}

// ── HMAC-SHA3 family ──────────────────────────────────────────────────
CHAOS_IL2CPP_INTPTR ChaosHmacSha3_256(CHAOS_IL2CPP_INTPTR key, CHAOS_IL2CPP_INTPTR data) noexcept
{
    return ChaosCngHmac(BCRYPT_SHA3_256_ALGORITHM, key, data);
}

CHAOS_IL2CPP_INTPTR ChaosHmacSha3_384(CHAOS_IL2CPP_INTPTR key, CHAOS_IL2CPP_INTPTR data) noexcept
{
    return ChaosCngHmac(BCRYPT_SHA3_384_ALGORITHM, key, data);
}

CHAOS_IL2CPP_INTPTR ChaosHmacSha3_512(CHAOS_IL2CPP_INTPTR key, CHAOS_IL2CPP_INTPTR data) noexcept
{
    return ChaosCngHmac(BCRYPT_SHA3_512_ALGORITHM, key, data);
}

// ── MD5 ──
CHAOS_IL2CPP_INTPTR ChaosMd5Hash(CHAOS_IL2CPP_INTPTR data) noexcept
{
    return ChaosCngHash(BCRYPT_MD5_ALGORITHM, data);
}

// ── HMACMD5 ──
CHAOS_IL2CPP_INTPTR ChaosHmacMd5(CHAOS_IL2CPP_INTPTR key, CHAOS_IL2CPP_INTPTR data) noexcept
{
    return ChaosCngHmac(BCRYPT_MD5_ALGORITHM, key, data);
}

// ── RNG GetBytes(int) ──
CHAOS_IL2CPP_INTPTR ChaosCngGetBytes(CHAOS_IL2CPP_INT32 count) noexcept
{
    if (count <= 0) return 0;
    // TODO: Allocate managed byte[], fill with BCryptGenRandom, return
    return 0;
}

// ── RNG Fill(byte[]) ──
void ChaosCngFill(CHAOS_IL2CPP_INTPTR buffer) noexcept
{
    if (buffer == 0) return;
    auto* arr = get_managed_array_mut(buffer);
    if (arr == nullptr || arr->length <= 0) return;
    auto* data = reinterpret_cast<CHAOS_IL2CPP_UINT8*>(accessor_get_elements(arr));
    if (data == nullptr) return;
    BCryptGenRandom(nullptr, data, static_cast<ULONG>(arr->length),
                    BCRYPT_USE_SYSTEM_PREFERRED_RNG);
}

#else  // !_WIN32 — OpenSSL-based implementations

#include <openssl/evp.h>
#include <openssl/hmac.h>
#include <openssl/rand.h>

// ── Managed byte[] allocation helper ─────────────────────────────
// Allocates a managed byte[] of the given length via the GC,
// returns the handle (0 on failure).  The byte content is zeroed.
static CHAOS_IL2CPP_INTPTR alloc_byte_array(CHAOS_IL2CPP_SIZE length) noexcept
{
    const auto alloc_size = sizeof(ManagedArrayAccessor) + length;
    auto* storage = static_cast<CHAOS_IL2CPP_UINT8*>(GcAllocateAtomic(alloc_size));
    if (storage == nullptr) return 0;
    auto* arr = reinterpret_cast<ManagedArrayAccessor*>(storage);
    arr->element_type_shape = 2;  // value type
    arr->length = static_cast<CHAOS_IL2CPP_INTPTR>(length);
    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(arr);
}

// ── OpenSSL one-shot hash helper ───────────────────────────────────
// Computes a hash over the managed byte[] data using OpenSSL EVP.
// Returns managed byte[] containing the hash, or 0 on failure.
static CHAOS_IL2CPP_INTPTR ChaosOpenSslHash(const EVP_MD* md, CHAOS_IL2CPP_INTPTR data) noexcept
{
    if (data == 0 || md == nullptr) return 0;
    auto* inArr = get_managed_array(data);
    if (inArr == nullptr) return 0;
    // Allow empty input — hash of empty data has well-defined output.
    auto* inData = reinterpret_cast<const CHAOS_IL2CPP_UINT8*>(accessor_get_elements(inArr));
    if (inData == nullptr) return 0;

    auto* ctx = EVP_MD_CTX_new();
    if (ctx == nullptr) return 0;

    unsigned int hashLen = 0;
    uint8_t hashBuf[EVP_MAX_MD_SIZE] = {0};
    int ok = EVP_DigestInit_ex(ctx, md, nullptr)
          && EVP_DigestUpdate(ctx, inData, static_cast<size_t>(inArr->length))
          && EVP_DigestFinal_ex(ctx, hashBuf, &hashLen);
    EVP_MD_CTX_free(ctx);

    if (!ok) return 0;

    auto result = alloc_byte_array(static_cast<CHAOS_IL2CPP_SIZE>(hashLen));
    if (result != 0)
    {
        auto* outArr = get_managed_array_mut(result);
        auto* outData = reinterpret_cast<CHAOS_IL2CPP_UINT8*>(accessor_get_elements(outArr));
        std::memcpy(outData, hashBuf, static_cast<size_t>(hashLen));
    }
    return result;
}

// ── OpenSSL one-shot HMAC helper ───────────────────────────────────
// Computes HMAC over managed byte[] key and data using OpenSSL HMAC.
// Returns managed byte[] containing the HMAC, or 0 on failure.
static CHAOS_IL2CPP_INTPTR ChaosOpenSslHmac(const EVP_MD* md,
    CHAOS_IL2CPP_INTPTR key, CHAOS_IL2CPP_INTPTR data) noexcept
{
    if (key == 0 || data == 0 || md == nullptr) { return 0; }
    auto* keyArr = get_managed_array(key);
    auto* dataArr = get_managed_array(data);
    if (keyArr == nullptr || dataArr == nullptr) { return 0; }
    // Allow empty key/data — HMAC of empty input has well-defined output.
    auto* keyData = reinterpret_cast<const CHAOS_IL2CPP_UINT8*>(accessor_get_elements(keyArr));
    auto* dataBuf = reinterpret_cast<const CHAOS_IL2CPP_UINT8*>(accessor_get_elements(dataArr));
    if (keyData == nullptr || dataBuf == nullptr) { return 0; }

    uint8_t resultBuf[EVP_MAX_MD_SIZE] = {0};
    unsigned int resultLen = 0;

    auto* ctx = HMAC_CTX_new();
    if (ctx == nullptr) return 0;

    int ok = HMAC_Init_ex(ctx, keyData, static_cast<int>(keyArr->length), md, nullptr)
          && HMAC_Update(ctx, dataBuf, static_cast<size_t>(dataArr->length))
          && HMAC_Final(ctx, resultBuf, &resultLen);
    HMAC_CTX_free(ctx);

    if (!ok) { return 0; }
    auto result = alloc_byte_array(static_cast<CHAOS_IL2CPP_SIZE>(resultLen));
    if (result != 0)
    {
        auto* outArr = get_managed_array_mut(result);
        auto* outData = reinterpret_cast<CHAOS_IL2CPP_UINT8*>(accessor_get_elements(outArr));
        std::memcpy(outData, resultBuf, static_cast<size_t>(resultLen));
    }
    return result;
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
    CHAOS_IL2CPP_INT32 avail = static_cast<CHAOS_IL2CPP_INT32>(arr->length) - offset;
    if (avail <= 0) return;
    if (count > avail) count = avail;
    RAND_bytes(data + offset, static_cast<int>(count));
}

// ── SHA1/256/384/512 One-shot Hashes ──────────────────────────────
CHAOS_IL2CPP_INTPTR ChaosSha1Hash(CHAOS_IL2CPP_INTPTR data) noexcept
{
    return ChaosOpenSslHash(EVP_sha1(), data);
}

CHAOS_IL2CPP_INTPTR ChaosSha256Hash(CHAOS_IL2CPP_INTPTR data) noexcept
{
    return ChaosOpenSslHash(EVP_sha256(), data);
}

CHAOS_IL2CPP_INTPTR ChaosSha384Hash(CHAOS_IL2CPP_INTPTR data) noexcept
{
    return ChaosOpenSslHash(EVP_sha384(), data);
}

CHAOS_IL2CPP_INTPTR ChaosSha512Hash(CHAOS_IL2CPP_INTPTR data) noexcept
{
    return ChaosOpenSslHash(EVP_sha512(), data);
}

// ── SHA-3 family one-shot hashes ──────────────────────────────────
CHAOS_IL2CPP_INTPTR ChaosSha3_256Hash(CHAOS_IL2CPP_INTPTR data) noexcept
{
    return ChaosOpenSslHash(EVP_sha3_256(), data);
}

CHAOS_IL2CPP_INTPTR ChaosSha3_384Hash(CHAOS_IL2CPP_INTPTR data) noexcept
{
    return ChaosOpenSslHash(EVP_sha3_384(), data);
}

CHAOS_IL2CPP_INTPTR ChaosSha3_512Hash(CHAOS_IL2CPP_INTPTR data) noexcept
{
    return ChaosOpenSslHash(EVP_sha3_512(), data);
}

// ── HMAC family ───────────────────────────────────────────────────
CHAOS_IL2CPP_INTPTR ChaosHmacSha1(CHAOS_IL2CPP_INTPTR key, CHAOS_IL2CPP_INTPTR data) noexcept
{
    return ChaosOpenSslHmac(EVP_sha1(), key, data);
}

CHAOS_IL2CPP_INTPTR ChaosHmacSha256(CHAOS_IL2CPP_INTPTR key, CHAOS_IL2CPP_INTPTR data) noexcept
{
    return ChaosOpenSslHmac(EVP_sha256(), key, data);
}

CHAOS_IL2CPP_INTPTR ChaosHmacSha384(CHAOS_IL2CPP_INTPTR key, CHAOS_IL2CPP_INTPTR data) noexcept
{
    return ChaosOpenSslHmac(EVP_sha384(), key, data);
}

CHAOS_IL2CPP_INTPTR ChaosHmacSha512(CHAOS_IL2CPP_INTPTR key, CHAOS_IL2CPP_INTPTR data) noexcept
{
    return ChaosOpenSslHmac(EVP_sha512(), key, data);
}

CHAOS_IL2CPP_INTPTR ChaosHmacSha3_256(CHAOS_IL2CPP_INTPTR key, CHAOS_IL2CPP_INTPTR data) noexcept
{
    return ChaosOpenSslHmac(EVP_sha3_256(), key, data);
}

CHAOS_IL2CPP_INTPTR ChaosHmacSha3_384(CHAOS_IL2CPP_INTPTR key, CHAOS_IL2CPP_INTPTR data) noexcept
{
    return ChaosOpenSslHmac(EVP_sha3_384(), key, data);
}

CHAOS_IL2CPP_INTPTR ChaosHmacSha3_512(CHAOS_IL2CPP_INTPTR key, CHAOS_IL2CPP_INTPTR data) noexcept
{
    return ChaosOpenSslHmac(EVP_sha3_512(), key, data);
}

// ── MD5 one-shot hash ────────────────────────────────────────────────
CHAOS_IL2CPP_INTPTR ChaosMd5Hash(CHAOS_IL2CPP_INTPTR data) noexcept
{
    return ChaosOpenSslHash(EVP_md5(), data);
}

// ── HMACMD5 one-shot HMAC ────────────────────────────────────────────
CHAOS_IL2CPP_INTPTR ChaosHmacMd5(CHAOS_IL2CPP_INTPTR key, CHAOS_IL2CPP_INTPTR data) noexcept
{
    return ChaosOpenSslHmac(EVP_md5(), key, data);
}

// ── RNG GetBytes(int) -> byte[] ──────────────────────────────────────
// Allocates a managed byte[] of the requested size and fills it with
// cryptographically strong random bytes.
CHAOS_IL2CPP_INTPTR ChaosCngGetBytes(CHAOS_IL2CPP_INT32 count) noexcept
{
    if (count <= 0) return 0;
    auto result = alloc_byte_array(static_cast<CHAOS_IL2CPP_SIZE>(count));
    if (result != 0)
    {
        auto* arr = get_managed_array_mut(result);
        auto* data = reinterpret_cast<CHAOS_IL2CPP_UINT8*>(accessor_get_elements(arr));
        RAND_bytes(data, static_cast<int>(count));
    }
    return result;
}

// ── RNG Fill(byte[]) ────────────────────────────────────────────────
// Fills an existing managed byte[] with cryptographically strong random bytes.
void ChaosCngFill(CHAOS_IL2CPP_INTPTR buffer) noexcept
{
    if (buffer == 0) return;
    auto* arr = get_managed_array_mut(buffer);
    if (arr == nullptr || arr->length <= 0) return;
    auto* data = reinterpret_cast<CHAOS_IL2CPP_UINT8*>(accessor_get_elements(arr));
    if (data == nullptr) return;
    RAND_bytes(data, static_cast<int>(arr->length));
}

#endif

// ── AES Create (stub — returns null sentinel) ────────────────────
// Aes.Create() returns a new AesCryptoServiceProvider instance.
// Proper implementation requires managed object allocation.
CHAOS_IL2CPP_INTPTR ChaosAesCreate() noexcept
{
    return 0;
}

}  // extern "C"
}  // namespace chaos::il2cpp::runtime_core
