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

#endif  // closes the main Windows vs OpenSSL implementation block

// ── BCrypt P/Invoke stubs (Windows: delegate to bcrypt.dll) ──────────
// These stubs are called from the P/Invoke routing path in ChaosExternalRuntimeFallback
// when managed Interop+BCrypt methods are encountered.
#ifdef _WIN32
#include <Windows.h>
#include <bcrypt.h>
#pragma comment(lib, "bcrypt.lib")

// Wrapper: extract a CHAOS_IL2CPP_INTPTR argument as a wide string pointer.
// The managed Interop+BCrypt passes string arguments as IntPtr (wchar_t*).
static inline LPCWSTR _AsWideStr(CHAOS_IL2CPP_INTPTR ptr) noexcept {
    return reinterpret_cast<LPCWSTR>(ptr);
}

CHAOS_IL2CPP_INT32 ChaosBCryptOpenAlgorithmProvider(
    CHAOS_IL2CPP_INTPTR phAlgorithm,
    CHAOS_IL2CPP_INTPTR pszAlgId,
    CHAOS_IL2CPP_INTPTR pszImplementation,
    CHAOS_IL2CPP_INT32 dwFlags) noexcept
{
    auto* phAlg = reinterpret_cast<BCRYPT_ALG_HANDLE*>(phAlgorithm);
    if (phAlg == nullptr) return -1; // STATUS_INVALID_PARAMETER
    NTSTATUS status = BCryptOpenAlgorithmProvider(
        phAlg,
        _AsWideStr(pszAlgId),
        _AsWideStr(pszImplementation),
        static_cast<ULONG>(dwFlags));
    return static_cast<CHAOS_IL2CPP_INT32>(status);
}

CHAOS_IL2CPP_INT32 ChaosBCryptCloseAlgorithmProvider(
    CHAOS_IL2CPP_INTPTR hAlgorithm,
    CHAOS_IL2CPP_INT32 dwFlags) noexcept
{
    auto hAlg = reinterpret_cast<BCRYPT_ALG_HANDLE>(hAlgorithm);
    NTSTATUS status = BCryptCloseAlgorithmProvider(hAlg, static_cast<ULONG>(dwFlags));
    return static_cast<CHAOS_IL2CPP_INT32>(status);
}

CHAOS_IL2CPP_INT32 ChaosBCryptCreateHash(
    CHAOS_IL2CPP_INTPTR hAlgorithm,
    CHAOS_IL2CPP_INTPTR phHash,
    CHAOS_IL2CPP_INTPTR pbHashObject,
    CHAOS_IL2CPP_INT32 cbHashObject,
    CHAOS_IL2CPP_INTPTR pbSecret,
    CHAOS_IL2CPP_INT32 cbSecret,
    CHAOS_IL2CPP_INT32 dwFlags) noexcept
{
    auto hAlg = reinterpret_cast<BCRYPT_ALG_HANDLE>(hAlgorithm);
    auto* phH = reinterpret_cast<BCRYPT_HASH_HANDLE*>(phHash);
    if (phH == nullptr) return -1;
    NTSTATUS status = BCryptCreateHash(
        hAlg, phH,
        reinterpret_cast<PUCHAR>(pbHashObject), static_cast<ULONG>(cbHashObject),
        reinterpret_cast<PUCHAR>(pbSecret), static_cast<ULONG>(cbSecret),
        static_cast<ULONG>(dwFlags));
    return static_cast<CHAOS_IL2CPP_INT32>(status);
}

CHAOS_IL2CPP_INT32 ChaosBCryptDestroyHash(
    CHAOS_IL2CPP_INTPTR hHash) noexcept
{
    auto hH = reinterpret_cast<BCRYPT_HASH_HANDLE>(hHash);
    NTSTATUS status = BCryptDestroyHash(hH);
    return static_cast<CHAOS_IL2CPP_INT32>(status);
}

CHAOS_IL2CPP_INT32 ChaosBCryptHashData(
    CHAOS_IL2CPP_INTPTR hHash,
    CHAOS_IL2CPP_INTPTR pbInput,
    CHAOS_IL2CPP_INT32 cbInput,
    CHAOS_IL2CPP_INT32 dwFlags) noexcept
{
    auto hH = reinterpret_cast<BCRYPT_HASH_HANDLE>(hHash);
    NTSTATUS status = BCryptHashData(
        hH,
        reinterpret_cast<PUCHAR>(pbInput), static_cast<ULONG>(cbInput),
        static_cast<ULONG>(dwFlags));
    return static_cast<CHAOS_IL2CPP_INT32>(status);
}

