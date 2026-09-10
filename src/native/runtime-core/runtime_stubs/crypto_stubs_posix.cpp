// crypto_stubs_posix.cpp — POSIX/OpenSSL-based crypto stub implementations
// This file is compiled unconditionally; the #ifndef _WIN32 guard ensures it
// only produces code on non-Windows platforms.  Contains:
//   1. High-level OpenSSL hash/HMAC/RNG helpers
//   2. Non-Windows BCrypt P/Invoke stubs (return error codes)

#include "crypto_stubs_internal.h"

namespace chaos::il2cpp::runtime_core {
extern "C" {

#ifndef _WIN32

#include <openssl/evp.h>
#include <openssl/hmac.h>
#include <openssl/rand.h>

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

#endif  // !_WIN32 — closes the OpenSSL hash/RNG implementation block

// ═══════════════════════════════════════════════════════════════════════
// BCrypt P/Invoke stubs (non-Windows: return error codes)
// On non-Windows, BCrypt P/Invoke calls are not available.
// These stubs return error codes so the managed code can handle
// PlatformNotSupportedException gracefully.
// ═══════════════════════════════════════════════════════════════════════

#ifndef _WIN32

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

#endif  // !_WIN32 — closes the non-Windows BCrypt stubs block

}  // extern "C"
}  // namespace chaos::il2cpp::runtime_core