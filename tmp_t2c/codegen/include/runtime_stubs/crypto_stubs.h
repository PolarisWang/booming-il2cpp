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

// ── BCrypt P/Invoke stubs ──────────────────────────────────────────
// These stubs provide extern "C" implementations of the BCrypt/CNG
// functions that the managed Interop+BCrypt class calls via DllImport.
// They route through the P/Invoke routing path in ChaosExternalRuntimeFallback.
// Each function receives raw CHAOS_IL2CPP_INTPTR arguments (flat ABI) and
// returns the NTSTATUS or handle as CHAOS_IL2CPP_INT32 / CHAOS_IL2CPP_INTPTR.

/// BCryptOpenAlgorithmProvider: opens a CNG algorithm provider.
/// phAlgorithm [out] safe handle, pszAlgId [in] wide string ptr,
/// pszImplementation [in] wide string ptr, dwFlags [in] int32.
/// Returns NTSTATUS as int32.
CHAOS_IL2CPP_INT32 ChaosBCryptOpenAlgorithmProvider(
    CHAOS_IL2CPP_INTPTR phAlgorithm,
    CHAOS_IL2CPP_INTPTR pszAlgId,
    CHAOS_IL2CPP_INTPTR pszImplementation,
    CHAOS_IL2CPP_INT32 dwFlags) noexcept;

/// BCryptCloseAlgorithmProvider: closes a CNG algorithm provider.
CHAOS_IL2CPP_INT32 ChaosBCryptCloseAlgorithmProvider(
    CHAOS_IL2CPP_INTPTR hAlgorithm,
    CHAOS_IL2CPP_INT32 dwFlags) noexcept;

/// BCryptCreateHash: creates a hash object.
CHAOS_IL2CPP_INT32 ChaosBCryptCreateHash(
    CHAOS_IL2CPP_INTPTR hAlgorithm,
    CHAOS_IL2CPP_INTPTR phHash,
    CHAOS_IL2CPP_INTPTR pbHashObject,
    CHAOS_IL2CPP_INT32 cbHashObject,
    CHAOS_IL2CPP_INTPTR pbSecret,
    CHAOS_IL2CPP_INT32 cbSecret,
    CHAOS_IL2CPP_INT32 dwFlags) noexcept;

/// BCryptDestroyHash: destroys a hash object.
CHAOS_IL2CPP_INT32 ChaosBCryptDestroyHash(
    CHAOS_IL2CPP_INTPTR hHash) noexcept;

/// BCryptHashData: feeds data into a hash object.
CHAOS_IL2CPP_INT32 ChaosBCryptHashData(
    CHAOS_IL2CPP_INTPTR hHash,
    CHAOS_IL2CPP_INTPTR pbInput,
    CHAOS_IL2CPP_INT32 cbInput,
    CHAOS_IL2CPP_INT32 dwFlags) noexcept;

/// BCryptFinishHash: retrieves the hash value.
CHAOS_IL2CPP_INT32 ChaosBCryptFinishHash(
    CHAOS_IL2CPP_INTPTR hHash,
    CHAOS_IL2CPP_INTPTR pbOutput,
    CHAOS_IL2CPP_INT32 cbOutput,
    CHAOS_IL2CPP_INT32 dwFlags) noexcept;

/// BCryptHash: one-shot hash (Windows 10+).
CHAOS_IL2CPP_INT32 ChaosBCryptHash(
    CHAOS_IL2CPP_INTPTR hAlgorithm,
    CHAOS_IL2CPP_INTPTR pbSecret,
    CHAOS_IL2CPP_INT32 cbSecret,
    CHAOS_IL2CPP_INTPTR pbInput,
    CHAOS_IL2CPP_INT32 cbInput,
    CHAOS_IL2CPP_INTPTR pbOutput,
    CHAOS_IL2CPP_INT32 cbOutput) noexcept;

/// BCryptGenerateSymmetricKey: creates a symmetric key from raw key material.
CHAOS_IL2CPP_INT32 ChaosBCryptGenerateSymmetricKey(
    CHAOS_IL2CPP_INTPTR hAlgorithm,
    CHAOS_IL2CPP_INTPTR phKey,
    CHAOS_IL2CPP_INTPTR pbKeyObject,
    CHAOS_IL2CPP_INT32 cbKeyObject,
    CHAOS_IL2CPP_INTPTR pbSecret,
    CHAOS_IL2CPP_INT32 cbSecret,
    CHAOS_IL2CPP_INT32 dwFlags) noexcept;