CHAOS_IL2CPP_INT32 ChaosBCryptFinishHash(
    CHAOS_IL2CPP_INTPTR hHash,
    CHAOS_IL2CPP_INTPTR pbOutput,
    CHAOS_IL2CPP_INT32 cbOutput,
    CHAOS_IL2CPP_INT32 dwFlags) noexcept
{
    auto hH = reinterpret_cast<BCRYPT_HASH_HANDLE>(hHash);
    NTSTATUS status = BCryptFinishHash(
        hH,
        reinterpret_cast<PUCHAR>(pbOutput), static_cast<ULONG>(cbOutput),
        static_cast<ULONG>(dwFlags));
    return static_cast<CHAOS_IL2CPP_INT32>(status);
}

CHAOS_IL2CPP_INT32 ChaosBCryptHash(
    CHAOS_IL2CPP_INTPTR hAlgorithm,
    CHAOS_IL2CPP_INTPTR pbSecret,
    CHAOS_IL2CPP_INT32 cbSecret,
    CHAOS_IL2CPP_INTPTR pbInput,
    CHAOS_IL2CPP_INT32 cbInput,
    CHAOS_IL2CPP_INTPTR pbOutput,
    CHAOS_IL2CPP_INT32 cbOutput) noexcept
{
    auto hAlg = reinterpret_cast<BCRYPT_ALG_HANDLE>(hAlgorithm);
    NTSTATUS status = BCryptHash(
        hAlg,
        reinterpret_cast<PUCHAR>(pbSecret), static_cast<ULONG>(cbSecret),
        reinterpret_cast<PUCHAR>(pbInput), static_cast<ULONG>(cbInput),
        reinterpret_cast<PUCHAR>(pbOutput), static_cast<ULONG>(cbOutput));
    return static_cast<CHAOS_IL2CPP_INT32>(status);
}

CHAOS_IL2CPP_INT32 ChaosBCryptGenerateSymmetricKey(
    CHAOS_IL2CPP_INTPTR hAlgorithm,
    CHAOS_IL2CPP_INTPTR phKey,
    CHAOS_IL2CPP_INTPTR pbKeyObject,
    CHAOS_IL2CPP_INT32 cbKeyObject,
    CHAOS_IL2CPP_INTPTR pbSecret,
    CHAOS_IL2CPP_INT32 cbSecret,
    CHAOS_IL2CPP_INT32 dwFlags) noexcept
{
    auto hAlg = reinterpret_cast<BCRYPT_ALG_HANDLE>(hAlgorithm);
    BCRYPT_KEY_HANDLE* phK = reinterpret_cast<BCRYPT_KEY_HANDLE*>(phKey);
    if (phK == nullptr) return -1;
    NTSTATUS status = BCryptGenerateSymmetricKey(
        hAlg, phK,
        reinterpret_cast<PUCHAR>(pbKeyObject), static_cast<ULONG>(cbKeyObject),
        reinterpret_cast<PUCHAR>(pbSecret), static_cast<ULONG>(cbSecret),
        static_cast<ULONG>(dwFlags));
    return static_cast<CHAOS_IL2CPP_INT32>(status);
}

CHAOS_IL2CPP_INT32 ChaosBCryptDestroyKey(
    CHAOS_IL2CPP_INTPTR hKey) noexcept
{
    auto hK = reinterpret_cast<BCRYPT_KEY_HANDLE>(hKey);
    NTSTATUS status = BCryptDestroyKey(hK);
    return static_cast<CHAOS_IL2CPP_INT32>(status);
}

