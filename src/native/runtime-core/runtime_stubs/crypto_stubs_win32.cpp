// crypto_stubs_win32.cpp — Windows BCrypt-based crypto stub implementations
// This file is compiled unconditionally; the #ifdef _WIN32 guard ensures it
// only produces code on Windows.  Contains:
//   1. High-level hash/HMAC/RNG helpers (ChaosCngHash, ChaosCngHmac, etc.)
//   2. Raw BCrypt P/Invoke stubs (ChaosBCryptOpenAlgorithmProvider, etc.)

#include "crypto_stubs_internal.h"

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
        // inArr->length is the byte count for a managed byte[]; the prior
        // `* sizeof(CHAOS_IL2CPP_INTPTR)` over-read past the buffer.
        status = BCryptHashData(hHash, (PUCHAR)(inData),
            static_cast<ULONG>(inArr->length), 0);
        if (ChaosCngOk(status)) {
            status = BCryptFinishHash(hHash, hashBuf.get(), hashLen, 0);
        }
        BCryptDestroyHash(hHash);
    }
    BCryptCloseAlgorithmProvider(hAlg, 0);

    if (!ChaosCngOk(status)) return 0;

    // Allocate output managed byte[] and copy the computed hash into it
    // (mirrors ChaosOpenSslHash). Previously returned 0 sentinel, discarding
    // the result — the managed side silently got an empty hash.
    auto result = alloc_byte_array(static_cast<CHAOS_IL2CPP_SIZE>(hashLen));
    if (result != 0)
    {
        auto* outArr = get_managed_array_mut(result);
        auto* outData = reinterpret_cast<CHAOS_IL2CPP_UINT8*>(accessor_get_elements(outArr));
        std::memcpy(outData, hashBuf.get(), static_cast<size_t>(hashLen));
    }
    return result;
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
        static_cast<ULONG>(keyArr->length),   // byte count for byte[] (was *sizeof)
        static_cast<PUCHAR>(const_cast<void*>(static_cast<const void*>(dataBuf))),
        static_cast<ULONG>(dataArr->length),  // byte count for byte[] (was *sizeof)
        buf.get(), hashLen);

    BCryptCloseAlgorithmProvider(hAlg, 0);
    if (!ChaosCngOk(status)) return 0;

    // Allocate output managed byte[] and copy the computed HMAC into it
    // (mirrors ChaosOpenSslHmac). Previously returned 0 sentinel, discarding
    // the result — the managed side silently got an empty HMAC.
    auto result = alloc_byte_array(static_cast<CHAOS_IL2CPP_SIZE>(hashLen));
    if (result != 0)
    {
        auto* outArr = get_managed_array_mut(result);
        auto* outData = reinterpret_cast<CHAOS_IL2CPP_UINT8*>(accessor_get_elements(outArr));
        std::memcpy(outData, buf.get(), static_cast<size_t>(hashLen));
    }
    return result;
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
    // Allocate managed byte[] and fill with cryptographically strong random
    // bytes via the system-preferred RNG (mirrors ChaosCngFillRandom).
    auto result = alloc_byte_array(static_cast<CHAOS_IL2CPP_SIZE>(count));
    if (result == 0) return 0;
    auto* outArr = get_managed_array_mut(result);
    auto* outData = reinterpret_cast<CHAOS_IL2CPP_UINT8*>(accessor_get_elements(outArr));
    if (outData == nullptr) return 0;
    NTSTATUS status = BCryptGenRandom(nullptr, outData, static_cast<ULONG>(count),
                                      BCRYPT_USE_SYSTEM_PREFERRED_RNG);
    if (!ChaosCngOk(status)) {
        // Fallback: PAL random (mirrors ChaosCngFillRandom).
        chaos::il2cpp::pal::PalRandomBytes(outData, static_cast<size_t>(count));
    }
    return result;
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

#endif  // _WIN32 — closes the BCrypt hash/RNG implementation block

// ═══════════════════════════════════════════════════════════════════════
// BCrypt P/Invoke stubs (Windows: delegate to bcrypt.dll)
// These stubs are called from the P/Invoke routing path in ChaosExternalRuntimeFallback
// when managed Interop+BCrypt methods are encountered.
// ═══════════════════════════════════════════════════════════════════════

#ifdef _WIN32

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

#endif  // _WIN32 — closes the BCrypt P/Invoke stubs block

}  // extern "C"
}  // namespace chaos::il2cpp::runtime_core