/// BCryptDestroyKey: destroys a key.
CHAOS_IL2CPP_INT32 ChaosBCryptDestroyKey(
    CHAOS_IL2CPP_INTPTR hKey) noexcept;

/// BCryptEncrypt: encrypts data.
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
    CHAOS_IL2CPP_INT32 dwFlags) noexcept;

/// BCryptDecrypt: decrypts data.
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
    CHAOS_IL2CPP_INT32 dwFlags) noexcept;

/// BCryptImportKey: imports a symmetric key.
CHAOS_IL2CPP_INT32 ChaosBCryptImportKey(
    CHAOS_IL2CPP_INTPTR hAlgorithm,
    CHAOS_IL2CPP_INTPTR hImportKey,
    CHAOS_IL2CPP_INTPTR pszBlobType,
    CHAOS_IL2CPP_INTPTR phKey,
    CHAOS_IL2CPP_INTPTR pbKeyObject,
    CHAOS_IL2CPP_INT32 cbKeyObject,
    CHAOS_IL2CPP_INTPTR pbInput,
    CHAOS_IL2CPP_INT32 cbInput,
    CHAOS_IL2CPP_INT32 dwFlags) noexcept;

/// BCryptExportKey: exports a key.
CHAOS_IL2CPP_INT32 ChaosBCryptExportKey(
    CHAOS_IL2CPP_INTPTR hKey,
    CHAOS_IL2CPP_INTPTR hExportKey,
    CHAOS_IL2CPP_INTPTR pszBlobType,
    CHAOS_IL2CPP_INTPTR pbOutput,
    CHAOS_IL2CPP_INT32 cbOutput,
    CHAOS_IL2CPP_INTPTR pcbResult,
    CHAOS_IL2CPP_INT32 dwFlags) noexcept;

/// BCryptGetProperty: gets a BCrypt property.
CHAOS_IL2CPP_INT32 ChaosBCryptGetProperty(
    CHAOS_IL2CPP_INTPTR hObject,
    CHAOS_IL2CPP_INTPTR pszProperty,
    CHAOS_IL2CPP_INTPTR pbOutput,
    CHAOS_IL2CPP_INT32 cbOutput,
    CHAOS_IL2CPP_INTPTR pcbResult,
    CHAOS_IL2CPP_INT32 dwFlags) noexcept;

/// BCryptSetProperty: sets a BCrypt property.
CHAOS_IL2CPP_INT32 ChaosBCryptSetProperty(
    CHAOS_IL2CPP_INTPTR hObject,
    CHAOS_IL2CPP_INTPTR pszProperty,
    CHAOS_IL2CPP_INTPTR pbInput,
    CHAOS_IL2CPP_INT32 cbInput,
    CHAOS_IL2CPP_INT32 dwFlags) noexcept;

/// BCryptGenerateKeyPair: generates an asymmetric key pair.
CHAOS_IL2CPP_INT32 ChaosBCryptGenerateKeyPair(
    CHAOS_IL2CPP_INTPTR hAlgorithm,
    CHAOS_IL2CPP_INTPTR phKey,
    CHAOS_IL2CPP_INT32 dwLength,
    CHAOS_IL2CPP_INT32 dwFlags) noexcept;

/// BCryptFinalizeKeyPair: finalizes an asymmetric key pair.
CHAOS_IL2CPP_INT32 ChaosBCryptFinalizeKeyPair(
    CHAOS_IL2CPP_INTPTR hKey,
    CHAOS_IL2CPP_INT32 dwFlags) noexcept;

/// BCryptImportKeyPair: imports an asymmetric key pair.
CHAOS_IL2CPP_INT32 ChaosBCryptImportKeyPair(
    CHAOS_IL2CPP_INTPTR hAlgorithm,
    CHAOS_IL2CPP_INTPTR hImportKey,
    CHAOS_IL2CPP_INTPTR pszBlobType,
    CHAOS_IL2CPP_INTPTR phKey,
    CHAOS_IL2CPP_INTPTR pbInput,
    CHAOS_IL2CPP_INT32 cbInput,
    CHAOS_IL2CPP_INT32 dwFlags) noexcept;