CHAOS_IL2CPP_INT32 ChaosBCryptEncrypt(
    CHAOS_IL2CPP_INTPTR hKey,
    CHAOS_IL2CPP_INTPTR pbInput,
    CHAOS_IL2CPP_INT32 cbInput,
    CHAOS_IL2CPP_INTPTR pPaddingInfo,
    CHAOS_IL2CPP_INTPTR pbIV,
    CHAOS_IL2CPP_INT32 cbIV,
    CHAOS_IL2CPP_INTPTR pbOutput,
    CHAOS_IL2CPP_INT32 cbOutput,
    CHAOS_IL2CPP_INTPTR pcbResult,
    CHAOS_IL2CPP_INT32 dwFlags) noexcept
{
    auto hK = reinterpret_cast<BCRYPT_KEY_HANDLE>(hKey);
    NTSTATUS status = BCryptEncrypt(
        hK,
        reinterpret_cast<PUCHAR>(pbInput), static_cast<ULONG>(cbInput),
        reinterpret_cast<void*>(pPaddingInfo),
        reinterpret_cast<PUCHAR>(pbIV), static_cast<ULONG>(cbIV),
        reinterpret_cast<PUCHAR>(pbOutput), static_cast<ULONG>(cbOutput),
        reinterpret_cast<ULONG*>(pcbResult),
        static_cast<ULONG>(dwFlags));
    return static_cast<CHAOS_IL2CPP_INT32>(status);
}

CHAOS_IL2CPP_INT32 ChaosBCryptDecrypt(
    CHAOS_IL2CPP_INTPTR hKey,
    CHAOS_IL2CPP_INTPTR pbInput,
    CHAOS_IL2CPP_INT32 cbInput,
    CHAOS_IL2CPP_INTPTR pPaddingInfo,
    CHAOS_IL2CPP_INTPTR pbIV,
    CHAOS_IL2CPP_INT32 cbIV,
    CHAOS_IL2CPP_INTPTR pbOutput,
    CHAOS_IL2CPP_INT32 cbOutput,
    CHAOS_IL2CPP_INTPTR pcbResult,
    CHAOS_IL2CPP_INT32 dwFlags) noexcept
{
    auto hK = reinterpret_cast<BCRYPT_KEY_HANDLE>(hKey);
    NTSTATUS status = BCryptDecrypt(
        hK,
        reinterpret_cast<PUCHAR>(pbInput), static_cast<ULONG>(cbInput),
        reinterpret_cast<void*>(pPaddingInfo),
        reinterpret_cast<PUCHAR>(pbIV), static_cast<ULONG>(cbIV),
        reinterpret_cast<PUCHAR>(pbOutput), static_cast<ULONG>(cbOutput),
        reinterpret_cast<ULONG*>(pcbResult),
        static_cast<ULONG>(dwFlags));
    return static_cast<CHAOS_IL2CPP_INT32>(status);
}

CHAOS_IL2CPP_INT32 ChaosBCryptImportKey(
    CHAOS_IL2CPP_INTPTR hAlgorithm,
    CHAOS_IL2CPP_INTPTR hImportKey,
    CHAOS_IL2CPP_INTPTR pszBlobType,
    CHAOS_IL2CPP_INTPTR phKey,
    CHAOS_IL2CPP_INTPTR pbKeyObject,
    CHAOS_IL2CPP_INT32 cbKeyObject,
    CHAOS_IL2CPP_INTPTR pbInput,
    CHAOS_IL2CPP_INT32 cbInput,
    CHAOS_IL2CPP_INT32 dwFlags) noexcept
{
    auto hAlg = reinterpret_cast<BCRYPT_ALG_HANDLE>(hAlgorithm);
    auto hImpKey = reinterpret_cast<BCRYPT_KEY_HANDLE>(hImportKey);
    BCRYPT_KEY_HANDLE* phK = reinterpret_cast<BCRYPT_KEY_HANDLE*>(phKey);
    if (phK == nullptr) return -1;
    NTSTATUS status = BCryptImportKey(
        hAlg, hImpKey,
        _AsWideStr(pszBlobType), phK,
        reinterpret_cast<PUCHAR>(pbKeyObject), static_cast<ULONG>(cbKeyObject),
        reinterpret_cast<PUCHAR>(pbInput), static_cast<ULONG>(cbInput),
        static_cast<ULONG>(dwFlags));
    return static_cast<CHAOS_IL2CPP_INT32>(status);
}

CHAOS_IL2CPP_INT32 ChaosBCryptExportKey(
    CHAOS_IL2CPP_INTPTR hKey,
    CHAOS_IL2CPP_INTPTR hExportKey,
    CHAOS_IL2CPP_INTPTR pszBlobType,
    CHAOS_IL2CPP_INTPTR pbOutput,
    CHAOS_IL2CPP_INT32 cbOutput,
    CHAOS_IL2CPP_INTPTR pcbResult,
    CHAOS_IL2CPP_INT32 dwFlags) noexcept
{
    auto hK = reinterpret_cast<BCRYPT_KEY_HANDLE>(hKey);
    auto hExpKey = reinterpret_cast<BCRYPT_KEY_HANDLE>(hExportKey);
    NTSTATUS status = BCryptExportKey(
        hK, hExpKey,
        _AsWideStr(pszBlobType),
        reinterpret_cast<PUCHAR>(pbOutput), static_cast<ULONG>(cbOutput),
        reinterpret_cast<ULONG*>(pcbResult),
        static_cast<ULONG>(dwFlags));
    return static_cast<CHAOS_IL2CPP_INT32>(status);
}

CHAOS_IL2CPP_INT32 ChaosBCryptGetProperty(
    CHAOS_IL2CPP_INTPTR hObject,
    CHAOS_IL2CPP_INTPTR pszProperty,
    CHAOS_IL2CPP_INTPTR pbOutput,
    CHAOS_IL2CPP_INT32 cbOutput,
    CHAOS_IL2CPP_INTPTR pcbResult,
    CHAOS_IL2CPP_INT32 dwFlags) noexcept
{
    NTSTATUS status = BCryptGetProperty(
        reinterpret_cast<void*>(hObject),
        _AsWideStr(pszProperty),
        reinterpret_cast<PUCHAR>(pbOutput), static_cast<ULONG>(cbOutput),
        reinterpret_cast<ULONG*>(pcbResult),
        static_cast<ULONG>(dwFlags));
    return static_cast<CHAOS_IL2CPP_INT32>(status);
}

CHAOS_IL2CPP_INT32 ChaosBCryptSetProperty(
    CHAOS_IL2CPP_INTPTR hObject,
    CHAOS_IL2CPP_INTPTR pszProperty,
    CHAOS_IL2CPP_INTPTR pbInput,
    CHAOS_IL2CPP_INT32 cbInput,
    CHAOS_IL2CPP_INT32 dwFlags) noexcept
{
    NTSTATUS status = BCryptSetProperty(
        reinterpret_cast<void*>(hObject),
        _AsWideStr(pszProperty),
        reinterpret_cast<PUCHAR>(pbInput), static_cast<ULONG>(cbInput),
        static_cast<ULONG>(dwFlags));
    return static_cast<CHAOS_IL2CPP_INT32>(status);
}

CHAOS_IL2CPP_INT32 ChaosBCryptGenerateKeyPair(
    CHAOS_IL2CPP_INTPTR hAlgorithm,
    CHAOS_IL2CPP_INTPTR phKey,
    CHAOS_IL2CPP_INT32 dwLength,
    CHAOS_IL2CPP_INT32 dwFlags) noexcept
{
    auto hAlg = reinterpret_cast<BCRYPT_ALG_HANDLE>(hAlgorithm);
    BCRYPT_KEY_HANDLE* phK = reinterpret_cast<BCRYPT_KEY_HANDLE*>(phKey);
    if (phK == nullptr) return -1;
    NTSTATUS status = BCryptGenerateKeyPair(
        hAlg, phK,
        static_cast<ULONG>(dwLength),
        static_cast<ULONG>(dwFlags));
    return static_cast<CHAOS_IL2CPP_INT32>(status);
}

CHAOS_IL2CPP_INT32 ChaosBCryptFinalizeKeyPair(
    CHAOS_IL2CPP_INTPTR hKey,
    CHAOS_IL2CPP_INT32 dwFlags) noexcept
{
    auto hK = reinterpret_cast<BCRYPT_KEY_HANDLE>(hKey);
    NTSTATUS status = BCryptFinalizeKeyPair(hK, static_cast<ULONG>(dwFlags));
    return static_cast<CHAOS_IL2CPP_INT32>(status);
}