/// BCryptSignHash: signs a hash value.
CHAOS_IL2CPP_INT32 ChaosBCryptSignHash(
    CHAOS_IL2CPP_INTPTR hKey,
    CHAOS_IL2CPP_INTPTR pPaddingInfo,
    CHAOS_IL2CPP_INTPTR pbInput,
    CHAOS_IL2CPP_INT32 cbInput,
    CHAOS_IL2CPP_INTPTR pbOutput,
    CHAOS_IL2CPP_INT32 cbOutput,
    CHAOS_IL2CPP_INTPTR pcbResult,
    CHAOS_IL2CPP_INT32 dwFlags) noexcept;

/// BCryptVerifySignature: verifies a signature.
CHAOS_IL2CPP_INT32 ChaosBCryptVerifySignature(
    CHAOS_IL2CPP_INTPTR hKey,
    CHAOS_IL2CPP_INTPTR pPaddingInfo,
    CHAOS_IL2CPP_INTPTR pbHash,
    CHAOS_IL2CPP_INT32 cbHash,
    CHAOS_IL2CPP_INTPTR pbSignature,
    CHAOS_IL2CPP_INT32 cbSignature,
    CHAOS_IL2CPP_INT32 dwFlags) noexcept;

/// BCryptSecretAgreement: creates a secret agreement (ECDH).
CHAOS_IL2CPP_INT32 ChaosBCryptSecretAgreement(
    CHAOS_IL2CPP_INTPTR hPrivKey,
    CHAOS_IL2CPP_INTPTR hPubKey,
    CHAOS_IL2CPP_INTPTR phAgreedSecret,
    CHAOS_IL2CPP_INT32 dwFlags) noexcept;

/// BCryptDestroySecret: destroys a secret agreement handle.
CHAOS_IL2CPP_INT32 ChaosBCryptDestroySecret(
    CHAOS_IL2CPP_INTPTR hSecret) noexcept;

/// BCryptDeriveKey: derives a key from a secret agreement.
CHAOS_IL2CPP_INT32 ChaosBCryptDeriveKey(
    CHAOS_IL2CPP_INTPTR hSecret,
    CHAOS_IL2CPP_INTPTR pwszKDF,
    CHAOS_IL2CPP_INTPTR pParameterList,
    CHAOS_IL2CPP_INTPTR pbDerivedKey,
    CHAOS_IL2CPP_INT32 cbDerivedKey,
    CHAOS_IL2CPP_INTPTR pcbResult,
    CHAOS_IL2CPP_INT32 dwFlags) noexcept;

/// BCryptKeyDerivation: derives a key using a KDF (Windows 10+).
CHAOS_IL2CPP_INT32 ChaosBCryptKeyDerivation(
    CHAOS_IL2CPP_INTPTR hKey,
    CHAOS_IL2CPP_INTPTR pParameterList,
    CHAOS_IL2CPP_INTPTR pbDerivedKey,
    CHAOS_IL2CPP_INT32 cbDerivedKey,
    CHAOS_IL2CPP_INTPTR pcbResult,
    CHAOS_IL2CPP_INT32 dwFlags) noexcept;

/// BCryptGenRandom: fills a buffer with random bytes.
CHAOS_IL2CPP_INT32 ChaosBCryptGenRandom(
    CHAOS_IL2CPP_INTPTR hAlgorithm,
    CHAOS_IL2CPP_INTPTR pbBuffer,
    CHAOS_IL2CPP_INT32 cbBuffer,
    CHAOS_IL2CPP_INT32 dwFlags) noexcept;

/// Check if the Windows BCrypt DLL is available (non-Windows fallback).
/// Returns non-zero if BCrypt functions are usable.
CHAOS_IL2CPP_INT32 ChaosBCryptIsAvailable() noexcept;

}  // extern "C"
}  // namespace runtime_core
}  // namespace il2cpp
}  // namespace chaos

#endif  // CHAOS_IL2CPP_CRYPTO_STUBS_H_