CHAOS_IL2CPP_INT32 ChaosBCryptImportKeyPair(
    CHAOS_IL2CPP_INTPTR hAlgorithm,
    CHAOS_IL2CPP_INTPTR hImportKey,
    CHAOS_IL2CPP_INTPTR pszBlobType,
    CHAOS_IL2CPP_INTPTR phKey,
    CHAOS_IL2CPP_INTPTR pbInput,
    CHAOS_IL2CPP_INT32 cbInput,
    CHAOS_IL2CPP_INT32 dwFlags) noexcept
{
    auto hAlg = reinterpret_cast<BCRYPT_ALG_HANDLE>(hAlgorithm);
    auto hImpKey = reinterpret_cast<BCRYPT_KEY_HANDLE>(hImportKey);
    BCRYPT_KEY_HANDLE* phK = reinterpret_cast<BCRYPT_KEY_HANDLE*>(phKey);
    if (phK == nullptr) return -1;
    NTSTATUS status = BCryptImportKeyPair(
        hAlg, hImpKey,
        _AsWideStr(pszBlobType), phK,
        reinterpret_cast<PUCHAR>(pbInput), static_cast<ULONG>(cbInput),
        static_cast<ULONG>(dwFlags));
    return static_cast<CHAOS_IL2CPP_INT32>(status);
}

CHAOS_IL2CPP_INT32 ChaosBCryptSignHash(
    CHAOS_IL2CPP_INTPTR hKey,
    CHAOS_IL2CPP_INTPTR pPaddingInfo,
    CHAOS_IL2CPP_INTPTR pbInput,
    CHAOS_IL2CPP_INT32 cbInput,
    CHAOS_IL2CPP_INTPTR pbOutput,
    CHAOS_IL2CPP_INT32 cbOutput,
    CHAOS_IL2CPP_INTPTR pcbResult,
    CHAOS_IL2CPP_INT32 dwFlags) noexcept
{
    auto hK = reinterpret_cast<BCRYPT_KEY_HANDLE>(hKey);
    NTSTATUS status = BCryptSignHash(
        hK,
        reinterpret_cast<void*>(pPaddingInfo),
        reinterpret_cast<PUCHAR>(pbInput), static_cast<ULONG>(cbInput),
        reinterpret_cast<PUCHAR>(pbOutput), static_cast<ULONG>(cbOutput),
        reinterpret_cast<ULONG*>(pcbResult),
        static_cast<ULONG>(dwFlags));
    return static_cast<CHAOS_IL2CPP_INT32>(status);
}

CHAOS_IL2CPP_INT32 ChaosBCryptVerifySignature(
    CHAOS_IL2CPP_INTPTR hKey,
    CHAOS_IL2CPP_INTPTR pPaddingInfo,
    CHAOS_IL2CPP_INTPTR pbHash,
    CHAOS_IL2CPP_INT32 cbHash,
    CHAOS_IL2CPP_INTPTR pbSignature,
    CHAOS_IL2CPP_INT32 cbSignature,
    CHAOS_IL2CPP_INT32 dwFlags) noexcept
{
    auto hK = reinterpret_cast<BCRYPT_KEY_HANDLE>(hKey);
    NTSTATUS status = BCryptVerifySignature(
        hK,
        reinterpret_cast<void*>(pPaddingInfo),
        reinterpret_cast<PUCHAR>(pbHash), static_cast<ULONG>(cbHash),
        reinterpret_cast<PUCHAR>(pbSignature), static_cast<ULONG>(cbSignature),
        static_cast<ULONG>(dwFlags));
    return static_cast<CHAOS_IL2CPP_INT32>(status);
}

CHAOS_IL2CPP_INT32 ChaosBCryptSecretAgreement(
    CHAOS_IL2CPP_INTPTR hPrivKey,
    CHAOS_IL2CPP_INTPTR hPubKey,
    CHAOS_IL2CPP_INTPTR phAgreedSecret,
    CHAOS_IL2CPP_INT32 dwFlags) noexcept
{
    auto hPriv = reinterpret_cast<BCRYPT_KEY_HANDLE>(hPrivKey);
    auto hPub = reinterpret_cast<BCRYPT_KEY_HANDLE>(hPubKey);
    BCRYPT_SECRET_HANDLE* phSecret = reinterpret_cast<BCRYPT_SECRET_HANDLE*>(phAgreedSecret);
    if (phSecret == nullptr) return -1;
    NTSTATUS status = BCryptSecretAgreement(
        hPriv, hPub, phSecret,
        static_cast<ULONG>(dwFlags));
    return static_cast<CHAOS_IL2CPP_INT32>(status);
}

CHAOS_IL2CPP_INT32 ChaosBCryptDestroySecret(
    CHAOS_IL2CPP_INTPTR hSecret) noexcept
{
    auto hS = reinterpret_cast<BCRYPT_SECRET_HANDLE>(hSecret);
    NTSTATUS status = BCryptDestroySecret(hS);
    return static_cast<CHAOS_IL2CPP_INT32>(status);
}

CHAOS_IL2CPP_INT32 ChaosBCryptDeriveKey(
    CHAOS_IL2CPP_INTPTR hSecret,
    CHAOS_IL2CPP_INTPTR pwszKDF,
    CHAOS_IL2CPP_INTPTR pParameterList,
    CHAOS_IL2CPP_INTPTR pbDerivedKey,
    CHAOS_IL2CPP_INT32 cbDerivedKey,
    CHAOS_IL2CPP_INTPTR pcbResult,
    CHAOS_IL2CPP_INT32 dwFlags) noexcept
{
    auto hS = reinterpret_cast<BCRYPT_SECRET_HANDLE>(hSecret);
    NTSTATUS status = BCryptDeriveKey(
        hS,
        _AsWideStr(pwszKDF),
        reinterpret_cast<BCryptBufferDesc*>(pParameterList),
        reinterpret_cast<PUCHAR>(pbDerivedKey), static_cast<ULONG>(cbDerivedKey),
        reinterpret_cast<ULONG*>(pcbResult),
        static_cast<ULONG>(dwFlags));
    return static_cast<CHAOS_IL2CPP_INT32>(status);
}

CHAOS_IL2CPP_INT32 ChaosBCryptKeyDerivation(
    CHAOS_IL2CPP_INTPTR hKey,
    CHAOS_IL2CPP_INTPTR pParameterList,
    CHAOS_IL2CPP_INTPTR pbDerivedKey,
    CHAOS_IL2CPP_INT32 cbDerivedKey,
    CHAOS_IL2CPP_INTPTR pcbResult,
    CHAOS_IL2CPP_INT32 dwFlags) noexcept
{
    auto hK = reinterpret_cast<BCRYPT_KEY_HANDLE>(hKey);
    NTSTATUS status = BCryptKeyDerivation(
        hK,
        reinterpret_cast<BCryptBufferDesc*>(pParameterList),
        reinterpret_cast<PUCHAR>(pbDerivedKey), static_cast<ULONG>(cbDerivedKey),
        reinterpret_cast<ULONG*>(pcbResult),
        static_cast<ULONG>(dwFlags));
    return static_cast<CHAOS_IL2CPP_INT32>(status);
}

CHAOS_IL2CPP_INT32 ChaosBCryptGenRandom(
    CHAOS_IL2CPP_INTPTR hAlgorithm,
    CHAOS_IL2CPP_INTPTR pbBuffer,
    CHAOS_IL2CPP_INT32 cbBuffer,
    CHAOS_IL2CPP_INT32 dwFlags) noexcept
{
    auto hAlg = reinterpret_cast<BCRYPT_ALG_HANDLE>(hAlgorithm);
    NTSTATUS status = BCryptGenRandom(
        hAlg,
        reinterpret_cast<PUCHAR>(pbBuffer), static_cast<ULONG>(cbBuffer),
        static_cast<ULONG>(dwFlags));
    return static_cast<CHAOS_IL2CPP_INT32>(status);
}

CHAOS_IL2CPP_INT32 ChaosBCryptIsAvailable() noexcept
{
    return 1;  // Windows always has BCrypt available
}

#else  // !_WIN32 — OpenSSL-based BCrypt P/Invoke stubs

// On non-Windows, BCrypt P/Invoke calls are not available.
// These stubs return error codes so the managed code can handle
// PlatformNotSupportedException gracefully.
// Full implementation would require an OpenSSL-based abstraction
// layer for each BCrypt function.

CHAOS_IL2CPP_INT32 ChaosBCryptOpenAlgorithmProvider(
    CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INT32) noexcept
{ return -1; }  // STATUS_NOT_SUPPORTED

CHAOS_IL2CPP_INT32 ChaosBCryptCloseAlgorithmProvider(
    CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INT32) noexcept
{ return 0; }  // STATUS_SUCCESS (no-op)

CHAOS_IL2CPP_INT32 ChaosBCryptCreateHash(
    CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR,
    CHAOS_IL2CPP_INT32, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INT32, CHAOS_IL2CPP_INT32) noexcept
{ return -1; }

CHAOS_IL2CPP_INT32 ChaosBCryptDestroyHash(CHAOS_IL2CPP_INTPTR) noexcept
{ return 0; }

CHAOS_IL2CPP_INT32 ChaosBCryptHashData(
    CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INT32, CHAOS_IL2CPP_INT32) noexcept
{ return -1; }

CHAOS_IL2CPP_INT32 ChaosBCryptFinishHash(
    CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INT32, CHAOS_IL2CPP_INT32) noexcept
{ return -1; }

CHAOS_IL2CPP_INT32 ChaosBCryptHash(
    CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INT32,
    CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INT32, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INT32) noexcept
{ return -1; }

CHAOS_IL2CPP_INT32 ChaosBCryptGenerateSymmetricKey(
    CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR,
    CHAOS_IL2CPP_INT32, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INT32, CHAOS_IL2CPP_INT32) noexcept
{ return -1; }

CHAOS_IL2CPP_INT32 ChaosBCryptDestroyKey(CHAOS_IL2CPP_INTPTR) noexcept
{ return 0; }

CHAOS_IL2CPP_INT32 ChaosBCryptEncrypt(
    CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INT32, CHAOS_IL2CPP_INTPTR,
    CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INT32, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INT32,
    CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INT32) noexcept
{ return -1; }

CHAOS_IL2CPP_INT32 ChaosBCryptDecrypt(
    CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INT32, CHAOS_IL2CPP_INTPTR,
    CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INT32, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INT32,
    CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INT32) noexcept
{ return -1; }

CHAOS_IL2CPP_INT32 ChaosBCryptImportKey(
    CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR,
    CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INT32, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INT32,
    CHAOS_IL2CPP_INT32) noexcept
{ return -1; }

CHAOS_IL2CPP_INT32 ChaosBCryptExportKey(
    CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR,
    CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INT32, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INT32) noexcept
{ return -1; }

CHAOS_IL2CPP_INT32 ChaosBCryptGetProperty(
    CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR,
    CHAOS_IL2CPP_INT32, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INT32) noexcept
{ return -1; }

CHAOS_IL2CPP_INT32 ChaosBCryptSetProperty(
    CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR,
    CHAOS_IL2CPP_INT32, CHAOS_IL2CPP_INT32) noexcept
{ return -1; }

CHAOS_IL2CPP_INT32 ChaosBCryptGenerateKeyPair(
    CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INT32, CHAOS_IL2CPP_INT32) noexcept
{ return -1; }

CHAOS_IL2CPP_INT32 ChaosBCryptFinalizeKeyPair(
    CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INT32) noexcept
{ return -1; }

CHAOS_IL2CPP_INT32 ChaosBCryptImportKeyPair(
    CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR,
    CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INT32, CHAOS_IL2CPP_INT32) noexcept
{ return -1; }

CHAOS_IL2CPP_INT32 ChaosBCryptSignHash(
    CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INT32,
    CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INT32, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INT32) noexcept
{ return -1; }

CHAOS_IL2CPP_INT32 ChaosBCryptVerifySignature(
    CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INT32,
    CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INT32, CHAOS_IL2CPP_INT32) noexcept
{ return -1; }

CHAOS_IL2CPP_INT32 ChaosBCryptSecretAgreement(
    CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INT32) noexcept
{ return -1; }

CHAOS_IL2CPP_INT32 ChaosBCryptDestroySecret(CHAOS_IL2CPP_INTPTR) noexcept
{ return 0; }

CHAOS_IL2CPP_INT32 ChaosBCryptDeriveKey(
    CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR,
    CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INT32, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INT32) noexcept
{ return -1; }

CHAOS_IL2CPP_INT32 ChaosBCryptKeyDerivation(
    CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR,
    CHAOS_IL2CPP_INT32, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INT32) noexcept
{ return -1; }

CHAOS_IL2CPP_INT32 ChaosBCryptGenRandom(
    CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INT32, CHAOS_IL2CPP_INT32) noexcept
{ return -1; }

CHAOS_IL2CPP_INT32 ChaosBCryptIsAvailable() noexcept
{
    return 0;  // BCrypt not available on non-Windows
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
