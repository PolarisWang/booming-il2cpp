// entry.cpp — Auto-generated
// CLI entry point for native-AOT verification dispatch.
//
// Modes:
//   (no args)            — fact: run all subject entries, print Passed: N/M
//   --benchmark N I      — benchmark method N for I iterations
//   --benchmark-all I    — benchmark ALL methods for I iterations, aggregated JSON
//   --hotupdate          — hotpatch fact: baseline + apply + semantic-check + revert
//   --hotupdate-and-benchmark N I — post-patch benchmark
//   --microbench         — interpreter microbenchmarks
//   --list               — list all subjects with their indices
//   --fact-json          — per-method JSON output for value-level verification
//   --profile             — profile mode: GC stats, alloc volume, heap delta, code size

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <chrono>
#include <cstdint>
#include <cinttypes>
#include <memory>

#if defined(_WIN32)
#define NOMINMAX
#include <Windows.h>
#endif

namespace chaos { namespace il2cpp { namespace common { namespace log_internal {
bool g_log_use_stderr = false;
}}}}

#include <chaos/config.h>
#include <chaos/native_types.h>
#include <chaos/profile.h>
#include <runtime_core.h>

#include "chaos_runtime_host.h"

#include <chaos/hotpatch_dispatch.h>
#include <patch_loader.h>
#include <profile_stats.h>
#include <profile_stats.h>

extern "C" const int kAotMethodCount;
extern "C" const int kSubjectEntryCount;
extern "C" const int kSubjectSlotMap[];

extern "C" const HotpatchEntryV0* GetHotpatchEntries() noexcept;

extern "C" const CodeRegistrationV0 chaos_codegen_code_registration;
extern "C" const MetadataRegistrationV0 chaos_codegen_metadata_registration;
extern "C" const CodegenRegistrationOptionsV0 chaos_codegen_options;
extern "C" void ChaosRegisterGcLayouts();

#define CHAOS_USE_DEFAULT_THUNKS nullptr

struct BenchmarkResult {
    double elapsed_ms;
    int64_t allocated_bytes;
};

extern "C" CHAOS_IL2CPP_INT32 RunFactAll();
extern "C" BenchmarkResult RunBenchmark(int entry_index, int iterations);
extern "C" CHAOS_IL2CPP_INT32 RunHotpatchAll();
extern "C" BenchmarkResult RunHotpatchBenchmark(int entry_index, int iterations);

extern const uint8_t kPatchData[];
extern const size_t kPatchDataSize;
extern const char* const kPatchDataHostClassName;

extern "C" void RunMicrobench();

// Subject metadata table for --list / --fact-json modes
struct SubjectEntry {
    int index;
    const char* subjectId;
    const char* assemblyName;
    const char* typeName;
    const char* methodName;
    bool isStatic;
    const char* kind;
};

static const SubjectEntry kSubjectEntries[] = {
    { 0, "System.Security.Cryptography/System.Security.Cryptography.AesCcm::Encrypt:System.Void(System.Byte[],System.Byte[],System.Byte[],System.Byte[],System.Byte[])", "codegen", "System.Security.Cryptography.AesCcm", "Encrypt", True, "benchmark" },
    { 1, "System.Security.Cryptography/System.Security.Cryptography.AesCcm::Encrypt:System.Void(System.Byte[],System.Byte[],System.Byte[],System.Byte[],System.Byte[])", "codegen", "System.Security.Cryptography.AesCcm", "Encrypt", True, "benchmark" },
    { 2, "System.Security.Cryptography/System.Security.Cryptography.AesCcm::Encrypt:System.Void(System.Byte[],System.Byte[],System.Byte[],System.Byte[],System.Byte[])", "codegen", "System.Security.Cryptography.AesCcm", "Encrypt", True, "benchmark" },
    { 3, "System.Security.Cryptography/System.Security.Cryptography.AesCcm::Decrypt:System.Void(System.Byte[],System.Byte[],System.Byte[],System.Byte[],System.Byte[])", "codegen", "System.Security.Cryptography.AesCcm", "Decrypt", True, "benchmark" },
    { 4, "System.Security.Cryptography/System.Security.Cryptography.AesCcm::Decrypt:System.Void(System.Byte[],System.Byte[],System.Byte[],System.Byte[],System.Byte[])", "codegen", "System.Security.Cryptography.AesCcm", "Decrypt", True, "benchmark" },
    { 5, "System.Security.Cryptography/System.Security.Cryptography.AesCcm::Decrypt:System.Void(System.Byte[],System.Byte[],System.Byte[],System.Byte[],System.Byte[])", "codegen", "System.Security.Cryptography.AesCcm", "Decrypt", True, "benchmark" },
    { 6, "System.Security.Cryptography/System.Security.Cryptography.AesCcm::Dispose:System.Void()", "codegen", "System.Security.Cryptography.AesCcm", "Dispose", True, "benchmark" },
    { 7, "System.Security.Cryptography/System.Security.Cryptography.AesCng::CreateDecryptor:System.Security.Cryptography.ICryptoTransform()", "codegen", "System.Security.Cryptography.AesCng", "CreateDecryptor", True, "benchmark" },
    { 8, "System.Security.Cryptography/System.Security.Cryptography.AesCng::CreateDecryptor:System.Security.Cryptography.ICryptoTransform(System.Byte[],System.Byte[])", "codegen", "System.Security.Cryptography.AesCng", "CreateDecryptor", True, "benchmark" },
    { 9, "System.Security.Cryptography/System.Security.Cryptography.AesCng::CreateDecryptor:System.Security.Cryptography.ICryptoTransform(System.Byte[],System.Byte[])", "codegen", "System.Security.Cryptography.AesCng", "CreateDecryptor", True, "benchmark" },
    { 10, "System.Security.Cryptography/System.Security.Cryptography.AesCng::CreateDecryptor:System.Security.Cryptography.ICryptoTransform(System.Byte[],System.Byte[])", "codegen", "System.Security.Cryptography.AesCng", "CreateDecryptor", True, "benchmark" },
    { 11, "System.Security.Cryptography/System.Security.Cryptography.AesCng::CreateEncryptor:System.Security.Cryptography.ICryptoTransform()", "codegen", "System.Security.Cryptography.AesCng", "CreateEncryptor", True, "benchmark" },
    { 12, "System.Security.Cryptography/System.Security.Cryptography.AesCng::CreateEncryptor:System.Security.Cryptography.ICryptoTransform(System.Byte[],System.Byte[])", "codegen", "System.Security.Cryptography.AesCng", "CreateEncryptor", True, "benchmark" },
    { 13, "System.Security.Cryptography/System.Security.Cryptography.AesCng::CreateEncryptor:System.Security.Cryptography.ICryptoTransform(System.Byte[],System.Byte[])", "codegen", "System.Security.Cryptography.AesCng", "CreateEncryptor", True, "benchmark" },
    { 14, "System.Security.Cryptography/System.Security.Cryptography.AesCng::CreateEncryptor:System.Security.Cryptography.ICryptoTransform(System.Byte[],System.Byte[])", "codegen", "System.Security.Cryptography.AesCng", "CreateEncryptor", True, "benchmark" },
    { 15, "System.Security.Cryptography/System.Security.Cryptography.AesCng::GenerateKey:System.Void()", "codegen", "System.Security.Cryptography.AesCng", "GenerateKey", True, "benchmark" },
    { 16, "System.Security.Cryptography/System.Security.Cryptography.AesCng::GenerateIV:System.Void()", "codegen", "System.Security.Cryptography.AesCng", "GenerateIV", True, "benchmark" },
    { 17, "System.Security.Cryptography/System.Security.Cryptography.AesCryptoServiceProvider::CreateEncryptor:System.Security.Cryptography.ICryptoTransform()", "codegen", "System.Security.Cryptography.AesCryptoServiceProvider", "CreateEncryptor", True, "benchmark" },
    { 18, "System.Security.Cryptography/System.Security.Cryptography.AesCryptoServiceProvider::CreateEncryptor:System.Security.Cryptography.ICryptoTransform(System.Byte[],System.Byte[])", "codegen", "System.Security.Cryptography.AesCryptoServiceProvider", "CreateEncryptor", True, "benchmark" },
    { 19, "System.Security.Cryptography/System.Security.Cryptography.AesCryptoServiceProvider::CreateEncryptor:System.Security.Cryptography.ICryptoTransform(System.Byte[],System.Byte[])", "codegen", "System.Security.Cryptography.AesCryptoServiceProvider", "CreateEncryptor", True, "benchmark" },
    { 20, "System.Security.Cryptography/System.Security.Cryptography.AesCryptoServiceProvider::CreateEncryptor:System.Security.Cryptography.ICryptoTransform(System.Byte[],System.Byte[])", "codegen", "System.Security.Cryptography.AesCryptoServiceProvider", "CreateEncryptor", True, "benchmark" },
    { 21, "System.Security.Cryptography/System.Security.Cryptography.AesCryptoServiceProvider::CreateDecryptor:System.Security.Cryptography.ICryptoTransform()", "codegen", "System.Security.Cryptography.AesCryptoServiceProvider", "CreateDecryptor", True, "benchmark" },
    { 22, "System.Security.Cryptography/System.Security.Cryptography.AesCryptoServiceProvider::CreateDecryptor:System.Security.Cryptography.ICryptoTransform(System.Byte[],System.Byte[])", "codegen", "System.Security.Cryptography.AesCryptoServiceProvider", "CreateDecryptor", True, "benchmark" },
    { 23, "System.Security.Cryptography/System.Security.Cryptography.AesCryptoServiceProvider::CreateDecryptor:System.Security.Cryptography.ICryptoTransform(System.Byte[],System.Byte[])", "codegen", "System.Security.Cryptography.AesCryptoServiceProvider", "CreateDecryptor", True, "benchmark" },
    { 24, "System.Security.Cryptography/System.Security.Cryptography.AesCryptoServiceProvider::CreateDecryptor:System.Security.Cryptography.ICryptoTransform(System.Byte[],System.Byte[])", "codegen", "System.Security.Cryptography.AesCryptoServiceProvider", "CreateDecryptor", True, "benchmark" },
    { 25, "System.Security.Cryptography/System.Security.Cryptography.AesCryptoServiceProvider::GenerateIV:System.Void()", "codegen", "System.Security.Cryptography.AesCryptoServiceProvider", "GenerateIV", True, "benchmark" },
    { 26, "System.Security.Cryptography/System.Security.Cryptography.AesCryptoServiceProvider::GenerateKey:System.Void()", "codegen", "System.Security.Cryptography.AesCryptoServiceProvider", "GenerateKey", True, "benchmark" },
    { 27, "System.Security.Cryptography/System.Security.Cryptography.AesGcm::Encrypt:System.Void(System.Byte[],System.Byte[],System.Byte[],System.Byte[],System.Byte[])", "codegen", "System.Security.Cryptography.AesGcm", "Encrypt", True, "benchmark" },
    { 28, "System.Security.Cryptography/System.Security.Cryptography.AesGcm::Encrypt:System.Void(System.Byte[],System.Byte[],System.Byte[],System.Byte[],System.Byte[])", "codegen", "System.Security.Cryptography.AesGcm", "Encrypt", True, "benchmark" },
    { 29, "System.Security.Cryptography/System.Security.Cryptography.AesGcm::Encrypt:System.Void(System.Byte[],System.Byte[],System.Byte[],System.Byte[],System.Byte[])", "codegen", "System.Security.Cryptography.AesGcm", "Encrypt", True, "benchmark" },
    { 30, "System.Security.Cryptography/System.Security.Cryptography.AesGcm::Decrypt:System.Void(System.Byte[],System.Byte[],System.Byte[],System.Byte[],System.Byte[])", "codegen", "System.Security.Cryptography.AesGcm", "Decrypt", True, "benchmark" },
    { 31, "System.Security.Cryptography/System.Security.Cryptography.AesGcm::Decrypt:System.Void(System.Byte[],System.Byte[],System.Byte[],System.Byte[],System.Byte[])", "codegen", "System.Security.Cryptography.AesGcm", "Decrypt", True, "benchmark" },
    { 32, "System.Security.Cryptography/System.Security.Cryptography.AesGcm::Decrypt:System.Void(System.Byte[],System.Byte[],System.Byte[],System.Byte[],System.Byte[])", "codegen", "System.Security.Cryptography.AesGcm", "Decrypt", True, "benchmark" },
    { 33, "System.Security.Cryptography/System.Security.Cryptography.AesGcm::Dispose:System.Void()", "codegen", "System.Security.Cryptography.AesGcm", "Dispose", True, "benchmark" },
    { 34, "System.Security.Cryptography/System.Security.Cryptography.AesManaged::CreateEncryptor:System.Security.Cryptography.ICryptoTransform()", "codegen", "System.Security.Cryptography.AesManaged", "CreateEncryptor", True, "benchmark" },
    { 35, "System.Security.Cryptography/System.Security.Cryptography.AesManaged::CreateEncryptor:System.Security.Cryptography.ICryptoTransform(System.Byte[],System.Byte[])", "codegen", "System.Security.Cryptography.AesManaged", "CreateEncryptor", True, "benchmark" },
    { 36, "System.Security.Cryptography/System.Security.Cryptography.AesManaged::CreateEncryptor:System.Security.Cryptography.ICryptoTransform(System.Byte[],System.Byte[])", "codegen", "System.Security.Cryptography.AesManaged", "CreateEncryptor", True, "benchmark" },
    { 37, "System.Security.Cryptography/System.Security.Cryptography.AesManaged::CreateEncryptor:System.Security.Cryptography.ICryptoTransform(System.Byte[],System.Byte[])", "codegen", "System.Security.Cryptography.AesManaged", "CreateEncryptor", True, "benchmark" },
    { 38, "System.Security.Cryptography/System.Security.Cryptography.AesManaged::CreateDecryptor:System.Security.Cryptography.ICryptoTransform()", "codegen", "System.Security.Cryptography.AesManaged", "CreateDecryptor", True, "benchmark" },
    { 39, "System.Security.Cryptography/System.Security.Cryptography.AesManaged::CreateDecryptor:System.Security.Cryptography.ICryptoTransform(System.Byte[],System.Byte[])", "codegen", "System.Security.Cryptography.AesManaged", "CreateDecryptor", True, "benchmark" },
    { 40, "System.Security.Cryptography/System.Security.Cryptography.AesManaged::CreateDecryptor:System.Security.Cryptography.ICryptoTransform(System.Byte[],System.Byte[])", "codegen", "System.Security.Cryptography.AesManaged", "CreateDecryptor", True, "benchmark" },
    { 41, "System.Security.Cryptography/System.Security.Cryptography.AesManaged::CreateDecryptor:System.Security.Cryptography.ICryptoTransform(System.Byte[],System.Byte[])", "codegen", "System.Security.Cryptography.AesManaged", "CreateDecryptor", True, "benchmark" },
    { 42, "System.Security.Cryptography/System.Security.Cryptography.AesManaged::GenerateIV:System.Void()", "codegen", "System.Security.Cryptography.AesManaged", "GenerateIV", True, "benchmark" },
    { 43, "System.Security.Cryptography/System.Security.Cryptography.AesManaged::GenerateKey:System.Void()", "codegen", "System.Security.Cryptography.AesManaged", "GenerateKey", True, "benchmark" },
    { 44, "System.Security.Cryptography/System.Security.Cryptography.AsnEncodedData::CopyFrom:System.Void(System.Security.Cryptography.AsnEncodedData)", "codegen", "System.Security.Cryptography.AsnEncodedData", "CopyFrom", True, "benchmark" },
    { 45, "System.Security.Cryptography/System.Security.Cryptography.AsnEncodedData::Format:System.String(System.Boolean)", "codegen", "System.Security.Cryptography.AsnEncodedData", "Format", True, "benchmark" },
    { 46, "System.Security.Cryptography/System.Security.Cryptography.AsnEncodedData::Format:System.String(System.Boolean)", "codegen", "System.Security.Cryptography.AsnEncodedData", "Format", True, "benchmark" },
    { 47, "System.Security.Cryptography/System.Security.Cryptography.AsnEncodedDataCollection::Add:System.Int32(System.Security.Cryptography.AsnEncodedData)", "codegen", "System.Security.Cryptography.AsnEncodedDataCollection", "Add", True, "benchmark" },
    { 48, "System.Security.Cryptography/System.Security.Cryptography.AsnEncodedDataCollection::Remove:System.Void(System.Security.Cryptography.AsnEncodedData)", "codegen", "System.Security.Cryptography.AsnEncodedDataCollection", "Remove", True, "benchmark" },
    { 49, "System.Security.Cryptography/System.Security.Cryptography.AsnEncodedDataCollection::GetEnumerator:System.Security.Cryptography.AsnEncodedDataEnumerator()", "codegen", "System.Security.Cryptography.AsnEncodedDataCollection", "GetEnumerator", True, "benchmark" },
    { 50, "System.Security.Cryptography/System.Security.Cryptography.AsymmetricAlgorithm::FromXmlString:System.Void(System.String)", "codegen", "System.Security.Cryptography.AsymmetricAlgorithm", "FromXmlString", True, "benchmark" },
    { 51, "System.Security.Cryptography/System.Security.Cryptography.AsymmetricAlgorithm::FromXmlString:System.Void(System.String)", "codegen", "System.Security.Cryptography.AsymmetricAlgorithm", "FromXmlString", True, "benchmark" },
    { 52, "System.Security.Cryptography/System.Security.Cryptography.AsymmetricAlgorithm::ToXmlString:System.String(System.Boolean)", "codegen", "System.Security.Cryptography.AsymmetricAlgorithm", "ToXmlString", True, "benchmark" },
    { 53, "System.Security.Cryptography/System.Security.Cryptography.AsymmetricAlgorithm::ToXmlString:System.String(System.Boolean)", "codegen", "System.Security.Cryptography.AsymmetricAlgorithm", "ToXmlString", True, "benchmark" },
    { 54, "System.Security.Cryptography/System.Security.Cryptography.AsymmetricAlgorithm::Clear:System.Void()", "codegen", "System.Security.Cryptography.AsymmetricAlgorithm", "Clear", True, "benchmark" },
    { 55, "System.Security.Cryptography/System.Security.Cryptography.AsymmetricAlgorithm::Dispose:System.Void()", "codegen", "System.Security.Cryptography.AsymmetricAlgorithm", "Dispose", True, "benchmark" },
    { 56, "System.Security.Cryptography/System.Security.Cryptography.AsymmetricAlgorithm::ExportPkcs8PrivateKey:System.Byte[]()", "codegen", "System.Security.Cryptography.AsymmetricAlgorithm", "ExportPkcs8PrivateKey", True, "benchmark" },
    { 57, "System.Security.Cryptography/System.Security.Cryptography.AsymmetricAlgorithm::ExportSubjectPublicKeyInfo:System.Byte[]()", "codegen", "System.Security.Cryptography.AsymmetricAlgorithm", "ExportSubjectPublicKeyInfo", True, "benchmark" },
    { 58, "System.Security.Cryptography/System.Security.Cryptography.AsymmetricAlgorithm::ExportPkcs8PrivateKeyPem:System.String()", "codegen", "System.Security.Cryptography.AsymmetricAlgorithm", "ExportPkcs8PrivateKeyPem", True, "benchmark" },
    { 59, "System.Security.Cryptography/System.Security.Cryptography.AsymmetricAlgorithm::ExportSubjectPublicKeyInfoPem:System.String()", "codegen", "System.Security.Cryptography.AsymmetricAlgorithm", "ExportSubjectPublicKeyInfoPem", True, "benchmark" },
    { 60, "System.Security.Cryptography/System.Security.Cryptography.AsymmetricKeyExchangeDeformatter::SetKey:System.Void(System.Security.Cryptography.AsymmetricAlgorithm)", "codegen", "System.Security.Cryptography.AsymmetricKeyExchangeDeformatter", "SetKey", True, "benchmark" },
    { 61, "System.Security.Cryptography/System.Security.Cryptography.AsymmetricKeyExchangeDeformatter::DecryptKeyExchange:System.Byte[](System.Byte[])", "codegen", "System.Security.Cryptography.AsymmetricKeyExchangeDeformatter", "DecryptKeyExchange", True, "benchmark" },
    { 62, "System.Security.Cryptography/System.Security.Cryptography.AsymmetricKeyExchangeDeformatter::DecryptKeyExchange:System.Byte[](System.Byte[])", "codegen", "System.Security.Cryptography.AsymmetricKeyExchangeDeformatter", "DecryptKeyExchange", True, "benchmark" },
    { 63, "System.Security.Cryptography/System.Security.Cryptography.AsymmetricKeyExchangeDeformatter::DecryptKeyExchange:System.Byte[](System.Byte[])", "codegen", "System.Security.Cryptography.AsymmetricKeyExchangeDeformatter", "DecryptKeyExchange", True, "benchmark" },
    { 64, "System.Security.Cryptography/System.Security.Cryptography.AsymmetricKeyExchangeFormatter::SetKey:System.Void(System.Security.Cryptography.AsymmetricAlgorithm)", "codegen", "System.Security.Cryptography.AsymmetricKeyExchangeFormatter", "SetKey", True, "benchmark" },
    { 65, "System.Security.Cryptography/System.Security.Cryptography.AsymmetricKeyExchangeFormatter::CreateKeyExchange:System.Byte[](System.Byte[])", "codegen", "System.Security.Cryptography.AsymmetricKeyExchangeFormatter", "CreateKeyExchange", True, "benchmark" },
    { 66, "System.Security.Cryptography/System.Security.Cryptography.AsymmetricKeyExchangeFormatter::CreateKeyExchange:System.Byte[](System.Byte[])", "codegen", "System.Security.Cryptography.AsymmetricKeyExchangeFormatter", "CreateKeyExchange", True, "benchmark" },
    { 67, "System.Security.Cryptography/System.Security.Cryptography.AsymmetricKeyExchangeFormatter::CreateKeyExchange:System.Byte[](System.Byte[])", "codegen", "System.Security.Cryptography.AsymmetricKeyExchangeFormatter", "CreateKeyExchange", True, "benchmark" },
    { 68, "System.Security.Cryptography/System.Security.Cryptography.AsymmetricKeyExchangeFormatter::CreateKeyExchange:System.Byte[](System.Byte[],System.Type)", "codegen", "System.Security.Cryptography.AsymmetricKeyExchangeFormatter", "CreateKeyExchange", True, "benchmark" },
    { 69, "System.Security.Cryptography/System.Security.Cryptography.AsymmetricKeyExchangeFormatter::CreateKeyExchange:System.Byte[](System.Byte[],System.Type)", "codegen", "System.Security.Cryptography.AsymmetricKeyExchangeFormatter", "CreateKeyExchange", True, "benchmark" },
    { 70, "System.Security.Cryptography/System.Security.Cryptography.AsymmetricKeyExchangeFormatter::CreateKeyExchange:System.Byte[](System.Byte[],System.Type)", "codegen", "System.Security.Cryptography.AsymmetricKeyExchangeFormatter", "CreateKeyExchange", True, "benchmark" },
    { 71, "System.Security.Cryptography/System.Security.Cryptography.AsymmetricSignatureDeformatter::SetKey:System.Void(System.Security.Cryptography.AsymmetricAlgorithm)", "codegen", "System.Security.Cryptography.AsymmetricSignatureDeformatter", "SetKey", True, "benchmark" },
    { 72, "System.Security.Cryptography/System.Security.Cryptography.AsymmetricSignatureDeformatter::SetHashAlgorithm:System.Void(System.String)", "codegen", "System.Security.Cryptography.AsymmetricSignatureDeformatter", "SetHashAlgorithm", True, "benchmark" },
    { 73, "System.Security.Cryptography/System.Security.Cryptography.AsymmetricSignatureDeformatter::SetHashAlgorithm:System.Void(System.String)", "codegen", "System.Security.Cryptography.AsymmetricSignatureDeformatter", "SetHashAlgorithm", True, "benchmark" },
    { 74, "System.Security.Cryptography/System.Security.Cryptography.AsymmetricSignatureDeformatter::VerifySignature:System.Boolean(System.Security.Cryptography.HashAlgorithm,System.Byte[])", "codegen", "System.Security.Cryptography.AsymmetricSignatureDeformatter", "VerifySignature", True, "benchmark" },
    { 75, "System.Security.Cryptography/System.Security.Cryptography.AsymmetricSignatureDeformatter::VerifySignature:System.Boolean(System.Security.Cryptography.HashAlgorithm,System.Byte[])", "codegen", "System.Security.Cryptography.AsymmetricSignatureDeformatter", "VerifySignature", True, "benchmark" },
    { 76, "System.Security.Cryptography/System.Security.Cryptography.AsymmetricSignatureDeformatter::VerifySignature:System.Boolean(System.Security.Cryptography.HashAlgorithm,System.Byte[])", "codegen", "System.Security.Cryptography.AsymmetricSignatureDeformatter", "VerifySignature", True, "benchmark" },
    { 77, "System.Security.Cryptography/System.Security.Cryptography.AsymmetricSignatureDeformatter::VerifySignature:System.Boolean(System.Byte[],System.Byte[])", "codegen", "System.Security.Cryptography.AsymmetricSignatureDeformatter", "VerifySignature", True, "benchmark" },
    { 78, "System.Security.Cryptography/System.Security.Cryptography.AsymmetricSignatureDeformatter::VerifySignature:System.Boolean(System.Byte[],System.Byte[])", "codegen", "System.Security.Cryptography.AsymmetricSignatureDeformatter", "VerifySignature", True, "benchmark" },
    { 79, "System.Security.Cryptography/System.Security.Cryptography.AsymmetricSignatureDeformatter::VerifySignature:System.Boolean(System.Byte[],System.Byte[])", "codegen", "System.Security.Cryptography.AsymmetricSignatureDeformatter", "VerifySignature", True, "benchmark" },
    { 80, "System.Security.Cryptography/System.Security.Cryptography.AsymmetricSignatureFormatter::SetKey:System.Void(System.Security.Cryptography.AsymmetricAlgorithm)", "codegen", "System.Security.Cryptography.AsymmetricSignatureFormatter", "SetKey", True, "benchmark" },
    { 81, "System.Security.Cryptography/System.Security.Cryptography.AsymmetricSignatureFormatter::SetHashAlgorithm:System.Void(System.String)", "codegen", "System.Security.Cryptography.AsymmetricSignatureFormatter", "SetHashAlgorithm", True, "benchmark" },
    { 82, "System.Security.Cryptography/System.Security.Cryptography.AsymmetricSignatureFormatter::SetHashAlgorithm:System.Void(System.String)", "codegen", "System.Security.Cryptography.AsymmetricSignatureFormatter", "SetHashAlgorithm", True, "benchmark" },
    { 83, "System.Security.Cryptography/System.Security.Cryptography.AsymmetricSignatureFormatter::CreateSignature:System.Byte[](System.Security.Cryptography.HashAlgorithm)", "codegen", "System.Security.Cryptography.AsymmetricSignatureFormatter", "CreateSignature", True, "benchmark" },
    { 84, "System.Security.Cryptography/System.Security.Cryptography.AsymmetricSignatureFormatter::CreateSignature:System.Byte[](System.Byte[])", "codegen", "System.Security.Cryptography.AsymmetricSignatureFormatter", "CreateSignature", True, "benchmark" },
    { 85, "System.Security.Cryptography/System.Security.Cryptography.AsymmetricSignatureFormatter::CreateSignature:System.Byte[](System.Byte[])", "codegen", "System.Security.Cryptography.AsymmetricSignatureFormatter", "CreateSignature", True, "benchmark" },
    { 86, "System.Security.Cryptography/System.Security.Cryptography.AsymmetricSignatureFormatter::CreateSignature:System.Byte[](System.Byte[])", "codegen", "System.Security.Cryptography.AsymmetricSignatureFormatter", "CreateSignature", True, "benchmark" },
    { 87, "System.Security.Cryptography/System.Security.Cryptography.ChaCha20Poly1305::Encrypt:System.Void(System.Byte[],System.Byte[],System.Byte[],System.Byte[],System.Byte[])", "codegen", "System.Security.Cryptography.ChaCha20Poly1305", "Encrypt", True, "benchmark" },
    { 88, "System.Security.Cryptography/System.Security.Cryptography.ChaCha20Poly1305::Encrypt:System.Void(System.Byte[],System.Byte[],System.Byte[],System.Byte[],System.Byte[])", "codegen", "System.Security.Cryptography.ChaCha20Poly1305", "Encrypt", True, "benchmark" },
    { 89, "System.Security.Cryptography/System.Security.Cryptography.ChaCha20Poly1305::Encrypt:System.Void(System.Byte[],System.Byte[],System.Byte[],System.Byte[],System.Byte[])", "codegen", "System.Security.Cryptography.ChaCha20Poly1305", "Encrypt", True, "benchmark" },
    { 90, "System.Security.Cryptography/System.Security.Cryptography.ChaCha20Poly1305::Decrypt:System.Void(System.Byte[],System.Byte[],System.Byte[],System.Byte[],System.Byte[])", "codegen", "System.Security.Cryptography.ChaCha20Poly1305", "Decrypt", True, "benchmark" },
    { 91, "System.Security.Cryptography/System.Security.Cryptography.ChaCha20Poly1305::Decrypt:System.Void(System.Byte[],System.Byte[],System.Byte[],System.Byte[],System.Byte[])", "codegen", "System.Security.Cryptography.ChaCha20Poly1305", "Decrypt", True, "benchmark" },
    { 92, "System.Security.Cryptography/System.Security.Cryptography.ChaCha20Poly1305::Decrypt:System.Void(System.Byte[],System.Byte[],System.Byte[],System.Byte[],System.Byte[])", "codegen", "System.Security.Cryptography.ChaCha20Poly1305", "Decrypt", True, "benchmark" },
    { 93, "System.Security.Cryptography/System.Security.Cryptography.ChaCha20Poly1305::Dispose:System.Void()", "codegen", "System.Security.Cryptography.ChaCha20Poly1305", "Dispose", True, "benchmark" },
    { 94, "System.Security.Cryptography/System.Security.Cryptography.CngKey::Dispose:System.Void()", "codegen", "System.Security.Cryptography.CngKey", "Dispose", True, "benchmark" },
    { 95, "System.Security.Cryptography/System.Security.Cryptography.CngKey::Delete:System.Void()", "codegen", "System.Security.Cryptography.CngKey", "Delete", True, "benchmark" },
    { 96, "System.Security.Cryptography/System.Security.Cryptography.CngKey::Exists:System.Boolean(System.String)", "codegen", "System.Security.Cryptography.CngKey", "Exists", True, "benchmark" },
    { 97, "System.Security.Cryptography/System.Security.Cryptography.CngKey::Exists:System.Boolean(System.String)", "codegen", "System.Security.Cryptography.CngKey", "Exists", True, "benchmark" },
    { 98, "System.Security.Cryptography/System.Security.Cryptography.CngKey::Exists:System.Boolean(System.String,System.Security.Cryptography.CngProvider)", "codegen", "System.Security.Cryptography.CngKey", "Exists", True, "benchmark" },
    { 99, "System.Security.Cryptography/System.Security.Cryptography.CngKey::Exists:System.Boolean(System.String,System.Security.Cryptography.CngProvider)", "codegen", "System.Security.Cryptography.CngKey", "Exists", True, "benchmark" },
    { 100, "System.Security.Cryptography/System.Security.Cryptography.CngKey::Exists:System.Boolean(System.String,System.Security.Cryptography.CngProvider,System.Security.Cryptography.CngKeyOpenOptions)", "codegen", "System.Security.Cryptography.CngKey", "Exists", True, "benchmark" },
    { 101, "System.Security.Cryptography/System.Security.Cryptography.CngKey::Exists:System.Boolean(System.String,System.Security.Cryptography.CngProvider,System.Security.Cryptography.CngKeyOpenOptions)", "codegen", "System.Security.Cryptography.CngKey", "Exists", True, "benchmark" },
    { 102, "System.Security.Cryptography/System.Security.Cryptography.CngKey::Export:System.Byte[](System.Security.Cryptography.CngKeyBlobFormat)", "codegen", "System.Security.Cryptography.CngKey", "Export", True, "benchmark" },
    { 103, "System.Security.Cryptography/System.Security.Cryptography.CngKey::Import:System.Security.Cryptography.CngKey(System.Byte[],System.Security.Cryptography.CngKeyBlobFormat)", "codegen", "System.Security.Cryptography.CngKey", "Import", True, "benchmark" },
    { 104, "System.Security.Cryptography/System.Security.Cryptography.CngKey::Import:System.Security.Cryptography.CngKey(System.Byte[],System.Security.Cryptography.CngKeyBlobFormat)", "codegen", "System.Security.Cryptography.CngKey", "Import", True, "benchmark" },
    { 105, "System.Security.Cryptography/System.Security.Cryptography.CngKey::Import:System.Security.Cryptography.CngKey(System.Byte[],System.Security.Cryptography.CngKeyBlobFormat)", "codegen", "System.Security.Cryptography.CngKey", "Import", True, "benchmark" },
    { 106, "System.Security.Cryptography/System.Security.Cryptography.CngKey::Import:System.Security.Cryptography.CngKey(System.Byte[],System.Security.Cryptography.CngKeyBlobFormat,System.Security.Cryptography.CngProvider)", "codegen", "System.Security.Cryptography.CngKey", "Import", True, "benchmark" },
    { 107, "System.Security.Cryptography/System.Security.Cryptography.CngKey::Import:System.Security.Cryptography.CngKey(System.Byte[],System.Security.Cryptography.CngKeyBlobFormat,System.Security.Cryptography.CngProvider)", "codegen", "System.Security.Cryptography.CngKey", "Import", True, "benchmark" },
    { 108, "System.Security.Cryptography/System.Security.Cryptography.CngKey::Import:System.Security.Cryptography.CngKey(System.Byte[],System.Security.Cryptography.CngKeyBlobFormat,System.Security.Cryptography.CngProvider)", "codegen", "System.Security.Cryptography.CngKey", "Import", True, "benchmark" },
    { 109, "System.Security.Cryptography/System.Security.Cryptography.CngKey::Open:System.Security.Cryptography.CngKey(System.String)", "codegen", "System.Security.Cryptography.CngKey", "Open", True, "benchmark" },
    { 110, "System.Security.Cryptography/System.Security.Cryptography.CngKey::Open:System.Security.Cryptography.CngKey(System.String)", "codegen", "System.Security.Cryptography.CngKey", "Open", True, "benchmark" },
    { 111, "System.Security.Cryptography/System.Security.Cryptography.CngKey::Open:System.Security.Cryptography.CngKey(System.String,System.Security.Cryptography.CngProvider)", "codegen", "System.Security.Cryptography.CngKey", "Open", True, "benchmark" },
    { 112, "System.Security.Cryptography/System.Security.Cryptography.CngKey::Open:System.Security.Cryptography.CngKey(System.String,System.Security.Cryptography.CngProvider)", "codegen", "System.Security.Cryptography.CngKey", "Open", True, "benchmark" },
    { 113, "System.Security.Cryptography/System.Security.Cryptography.CngKey::Open:System.Security.Cryptography.CngKey(System.String,System.Security.Cryptography.CngProvider,System.Security.Cryptography.CngKeyOpenOptions)", "codegen", "System.Security.Cryptography.CngKey", "Open", True, "benchmark" },
    { 114, "System.Security.Cryptography/System.Security.Cryptography.CngKey::Open:System.Security.Cryptography.CngKey(System.String,System.Security.Cryptography.CngProvider,System.Security.Cryptography.CngKeyOpenOptions)", "codegen", "System.Security.Cryptography.CngKey", "Open", True, "benchmark" },
    { 115, "System.Security.Cryptography/System.Security.Cryptography.CngKey::Open:System.Security.Cryptography.CngKey(Microsoft.Win32.SafeHandles.SafeNCryptKeyHandle,System.Security.Cryptography.CngKeyHandleOpenOptions)", "codegen", "System.Security.Cryptography.CngKey", "Open", True, "benchmark" },
    { 116, "System.Security.Cryptography/System.Security.Cryptography.CngKey::GetProperty:System.Security.Cryptography.CngProperty(System.String,System.Security.Cryptography.CngPropertyOptions)", "codegen", "System.Security.Cryptography.CngKey", "GetProperty", True, "benchmark" },
    { 117, "System.Security.Cryptography/System.Security.Cryptography.CngKey::GetProperty:System.Security.Cryptography.CngProperty(System.String,System.Security.Cryptography.CngPropertyOptions)", "codegen", "System.Security.Cryptography.CngKey", "GetProperty", True, "benchmark" },
    { 118, "System.Security.Cryptography/System.Security.Cryptography.CngKey::HasProperty:System.Boolean(System.String,System.Security.Cryptography.CngPropertyOptions)", "codegen", "System.Security.Cryptography.CngKey", "HasProperty", True, "benchmark" },
    { 119, "System.Security.Cryptography/System.Security.Cryptography.CngKey::HasProperty:System.Boolean(System.String,System.Security.Cryptography.CngPropertyOptions)", "codegen", "System.Security.Cryptography.CngKey", "HasProperty", True, "benchmark" },
    { 120, "System.Security.Cryptography/System.Security.Cryptography.CngKey::SetProperty:System.Void(System.Security.Cryptography.CngProperty)", "codegen", "System.Security.Cryptography.CngKey", "SetProperty", True, "benchmark" },
    { 121, "System.Security.Cryptography/System.Security.Cryptography.CngProperty::GetValue:System.Byte[]()", "codegen", "System.Security.Cryptography.CngProperty", "GetValue", True, "benchmark" },
    { 122, "System.Security.Cryptography/System.Security.Cryptography.CryptoConfig::AddAlgorithm:System.Void(System.Type,System.String[])", "codegen", "System.Security.Cryptography.CryptoConfig", "AddAlgorithm", True, "benchmark" },
    { 123, "System.Security.Cryptography/System.Security.Cryptography.CryptoConfig::AddAlgorithm:System.Void(System.Type,System.String[])", "codegen", "System.Security.Cryptography.CryptoConfig", "AddAlgorithm", True, "benchmark" },
    { 124, "System.Security.Cryptography/System.Security.Cryptography.CryptoConfig::AddAlgorithm:System.Void(System.Type,System.String[])", "codegen", "System.Security.Cryptography.CryptoConfig", "AddAlgorithm", True, "benchmark" },
    { 125, "System.Security.Cryptography/System.Security.Cryptography.CryptoConfig::CreateFromName:System.Object(System.String,System.Object[])", "codegen", "System.Security.Cryptography.CryptoConfig", "CreateFromName", True, "benchmark" },
    { 126, "System.Security.Cryptography/System.Security.Cryptography.CryptoConfig::CreateFromName:System.Object(System.String,System.Object[])", "codegen", "System.Security.Cryptography.CryptoConfig", "CreateFromName", True, "benchmark" },
    { 127, "System.Security.Cryptography/System.Security.Cryptography.CryptoConfig::CreateFromName:System.Object(System.String,System.Object[])", "codegen", "System.Security.Cryptography.CryptoConfig", "CreateFromName", True, "benchmark" },
    { 128, "System.Security.Cryptography/System.Security.Cryptography.CryptoConfig::CreateFromName:System.Object(System.String,System.Object[])", "codegen", "System.Security.Cryptography.CryptoConfig", "CreateFromName", True, "benchmark" },
    { 129, "System.Security.Cryptography/System.Security.Cryptography.CryptoConfig::CreateFromName:System.Object(System.String)", "codegen", "System.Security.Cryptography.CryptoConfig", "CreateFromName", True, "benchmark" },
    { 130, "System.Security.Cryptography/System.Security.Cryptography.CryptoConfig::CreateFromName:System.Object(System.String)", "codegen", "System.Security.Cryptography.CryptoConfig", "CreateFromName", True, "benchmark" },
    { 131, "System.Security.Cryptography/System.Security.Cryptography.CryptoConfig::AddOID:System.Void(System.String,System.String[])", "codegen", "System.Security.Cryptography.CryptoConfig", "AddOID", True, "benchmark" },
    { 132, "System.Security.Cryptography/System.Security.Cryptography.CryptoConfig::AddOID:System.Void(System.String,System.String[])", "codegen", "System.Security.Cryptography.CryptoConfig", "AddOID", True, "benchmark" },
    { 133, "System.Security.Cryptography/System.Security.Cryptography.CryptoConfig::AddOID:System.Void(System.String,System.String[])", "codegen", "System.Security.Cryptography.CryptoConfig", "AddOID", True, "benchmark" },
    { 134, "System.Security.Cryptography/System.Security.Cryptography.CryptoConfig::AddOID:System.Void(System.String,System.String[])", "codegen", "System.Security.Cryptography.CryptoConfig", "AddOID", True, "benchmark" },
    { 135, "System.Security.Cryptography/System.Security.Cryptography.CryptoConfig::MapNameToOID:System.String(System.String)", "codegen", "System.Security.Cryptography.CryptoConfig", "MapNameToOID", True, "benchmark" },
    { 136, "System.Security.Cryptography/System.Security.Cryptography.CryptoConfig::MapNameToOID:System.String(System.String)", "codegen", "System.Security.Cryptography.CryptoConfig", "MapNameToOID", True, "benchmark" },
    { 137, "System.Security.Cryptography/System.Security.Cryptography.CryptoConfig::EncodeOID:System.Byte[](System.String)", "codegen", "System.Security.Cryptography.CryptoConfig", "EncodeOID", True, "benchmark" },
    { 138, "System.Security.Cryptography/System.Security.Cryptography.CryptoConfig::EncodeOID:System.Byte[](System.String)", "codegen", "System.Security.Cryptography.CryptoConfig", "EncodeOID", True, "benchmark" },
    { 139, "System.Security.Cryptography/System.Security.Cryptography.CryptographicOperations::HashData:System.Byte[](System.Security.Cryptography.HashAlgorithmName,System.Byte[])", "codegen", "System.Security.Cryptography.CryptographicOperations", "HashData", True, "benchmark" },
    { 140, "System.Security.Cryptography/System.Security.Cryptography.CryptographicOperations::HashData:System.Byte[](System.Security.Cryptography.HashAlgorithmName,System.Byte[])", "codegen", "System.Security.Cryptography.CryptographicOperations", "HashData", True, "benchmark" },
    { 141, "System.Security.Cryptography/System.Security.Cryptography.CryptographicOperations::HashData:System.Byte[](System.Security.Cryptography.HashAlgorithmName,System.Byte[])", "codegen", "System.Security.Cryptography.CryptographicOperations", "HashData", True, "benchmark" },
    { 142, "System.Security.Cryptography/System.Security.Cryptography.CryptographicOperations::HashData:System.Byte[](System.Security.Cryptography.HashAlgorithmName,System.IO.Stream)", "codegen", "System.Security.Cryptography.CryptographicOperations", "HashData", True, "benchmark" },
    { 143, "System.Security.Cryptography/System.Security.Cryptography.CryptographicOperations::HashData:System.Byte[](System.Security.Cryptography.HashAlgorithmName,System.IO.Stream)", "codegen", "System.Security.Cryptography.CryptographicOperations", "HashData", True, "benchmark" },
    { 144, "System.Security.Cryptography/System.Security.Cryptography.CryptographicOperations::HashDataAsync:System.Threading.Tasks.ValueTask<System.Int32>(System.Security.Cryptography.HashAlgorithmName,System.IO.Stream,System.Memory<System.Byte>,System.Threading.CancellationToken)", "codegen", "System.Security.Cryptography.CryptographicOperations", "HashDataAsync", True, "benchmark" },
    { 145, "System.Security.Cryptography/System.Security.Cryptography.CryptographicOperations::HashDataAsync:System.Threading.Tasks.ValueTask<System.Int32>(System.Security.Cryptography.HashAlgorithmName,System.IO.Stream,System.Memory<System.Byte>,System.Threading.CancellationToken)", "codegen", "System.Security.Cryptography.CryptographicOperations", "HashDataAsync", True, "benchmark" },
    { 146, "System.Security.Cryptography/System.Security.Cryptography.CryptographicOperations::HashDataAsync:System.Threading.Tasks.ValueTask<System.Byte[]>(System.Security.Cryptography.HashAlgorithmName,System.IO.Stream,System.Threading.CancellationToken)", "codegen", "System.Security.Cryptography.CryptographicOperations", "HashDataAsync", True, "benchmark" },
    { 147, "System.Security.Cryptography/System.Security.Cryptography.CryptographicOperations::HashDataAsync:System.Threading.Tasks.ValueTask<System.Byte[]>(System.Security.Cryptography.HashAlgorithmName,System.IO.Stream,System.Threading.CancellationToken)", "codegen", "System.Security.Cryptography.CryptographicOperations", "HashDataAsync", True, "benchmark" },
    { 148, "System.Security.Cryptography/System.Security.Cryptography.CryptographicOperations::HmacData:System.Byte[](System.Security.Cryptography.HashAlgorithmName,System.Byte[],System.Byte[])", "codegen", "System.Security.Cryptography.CryptographicOperations", "HmacData", True, "benchmark" },
    { 149, "System.Security.Cryptography/System.Security.Cryptography.CryptographicOperations::HmacData:System.Byte[](System.Security.Cryptography.HashAlgorithmName,System.Byte[],System.Byte[])", "codegen", "System.Security.Cryptography.CryptographicOperations", "HmacData", True, "benchmark" },
    { 150, "System.Security.Cryptography/System.Security.Cryptography.CryptographicOperations::HmacData:System.Byte[](System.Security.Cryptography.HashAlgorithmName,System.Byte[],System.Byte[])", "codegen", "System.Security.Cryptography.CryptographicOperations", "HmacData", True, "benchmark" },
    { 151, "System.Security.Cryptography/System.Security.Cryptography.CryptographicOperations::HmacData:System.Byte[](System.Security.Cryptography.HashAlgorithmName,System.Byte[],System.IO.Stream)", "codegen", "System.Security.Cryptography.CryptographicOperations", "HmacData", True, "benchmark" },
    { 152, "System.Security.Cryptography/System.Security.Cryptography.CryptographicOperations::HmacData:System.Byte[](System.Security.Cryptography.HashAlgorithmName,System.Byte[],System.IO.Stream)", "codegen", "System.Security.Cryptography.CryptographicOperations", "HmacData", True, "benchmark" },
    { 153, "System.Security.Cryptography/System.Security.Cryptography.CryptographicOperations::HmacData:System.Byte[](System.Security.Cryptography.HashAlgorithmName,System.Byte[],System.IO.Stream)", "codegen", "System.Security.Cryptography.CryptographicOperations", "HmacData", True, "benchmark" },
    { 154, "System.Security.Cryptography/System.Security.Cryptography.CryptographicOperations::HmacDataAsync:System.Threading.Tasks.ValueTask<System.Byte[]>(System.Security.Cryptography.HashAlgorithmName,System.Byte[],System.IO.Stream,System.Threading.CancellationToken)", "codegen", "System.Security.Cryptography.CryptographicOperations", "HmacDataAsync", True, "benchmark" },
    { 155, "System.Security.Cryptography/System.Security.Cryptography.CryptographicOperations::HmacDataAsync:System.Threading.Tasks.ValueTask<System.Byte[]>(System.Security.Cryptography.HashAlgorithmName,System.Byte[],System.IO.Stream,System.Threading.CancellationToken)", "codegen", "System.Security.Cryptography.CryptographicOperations", "HmacDataAsync", True, "benchmark" },
    { 156, "System.Security.Cryptography/System.Security.Cryptography.CryptographicOperations::HmacDataAsync:System.Threading.Tasks.ValueTask<System.Byte[]>(System.Security.Cryptography.HashAlgorithmName,System.Byte[],System.IO.Stream,System.Threading.CancellationToken)", "codegen", "System.Security.Cryptography.CryptographicOperations", "HmacDataAsync", True, "benchmark" },
    { 157, "System.Security.Cryptography/System.Security.Cryptography.CryptographicOperations::HmacDataAsync:System.Threading.Tasks.ValueTask<System.Int32>(System.Security.Cryptography.HashAlgorithmName,System.ReadOnlyMemory<System.Byte>,System.IO.Stream,System.Memory<System.Byte>,System.Threading.CancellationToken)", "codegen", "System.Security.Cryptography.CryptographicOperations", "HmacDataAsync", True, "benchmark" },
    { 158, "System.Security.Cryptography/System.Security.Cryptography.CryptographicOperations::HmacDataAsync:System.Threading.Tasks.ValueTask<System.Int32>(System.Security.Cryptography.HashAlgorithmName,System.ReadOnlyMemory<System.Byte>,System.IO.Stream,System.Memory<System.Byte>,System.Threading.CancellationToken)", "codegen", "System.Security.Cryptography.CryptographicOperations", "HmacDataAsync", True, "benchmark" },
    { 159, "System.Security.Cryptography/System.Security.Cryptography.CryptographicOperations::HmacDataAsync:System.Threading.Tasks.ValueTask<System.Byte[]>(System.Security.Cryptography.HashAlgorithmName,System.ReadOnlyMemory<System.Byte>,System.IO.Stream,System.Threading.CancellationToken)", "codegen", "System.Security.Cryptography.CryptographicOperations", "HmacDataAsync", True, "benchmark" },
    { 160, "System.Security.Cryptography/System.Security.Cryptography.CryptographicOperations::HmacDataAsync:System.Threading.Tasks.ValueTask<System.Byte[]>(System.Security.Cryptography.HashAlgorithmName,System.ReadOnlyMemory<System.Byte>,System.IO.Stream,System.Threading.CancellationToken)", "codegen", "System.Security.Cryptography.CryptographicOperations", "HmacDataAsync", True, "benchmark" },
    { 161, "System.Security.Cryptography/System.Security.Cryptography.CryptoStream::FlushFinalBlock:System.Void()", "codegen", "System.Security.Cryptography.CryptoStream", "FlushFinalBlock", True, "benchmark" },
    { 162, "System.Security.Cryptography/System.Security.Cryptography.CryptoStream::FlushFinalBlockAsync:System.Threading.Tasks.ValueTask(System.Threading.CancellationToken)", "codegen", "System.Security.Cryptography.CryptoStream", "FlushFinalBlockAsync", True, "benchmark" },
    { 163, "System.Security.Cryptography/System.Security.Cryptography.CryptoStream::Flush:System.Void()", "codegen", "System.Security.Cryptography.CryptoStream", "Flush", True, "benchmark" },
    { 164, "System.Security.Cryptography/System.Security.Cryptography.CryptoStream::FlushAsync:System.Threading.Tasks.Task(System.Threading.CancellationToken)", "codegen", "System.Security.Cryptography.CryptoStream", "FlushAsync", True, "benchmark" },
    { 165, "System.Security.Cryptography/System.Security.Cryptography.CryptoStream::Seek:System.Int64(System.Int64,System.IO.SeekOrigin)", "codegen", "System.Security.Cryptography.CryptoStream", "Seek", True, "benchmark" },
    { 166, "System.Security.Cryptography/System.Security.Cryptography.CryptoStream::Seek:System.Int64(System.Int64,System.IO.SeekOrigin)", "codegen", "System.Security.Cryptography.CryptoStream", "Seek", True, "benchmark" },
    { 167, "System.Security.Cryptography/System.Security.Cryptography.CryptoStream::Seek:System.Int64(System.Int64,System.IO.SeekOrigin)", "codegen", "System.Security.Cryptography.CryptoStream", "Seek", True, "benchmark" },
    { 168, "System.Security.Cryptography/System.Security.Cryptography.CryptoStream::SetLength:System.Void(System.Int64)", "codegen", "System.Security.Cryptography.CryptoStream", "SetLength", True, "benchmark" },
    { 169, "System.Security.Cryptography/System.Security.Cryptography.CryptoStream::SetLength:System.Void(System.Int64)", "codegen", "System.Security.Cryptography.CryptoStream", "SetLength", True, "benchmark" },
    { 170, "System.Security.Cryptography/System.Security.Cryptography.CryptoStream::ReadAsync:System.Threading.Tasks.Task<System.Int32>(System.Byte[],System.Int32,System.Int32,System.Threading.CancellationToken)", "codegen", "System.Security.Cryptography.CryptoStream", "ReadAsync", True, "benchmark" },
    { 171, "System.Security.Cryptography/System.Security.Cryptography.CryptoStream::ReadAsync:System.Threading.Tasks.Task<System.Int32>(System.Byte[],System.Int32,System.Int32,System.Threading.CancellationToken)", "codegen", "System.Security.Cryptography.CryptoStream", "ReadAsync", True, "benchmark" },
    { 172, "System.Security.Cryptography/System.Security.Cryptography.CryptoStream::ReadAsync:System.Threading.Tasks.Task<System.Int32>(System.Byte[],System.Int32,System.Int32,System.Threading.CancellationToken)", "codegen", "System.Security.Cryptography.CryptoStream", "ReadAsync", True, "benchmark" },
    { 173, "System.Security.Cryptography/System.Security.Cryptography.CryptoStream::ReadAsync:System.Threading.Tasks.Task<System.Int32>(System.Byte[],System.Int32,System.Int32,System.Threading.CancellationToken)", "codegen", "System.Security.Cryptography.CryptoStream", "ReadAsync", True, "benchmark" },
    { 174, "System.Security.Cryptography/System.Security.Cryptography.CryptoStream::ReadAsync:System.Threading.Tasks.Task<System.Int32>(System.Byte[],System.Int32,System.Int32,System.Threading.CancellationToken)", "codegen", "System.Security.Cryptography.CryptoStream", "ReadAsync", True, "benchmark" },
    { 175, "System.Security.Cryptography/System.Security.Cryptography.CryptoStream::ReadAsync:System.Threading.Tasks.ValueTask<System.Int32>(System.Memory<System.Byte>,System.Threading.CancellationToken)", "codegen", "System.Security.Cryptography.CryptoStream", "ReadAsync", True, "benchmark" },
    { 176, "System.Security.Cryptography/System.Security.Cryptography.CryptoStream::BeginRead:System.IAsyncResult(System.Byte[],System.Int32,System.Int32,System.AsyncCallback,System.Object)", "codegen", "System.Security.Cryptography.CryptoStream", "BeginRead", True, "benchmark" },
    { 177, "System.Security.Cryptography/System.Security.Cryptography.CryptoStream::BeginRead:System.IAsyncResult(System.Byte[],System.Int32,System.Int32,System.AsyncCallback,System.Object)", "codegen", "System.Security.Cryptography.CryptoStream", "BeginRead", True, "benchmark" },
    { 178, "System.Security.Cryptography/System.Security.Cryptography.CryptoStream::BeginRead:System.IAsyncResult(System.Byte[],System.Int32,System.Int32,System.AsyncCallback,System.Object)", "codegen", "System.Security.Cryptography.CryptoStream", "BeginRead", True, "benchmark" },
    { 179, "System.Security.Cryptography/System.Security.Cryptography.CryptoStream::BeginRead:System.IAsyncResult(System.Byte[],System.Int32,System.Int32,System.AsyncCallback,System.Object)", "codegen", "System.Security.Cryptography.CryptoStream", "BeginRead", True, "benchmark" },
    { 180, "System.Security.Cryptography/System.Security.Cryptography.CryptoStream::BeginRead:System.IAsyncResult(System.Byte[],System.Int32,System.Int32,System.AsyncCallback,System.Object)", "codegen", "System.Security.Cryptography.CryptoStream", "BeginRead", True, "benchmark" },
    { 181, "System.Security.Cryptography/System.Security.Cryptography.CryptoStream::EndRead:System.Int32(System.IAsyncResult)", "codegen", "System.Security.Cryptography.CryptoStream", "EndRead", True, "benchmark" },
    { 182, "System.Security.Cryptography/System.Security.Cryptography.CryptoStream::ReadByte:System.Int32()", "codegen", "System.Security.Cryptography.CryptoStream", "ReadByte", True, "benchmark" },
    { 183, "System.Security.Cryptography/System.Security.Cryptography.CryptoStream::WriteByte:System.Void(System.Byte)", "codegen", "System.Security.Cryptography.CryptoStream", "WriteByte", True, "benchmark" },
    { 184, "System.Security.Cryptography/System.Security.Cryptography.CryptoStream::WriteByte:System.Void(System.Byte)", "codegen", "System.Security.Cryptography.CryptoStream", "WriteByte", True, "benchmark" },
    { 185, "System.Security.Cryptography/System.Security.Cryptography.CryptoStream::Read:System.Int32(System.Byte[],System.Int32,System.Int32)", "codegen", "System.Security.Cryptography.CryptoStream", "Read", True, "benchmark" },
    { 186, "System.Security.Cryptography/System.Security.Cryptography.CryptoStream::Read:System.Int32(System.Byte[],System.Int32,System.Int32)", "codegen", "System.Security.Cryptography.CryptoStream", "Read", True, "benchmark" },
    { 187, "System.Security.Cryptography/System.Security.Cryptography.CryptoStream::Read:System.Int32(System.Byte[],System.Int32,System.Int32)", "codegen", "System.Security.Cryptography.CryptoStream", "Read", True, "benchmark" },
    { 188, "System.Security.Cryptography/System.Security.Cryptography.CryptoStream::Read:System.Int32(System.Byte[],System.Int32,System.Int32)", "codegen", "System.Security.Cryptography.CryptoStream", "Read", True, "benchmark" },
    { 189, "System.Security.Cryptography/System.Security.Cryptography.CryptoStream::Read:System.Int32(System.Byte[],System.Int32,System.Int32)", "codegen", "System.Security.Cryptography.CryptoStream", "Read", True, "benchmark" },
    { 190, "System.Security.Cryptography/System.Security.Cryptography.CryptoStream::WriteAsync:System.Threading.Tasks.Task(System.Byte[],System.Int32,System.Int32,System.Threading.CancellationToken)", "codegen", "System.Security.Cryptography.CryptoStream", "WriteAsync", True, "benchmark" },
    { 191, "System.Security.Cryptography/System.Security.Cryptography.CryptoStream::WriteAsync:System.Threading.Tasks.Task(System.Byte[],System.Int32,System.Int32,System.Threading.CancellationToken)", "codegen", "System.Security.Cryptography.CryptoStream", "WriteAsync", True, "benchmark" },
    { 192, "System.Security.Cryptography/System.Security.Cryptography.CryptoStream::WriteAsync:System.Threading.Tasks.Task(System.Byte[],System.Int32,System.Int32,System.Threading.CancellationToken)", "codegen", "System.Security.Cryptography.CryptoStream", "WriteAsync", True, "benchmark" },
    { 193, "System.Security.Cryptography/System.Security.Cryptography.CryptoStream::WriteAsync:System.Threading.Tasks.Task(System.Byte[],System.Int32,System.Int32,System.Threading.CancellationToken)", "codegen", "System.Security.Cryptography.CryptoStream", "WriteAsync", True, "benchmark" },
    { 194, "System.Security.Cryptography/System.Security.Cryptography.CryptoStream::WriteAsync:System.Threading.Tasks.Task(System.Byte[],System.Int32,System.Int32,System.Threading.CancellationToken)", "codegen", "System.Security.Cryptography.CryptoStream", "WriteAsync", True, "benchmark" },
    { 195, "System.Security.Cryptography/System.Security.Cryptography.CryptoStream::WriteAsync:System.Threading.Tasks.ValueTask(System.ReadOnlyMemory<System.Byte>,System.Threading.CancellationToken)", "codegen", "System.Security.Cryptography.CryptoStream", "WriteAsync", True, "benchmark" },
    { 196, "System.Security.Cryptography/System.Security.Cryptography.CryptoStream::BeginWrite:System.IAsyncResult(System.Byte[],System.Int32,System.Int32,System.AsyncCallback,System.Object)", "codegen", "System.Security.Cryptography.CryptoStream", "BeginWrite", True, "benchmark" },
    { 197, "System.Security.Cryptography/System.Security.Cryptography.CryptoStream::BeginWrite:System.IAsyncResult(System.Byte[],System.Int32,System.Int32,System.AsyncCallback,System.Object)", "codegen", "System.Security.Cryptography.CryptoStream", "BeginWrite", True, "benchmark" },
    { 198, "System.Security.Cryptography/System.Security.Cryptography.CryptoStream::BeginWrite:System.IAsyncResult(System.Byte[],System.Int32,System.Int32,System.AsyncCallback,System.Object)", "codegen", "System.Security.Cryptography.CryptoStream", "BeginWrite", True, "benchmark" },
    { 199, "System.Security.Cryptography/System.Security.Cryptography.CryptoStream::BeginWrite:System.IAsyncResult(System.Byte[],System.Int32,System.Int32,System.AsyncCallback,System.Object)", "codegen", "System.Security.Cryptography.CryptoStream", "BeginWrite", True, "benchmark" },
    { 200, "System.Security.Cryptography/System.Security.Cryptography.CryptoStream::BeginWrite:System.IAsyncResult(System.Byte[],System.Int32,System.Int32,System.AsyncCallback,System.Object)", "codegen", "System.Security.Cryptography.CryptoStream", "BeginWrite", True, "benchmark" },
    { 201, "System.Security.Cryptography/System.Security.Cryptography.CryptoStream::EndWrite:System.Void(System.IAsyncResult)", "codegen", "System.Security.Cryptography.CryptoStream", "EndWrite", True, "benchmark" },
    { 202, "System.Security.Cryptography/System.Security.Cryptography.CryptoStream::Write:System.Void(System.Byte[],System.Int32,System.Int32)", "codegen", "System.Security.Cryptography.CryptoStream", "Write", True, "benchmark" },
    { 203, "System.Security.Cryptography/System.Security.Cryptography.CryptoStream::Write:System.Void(System.Byte[],System.Int32,System.Int32)", "codegen", "System.Security.Cryptography.CryptoStream", "Write", True, "benchmark" },
    { 204, "System.Security.Cryptography/System.Security.Cryptography.CryptoStream::Write:System.Void(System.Byte[],System.Int32,System.Int32)", "codegen", "System.Security.Cryptography.CryptoStream", "Write", True, "benchmark" },
    { 205, "System.Security.Cryptography/System.Security.Cryptography.CryptoStream::Write:System.Void(System.Byte[],System.Int32,System.Int32)", "codegen", "System.Security.Cryptography.CryptoStream", "Write", True, "benchmark" },
    { 206, "System.Security.Cryptography/System.Security.Cryptography.CryptoStream::Write:System.Void(System.Byte[],System.Int32,System.Int32)", "codegen", "System.Security.Cryptography.CryptoStream", "Write", True, "benchmark" },
    { 207, "System.Security.Cryptography/System.Security.Cryptography.CryptoStream::CopyTo:System.Void(System.IO.Stream,System.Int32)", "codegen", "System.Security.Cryptography.CryptoStream", "CopyTo", True, "benchmark" },
    { 208, "System.Security.Cryptography/System.Security.Cryptography.CryptoStream::CopyTo:System.Void(System.IO.Stream,System.Int32)", "codegen", "System.Security.Cryptography.CryptoStream", "CopyTo", True, "benchmark" },
    { 209, "System.Security.Cryptography/System.Security.Cryptography.CryptoStream::CopyTo:System.Void(System.IO.Stream,System.Int32)", "codegen", "System.Security.Cryptography.CryptoStream", "CopyTo", True, "benchmark" },
    { 210, "System.Security.Cryptography/System.Security.Cryptography.CryptoStream::CopyToAsync:System.Threading.Tasks.Task(System.IO.Stream,System.Int32,System.Threading.CancellationToken)", "codegen", "System.Security.Cryptography.CryptoStream", "CopyToAsync", True, "benchmark" },
    { 211, "System.Security.Cryptography/System.Security.Cryptography.CryptoStream::CopyToAsync:System.Threading.Tasks.Task(System.IO.Stream,System.Int32,System.Threading.CancellationToken)", "codegen", "System.Security.Cryptography.CryptoStream", "CopyToAsync", True, "benchmark" },
    { 212, "System.Security.Cryptography/System.Security.Cryptography.CryptoStream::CopyToAsync:System.Threading.Tasks.Task(System.IO.Stream,System.Int32,System.Threading.CancellationToken)", "codegen", "System.Security.Cryptography.CryptoStream", "CopyToAsync", True, "benchmark" },
    { 213, "System.Security.Cryptography/System.Security.Cryptography.CryptoStream::Clear:System.Void()", "codegen", "System.Security.Cryptography.CryptoStream", "Clear", True, "benchmark" },
    { 214, "System.Security.Cryptography/System.Security.Cryptography.CryptoStream::DisposeAsync:System.Threading.Tasks.ValueTask()", "codegen", "System.Security.Cryptography.CryptoStream", "DisposeAsync", True, "benchmark" },
    { 215, "System.Security.Cryptography/System.Security.Cryptography.DeriveBytes::GetBytes:System.Byte[](System.Int32)", "codegen", "System.Security.Cryptography.DeriveBytes", "GetBytes", True, "benchmark" },
    { 216, "System.Security.Cryptography/System.Security.Cryptography.DeriveBytes::GetBytes:System.Byte[](System.Int32)", "codegen", "System.Security.Cryptography.DeriveBytes", "GetBytes", True, "benchmark" },
    { 217, "System.Security.Cryptography/System.Security.Cryptography.DeriveBytes::Dispose:System.Void()", "codegen", "System.Security.Cryptography.DeriveBytes", "Dispose", True, "benchmark" },
    { 218, "System.Security.Cryptography/System.Security.Cryptography.DES::IsWeakKey:System.Boolean(System.Byte[])", "codegen", "System.Security.Cryptography.DES", "IsWeakKey", True, "benchmark" },
    { 219, "System.Security.Cryptography/System.Security.Cryptography.DES::IsWeakKey:System.Boolean(System.Byte[])", "codegen", "System.Security.Cryptography.DES", "IsWeakKey", True, "benchmark" },
    { 220, "System.Security.Cryptography/System.Security.Cryptography.DES::IsWeakKey:System.Boolean(System.Byte[])", "codegen", "System.Security.Cryptography.DES", "IsWeakKey", True, "benchmark" },
    { 221, "System.Security.Cryptography/System.Security.Cryptography.DES::IsSemiWeakKey:System.Boolean(System.Byte[])", "codegen", "System.Security.Cryptography.DES", "IsSemiWeakKey", True, "benchmark" },
    { 222, "System.Security.Cryptography/System.Security.Cryptography.DES::IsSemiWeakKey:System.Boolean(System.Byte[])", "codegen", "System.Security.Cryptography.DES", "IsSemiWeakKey", True, "benchmark" },
    { 223, "System.Security.Cryptography/System.Security.Cryptography.DES::IsSemiWeakKey:System.Boolean(System.Byte[])", "codegen", "System.Security.Cryptography.DES", "IsSemiWeakKey", True, "benchmark" },
    { 224, "System.Security.Cryptography/System.Security.Cryptography.DESCryptoServiceProvider::GenerateKey:System.Void()", "codegen", "System.Security.Cryptography.DESCryptoServiceProvider", "GenerateKey", True, "benchmark" },
    { 225, "System.Security.Cryptography/System.Security.Cryptography.DESCryptoServiceProvider::GenerateIV:System.Void()", "codegen", "System.Security.Cryptography.DESCryptoServiceProvider", "GenerateIV", True, "benchmark" },
    { 226, "System.Security.Cryptography/System.Security.Cryptography.DESCryptoServiceProvider::CreateDecryptor:System.Security.Cryptography.ICryptoTransform()", "codegen", "System.Security.Cryptography.DESCryptoServiceProvider", "CreateDecryptor", True, "benchmark" },
    { 227, "System.Security.Cryptography/System.Security.Cryptography.DESCryptoServiceProvider::CreateDecryptor:System.Security.Cryptography.ICryptoTransform(System.Byte[],System.Byte[])", "codegen", "System.Security.Cryptography.DESCryptoServiceProvider", "CreateDecryptor", True, "benchmark" },
    { 228, "System.Security.Cryptography/System.Security.Cryptography.DESCryptoServiceProvider::CreateDecryptor:System.Security.Cryptography.ICryptoTransform(System.Byte[],System.Byte[])", "codegen", "System.Security.Cryptography.DESCryptoServiceProvider", "CreateDecryptor", True, "benchmark" },
    { 229, "System.Security.Cryptography/System.Security.Cryptography.DESCryptoServiceProvider::CreateDecryptor:System.Security.Cryptography.ICryptoTransform(System.Byte[],System.Byte[])", "codegen", "System.Security.Cryptography.DESCryptoServiceProvider", "CreateDecryptor", True, "benchmark" },
    { 230, "System.Security.Cryptography/System.Security.Cryptography.DESCryptoServiceProvider::CreateEncryptor:System.Security.Cryptography.ICryptoTransform()", "codegen", "System.Security.Cryptography.DESCryptoServiceProvider", "CreateEncryptor", True, "benchmark" },
    { 231, "System.Security.Cryptography/System.Security.Cryptography.DESCryptoServiceProvider::CreateEncryptor:System.Security.Cryptography.ICryptoTransform(System.Byte[],System.Byte[])", "codegen", "System.Security.Cryptography.DESCryptoServiceProvider", "CreateEncryptor", True, "benchmark" },
    { 232, "System.Security.Cryptography/System.Security.Cryptography.DESCryptoServiceProvider::CreateEncryptor:System.Security.Cryptography.ICryptoTransform(System.Byte[],System.Byte[])", "codegen", "System.Security.Cryptography.DESCryptoServiceProvider", "CreateEncryptor", True, "benchmark" },
    { 233, "System.Security.Cryptography/System.Security.Cryptography.DESCryptoServiceProvider::CreateEncryptor:System.Security.Cryptography.ICryptoTransform(System.Byte[],System.Byte[])", "codegen", "System.Security.Cryptography.DESCryptoServiceProvider", "CreateEncryptor", True, "benchmark" },
    { 234, "System.Security.Cryptography/System.Security.Cryptography.DSA::ExportParameters:System.Security.Cryptography.DSAParameters(System.Boolean)", "codegen", "System.Security.Cryptography.DSA", "ExportParameters", True, "benchmark" },
    { 235, "System.Security.Cryptography/System.Security.Cryptography.DSA::ExportParameters:System.Security.Cryptography.DSAParameters(System.Boolean)", "codegen", "System.Security.Cryptography.DSA", "ExportParameters", True, "benchmark" },
    { 236, "System.Security.Cryptography/System.Security.Cryptography.DSA::ImportParameters:System.Void(System.Security.Cryptography.DSAParameters)", "codegen", "System.Security.Cryptography.DSA", "ImportParameters", True, "benchmark" },
    { 237, "System.Security.Cryptography/System.Security.Cryptography.DSA::CreateSignature:System.Byte[](System.Byte[])", "codegen", "System.Security.Cryptography.DSA", "CreateSignature", True, "benchmark" },
    { 238, "System.Security.Cryptography/System.Security.Cryptography.DSA::CreateSignature:System.Byte[](System.Byte[])", "codegen", "System.Security.Cryptography.DSA", "CreateSignature", True, "benchmark" },
    { 239, "System.Security.Cryptography/System.Security.Cryptography.DSA::CreateSignature:System.Byte[](System.Byte[])", "codegen", "System.Security.Cryptography.DSA", "CreateSignature", True, "benchmark" },
    { 240, "System.Security.Cryptography/System.Security.Cryptography.DSA::VerifySignature:System.Boolean(System.Byte[],System.Byte[])", "codegen", "System.Security.Cryptography.DSA", "VerifySignature", True, "benchmark" },
    { 241, "System.Security.Cryptography/System.Security.Cryptography.DSA::VerifySignature:System.Boolean(System.Byte[],System.Byte[])", "codegen", "System.Security.Cryptography.DSA", "VerifySignature", True, "benchmark" },
    { 242, "System.Security.Cryptography/System.Security.Cryptography.DSA::VerifySignature:System.Boolean(System.Byte[],System.Byte[])", "codegen", "System.Security.Cryptography.DSA", "VerifySignature", True, "benchmark" },
    { 243, "System.Security.Cryptography/System.Security.Cryptography.DSA::SignData:System.Byte[](System.Byte[],System.Security.Cryptography.HashAlgorithmName)", "codegen", "System.Security.Cryptography.DSA", "SignData", True, "benchmark" },
    { 244, "System.Security.Cryptography/System.Security.Cryptography.DSA::SignData:System.Byte[](System.Byte[],System.Security.Cryptography.HashAlgorithmName)", "codegen", "System.Security.Cryptography.DSA", "SignData", True, "benchmark" },
    { 245, "System.Security.Cryptography/System.Security.Cryptography.DSA::SignData:System.Byte[](System.Byte[],System.Security.Cryptography.HashAlgorithmName)", "codegen", "System.Security.Cryptography.DSA", "SignData", True, "benchmark" },
    { 246, "System.Security.Cryptography/System.Security.Cryptography.DSA::SignData:System.Byte[](System.Byte[],System.Security.Cryptography.HashAlgorithmName,System.Security.Cryptography.DSASignatureFormat)", "codegen", "System.Security.Cryptography.DSA", "SignData", True, "benchmark" },
    { 247, "System.Security.Cryptography/System.Security.Cryptography.DSA::SignData:System.Byte[](System.Byte[],System.Security.Cryptography.HashAlgorithmName,System.Security.Cryptography.DSASignatureFormat)", "codegen", "System.Security.Cryptography.DSA", "SignData", True, "benchmark" },
    { 248, "System.Security.Cryptography/System.Security.Cryptography.DSA::SignData:System.Byte[](System.Byte[],System.Security.Cryptography.HashAlgorithmName,System.Security.Cryptography.DSASignatureFormat)", "codegen", "System.Security.Cryptography.DSA", "SignData", True, "benchmark" },
    { 249, "System.Security.Cryptography/System.Security.Cryptography.DSA::SignData:System.Byte[](System.Byte[],System.Int32,System.Int32,System.Security.Cryptography.HashAlgorithmName)", "codegen", "System.Security.Cryptography.DSA", "SignData", True, "benchmark" },
    { 250, "System.Security.Cryptography/System.Security.Cryptography.DSA::SignData:System.Byte[](System.Byte[],System.Int32,System.Int32,System.Security.Cryptography.HashAlgorithmName)", "codegen", "System.Security.Cryptography.DSA", "SignData", True, "benchmark" },
    { 251, "System.Security.Cryptography/System.Security.Cryptography.DSA::SignData:System.Byte[](System.Byte[],System.Int32,System.Int32,System.Security.Cryptography.HashAlgorithmName)", "codegen", "System.Security.Cryptography.DSA", "SignData", True, "benchmark" },
    { 252, "System.Security.Cryptography/System.Security.Cryptography.DSA::SignData:System.Byte[](System.Byte[],System.Int32,System.Int32,System.Security.Cryptography.HashAlgorithmName)", "codegen", "System.Security.Cryptography.DSA", "SignData", True, "benchmark" },
    { 253, "System.Security.Cryptography/System.Security.Cryptography.DSA::SignData:System.Byte[](System.Byte[],System.Int32,System.Int32,System.Security.Cryptography.HashAlgorithmName)", "codegen", "System.Security.Cryptography.DSA", "SignData", True, "benchmark" },
    { 254, "System.Security.Cryptography/System.Security.Cryptography.DSA::SignData:System.Byte[](System.Byte[],System.Int32,System.Int32,System.Security.Cryptography.HashAlgorithmName,System.Security.Cryptography.DSASignatureFormat)", "codegen", "System.Security.Cryptography.DSA", "SignData", True, "benchmark" },
    { 255, "System.Security.Cryptography/System.Security.Cryptography.DSA::SignData:System.Byte[](System.Byte[],System.Int32,System.Int32,System.Security.Cryptography.HashAlgorithmName,System.Security.Cryptography.DSASignatureFormat)", "codegen", "System.Security.Cryptography.DSA", "SignData", True, "benchmark" },
    { 256, "System.Security.Cryptography/System.Security.Cryptography.DSA::SignData:System.Byte[](System.Byte[],System.Int32,System.Int32,System.Security.Cryptography.HashAlgorithmName,System.Security.Cryptography.DSASignatureFormat)", "codegen", "System.Security.Cryptography.DSA", "SignData", True, "benchmark" },
    { 257, "System.Security.Cryptography/System.Security.Cryptography.DSA::SignData:System.Byte[](System.Byte[],System.Int32,System.Int32,System.Security.Cryptography.HashAlgorithmName,System.Security.Cryptography.DSASignatureFormat)", "codegen", "System.Security.Cryptography.DSA", "SignData", True, "benchmark" },
    { 258, "System.Security.Cryptography/System.Security.Cryptography.DSA::SignData:System.Byte[](System.Byte[],System.Int32,System.Int32,System.Security.Cryptography.HashAlgorithmName,System.Security.Cryptography.DSASignatureFormat)", "codegen", "System.Security.Cryptography.DSA", "SignData", True, "benchmark" },
    { 259, "System.Security.Cryptography/System.Security.Cryptography.DSA::SignData:System.Byte[](System.IO.Stream,System.Security.Cryptography.HashAlgorithmName)", "codegen", "System.Security.Cryptography.DSA", "SignData", True, "benchmark" },
    { 260, "System.Security.Cryptography/System.Security.Cryptography.DSA::SignData:System.Byte[](System.IO.Stream,System.Security.Cryptography.HashAlgorithmName)", "codegen", "System.Security.Cryptography.DSA", "SignData", True, "benchmark" },
    { 261, "System.Security.Cryptography/System.Security.Cryptography.DSA::SignData:System.Byte[](System.IO.Stream,System.Security.Cryptography.HashAlgorithmName,System.Security.Cryptography.DSASignatureFormat)", "codegen", "System.Security.Cryptography.DSA", "SignData", True, "benchmark" },
    { 262, "System.Security.Cryptography/System.Security.Cryptography.DSA::SignData:System.Byte[](System.IO.Stream,System.Security.Cryptography.HashAlgorithmName,System.Security.Cryptography.DSASignatureFormat)", "codegen", "System.Security.Cryptography.DSA", "SignData", True, "benchmark" },
    { 263, "System.Security.Cryptography/System.Security.Cryptography.DSA::VerifyData:System.Boolean(System.Byte[],System.Byte[],System.Security.Cryptography.HashAlgorithmName)", "codegen", "System.Security.Cryptography.DSA", "VerifyData", True, "benchmark" },
    { 264, "System.Security.Cryptography/System.Security.Cryptography.DSA::VerifyData:System.Boolean(System.Byte[],System.Byte[],System.Security.Cryptography.HashAlgorithmName)", "codegen", "System.Security.Cryptography.DSA", "VerifyData", True, "benchmark" },
    { 265, "System.Security.Cryptography/System.Security.Cryptography.DSA::VerifyData:System.Boolean(System.Byte[],System.Byte[],System.Security.Cryptography.HashAlgorithmName)", "codegen", "System.Security.Cryptography.DSA", "VerifyData", True, "benchmark" },
    { 266, "System.Security.Cryptography/System.Security.Cryptography.DSA::VerifyData:System.Boolean(System.Byte[],System.Int32,System.Int32,System.Byte[],System.Security.Cryptography.HashAlgorithmName)", "codegen", "System.Security.Cryptography.DSA", "VerifyData", True, "benchmark" },
    { 267, "System.Security.Cryptography/System.Security.Cryptography.DSA::VerifyData:System.Boolean(System.Byte[],System.Int32,System.Int32,System.Byte[],System.Security.Cryptography.HashAlgorithmName)", "codegen", "System.Security.Cryptography.DSA", "VerifyData", True, "benchmark" },
    { 268, "System.Security.Cryptography/System.Security.Cryptography.DSA::VerifyData:System.Boolean(System.Byte[],System.Int32,System.Int32,System.Byte[],System.Security.Cryptography.HashAlgorithmName)", "codegen", "System.Security.Cryptography.DSA", "VerifyData", True, "benchmark" },
    { 269, "System.Security.Cryptography/System.Security.Cryptography.DSA::VerifyData:System.Boolean(System.Byte[],System.Int32,System.Int32,System.Byte[],System.Security.Cryptography.HashAlgorithmName)", "codegen", "System.Security.Cryptography.DSA", "VerifyData", True, "benchmark" },
    { 270, "System.Security.Cryptography/System.Security.Cryptography.DSA::VerifyData:System.Boolean(System.Byte[],System.Int32,System.Int32,System.Byte[],System.Security.Cryptography.HashAlgorithmName)", "codegen", "System.Security.Cryptography.DSA", "VerifyData", True, "benchmark" },
    { 271, "System.Security.Cryptography/System.Security.Cryptography.DSA::VerifyData:System.Boolean(System.Byte[],System.Int32,System.Int32,System.Byte[],System.Security.Cryptography.HashAlgorithmName,System.Security.Cryptography.DSASignatureFormat)", "codegen", "System.Security.Cryptography.DSA", "VerifyData", True, "benchmark" },
    { 272, "System.Security.Cryptography/System.Security.Cryptography.DSA::VerifyData:System.Boolean(System.Byte[],System.Int32,System.Int32,System.Byte[],System.Security.Cryptography.HashAlgorithmName,System.Security.Cryptography.DSASignatureFormat)", "codegen", "System.Security.Cryptography.DSA", "VerifyData", True, "benchmark" },
    { 273, "System.Security.Cryptography/System.Security.Cryptography.DSA::VerifyData:System.Boolean(System.Byte[],System.Int32,System.Int32,System.Byte[],System.Security.Cryptography.HashAlgorithmName,System.Security.Cryptography.DSASignatureFormat)", "codegen", "System.Security.Cryptography.DSA", "VerifyData", True, "benchmark" },
    { 274, "System.Security.Cryptography/System.Security.Cryptography.DSA::VerifyData:System.Boolean(System.Byte[],System.Int32,System.Int32,System.Byte[],System.Security.Cryptography.HashAlgorithmName,System.Security.Cryptography.DSASignatureFormat)", "codegen", "System.Security.Cryptography.DSA", "VerifyData", True, "benchmark" },
    { 275, "System.Security.Cryptography/System.Security.Cryptography.DSA::VerifyData:System.Boolean(System.Byte[],System.Int32,System.Int32,System.Byte[],System.Security.Cryptography.HashAlgorithmName,System.Security.Cryptography.DSASignatureFormat)", "codegen", "System.Security.Cryptography.DSA", "VerifyData", True, "benchmark" },
    { 276, "System.Security.Cryptography/System.Security.Cryptography.DSA::VerifyData:System.Boolean(System.IO.Stream,System.Byte[],System.Security.Cryptography.HashAlgorithmName)", "codegen", "System.Security.Cryptography.DSA", "VerifyData", True, "benchmark" },
    { 277, "System.Security.Cryptography/System.Security.Cryptography.DSA::VerifyData:System.Boolean(System.IO.Stream,System.Byte[],System.Security.Cryptography.HashAlgorithmName)", "codegen", "System.Security.Cryptography.DSA", "VerifyData", True, "benchmark" },
    { 278, "System.Security.Cryptography/System.Security.Cryptography.DSA::VerifyData:System.Boolean(System.IO.Stream,System.Byte[],System.Security.Cryptography.HashAlgorithmName)", "codegen", "System.Security.Cryptography.DSA", "VerifyData", True, "benchmark" },
    { 279, "System.Security.Cryptography/System.Security.Cryptography.DSA::CreateSignature:System.Byte[](System.Byte[],System.Security.Cryptography.DSASignatureFormat)", "codegen", "System.Security.Cryptography.DSA", "CreateSignature", True, "benchmark" },
    { 280, "System.Security.Cryptography/System.Security.Cryptography.DSA::CreateSignature:System.Byte[](System.Byte[],System.Security.Cryptography.DSASignatureFormat)", "codegen", "System.Security.Cryptography.DSA", "CreateSignature", True, "benchmark" },
    { 281, "System.Security.Cryptography/System.Security.Cryptography.DSA::CreateSignature:System.Byte[](System.Byte[],System.Security.Cryptography.DSASignatureFormat)", "codegen", "System.Security.Cryptography.DSA", "CreateSignature", True, "benchmark" },
    { 282, "System.Security.Cryptography/System.Security.Cryptography.DSA::VerifyData:System.Boolean(System.Byte[],System.Byte[],System.Security.Cryptography.HashAlgorithmName,System.Security.Cryptography.DSASignatureFormat)", "codegen", "System.Security.Cryptography.DSA", "VerifyData", True, "benchmark" },
    { 283, "System.Security.Cryptography/System.Security.Cryptography.DSA::VerifyData:System.Boolean(System.Byte[],System.Byte[],System.Security.Cryptography.HashAlgorithmName,System.Security.Cryptography.DSASignatureFormat)", "codegen", "System.Security.Cryptography.DSA", "VerifyData", True, "benchmark" },
    { 284, "System.Security.Cryptography/System.Security.Cryptography.DSA::VerifyData:System.Boolean(System.Byte[],System.Byte[],System.Security.Cryptography.HashAlgorithmName,System.Security.Cryptography.DSASignatureFormat)", "codegen", "System.Security.Cryptography.DSA", "VerifyData", True, "benchmark" },
    { 285, "System.Security.Cryptography/System.Security.Cryptography.DSA::VerifyData:System.Boolean(System.IO.Stream,System.Byte[],System.Security.Cryptography.HashAlgorithmName,System.Security.Cryptography.DSASignatureFormat)", "codegen", "System.Security.Cryptography.DSA", "VerifyData", True, "benchmark" },
    { 286, "System.Security.Cryptography/System.Security.Cryptography.DSA::VerifyData:System.Boolean(System.IO.Stream,System.Byte[],System.Security.Cryptography.HashAlgorithmName,System.Security.Cryptography.DSASignatureFormat)", "codegen", "System.Security.Cryptography.DSA", "VerifyData", True, "benchmark" },
    { 287, "System.Security.Cryptography/System.Security.Cryptography.DSA::VerifyData:System.Boolean(System.IO.Stream,System.Byte[],System.Security.Cryptography.HashAlgorithmName,System.Security.Cryptography.DSASignatureFormat)", "codegen", "System.Security.Cryptography.DSA", "VerifyData", True, "benchmark" },
    { 288, "System.Security.Cryptography/System.Security.Cryptography.DSA::VerifySignature:System.Boolean(System.Byte[],System.Byte[],System.Security.Cryptography.DSASignatureFormat)", "codegen", "System.Security.Cryptography.DSA", "VerifySignature", True, "benchmark" },
    { 289, "System.Security.Cryptography/System.Security.Cryptography.DSA::VerifySignature:System.Boolean(System.Byte[],System.Byte[],System.Security.Cryptography.DSASignatureFormat)", "codegen", "System.Security.Cryptography.DSA", "VerifySignature", True, "benchmark" },
    { 290, "System.Security.Cryptography/System.Security.Cryptography.DSA::VerifySignature:System.Boolean(System.Byte[],System.Byte[],System.Security.Cryptography.DSASignatureFormat)", "codegen", "System.Security.Cryptography.DSA", "VerifySignature", True, "benchmark" },
    { 291, "System.Security.Cryptography/System.Security.Cryptography.DSA::GetMaxSignatureSize:System.Int32(System.Security.Cryptography.DSASignatureFormat)", "codegen", "System.Security.Cryptography.DSA", "GetMaxSignatureSize", True, "benchmark" },
    { 292, "System.Security.Cryptography/System.Security.Cryptography.DSA::FromXmlString:System.Void(System.String)", "codegen", "System.Security.Cryptography.DSA", "FromXmlString", True, "benchmark" },
    { 293, "System.Security.Cryptography/System.Security.Cryptography.DSA::FromXmlString:System.Void(System.String)", "codegen", "System.Security.Cryptography.DSA", "FromXmlString", True, "benchmark" },
    { 294, "System.Security.Cryptography/System.Security.Cryptography.DSA::ToXmlString:System.String(System.Boolean)", "codegen", "System.Security.Cryptography.DSA", "ToXmlString", True, "benchmark" },
    { 295, "System.Security.Cryptography/System.Security.Cryptography.DSA::ToXmlString:System.String(System.Boolean)", "codegen", "System.Security.Cryptography.DSA", "ToXmlString", True, "benchmark" },
    { 296, "System.Security.Cryptography/System.Security.Cryptography.DSACng::ImportParameters:System.Void(System.Security.Cryptography.DSAParameters)", "codegen", "System.Security.Cryptography.DSACng", "ImportParameters", True, "benchmark" },
    { 297, "System.Security.Cryptography/System.Security.Cryptography.DSACng::ExportParameters:System.Security.Cryptography.DSAParameters(System.Boolean)", "codegen", "System.Security.Cryptography.DSACng", "ExportParameters", True, "benchmark" },
    { 298, "System.Security.Cryptography/System.Security.Cryptography.DSACng::ExportParameters:System.Security.Cryptography.DSAParameters(System.Boolean)", "codegen", "System.Security.Cryptography.DSACng", "ExportParameters", True, "benchmark" },
    { 299, "System.Security.Cryptography/System.Security.Cryptography.DSACng::CreateSignature:System.Byte[](System.Byte[])", "codegen", "System.Security.Cryptography.DSACng", "CreateSignature", True, "benchmark" },
    { 300, "System.Security.Cryptography/System.Security.Cryptography.DSACng::CreateSignature:System.Byte[](System.Byte[])", "codegen", "System.Security.Cryptography.DSACng", "CreateSignature", True, "benchmark" },
    { 301, "System.Security.Cryptography/System.Security.Cryptography.DSACng::CreateSignature:System.Byte[](System.Byte[])", "codegen", "System.Security.Cryptography.DSACng", "CreateSignature", True, "benchmark" },
    { 302, "System.Security.Cryptography/System.Security.Cryptography.DSACng::VerifySignature:System.Boolean(System.Byte[],System.Byte[])", "codegen", "System.Security.Cryptography.DSACng", "VerifySignature", True, "benchmark" },
    { 303, "System.Security.Cryptography/System.Security.Cryptography.DSACng::VerifySignature:System.Boolean(System.Byte[],System.Byte[])", "codegen", "System.Security.Cryptography.DSACng", "VerifySignature", True, "benchmark" },
    { 304, "System.Security.Cryptography/System.Security.Cryptography.DSACng::VerifySignature:System.Boolean(System.Byte[],System.Byte[])", "codegen", "System.Security.Cryptography.DSACng", "VerifySignature", True, "benchmark" },
    { 305, "System.Security.Cryptography/System.Security.Cryptography.DSACryptoServiceProvider::ExportCspBlob:System.Byte[](System.Boolean)", "codegen", "System.Security.Cryptography.DSACryptoServiceProvider", "ExportCspBlob", True, "benchmark" },
    { 306, "System.Security.Cryptography/System.Security.Cryptography.DSACryptoServiceProvider::ExportCspBlob:System.Byte[](System.Boolean)", "codegen", "System.Security.Cryptography.DSACryptoServiceProvider", "ExportCspBlob", True, "benchmark" },
    { 307, "System.Security.Cryptography/System.Security.Cryptography.DSACryptoServiceProvider::ExportParameters:System.Security.Cryptography.DSAParameters(System.Boolean)", "codegen", "System.Security.Cryptography.DSACryptoServiceProvider", "ExportParameters", True, "benchmark" },
    { 308, "System.Security.Cryptography/System.Security.Cryptography.DSACryptoServiceProvider::ExportParameters:System.Security.Cryptography.DSAParameters(System.Boolean)", "codegen", "System.Security.Cryptography.DSACryptoServiceProvider", "ExportParameters", True, "benchmark" },
    { 309, "System.Security.Cryptography/System.Security.Cryptography.DSACryptoServiceProvider::ImportCspBlob:System.Void(System.Byte[])", "codegen", "System.Security.Cryptography.DSACryptoServiceProvider", "ImportCspBlob", True, "benchmark" },
    { 310, "System.Security.Cryptography/System.Security.Cryptography.DSACryptoServiceProvider::ImportCspBlob:System.Void(System.Byte[])", "codegen", "System.Security.Cryptography.DSACryptoServiceProvider", "ImportCspBlob", True, "benchmark" },
    { 311, "System.Security.Cryptography/System.Security.Cryptography.DSACryptoServiceProvider::ImportCspBlob:System.Void(System.Byte[])", "codegen", "System.Security.Cryptography.DSACryptoServiceProvider", "ImportCspBlob", True, "benchmark" },
    { 312, "System.Security.Cryptography/System.Security.Cryptography.DSACryptoServiceProvider::ImportParameters:System.Void(System.Security.Cryptography.DSAParameters)", "codegen", "System.Security.Cryptography.DSACryptoServiceProvider", "ImportParameters", True, "benchmark" },
    { 313, "System.Security.Cryptography/System.Security.Cryptography.DSACryptoServiceProvider::SignData:System.Byte[](System.IO.Stream)", "codegen", "System.Security.Cryptography.DSACryptoServiceProvider", "SignData", True, "benchmark" },
    { 314, "System.Security.Cryptography/System.Security.Cryptography.DSACryptoServiceProvider::SignData:System.Byte[](System.IO.Stream)", "codegen", "System.Security.Cryptography.DSACryptoServiceProvider", "SignData", True, "benchmark" },
    { 315, "System.Security.Cryptography/System.Security.Cryptography.DSACryptoServiceProvider::SignData:System.Byte[](System.Byte[])", "codegen", "System.Security.Cryptography.DSACryptoServiceProvider", "SignData", True, "benchmark" },
    { 316, "System.Security.Cryptography/System.Security.Cryptography.DSACryptoServiceProvider::SignData:System.Byte[](System.Byte[])", "codegen", "System.Security.Cryptography.DSACryptoServiceProvider", "SignData", True, "benchmark" },
    { 317, "System.Security.Cryptography/System.Security.Cryptography.DSACryptoServiceProvider::SignData:System.Byte[](System.Byte[])", "codegen", "System.Security.Cryptography.DSACryptoServiceProvider", "SignData", True, "benchmark" },
    { 318, "System.Security.Cryptography/System.Security.Cryptography.DSACryptoServiceProvider::SignData:System.Byte[](System.Byte[],System.Int32,System.Int32)", "codegen", "System.Security.Cryptography.DSACryptoServiceProvider", "SignData", True, "benchmark" },
    { 319, "System.Security.Cryptography/System.Security.Cryptography.DSACryptoServiceProvider::SignData:System.Byte[](System.Byte[],System.Int32,System.Int32)", "codegen", "System.Security.Cryptography.DSACryptoServiceProvider", "SignData", True, "benchmark" },
    { 320, "System.Security.Cryptography/System.Security.Cryptography.DSACryptoServiceProvider::SignData:System.Byte[](System.Byte[],System.Int32,System.Int32)", "codegen", "System.Security.Cryptography.DSACryptoServiceProvider", "SignData", True, "benchmark" },
    { 321, "System.Security.Cryptography/System.Security.Cryptography.DSACryptoServiceProvider::SignData:System.Byte[](System.Byte[],System.Int32,System.Int32)", "codegen", "System.Security.Cryptography.DSACryptoServiceProvider", "SignData", True, "benchmark" },
    { 322, "System.Security.Cryptography/System.Security.Cryptography.DSACryptoServiceProvider::SignData:System.Byte[](System.Byte[],System.Int32,System.Int32)", "codegen", "System.Security.Cryptography.DSACryptoServiceProvider", "SignData", True, "benchmark" },
    { 323, "System.Security.Cryptography/System.Security.Cryptography.DSACryptoServiceProvider::VerifyData:System.Boolean(System.Byte[],System.Byte[])", "codegen", "System.Security.Cryptography.DSACryptoServiceProvider", "VerifyData", True, "benchmark" },
    { 324, "System.Security.Cryptography/System.Security.Cryptography.DSACryptoServiceProvider::VerifyData:System.Boolean(System.Byte[],System.Byte[])", "codegen", "System.Security.Cryptography.DSACryptoServiceProvider", "VerifyData", True, "benchmark" },
    { 325, "System.Security.Cryptography/System.Security.Cryptography.DSACryptoServiceProvider::VerifyData:System.Boolean(System.Byte[],System.Byte[])", "codegen", "System.Security.Cryptography.DSACryptoServiceProvider", "VerifyData", True, "benchmark" },
    { 326, "System.Security.Cryptography/System.Security.Cryptography.DSACryptoServiceProvider::CreateSignature:System.Byte[](System.Byte[])", "codegen", "System.Security.Cryptography.DSACryptoServiceProvider", "CreateSignature", True, "benchmark" },
    { 327, "System.Security.Cryptography/System.Security.Cryptography.DSACryptoServiceProvider::CreateSignature:System.Byte[](System.Byte[])", "codegen", "System.Security.Cryptography.DSACryptoServiceProvider", "CreateSignature", True, "benchmark" },
    { 328, "System.Security.Cryptography/System.Security.Cryptography.DSACryptoServiceProvider::CreateSignature:System.Byte[](System.Byte[])", "codegen", "System.Security.Cryptography.DSACryptoServiceProvider", "CreateSignature", True, "benchmark" },
    { 329, "System.Security.Cryptography/System.Security.Cryptography.DSACryptoServiceProvider::VerifySignature:System.Boolean(System.Byte[],System.Byte[])", "codegen", "System.Security.Cryptography.DSACryptoServiceProvider", "VerifySignature", True, "benchmark" },
    { 330, "System.Security.Cryptography/System.Security.Cryptography.DSACryptoServiceProvider::VerifySignature:System.Boolean(System.Byte[],System.Byte[])", "codegen", "System.Security.Cryptography.DSACryptoServiceProvider", "VerifySignature", True, "benchmark" },
    { 331, "System.Security.Cryptography/System.Security.Cryptography.DSACryptoServiceProvider::VerifySignature:System.Boolean(System.Byte[],System.Byte[])", "codegen", "System.Security.Cryptography.DSACryptoServiceProvider", "VerifySignature", True, "benchmark" },
    { 332, "System.Security.Cryptography/System.Security.Cryptography.DSACryptoServiceProvider::SignHash:System.Byte[](System.Byte[],System.String)", "codegen", "System.Security.Cryptography.DSACryptoServiceProvider", "SignHash", True, "benchmark" },
    { 333, "System.Security.Cryptography/System.Security.Cryptography.DSACryptoServiceProvider::SignHash:System.Byte[](System.Byte[],System.String)", "codegen", "System.Security.Cryptography.DSACryptoServiceProvider", "SignHash", True, "benchmark" },
    { 334, "System.Security.Cryptography/System.Security.Cryptography.DSACryptoServiceProvider::SignHash:System.Byte[](System.Byte[],System.String)", "codegen", "System.Security.Cryptography.DSACryptoServiceProvider", "SignHash", True, "benchmark" },
    { 335, "System.Security.Cryptography/System.Security.Cryptography.DSACryptoServiceProvider::SignHash:System.Byte[](System.Byte[],System.String)", "codegen", "System.Security.Cryptography.DSACryptoServiceProvider", "SignHash", True, "benchmark" },
    { 336, "System.Security.Cryptography/System.Security.Cryptography.DSACryptoServiceProvider::VerifyHash:System.Boolean(System.Byte[],System.String,System.Byte[])", "codegen", "System.Security.Cryptography.DSACryptoServiceProvider", "VerifyHash", True, "benchmark" },
    { 337, "System.Security.Cryptography/System.Security.Cryptography.DSACryptoServiceProvider::VerifyHash:System.Boolean(System.Byte[],System.String,System.Byte[])", "codegen", "System.Security.Cryptography.DSACryptoServiceProvider", "VerifyHash", True, "benchmark" },
    { 338, "System.Security.Cryptography/System.Security.Cryptography.DSACryptoServiceProvider::VerifyHash:System.Boolean(System.Byte[],System.String,System.Byte[])", "codegen", "System.Security.Cryptography.DSACryptoServiceProvider", "VerifyHash", True, "benchmark" },
    { 339, "System.Security.Cryptography/System.Security.Cryptography.DSACryptoServiceProvider::VerifyHash:System.Boolean(System.Byte[],System.String,System.Byte[])", "codegen", "System.Security.Cryptography.DSACryptoServiceProvider", "VerifyHash", True, "benchmark" },
    { 340, "System.Security.Cryptography/System.Security.Cryptography.DSAOpenSsl::DuplicateKeyHandle:System.Security.Cryptography.SafeEvpPKeyHandle()", "codegen", "System.Security.Cryptography.DSAOpenSsl", "DuplicateKeyHandle", True, "benchmark" },
    { 341, "System.Security.Cryptography/System.Security.Cryptography.DSAOpenSsl::CreateSignature:System.Byte[](System.Byte[])", "codegen", "System.Security.Cryptography.DSAOpenSsl", "CreateSignature", True, "benchmark" },
    { 342, "System.Security.Cryptography/System.Security.Cryptography.DSAOpenSsl::CreateSignature:System.Byte[](System.Byte[])", "codegen", "System.Security.Cryptography.DSAOpenSsl", "CreateSignature", True, "benchmark" },
    { 343, "System.Security.Cryptography/System.Security.Cryptography.DSAOpenSsl::CreateSignature:System.Byte[](System.Byte[])", "codegen", "System.Security.Cryptography.DSAOpenSsl", "CreateSignature", True, "benchmark" },
    { 344, "System.Security.Cryptography/System.Security.Cryptography.DSAOpenSsl::ExportParameters:System.Security.Cryptography.DSAParameters(System.Boolean)", "codegen", "System.Security.Cryptography.DSAOpenSsl", "ExportParameters", True, "benchmark" },
    { 345, "System.Security.Cryptography/System.Security.Cryptography.DSAOpenSsl::ExportParameters:System.Security.Cryptography.DSAParameters(System.Boolean)", "codegen", "System.Security.Cryptography.DSAOpenSsl", "ExportParameters", True, "benchmark" },
    { 346, "System.Security.Cryptography/System.Security.Cryptography.DSAOpenSsl::ImportParameters:System.Void(System.Security.Cryptography.DSAParameters)", "codegen", "System.Security.Cryptography.DSAOpenSsl", "ImportParameters", True, "benchmark" },
    { 347, "System.Security.Cryptography/System.Security.Cryptography.DSAOpenSsl::VerifySignature:System.Boolean(System.Byte[],System.Byte[])", "codegen", "System.Security.Cryptography.DSAOpenSsl", "VerifySignature", True, "benchmark" },
    { 348, "System.Security.Cryptography/System.Security.Cryptography.DSAOpenSsl::VerifySignature:System.Boolean(System.Byte[],System.Byte[])", "codegen", "System.Security.Cryptography.DSAOpenSsl", "VerifySignature", True, "benchmark" },
    { 349, "System.Security.Cryptography/System.Security.Cryptography.DSAOpenSsl::VerifySignature:System.Boolean(System.Byte[],System.Byte[])", "codegen", "System.Security.Cryptography.DSAOpenSsl", "VerifySignature", True, "benchmark" },
    { 350, "System.Security.Cryptography/System.Security.Cryptography.DSASignatureDeformatter::SetKey:System.Void(System.Security.Cryptography.AsymmetricAlgorithm)", "codegen", "System.Security.Cryptography.DSASignatureDeformatter", "SetKey", True, "benchmark" },
    { 351, "System.Security.Cryptography/System.Security.Cryptography.DSASignatureDeformatter::SetHashAlgorithm:System.Void(System.String)", "codegen", "System.Security.Cryptography.DSASignatureDeformatter", "SetHashAlgorithm", True, "benchmark" },
    { 352, "System.Security.Cryptography/System.Security.Cryptography.DSASignatureDeformatter::SetHashAlgorithm:System.Void(System.String)", "codegen", "System.Security.Cryptography.DSASignatureDeformatter", "SetHashAlgorithm", True, "benchmark" },
    { 353, "System.Security.Cryptography/System.Security.Cryptography.DSASignatureDeformatter::VerifySignature:System.Boolean(System.Byte[],System.Byte[])", "codegen", "System.Security.Cryptography.DSASignatureDeformatter", "VerifySignature", True, "benchmark" },
    { 354, "System.Security.Cryptography/System.Security.Cryptography.DSASignatureDeformatter::VerifySignature:System.Boolean(System.Byte[],System.Byte[])", "codegen", "System.Security.Cryptography.DSASignatureDeformatter", "VerifySignature", True, "benchmark" },
    { 355, "System.Security.Cryptography/System.Security.Cryptography.DSASignatureDeformatter::VerifySignature:System.Boolean(System.Byte[],System.Byte[])", "codegen", "System.Security.Cryptography.DSASignatureDeformatter", "VerifySignature", True, "benchmark" },
    { 356, "System.Security.Cryptography/System.Security.Cryptography.DSASignatureFormatter::SetKey:System.Void(System.Security.Cryptography.AsymmetricAlgorithm)", "codegen", "System.Security.Cryptography.DSASignatureFormatter", "SetKey", True, "benchmark" },
    { 357, "System.Security.Cryptography/System.Security.Cryptography.DSASignatureFormatter::SetHashAlgorithm:System.Void(System.String)", "codegen", "System.Security.Cryptography.DSASignatureFormatter", "SetHashAlgorithm", True, "benchmark" },
    { 358, "System.Security.Cryptography/System.Security.Cryptography.DSASignatureFormatter::SetHashAlgorithm:System.Void(System.String)", "codegen", "System.Security.Cryptography.DSASignatureFormatter", "SetHashAlgorithm", True, "benchmark" },
    { 359, "System.Security.Cryptography/System.Security.Cryptography.DSASignatureFormatter::CreateSignature:System.Byte[](System.Byte[])", "codegen", "System.Security.Cryptography.DSASignatureFormatter", "CreateSignature", True, "benchmark" },
    { 360, "System.Security.Cryptography/System.Security.Cryptography.DSASignatureFormatter::CreateSignature:System.Byte[](System.Byte[])", "codegen", "System.Security.Cryptography.DSASignatureFormatter", "CreateSignature", True, "benchmark" },
    { 361, "System.Security.Cryptography/System.Security.Cryptography.DSASignatureFormatter::CreateSignature:System.Byte[](System.Byte[])", "codegen", "System.Security.Cryptography.DSASignatureFormatter", "CreateSignature", True, "benchmark" },
    { 362, "System.Security.Cryptography/System.Security.Cryptography.ECAlgorithm::ExportParameters:System.Security.Cryptography.ECParameters(System.Boolean)", "codegen", "System.Security.Cryptography.ECAlgorithm", "ExportParameters", True, "benchmark" },
    { 363, "System.Security.Cryptography/System.Security.Cryptography.ECAlgorithm::ExportParameters:System.Security.Cryptography.ECParameters(System.Boolean)", "codegen", "System.Security.Cryptography.ECAlgorithm", "ExportParameters", True, "benchmark" },
    { 364, "System.Security.Cryptography/System.Security.Cryptography.ECAlgorithm::ExportExplicitParameters:System.Security.Cryptography.ECParameters(System.Boolean)", "codegen", "System.Security.Cryptography.ECAlgorithm", "ExportExplicitParameters", True, "benchmark" },
    { 365, "System.Security.Cryptography/System.Security.Cryptography.ECAlgorithm::ExportExplicitParameters:System.Security.Cryptography.ECParameters(System.Boolean)", "codegen", "System.Security.Cryptography.ECAlgorithm", "ExportExplicitParameters", True, "benchmark" },
    { 366, "System.Security.Cryptography/System.Security.Cryptography.ECAlgorithm::ImportParameters:System.Void(System.Security.Cryptography.ECParameters)", "codegen", "System.Security.Cryptography.ECAlgorithm", "ImportParameters", True, "benchmark" },
    { 367, "System.Security.Cryptography/System.Security.Cryptography.ECAlgorithm::GenerateKey:System.Void(System.Security.Cryptography.ECCurve)", "codegen", "System.Security.Cryptography.ECAlgorithm", "GenerateKey", True, "benchmark" },
    { 368, "System.Security.Cryptography/System.Security.Cryptography.ECAlgorithm::ExportECPrivateKey:System.Byte[]()", "codegen", "System.Security.Cryptography.ECAlgorithm", "ExportECPrivateKey", True, "benchmark" },
    { 369, "System.Security.Cryptography/System.Security.Cryptography.ECAlgorithm::ExportECPrivateKeyPem:System.String()", "codegen", "System.Security.Cryptography.ECAlgorithm", "ExportECPrivateKeyPem", True, "benchmark" },
    { 370, "System.Security.Cryptography/System.Security.Cryptography.ECCurve::CreateFromOid:System.Security.Cryptography.ECCurve(System.Security.Cryptography.Oid)", "codegen", "System.Security.Cryptography.ECCurve", "CreateFromOid", True, "benchmark" },
    { 371, "System.Security.Cryptography/System.Security.Cryptography.ECCurve::CreateFromFriendlyName:System.Security.Cryptography.ECCurve(System.String)", "codegen", "System.Security.Cryptography.ECCurve", "CreateFromFriendlyName", True, "benchmark" },
    { 372, "System.Security.Cryptography/System.Security.Cryptography.ECCurve::CreateFromFriendlyName:System.Security.Cryptography.ECCurve(System.String)", "codegen", "System.Security.Cryptography.ECCurve", "CreateFromFriendlyName", True, "benchmark" },
    { 373, "System.Security.Cryptography/System.Security.Cryptography.ECCurve::CreateFromValue:System.Security.Cryptography.ECCurve(System.String)", "codegen", "System.Security.Cryptography.ECCurve", "CreateFromValue", True, "benchmark" },
    { 374, "System.Security.Cryptography/System.Security.Cryptography.ECCurve::CreateFromValue:System.Security.Cryptography.ECCurve(System.String)", "codegen", "System.Security.Cryptography.ECCurve", "CreateFromValue", True, "benchmark" },
    { 375, "System.Security.Cryptography/System.Security.Cryptography.ECCurve::Validate:System.Void()", "codegen", "System.Security.Cryptography.ECCurve", "Validate", True, "benchmark" },
    { 376, "System.Security.Cryptography/System.Security.Cryptography.ECDiffieHellman::DeriveKeyMaterial:System.Byte[](System.Security.Cryptography.ECDiffieHellmanPublicKey)", "codegen", "System.Security.Cryptography.ECDiffieHellman", "DeriveKeyMaterial", True, "benchmark" },
    { 377, "System.Security.Cryptography/System.Security.Cryptography.ECDiffieHellman::DeriveKeyFromHash:System.Byte[](System.Security.Cryptography.ECDiffieHellmanPublicKey,System.Security.Cryptography.HashAlgorithmName)", "codegen", "System.Security.Cryptography.ECDiffieHellman", "DeriveKeyFromHash", True, "benchmark" },
    { 378, "System.Security.Cryptography/System.Security.Cryptography.ECDiffieHellman::DeriveKeyFromHash:System.Byte[](System.Security.Cryptography.ECDiffieHellmanPublicKey,System.Security.Cryptography.HashAlgorithmName,System.Byte[],System.Byte[])", "codegen", "System.Security.Cryptography.ECDiffieHellman", "DeriveKeyFromHash", True, "benchmark" },
    { 379, "System.Security.Cryptography/System.Security.Cryptography.ECDiffieHellman::DeriveKeyFromHash:System.Byte[](System.Security.Cryptography.ECDiffieHellmanPublicKey,System.Security.Cryptography.HashAlgorithmName,System.Byte[],System.Byte[])", "codegen", "System.Security.Cryptography.ECDiffieHellman", "DeriveKeyFromHash", True, "benchmark" },
    { 380, "System.Security.Cryptography/System.Security.Cryptography.ECDiffieHellman::DeriveKeyFromHash:System.Byte[](System.Security.Cryptography.ECDiffieHellmanPublicKey,System.Security.Cryptography.HashAlgorithmName,System.Byte[],System.Byte[])", "codegen", "System.Security.Cryptography.ECDiffieHellman", "DeriveKeyFromHash", True, "benchmark" },
    { 381, "System.Security.Cryptography/System.Security.Cryptography.ECDiffieHellman::DeriveKeyFromHmac:System.Byte[](System.Security.Cryptography.ECDiffieHellmanPublicKey,System.Security.Cryptography.HashAlgorithmName,System.Byte[])", "codegen", "System.Security.Cryptography.ECDiffieHellman", "DeriveKeyFromHmac", True, "benchmark" },
    { 382, "System.Security.Cryptography/System.Security.Cryptography.ECDiffieHellman::DeriveKeyFromHmac:System.Byte[](System.Security.Cryptography.ECDiffieHellmanPublicKey,System.Security.Cryptography.HashAlgorithmName,System.Byte[])", "codegen", "System.Security.Cryptography.ECDiffieHellman", "DeriveKeyFromHmac", True, "benchmark" },
    { 383, "System.Security.Cryptography/System.Security.Cryptography.ECDiffieHellman::DeriveKeyFromHmac:System.Byte[](System.Security.Cryptography.ECDiffieHellmanPublicKey,System.Security.Cryptography.HashAlgorithmName,System.Byte[])", "codegen", "System.Security.Cryptography.ECDiffieHellman", "DeriveKeyFromHmac", True, "benchmark" },
    { 384, "System.Security.Cryptography/System.Security.Cryptography.ECDiffieHellman::DeriveKeyFromHmac:System.Byte[](System.Security.Cryptography.ECDiffieHellmanPublicKey,System.Security.Cryptography.HashAlgorithmName,System.Byte[],System.Byte[],System.Byte[])", "codegen", "System.Security.Cryptography.ECDiffieHellman", "DeriveKeyFromHmac", True, "benchmark" },
    { 385, "System.Security.Cryptography/System.Security.Cryptography.ECDiffieHellman::DeriveKeyFromHmac:System.Byte[](System.Security.Cryptography.ECDiffieHellmanPublicKey,System.Security.Cryptography.HashAlgorithmName,System.Byte[],System.Byte[],System.Byte[])", "codegen", "System.Security.Cryptography.ECDiffieHellman", "DeriveKeyFromHmac", True, "benchmark" },
    { 386, "System.Security.Cryptography/System.Security.Cryptography.ECDiffieHellman::DeriveKeyFromHmac:System.Byte[](System.Security.Cryptography.ECDiffieHellmanPublicKey,System.Security.Cryptography.HashAlgorithmName,System.Byte[],System.Byte[],System.Byte[])", "codegen", "System.Security.Cryptography.ECDiffieHellman", "DeriveKeyFromHmac", True, "benchmark" },
    { 387, "System.Security.Cryptography/System.Security.Cryptography.ECDiffieHellman::DeriveKeyTls:System.Byte[](System.Security.Cryptography.ECDiffieHellmanPublicKey,System.Byte[],System.Byte[])", "codegen", "System.Security.Cryptography.ECDiffieHellman", "DeriveKeyTls", True, "benchmark" },
    { 388, "System.Security.Cryptography/System.Security.Cryptography.ECDiffieHellman::DeriveKeyTls:System.Byte[](System.Security.Cryptography.ECDiffieHellmanPublicKey,System.Byte[],System.Byte[])", "codegen", "System.Security.Cryptography.ECDiffieHellman", "DeriveKeyTls", True, "benchmark" },
    { 389, "System.Security.Cryptography/System.Security.Cryptography.ECDiffieHellman::DeriveKeyTls:System.Byte[](System.Security.Cryptography.ECDiffieHellmanPublicKey,System.Byte[],System.Byte[])", "codegen", "System.Security.Cryptography.ECDiffieHellman", "DeriveKeyTls", True, "benchmark" },
    { 390, "System.Security.Cryptography/System.Security.Cryptography.ECDiffieHellman::DeriveRawSecretAgreement:System.Byte[](System.Security.Cryptography.ECDiffieHellmanPublicKey)", "codegen", "System.Security.Cryptography.ECDiffieHellman", "DeriveRawSecretAgreement", True, "benchmark" },
    { 391, "System.Security.Cryptography/System.Security.Cryptography.ECDiffieHellman::FromXmlString:System.Void(System.String)", "codegen", "System.Security.Cryptography.ECDiffieHellman", "FromXmlString", True, "benchmark" },
    { 392, "System.Security.Cryptography/System.Security.Cryptography.ECDiffieHellman::FromXmlString:System.Void(System.String)", "codegen", "System.Security.Cryptography.ECDiffieHellman", "FromXmlString", True, "benchmark" },
    { 393, "System.Security.Cryptography/System.Security.Cryptography.ECDiffieHellman::ToXmlString:System.String(System.Boolean)", "codegen", "System.Security.Cryptography.ECDiffieHellman", "ToXmlString", True, "benchmark" },
    { 394, "System.Security.Cryptography/System.Security.Cryptography.ECDiffieHellman::ToXmlString:System.String(System.Boolean)", "codegen", "System.Security.Cryptography.ECDiffieHellman", "ToXmlString", True, "benchmark" },
    { 395, "System.Security.Cryptography/System.Security.Cryptography.ECDiffieHellmanCng::DeriveKeyFromHash:System.Byte[](System.Security.Cryptography.ECDiffieHellmanPublicKey,System.Security.Cryptography.HashAlgorithmName,System.Byte[],System.Byte[])", "codegen", "System.Security.Cryptography.ECDiffieHellmanCng", "DeriveKeyFromHash", True, "benchmark" },
    { 396, "System.Security.Cryptography/System.Security.Cryptography.ECDiffieHellmanCng::DeriveKeyFromHash:System.Byte[](System.Security.Cryptography.ECDiffieHellmanPublicKey,System.Security.Cryptography.HashAlgorithmName,System.Byte[],System.Byte[])", "codegen", "System.Security.Cryptography.ECDiffieHellmanCng", "DeriveKeyFromHash", True, "benchmark" },
    { 397, "System.Security.Cryptography/System.Security.Cryptography.ECDiffieHellmanCng::DeriveKeyFromHash:System.Byte[](System.Security.Cryptography.ECDiffieHellmanPublicKey,System.Security.Cryptography.HashAlgorithmName,System.Byte[],System.Byte[])", "codegen", "System.Security.Cryptography.ECDiffieHellmanCng", "DeriveKeyFromHash", True, "benchmark" },
    { 398, "System.Security.Cryptography/System.Security.Cryptography.ECDiffieHellmanCng::DeriveKeyFromHmac:System.Byte[](System.Security.Cryptography.ECDiffieHellmanPublicKey,System.Security.Cryptography.HashAlgorithmName,System.Byte[],System.Byte[],System.Byte[])", "codegen", "System.Security.Cryptography.ECDiffieHellmanCng", "DeriveKeyFromHmac", True, "benchmark" },
    { 399, "System.Security.Cryptography/System.Security.Cryptography.ECDiffieHellmanCng::DeriveKeyFromHmac:System.Byte[](System.Security.Cryptography.ECDiffieHellmanPublicKey,System.Security.Cryptography.HashAlgorithmName,System.Byte[],System.Byte[],System.Byte[])", "codegen", "System.Security.Cryptography.ECDiffieHellmanCng", "DeriveKeyFromHmac", True, "benchmark" },
    { 400, "System.Security.Cryptography/System.Security.Cryptography.ECDiffieHellmanCng::DeriveKeyFromHmac:System.Byte[](System.Security.Cryptography.ECDiffieHellmanPublicKey,System.Security.Cryptography.HashAlgorithmName,System.Byte[],System.Byte[],System.Byte[])", "codegen", "System.Security.Cryptography.ECDiffieHellmanCng", "DeriveKeyFromHmac", True, "benchmark" },
    { 401, "System.Security.Cryptography/System.Security.Cryptography.ECDiffieHellmanCng::DeriveKeyTls:System.Byte[](System.Security.Cryptography.ECDiffieHellmanPublicKey,System.Byte[],System.Byte[])", "codegen", "System.Security.Cryptography.ECDiffieHellmanCng", "DeriveKeyTls", True, "benchmark" },
    { 402, "System.Security.Cryptography/System.Security.Cryptography.ECDiffieHellmanCng::DeriveKeyTls:System.Byte[](System.Security.Cryptography.ECDiffieHellmanPublicKey,System.Byte[],System.Byte[])", "codegen", "System.Security.Cryptography.ECDiffieHellmanCng", "DeriveKeyTls", True, "benchmark" },
    { 403, "System.Security.Cryptography/System.Security.Cryptography.ECDiffieHellmanCng::DeriveKeyTls:System.Byte[](System.Security.Cryptography.ECDiffieHellmanPublicKey,System.Byte[],System.Byte[])", "codegen", "System.Security.Cryptography.ECDiffieHellmanCng", "DeriveKeyTls", True, "benchmark" },
    { 404, "System.Security.Cryptography/System.Security.Cryptography.ECDiffieHellmanCng::DeriveRawSecretAgreement:System.Byte[](System.Security.Cryptography.ECDiffieHellmanPublicKey)", "codegen", "System.Security.Cryptography.ECDiffieHellmanCng", "DeriveRawSecretAgreement", True, "benchmark" },
    { 405, "System.Security.Cryptography/System.Security.Cryptography.ECDiffieHellmanCng::ImportParameters:System.Void(System.Security.Cryptography.ECParameters)", "codegen", "System.Security.Cryptography.ECDiffieHellmanCng", "ImportParameters", True, "benchmark" },
    { 406, "System.Security.Cryptography/System.Security.Cryptography.ECDiffieHellmanCng::ExportExplicitParameters:System.Security.Cryptography.ECParameters(System.Boolean)", "codegen", "System.Security.Cryptography.ECDiffieHellmanCng", "ExportExplicitParameters", True, "benchmark" },
    { 407, "System.Security.Cryptography/System.Security.Cryptography.ECDiffieHellmanCng::ExportExplicitParameters:System.Security.Cryptography.ECParameters(System.Boolean)", "codegen", "System.Security.Cryptography.ECDiffieHellmanCng", "ExportExplicitParameters", True, "benchmark" },
    { 408, "System.Security.Cryptography/System.Security.Cryptography.ECDiffieHellmanCng::ExportParameters:System.Security.Cryptography.ECParameters(System.Boolean)", "codegen", "System.Security.Cryptography.ECDiffieHellmanCng", "ExportParameters", True, "benchmark" },
    { 409, "System.Security.Cryptography/System.Security.Cryptography.ECDiffieHellmanCng::ExportParameters:System.Security.Cryptography.ECParameters(System.Boolean)", "codegen", "System.Security.Cryptography.ECDiffieHellmanCng", "ExportParameters", True, "benchmark" },
    { 410, "System.Security.Cryptography/System.Security.Cryptography.ECDiffieHellmanCng::DeriveKeyMaterial:System.Byte[](System.Security.Cryptography.ECDiffieHellmanPublicKey)", "codegen", "System.Security.Cryptography.ECDiffieHellmanCng", "DeriveKeyMaterial", True, "benchmark" },
    { 411, "System.Security.Cryptography/System.Security.Cryptography.ECDiffieHellmanCng::DeriveKeyMaterial:System.Byte[](System.Security.Cryptography.CngKey)", "codegen", "System.Security.Cryptography.ECDiffieHellmanCng", "DeriveKeyMaterial", True, "benchmark" },
    { 412, "System.Security.Cryptography/System.Security.Cryptography.ECDiffieHellmanCng::DeriveSecretAgreementHandle:Microsoft.Win32.SafeHandles.SafeNCryptSecretHandle(System.Security.Cryptography.ECDiffieHellmanPublicKey)", "codegen", "System.Security.Cryptography.ECDiffieHellmanCng", "DeriveSecretAgreementHandle", True, "benchmark" },
    { 413, "System.Security.Cryptography/System.Security.Cryptography.ECDiffieHellmanCng::DeriveSecretAgreementHandle:Microsoft.Win32.SafeHandles.SafeNCryptSecretHandle(System.Security.Cryptography.CngKey)", "codegen", "System.Security.Cryptography.ECDiffieHellmanCng", "DeriveSecretAgreementHandle", True, "benchmark" },
    { 414, "System.Security.Cryptography/System.Security.Cryptography.ECDiffieHellmanCng::GenerateKey:System.Void(System.Security.Cryptography.ECCurve)", "codegen", "System.Security.Cryptography.ECDiffieHellmanCng", "GenerateKey", True, "benchmark" },
    { 415, "System.Security.Cryptography/System.Security.Cryptography.ECDiffieHellmanCng::FromXmlString:System.Void(System.String,System.Security.Cryptography.ECKeyXmlFormat)", "codegen", "System.Security.Cryptography.ECDiffieHellmanCng", "FromXmlString", True, "benchmark" },
    { 416, "System.Security.Cryptography/System.Security.Cryptography.ECDiffieHellmanCng::FromXmlString:System.Void(System.String,System.Security.Cryptography.ECKeyXmlFormat)", "codegen", "System.Security.Cryptography.ECDiffieHellmanCng", "FromXmlString", True, "benchmark" },
    { 417, "System.Security.Cryptography/System.Security.Cryptography.ECDiffieHellmanCng::ToXmlString:System.String(System.Security.Cryptography.ECKeyXmlFormat)", "codegen", "System.Security.Cryptography.ECDiffieHellmanCng", "ToXmlString", True, "benchmark" },
    { 418, "System.Security.Cryptography/System.Security.Cryptography.ECDiffieHellmanCngPublicKey::ToXmlString:System.String()", "codegen", "System.Security.Cryptography.ECDiffieHellmanCngPublicKey", "ToXmlString", True, "benchmark" },
    { 419, "System.Security.Cryptography/System.Security.Cryptography.ECDiffieHellmanCngPublicKey::FromXmlString:System.Security.Cryptography.ECDiffieHellmanCngPublicKey(System.String)", "codegen", "System.Security.Cryptography.ECDiffieHellmanCngPublicKey", "FromXmlString", True, "benchmark" },
    { 420, "System.Security.Cryptography/System.Security.Cryptography.ECDiffieHellmanCngPublicKey::FromXmlString:System.Security.Cryptography.ECDiffieHellmanCngPublicKey(System.String)", "codegen", "System.Security.Cryptography.ECDiffieHellmanCngPublicKey", "FromXmlString", True, "benchmark" },
    { 421, "System.Security.Cryptography/System.Security.Cryptography.ECDiffieHellmanCngPublicKey::FromByteArray:System.Security.Cryptography.ECDiffieHellmanPublicKey(System.Byte[],System.Security.Cryptography.CngKeyBlobFormat)", "codegen", "System.Security.Cryptography.ECDiffieHellmanCngPublicKey", "FromByteArray", True, "benchmark" },
    { 422, "System.Security.Cryptography/System.Security.Cryptography.ECDiffieHellmanCngPublicKey::FromByteArray:System.Security.Cryptography.ECDiffieHellmanPublicKey(System.Byte[],System.Security.Cryptography.CngKeyBlobFormat)", "codegen", "System.Security.Cryptography.ECDiffieHellmanCngPublicKey", "FromByteArray", True, "benchmark" },
    { 423, "System.Security.Cryptography/System.Security.Cryptography.ECDiffieHellmanCngPublicKey::FromByteArray:System.Security.Cryptography.ECDiffieHellmanPublicKey(System.Byte[],System.Security.Cryptography.CngKeyBlobFormat)", "codegen", "System.Security.Cryptography.ECDiffieHellmanCngPublicKey", "FromByteArray", True, "benchmark" },
    { 424, "System.Security.Cryptography/System.Security.Cryptography.ECDiffieHellmanCngPublicKey::Import:System.Security.Cryptography.CngKey()", "codegen", "System.Security.Cryptography.ECDiffieHellmanCngPublicKey", "Import", True, "benchmark" },
    { 425, "System.Security.Cryptography/System.Security.Cryptography.ECDiffieHellmanCngPublicKey::ExportExplicitParameters:System.Security.Cryptography.ECParameters()", "codegen", "System.Security.Cryptography.ECDiffieHellmanCngPublicKey", "ExportExplicitParameters", True, "benchmark" },
    { 426, "System.Security.Cryptography/System.Security.Cryptography.ECDiffieHellmanCngPublicKey::ExportParameters:System.Security.Cryptography.ECParameters()", "codegen", "System.Security.Cryptography.ECDiffieHellmanCngPublicKey", "ExportParameters", True, "benchmark" },
    { 427, "System.Security.Cryptography/System.Security.Cryptography.ECDiffieHellmanOpenSsl::DuplicateKeyHandle:System.Security.Cryptography.SafeEvpPKeyHandle()", "codegen", "System.Security.Cryptography.ECDiffieHellmanOpenSsl", "DuplicateKeyHandle", True, "benchmark" },
    { 428, "System.Security.Cryptography/System.Security.Cryptography.ECDiffieHellmanOpenSsl::ExportParameters:System.Security.Cryptography.ECParameters(System.Boolean)", "codegen", "System.Security.Cryptography.ECDiffieHellmanOpenSsl", "ExportParameters", True, "benchmark" },
    { 429, "System.Security.Cryptography/System.Security.Cryptography.ECDiffieHellmanOpenSsl::ExportParameters:System.Security.Cryptography.ECParameters(System.Boolean)", "codegen", "System.Security.Cryptography.ECDiffieHellmanOpenSsl", "ExportParameters", True, "benchmark" },
    { 430, "System.Security.Cryptography/System.Security.Cryptography.ECDiffieHellmanOpenSsl::ImportParameters:System.Void(System.Security.Cryptography.ECParameters)", "codegen", "System.Security.Cryptography.ECDiffieHellmanOpenSsl", "ImportParameters", True, "benchmark" },
    { 431, "System.Security.Cryptography/System.Security.Cryptography.ECDiffieHellmanPublicKey::Dispose:System.Void()", "codegen", "System.Security.Cryptography.ECDiffieHellmanPublicKey", "Dispose", True, "benchmark" },
    { 432, "System.Security.Cryptography/System.Security.Cryptography.ECDiffieHellmanPublicKey::ToByteArray:System.Byte[]()", "codegen", "System.Security.Cryptography.ECDiffieHellmanPublicKey", "ToByteArray", True, "benchmark" },
    { 433, "System.Security.Cryptography/System.Security.Cryptography.ECDiffieHellmanPublicKey::ToXmlString:System.String()", "codegen", "System.Security.Cryptography.ECDiffieHellmanPublicKey", "ToXmlString", True, "benchmark" },
    { 434, "System.Security.Cryptography/System.Security.Cryptography.ECDiffieHellmanPublicKey::ExportParameters:System.Security.Cryptography.ECParameters()", "codegen", "System.Security.Cryptography.ECDiffieHellmanPublicKey", "ExportParameters", True, "benchmark" },
    { 435, "System.Security.Cryptography/System.Security.Cryptography.ECDiffieHellmanPublicKey::ExportExplicitParameters:System.Security.Cryptography.ECParameters()", "codegen", "System.Security.Cryptography.ECDiffieHellmanPublicKey", "ExportExplicitParameters", True, "benchmark" },
    { 436, "System.Security.Cryptography/System.Security.Cryptography.ECDsa::SignData:System.Byte[](System.Byte[],System.Security.Cryptography.HashAlgorithmName)", "codegen", "System.Security.Cryptography.ECDsa", "SignData", True, "benchmark" },
    { 437, "System.Security.Cryptography/System.Security.Cryptography.ECDsa::SignData:System.Byte[](System.Byte[],System.Security.Cryptography.HashAlgorithmName)", "codegen", "System.Security.Cryptography.ECDsa", "SignData", True, "benchmark" },
    { 438, "System.Security.Cryptography/System.Security.Cryptography.ECDsa::SignData:System.Byte[](System.Byte[],System.Security.Cryptography.HashAlgorithmName)", "codegen", "System.Security.Cryptography.ECDsa", "SignData", True, "benchmark" },
    { 439, "System.Security.Cryptography/System.Security.Cryptography.ECDsa::SignData:System.Byte[](System.Byte[],System.Int32,System.Int32,System.Security.Cryptography.HashAlgorithmName)", "codegen", "System.Security.Cryptography.ECDsa", "SignData", True, "benchmark" },
    { 440, "System.Security.Cryptography/System.Security.Cryptography.ECDsa::SignData:System.Byte[](System.Byte[],System.Int32,System.Int32,System.Security.Cryptography.HashAlgorithmName)", "codegen", "System.Security.Cryptography.ECDsa", "SignData", True, "benchmark" },
    { 441, "System.Security.Cryptography/System.Security.Cryptography.ECDsa::SignData:System.Byte[](System.Byte[],System.Int32,System.Int32,System.Security.Cryptography.HashAlgorithmName)", "codegen", "System.Security.Cryptography.ECDsa", "SignData", True, "benchmark" },
    { 442, "System.Security.Cryptography/System.Security.Cryptography.ECDsa::SignData:System.Byte[](System.Byte[],System.Int32,System.Int32,System.Security.Cryptography.HashAlgorithmName)", "codegen", "System.Security.Cryptography.ECDsa", "SignData", True, "benchmark" },
    { 443, "System.Security.Cryptography/System.Security.Cryptography.ECDsa::SignData:System.Byte[](System.Byte[],System.Int32,System.Int32,System.Security.Cryptography.HashAlgorithmName)", "codegen", "System.Security.Cryptography.ECDsa", "SignData", True, "benchmark" },
    { 444, "System.Security.Cryptography/System.Security.Cryptography.ECDsa::SignData:System.Byte[](System.Byte[],System.Int32,System.Int32,System.Security.Cryptography.HashAlgorithmName,System.Security.Cryptography.DSASignatureFormat)", "codegen", "System.Security.Cryptography.ECDsa", "SignData", True, "benchmark" },
    { 445, "System.Security.Cryptography/System.Security.Cryptography.ECDsa::SignData:System.Byte[](System.Byte[],System.Int32,System.Int32,System.Security.Cryptography.HashAlgorithmName,System.Security.Cryptography.DSASignatureFormat)", "codegen", "System.Security.Cryptography.ECDsa", "SignData", True, "benchmark" },
    { 446, "System.Security.Cryptography/System.Security.Cryptography.ECDsa::SignData:System.Byte[](System.Byte[],System.Int32,System.Int32,System.Security.Cryptography.HashAlgorithmName,System.Security.Cryptography.DSASignatureFormat)", "codegen", "System.Security.Cryptography.ECDsa", "SignData", True, "benchmark" },
    { 447, "System.Security.Cryptography/System.Security.Cryptography.ECDsa::SignData:System.Byte[](System.Byte[],System.Int32,System.Int32,System.Security.Cryptography.HashAlgorithmName,System.Security.Cryptography.DSASignatureFormat)", "codegen", "System.Security.Cryptography.ECDsa", "SignData", True, "benchmark" },
    { 448, "System.Security.Cryptography/System.Security.Cryptography.ECDsa::SignData:System.Byte[](System.Byte[],System.Int32,System.Int32,System.Security.Cryptography.HashAlgorithmName,System.Security.Cryptography.DSASignatureFormat)", "codegen", "System.Security.Cryptography.ECDsa", "SignData", True, "benchmark" },
    { 449, "System.Security.Cryptography/System.Security.Cryptography.ECDsa::SignData:System.Byte[](System.Byte[],System.Security.Cryptography.HashAlgorithmName,System.Security.Cryptography.DSASignatureFormat)", "codegen", "System.Security.Cryptography.ECDsa", "SignData", True, "benchmark" },
    { 450, "System.Security.Cryptography/System.Security.Cryptography.ECDsa::SignData:System.Byte[](System.Byte[],System.Security.Cryptography.HashAlgorithmName,System.Security.Cryptography.DSASignatureFormat)", "codegen", "System.Security.Cryptography.ECDsa", "SignData", True, "benchmark" },
    { 451, "System.Security.Cryptography/System.Security.Cryptography.ECDsa::SignData:System.Byte[](System.Byte[],System.Security.Cryptography.HashAlgorithmName,System.Security.Cryptography.DSASignatureFormat)", "codegen", "System.Security.Cryptography.ECDsa", "SignData", True, "benchmark" },
    { 452, "System.Security.Cryptography/System.Security.Cryptography.ECDsa::SignData:System.Byte[](System.IO.Stream,System.Security.Cryptography.HashAlgorithmName,System.Security.Cryptography.DSASignatureFormat)", "codegen", "System.Security.Cryptography.ECDsa", "SignData", True, "benchmark" },
    { 453, "System.Security.Cryptography/System.Security.Cryptography.ECDsa::SignData:System.Byte[](System.IO.Stream,System.Security.Cryptography.HashAlgorithmName,System.Security.Cryptography.DSASignatureFormat)", "codegen", "System.Security.Cryptography.ECDsa", "SignData", True, "benchmark" },
    { 454, "System.Security.Cryptography/System.Security.Cryptography.ECDsa::SignHash:System.Byte[](System.Byte[],System.Security.Cryptography.DSASignatureFormat)", "codegen", "System.Security.Cryptography.ECDsa", "SignHash", True, "benchmark" },
    { 455, "System.Security.Cryptography/System.Security.Cryptography.ECDsa::SignHash:System.Byte[](System.Byte[],System.Security.Cryptography.DSASignatureFormat)", "codegen", "System.Security.Cryptography.ECDsa", "SignHash", True, "benchmark" },
    { 456, "System.Security.Cryptography/System.Security.Cryptography.ECDsa::SignHash:System.Byte[](System.Byte[],System.Security.Cryptography.DSASignatureFormat)", "codegen", "System.Security.Cryptography.ECDsa", "SignHash", True, "benchmark" },
    { 457, "System.Security.Cryptography/System.Security.Cryptography.ECDsa::SignData:System.Byte[](System.IO.Stream,System.Security.Cryptography.HashAlgorithmName)", "codegen", "System.Security.Cryptography.ECDsa", "SignData", True, "benchmark" },
    { 458, "System.Security.Cryptography/System.Security.Cryptography.ECDsa::SignData:System.Byte[](System.IO.Stream,System.Security.Cryptography.HashAlgorithmName)", "codegen", "System.Security.Cryptography.ECDsa", "SignData", True, "benchmark" },
    { 459, "System.Security.Cryptography/System.Security.Cryptography.ECDsa::VerifyData:System.Boolean(System.Byte[],System.Byte[],System.Security.Cryptography.HashAlgorithmName)", "codegen", "System.Security.Cryptography.ECDsa", "VerifyData", True, "benchmark" },
    { 460, "System.Security.Cryptography/System.Security.Cryptography.ECDsa::VerifyData:System.Boolean(System.Byte[],System.Byte[],System.Security.Cryptography.HashAlgorithmName)", "codegen", "System.Security.Cryptography.ECDsa", "VerifyData", True, "benchmark" },
    { 461, "System.Security.Cryptography/System.Security.Cryptography.ECDsa::VerifyData:System.Boolean(System.Byte[],System.Byte[],System.Security.Cryptography.HashAlgorithmName)", "codegen", "System.Security.Cryptography.ECDsa", "VerifyData", True, "benchmark" },
    { 462, "System.Security.Cryptography/System.Security.Cryptography.ECDsa::VerifyData:System.Boolean(System.Byte[],System.Int32,System.Int32,System.Byte[],System.Security.Cryptography.HashAlgorithmName)", "codegen", "System.Security.Cryptography.ECDsa", "VerifyData", True, "benchmark" },
    { 463, "System.Security.Cryptography/System.Security.Cryptography.ECDsa::VerifyData:System.Boolean(System.Byte[],System.Int32,System.Int32,System.Byte[],System.Security.Cryptography.HashAlgorithmName)", "codegen", "System.Security.Cryptography.ECDsa", "VerifyData", True, "benchmark" },
    { 464, "System.Security.Cryptography/System.Security.Cryptography.ECDsa::VerifyData:System.Boolean(System.Byte[],System.Int32,System.Int32,System.Byte[],System.Security.Cryptography.HashAlgorithmName)", "codegen", "System.Security.Cryptography.ECDsa", "VerifyData", True, "benchmark" },
    { 465, "System.Security.Cryptography/System.Security.Cryptography.ECDsa::VerifyData:System.Boolean(System.Byte[],System.Int32,System.Int32,System.Byte[],System.Security.Cryptography.HashAlgorithmName)", "codegen", "System.Security.Cryptography.ECDsa", "VerifyData", True, "benchmark" },
    { 466, "System.Security.Cryptography/System.Security.Cryptography.ECDsa::VerifyData:System.Boolean(System.Byte[],System.Int32,System.Int32,System.Byte[],System.Security.Cryptography.HashAlgorithmName)", "codegen", "System.Security.Cryptography.ECDsa", "VerifyData", True, "benchmark" },
    { 467, "System.Security.Cryptography/System.Security.Cryptography.ECDsa::VerifyData:System.Boolean(System.Byte[],System.Int32,System.Int32,System.Byte[],System.Security.Cryptography.HashAlgorithmName,System.Security.Cryptography.DSASignatureFormat)", "codegen", "System.Security.Cryptography.ECDsa", "VerifyData", True, "benchmark" },
    { 468, "System.Security.Cryptography/System.Security.Cryptography.ECDsa::VerifyData:System.Boolean(System.Byte[],System.Int32,System.Int32,System.Byte[],System.Security.Cryptography.HashAlgorithmName,System.Security.Cryptography.DSASignatureFormat)", "codegen", "System.Security.Cryptography.ECDsa", "VerifyData", True, "benchmark" },
    { 469, "System.Security.Cryptography/System.Security.Cryptography.ECDsa::VerifyData:System.Boolean(System.Byte[],System.Int32,System.Int32,System.Byte[],System.Security.Cryptography.HashAlgorithmName,System.Security.Cryptography.DSASignatureFormat)", "codegen", "System.Security.Cryptography.ECDsa", "VerifyData", True, "benchmark" },
    { 470, "System.Security.Cryptography/System.Security.Cryptography.ECDsa::VerifyData:System.Boolean(System.Byte[],System.Int32,System.Int32,System.Byte[],System.Security.Cryptography.HashAlgorithmName,System.Security.Cryptography.DSASignatureFormat)", "codegen", "System.Security.Cryptography.ECDsa", "VerifyData", True, "benchmark" },
    { 471, "System.Security.Cryptography/System.Security.Cryptography.ECDsa::VerifyData:System.Boolean(System.Byte[],System.Int32,System.Int32,System.Byte[],System.Security.Cryptography.HashAlgorithmName,System.Security.Cryptography.DSASignatureFormat)", "codegen", "System.Security.Cryptography.ECDsa", "VerifyData", True, "benchmark" },
    { 472, "System.Security.Cryptography/System.Security.Cryptography.ECDsa::VerifyData:System.Boolean(System.Byte[],System.Byte[],System.Security.Cryptography.HashAlgorithmName,System.Security.Cryptography.DSASignatureFormat)", "codegen", "System.Security.Cryptography.ECDsa", "VerifyData", True, "benchmark" },
    { 473, "System.Security.Cryptography/System.Security.Cryptography.ECDsa::VerifyData:System.Boolean(System.Byte[],System.Byte[],System.Security.Cryptography.HashAlgorithmName,System.Security.Cryptography.DSASignatureFormat)", "codegen", "System.Security.Cryptography.ECDsa", "VerifyData", True, "benchmark" },
    { 474, "System.Security.Cryptography/System.Security.Cryptography.ECDsa::VerifyData:System.Boolean(System.Byte[],System.Byte[],System.Security.Cryptography.HashAlgorithmName,System.Security.Cryptography.DSASignatureFormat)", "codegen", "System.Security.Cryptography.ECDsa", "VerifyData", True, "benchmark" },
    { 475, "System.Security.Cryptography/System.Security.Cryptography.ECDsa::VerifyData:System.Boolean(System.IO.Stream,System.Byte[],System.Security.Cryptography.HashAlgorithmName)", "codegen", "System.Security.Cryptography.ECDsa", "VerifyData", True, "benchmark" },
    { 476, "System.Security.Cryptography/System.Security.Cryptography.ECDsa::VerifyData:System.Boolean(System.IO.Stream,System.Byte[],System.Security.Cryptography.HashAlgorithmName)", "codegen", "System.Security.Cryptography.ECDsa", "VerifyData", True, "benchmark" },
    { 477, "System.Security.Cryptography/System.Security.Cryptography.ECDsa::VerifyData:System.Boolean(System.IO.Stream,System.Byte[],System.Security.Cryptography.HashAlgorithmName)", "codegen", "System.Security.Cryptography.ECDsa", "VerifyData", True, "benchmark" },
    { 478, "System.Security.Cryptography/System.Security.Cryptography.ECDsa::VerifyData:System.Boolean(System.IO.Stream,System.Byte[],System.Security.Cryptography.HashAlgorithmName,System.Security.Cryptography.DSASignatureFormat)", "codegen", "System.Security.Cryptography.ECDsa", "VerifyData", True, "benchmark" },
    { 479, "System.Security.Cryptography/System.Security.Cryptography.ECDsa::VerifyData:System.Boolean(System.IO.Stream,System.Byte[],System.Security.Cryptography.HashAlgorithmName,System.Security.Cryptography.DSASignatureFormat)", "codegen", "System.Security.Cryptography.ECDsa", "VerifyData", True, "benchmark" },
    { 480, "System.Security.Cryptography/System.Security.Cryptography.ECDsa::VerifyData:System.Boolean(System.IO.Stream,System.Byte[],System.Security.Cryptography.HashAlgorithmName,System.Security.Cryptography.DSASignatureFormat)", "codegen", "System.Security.Cryptography.ECDsa", "VerifyData", True, "benchmark" },
    { 481, "System.Security.Cryptography/System.Security.Cryptography.ECDsa::SignHash:System.Byte[](System.Byte[])", "codegen", "System.Security.Cryptography.ECDsa", "SignHash", True, "benchmark" },
    { 482, "System.Security.Cryptography/System.Security.Cryptography.ECDsa::SignHash:System.Byte[](System.Byte[])", "codegen", "System.Security.Cryptography.ECDsa", "SignHash", True, "benchmark" },
    { 483, "System.Security.Cryptography/System.Security.Cryptography.ECDsa::SignHash:System.Byte[](System.Byte[])", "codegen", "System.Security.Cryptography.ECDsa", "SignHash", True, "benchmark" },
    { 484, "System.Security.Cryptography/System.Security.Cryptography.ECDsa::VerifyHash:System.Boolean(System.Byte[],System.Byte[])", "codegen", "System.Security.Cryptography.ECDsa", "VerifyHash", True, "benchmark" },
    { 485, "System.Security.Cryptography/System.Security.Cryptography.ECDsa::VerifyHash:System.Boolean(System.Byte[],System.Byte[])", "codegen", "System.Security.Cryptography.ECDsa", "VerifyHash", True, "benchmark" },
    { 486, "System.Security.Cryptography/System.Security.Cryptography.ECDsa::VerifyHash:System.Boolean(System.Byte[],System.Byte[])", "codegen", "System.Security.Cryptography.ECDsa", "VerifyHash", True, "benchmark" },
    { 487, "System.Security.Cryptography/System.Security.Cryptography.ECDsa::VerifyHash:System.Boolean(System.Byte[],System.Byte[],System.Security.Cryptography.DSASignatureFormat)", "codegen", "System.Security.Cryptography.ECDsa", "VerifyHash", True, "benchmark" },
    { 488, "System.Security.Cryptography/System.Security.Cryptography.ECDsa::VerifyHash:System.Boolean(System.Byte[],System.Byte[],System.Security.Cryptography.DSASignatureFormat)", "codegen", "System.Security.Cryptography.ECDsa", "VerifyHash", True, "benchmark" },
    { 489, "System.Security.Cryptography/System.Security.Cryptography.ECDsa::VerifyHash:System.Boolean(System.Byte[],System.Byte[],System.Security.Cryptography.DSASignatureFormat)", "codegen", "System.Security.Cryptography.ECDsa", "VerifyHash", True, "benchmark" },
    { 490, "System.Security.Cryptography/System.Security.Cryptography.ECDsa::GetMaxSignatureSize:System.Int32(System.Security.Cryptography.DSASignatureFormat)", "codegen", "System.Security.Cryptography.ECDsa", "GetMaxSignatureSize", True, "benchmark" },
    { 491, "System.Security.Cryptography/System.Security.Cryptography.ECDsa::FromXmlString:System.Void(System.String)", "codegen", "System.Security.Cryptography.ECDsa", "FromXmlString", True, "benchmark" },
    { 492, "System.Security.Cryptography/System.Security.Cryptography.ECDsa::FromXmlString:System.Void(System.String)", "codegen", "System.Security.Cryptography.ECDsa", "FromXmlString", True, "benchmark" },
    { 493, "System.Security.Cryptography/System.Security.Cryptography.ECDsa::ToXmlString:System.String(System.Boolean)", "codegen", "System.Security.Cryptography.ECDsa", "ToXmlString", True, "benchmark" },
    { 494, "System.Security.Cryptography/System.Security.Cryptography.ECDsa::ToXmlString:System.String(System.Boolean)", "codegen", "System.Security.Cryptography.ECDsa", "ToXmlString", True, "benchmark" },
    { 495, "System.Security.Cryptography/System.Security.Cryptography.ECDsaCng::ImportParameters:System.Void(System.Security.Cryptography.ECParameters)", "codegen", "System.Security.Cryptography.ECDsaCng", "ImportParameters", True, "benchmark" },
    { 496, "System.Security.Cryptography/System.Security.Cryptography.ECDsaCng::ExportExplicitParameters:System.Security.Cryptography.ECParameters(System.Boolean)", "codegen", "System.Security.Cryptography.ECDsaCng", "ExportExplicitParameters", True, "benchmark" },
    { 497, "System.Security.Cryptography/System.Security.Cryptography.ECDsaCng::ExportExplicitParameters:System.Security.Cryptography.ECParameters(System.Boolean)", "codegen", "System.Security.Cryptography.ECDsaCng", "ExportExplicitParameters", True, "benchmark" },
    { 498, "System.Security.Cryptography/System.Security.Cryptography.ECDsaCng::ExportParameters:System.Security.Cryptography.ECParameters(System.Boolean)", "codegen", "System.Security.Cryptography.ECDsaCng", "ExportParameters", True, "benchmark" },
    { 499, "System.Security.Cryptography/System.Security.Cryptography.ECDsaCng::ExportParameters:System.Security.Cryptography.ECParameters(System.Boolean)", "codegen", "System.Security.Cryptography.ECDsaCng", "ExportParameters", True, "benchmark" },
    { 500, "System.Security.Cryptography/System.Security.Cryptography.ECDsaCng::SignHash:System.Byte[](System.Byte[])", "codegen", "System.Security.Cryptography.ECDsaCng", "SignHash", True, "benchmark" },
    { 501, "System.Security.Cryptography/System.Security.Cryptography.ECDsaCng::SignHash:System.Byte[](System.Byte[])", "codegen", "System.Security.Cryptography.ECDsaCng", "SignHash", True, "benchmark" },
    { 502, "System.Security.Cryptography/System.Security.Cryptography.ECDsaCng::SignHash:System.Byte[](System.Byte[])", "codegen", "System.Security.Cryptography.ECDsaCng", "SignHash", True, "benchmark" },
    { 503, "System.Security.Cryptography/System.Security.Cryptography.ECDsaCng::VerifyHash:System.Boolean(System.Byte[],System.Byte[])", "codegen", "System.Security.Cryptography.ECDsaCng", "VerifyHash", True, "benchmark" },
    { 504, "System.Security.Cryptography/System.Security.Cryptography.ECDsaCng::VerifyHash:System.Boolean(System.Byte[],System.Byte[])", "codegen", "System.Security.Cryptography.ECDsaCng", "VerifyHash", True, "benchmark" },
    { 505, "System.Security.Cryptography/System.Security.Cryptography.ECDsaCng::VerifyHash:System.Boolean(System.Byte[],System.Byte[])", "codegen", "System.Security.Cryptography.ECDsaCng", "VerifyHash", True, "benchmark" },
    { 506, "System.Security.Cryptography/System.Security.Cryptography.ECDsaCng::FromXmlString:System.Void(System.String,System.Security.Cryptography.ECKeyXmlFormat)", "codegen", "System.Security.Cryptography.ECDsaCng", "FromXmlString", True, "benchmark" },
    { 507, "System.Security.Cryptography/System.Security.Cryptography.ECDsaCng::FromXmlString:System.Void(System.String,System.Security.Cryptography.ECKeyXmlFormat)", "codegen", "System.Security.Cryptography.ECDsaCng", "FromXmlString", True, "benchmark" },
    { 508, "System.Security.Cryptography/System.Security.Cryptography.ECDsaCng::SignData:System.Byte[](System.Byte[])", "codegen", "System.Security.Cryptography.ECDsaCng", "SignData", True, "benchmark" },
    { 509, "System.Security.Cryptography/System.Security.Cryptography.ECDsaCng::SignData:System.Byte[](System.Byte[])", "codegen", "System.Security.Cryptography.ECDsaCng", "SignData", True, "benchmark" },
    { 510, "System.Security.Cryptography/System.Security.Cryptography.ECDsaCng::SignData:System.Byte[](System.Byte[])", "codegen", "System.Security.Cryptography.ECDsaCng", "SignData", True, "benchmark" },
    { 511, "System.Security.Cryptography/System.Security.Cryptography.ECDsaCng::SignData:System.Byte[](System.Byte[],System.Int32,System.Int32)", "codegen", "System.Security.Cryptography.ECDsaCng", "SignData", True, "benchmark" },
    { 512, "System.Security.Cryptography/System.Security.Cryptography.ECDsaCng::SignData:System.Byte[](System.Byte[],System.Int32,System.Int32)", "codegen", "System.Security.Cryptography.ECDsaCng", "SignData", True, "benchmark" },
    { 513, "System.Security.Cryptography/System.Security.Cryptography.ECDsaCng::SignData:System.Byte[](System.Byte[],System.Int32,System.Int32)", "codegen", "System.Security.Cryptography.ECDsaCng", "SignData", True, "benchmark" },
    { 514, "System.Security.Cryptography/System.Security.Cryptography.ECDsaCng::SignData:System.Byte[](System.Byte[],System.Int32,System.Int32)", "codegen", "System.Security.Cryptography.ECDsaCng", "SignData", True, "benchmark" },
    { 515, "System.Security.Cryptography/System.Security.Cryptography.ECDsaCng::SignData:System.Byte[](System.Byte[],System.Int32,System.Int32)", "codegen", "System.Security.Cryptography.ECDsaCng", "SignData", True, "benchmark" },
    { 516, "System.Security.Cryptography/System.Security.Cryptography.ECDsaCng::SignData:System.Byte[](System.IO.Stream)", "codegen", "System.Security.Cryptography.ECDsaCng", "SignData", True, "benchmark" },
    { 517, "System.Security.Cryptography/System.Security.Cryptography.ECDsaCng::SignData:System.Byte[](System.IO.Stream)", "codegen", "System.Security.Cryptography.ECDsaCng", "SignData", True, "benchmark" },
    { 518, "System.Security.Cryptography/System.Security.Cryptography.ECDsaCng::ToXmlString:System.String(System.Security.Cryptography.ECKeyXmlFormat)", "codegen", "System.Security.Cryptography.ECDsaCng", "ToXmlString", True, "benchmark" },
    { 519, "System.Security.Cryptography/System.Security.Cryptography.ECDsaCng::VerifyData:System.Boolean(System.Byte[],System.Byte[])", "codegen", "System.Security.Cryptography.ECDsaCng", "VerifyData", True, "benchmark" },
    { 520, "System.Security.Cryptography/System.Security.Cryptography.ECDsaCng::VerifyData:System.Boolean(System.Byte[],System.Byte[])", "codegen", "System.Security.Cryptography.ECDsaCng", "VerifyData", True, "benchmark" },
    { 521, "System.Security.Cryptography/System.Security.Cryptography.ECDsaCng::VerifyData:System.Boolean(System.Byte[],System.Byte[])", "codegen", "System.Security.Cryptography.ECDsaCng", "VerifyData", True, "benchmark" },
    { 522, "System.Security.Cryptography/System.Security.Cryptography.ECDsaCng::VerifyData:System.Boolean(System.Byte[],System.Int32,System.Int32,System.Byte[])", "codegen", "System.Security.Cryptography.ECDsaCng", "VerifyData", True, "benchmark" },
    { 523, "System.Security.Cryptography/System.Security.Cryptography.ECDsaCng::VerifyData:System.Boolean(System.Byte[],System.Int32,System.Int32,System.Byte[])", "codegen", "System.Security.Cryptography.ECDsaCng", "VerifyData", True, "benchmark" },
    { 524, "System.Security.Cryptography/System.Security.Cryptography.ECDsaCng::VerifyData:System.Boolean(System.Byte[],System.Int32,System.Int32,System.Byte[])", "codegen", "System.Security.Cryptography.ECDsaCng", "VerifyData", True, "benchmark" },
    { 525, "System.Security.Cryptography/System.Security.Cryptography.ECDsaCng::VerifyData:System.Boolean(System.Byte[],System.Int32,System.Int32,System.Byte[])", "codegen", "System.Security.Cryptography.ECDsaCng", "VerifyData", True, "benchmark" },
    { 526, "System.Security.Cryptography/System.Security.Cryptography.ECDsaCng::VerifyData:System.Boolean(System.Byte[],System.Int32,System.Int32,System.Byte[])", "codegen", "System.Security.Cryptography.ECDsaCng", "VerifyData", True, "benchmark" },
    { 527, "System.Security.Cryptography/System.Security.Cryptography.ECDsaCng::VerifyData:System.Boolean(System.IO.Stream,System.Byte[])", "codegen", "System.Security.Cryptography.ECDsaCng", "VerifyData", True, "benchmark" },
    { 528, "System.Security.Cryptography/System.Security.Cryptography.ECDsaCng::VerifyData:System.Boolean(System.IO.Stream,System.Byte[])", "codegen", "System.Security.Cryptography.ECDsaCng", "VerifyData", True, "benchmark" },
    { 529, "System.Security.Cryptography/System.Security.Cryptography.ECDsaCng::VerifyData:System.Boolean(System.IO.Stream,System.Byte[])", "codegen", "System.Security.Cryptography.ECDsaCng", "VerifyData", True, "benchmark" },
    { 530, "System.Security.Cryptography/System.Security.Cryptography.ECDsaCng::GenerateKey:System.Void(System.Security.Cryptography.ECCurve)", "codegen", "System.Security.Cryptography.ECDsaCng", "GenerateKey", True, "benchmark" },
    { 531, "System.Security.Cryptography/System.Security.Cryptography.ECDsaOpenSsl::DuplicateKeyHandle:System.Security.Cryptography.SafeEvpPKeyHandle()", "codegen", "System.Security.Cryptography.ECDsaOpenSsl", "DuplicateKeyHandle", True, "benchmark" },
    { 532, "System.Security.Cryptography/System.Security.Cryptography.ECDsaOpenSsl::SignHash:System.Byte[](System.Byte[])", "codegen", "System.Security.Cryptography.ECDsaOpenSsl", "SignHash", True, "benchmark" },
    { 533, "System.Security.Cryptography/System.Security.Cryptography.ECDsaOpenSsl::SignHash:System.Byte[](System.Byte[])", "codegen", "System.Security.Cryptography.ECDsaOpenSsl", "SignHash", True, "benchmark" },
    { 534, "System.Security.Cryptography/System.Security.Cryptography.ECDsaOpenSsl::SignHash:System.Byte[](System.Byte[])", "codegen", "System.Security.Cryptography.ECDsaOpenSsl", "SignHash", True, "benchmark" },
    { 535, "System.Security.Cryptography/System.Security.Cryptography.ECDsaOpenSsl::VerifyHash:System.Boolean(System.Byte[],System.Byte[])", "codegen", "System.Security.Cryptography.ECDsaOpenSsl", "VerifyHash", True, "benchmark" },
    { 536, "System.Security.Cryptography/System.Security.Cryptography.ECDsaOpenSsl::VerifyHash:System.Boolean(System.Byte[],System.Byte[])", "codegen", "System.Security.Cryptography.ECDsaOpenSsl", "VerifyHash", True, "benchmark" },
    { 537, "System.Security.Cryptography/System.Security.Cryptography.ECDsaOpenSsl::VerifyHash:System.Boolean(System.Byte[],System.Byte[])", "codegen", "System.Security.Cryptography.ECDsaOpenSsl", "VerifyHash", True, "benchmark" },
    { 538, "System.Security.Cryptography/System.Security.Cryptography.ECParameters::Validate:System.Void()", "codegen", "System.Security.Cryptography.ECParameters", "Validate", True, "benchmark" },
    { 539, "System.Security.Cryptography/System.Security.Cryptography.FromBase64Transform::TransformBlock:System.Int32(System.Byte[],System.Int32,System.Int32,System.Byte[],System.Int32)", "codegen", "System.Security.Cryptography.FromBase64Transform", "TransformBlock", True, "benchmark" },
    { 540, "System.Security.Cryptography/System.Security.Cryptography.FromBase64Transform::TransformBlock:System.Int32(System.Byte[],System.Int32,System.Int32,System.Byte[],System.Int32)", "codegen", "System.Security.Cryptography.FromBase64Transform", "TransformBlock", True, "benchmark" },
    { 541, "System.Security.Cryptography/System.Security.Cryptography.FromBase64Transform::TransformBlock:System.Int32(System.Byte[],System.Int32,System.Int32,System.Byte[],System.Int32)", "codegen", "System.Security.Cryptography.FromBase64Transform", "TransformBlock", True, "benchmark" },
    { 542, "System.Security.Cryptography/System.Security.Cryptography.FromBase64Transform::TransformBlock:System.Int32(System.Byte[],System.Int32,System.Int32,System.Byte[],System.Int32)", "codegen", "System.Security.Cryptography.FromBase64Transform", "TransformBlock", True, "benchmark" },
    { 543, "System.Security.Cryptography/System.Security.Cryptography.FromBase64Transform::TransformBlock:System.Int32(System.Byte[],System.Int32,System.Int32,System.Byte[],System.Int32)", "codegen", "System.Security.Cryptography.FromBase64Transform", "TransformBlock", True, "benchmark" },
    { 544, "System.Security.Cryptography/System.Security.Cryptography.FromBase64Transform::TransformFinalBlock:System.Byte[](System.Byte[],System.Int32,System.Int32)", "codegen", "System.Security.Cryptography.FromBase64Transform", "TransformFinalBlock", True, "benchmark" },
    { 545, "System.Security.Cryptography/System.Security.Cryptography.FromBase64Transform::TransformFinalBlock:System.Byte[](System.Byte[],System.Int32,System.Int32)", "codegen", "System.Security.Cryptography.FromBase64Transform", "TransformFinalBlock", True, "benchmark" },
    { 546, "System.Security.Cryptography/System.Security.Cryptography.FromBase64Transform::TransformFinalBlock:System.Byte[](System.Byte[],System.Int32,System.Int32)", "codegen", "System.Security.Cryptography.FromBase64Transform", "TransformFinalBlock", True, "benchmark" },
    { 547, "System.Security.Cryptography/System.Security.Cryptography.FromBase64Transform::TransformFinalBlock:System.Byte[](System.Byte[],System.Int32,System.Int32)", "codegen", "System.Security.Cryptography.FromBase64Transform", "TransformFinalBlock", True, "benchmark" },
    { 548, "System.Security.Cryptography/System.Security.Cryptography.FromBase64Transform::TransformFinalBlock:System.Byte[](System.Byte[],System.Int32,System.Int32)", "codegen", "System.Security.Cryptography.FromBase64Transform", "TransformFinalBlock", True, "benchmark" },
    { 549, "System.Security.Cryptography/System.Security.Cryptography.FromBase64Transform::Clear:System.Void()", "codegen", "System.Security.Cryptography.FromBase64Transform", "Clear", True, "benchmark" },
    { 550, "System.Security.Cryptography/System.Security.Cryptography.FromBase64Transform::Dispose:System.Void()", "codegen", "System.Security.Cryptography.FromBase64Transform", "Dispose", True, "benchmark" },
    { 551, "System.Security.Cryptography/System.Security.Cryptography.HashAlgorithm::ComputeHash:System.Byte[](System.Byte[])", "codegen", "System.Security.Cryptography.HashAlgorithm", "ComputeHash", True, "benchmark" },
    { 552, "System.Security.Cryptography/System.Security.Cryptography.HashAlgorithm::ComputeHash:System.Byte[](System.Byte[])", "codegen", "System.Security.Cryptography.HashAlgorithm", "ComputeHash", True, "benchmark" },
    { 553, "System.Security.Cryptography/System.Security.Cryptography.HashAlgorithm::ComputeHash:System.Byte[](System.Byte[])", "codegen", "System.Security.Cryptography.HashAlgorithm", "ComputeHash", True, "benchmark" },
    { 554, "System.Security.Cryptography/System.Security.Cryptography.HashAlgorithm::ComputeHash:System.Byte[](System.Byte[],System.Int32,System.Int32)", "codegen", "System.Security.Cryptography.HashAlgorithm", "ComputeHash", True, "benchmark" },
    { 555, "System.Security.Cryptography/System.Security.Cryptography.HashAlgorithm::ComputeHash:System.Byte[](System.Byte[],System.Int32,System.Int32)", "codegen", "System.Security.Cryptography.HashAlgorithm", "ComputeHash", True, "benchmark" },
    { 556, "System.Security.Cryptography/System.Security.Cryptography.HashAlgorithm::ComputeHash:System.Byte[](System.Byte[],System.Int32,System.Int32)", "codegen", "System.Security.Cryptography.HashAlgorithm", "ComputeHash", True, "benchmark" },
    { 557, "System.Security.Cryptography/System.Security.Cryptography.HashAlgorithm::ComputeHash:System.Byte[](System.Byte[],System.Int32,System.Int32)", "codegen", "System.Security.Cryptography.HashAlgorithm", "ComputeHash", True, "benchmark" },
    { 558, "System.Security.Cryptography/System.Security.Cryptography.HashAlgorithm::ComputeHash:System.Byte[](System.Byte[],System.Int32,System.Int32)", "codegen", "System.Security.Cryptography.HashAlgorithm", "ComputeHash", True, "benchmark" },
    { 559, "System.Security.Cryptography/System.Security.Cryptography.HashAlgorithm::ComputeHash:System.Byte[](System.IO.Stream)", "codegen", "System.Security.Cryptography.HashAlgorithm", "ComputeHash", True, "benchmark" },
    { 560, "System.Security.Cryptography/System.Security.Cryptography.HashAlgorithm::ComputeHash:System.Byte[](System.IO.Stream)", "codegen", "System.Security.Cryptography.HashAlgorithm", "ComputeHash", True, "benchmark" },
    { 561, "System.Security.Cryptography/System.Security.Cryptography.HashAlgorithm::ComputeHashAsync:System.Threading.Tasks.Task<System.Byte[]>(System.IO.Stream,System.Threading.CancellationToken)", "codegen", "System.Security.Cryptography.HashAlgorithm", "ComputeHashAsync", True, "benchmark" },
    { 562, "System.Security.Cryptography/System.Security.Cryptography.HashAlgorithm::ComputeHashAsync:System.Threading.Tasks.Task<System.Byte[]>(System.IO.Stream,System.Threading.CancellationToken)", "codegen", "System.Security.Cryptography.HashAlgorithm", "ComputeHashAsync", True, "benchmark" },
    { 563, "System.Security.Cryptography/System.Security.Cryptography.HashAlgorithm::Dispose:System.Void()", "codegen", "System.Security.Cryptography.HashAlgorithm", "Dispose", True, "benchmark" },
    { 564, "System.Security.Cryptography/System.Security.Cryptography.HashAlgorithm::Clear:System.Void()", "codegen", "System.Security.Cryptography.HashAlgorithm", "Clear", True, "benchmark" },
    { 565, "System.Security.Cryptography/System.Security.Cryptography.HashAlgorithm::TransformBlock:System.Int32(System.Byte[],System.Int32,System.Int32,System.Byte[],System.Int32)", "codegen", "System.Security.Cryptography.HashAlgorithm", "TransformBlock", True, "benchmark" },
    { 566, "System.Security.Cryptography/System.Security.Cryptography.HashAlgorithm::TransformBlock:System.Int32(System.Byte[],System.Int32,System.Int32,System.Byte[],System.Int32)", "codegen", "System.Security.Cryptography.HashAlgorithm", "TransformBlock", True, "benchmark" },
    { 567, "System.Security.Cryptography/System.Security.Cryptography.HashAlgorithm::TransformBlock:System.Int32(System.Byte[],System.Int32,System.Int32,System.Byte[],System.Int32)", "codegen", "System.Security.Cryptography.HashAlgorithm", "TransformBlock", True, "benchmark" },
    { 568, "System.Security.Cryptography/System.Security.Cryptography.HashAlgorithm::TransformBlock:System.Int32(System.Byte[],System.Int32,System.Int32,System.Byte[],System.Int32)", "codegen", "System.Security.Cryptography.HashAlgorithm", "TransformBlock", True, "benchmark" },
    { 569, "System.Security.Cryptography/System.Security.Cryptography.HashAlgorithm::TransformBlock:System.Int32(System.Byte[],System.Int32,System.Int32,System.Byte[],System.Int32)", "codegen", "System.Security.Cryptography.HashAlgorithm", "TransformBlock", True, "benchmark" },
    { 570, "System.Security.Cryptography/System.Security.Cryptography.HashAlgorithm::TransformFinalBlock:System.Byte[](System.Byte[],System.Int32,System.Int32)", "codegen", "System.Security.Cryptography.HashAlgorithm", "TransformFinalBlock", True, "benchmark" },
    { 571, "System.Security.Cryptography/System.Security.Cryptography.HashAlgorithm::TransformFinalBlock:System.Byte[](System.Byte[],System.Int32,System.Int32)", "codegen", "System.Security.Cryptography.HashAlgorithm", "TransformFinalBlock", True, "benchmark" },
    { 572, "System.Security.Cryptography/System.Security.Cryptography.HashAlgorithm::TransformFinalBlock:System.Byte[](System.Byte[],System.Int32,System.Int32)", "codegen", "System.Security.Cryptography.HashAlgorithm", "TransformFinalBlock", True, "benchmark" },
    { 573, "System.Security.Cryptography/System.Security.Cryptography.HashAlgorithm::TransformFinalBlock:System.Byte[](System.Byte[],System.Int32,System.Int32)", "codegen", "System.Security.Cryptography.HashAlgorithm", "TransformFinalBlock", True, "benchmark" },
    { 574, "System.Security.Cryptography/System.Security.Cryptography.HashAlgorithm::TransformFinalBlock:System.Byte[](System.Byte[],System.Int32,System.Int32)", "codegen", "System.Security.Cryptography.HashAlgorithm", "TransformFinalBlock", True, "benchmark" },
    { 575, "System.Security.Cryptography/System.Security.Cryptography.HashAlgorithm::Initialize:System.Void()", "codegen", "System.Security.Cryptography.HashAlgorithm", "Initialize", True, "benchmark" },
    { 576, "System.Security.Cryptography/System.Security.Cryptography.HashAlgorithmName::TryFromOid:System.Boolean(System.String,System.Security.Cryptography.HashAlgorithmName)", "codegen", "System.Security.Cryptography.HashAlgorithmName", "TryFromOid", True, "benchmark" },
    { 577, "System.Security.Cryptography/System.Security.Cryptography.HashAlgorithmName::TryFromOid:System.Boolean(System.String,System.Security.Cryptography.HashAlgorithmName)", "codegen", "System.Security.Cryptography.HashAlgorithmName", "TryFromOid", True, "benchmark" },
    { 578, "System.Security.Cryptography/System.Security.Cryptography.HashAlgorithmName::FromOid:System.Security.Cryptography.HashAlgorithmName(System.String)", "codegen", "System.Security.Cryptography.HashAlgorithmName", "FromOid", True, "benchmark" },
    { 579, "System.Security.Cryptography/System.Security.Cryptography.HashAlgorithmName::FromOid:System.Security.Cryptography.HashAlgorithmName(System.String)", "codegen", "System.Security.Cryptography.HashAlgorithmName", "FromOid", True, "benchmark" },
    { 580, "System.Security.Cryptography/System.Security.Cryptography.HKDF::Extract:System.Byte[](System.Security.Cryptography.HashAlgorithmName,System.Byte[],System.Byte[])", "codegen", "System.Security.Cryptography.HKDF", "Extract", True, "benchmark" },
    { 581, "System.Security.Cryptography/System.Security.Cryptography.HKDF::Extract:System.Byte[](System.Security.Cryptography.HashAlgorithmName,System.Byte[],System.Byte[])", "codegen", "System.Security.Cryptography.HKDF", "Extract", True, "benchmark" },
    { 582, "System.Security.Cryptography/System.Security.Cryptography.HKDF::Extract:System.Byte[](System.Security.Cryptography.HashAlgorithmName,System.Byte[],System.Byte[])", "codegen", "System.Security.Cryptography.HKDF", "Extract", True, "benchmark" },
    { 583, "System.Security.Cryptography/System.Security.Cryptography.HKDF::Expand:System.Byte[](System.Security.Cryptography.HashAlgorithmName,System.Byte[],System.Int32,System.Byte[])", "codegen", "System.Security.Cryptography.HKDF", "Expand", True, "benchmark" },
    { 584, "System.Security.Cryptography/System.Security.Cryptography.HKDF::Expand:System.Byte[](System.Security.Cryptography.HashAlgorithmName,System.Byte[],System.Int32,System.Byte[])", "codegen", "System.Security.Cryptography.HKDF", "Expand", True, "benchmark" },
    { 585, "System.Security.Cryptography/System.Security.Cryptography.HKDF::Expand:System.Byte[](System.Security.Cryptography.HashAlgorithmName,System.Byte[],System.Int32,System.Byte[])", "codegen", "System.Security.Cryptography.HKDF", "Expand", True, "benchmark" },
    { 586, "System.Security.Cryptography/System.Security.Cryptography.HKDF::Expand:System.Byte[](System.Security.Cryptography.HashAlgorithmName,System.Byte[],System.Int32,System.Byte[])", "codegen", "System.Security.Cryptography.HKDF", "Expand", True, "benchmark" },
    { 587, "System.Security.Cryptography/System.Security.Cryptography.HKDF::DeriveKey:System.Byte[](System.Security.Cryptography.HashAlgorithmName,System.Byte[],System.Int32,System.Byte[],System.Byte[])", "codegen", "System.Security.Cryptography.HKDF", "DeriveKey", True, "benchmark" },
    { 588, "System.Security.Cryptography/System.Security.Cryptography.HKDF::DeriveKey:System.Byte[](System.Security.Cryptography.HashAlgorithmName,System.Byte[],System.Int32,System.Byte[],System.Byte[])", "codegen", "System.Security.Cryptography.HKDF", "DeriveKey", True, "benchmark" },
    { 589, "System.Security.Cryptography/System.Security.Cryptography.HKDF::DeriveKey:System.Byte[](System.Security.Cryptography.HashAlgorithmName,System.Byte[],System.Int32,System.Byte[],System.Byte[])", "codegen", "System.Security.Cryptography.HKDF", "DeriveKey", True, "benchmark" },
    { 590, "System.Security.Cryptography/System.Security.Cryptography.HKDF::DeriveKey:System.Byte[](System.Security.Cryptography.HashAlgorithmName,System.Byte[],System.Int32,System.Byte[],System.Byte[])", "codegen", "System.Security.Cryptography.HKDF", "DeriveKey", True, "benchmark" },
    { 591, "System.Security.Cryptography/System.Security.Cryptography.HMAC::Initialize:System.Void()", "codegen", "System.Security.Cryptography.HMAC", "Initialize", True, "benchmark" },
    { 592, "System.Security.Cryptography/System.Security.Cryptography.HMACMD5::Initialize:System.Void()", "codegen", "System.Security.Cryptography.HMACMD5", "Initialize", True, "benchmark" },
    { 593, "System.Security.Cryptography/System.Security.Cryptography.HMACMD5::HashData:System.Byte[](System.Byte[],System.Byte[])", "codegen", "System.Security.Cryptography.HMACMD5", "HashData", True, "benchmark" },
    { 594, "System.Security.Cryptography/System.Security.Cryptography.HMACMD5::HashData:System.Byte[](System.Byte[],System.Byte[])", "codegen", "System.Security.Cryptography.HMACMD5", "HashData", True, "benchmark" },
    { 595, "System.Security.Cryptography/System.Security.Cryptography.HMACMD5::HashData:System.Byte[](System.Byte[],System.Byte[])", "codegen", "System.Security.Cryptography.HMACMD5", "HashData", True, "benchmark" },
    { 596, "System.Security.Cryptography/System.Security.Cryptography.HMACMD5::HashData:System.Byte[](System.Byte[],System.IO.Stream)", "codegen", "System.Security.Cryptography.HMACMD5", "HashData", True, "benchmark" },
    { 597, "System.Security.Cryptography/System.Security.Cryptography.HMACMD5::HashData:System.Byte[](System.Byte[],System.IO.Stream)", "codegen", "System.Security.Cryptography.HMACMD5", "HashData", True, "benchmark" },
    { 598, "System.Security.Cryptography/System.Security.Cryptography.HMACMD5::HashData:System.Byte[](System.Byte[],System.IO.Stream)", "codegen", "System.Security.Cryptography.HMACMD5", "HashData", True, "benchmark" },
    { 599, "System.Security.Cryptography/System.Security.Cryptography.HMACMD5::HashDataAsync:System.Threading.Tasks.ValueTask<System.Byte[]>(System.ReadOnlyMemory<System.Byte>,System.IO.Stream,System.Threading.CancellationToken)", "codegen", "System.Security.Cryptography.HMACMD5", "HashDataAsync", True, "benchmark" },
    { 600, "System.Security.Cryptography/System.Security.Cryptography.HMACMD5::HashDataAsync:System.Threading.Tasks.ValueTask<System.Byte[]>(System.ReadOnlyMemory<System.Byte>,System.IO.Stream,System.Threading.CancellationToken)", "codegen", "System.Security.Cryptography.HMACMD5", "HashDataAsync", True, "benchmark" },
    { 601, "System.Security.Cryptography/System.Security.Cryptography.HMACMD5::HashDataAsync:System.Threading.Tasks.ValueTask<System.Byte[]>(System.Byte[],System.IO.Stream,System.Threading.CancellationToken)", "codegen", "System.Security.Cryptography.HMACMD5", "HashDataAsync", True, "benchmark" },
    { 602, "System.Security.Cryptography/System.Security.Cryptography.HMACMD5::HashDataAsync:System.Threading.Tasks.ValueTask<System.Byte[]>(System.Byte[],System.IO.Stream,System.Threading.CancellationToken)", "codegen", "System.Security.Cryptography.HMACMD5", "HashDataAsync", True, "benchmark" },
    { 603, "System.Security.Cryptography/System.Security.Cryptography.HMACMD5::HashDataAsync:System.Threading.Tasks.ValueTask<System.Byte[]>(System.Byte[],System.IO.Stream,System.Threading.CancellationToken)", "codegen", "System.Security.Cryptography.HMACMD5", "HashDataAsync", True, "benchmark" },
    { 604, "System.Security.Cryptography/System.Security.Cryptography.HMACMD5::HashDataAsync:System.Threading.Tasks.ValueTask<System.Int32>(System.ReadOnlyMemory<System.Byte>,System.IO.Stream,System.Memory<System.Byte>,System.Threading.CancellationToken)", "codegen", "System.Security.Cryptography.HMACMD5", "HashDataAsync", True, "benchmark" },
    { 605, "System.Security.Cryptography/System.Security.Cryptography.HMACMD5::HashDataAsync:System.Threading.Tasks.ValueTask<System.Int32>(System.ReadOnlyMemory<System.Byte>,System.IO.Stream,System.Memory<System.Byte>,System.Threading.CancellationToken)", "codegen", "System.Security.Cryptography.HMACMD5", "HashDataAsync", True, "benchmark" },
    { 606, "System.Security.Cryptography/System.Security.Cryptography.HMACSHA1::Initialize:System.Void()", "codegen", "System.Security.Cryptography.HMACSHA1", "Initialize", True, "benchmark" },
    { 607, "System.Security.Cryptography/System.Security.Cryptography.HMACSHA1::HashData:System.Byte[](System.Byte[],System.Byte[])", "codegen", "System.Security.Cryptography.HMACSHA1", "HashData", True, "benchmark" },
    { 608, "System.Security.Cryptography/System.Security.Cryptography.HMACSHA1::HashData:System.Byte[](System.Byte[],System.Byte[])", "codegen", "System.Security.Cryptography.HMACSHA1", "HashData", True, "benchmark" },
    { 609, "System.Security.Cryptography/System.Security.Cryptography.HMACSHA1::HashData:System.Byte[](System.Byte[],System.Byte[])", "codegen", "System.Security.Cryptography.HMACSHA1", "HashData", True, "benchmark" },
    { 610, "System.Security.Cryptography/System.Security.Cryptography.HMACSHA1::HashData:System.Byte[](System.Byte[],System.IO.Stream)", "codegen", "System.Security.Cryptography.HMACSHA1", "HashData", True, "benchmark" },
    { 611, "System.Security.Cryptography/System.Security.Cryptography.HMACSHA1::HashData:System.Byte[](System.Byte[],System.IO.Stream)", "codegen", "System.Security.Cryptography.HMACSHA1", "HashData", True, "benchmark" },
    { 612, "System.Security.Cryptography/System.Security.Cryptography.HMACSHA1::HashData:System.Byte[](System.Byte[],System.IO.Stream)", "codegen", "System.Security.Cryptography.HMACSHA1", "HashData", True, "benchmark" },
    { 613, "System.Security.Cryptography/System.Security.Cryptography.HMACSHA1::HashDataAsync:System.Threading.Tasks.ValueTask<System.Byte[]>(System.ReadOnlyMemory<System.Byte>,System.IO.Stream,System.Threading.CancellationToken)", "codegen", "System.Security.Cryptography.HMACSHA1", "HashDataAsync", True, "benchmark" },
    { 614, "System.Security.Cryptography/System.Security.Cryptography.HMACSHA1::HashDataAsync:System.Threading.Tasks.ValueTask<System.Byte[]>(System.ReadOnlyMemory<System.Byte>,System.IO.Stream,System.Threading.CancellationToken)", "codegen", "System.Security.Cryptography.HMACSHA1", "HashDataAsync", True, "benchmark" },
    { 615, "System.Security.Cryptography/System.Security.Cryptography.HMACSHA1::HashDataAsync:System.Threading.Tasks.ValueTask<System.Byte[]>(System.Byte[],System.IO.Stream,System.Threading.CancellationToken)", "codegen", "System.Security.Cryptography.HMACSHA1", "HashDataAsync", True, "benchmark" },
    { 616, "System.Security.Cryptography/System.Security.Cryptography.HMACSHA1::HashDataAsync:System.Threading.Tasks.ValueTask<System.Byte[]>(System.Byte[],System.IO.Stream,System.Threading.CancellationToken)", "codegen", "System.Security.Cryptography.HMACSHA1", "HashDataAsync", True, "benchmark" },
    { 617, "System.Security.Cryptography/System.Security.Cryptography.HMACSHA1::HashDataAsync:System.Threading.Tasks.ValueTask<System.Byte[]>(System.Byte[],System.IO.Stream,System.Threading.CancellationToken)", "codegen", "System.Security.Cryptography.HMACSHA1", "HashDataAsync", True, "benchmark" },
    { 618, "System.Security.Cryptography/System.Security.Cryptography.HMACSHA1::HashDataAsync:System.Threading.Tasks.ValueTask<System.Int32>(System.ReadOnlyMemory<System.Byte>,System.IO.Stream,System.Memory<System.Byte>,System.Threading.CancellationToken)", "codegen", "System.Security.Cryptography.HMACSHA1", "HashDataAsync", True, "benchmark" },
    { 619, "System.Security.Cryptography/System.Security.Cryptography.HMACSHA1::HashDataAsync:System.Threading.Tasks.ValueTask<System.Int32>(System.ReadOnlyMemory<System.Byte>,System.IO.Stream,System.Memory<System.Byte>,System.Threading.CancellationToken)", "codegen", "System.Security.Cryptography.HMACSHA1", "HashDataAsync", True, "benchmark" },
    { 620, "System.Security.Cryptography/System.Security.Cryptography.HMACSHA256::Initialize:System.Void()", "codegen", "System.Security.Cryptography.HMACSHA256", "Initialize", True, "benchmark" },
    { 621, "System.Security.Cryptography/System.Security.Cryptography.HMACSHA256::HashData:System.Byte[](System.Byte[],System.Byte[])", "codegen", "System.Security.Cryptography.HMACSHA256", "HashData", True, "benchmark" },
    { 622, "System.Security.Cryptography/System.Security.Cryptography.HMACSHA256::HashData:System.Byte[](System.Byte[],System.Byte[])", "codegen", "System.Security.Cryptography.HMACSHA256", "HashData", True, "benchmark" },
    { 623, "System.Security.Cryptography/System.Security.Cryptography.HMACSHA256::HashData:System.Byte[](System.Byte[],System.Byte[])", "codegen", "System.Security.Cryptography.HMACSHA256", "HashData", True, "benchmark" },
    { 624, "System.Security.Cryptography/System.Security.Cryptography.HMACSHA256::HashData:System.Byte[](System.Byte[],System.IO.Stream)", "codegen", "System.Security.Cryptography.HMACSHA256", "HashData", True, "benchmark" },
    { 625, "System.Security.Cryptography/System.Security.Cryptography.HMACSHA256::HashData:System.Byte[](System.Byte[],System.IO.Stream)", "codegen", "System.Security.Cryptography.HMACSHA256", "HashData", True, "benchmark" },
    { 626, "System.Security.Cryptography/System.Security.Cryptography.HMACSHA256::HashData:System.Byte[](System.Byte[],System.IO.Stream)", "codegen", "System.Security.Cryptography.HMACSHA256", "HashData", True, "benchmark" },
    { 627, "System.Security.Cryptography/System.Security.Cryptography.HMACSHA256::HashDataAsync:System.Threading.Tasks.ValueTask<System.Byte[]>(System.ReadOnlyMemory<System.Byte>,System.IO.Stream,System.Threading.CancellationToken)", "codegen", "System.Security.Cryptography.HMACSHA256", "HashDataAsync", True, "benchmark" },
    { 628, "System.Security.Cryptography/System.Security.Cryptography.HMACSHA256::HashDataAsync:System.Threading.Tasks.ValueTask<System.Byte[]>(System.ReadOnlyMemory<System.Byte>,System.IO.Stream,System.Threading.CancellationToken)", "codegen", "System.Security.Cryptography.HMACSHA256", "HashDataAsync", True, "benchmark" },
    { 629, "System.Security.Cryptography/System.Security.Cryptography.HMACSHA256::HashDataAsync:System.Threading.Tasks.ValueTask<System.Byte[]>(System.Byte[],System.IO.Stream,System.Threading.CancellationToken)", "codegen", "System.Security.Cryptography.HMACSHA256", "HashDataAsync", True, "benchmark" },
    { 630, "System.Security.Cryptography/System.Security.Cryptography.HMACSHA256::HashDataAsync:System.Threading.Tasks.ValueTask<System.Byte[]>(System.Byte[],System.IO.Stream,System.Threading.CancellationToken)", "codegen", "System.Security.Cryptography.HMACSHA256", "HashDataAsync", True, "benchmark" },
    { 631, "System.Security.Cryptography/System.Security.Cryptography.HMACSHA256::HashDataAsync:System.Threading.Tasks.ValueTask<System.Byte[]>(System.Byte[],System.IO.Stream,System.Threading.CancellationToken)", "codegen", "System.Security.Cryptography.HMACSHA256", "HashDataAsync", True, "benchmark" },
    { 632, "System.Security.Cryptography/System.Security.Cryptography.HMACSHA256::HashDataAsync:System.Threading.Tasks.ValueTask<System.Int32>(System.ReadOnlyMemory<System.Byte>,System.IO.Stream,System.Memory<System.Byte>,System.Threading.CancellationToken)", "codegen", "System.Security.Cryptography.HMACSHA256", "HashDataAsync", True, "benchmark" },
    { 633, "System.Security.Cryptography/System.Security.Cryptography.HMACSHA256::HashDataAsync:System.Threading.Tasks.ValueTask<System.Int32>(System.ReadOnlyMemory<System.Byte>,System.IO.Stream,System.Memory<System.Byte>,System.Threading.CancellationToken)", "codegen", "System.Security.Cryptography.HMACSHA256", "HashDataAsync", True, "benchmark" },
    { 634, "System.Security.Cryptography/System.Security.Cryptography.HMACSHA3_256::Initialize:System.Void()", "codegen", "System.Security.Cryptography.HMACSHA3_256", "Initialize", True, "benchmark" },
    { 635, "System.Security.Cryptography/System.Security.Cryptography.HMACSHA3_256::HashData:System.Byte[](System.Byte[],System.Byte[])", "codegen", "System.Security.Cryptography.HMACSHA3_256", "HashData", True, "benchmark" },
    { 636, "System.Security.Cryptography/System.Security.Cryptography.HMACSHA3_256::HashData:System.Byte[](System.Byte[],System.Byte[])", "codegen", "System.Security.Cryptography.HMACSHA3_256", "HashData", True, "benchmark" },
    { 637, "System.Security.Cryptography/System.Security.Cryptography.HMACSHA3_256::HashData:System.Byte[](System.Byte[],System.Byte[])", "codegen", "System.Security.Cryptography.HMACSHA3_256", "HashData", True, "benchmark" },
    { 638, "System.Security.Cryptography/System.Security.Cryptography.HMACSHA3_256::HashData:System.Byte[](System.Byte[],System.IO.Stream)", "codegen", "System.Security.Cryptography.HMACSHA3_256", "HashData", True, "benchmark" },
    { 639, "System.Security.Cryptography/System.Security.Cryptography.HMACSHA3_256::HashData:System.Byte[](System.Byte[],System.IO.Stream)", "codegen", "System.Security.Cryptography.HMACSHA3_256", "HashData", True, "benchmark" },
    { 640, "System.Security.Cryptography/System.Security.Cryptography.HMACSHA3_256::HashData:System.Byte[](System.Byte[],System.IO.Stream)", "codegen", "System.Security.Cryptography.HMACSHA3_256", "HashData", True, "benchmark" },
    { 641, "System.Security.Cryptography/System.Security.Cryptography.HMACSHA3_256::HashDataAsync:System.Threading.Tasks.ValueTask<System.Byte[]>(System.ReadOnlyMemory<System.Byte>,System.IO.Stream,System.Threading.CancellationToken)", "codegen", "System.Security.Cryptography.HMACSHA3_256", "HashDataAsync", True, "benchmark" },
    { 642, "System.Security.Cryptography/System.Security.Cryptography.HMACSHA3_256::HashDataAsync:System.Threading.Tasks.ValueTask<System.Byte[]>(System.ReadOnlyMemory<System.Byte>,System.IO.Stream,System.Threading.CancellationToken)", "codegen", "System.Security.Cryptography.HMACSHA3_256", "HashDataAsync", True, "benchmark" },
    { 643, "System.Security.Cryptography/System.Security.Cryptography.HMACSHA3_256::HashDataAsync:System.Threading.Tasks.ValueTask<System.Byte[]>(System.Byte[],System.IO.Stream,System.Threading.CancellationToken)", "codegen", "System.Security.Cryptography.HMACSHA3_256", "HashDataAsync", True, "benchmark" },
    { 644, "System.Security.Cryptography/System.Security.Cryptography.HMACSHA3_256::HashDataAsync:System.Threading.Tasks.ValueTask<System.Byte[]>(System.Byte[],System.IO.Stream,System.Threading.CancellationToken)", "codegen", "System.Security.Cryptography.HMACSHA3_256", "HashDataAsync", True, "benchmark" },
    { 645, "System.Security.Cryptography/System.Security.Cryptography.HMACSHA3_256::HashDataAsync:System.Threading.Tasks.ValueTask<System.Byte[]>(System.Byte[],System.IO.Stream,System.Threading.CancellationToken)", "codegen", "System.Security.Cryptography.HMACSHA3_256", "HashDataAsync", True, "benchmark" },
    { 646, "System.Security.Cryptography/System.Security.Cryptography.HMACSHA3_256::HashDataAsync:System.Threading.Tasks.ValueTask<System.Int32>(System.ReadOnlyMemory<System.Byte>,System.IO.Stream,System.Memory<System.Byte>,System.Threading.CancellationToken)", "codegen", "System.Security.Cryptography.HMACSHA3_256", "HashDataAsync", True, "benchmark" },
    { 647, "System.Security.Cryptography/System.Security.Cryptography.HMACSHA3_256::HashDataAsync:System.Threading.Tasks.ValueTask<System.Int32>(System.ReadOnlyMemory<System.Byte>,System.IO.Stream,System.Memory<System.Byte>,System.Threading.CancellationToken)", "codegen", "System.Security.Cryptography.HMACSHA3_256", "HashDataAsync", True, "benchmark" },
    { 648, "System.Security.Cryptography/System.Security.Cryptography.HMACSHA3_384::Initialize:System.Void()", "codegen", "System.Security.Cryptography.HMACSHA3_384", "Initialize", True, "benchmark" },
    { 649, "System.Security.Cryptography/System.Security.Cryptography.HMACSHA3_384::HashData:System.Byte[](System.Byte[],System.Byte[])", "codegen", "System.Security.Cryptography.HMACSHA3_384", "HashData", True, "benchmark" },
    { 650, "System.Security.Cryptography/System.Security.Cryptography.HMACSHA3_384::HashData:System.Byte[](System.Byte[],System.Byte[])", "codegen", "System.Security.Cryptography.HMACSHA3_384", "HashData", True, "benchmark" },
    { 651, "System.Security.Cryptography/System.Security.Cryptography.HMACSHA3_384::HashData:System.Byte[](System.Byte[],System.Byte[])", "codegen", "System.Security.Cryptography.HMACSHA3_384", "HashData", True, "benchmark" },
    { 652, "System.Security.Cryptography/System.Security.Cryptography.HMACSHA3_384::HashData:System.Byte[](System.Byte[],System.IO.Stream)", "codegen", "System.Security.Cryptography.HMACSHA3_384", "HashData", True, "benchmark" },
    { 653, "System.Security.Cryptography/System.Security.Cryptography.HMACSHA3_384::HashData:System.Byte[](System.Byte[],System.IO.Stream)", "codegen", "System.Security.Cryptography.HMACSHA3_384", "HashData", True, "benchmark" },
    { 654, "System.Security.Cryptography/System.Security.Cryptography.HMACSHA3_384::HashData:System.Byte[](System.Byte[],System.IO.Stream)", "codegen", "System.Security.Cryptography.HMACSHA3_384", "HashData", True, "benchmark" },
    { 655, "System.Security.Cryptography/System.Security.Cryptography.HMACSHA3_384::HashDataAsync:System.Threading.Tasks.ValueTask<System.Byte[]>(System.ReadOnlyMemory<System.Byte>,System.IO.Stream,System.Threading.CancellationToken)", "codegen", "System.Security.Cryptography.HMACSHA3_384", "HashDataAsync", True, "benchmark" },
    { 656, "System.Security.Cryptography/System.Security.Cryptography.HMACSHA3_384::HashDataAsync:System.Threading.Tasks.ValueTask<System.Byte[]>(System.ReadOnlyMemory<System.Byte>,System.IO.Stream,System.Threading.CancellationToken)", "codegen", "System.Security.Cryptography.HMACSHA3_384", "HashDataAsync", True, "benchmark" },
    { 657, "System.Security.Cryptography/System.Security.Cryptography.HMACSHA3_384::HashDataAsync:System.Threading.Tasks.ValueTask<System.Byte[]>(System.Byte[],System.IO.Stream,System.Threading.CancellationToken)", "codegen", "System.Security.Cryptography.HMACSHA3_384", "HashDataAsync", True, "benchmark" },
    { 658, "System.Security.Cryptography/System.Security.Cryptography.HMACSHA3_384::HashDataAsync:System.Threading.Tasks.ValueTask<System.Byte[]>(System.Byte[],System.IO.Stream,System.Threading.CancellationToken)", "codegen", "System.Security.Cryptography.HMACSHA3_384", "HashDataAsync", True, "benchmark" },
    { 659, "System.Security.Cryptography/System.Security.Cryptography.HMACSHA3_384::HashDataAsync:System.Threading.Tasks.ValueTask<System.Byte[]>(System.Byte[],System.IO.Stream,System.Threading.CancellationToken)", "codegen", "System.Security.Cryptography.HMACSHA3_384", "HashDataAsync", True, "benchmark" },
    { 660, "System.Security.Cryptography/System.Security.Cryptography.HMACSHA3_384::HashDataAsync:System.Threading.Tasks.ValueTask<System.Int32>(System.ReadOnlyMemory<System.Byte>,System.IO.Stream,System.Memory<System.Byte>,System.Threading.CancellationToken)", "codegen", "System.Security.Cryptography.HMACSHA3_384", "HashDataAsync", True, "benchmark" },
    { 661, "System.Security.Cryptography/System.Security.Cryptography.HMACSHA3_384::HashDataAsync:System.Threading.Tasks.ValueTask<System.Int32>(System.ReadOnlyMemory<System.Byte>,System.IO.Stream,System.Memory<System.Byte>,System.Threading.CancellationToken)", "codegen", "System.Security.Cryptography.HMACSHA3_384", "HashDataAsync", True, "benchmark" },
    { 662, "System.Security.Cryptography/System.Security.Cryptography.HMACSHA3_512::Initialize:System.Void()", "codegen", "System.Security.Cryptography.HMACSHA3_512", "Initialize", True, "benchmark" },
    { 663, "System.Security.Cryptography/System.Security.Cryptography.HMACSHA3_512::HashData:System.Byte[](System.Byte[],System.Byte[])", "codegen", "System.Security.Cryptography.HMACSHA3_512", "HashData", True, "benchmark" },
    { 664, "System.Security.Cryptography/System.Security.Cryptography.HMACSHA3_512::HashData:System.Byte[](System.Byte[],System.Byte[])", "codegen", "System.Security.Cryptography.HMACSHA3_512", "HashData", True, "benchmark" },
    { 665, "System.Security.Cryptography/System.Security.Cryptography.HMACSHA3_512::HashData:System.Byte[](System.Byte[],System.Byte[])", "codegen", "System.Security.Cryptography.HMACSHA3_512", "HashData", True, "benchmark" },
    { 666, "System.Security.Cryptography/System.Security.Cryptography.HMACSHA3_512::HashData:System.Byte[](System.Byte[],System.IO.Stream)", "codegen", "System.Security.Cryptography.HMACSHA3_512", "HashData", True, "benchmark" },
    { 667, "System.Security.Cryptography/System.Security.Cryptography.HMACSHA3_512::HashData:System.Byte[](System.Byte[],System.IO.Stream)", "codegen", "System.Security.Cryptography.HMACSHA3_512", "HashData", True, "benchmark" },
    { 668, "System.Security.Cryptography/System.Security.Cryptography.HMACSHA3_512::HashData:System.Byte[](System.Byte[],System.IO.Stream)", "codegen", "System.Security.Cryptography.HMACSHA3_512", "HashData", True, "benchmark" },
    { 669, "System.Security.Cryptography/System.Security.Cryptography.HMACSHA3_512::HashDataAsync:System.Threading.Tasks.ValueTask<System.Byte[]>(System.ReadOnlyMemory<System.Byte>,System.IO.Stream,System.Threading.CancellationToken)", "codegen", "System.Security.Cryptography.HMACSHA3_512", "HashDataAsync", True, "benchmark" },
    { 670, "System.Security.Cryptography/System.Security.Cryptography.HMACSHA3_512::HashDataAsync:System.Threading.Tasks.ValueTask<System.Byte[]>(System.ReadOnlyMemory<System.Byte>,System.IO.Stream,System.Threading.CancellationToken)", "codegen", "System.Security.Cryptography.HMACSHA3_512", "HashDataAsync", True, "benchmark" },
    { 671, "System.Security.Cryptography/System.Security.Cryptography.HMACSHA3_512::HashDataAsync:System.Threading.Tasks.ValueTask<System.Byte[]>(System.Byte[],System.IO.Stream,System.Threading.CancellationToken)", "codegen", "System.Security.Cryptography.HMACSHA3_512", "HashDataAsync", True, "benchmark" },
    { 672, "System.Security.Cryptography/System.Security.Cryptography.HMACSHA3_512::HashDataAsync:System.Threading.Tasks.ValueTask<System.Byte[]>(System.Byte[],System.IO.Stream,System.Threading.CancellationToken)", "codegen", "System.Security.Cryptography.HMACSHA3_512", "HashDataAsync", True, "benchmark" },
    { 673, "System.Security.Cryptography/System.Security.Cryptography.HMACSHA3_512::HashDataAsync:System.Threading.Tasks.ValueTask<System.Byte[]>(System.Byte[],System.IO.Stream,System.Threading.CancellationToken)", "codegen", "System.Security.Cryptography.HMACSHA3_512", "HashDataAsync", True, "benchmark" },
    { 674, "System.Security.Cryptography/System.Security.Cryptography.HMACSHA3_512::HashDataAsync:System.Threading.Tasks.ValueTask<System.Int32>(System.ReadOnlyMemory<System.Byte>,System.IO.Stream,System.Memory<System.Byte>,System.Threading.CancellationToken)", "codegen", "System.Security.Cryptography.HMACSHA3_512", "HashDataAsync", True, "benchmark" },
    { 675, "System.Security.Cryptography/System.Security.Cryptography.HMACSHA3_512::HashDataAsync:System.Threading.Tasks.ValueTask<System.Int32>(System.ReadOnlyMemory<System.Byte>,System.IO.Stream,System.Memory<System.Byte>,System.Threading.CancellationToken)", "codegen", "System.Security.Cryptography.HMACSHA3_512", "HashDataAsync", True, "benchmark" },
    { 676, "System.Security.Cryptography/System.Security.Cryptography.HMACSHA384::Initialize:System.Void()", "codegen", "System.Security.Cryptography.HMACSHA384", "Initialize", True, "benchmark" },
    { 677, "System.Security.Cryptography/System.Security.Cryptography.HMACSHA384::HashData:System.Byte[](System.Byte[],System.Byte[])", "codegen", "System.Security.Cryptography.HMACSHA384", "HashData", True, "benchmark" },
    { 678, "System.Security.Cryptography/System.Security.Cryptography.HMACSHA384::HashData:System.Byte[](System.Byte[],System.Byte[])", "codegen", "System.Security.Cryptography.HMACSHA384", "HashData", True, "benchmark" },
    { 679, "System.Security.Cryptography/System.Security.Cryptography.HMACSHA384::HashData:System.Byte[](System.Byte[],System.Byte[])", "codegen", "System.Security.Cryptography.HMACSHA384", "HashData", True, "benchmark" },
    { 680, "System.Security.Cryptography/System.Security.Cryptography.HMACSHA384::HashData:System.Byte[](System.Byte[],System.IO.Stream)", "codegen", "System.Security.Cryptography.HMACSHA384", "HashData", True, "benchmark" },
    { 681, "System.Security.Cryptography/System.Security.Cryptography.HMACSHA384::HashData:System.Byte[](System.Byte[],System.IO.Stream)", "codegen", "System.Security.Cryptography.HMACSHA384", "HashData", True, "benchmark" },
    { 682, "System.Security.Cryptography/System.Security.Cryptography.HMACSHA384::HashData:System.Byte[](System.Byte[],System.IO.Stream)", "codegen", "System.Security.Cryptography.HMACSHA384", "HashData", True, "benchmark" },
    { 683, "System.Security.Cryptography/System.Security.Cryptography.HMACSHA384::HashDataAsync:System.Threading.Tasks.ValueTask<System.Byte[]>(System.ReadOnlyMemory<System.Byte>,System.IO.Stream,System.Threading.CancellationToken)", "codegen", "System.Security.Cryptography.HMACSHA384", "HashDataAsync", True, "benchmark" },
    { 684, "System.Security.Cryptography/System.Security.Cryptography.HMACSHA384::HashDataAsync:System.Threading.Tasks.ValueTask<System.Byte[]>(System.ReadOnlyMemory<System.Byte>,System.IO.Stream,System.Threading.CancellationToken)", "codegen", "System.Security.Cryptography.HMACSHA384", "HashDataAsync", True, "benchmark" },
    { 685, "System.Security.Cryptography/System.Security.Cryptography.HMACSHA384::HashDataAsync:System.Threading.Tasks.ValueTask<System.Byte[]>(System.Byte[],System.IO.Stream,System.Threading.CancellationToken)", "codegen", "System.Security.Cryptography.HMACSHA384", "HashDataAsync", True, "benchmark" },
    { 686, "System.Security.Cryptography/System.Security.Cryptography.HMACSHA384::HashDataAsync:System.Threading.Tasks.ValueTask<System.Byte[]>(System.Byte[],System.IO.Stream,System.Threading.CancellationToken)", "codegen", "System.Security.Cryptography.HMACSHA384", "HashDataAsync", True, "benchmark" },
    { 687, "System.Security.Cryptography/System.Security.Cryptography.HMACSHA384::HashDataAsync:System.Threading.Tasks.ValueTask<System.Byte[]>(System.Byte[],System.IO.Stream,System.Threading.CancellationToken)", "codegen", "System.Security.Cryptography.HMACSHA384", "HashDataAsync", True, "benchmark" },
    { 688, "System.Security.Cryptography/System.Security.Cryptography.HMACSHA384::HashDataAsync:System.Threading.Tasks.ValueTask<System.Int32>(System.ReadOnlyMemory<System.Byte>,System.IO.Stream,System.Memory<System.Byte>,System.Threading.CancellationToken)", "codegen", "System.Security.Cryptography.HMACSHA384", "HashDataAsync", True, "benchmark" },
    { 689, "System.Security.Cryptography/System.Security.Cryptography.HMACSHA384::HashDataAsync:System.Threading.Tasks.ValueTask<System.Int32>(System.ReadOnlyMemory<System.Byte>,System.IO.Stream,System.Memory<System.Byte>,System.Threading.CancellationToken)", "codegen", "System.Security.Cryptography.HMACSHA384", "HashDataAsync", True, "benchmark" },
    { 690, "System.Security.Cryptography/System.Security.Cryptography.HMACSHA512::Initialize:System.Void()", "codegen", "System.Security.Cryptography.HMACSHA512", "Initialize", True, "benchmark" },
    { 691, "System.Security.Cryptography/System.Security.Cryptography.HMACSHA512::HashData:System.Byte[](System.Byte[],System.Byte[])", "codegen", "System.Security.Cryptography.HMACSHA512", "HashData", True, "benchmark" },
    { 692, "System.Security.Cryptography/System.Security.Cryptography.HMACSHA512::HashData:System.Byte[](System.Byte[],System.Byte[])", "codegen", "System.Security.Cryptography.HMACSHA512", "HashData", True, "benchmark" },
    { 693, "System.Security.Cryptography/System.Security.Cryptography.HMACSHA512::HashData:System.Byte[](System.Byte[],System.Byte[])", "codegen", "System.Security.Cryptography.HMACSHA512", "HashData", True, "benchmark" },
    { 694, "System.Security.Cryptography/System.Security.Cryptography.HMACSHA512::HashData:System.Byte[](System.Byte[],System.IO.Stream)", "codegen", "System.Security.Cryptography.HMACSHA512", "HashData", True, "benchmark" },
    { 695, "System.Security.Cryptography/System.Security.Cryptography.HMACSHA512::HashData:System.Byte[](System.Byte[],System.IO.Stream)", "codegen", "System.Security.Cryptography.HMACSHA512", "HashData", True, "benchmark" },
    { 696, "System.Security.Cryptography/System.Security.Cryptography.HMACSHA512::HashData:System.Byte[](System.Byte[],System.IO.Stream)", "codegen", "System.Security.Cryptography.HMACSHA512", "HashData", True, "benchmark" },
    { 697, "System.Security.Cryptography/System.Security.Cryptography.HMACSHA512::HashDataAsync:System.Threading.Tasks.ValueTask<System.Byte[]>(System.ReadOnlyMemory<System.Byte>,System.IO.Stream,System.Threading.CancellationToken)", "codegen", "System.Security.Cryptography.HMACSHA512", "HashDataAsync", True, "benchmark" },
    { 698, "System.Security.Cryptography/System.Security.Cryptography.HMACSHA512::HashDataAsync:System.Threading.Tasks.ValueTask<System.Byte[]>(System.ReadOnlyMemory<System.Byte>,System.IO.Stream,System.Threading.CancellationToken)", "codegen", "System.Security.Cryptography.HMACSHA512", "HashDataAsync", True, "benchmark" },
    { 699, "System.Security.Cryptography/System.Security.Cryptography.HMACSHA512::HashDataAsync:System.Threading.Tasks.ValueTask<System.Byte[]>(System.Byte[],System.IO.Stream,System.Threading.CancellationToken)", "codegen", "System.Security.Cryptography.HMACSHA512", "HashDataAsync", True, "benchmark" },
    { 700, "System.Security.Cryptography/System.Security.Cryptography.HMACSHA512::HashDataAsync:System.Threading.Tasks.ValueTask<System.Byte[]>(System.Byte[],System.IO.Stream,System.Threading.CancellationToken)", "codegen", "System.Security.Cryptography.HMACSHA512", "HashDataAsync", True, "benchmark" },
    { 701, "System.Security.Cryptography/System.Security.Cryptography.HMACSHA512::HashDataAsync:System.Threading.Tasks.ValueTask<System.Byte[]>(System.Byte[],System.IO.Stream,System.Threading.CancellationToken)", "codegen", "System.Security.Cryptography.HMACSHA512", "HashDataAsync", True, "benchmark" },
    { 702, "System.Security.Cryptography/System.Security.Cryptography.HMACSHA512::HashDataAsync:System.Threading.Tasks.ValueTask<System.Int32>(System.ReadOnlyMemory<System.Byte>,System.IO.Stream,System.Memory<System.Byte>,System.Threading.CancellationToken)", "codegen", "System.Security.Cryptography.HMACSHA512", "HashDataAsync", True, "benchmark" },
    { 703, "System.Security.Cryptography/System.Security.Cryptography.HMACSHA512::HashDataAsync:System.Threading.Tasks.ValueTask<System.Int32>(System.ReadOnlyMemory<System.Byte>,System.IO.Stream,System.Memory<System.Byte>,System.Threading.CancellationToken)", "codegen", "System.Security.Cryptography.HMACSHA512", "HashDataAsync", True, "benchmark" },
    { 704, "System.Security.Cryptography/System.Security.Cryptography.IncrementalHash::AppendData:System.Void(System.Byte[])", "codegen", "System.Security.Cryptography.IncrementalHash", "AppendData", True, "benchmark" },
    { 705, "System.Security.Cryptography/System.Security.Cryptography.IncrementalHash::AppendData:System.Void(System.Byte[])", "codegen", "System.Security.Cryptography.IncrementalHash", "AppendData", True, "benchmark" },
    { 706, "System.Security.Cryptography/System.Security.Cryptography.IncrementalHash::AppendData:System.Void(System.Byte[])", "codegen", "System.Security.Cryptography.IncrementalHash", "AppendData", True, "benchmark" },
    { 707, "System.Security.Cryptography/System.Security.Cryptography.IncrementalHash::AppendData:System.Void(System.Byte[],System.Int32,System.Int32)", "codegen", "System.Security.Cryptography.IncrementalHash", "AppendData", True, "benchmark" },
    { 708, "System.Security.Cryptography/System.Security.Cryptography.IncrementalHash::AppendData:System.Void(System.Byte[],System.Int32,System.Int32)", "codegen", "System.Security.Cryptography.IncrementalHash", "AppendData", True, "benchmark" },
    { 709, "System.Security.Cryptography/System.Security.Cryptography.IncrementalHash::AppendData:System.Void(System.Byte[],System.Int32,System.Int32)", "codegen", "System.Security.Cryptography.IncrementalHash", "AppendData", True, "benchmark" },
    { 710, "System.Security.Cryptography/System.Security.Cryptography.IncrementalHash::AppendData:System.Void(System.Byte[],System.Int32,System.Int32)", "codegen", "System.Security.Cryptography.IncrementalHash", "AppendData", True, "benchmark" },
    { 711, "System.Security.Cryptography/System.Security.Cryptography.IncrementalHash::AppendData:System.Void(System.Byte[],System.Int32,System.Int32)", "codegen", "System.Security.Cryptography.IncrementalHash", "AppendData", True, "benchmark" },
    { 712, "System.Security.Cryptography/System.Security.Cryptography.IncrementalHash::GetHashAndReset:System.Byte[]()", "codegen", "System.Security.Cryptography.IncrementalHash", "GetHashAndReset", True, "benchmark" },
    { 713, "System.Security.Cryptography/System.Security.Cryptography.IncrementalHash::GetCurrentHash:System.Byte[]()", "codegen", "System.Security.Cryptography.IncrementalHash", "GetCurrentHash", True, "benchmark" },
    { 714, "System.Security.Cryptography/System.Security.Cryptography.IncrementalHash::Clone:System.Security.Cryptography.IncrementalHash()", "codegen", "System.Security.Cryptography.IncrementalHash", "Clone", True, "benchmark" },
    { 715, "System.Security.Cryptography/System.Security.Cryptography.IncrementalHash::Dispose:System.Void()", "codegen", "System.Security.Cryptography.IncrementalHash", "Dispose", True, "benchmark" },
    { 716, "System.Security.Cryptography/System.Security.Cryptography.IncrementalHash::CreateHash:System.Security.Cryptography.IncrementalHash(System.Security.Cryptography.HashAlgorithmName)", "codegen", "System.Security.Cryptography.IncrementalHash", "CreateHash", True, "benchmark" },
    { 717, "System.Security.Cryptography/System.Security.Cryptography.IncrementalHash::CreateHMAC:System.Security.Cryptography.IncrementalHash(System.Security.Cryptography.HashAlgorithmName,System.Byte[])", "codegen", "System.Security.Cryptography.IncrementalHash", "CreateHMAC", True, "benchmark" },
    { 718, "System.Security.Cryptography/System.Security.Cryptography.IncrementalHash::CreateHMAC:System.Security.Cryptography.IncrementalHash(System.Security.Cryptography.HashAlgorithmName,System.Byte[])", "codegen", "System.Security.Cryptography.IncrementalHash", "CreateHMAC", True, "benchmark" },
    { 719, "System.Security.Cryptography/System.Security.Cryptography.IncrementalHash::CreateHMAC:System.Security.Cryptography.IncrementalHash(System.Security.Cryptography.HashAlgorithmName,System.Byte[])", "codegen", "System.Security.Cryptography.IncrementalHash", "CreateHMAC", True, "benchmark" },
    { 720, "System.Security.Cryptography/System.Security.Cryptography.Kmac128::AppendData:System.Void(System.Byte[])", "codegen", "System.Security.Cryptography.Kmac128", "AppendData", True, "benchmark" },
    { 721, "System.Security.Cryptography/System.Security.Cryptography.Kmac128::AppendData:System.Void(System.Byte[])", "codegen", "System.Security.Cryptography.Kmac128", "AppendData", True, "benchmark" },
    { 722, "System.Security.Cryptography/System.Security.Cryptography.Kmac128::AppendData:System.Void(System.Byte[])", "codegen", "System.Security.Cryptography.Kmac128", "AppendData", True, "benchmark" },
    { 723, "System.Security.Cryptography/System.Security.Cryptography.Kmac128::GetHashAndReset:System.Byte[](System.Int32)", "codegen", "System.Security.Cryptography.Kmac128", "GetHashAndReset", True, "benchmark" },
    { 724, "System.Security.Cryptography/System.Security.Cryptography.Kmac128::GetHashAndReset:System.Byte[](System.Int32)", "codegen", "System.Security.Cryptography.Kmac128", "GetHashAndReset", True, "benchmark" },
    { 725, "System.Security.Cryptography/System.Security.Cryptography.Kmac128::GetCurrentHash:System.Byte[](System.Int32)", "codegen", "System.Security.Cryptography.Kmac128", "GetCurrentHash", True, "benchmark" },
    { 726, "System.Security.Cryptography/System.Security.Cryptography.Kmac128::GetCurrentHash:System.Byte[](System.Int32)", "codegen", "System.Security.Cryptography.Kmac128", "GetCurrentHash", True, "benchmark" },
    { 727, "System.Security.Cryptography/System.Security.Cryptography.Kmac128::Clone:System.Security.Cryptography.Kmac128()", "codegen", "System.Security.Cryptography.Kmac128", "Clone", True, "benchmark" },
    { 728, "System.Security.Cryptography/System.Security.Cryptography.Kmac128::Dispose:System.Void()", "codegen", "System.Security.Cryptography.Kmac128", "Dispose", True, "benchmark" },
    { 729, "System.Security.Cryptography/System.Security.Cryptography.Kmac128::HashData:System.Byte[](System.Byte[],System.Byte[],System.Int32,System.Byte[])", "codegen", "System.Security.Cryptography.Kmac128", "HashData", True, "benchmark" },
    { 730, "System.Security.Cryptography/System.Security.Cryptography.Kmac128::HashData:System.Byte[](System.Byte[],System.Byte[],System.Int32,System.Byte[])", "codegen", "System.Security.Cryptography.Kmac128", "HashData", True, "benchmark" },
    { 731, "System.Security.Cryptography/System.Security.Cryptography.Kmac128::HashData:System.Byte[](System.Byte[],System.Byte[],System.Int32,System.Byte[])", "codegen", "System.Security.Cryptography.Kmac128", "HashData", True, "benchmark" },
    { 732, "System.Security.Cryptography/System.Security.Cryptography.Kmac128::HashData:System.Byte[](System.Byte[],System.Byte[],System.Int32,System.Byte[])", "codegen", "System.Security.Cryptography.Kmac128", "HashData", True, "benchmark" },
    { 733, "System.Security.Cryptography/System.Security.Cryptography.Kmac128::HashData:System.Byte[](System.Byte[],System.IO.Stream,System.Int32,System.Byte[])", "codegen", "System.Security.Cryptography.Kmac128", "HashData", True, "benchmark" },
    { 734, "System.Security.Cryptography/System.Security.Cryptography.Kmac128::HashData:System.Byte[](System.Byte[],System.IO.Stream,System.Int32,System.Byte[])", "codegen", "System.Security.Cryptography.Kmac128", "HashData", True, "benchmark" },
    { 735, "System.Security.Cryptography/System.Security.Cryptography.Kmac128::HashData:System.Byte[](System.Byte[],System.IO.Stream,System.Int32,System.Byte[])", "codegen", "System.Security.Cryptography.Kmac128", "HashData", True, "benchmark" },
    { 736, "System.Security.Cryptography/System.Security.Cryptography.Kmac128::HashData:System.Byte[](System.Byte[],System.IO.Stream,System.Int32,System.Byte[])", "codegen", "System.Security.Cryptography.Kmac128", "HashData", True, "benchmark" },
    { 737, "System.Security.Cryptography/System.Security.Cryptography.Kmac128::HashDataAsync:System.Threading.Tasks.ValueTask<System.Byte[]>(System.Byte[],System.IO.Stream,System.Int32,System.Byte[],System.Threading.CancellationToken)", "codegen", "System.Security.Cryptography.Kmac128", "HashDataAsync", True, "benchmark" },
    { 738, "System.Security.Cryptography/System.Security.Cryptography.Kmac128::HashDataAsync:System.Threading.Tasks.ValueTask<System.Byte[]>(System.Byte[],System.IO.Stream,System.Int32,System.Byte[],System.Threading.CancellationToken)", "codegen", "System.Security.Cryptography.Kmac128", "HashDataAsync", True, "benchmark" },
    { 739, "System.Security.Cryptography/System.Security.Cryptography.Kmac128::HashDataAsync:System.Threading.Tasks.ValueTask<System.Byte[]>(System.Byte[],System.IO.Stream,System.Int32,System.Byte[],System.Threading.CancellationToken)", "codegen", "System.Security.Cryptography.Kmac128", "HashDataAsync", True, "benchmark" },
    { 740, "System.Security.Cryptography/System.Security.Cryptography.Kmac128::HashDataAsync:System.Threading.Tasks.ValueTask<System.Byte[]>(System.Byte[],System.IO.Stream,System.Int32,System.Byte[],System.Threading.CancellationToken)", "codegen", "System.Security.Cryptography.Kmac128", "HashDataAsync", True, "benchmark" },
    { 741, "System.Security.Cryptography/System.Security.Cryptography.Kmac128::HashDataAsync:System.Threading.Tasks.ValueTask<System.Byte[]>(System.ReadOnlyMemory<System.Byte>,System.IO.Stream,System.Int32,System.ReadOnlyMemory<System.Byte>,System.Threading.CancellationToken)", "codegen", "System.Security.Cryptography.Kmac128", "HashDataAsync", True, "benchmark" },
    { 742, "System.Security.Cryptography/System.Security.Cryptography.Kmac128::HashDataAsync:System.Threading.Tasks.ValueTask<System.Byte[]>(System.ReadOnlyMemory<System.Byte>,System.IO.Stream,System.Int32,System.ReadOnlyMemory<System.Byte>,System.Threading.CancellationToken)", "codegen", "System.Security.Cryptography.Kmac128", "HashDataAsync", True, "benchmark" },
    { 743, "System.Security.Cryptography/System.Security.Cryptography.Kmac128::HashDataAsync:System.Threading.Tasks.ValueTask<System.Byte[]>(System.ReadOnlyMemory<System.Byte>,System.IO.Stream,System.Int32,System.ReadOnlyMemory<System.Byte>,System.Threading.CancellationToken)", "codegen", "System.Security.Cryptography.Kmac128", "HashDataAsync", True, "benchmark" },
    { 744, "System.Security.Cryptography/System.Security.Cryptography.Kmac128::HashDataAsync:System.Threading.Tasks.ValueTask(System.ReadOnlyMemory<System.Byte>,System.IO.Stream,System.Memory<System.Byte>,System.ReadOnlyMemory<System.Byte>,System.Threading.CancellationToken)", "codegen", "System.Security.Cryptography.Kmac128", "HashDataAsync", True, "benchmark" },
    { 745, "System.Security.Cryptography/System.Security.Cryptography.Kmac128::HashDataAsync:System.Threading.Tasks.ValueTask(System.ReadOnlyMemory<System.Byte>,System.IO.Stream,System.Memory<System.Byte>,System.ReadOnlyMemory<System.Byte>,System.Threading.CancellationToken)", "codegen", "System.Security.Cryptography.Kmac128", "HashDataAsync", True, "benchmark" },
    { 746, "System.Security.Cryptography/System.Security.Cryptography.Kmac256::AppendData:System.Void(System.Byte[])", "codegen", "System.Security.Cryptography.Kmac256", "AppendData", True, "benchmark" },
    { 747, "System.Security.Cryptography/System.Security.Cryptography.Kmac256::AppendData:System.Void(System.Byte[])", "codegen", "System.Security.Cryptography.Kmac256", "AppendData", True, "benchmark" },
    { 748, "System.Security.Cryptography/System.Security.Cryptography.Kmac256::AppendData:System.Void(System.Byte[])", "codegen", "System.Security.Cryptography.Kmac256", "AppendData", True, "benchmark" },
    { 749, "System.Security.Cryptography/System.Security.Cryptography.Kmac256::GetHashAndReset:System.Byte[](System.Int32)", "codegen", "System.Security.Cryptography.Kmac256", "GetHashAndReset", True, "benchmark" },
    { 750, "System.Security.Cryptography/System.Security.Cryptography.Kmac256::GetHashAndReset:System.Byte[](System.Int32)", "codegen", "System.Security.Cryptography.Kmac256", "GetHashAndReset", True, "benchmark" },
    { 751, "System.Security.Cryptography/System.Security.Cryptography.Kmac256::GetCurrentHash:System.Byte[](System.Int32)", "codegen", "System.Security.Cryptography.Kmac256", "GetCurrentHash", True, "benchmark" },
    { 752, "System.Security.Cryptography/System.Security.Cryptography.Kmac256::GetCurrentHash:System.Byte[](System.Int32)", "codegen", "System.Security.Cryptography.Kmac256", "GetCurrentHash", True, "benchmark" },
    { 753, "System.Security.Cryptography/System.Security.Cryptography.Kmac256::Clone:System.Security.Cryptography.Kmac256()", "codegen", "System.Security.Cryptography.Kmac256", "Clone", True, "benchmark" },
    { 754, "System.Security.Cryptography/System.Security.Cryptography.Kmac256::Dispose:System.Void()", "codegen", "System.Security.Cryptography.Kmac256", "Dispose", True, "benchmark" },
    { 755, "System.Security.Cryptography/System.Security.Cryptography.Kmac256::HashData:System.Byte[](System.Byte[],System.Byte[],System.Int32,System.Byte[])", "codegen", "System.Security.Cryptography.Kmac256", "HashData", True, "benchmark" },
    { 756, "System.Security.Cryptography/System.Security.Cryptography.Kmac256::HashData:System.Byte[](System.Byte[],System.Byte[],System.Int32,System.Byte[])", "codegen", "System.Security.Cryptography.Kmac256", "HashData", True, "benchmark" },
    { 757, "System.Security.Cryptography/System.Security.Cryptography.Kmac256::HashData:System.Byte[](System.Byte[],System.Byte[],System.Int32,System.Byte[])", "codegen", "System.Security.Cryptography.Kmac256", "HashData", True, "benchmark" },
    { 758, "System.Security.Cryptography/System.Security.Cryptography.Kmac256::HashData:System.Byte[](System.Byte[],System.Byte[],System.Int32,System.Byte[])", "codegen", "System.Security.Cryptography.Kmac256", "HashData", True, "benchmark" },
    { 759, "System.Security.Cryptography/System.Security.Cryptography.Kmac256::HashData:System.Byte[](System.Byte[],System.IO.Stream,System.Int32,System.Byte[])", "codegen", "System.Security.Cryptography.Kmac256", "HashData", True, "benchmark" },
    { 760, "System.Security.Cryptography/System.Security.Cryptography.Kmac256::HashData:System.Byte[](System.Byte[],System.IO.Stream,System.Int32,System.Byte[])", "codegen", "System.Security.Cryptography.Kmac256", "HashData", True, "benchmark" },
    { 761, "System.Security.Cryptography/System.Security.Cryptography.Kmac256::HashData:System.Byte[](System.Byte[],System.IO.Stream,System.Int32,System.Byte[])", "codegen", "System.Security.Cryptography.Kmac256", "HashData", True, "benchmark" },
    { 762, "System.Security.Cryptography/System.Security.Cryptography.Kmac256::HashData:System.Byte[](System.Byte[],System.IO.Stream,System.Int32,System.Byte[])", "codegen", "System.Security.Cryptography.Kmac256", "HashData", True, "benchmark" },
    { 763, "System.Security.Cryptography/System.Security.Cryptography.Kmac256::HashDataAsync:System.Threading.Tasks.ValueTask<System.Byte[]>(System.Byte[],System.IO.Stream,System.Int32,System.Byte[],System.Threading.CancellationToken)", "codegen", "System.Security.Cryptography.Kmac256", "HashDataAsync", True, "benchmark" },
    { 764, "System.Security.Cryptography/System.Security.Cryptography.Kmac256::HashDataAsync:System.Threading.Tasks.ValueTask<System.Byte[]>(System.Byte[],System.IO.Stream,System.Int32,System.Byte[],System.Threading.CancellationToken)", "codegen", "System.Security.Cryptography.Kmac256", "HashDataAsync", True, "benchmark" },
    { 765, "System.Security.Cryptography/System.Security.Cryptography.Kmac256::HashDataAsync:System.Threading.Tasks.ValueTask<System.Byte[]>(System.Byte[],System.IO.Stream,System.Int32,System.Byte[],System.Threading.CancellationToken)", "codegen", "System.Security.Cryptography.Kmac256", "HashDataAsync", True, "benchmark" },
    { 766, "System.Security.Cryptography/System.Security.Cryptography.Kmac256::HashDataAsync:System.Threading.Tasks.ValueTask<System.Byte[]>(System.Byte[],System.IO.Stream,System.Int32,System.Byte[],System.Threading.CancellationToken)", "codegen", "System.Security.Cryptography.Kmac256", "HashDataAsync", True, "benchmark" },
    { 767, "System.Security.Cryptography/System.Security.Cryptography.Kmac256::HashDataAsync:System.Threading.Tasks.ValueTask<System.Byte[]>(System.ReadOnlyMemory<System.Byte>,System.IO.Stream,System.Int32,System.ReadOnlyMemory<System.Byte>,System.Threading.CancellationToken)", "codegen", "System.Security.Cryptography.Kmac256", "HashDataAsync", True, "benchmark" },
    { 768, "System.Security.Cryptography/System.Security.Cryptography.Kmac256::HashDataAsync:System.Threading.Tasks.ValueTask<System.Byte[]>(System.ReadOnlyMemory<System.Byte>,System.IO.Stream,System.Int32,System.ReadOnlyMemory<System.Byte>,System.Threading.CancellationToken)", "codegen", "System.Security.Cryptography.Kmac256", "HashDataAsync", True, "benchmark" },
    { 769, "System.Security.Cryptography/System.Security.Cryptography.Kmac256::HashDataAsync:System.Threading.Tasks.ValueTask<System.Byte[]>(System.ReadOnlyMemory<System.Byte>,System.IO.Stream,System.Int32,System.ReadOnlyMemory<System.Byte>,System.Threading.CancellationToken)", "codegen", "System.Security.Cryptography.Kmac256", "HashDataAsync", True, "benchmark" },
    { 770, "System.Security.Cryptography/System.Security.Cryptography.Kmac256::HashDataAsync:System.Threading.Tasks.ValueTask(System.ReadOnlyMemory<System.Byte>,System.IO.Stream,System.Memory<System.Byte>,System.ReadOnlyMemory<System.Byte>,System.Threading.CancellationToken)", "codegen", "System.Security.Cryptography.Kmac256", "HashDataAsync", True, "benchmark" },
    { 771, "System.Security.Cryptography/System.Security.Cryptography.Kmac256::HashDataAsync:System.Threading.Tasks.ValueTask(System.ReadOnlyMemory<System.Byte>,System.IO.Stream,System.Memory<System.Byte>,System.ReadOnlyMemory<System.Byte>,System.Threading.CancellationToken)", "codegen", "System.Security.Cryptography.Kmac256", "HashDataAsync", True, "benchmark" },
    { 772, "System.Security.Cryptography/System.Security.Cryptography.KmacXof128::AppendData:System.Void(System.Byte[])", "codegen", "System.Security.Cryptography.KmacXof128", "AppendData", True, "benchmark" },
    { 773, "System.Security.Cryptography/System.Security.Cryptography.KmacXof128::AppendData:System.Void(System.Byte[])", "codegen", "System.Security.Cryptography.KmacXof128", "AppendData", True, "benchmark" },
    { 774, "System.Security.Cryptography/System.Security.Cryptography.KmacXof128::AppendData:System.Void(System.Byte[])", "codegen", "System.Security.Cryptography.KmacXof128", "AppendData", True, "benchmark" },
    { 775, "System.Security.Cryptography/System.Security.Cryptography.KmacXof128::GetHashAndReset:System.Byte[](System.Int32)", "codegen", "System.Security.Cryptography.KmacXof128", "GetHashAndReset", True, "benchmark" },
    { 776, "System.Security.Cryptography/System.Security.Cryptography.KmacXof128::GetHashAndReset:System.Byte[](System.Int32)", "codegen", "System.Security.Cryptography.KmacXof128", "GetHashAndReset", True, "benchmark" },
    { 777, "System.Security.Cryptography/System.Security.Cryptography.KmacXof128::GetCurrentHash:System.Byte[](System.Int32)", "codegen", "System.Security.Cryptography.KmacXof128", "GetCurrentHash", True, "benchmark" },
    { 778, "System.Security.Cryptography/System.Security.Cryptography.KmacXof128::GetCurrentHash:System.Byte[](System.Int32)", "codegen", "System.Security.Cryptography.KmacXof128", "GetCurrentHash", True, "benchmark" },
    { 779, "System.Security.Cryptography/System.Security.Cryptography.KmacXof128::Clone:System.Security.Cryptography.KmacXof128()", "codegen", "System.Security.Cryptography.KmacXof128", "Clone", True, "benchmark" },
    { 780, "System.Security.Cryptography/System.Security.Cryptography.KmacXof128::Dispose:System.Void()", "codegen", "System.Security.Cryptography.KmacXof128", "Dispose", True, "benchmark" },
    { 781, "System.Security.Cryptography/System.Security.Cryptography.KmacXof128::HashData:System.Byte[](System.Byte[],System.Byte[],System.Int32,System.Byte[])", "codegen", "System.Security.Cryptography.KmacXof128", "HashData", True, "benchmark" },
    { 782, "System.Security.Cryptography/System.Security.Cryptography.KmacXof128::HashData:System.Byte[](System.Byte[],System.Byte[],System.Int32,System.Byte[])", "codegen", "System.Security.Cryptography.KmacXof128", "HashData", True, "benchmark" },
    { 783, "System.Security.Cryptography/System.Security.Cryptography.KmacXof128::HashData:System.Byte[](System.Byte[],System.Byte[],System.Int32,System.Byte[])", "codegen", "System.Security.Cryptography.KmacXof128", "HashData", True, "benchmark" },
    { 784, "System.Security.Cryptography/System.Security.Cryptography.KmacXof128::HashData:System.Byte[](System.Byte[],System.Byte[],System.Int32,System.Byte[])", "codegen", "System.Security.Cryptography.KmacXof128", "HashData", True, "benchmark" },
    { 785, "System.Security.Cryptography/System.Security.Cryptography.KmacXof128::HashData:System.Byte[](System.Byte[],System.IO.Stream,System.Int32,System.Byte[])", "codegen", "System.Security.Cryptography.KmacXof128", "HashData", True, "benchmark" },
    { 786, "System.Security.Cryptography/System.Security.Cryptography.KmacXof128::HashData:System.Byte[](System.Byte[],System.IO.Stream,System.Int32,System.Byte[])", "codegen", "System.Security.Cryptography.KmacXof128", "HashData", True, "benchmark" },
    { 787, "System.Security.Cryptography/System.Security.Cryptography.KmacXof128::HashData:System.Byte[](System.Byte[],System.IO.Stream,System.Int32,System.Byte[])", "codegen", "System.Security.Cryptography.KmacXof128", "HashData", True, "benchmark" },
    { 788, "System.Security.Cryptography/System.Security.Cryptography.KmacXof128::HashData:System.Byte[](System.Byte[],System.IO.Stream,System.Int32,System.Byte[])", "codegen", "System.Security.Cryptography.KmacXof128", "HashData", True, "benchmark" },
    { 789, "System.Security.Cryptography/System.Security.Cryptography.KmacXof128::HashDataAsync:System.Threading.Tasks.ValueTask<System.Byte[]>(System.Byte[],System.IO.Stream,System.Int32,System.Byte[],System.Threading.CancellationToken)", "codegen", "System.Security.Cryptography.KmacXof128", "HashDataAsync", True, "benchmark" },
    { 790, "System.Security.Cryptography/System.Security.Cryptography.KmacXof128::HashDataAsync:System.Threading.Tasks.ValueTask<System.Byte[]>(System.Byte[],System.IO.Stream,System.Int32,System.Byte[],System.Threading.CancellationToken)", "codegen", "System.Security.Cryptography.KmacXof128", "HashDataAsync", True, "benchmark" },
    { 791, "System.Security.Cryptography/System.Security.Cryptography.KmacXof128::HashDataAsync:System.Threading.Tasks.ValueTask<System.Byte[]>(System.Byte[],System.IO.Stream,System.Int32,System.Byte[],System.Threading.CancellationToken)", "codegen", "System.Security.Cryptography.KmacXof128", "HashDataAsync", True, "benchmark" },
    { 792, "System.Security.Cryptography/System.Security.Cryptography.KmacXof128::HashDataAsync:System.Threading.Tasks.ValueTask<System.Byte[]>(System.Byte[],System.IO.Stream,System.Int32,System.Byte[],System.Threading.CancellationToken)", "codegen", "System.Security.Cryptography.KmacXof128", "HashDataAsync", True, "benchmark" },
    { 793, "System.Security.Cryptography/System.Security.Cryptography.KmacXof128::HashDataAsync:System.Threading.Tasks.ValueTask<System.Byte[]>(System.ReadOnlyMemory<System.Byte>,System.IO.Stream,System.Int32,System.ReadOnlyMemory<System.Byte>,System.Threading.CancellationToken)", "codegen", "System.Security.Cryptography.KmacXof128", "HashDataAsync", True, "benchmark" },
    { 794, "System.Security.Cryptography/System.Security.Cryptography.KmacXof128::HashDataAsync:System.Threading.Tasks.ValueTask<System.Byte[]>(System.ReadOnlyMemory<System.Byte>,System.IO.Stream,System.Int32,System.ReadOnlyMemory<System.Byte>,System.Threading.CancellationToken)", "codegen", "System.Security.Cryptography.KmacXof128", "HashDataAsync", True, "benchmark" },
    { 795, "System.Security.Cryptography/System.Security.Cryptography.KmacXof128::HashDataAsync:System.Threading.Tasks.ValueTask<System.Byte[]>(System.ReadOnlyMemory<System.Byte>,System.IO.Stream,System.Int32,System.ReadOnlyMemory<System.Byte>,System.Threading.CancellationToken)", "codegen", "System.Security.Cryptography.KmacXof128", "HashDataAsync", True, "benchmark" },
    { 796, "System.Security.Cryptography/System.Security.Cryptography.KmacXof128::HashDataAsync:System.Threading.Tasks.ValueTask(System.ReadOnlyMemory<System.Byte>,System.IO.Stream,System.Memory<System.Byte>,System.ReadOnlyMemory<System.Byte>,System.Threading.CancellationToken)", "codegen", "System.Security.Cryptography.KmacXof128", "HashDataAsync", True, "benchmark" },
    { 797, "System.Security.Cryptography/System.Security.Cryptography.KmacXof128::HashDataAsync:System.Threading.Tasks.ValueTask(System.ReadOnlyMemory<System.Byte>,System.IO.Stream,System.Memory<System.Byte>,System.ReadOnlyMemory<System.Byte>,System.Threading.CancellationToken)", "codegen", "System.Security.Cryptography.KmacXof128", "HashDataAsync", True, "benchmark" },
    { 798, "System.Security.Cryptography/System.Security.Cryptography.KmacXof256::AppendData:System.Void(System.Byte[])", "codegen", "System.Security.Cryptography.KmacXof256", "AppendData", True, "benchmark" },
    { 799, "System.Security.Cryptography/System.Security.Cryptography.KmacXof256::AppendData:System.Void(System.Byte[])", "codegen", "System.Security.Cryptography.KmacXof256", "AppendData", True, "benchmark" },
    { 800, "System.Security.Cryptography/System.Security.Cryptography.KmacXof256::AppendData:System.Void(System.Byte[])", "codegen", "System.Security.Cryptography.KmacXof256", "AppendData", True, "benchmark" },
    { 801, "System.Security.Cryptography/System.Security.Cryptography.KmacXof256::GetHashAndReset:System.Byte[](System.Int32)", "codegen", "System.Security.Cryptography.KmacXof256", "GetHashAndReset", True, "benchmark" },
    { 802, "System.Security.Cryptography/System.Security.Cryptography.KmacXof256::GetHashAndReset:System.Byte[](System.Int32)", "codegen", "System.Security.Cryptography.KmacXof256", "GetHashAndReset", True, "benchmark" },
    { 803, "System.Security.Cryptography/System.Security.Cryptography.KmacXof256::GetCurrentHash:System.Byte[](System.Int32)", "codegen", "System.Security.Cryptography.KmacXof256", "GetCurrentHash", True, "benchmark" },
    { 804, "System.Security.Cryptography/System.Security.Cryptography.KmacXof256::GetCurrentHash:System.Byte[](System.Int32)", "codegen", "System.Security.Cryptography.KmacXof256", "GetCurrentHash", True, "benchmark" },
    { 805, "System.Security.Cryptography/System.Security.Cryptography.KmacXof256::Clone:System.Security.Cryptography.KmacXof256()", "codegen", "System.Security.Cryptography.KmacXof256", "Clone", True, "benchmark" },
    { 806, "System.Security.Cryptography/System.Security.Cryptography.KmacXof256::Dispose:System.Void()", "codegen", "System.Security.Cryptography.KmacXof256", "Dispose", True, "benchmark" },
    { 807, "System.Security.Cryptography/System.Security.Cryptography.KmacXof256::HashData:System.Byte[](System.Byte[],System.Byte[],System.Int32,System.Byte[])", "codegen", "System.Security.Cryptography.KmacXof256", "HashData", True, "benchmark" },
    { 808, "System.Security.Cryptography/System.Security.Cryptography.KmacXof256::HashData:System.Byte[](System.Byte[],System.Byte[],System.Int32,System.Byte[])", "codegen", "System.Security.Cryptography.KmacXof256", "HashData", True, "benchmark" },
    { 809, "System.Security.Cryptography/System.Security.Cryptography.KmacXof256::HashData:System.Byte[](System.Byte[],System.Byte[],System.Int32,System.Byte[])", "codegen", "System.Security.Cryptography.KmacXof256", "HashData", True, "benchmark" },
    { 810, "System.Security.Cryptography/System.Security.Cryptography.KmacXof256::HashData:System.Byte[](System.Byte[],System.Byte[],System.Int32,System.Byte[])", "codegen", "System.Security.Cryptography.KmacXof256", "HashData", True, "benchmark" },
    { 811, "System.Security.Cryptography/System.Security.Cryptography.KmacXof256::HashData:System.Byte[](System.Byte[],System.IO.Stream,System.Int32,System.Byte[])", "codegen", "System.Security.Cryptography.KmacXof256", "HashData", True, "benchmark" },
    { 812, "System.Security.Cryptography/System.Security.Cryptography.KmacXof256::HashData:System.Byte[](System.Byte[],System.IO.Stream,System.Int32,System.Byte[])", "codegen", "System.Security.Cryptography.KmacXof256", "HashData", True, "benchmark" },
    { 813, "System.Security.Cryptography/System.Security.Cryptography.KmacXof256::HashData:System.Byte[](System.Byte[],System.IO.Stream,System.Int32,System.Byte[])", "codegen", "System.Security.Cryptography.KmacXof256", "HashData", True, "benchmark" },
    { 814, "System.Security.Cryptography/System.Security.Cryptography.KmacXof256::HashData:System.Byte[](System.Byte[],System.IO.Stream,System.Int32,System.Byte[])", "codegen", "System.Security.Cryptography.KmacXof256", "HashData", True, "benchmark" },
    { 815, "System.Security.Cryptography/System.Security.Cryptography.KmacXof256::HashDataAsync:System.Threading.Tasks.ValueTask<System.Byte[]>(System.Byte[],System.IO.Stream,System.Int32,System.Byte[],System.Threading.CancellationToken)", "codegen", "System.Security.Cryptography.KmacXof256", "HashDataAsync", True, "benchmark" },
    { 816, "System.Security.Cryptography/System.Security.Cryptography.KmacXof256::HashDataAsync:System.Threading.Tasks.ValueTask<System.Byte[]>(System.Byte[],System.IO.Stream,System.Int32,System.Byte[],System.Threading.CancellationToken)", "codegen", "System.Security.Cryptography.KmacXof256", "HashDataAsync", True, "benchmark" },
    { 817, "System.Security.Cryptography/System.Security.Cryptography.KmacXof256::HashDataAsync:System.Threading.Tasks.ValueTask<System.Byte[]>(System.Byte[],System.IO.Stream,System.Int32,System.Byte[],System.Threading.CancellationToken)", "codegen", "System.Security.Cryptography.KmacXof256", "HashDataAsync", True, "benchmark" },
    { 818, "System.Security.Cryptography/System.Security.Cryptography.KmacXof256::HashDataAsync:System.Threading.Tasks.ValueTask<System.Byte[]>(System.Byte[],System.IO.Stream,System.Int32,System.Byte[],System.Threading.CancellationToken)", "codegen", "System.Security.Cryptography.KmacXof256", "HashDataAsync", True, "benchmark" },
    { 819, "System.Security.Cryptography/System.Security.Cryptography.KmacXof256::HashDataAsync:System.Threading.Tasks.ValueTask<System.Byte[]>(System.ReadOnlyMemory<System.Byte>,System.IO.Stream,System.Int32,System.ReadOnlyMemory<System.Byte>,System.Threading.CancellationToken)", "codegen", "System.Security.Cryptography.KmacXof256", "HashDataAsync", True, "benchmark" },
    { 820, "System.Security.Cryptography/System.Security.Cryptography.KmacXof256::HashDataAsync:System.Threading.Tasks.ValueTask<System.Byte[]>(System.ReadOnlyMemory<System.Byte>,System.IO.Stream,System.Int32,System.ReadOnlyMemory<System.Byte>,System.Threading.CancellationToken)", "codegen", "System.Security.Cryptography.KmacXof256", "HashDataAsync", True, "benchmark" },
    { 821, "System.Security.Cryptography/System.Security.Cryptography.KmacXof256::HashDataAsync:System.Threading.Tasks.ValueTask<System.Byte[]>(System.ReadOnlyMemory<System.Byte>,System.IO.Stream,System.Int32,System.ReadOnlyMemory<System.Byte>,System.Threading.CancellationToken)", "codegen", "System.Security.Cryptography.KmacXof256", "HashDataAsync", True, "benchmark" },
    { 822, "System.Security.Cryptography/System.Security.Cryptography.KmacXof256::HashDataAsync:System.Threading.Tasks.ValueTask(System.ReadOnlyMemory<System.Byte>,System.IO.Stream,System.Memory<System.Byte>,System.ReadOnlyMemory<System.Byte>,System.Threading.CancellationToken)", "codegen", "System.Security.Cryptography.KmacXof256", "HashDataAsync", True, "benchmark" },
    { 823, "System.Security.Cryptography/System.Security.Cryptography.KmacXof256::HashDataAsync:System.Threading.Tasks.ValueTask(System.ReadOnlyMemory<System.Byte>,System.IO.Stream,System.Memory<System.Byte>,System.ReadOnlyMemory<System.Byte>,System.Threading.CancellationToken)", "codegen", "System.Security.Cryptography.KmacXof256", "HashDataAsync", True, "benchmark" },
    { 824, "System.Security.Cryptography/System.Security.Cryptography.MaskGenerationMethod::GenerateMask:System.Byte[](System.Byte[],System.Int32)", "codegen", "System.Security.Cryptography.MaskGenerationMethod", "GenerateMask", True, "benchmark" },
    { 825, "System.Security.Cryptography/System.Security.Cryptography.MaskGenerationMethod::GenerateMask:System.Byte[](System.Byte[],System.Int32)", "codegen", "System.Security.Cryptography.MaskGenerationMethod", "GenerateMask", True, "benchmark" },
    { 826, "System.Security.Cryptography/System.Security.Cryptography.MaskGenerationMethod::GenerateMask:System.Byte[](System.Byte[],System.Int32)", "codegen", "System.Security.Cryptography.MaskGenerationMethod", "GenerateMask", True, "benchmark" },
    { 827, "System.Security.Cryptography/System.Security.Cryptography.MaskGenerationMethod::GenerateMask:System.Byte[](System.Byte[],System.Int32)", "codegen", "System.Security.Cryptography.MaskGenerationMethod", "GenerateMask", True, "benchmark" },
    { 828, "System.Security.Cryptography/System.Security.Cryptography.MD5::HashData:System.Byte[](System.Byte[])", "codegen", "System.Security.Cryptography.MD5", "HashData", True, "benchmark" },
    { 829, "System.Security.Cryptography/System.Security.Cryptography.MD5::HashData:System.Byte[](System.Byte[])", "codegen", "System.Security.Cryptography.MD5", "HashData", True, "benchmark" },
    { 830, "System.Security.Cryptography/System.Security.Cryptography.MD5::HashData:System.Byte[](System.Byte[])", "codegen", "System.Security.Cryptography.MD5", "HashData", True, "benchmark" },
    { 831, "System.Security.Cryptography/System.Security.Cryptography.MD5::HashData:System.Byte[](System.IO.Stream)", "codegen", "System.Security.Cryptography.MD5", "HashData", True, "benchmark" },
    { 832, "System.Security.Cryptography/System.Security.Cryptography.MD5::HashData:System.Byte[](System.IO.Stream)", "codegen", "System.Security.Cryptography.MD5", "HashData", True, "benchmark" },
    { 833, "System.Security.Cryptography/System.Security.Cryptography.MD5::HashDataAsync:System.Threading.Tasks.ValueTask<System.Byte[]>(System.IO.Stream,System.Threading.CancellationToken)", "codegen", "System.Security.Cryptography.MD5", "HashDataAsync", True, "benchmark" },
    { 834, "System.Security.Cryptography/System.Security.Cryptography.MD5::HashDataAsync:System.Threading.Tasks.ValueTask<System.Byte[]>(System.IO.Stream,System.Threading.CancellationToken)", "codegen", "System.Security.Cryptography.MD5", "HashDataAsync", True, "benchmark" },
    { 835, "System.Security.Cryptography/System.Security.Cryptography.MD5::HashDataAsync:System.Threading.Tasks.ValueTask<System.Int32>(System.IO.Stream,System.Memory<System.Byte>,System.Threading.CancellationToken)", "codegen", "System.Security.Cryptography.MD5", "HashDataAsync", True, "benchmark" },
    { 836, "System.Security.Cryptography/System.Security.Cryptography.MD5::HashDataAsync:System.Threading.Tasks.ValueTask<System.Int32>(System.IO.Stream,System.Memory<System.Byte>,System.Threading.CancellationToken)", "codegen", "System.Security.Cryptography.MD5", "HashDataAsync", True, "benchmark" },
    { 837, "System.Security.Cryptography/System.Security.Cryptography.MD5CryptoServiceProvider::Initialize:System.Void()", "codegen", "System.Security.Cryptography.MD5CryptoServiceProvider", "Initialize", True, "benchmark" },
    { 838, "System.Security.Cryptography/System.Security.Cryptography.Oid::FromFriendlyName:System.Security.Cryptography.Oid(System.String,System.Security.Cryptography.OidGroup)", "codegen", "System.Security.Cryptography.Oid", "FromFriendlyName", True, "benchmark" },
    { 839, "System.Security.Cryptography/System.Security.Cryptography.Oid::FromFriendlyName:System.Security.Cryptography.Oid(System.String,System.Security.Cryptography.OidGroup)", "codegen", "System.Security.Cryptography.Oid", "FromFriendlyName", True, "benchmark" },
    { 840, "System.Security.Cryptography/System.Security.Cryptography.Oid::FromOidValue:System.Security.Cryptography.Oid(System.String,System.Security.Cryptography.OidGroup)", "codegen", "System.Security.Cryptography.Oid", "FromOidValue", True, "benchmark" },
    { 841, "System.Security.Cryptography/System.Security.Cryptography.Oid::FromOidValue:System.Security.Cryptography.Oid(System.String,System.Security.Cryptography.OidGroup)", "codegen", "System.Security.Cryptography.Oid", "FromOidValue", True, "benchmark" },
    { 842, "System.Security.Cryptography/System.Security.Cryptography.OidCollection::Add:System.Int32(System.Security.Cryptography.Oid)", "codegen", "System.Security.Cryptography.OidCollection", "Add", True, "benchmark" },
    { 843, "System.Security.Cryptography/System.Security.Cryptography.OidCollection::GetEnumerator:System.Security.Cryptography.OidEnumerator()", "codegen", "System.Security.Cryptography.OidCollection", "GetEnumerator", True, "benchmark" },
    { 844, "System.Security.Cryptography/System.Security.Cryptography.PasswordDeriveBytes::GetBytes:System.Byte[](System.Int32)", "codegen", "System.Security.Cryptography.PasswordDeriveBytes", "GetBytes", True, "benchmark" },
    { 845, "System.Security.Cryptography/System.Security.Cryptography.PasswordDeriveBytes::GetBytes:System.Byte[](System.Int32)", "codegen", "System.Security.Cryptography.PasswordDeriveBytes", "GetBytes", True, "benchmark" },
    { 846, "System.Security.Cryptography/System.Security.Cryptography.PasswordDeriveBytes::CryptDeriveKey:System.Byte[](System.String,System.String,System.Int32,System.Byte[])", "codegen", "System.Security.Cryptography.PasswordDeriveBytes", "CryptDeriveKey", True, "benchmark" },
    { 847, "System.Security.Cryptography/System.Security.Cryptography.PasswordDeriveBytes::CryptDeriveKey:System.Byte[](System.String,System.String,System.Int32,System.Byte[])", "codegen", "System.Security.Cryptography.PasswordDeriveBytes", "CryptDeriveKey", True, "benchmark" },
    { 848, "System.Security.Cryptography/System.Security.Cryptography.PasswordDeriveBytes::CryptDeriveKey:System.Byte[](System.String,System.String,System.Int32,System.Byte[])", "codegen", "System.Security.Cryptography.PasswordDeriveBytes", "CryptDeriveKey", True, "benchmark" },
    { 849, "System.Security.Cryptography/System.Security.Cryptography.PasswordDeriveBytes::CryptDeriveKey:System.Byte[](System.String,System.String,System.Int32,System.Byte[])", "codegen", "System.Security.Cryptography.PasswordDeriveBytes", "CryptDeriveKey", True, "benchmark" },
    { 850, "System.Security.Cryptography/System.Security.Cryptography.PasswordDeriveBytes::CryptDeriveKey:System.Byte[](System.String,System.String,System.Int32,System.Byte[])", "codegen", "System.Security.Cryptography.PasswordDeriveBytes", "CryptDeriveKey", True, "benchmark" },
    { 851, "System.Security.Cryptography/System.Security.Cryptography.PasswordDeriveBytes::CryptDeriveKey:System.Byte[](System.String,System.String,System.Int32,System.Byte[])", "codegen", "System.Security.Cryptography.PasswordDeriveBytes", "CryptDeriveKey", True, "benchmark" },
    { 852, "System.Security.Cryptography/System.Security.Cryptography.PemEncoding::GetEncodedSize:System.Int32(System.Int32,System.Int32)", "codegen", "System.Security.Cryptography.PemEncoding", "GetEncodedSize", True, "benchmark" },
    { 853, "System.Security.Cryptography/System.Security.Cryptography.PemEncoding::GetEncodedSize:System.Int32(System.Int32,System.Int32)", "codegen", "System.Security.Cryptography.PemEncoding", "GetEncodedSize", True, "benchmark" },
    { 854, "System.Security.Cryptography/System.Security.Cryptography.PemEncoding::GetEncodedSize:System.Int32(System.Int32,System.Int32)", "codegen", "System.Security.Cryptography.PemEncoding", "GetEncodedSize", True, "benchmark" },
    { 855, "System.Security.Cryptography/System.Security.Cryptography.PKCS1MaskGenerationMethod::GenerateMask:System.Byte[](System.Byte[],System.Int32)", "codegen", "System.Security.Cryptography.PKCS1MaskGenerationMethod", "GenerateMask", True, "benchmark" },
    { 856, "System.Security.Cryptography/System.Security.Cryptography.PKCS1MaskGenerationMethod::GenerateMask:System.Byte[](System.Byte[],System.Int32)", "codegen", "System.Security.Cryptography.PKCS1MaskGenerationMethod", "GenerateMask", True, "benchmark" },
    { 857, "System.Security.Cryptography/System.Security.Cryptography.PKCS1MaskGenerationMethod::GenerateMask:System.Byte[](System.Byte[],System.Int32)", "codegen", "System.Security.Cryptography.PKCS1MaskGenerationMethod", "GenerateMask", True, "benchmark" },
    { 858, "System.Security.Cryptography/System.Security.Cryptography.PKCS1MaskGenerationMethod::GenerateMask:System.Byte[](System.Byte[],System.Int32)", "codegen", "System.Security.Cryptography.PKCS1MaskGenerationMethod", "GenerateMask", True, "benchmark" },
    { 859, "System.Security.Cryptography/System.Security.Cryptography.RandomNumberGenerator::Dispose:System.Void()", "codegen", "System.Security.Cryptography.RandomNumberGenerator", "Dispose", True, "benchmark" },
    { 860, "System.Security.Cryptography/System.Security.Cryptography.RandomNumberGenerator::GetBytes:System.Void(System.Byte[])", "codegen", "System.Security.Cryptography.RandomNumberGenerator", "GetBytes", True, "benchmark" },
    { 861, "System.Security.Cryptography/System.Security.Cryptography.RandomNumberGenerator::GetBytes:System.Void(System.Byte[])", "codegen", "System.Security.Cryptography.RandomNumberGenerator", "GetBytes", True, "benchmark" },
    { 862, "System.Security.Cryptography/System.Security.Cryptography.RandomNumberGenerator::GetBytes:System.Void(System.Byte[])", "codegen", "System.Security.Cryptography.RandomNumberGenerator", "GetBytes", True, "benchmark" },
    { 863, "System.Security.Cryptography/System.Security.Cryptography.RandomNumberGenerator::GetBytes:System.Void(System.Byte[],System.Int32,System.Int32)", "codegen", "System.Security.Cryptography.RandomNumberGenerator", "GetBytes", True, "benchmark" },
    { 864, "System.Security.Cryptography/System.Security.Cryptography.RandomNumberGenerator::GetBytes:System.Void(System.Byte[],System.Int32,System.Int32)", "codegen", "System.Security.Cryptography.RandomNumberGenerator", "GetBytes", True, "benchmark" },
    { 865, "System.Security.Cryptography/System.Security.Cryptography.RandomNumberGenerator::GetBytes:System.Void(System.Byte[],System.Int32,System.Int32)", "codegen", "System.Security.Cryptography.RandomNumberGenerator", "GetBytes", True, "benchmark" },
    { 866, "System.Security.Cryptography/System.Security.Cryptography.RandomNumberGenerator::GetBytes:System.Void(System.Byte[],System.Int32,System.Int32)", "codegen", "System.Security.Cryptography.RandomNumberGenerator", "GetBytes", True, "benchmark" },
    { 867, "System.Security.Cryptography/System.Security.Cryptography.RandomNumberGenerator::GetBytes:System.Void(System.Byte[],System.Int32,System.Int32)", "codegen", "System.Security.Cryptography.RandomNumberGenerator", "GetBytes", True, "benchmark" },
    { 868, "System.Security.Cryptography/System.Security.Cryptography.RandomNumberGenerator::GetNonZeroBytes:System.Void(System.Byte[])", "codegen", "System.Security.Cryptography.RandomNumberGenerator", "GetNonZeroBytes", True, "benchmark" },
    { 869, "System.Security.Cryptography/System.Security.Cryptography.RandomNumberGenerator::GetNonZeroBytes:System.Void(System.Byte[])", "codegen", "System.Security.Cryptography.RandomNumberGenerator", "GetNonZeroBytes", True, "benchmark" },
    { 870, "System.Security.Cryptography/System.Security.Cryptography.RandomNumberGenerator::GetNonZeroBytes:System.Void(System.Byte[])", "codegen", "System.Security.Cryptography.RandomNumberGenerator", "GetNonZeroBytes", True, "benchmark" },
    { 871, "System.Security.Cryptography/System.Security.Cryptography.RandomNumberGenerator::GetInt32:System.Int32(System.Int32,System.Int32)", "codegen", "System.Security.Cryptography.RandomNumberGenerator", "GetInt32", True, "benchmark" },
    { 872, "System.Security.Cryptography/System.Security.Cryptography.RandomNumberGenerator::GetInt32:System.Int32(System.Int32,System.Int32)", "codegen", "System.Security.Cryptography.RandomNumberGenerator", "GetInt32", True, "benchmark" },
    { 873, "System.Security.Cryptography/System.Security.Cryptography.RandomNumberGenerator::GetInt32:System.Int32(System.Int32,System.Int32)", "codegen", "System.Security.Cryptography.RandomNumberGenerator", "GetInt32", True, "benchmark" },
    { 874, "System.Security.Cryptography/System.Security.Cryptography.RandomNumberGenerator::GetInt32:System.Int32(System.Int32)", "codegen", "System.Security.Cryptography.RandomNumberGenerator", "GetInt32", True, "benchmark" },
    { 875, "System.Security.Cryptography/System.Security.Cryptography.RandomNumberGenerator::GetInt32:System.Int32(System.Int32)", "codegen", "System.Security.Cryptography.RandomNumberGenerator", "GetInt32", True, "benchmark" },
    { 876, "System.Security.Cryptography/System.Security.Cryptography.RandomNumberGenerator::GetBytes:System.Byte[](System.Int32)", "codegen", "System.Security.Cryptography.RandomNumberGenerator", "GetBytes", True, "benchmark" },
    { 877, "System.Security.Cryptography/System.Security.Cryptography.RandomNumberGenerator::GetBytes:System.Byte[](System.Int32)", "codegen", "System.Security.Cryptography.RandomNumberGenerator", "GetBytes", True, "benchmark" },
    { 878, "System.Security.Cryptography/System.Security.Cryptography.RandomNumberGenerator::GetHexString:System.String(System.Int32,System.Boolean)", "codegen", "System.Security.Cryptography.RandomNumberGenerator", "GetHexString", True, "benchmark" },
    { 879, "System.Security.Cryptography/System.Security.Cryptography.RandomNumberGenerator::GetHexString:System.String(System.Int32,System.Boolean)", "codegen", "System.Security.Cryptography.RandomNumberGenerator", "GetHexString", True, "benchmark" },
    { 880, "System.Security.Cryptography/System.Security.Cryptography.RandomNumberGenerator::GetHexString:System.String(System.Int32,System.Boolean)", "codegen", "System.Security.Cryptography.RandomNumberGenerator", "GetHexString", True, "benchmark" },
    { 881, "System.Security.Cryptography/System.Security.Cryptography.RC2CryptoServiceProvider::CreateEncryptor:System.Security.Cryptography.ICryptoTransform(System.Byte[],System.Byte[])", "codegen", "System.Security.Cryptography.RC2CryptoServiceProvider", "CreateEncryptor", True, "benchmark" },
    { 882, "System.Security.Cryptography/System.Security.Cryptography.RC2CryptoServiceProvider::CreateEncryptor:System.Security.Cryptography.ICryptoTransform(System.Byte[],System.Byte[])", "codegen", "System.Security.Cryptography.RC2CryptoServiceProvider", "CreateEncryptor", True, "benchmark" },
    { 883, "System.Security.Cryptography/System.Security.Cryptography.RC2CryptoServiceProvider::CreateEncryptor:System.Security.Cryptography.ICryptoTransform(System.Byte[],System.Byte[])", "codegen", "System.Security.Cryptography.RC2CryptoServiceProvider", "CreateEncryptor", True, "benchmark" },
    { 884, "System.Security.Cryptography/System.Security.Cryptography.RC2CryptoServiceProvider::CreateDecryptor:System.Security.Cryptography.ICryptoTransform(System.Byte[],System.Byte[])", "codegen", "System.Security.Cryptography.RC2CryptoServiceProvider", "CreateDecryptor", True, "benchmark" },
    { 885, "System.Security.Cryptography/System.Security.Cryptography.RC2CryptoServiceProvider::CreateDecryptor:System.Security.Cryptography.ICryptoTransform(System.Byte[],System.Byte[])", "codegen", "System.Security.Cryptography.RC2CryptoServiceProvider", "CreateDecryptor", True, "benchmark" },
    { 886, "System.Security.Cryptography/System.Security.Cryptography.RC2CryptoServiceProvider::CreateDecryptor:System.Security.Cryptography.ICryptoTransform(System.Byte[],System.Byte[])", "codegen", "System.Security.Cryptography.RC2CryptoServiceProvider", "CreateDecryptor", True, "benchmark" },
    { 887, "System.Security.Cryptography/System.Security.Cryptography.RC2CryptoServiceProvider::GenerateKey:System.Void()", "codegen", "System.Security.Cryptography.RC2CryptoServiceProvider", "GenerateKey", True, "benchmark" },
    { 888, "System.Security.Cryptography/System.Security.Cryptography.RC2CryptoServiceProvider::GenerateIV:System.Void()", "codegen", "System.Security.Cryptography.RC2CryptoServiceProvider", "GenerateIV", True, "benchmark" },
    { 889, "System.Security.Cryptography/System.Security.Cryptography.Rfc2898DeriveBytes::GetBytes:System.Byte[](System.Int32)", "codegen", "System.Security.Cryptography.Rfc2898DeriveBytes", "GetBytes", True, "benchmark" },
    { 890, "System.Security.Cryptography/System.Security.Cryptography.Rfc2898DeriveBytes::GetBytes:System.Byte[](System.Int32)", "codegen", "System.Security.Cryptography.Rfc2898DeriveBytes", "GetBytes", True, "benchmark" },
    { 891, "System.Security.Cryptography/System.Security.Cryptography.Rfc2898DeriveBytes::CryptDeriveKey:System.Byte[](System.String,System.String,System.Int32,System.Byte[])", "codegen", "System.Security.Cryptography.Rfc2898DeriveBytes", "CryptDeriveKey", True, "benchmark" },
    { 892, "System.Security.Cryptography/System.Security.Cryptography.Rfc2898DeriveBytes::CryptDeriveKey:System.Byte[](System.String,System.String,System.Int32,System.Byte[])", "codegen", "System.Security.Cryptography.Rfc2898DeriveBytes", "CryptDeriveKey", True, "benchmark" },
    { 893, "System.Security.Cryptography/System.Security.Cryptography.Rfc2898DeriveBytes::CryptDeriveKey:System.Byte[](System.String,System.String,System.Int32,System.Byte[])", "codegen", "System.Security.Cryptography.Rfc2898DeriveBytes", "CryptDeriveKey", True, "benchmark" },
    { 894, "System.Security.Cryptography/System.Security.Cryptography.Rfc2898DeriveBytes::CryptDeriveKey:System.Byte[](System.String,System.String,System.Int32,System.Byte[])", "codegen", "System.Security.Cryptography.Rfc2898DeriveBytes", "CryptDeriveKey", True, "benchmark" },
    { 895, "System.Security.Cryptography/System.Security.Cryptography.Rfc2898DeriveBytes::CryptDeriveKey:System.Byte[](System.String,System.String,System.Int32,System.Byte[])", "codegen", "System.Security.Cryptography.Rfc2898DeriveBytes", "CryptDeriveKey", True, "benchmark" },
    { 896, "System.Security.Cryptography/System.Security.Cryptography.Rfc2898DeriveBytes::CryptDeriveKey:System.Byte[](System.String,System.String,System.Int32,System.Byte[])", "codegen", "System.Security.Cryptography.Rfc2898DeriveBytes", "CryptDeriveKey", True, "benchmark" },
    { 897, "System.Security.Cryptography/System.Security.Cryptography.Rfc2898DeriveBytes::Pbkdf2:System.Byte[](System.Byte[],System.Byte[],System.Int32,System.Security.Cryptography.HashAlgorithmName,System.Int32)", "codegen", "System.Security.Cryptography.Rfc2898DeriveBytes", "Pbkdf2", True, "benchmark" },
    { 898, "System.Security.Cryptography/System.Security.Cryptography.Rfc2898DeriveBytes::Pbkdf2:System.Byte[](System.Byte[],System.Byte[],System.Int32,System.Security.Cryptography.HashAlgorithmName,System.Int32)", "codegen", "System.Security.Cryptography.Rfc2898DeriveBytes", "Pbkdf2", True, "benchmark" },
    { 899, "System.Security.Cryptography/System.Security.Cryptography.Rfc2898DeriveBytes::Pbkdf2:System.Byte[](System.Byte[],System.Byte[],System.Int32,System.Security.Cryptography.HashAlgorithmName,System.Int32)", "codegen", "System.Security.Cryptography.Rfc2898DeriveBytes", "Pbkdf2", True, "benchmark" },
    { 900, "System.Security.Cryptography/System.Security.Cryptography.Rfc2898DeriveBytes::Pbkdf2:System.Byte[](System.Byte[],System.Byte[],System.Int32,System.Security.Cryptography.HashAlgorithmName,System.Int32)", "codegen", "System.Security.Cryptography.Rfc2898DeriveBytes", "Pbkdf2", True, "benchmark" },
    { 901, "System.Security.Cryptography/System.Security.Cryptography.Rfc2898DeriveBytes::Pbkdf2:System.Byte[](System.String,System.Byte[],System.Int32,System.Security.Cryptography.HashAlgorithmName,System.Int32)", "codegen", "System.Security.Cryptography.Rfc2898DeriveBytes", "Pbkdf2", True, "benchmark" },
    { 902, "System.Security.Cryptography/System.Security.Cryptography.Rfc2898DeriveBytes::Pbkdf2:System.Byte[](System.String,System.Byte[],System.Int32,System.Security.Cryptography.HashAlgorithmName,System.Int32)", "codegen", "System.Security.Cryptography.Rfc2898DeriveBytes", "Pbkdf2", True, "benchmark" },
    { 903, "System.Security.Cryptography/System.Security.Cryptography.Rfc2898DeriveBytes::Pbkdf2:System.Byte[](System.String,System.Byte[],System.Int32,System.Security.Cryptography.HashAlgorithmName,System.Int32)", "codegen", "System.Security.Cryptography.Rfc2898DeriveBytes", "Pbkdf2", True, "benchmark" },
    { 904, "System.Security.Cryptography/System.Security.Cryptography.Rfc2898DeriveBytes::Pbkdf2:System.Byte[](System.String,System.Byte[],System.Int32,System.Security.Cryptography.HashAlgorithmName,System.Int32)", "codegen", "System.Security.Cryptography.Rfc2898DeriveBytes", "Pbkdf2", True, "benchmark" },
    { 905, "System.Security.Cryptography/System.Security.Cryptography.Rfc2898DeriveBytes::Pbkdf2:System.Byte[](System.String,System.Byte[],System.Int32,System.Security.Cryptography.HashAlgorithmName,System.Int32)", "codegen", "System.Security.Cryptography.Rfc2898DeriveBytes", "Pbkdf2", True, "benchmark" },
    { 906, "System.Security.Cryptography/System.Security.Cryptography.RijndaelManaged::CreateEncryptor:System.Security.Cryptography.ICryptoTransform()", "codegen", "System.Security.Cryptography.RijndaelManaged", "CreateEncryptor", True, "benchmark" },
    { 907, "System.Security.Cryptography/System.Security.Cryptography.RijndaelManaged::CreateEncryptor:System.Security.Cryptography.ICryptoTransform(System.Byte[],System.Byte[])", "codegen", "System.Security.Cryptography.RijndaelManaged", "CreateEncryptor", True, "benchmark" },
    { 908, "System.Security.Cryptography/System.Security.Cryptography.RijndaelManaged::CreateEncryptor:System.Security.Cryptography.ICryptoTransform(System.Byte[],System.Byte[])", "codegen", "System.Security.Cryptography.RijndaelManaged", "CreateEncryptor", True, "benchmark" },
    { 909, "System.Security.Cryptography/System.Security.Cryptography.RijndaelManaged::CreateEncryptor:System.Security.Cryptography.ICryptoTransform(System.Byte[],System.Byte[])", "codegen", "System.Security.Cryptography.RijndaelManaged", "CreateEncryptor", True, "benchmark" },
    { 910, "System.Security.Cryptography/System.Security.Cryptography.RijndaelManaged::CreateDecryptor:System.Security.Cryptography.ICryptoTransform()", "codegen", "System.Security.Cryptography.RijndaelManaged", "CreateDecryptor", True, "benchmark" },
    { 911, "System.Security.Cryptography/System.Security.Cryptography.RijndaelManaged::CreateDecryptor:System.Security.Cryptography.ICryptoTransform(System.Byte[],System.Byte[])", "codegen", "System.Security.Cryptography.RijndaelManaged", "CreateDecryptor", True, "benchmark" },
    { 912, "System.Security.Cryptography/System.Security.Cryptography.RijndaelManaged::CreateDecryptor:System.Security.Cryptography.ICryptoTransform(System.Byte[],System.Byte[])", "codegen", "System.Security.Cryptography.RijndaelManaged", "CreateDecryptor", True, "benchmark" },
    { 913, "System.Security.Cryptography/System.Security.Cryptography.RijndaelManaged::CreateDecryptor:System.Security.Cryptography.ICryptoTransform(System.Byte[],System.Byte[])", "codegen", "System.Security.Cryptography.RijndaelManaged", "CreateDecryptor", True, "benchmark" },
    { 914, "System.Security.Cryptography/System.Security.Cryptography.RijndaelManaged::GenerateIV:System.Void()", "codegen", "System.Security.Cryptography.RijndaelManaged", "GenerateIV", True, "benchmark" },
    { 915, "System.Security.Cryptography/System.Security.Cryptography.RijndaelManaged::GenerateKey:System.Void()", "codegen", "System.Security.Cryptography.RijndaelManaged", "GenerateKey", True, "benchmark" },
    { 916, "System.Security.Cryptography/System.Security.Cryptography.RNGCryptoServiceProvider::GetBytes:System.Void(System.Byte[])", "codegen", "System.Security.Cryptography.RNGCryptoServiceProvider", "GetBytes", True, "benchmark" },
    { 917, "System.Security.Cryptography/System.Security.Cryptography.RNGCryptoServiceProvider::GetBytes:System.Void(System.Byte[])", "codegen", "System.Security.Cryptography.RNGCryptoServiceProvider", "GetBytes", True, "benchmark" },
    { 918, "System.Security.Cryptography/System.Security.Cryptography.RNGCryptoServiceProvider::GetBytes:System.Void(System.Byte[])", "codegen", "System.Security.Cryptography.RNGCryptoServiceProvider", "GetBytes", True, "benchmark" },
    { 919, "System.Security.Cryptography/System.Security.Cryptography.RNGCryptoServiceProvider::GetBytes:System.Void(System.Byte[],System.Int32,System.Int32)", "codegen", "System.Security.Cryptography.RNGCryptoServiceProvider", "GetBytes", True, "benchmark" },
    { 920, "System.Security.Cryptography/System.Security.Cryptography.RNGCryptoServiceProvider::GetBytes:System.Void(System.Byte[],System.Int32,System.Int32)", "codegen", "System.Security.Cryptography.RNGCryptoServiceProvider", "GetBytes", True, "benchmark" },
    { 921, "System.Security.Cryptography/System.Security.Cryptography.RNGCryptoServiceProvider::GetBytes:System.Void(System.Byte[],System.Int32,System.Int32)", "codegen", "System.Security.Cryptography.RNGCryptoServiceProvider", "GetBytes", True, "benchmark" },
    { 922, "System.Security.Cryptography/System.Security.Cryptography.RNGCryptoServiceProvider::GetBytes:System.Void(System.Byte[],System.Int32,System.Int32)", "codegen", "System.Security.Cryptography.RNGCryptoServiceProvider", "GetBytes", True, "benchmark" },
    { 923, "System.Security.Cryptography/System.Security.Cryptography.RNGCryptoServiceProvider::GetBytes:System.Void(System.Byte[],System.Int32,System.Int32)", "codegen", "System.Security.Cryptography.RNGCryptoServiceProvider", "GetBytes", True, "benchmark" },
    { 924, "System.Security.Cryptography/System.Security.Cryptography.RNGCryptoServiceProvider::GetNonZeroBytes:System.Void(System.Byte[])", "codegen", "System.Security.Cryptography.RNGCryptoServiceProvider", "GetNonZeroBytes", True, "benchmark" },
    { 925, "System.Security.Cryptography/System.Security.Cryptography.RNGCryptoServiceProvider::GetNonZeroBytes:System.Void(System.Byte[])", "codegen", "System.Security.Cryptography.RNGCryptoServiceProvider", "GetNonZeroBytes", True, "benchmark" },
    { 926, "System.Security.Cryptography/System.Security.Cryptography.RNGCryptoServiceProvider::GetNonZeroBytes:System.Void(System.Byte[])", "codegen", "System.Security.Cryptography.RNGCryptoServiceProvider", "GetNonZeroBytes", True, "benchmark" },
    { 927, "System.Security.Cryptography/System.Security.Cryptography.RSA::GetMaxOutputSize:System.Int32()", "codegen", "System.Security.Cryptography.RSA", "GetMaxOutputSize", True, "benchmark" },
    { 928, "System.Security.Cryptography/System.Security.Cryptography.RSA::ExportParameters:System.Security.Cryptography.RSAParameters(System.Boolean)", "codegen", "System.Security.Cryptography.RSA", "ExportParameters", True, "benchmark" },
    { 929, "System.Security.Cryptography/System.Security.Cryptography.RSA::ExportParameters:System.Security.Cryptography.RSAParameters(System.Boolean)", "codegen", "System.Security.Cryptography.RSA", "ExportParameters", True, "benchmark" },
    { 930, "System.Security.Cryptography/System.Security.Cryptography.RSA::ImportParameters:System.Void(System.Security.Cryptography.RSAParameters)", "codegen", "System.Security.Cryptography.RSA", "ImportParameters", True, "benchmark" },
    { 931, "System.Security.Cryptography/System.Security.Cryptography.RSA::Encrypt:System.Byte[](System.Byte[],System.Security.Cryptography.RSAEncryptionPadding)", "codegen", "System.Security.Cryptography.RSA", "Encrypt", True, "benchmark" },
    { 932, "System.Security.Cryptography/System.Security.Cryptography.RSA::Encrypt:System.Byte[](System.Byte[],System.Security.Cryptography.RSAEncryptionPadding)", "codegen", "System.Security.Cryptography.RSA", "Encrypt", True, "benchmark" },
    { 933, "System.Security.Cryptography/System.Security.Cryptography.RSA::Encrypt:System.Byte[](System.Byte[],System.Security.Cryptography.RSAEncryptionPadding)", "codegen", "System.Security.Cryptography.RSA", "Encrypt", True, "benchmark" },
    { 934, "System.Security.Cryptography/System.Security.Cryptography.RSA::Decrypt:System.Byte[](System.Byte[],System.Security.Cryptography.RSAEncryptionPadding)", "codegen", "System.Security.Cryptography.RSA", "Decrypt", True, "benchmark" },
    { 935, "System.Security.Cryptography/System.Security.Cryptography.RSA::Decrypt:System.Byte[](System.Byte[],System.Security.Cryptography.RSAEncryptionPadding)", "codegen", "System.Security.Cryptography.RSA", "Decrypt", True, "benchmark" },
    { 936, "System.Security.Cryptography/System.Security.Cryptography.RSA::Decrypt:System.Byte[](System.Byte[],System.Security.Cryptography.RSAEncryptionPadding)", "codegen", "System.Security.Cryptography.RSA", "Decrypt", True, "benchmark" },
    { 937, "System.Security.Cryptography/System.Security.Cryptography.RSA::SignHash:System.Byte[](System.Byte[],System.Security.Cryptography.HashAlgorithmName,System.Security.Cryptography.RSASignaturePadding)", "codegen", "System.Security.Cryptography.RSA", "SignHash", True, "benchmark" },
    { 938, "System.Security.Cryptography/System.Security.Cryptography.RSA::SignHash:System.Byte[](System.Byte[],System.Security.Cryptography.HashAlgorithmName,System.Security.Cryptography.RSASignaturePadding)", "codegen", "System.Security.Cryptography.RSA", "SignHash", True, "benchmark" },
    { 939, "System.Security.Cryptography/System.Security.Cryptography.RSA::SignHash:System.Byte[](System.Byte[],System.Security.Cryptography.HashAlgorithmName,System.Security.Cryptography.RSASignaturePadding)", "codegen", "System.Security.Cryptography.RSA", "SignHash", True, "benchmark" },
    { 940, "System.Security.Cryptography/System.Security.Cryptography.RSA::VerifyHash:System.Boolean(System.Byte[],System.Byte[],System.Security.Cryptography.HashAlgorithmName,System.Security.Cryptography.RSASignaturePadding)", "codegen", "System.Security.Cryptography.RSA", "VerifyHash", True, "benchmark" },
    { 941, "System.Security.Cryptography/System.Security.Cryptography.RSA::VerifyHash:System.Boolean(System.Byte[],System.Byte[],System.Security.Cryptography.HashAlgorithmName,System.Security.Cryptography.RSASignaturePadding)", "codegen", "System.Security.Cryptography.RSA", "VerifyHash", True, "benchmark" },
    { 942, "System.Security.Cryptography/System.Security.Cryptography.RSA::VerifyHash:System.Boolean(System.Byte[],System.Byte[],System.Security.Cryptography.HashAlgorithmName,System.Security.Cryptography.RSASignaturePadding)", "codegen", "System.Security.Cryptography.RSA", "VerifyHash", True, "benchmark" },
    { 943, "System.Security.Cryptography/System.Security.Cryptography.RSA::DecryptValue:System.Byte[](System.Byte[])", "codegen", "System.Security.Cryptography.RSA", "DecryptValue", True, "benchmark" },
    { 944, "System.Security.Cryptography/System.Security.Cryptography.RSA::DecryptValue:System.Byte[](System.Byte[])", "codegen", "System.Security.Cryptography.RSA", "DecryptValue", True, "benchmark" },
    { 945, "System.Security.Cryptography/System.Security.Cryptography.RSA::DecryptValue:System.Byte[](System.Byte[])", "codegen", "System.Security.Cryptography.RSA", "DecryptValue", True, "benchmark" },
    { 946, "System.Security.Cryptography/System.Security.Cryptography.RSA::EncryptValue:System.Byte[](System.Byte[])", "codegen", "System.Security.Cryptography.RSA", "EncryptValue", True, "benchmark" },
    { 947, "System.Security.Cryptography/System.Security.Cryptography.RSA::EncryptValue:System.Byte[](System.Byte[])", "codegen", "System.Security.Cryptography.RSA", "EncryptValue", True, "benchmark" },
    { 948, "System.Security.Cryptography/System.Security.Cryptography.RSA::EncryptValue:System.Byte[](System.Byte[])", "codegen", "System.Security.Cryptography.RSA", "EncryptValue", True, "benchmark" },
    { 949, "System.Security.Cryptography/System.Security.Cryptography.RSA::SignData:System.Byte[](System.Byte[],System.Security.Cryptography.HashAlgorithmName,System.Security.Cryptography.RSASignaturePadding)", "codegen", "System.Security.Cryptography.RSA", "SignData", True, "benchmark" },
    { 950, "System.Security.Cryptography/System.Security.Cryptography.RSA::SignData:System.Byte[](System.Byte[],System.Security.Cryptography.HashAlgorithmName,System.Security.Cryptography.RSASignaturePadding)", "codegen", "System.Security.Cryptography.RSA", "SignData", True, "benchmark" },
    { 951, "System.Security.Cryptography/System.Security.Cryptography.RSA::SignData:System.Byte[](System.Byte[],System.Security.Cryptography.HashAlgorithmName,System.Security.Cryptography.RSASignaturePadding)", "codegen", "System.Security.Cryptography.RSA", "SignData", True, "benchmark" },
    { 952, "System.Security.Cryptography/System.Security.Cryptography.RSA::SignData:System.Byte[](System.Byte[],System.Int32,System.Int32,System.Security.Cryptography.HashAlgorithmName,System.Security.Cryptography.RSASignaturePadding)", "codegen", "System.Security.Cryptography.RSA", "SignData", True, "benchmark" },
    { 953, "System.Security.Cryptography/System.Security.Cryptography.RSA::SignData:System.Byte[](System.Byte[],System.Int32,System.Int32,System.Security.Cryptography.HashAlgorithmName,System.Security.Cryptography.RSASignaturePadding)", "codegen", "System.Security.Cryptography.RSA", "SignData", True, "benchmark" },
    { 954, "System.Security.Cryptography/System.Security.Cryptography.RSA::SignData:System.Byte[](System.Byte[],System.Int32,System.Int32,System.Security.Cryptography.HashAlgorithmName,System.Security.Cryptography.RSASignaturePadding)", "codegen", "System.Security.Cryptography.RSA", "SignData", True, "benchmark" },
    { 955, "System.Security.Cryptography/System.Security.Cryptography.RSA::SignData:System.Byte[](System.Byte[],System.Int32,System.Int32,System.Security.Cryptography.HashAlgorithmName,System.Security.Cryptography.RSASignaturePadding)", "codegen", "System.Security.Cryptography.RSA", "SignData", True, "benchmark" },
    { 956, "System.Security.Cryptography/System.Security.Cryptography.RSA::SignData:System.Byte[](System.Byte[],System.Int32,System.Int32,System.Security.Cryptography.HashAlgorithmName,System.Security.Cryptography.RSASignaturePadding)", "codegen", "System.Security.Cryptography.RSA", "SignData", True, "benchmark" },
    { 957, "System.Security.Cryptography/System.Security.Cryptography.RSA::SignData:System.Byte[](System.IO.Stream,System.Security.Cryptography.HashAlgorithmName,System.Security.Cryptography.RSASignaturePadding)", "codegen", "System.Security.Cryptography.RSA", "SignData", True, "benchmark" },
    { 958, "System.Security.Cryptography/System.Security.Cryptography.RSA::SignData:System.Byte[](System.IO.Stream,System.Security.Cryptography.HashAlgorithmName,System.Security.Cryptography.RSASignaturePadding)", "codegen", "System.Security.Cryptography.RSA", "SignData", True, "benchmark" },
    { 959, "System.Security.Cryptography/System.Security.Cryptography.RSA::VerifyData:System.Boolean(System.Byte[],System.Byte[],System.Security.Cryptography.HashAlgorithmName,System.Security.Cryptography.RSASignaturePadding)", "codegen", "System.Security.Cryptography.RSA", "VerifyData", True, "benchmark" },
    { 960, "System.Security.Cryptography/System.Security.Cryptography.RSA::VerifyData:System.Boolean(System.Byte[],System.Byte[],System.Security.Cryptography.HashAlgorithmName,System.Security.Cryptography.RSASignaturePadding)", "codegen", "System.Security.Cryptography.RSA", "VerifyData", True, "benchmark" },
    { 961, "System.Security.Cryptography/System.Security.Cryptography.RSA::VerifyData:System.Boolean(System.Byte[],System.Byte[],System.Security.Cryptography.HashAlgorithmName,System.Security.Cryptography.RSASignaturePadding)", "codegen", "System.Security.Cryptography.RSA", "VerifyData", True, "benchmark" },
    { 962, "System.Security.Cryptography/System.Security.Cryptography.RSA::VerifyData:System.Boolean(System.Byte[],System.Int32,System.Int32,System.Byte[],System.Security.Cryptography.HashAlgorithmName,System.Security.Cryptography.RSASignaturePadding)", "codegen", "System.Security.Cryptography.RSA", "VerifyData", True, "benchmark" },
    { 963, "System.Security.Cryptography/System.Security.Cryptography.RSA::VerifyData:System.Boolean(System.Byte[],System.Int32,System.Int32,System.Byte[],System.Security.Cryptography.HashAlgorithmName,System.Security.Cryptography.RSASignaturePadding)", "codegen", "System.Security.Cryptography.RSA", "VerifyData", True, "benchmark" },
    { 964, "System.Security.Cryptography/System.Security.Cryptography.RSA::VerifyData:System.Boolean(System.Byte[],System.Int32,System.Int32,System.Byte[],System.Security.Cryptography.HashAlgorithmName,System.Security.Cryptography.RSASignaturePadding)", "codegen", "System.Security.Cryptography.RSA", "VerifyData", True, "benchmark" },
    { 965, "System.Security.Cryptography/System.Security.Cryptography.RSA::VerifyData:System.Boolean(System.Byte[],System.Int32,System.Int32,System.Byte[],System.Security.Cryptography.HashAlgorithmName,System.Security.Cryptography.RSASignaturePadding)", "codegen", "System.Security.Cryptography.RSA", "VerifyData", True, "benchmark" },
    { 966, "System.Security.Cryptography/System.Security.Cryptography.RSA::VerifyData:System.Boolean(System.Byte[],System.Int32,System.Int32,System.Byte[],System.Security.Cryptography.HashAlgorithmName,System.Security.Cryptography.RSASignaturePadding)", "codegen", "System.Security.Cryptography.RSA", "VerifyData", True, "benchmark" },
    { 967, "System.Security.Cryptography/System.Security.Cryptography.RSA::VerifyData:System.Boolean(System.IO.Stream,System.Byte[],System.Security.Cryptography.HashAlgorithmName,System.Security.Cryptography.RSASignaturePadding)", "codegen", "System.Security.Cryptography.RSA", "VerifyData", True, "benchmark" },
    { 968, "System.Security.Cryptography/System.Security.Cryptography.RSA::VerifyData:System.Boolean(System.IO.Stream,System.Byte[],System.Security.Cryptography.HashAlgorithmName,System.Security.Cryptography.RSASignaturePadding)", "codegen", "System.Security.Cryptography.RSA", "VerifyData", True, "benchmark" },
    { 969, "System.Security.Cryptography/System.Security.Cryptography.RSA::VerifyData:System.Boolean(System.IO.Stream,System.Byte[],System.Security.Cryptography.HashAlgorithmName,System.Security.Cryptography.RSASignaturePadding)", "codegen", "System.Security.Cryptography.RSA", "VerifyData", True, "benchmark" },
    { 970, "System.Security.Cryptography/System.Security.Cryptography.RSA::ExportRSAPrivateKey:System.Byte[]()", "codegen", "System.Security.Cryptography.RSA", "ExportRSAPrivateKey", True, "benchmark" },
    { 971, "System.Security.Cryptography/System.Security.Cryptography.RSA::ExportRSAPublicKey:System.Byte[]()", "codegen", "System.Security.Cryptography.RSA", "ExportRSAPublicKey", True, "benchmark" },
    { 972, "System.Security.Cryptography/System.Security.Cryptography.RSA::ExportRSAPrivateKeyPem:System.String()", "codegen", "System.Security.Cryptography.RSA", "ExportRSAPrivateKeyPem", True, "benchmark" },
    { 973, "System.Security.Cryptography/System.Security.Cryptography.RSA::ExportRSAPublicKeyPem:System.String()", "codegen", "System.Security.Cryptography.RSA", "ExportRSAPublicKeyPem", True, "benchmark" },
    { 974, "System.Security.Cryptography/System.Security.Cryptography.RSA::FromXmlString:System.Void(System.String)", "codegen", "System.Security.Cryptography.RSA", "FromXmlString", True, "benchmark" },
    { 975, "System.Security.Cryptography/System.Security.Cryptography.RSA::FromXmlString:System.Void(System.String)", "codegen", "System.Security.Cryptography.RSA", "FromXmlString", True, "benchmark" },
    { 976, "System.Security.Cryptography/System.Security.Cryptography.RSA::ToXmlString:System.String(System.Boolean)", "codegen", "System.Security.Cryptography.RSA", "ToXmlString", True, "benchmark" },
    { 977, "System.Security.Cryptography/System.Security.Cryptography.RSA::ToXmlString:System.String(System.Boolean)", "codegen", "System.Security.Cryptography.RSA", "ToXmlString", True, "benchmark" },
    { 978, "System.Security.Cryptography/System.Security.Cryptography.RSACng::Encrypt:System.Byte[](System.Byte[],System.Security.Cryptography.RSAEncryptionPadding)", "codegen", "System.Security.Cryptography.RSACng", "Encrypt", True, "benchmark" },
    { 979, "System.Security.Cryptography/System.Security.Cryptography.RSACng::Encrypt:System.Byte[](System.Byte[],System.Security.Cryptography.RSAEncryptionPadding)", "codegen", "System.Security.Cryptography.RSACng", "Encrypt", True, "benchmark" },
    { 980, "System.Security.Cryptography/System.Security.Cryptography.RSACng::Encrypt:System.Byte[](System.Byte[],System.Security.Cryptography.RSAEncryptionPadding)", "codegen", "System.Security.Cryptography.RSACng", "Encrypt", True, "benchmark" },
    { 981, "System.Security.Cryptography/System.Security.Cryptography.RSACng::Decrypt:System.Byte[](System.Byte[],System.Security.Cryptography.RSAEncryptionPadding)", "codegen", "System.Security.Cryptography.RSACng", "Decrypt", True, "benchmark" },
    { 982, "System.Security.Cryptography/System.Security.Cryptography.RSACng::Decrypt:System.Byte[](System.Byte[],System.Security.Cryptography.RSAEncryptionPadding)", "codegen", "System.Security.Cryptography.RSACng", "Decrypt", True, "benchmark" },
    { 983, "System.Security.Cryptography/System.Security.Cryptography.RSACng::Decrypt:System.Byte[](System.Byte[],System.Security.Cryptography.RSAEncryptionPadding)", "codegen", "System.Security.Cryptography.RSACng", "Decrypt", True, "benchmark" },
    { 984, "System.Security.Cryptography/System.Security.Cryptography.RSACng::ImportParameters:System.Void(System.Security.Cryptography.RSAParameters)", "codegen", "System.Security.Cryptography.RSACng", "ImportParameters", True, "benchmark" },
    { 985, "System.Security.Cryptography/System.Security.Cryptography.RSACng::ExportParameters:System.Security.Cryptography.RSAParameters(System.Boolean)", "codegen", "System.Security.Cryptography.RSACng", "ExportParameters", True, "benchmark" },
    { 986, "System.Security.Cryptography/System.Security.Cryptography.RSACng::ExportParameters:System.Security.Cryptography.RSAParameters(System.Boolean)", "codegen", "System.Security.Cryptography.RSACng", "ExportParameters", True, "benchmark" },
    { 987, "System.Security.Cryptography/System.Security.Cryptography.RSACng::SignHash:System.Byte[](System.Byte[],System.Security.Cryptography.HashAlgorithmName,System.Security.Cryptography.RSASignaturePadding)", "codegen", "System.Security.Cryptography.RSACng", "SignHash", True, "benchmark" },
    { 988, "System.Security.Cryptography/System.Security.Cryptography.RSACng::SignHash:System.Byte[](System.Byte[],System.Security.Cryptography.HashAlgorithmName,System.Security.Cryptography.RSASignaturePadding)", "codegen", "System.Security.Cryptography.RSACng", "SignHash", True, "benchmark" },
    { 989, "System.Security.Cryptography/System.Security.Cryptography.RSACng::SignHash:System.Byte[](System.Byte[],System.Security.Cryptography.HashAlgorithmName,System.Security.Cryptography.RSASignaturePadding)", "codegen", "System.Security.Cryptography.RSACng", "SignHash", True, "benchmark" },
    { 990, "System.Security.Cryptography/System.Security.Cryptography.RSACng::VerifyHash:System.Boolean(System.Byte[],System.Byte[],System.Security.Cryptography.HashAlgorithmName,System.Security.Cryptography.RSASignaturePadding)", "codegen", "System.Security.Cryptography.RSACng", "VerifyHash", True, "benchmark" },
    { 991, "System.Security.Cryptography/System.Security.Cryptography.RSACng::VerifyHash:System.Boolean(System.Byte[],System.Byte[],System.Security.Cryptography.HashAlgorithmName,System.Security.Cryptography.RSASignaturePadding)", "codegen", "System.Security.Cryptography.RSACng", "VerifyHash", True, "benchmark" },
    { 992, "System.Security.Cryptography/System.Security.Cryptography.RSACng::VerifyHash:System.Boolean(System.Byte[],System.Byte[],System.Security.Cryptography.HashAlgorithmName,System.Security.Cryptography.RSASignaturePadding)", "codegen", "System.Security.Cryptography.RSACng", "VerifyHash", True, "benchmark" },
    { 993, "System.Security.Cryptography/System.Security.Cryptography.RSACryptoServiceProvider::Decrypt:System.Byte[](System.Byte[],System.Boolean)", "codegen", "System.Security.Cryptography.RSACryptoServiceProvider", "Decrypt", True, "benchmark" },
    { 994, "System.Security.Cryptography/System.Security.Cryptography.RSACryptoServiceProvider::Decrypt:System.Byte[](System.Byte[],System.Boolean)", "codegen", "System.Security.Cryptography.RSACryptoServiceProvider", "Decrypt", True, "benchmark" },
    { 995, "System.Security.Cryptography/System.Security.Cryptography.RSACryptoServiceProvider::Decrypt:System.Byte[](System.Byte[],System.Boolean)", "codegen", "System.Security.Cryptography.RSACryptoServiceProvider", "Decrypt", True, "benchmark" },
    { 996, "System.Security.Cryptography/System.Security.Cryptography.RSACryptoServiceProvider::Decrypt:System.Byte[](System.Byte[],System.Boolean)", "codegen", "System.Security.Cryptography.RSACryptoServiceProvider", "Decrypt", True, "benchmark" },
    { 997, "System.Security.Cryptography/System.Security.Cryptography.RSACryptoServiceProvider::DecryptValue:System.Byte[](System.Byte[])", "codegen", "System.Security.Cryptography.RSACryptoServiceProvider", "DecryptValue", True, "benchmark" },
    { 998, "System.Security.Cryptography/System.Security.Cryptography.RSACryptoServiceProvider::DecryptValue:System.Byte[](System.Byte[])", "codegen", "System.Security.Cryptography.RSACryptoServiceProvider", "DecryptValue", True, "benchmark" },
    { 999, "System.Security.Cryptography/System.Security.Cryptography.RSACryptoServiceProvider::DecryptValue:System.Byte[](System.Byte[])", "codegen", "System.Security.Cryptography.RSACryptoServiceProvider", "DecryptValue", True, "benchmark" },
    { 1000, "System.Security.Cryptography/System.Security.Cryptography.RSACryptoServiceProvider::Encrypt:System.Byte[](System.Byte[],System.Boolean)", "codegen", "System.Security.Cryptography.RSACryptoServiceProvider", "Encrypt", True, "benchmark" },
    { 1001, "System.Security.Cryptography/System.Security.Cryptography.RSACryptoServiceProvider::Encrypt:System.Byte[](System.Byte[],System.Boolean)", "codegen", "System.Security.Cryptography.RSACryptoServiceProvider", "Encrypt", True, "benchmark" },
    { 1002, "System.Security.Cryptography/System.Security.Cryptography.RSACryptoServiceProvider::Encrypt:System.Byte[](System.Byte[],System.Boolean)", "codegen", "System.Security.Cryptography.RSACryptoServiceProvider", "Encrypt", True, "benchmark" },
    { 1003, "System.Security.Cryptography/System.Security.Cryptography.RSACryptoServiceProvider::Encrypt:System.Byte[](System.Byte[],System.Boolean)", "codegen", "System.Security.Cryptography.RSACryptoServiceProvider", "Encrypt", True, "benchmark" },
    { 1004, "System.Security.Cryptography/System.Security.Cryptography.RSACryptoServiceProvider::EncryptValue:System.Byte[](System.Byte[])", "codegen", "System.Security.Cryptography.RSACryptoServiceProvider", "EncryptValue", True, "benchmark" },
    { 1005, "System.Security.Cryptography/System.Security.Cryptography.RSACryptoServiceProvider::EncryptValue:System.Byte[](System.Byte[])", "codegen", "System.Security.Cryptography.RSACryptoServiceProvider", "EncryptValue", True, "benchmark" },
    { 1006, "System.Security.Cryptography/System.Security.Cryptography.RSACryptoServiceProvider::EncryptValue:System.Byte[](System.Byte[])", "codegen", "System.Security.Cryptography.RSACryptoServiceProvider", "EncryptValue", True, "benchmark" },
    { 1007, "System.Security.Cryptography/System.Security.Cryptography.RSACryptoServiceProvider::ExportCspBlob:System.Byte[](System.Boolean)", "codegen", "System.Security.Cryptography.RSACryptoServiceProvider", "ExportCspBlob", True, "benchmark" },
    { 1008, "System.Security.Cryptography/System.Security.Cryptography.RSACryptoServiceProvider::ExportCspBlob:System.Byte[](System.Boolean)", "codegen", "System.Security.Cryptography.RSACryptoServiceProvider", "ExportCspBlob", True, "benchmark" },
    { 1009, "System.Security.Cryptography/System.Security.Cryptography.RSACryptoServiceProvider::ExportParameters:System.Security.Cryptography.RSAParameters(System.Boolean)", "codegen", "System.Security.Cryptography.RSACryptoServiceProvider", "ExportParameters", True, "benchmark" },
    { 1010, "System.Security.Cryptography/System.Security.Cryptography.RSACryptoServiceProvider::ExportParameters:System.Security.Cryptography.RSAParameters(System.Boolean)", "codegen", "System.Security.Cryptography.RSACryptoServiceProvider", "ExportParameters", True, "benchmark" },
    { 1011, "System.Security.Cryptography/System.Security.Cryptography.RSACryptoServiceProvider::ImportCspBlob:System.Void(System.Byte[])", "codegen", "System.Security.Cryptography.RSACryptoServiceProvider", "ImportCspBlob", True, "benchmark" },
    { 1012, "System.Security.Cryptography/System.Security.Cryptography.RSACryptoServiceProvider::ImportCspBlob:System.Void(System.Byte[])", "codegen", "System.Security.Cryptography.RSACryptoServiceProvider", "ImportCspBlob", True, "benchmark" },
    { 1013, "System.Security.Cryptography/System.Security.Cryptography.RSACryptoServiceProvider::ImportCspBlob:System.Void(System.Byte[])", "codegen", "System.Security.Cryptography.RSACryptoServiceProvider", "ImportCspBlob", True, "benchmark" },
    { 1014, "System.Security.Cryptography/System.Security.Cryptography.RSACryptoServiceProvider::ImportParameters:System.Void(System.Security.Cryptography.RSAParameters)", "codegen", "System.Security.Cryptography.RSACryptoServiceProvider", "ImportParameters", True, "benchmark" },
    { 1015, "System.Security.Cryptography/System.Security.Cryptography.RSACryptoServiceProvider::SignData:System.Byte[](System.Byte[],System.Int32,System.Int32,System.Object)", "codegen", "System.Security.Cryptography.RSACryptoServiceProvider", "SignData", True, "benchmark" },
    { 1016, "System.Security.Cryptography/System.Security.Cryptography.RSACryptoServiceProvider::SignData:System.Byte[](System.Byte[],System.Int32,System.Int32,System.Object)", "codegen", "System.Security.Cryptography.RSACryptoServiceProvider", "SignData", True, "benchmark" },
    { 1017, "System.Security.Cryptography/System.Security.Cryptography.RSACryptoServiceProvider::SignData:System.Byte[](System.Byte[],System.Int32,System.Int32,System.Object)", "codegen", "System.Security.Cryptography.RSACryptoServiceProvider", "SignData", True, "benchmark" },
    { 1018, "System.Security.Cryptography/System.Security.Cryptography.RSACryptoServiceProvider::SignData:System.Byte[](System.Byte[],System.Int32,System.Int32,System.Object)", "codegen", "System.Security.Cryptography.RSACryptoServiceProvider", "SignData", True, "benchmark" },
    { 1019, "System.Security.Cryptography/System.Security.Cryptography.RSACryptoServiceProvider::SignData:System.Byte[](System.Byte[],System.Int32,System.Int32,System.Object)", "codegen", "System.Security.Cryptography.RSACryptoServiceProvider", "SignData", True, "benchmark" },
    { 1020, "System.Security.Cryptography/System.Security.Cryptography.RSACryptoServiceProvider::SignData:System.Byte[](System.Byte[],System.Object)", "codegen", "System.Security.Cryptography.RSACryptoServiceProvider", "SignData", True, "benchmark" },
    { 1021, "System.Security.Cryptography/System.Security.Cryptography.RSACryptoServiceProvider::SignData:System.Byte[](System.Byte[],System.Object)", "codegen", "System.Security.Cryptography.RSACryptoServiceProvider", "SignData", True, "benchmark" },
    { 1022, "System.Security.Cryptography/System.Security.Cryptography.RSACryptoServiceProvider::SignData:System.Byte[](System.Byte[],System.Object)", "codegen", "System.Security.Cryptography.RSACryptoServiceProvider", "SignData", True, "benchmark" },
    { 1023, "System.Security.Cryptography/System.Security.Cryptography.RSACryptoServiceProvider::SignData:System.Byte[](System.IO.Stream,System.Object)", "codegen", "System.Security.Cryptography.RSACryptoServiceProvider", "SignData", True, "benchmark" },
    { 1024, "System.Security.Cryptography/System.Security.Cryptography.RSACryptoServiceProvider::SignData:System.Byte[](System.IO.Stream,System.Object)", "codegen", "System.Security.Cryptography.RSACryptoServiceProvider", "SignData", True, "benchmark" },
    { 1025, "System.Security.Cryptography/System.Security.Cryptography.RSACryptoServiceProvider::SignHash:System.Byte[](System.Byte[],System.String)", "codegen", "System.Security.Cryptography.RSACryptoServiceProvider", "SignHash", True, "benchmark" },
    { 1026, "System.Security.Cryptography/System.Security.Cryptography.RSACryptoServiceProvider::SignHash:System.Byte[](System.Byte[],System.String)", "codegen", "System.Security.Cryptography.RSACryptoServiceProvider", "SignHash", True, "benchmark" },
    { 1027, "System.Security.Cryptography/System.Security.Cryptography.RSACryptoServiceProvider::SignHash:System.Byte[](System.Byte[],System.String)", "codegen", "System.Security.Cryptography.RSACryptoServiceProvider", "SignHash", True, "benchmark" },
    { 1028, "System.Security.Cryptography/System.Security.Cryptography.RSACryptoServiceProvider::SignHash:System.Byte[](System.Byte[],System.String)", "codegen", "System.Security.Cryptography.RSACryptoServiceProvider", "SignHash", True, "benchmark" },
    { 1029, "System.Security.Cryptography/System.Security.Cryptography.RSACryptoServiceProvider::VerifyData:System.Boolean(System.Byte[],System.Object,System.Byte[])", "codegen", "System.Security.Cryptography.RSACryptoServiceProvider", "VerifyData", True, "benchmark" },
    { 1030, "System.Security.Cryptography/System.Security.Cryptography.RSACryptoServiceProvider::VerifyData:System.Boolean(System.Byte[],System.Object,System.Byte[])", "codegen", "System.Security.Cryptography.RSACryptoServiceProvider", "VerifyData", True, "benchmark" },
    { 1031, "System.Security.Cryptography/System.Security.Cryptography.RSACryptoServiceProvider::VerifyData:System.Boolean(System.Byte[],System.Object,System.Byte[])", "codegen", "System.Security.Cryptography.RSACryptoServiceProvider", "VerifyData", True, "benchmark" },
    { 1032, "System.Security.Cryptography/System.Security.Cryptography.RSACryptoServiceProvider::VerifyHash:System.Boolean(System.Byte[],System.String,System.Byte[])", "codegen", "System.Security.Cryptography.RSACryptoServiceProvider", "VerifyHash", True, "benchmark" },
    { 1033, "System.Security.Cryptography/System.Security.Cryptography.RSACryptoServiceProvider::VerifyHash:System.Boolean(System.Byte[],System.String,System.Byte[])", "codegen", "System.Security.Cryptography.RSACryptoServiceProvider", "VerifyHash", True, "benchmark" },
    { 1034, "System.Security.Cryptography/System.Security.Cryptography.RSACryptoServiceProvider::VerifyHash:System.Boolean(System.Byte[],System.String,System.Byte[])", "codegen", "System.Security.Cryptography.RSACryptoServiceProvider", "VerifyHash", True, "benchmark" },
    { 1035, "System.Security.Cryptography/System.Security.Cryptography.RSACryptoServiceProvider::VerifyHash:System.Boolean(System.Byte[],System.String,System.Byte[])", "codegen", "System.Security.Cryptography.RSACryptoServiceProvider", "VerifyHash", True, "benchmark" },
    { 1036, "System.Security.Cryptography/System.Security.Cryptography.RSACryptoServiceProvider::Encrypt:System.Byte[](System.Byte[],System.Security.Cryptography.RSAEncryptionPadding)", "codegen", "System.Security.Cryptography.RSACryptoServiceProvider", "Encrypt", True, "benchmark" },
    { 1037, "System.Security.Cryptography/System.Security.Cryptography.RSACryptoServiceProvider::Encrypt:System.Byte[](System.Byte[],System.Security.Cryptography.RSAEncryptionPadding)", "codegen", "System.Security.Cryptography.RSACryptoServiceProvider", "Encrypt", True, "benchmark" },
    { 1038, "System.Security.Cryptography/System.Security.Cryptography.RSACryptoServiceProvider::Encrypt:System.Byte[](System.Byte[],System.Security.Cryptography.RSAEncryptionPadding)", "codegen", "System.Security.Cryptography.RSACryptoServiceProvider", "Encrypt", True, "benchmark" },
    { 1039, "System.Security.Cryptography/System.Security.Cryptography.RSACryptoServiceProvider::Decrypt:System.Byte[](System.Byte[],System.Security.Cryptography.RSAEncryptionPadding)", "codegen", "System.Security.Cryptography.RSACryptoServiceProvider", "Decrypt", True, "benchmark" },
    { 1040, "System.Security.Cryptography/System.Security.Cryptography.RSACryptoServiceProvider::Decrypt:System.Byte[](System.Byte[],System.Security.Cryptography.RSAEncryptionPadding)", "codegen", "System.Security.Cryptography.RSACryptoServiceProvider", "Decrypt", True, "benchmark" },
    { 1041, "System.Security.Cryptography/System.Security.Cryptography.RSACryptoServiceProvider::Decrypt:System.Byte[](System.Byte[],System.Security.Cryptography.RSAEncryptionPadding)", "codegen", "System.Security.Cryptography.RSACryptoServiceProvider", "Decrypt", True, "benchmark" },
    { 1042, "System.Security.Cryptography/System.Security.Cryptography.RSACryptoServiceProvider::SignHash:System.Byte[](System.Byte[],System.Security.Cryptography.HashAlgorithmName,System.Security.Cryptography.RSASignaturePadding)", "codegen", "System.Security.Cryptography.RSACryptoServiceProvider", "SignHash", True, "benchmark" },
    { 1043, "System.Security.Cryptography/System.Security.Cryptography.RSACryptoServiceProvider::SignHash:System.Byte[](System.Byte[],System.Security.Cryptography.HashAlgorithmName,System.Security.Cryptography.RSASignaturePadding)", "codegen", "System.Security.Cryptography.RSACryptoServiceProvider", "SignHash", True, "benchmark" },
    { 1044, "System.Security.Cryptography/System.Security.Cryptography.RSACryptoServiceProvider::SignHash:System.Byte[](System.Byte[],System.Security.Cryptography.HashAlgorithmName,System.Security.Cryptography.RSASignaturePadding)", "codegen", "System.Security.Cryptography.RSACryptoServiceProvider", "SignHash", True, "benchmark" },
    { 1045, "System.Security.Cryptography/System.Security.Cryptography.RSACryptoServiceProvider::VerifyHash:System.Boolean(System.Byte[],System.Byte[],System.Security.Cryptography.HashAlgorithmName,System.Security.Cryptography.RSASignaturePadding)", "codegen", "System.Security.Cryptography.RSACryptoServiceProvider", "VerifyHash", True, "benchmark" },
    { 1046, "System.Security.Cryptography/System.Security.Cryptography.RSACryptoServiceProvider::VerifyHash:System.Boolean(System.Byte[],System.Byte[],System.Security.Cryptography.HashAlgorithmName,System.Security.Cryptography.RSASignaturePadding)", "codegen", "System.Security.Cryptography.RSACryptoServiceProvider", "VerifyHash", True, "benchmark" },
    { 1047, "System.Security.Cryptography/System.Security.Cryptography.RSACryptoServiceProvider::VerifyHash:System.Boolean(System.Byte[],System.Byte[],System.Security.Cryptography.HashAlgorithmName,System.Security.Cryptography.RSASignaturePadding)", "codegen", "System.Security.Cryptography.RSACryptoServiceProvider", "VerifyHash", True, "benchmark" },
    { 1048, "System.Security.Cryptography/System.Security.Cryptography.RSAEncryptionPadding::CreateOaep:System.Security.Cryptography.RSAEncryptionPadding(System.Security.Cryptography.HashAlgorithmName)", "codegen", "System.Security.Cryptography.RSAEncryptionPadding", "CreateOaep", True, "benchmark" },
    { 1049, "System.Security.Cryptography/System.Security.Cryptography.RSAOAEPKeyExchangeDeformatter::DecryptKeyExchange:System.Byte[](System.Byte[])", "codegen", "System.Security.Cryptography.RSAOAEPKeyExchangeDeformatter", "DecryptKeyExchange", True, "benchmark" },
    { 1050, "System.Security.Cryptography/System.Security.Cryptography.RSAOAEPKeyExchangeDeformatter::DecryptKeyExchange:System.Byte[](System.Byte[])", "codegen", "System.Security.Cryptography.RSAOAEPKeyExchangeDeformatter", "DecryptKeyExchange", True, "benchmark" },
    { 1051, "System.Security.Cryptography/System.Security.Cryptography.RSAOAEPKeyExchangeDeformatter::DecryptKeyExchange:System.Byte[](System.Byte[])", "codegen", "System.Security.Cryptography.RSAOAEPKeyExchangeDeformatter", "DecryptKeyExchange", True, "benchmark" },
    { 1052, "System.Security.Cryptography/System.Security.Cryptography.RSAOAEPKeyExchangeDeformatter::SetKey:System.Void(System.Security.Cryptography.AsymmetricAlgorithm)", "codegen", "System.Security.Cryptography.RSAOAEPKeyExchangeDeformatter", "SetKey", True, "benchmark" },
    { 1053, "System.Security.Cryptography/System.Security.Cryptography.RSAOAEPKeyExchangeFormatter::SetKey:System.Void(System.Security.Cryptography.AsymmetricAlgorithm)", "codegen", "System.Security.Cryptography.RSAOAEPKeyExchangeFormatter", "SetKey", True, "benchmark" },
    { 1054, "System.Security.Cryptography/System.Security.Cryptography.RSAOAEPKeyExchangeFormatter::CreateKeyExchange:System.Byte[](System.Byte[],System.Type)", "codegen", "System.Security.Cryptography.RSAOAEPKeyExchangeFormatter", "CreateKeyExchange", True, "benchmark" },
    { 1055, "System.Security.Cryptography/System.Security.Cryptography.RSAOAEPKeyExchangeFormatter::CreateKeyExchange:System.Byte[](System.Byte[],System.Type)", "codegen", "System.Security.Cryptography.RSAOAEPKeyExchangeFormatter", "CreateKeyExchange", True, "benchmark" },
    { 1056, "System.Security.Cryptography/System.Security.Cryptography.RSAOAEPKeyExchangeFormatter::CreateKeyExchange:System.Byte[](System.Byte[],System.Type)", "codegen", "System.Security.Cryptography.RSAOAEPKeyExchangeFormatter", "CreateKeyExchange", True, "benchmark" },
    { 1057, "System.Security.Cryptography/System.Security.Cryptography.RSAOAEPKeyExchangeFormatter::CreateKeyExchange:System.Byte[](System.Byte[])", "codegen", "System.Security.Cryptography.RSAOAEPKeyExchangeFormatter", "CreateKeyExchange", True, "benchmark" },
    { 1058, "System.Security.Cryptography/System.Security.Cryptography.RSAOAEPKeyExchangeFormatter::CreateKeyExchange:System.Byte[](System.Byte[])", "codegen", "System.Security.Cryptography.RSAOAEPKeyExchangeFormatter", "CreateKeyExchange", True, "benchmark" },
    { 1059, "System.Security.Cryptography/System.Security.Cryptography.RSAOAEPKeyExchangeFormatter::CreateKeyExchange:System.Byte[](System.Byte[])", "codegen", "System.Security.Cryptography.RSAOAEPKeyExchangeFormatter", "CreateKeyExchange", True, "benchmark" },
    { 1060, "System.Security.Cryptography/System.Security.Cryptography.RSAOpenSsl::DuplicateKeyHandle:System.Security.Cryptography.SafeEvpPKeyHandle()", "codegen", "System.Security.Cryptography.RSAOpenSsl", "DuplicateKeyHandle", True, "benchmark" },
    { 1061, "System.Security.Cryptography/System.Security.Cryptography.RSAOpenSsl::ExportParameters:System.Security.Cryptography.RSAParameters(System.Boolean)", "codegen", "System.Security.Cryptography.RSAOpenSsl", "ExportParameters", True, "benchmark" },
    { 1062, "System.Security.Cryptography/System.Security.Cryptography.RSAOpenSsl::ExportParameters:System.Security.Cryptography.RSAParameters(System.Boolean)", "codegen", "System.Security.Cryptography.RSAOpenSsl", "ExportParameters", True, "benchmark" },
    { 1063, "System.Security.Cryptography/System.Security.Cryptography.RSAOpenSsl::ImportParameters:System.Void(System.Security.Cryptography.RSAParameters)", "codegen", "System.Security.Cryptography.RSAOpenSsl", "ImportParameters", True, "benchmark" },
    { 1064, "System.Security.Cryptography/System.Security.Cryptography.RSAPKCS1KeyExchangeDeformatter::DecryptKeyExchange:System.Byte[](System.Byte[])", "codegen", "System.Security.Cryptography.RSAPKCS1KeyExchangeDeformatter", "DecryptKeyExchange", True, "benchmark" },
    { 1065, "System.Security.Cryptography/System.Security.Cryptography.RSAPKCS1KeyExchangeDeformatter::DecryptKeyExchange:System.Byte[](System.Byte[])", "codegen", "System.Security.Cryptography.RSAPKCS1KeyExchangeDeformatter", "DecryptKeyExchange", True, "benchmark" },
    { 1066, "System.Security.Cryptography/System.Security.Cryptography.RSAPKCS1KeyExchangeDeformatter::DecryptKeyExchange:System.Byte[](System.Byte[])", "codegen", "System.Security.Cryptography.RSAPKCS1KeyExchangeDeformatter", "DecryptKeyExchange", True, "benchmark" },
    { 1067, "System.Security.Cryptography/System.Security.Cryptography.RSAPKCS1KeyExchangeDeformatter::SetKey:System.Void(System.Security.Cryptography.AsymmetricAlgorithm)", "codegen", "System.Security.Cryptography.RSAPKCS1KeyExchangeDeformatter", "SetKey", True, "benchmark" },
    { 1068, "System.Security.Cryptography/System.Security.Cryptography.RSAPKCS1KeyExchangeFormatter::SetKey:System.Void(System.Security.Cryptography.AsymmetricAlgorithm)", "codegen", "System.Security.Cryptography.RSAPKCS1KeyExchangeFormatter", "SetKey", True, "benchmark" },
    { 1069, "System.Security.Cryptography/System.Security.Cryptography.RSAPKCS1KeyExchangeFormatter::CreateKeyExchange:System.Byte[](System.Byte[],System.Type)", "codegen", "System.Security.Cryptography.RSAPKCS1KeyExchangeFormatter", "CreateKeyExchange", True, "benchmark" },
    { 1070, "System.Security.Cryptography/System.Security.Cryptography.RSAPKCS1KeyExchangeFormatter::CreateKeyExchange:System.Byte[](System.Byte[],System.Type)", "codegen", "System.Security.Cryptography.RSAPKCS1KeyExchangeFormatter", "CreateKeyExchange", True, "benchmark" },
    { 1071, "System.Security.Cryptography/System.Security.Cryptography.RSAPKCS1KeyExchangeFormatter::CreateKeyExchange:System.Byte[](System.Byte[],System.Type)", "codegen", "System.Security.Cryptography.RSAPKCS1KeyExchangeFormatter", "CreateKeyExchange", True, "benchmark" },
    { 1072, "System.Security.Cryptography/System.Security.Cryptography.RSAPKCS1KeyExchangeFormatter::CreateKeyExchange:System.Byte[](System.Byte[])", "codegen", "System.Security.Cryptography.RSAPKCS1KeyExchangeFormatter", "CreateKeyExchange", True, "benchmark" },
    { 1073, "System.Security.Cryptography/System.Security.Cryptography.RSAPKCS1KeyExchangeFormatter::CreateKeyExchange:System.Byte[](System.Byte[])", "codegen", "System.Security.Cryptography.RSAPKCS1KeyExchangeFormatter", "CreateKeyExchange", True, "benchmark" },
    { 1074, "System.Security.Cryptography/System.Security.Cryptography.RSAPKCS1KeyExchangeFormatter::CreateKeyExchange:System.Byte[](System.Byte[])", "codegen", "System.Security.Cryptography.RSAPKCS1KeyExchangeFormatter", "CreateKeyExchange", True, "benchmark" },
    { 1075, "System.Security.Cryptography/System.Security.Cryptography.RSAPKCS1SignatureDeformatter::SetKey:System.Void(System.Security.Cryptography.AsymmetricAlgorithm)", "codegen", "System.Security.Cryptography.RSAPKCS1SignatureDeformatter", "SetKey", True, "benchmark" },
    { 1076, "System.Security.Cryptography/System.Security.Cryptography.RSAPKCS1SignatureDeformatter::SetHashAlgorithm:System.Void(System.String)", "codegen", "System.Security.Cryptography.RSAPKCS1SignatureDeformatter", "SetHashAlgorithm", True, "benchmark" },
    { 1077, "System.Security.Cryptography/System.Security.Cryptography.RSAPKCS1SignatureDeformatter::SetHashAlgorithm:System.Void(System.String)", "codegen", "System.Security.Cryptography.RSAPKCS1SignatureDeformatter", "SetHashAlgorithm", True, "benchmark" },
    { 1078, "System.Security.Cryptography/System.Security.Cryptography.RSAPKCS1SignatureDeformatter::VerifySignature:System.Boolean(System.Byte[],System.Byte[])", "codegen", "System.Security.Cryptography.RSAPKCS1SignatureDeformatter", "VerifySignature", True, "benchmark" },
    { 1079, "System.Security.Cryptography/System.Security.Cryptography.RSAPKCS1SignatureDeformatter::VerifySignature:System.Boolean(System.Byte[],System.Byte[])", "codegen", "System.Security.Cryptography.RSAPKCS1SignatureDeformatter", "VerifySignature", True, "benchmark" },
    { 1080, "System.Security.Cryptography/System.Security.Cryptography.RSAPKCS1SignatureDeformatter::VerifySignature:System.Boolean(System.Byte[],System.Byte[])", "codegen", "System.Security.Cryptography.RSAPKCS1SignatureDeformatter", "VerifySignature", True, "benchmark" },
    { 1081, "System.Security.Cryptography/System.Security.Cryptography.RSAPKCS1SignatureFormatter::SetKey:System.Void(System.Security.Cryptography.AsymmetricAlgorithm)", "codegen", "System.Security.Cryptography.RSAPKCS1SignatureFormatter", "SetKey", True, "benchmark" },
    { 1082, "System.Security.Cryptography/System.Security.Cryptography.RSAPKCS1SignatureFormatter::SetHashAlgorithm:System.Void(System.String)", "codegen", "System.Security.Cryptography.RSAPKCS1SignatureFormatter", "SetHashAlgorithm", True, "benchmark" },
    { 1083, "System.Security.Cryptography/System.Security.Cryptography.RSAPKCS1SignatureFormatter::SetHashAlgorithm:System.Void(System.String)", "codegen", "System.Security.Cryptography.RSAPKCS1SignatureFormatter", "SetHashAlgorithm", True, "benchmark" },
    { 1084, "System.Security.Cryptography/System.Security.Cryptography.RSAPKCS1SignatureFormatter::CreateSignature:System.Byte[](System.Byte[])", "codegen", "System.Security.Cryptography.RSAPKCS1SignatureFormatter", "CreateSignature", True, "benchmark" },
    { 1085, "System.Security.Cryptography/System.Security.Cryptography.RSAPKCS1SignatureFormatter::CreateSignature:System.Byte[](System.Byte[])", "codegen", "System.Security.Cryptography.RSAPKCS1SignatureFormatter", "CreateSignature", True, "benchmark" },
    { 1086, "System.Security.Cryptography/System.Security.Cryptography.RSAPKCS1SignatureFormatter::CreateSignature:System.Byte[](System.Byte[])", "codegen", "System.Security.Cryptography.RSAPKCS1SignatureFormatter", "CreateSignature", True, "benchmark" },
    { 1087, "System.Security.Cryptography/System.Security.Cryptography.SafeEvpPKeyHandle::OpenPrivateKeyFromEngine:System.Security.Cryptography.SafeEvpPKeyHandle(System.String,System.String)", "codegen", "System.Security.Cryptography.SafeEvpPKeyHandle", "OpenPrivateKeyFromEngine", True, "benchmark" },
    { 1088, "System.Security.Cryptography/System.Security.Cryptography.SafeEvpPKeyHandle::OpenPrivateKeyFromEngine:System.Security.Cryptography.SafeEvpPKeyHandle(System.String,System.String)", "codegen", "System.Security.Cryptography.SafeEvpPKeyHandle", "OpenPrivateKeyFromEngine", True, "benchmark" },
    { 1089, "System.Security.Cryptography/System.Security.Cryptography.SafeEvpPKeyHandle::OpenPrivateKeyFromEngine:System.Security.Cryptography.SafeEvpPKeyHandle(System.String,System.String)", "codegen", "System.Security.Cryptography.SafeEvpPKeyHandle", "OpenPrivateKeyFromEngine", True, "benchmark" },
    { 1090, "System.Security.Cryptography/System.Security.Cryptography.SafeEvpPKeyHandle::OpenPublicKeyFromEngine:System.Security.Cryptography.SafeEvpPKeyHandle(System.String,System.String)", "codegen", "System.Security.Cryptography.SafeEvpPKeyHandle", "OpenPublicKeyFromEngine", True, "benchmark" },
    { 1091, "System.Security.Cryptography/System.Security.Cryptography.SafeEvpPKeyHandle::OpenPublicKeyFromEngine:System.Security.Cryptography.SafeEvpPKeyHandle(System.String,System.String)", "codegen", "System.Security.Cryptography.SafeEvpPKeyHandle", "OpenPublicKeyFromEngine", True, "benchmark" },
    { 1092, "System.Security.Cryptography/System.Security.Cryptography.SafeEvpPKeyHandle::OpenPublicKeyFromEngine:System.Security.Cryptography.SafeEvpPKeyHandle(System.String,System.String)", "codegen", "System.Security.Cryptography.SafeEvpPKeyHandle", "OpenPublicKeyFromEngine", True, "benchmark" },
    { 1093, "System.Security.Cryptography/System.Security.Cryptography.SafeEvpPKeyHandle::OpenKeyFromProvider:System.Security.Cryptography.SafeEvpPKeyHandle(System.String,System.String)", "codegen", "System.Security.Cryptography.SafeEvpPKeyHandle", "OpenKeyFromProvider", True, "benchmark" },
    { 1094, "System.Security.Cryptography/System.Security.Cryptography.SafeEvpPKeyHandle::OpenKeyFromProvider:System.Security.Cryptography.SafeEvpPKeyHandle(System.String,System.String)", "codegen", "System.Security.Cryptography.SafeEvpPKeyHandle", "OpenKeyFromProvider", True, "benchmark" },
    { 1095, "System.Security.Cryptography/System.Security.Cryptography.SafeEvpPKeyHandle::OpenKeyFromProvider:System.Security.Cryptography.SafeEvpPKeyHandle(System.String,System.String)", "codegen", "System.Security.Cryptography.SafeEvpPKeyHandle", "OpenKeyFromProvider", True, "benchmark" },
    { 1096, "System.Security.Cryptography/System.Security.Cryptography.SafeEvpPKeyHandle::DuplicateHandle:System.Security.Cryptography.SafeEvpPKeyHandle()", "codegen", "System.Security.Cryptography.SafeEvpPKeyHandle", "DuplicateHandle", True, "benchmark" },
    { 1097, "System.Security.Cryptography/System.Security.Cryptography.SHA1::HashData:System.Byte[](System.Byte[])", "codegen", "System.Security.Cryptography.SHA1", "HashData", True, "benchmark" },
    { 1098, "System.Security.Cryptography/System.Security.Cryptography.SHA1::HashData:System.Byte[](System.Byte[])", "codegen", "System.Security.Cryptography.SHA1", "HashData", True, "benchmark" },
    { 1099, "System.Security.Cryptography/System.Security.Cryptography.SHA1::HashData:System.Byte[](System.Byte[])", "codegen", "System.Security.Cryptography.SHA1", "HashData", True, "benchmark" },
    { 1100, "System.Security.Cryptography/System.Security.Cryptography.SHA1::HashData:System.Byte[](System.IO.Stream)", "codegen", "System.Security.Cryptography.SHA1", "HashData", True, "benchmark" },
    { 1101, "System.Security.Cryptography/System.Security.Cryptography.SHA1::HashData:System.Byte[](System.IO.Stream)", "codegen", "System.Security.Cryptography.SHA1", "HashData", True, "benchmark" },
    { 1102, "System.Security.Cryptography/System.Security.Cryptography.SHA1::HashDataAsync:System.Threading.Tasks.ValueTask<System.Byte[]>(System.IO.Stream,System.Threading.CancellationToken)", "codegen", "System.Security.Cryptography.SHA1", "HashDataAsync", True, "benchmark" },
    { 1103, "System.Security.Cryptography/System.Security.Cryptography.SHA1::HashDataAsync:System.Threading.Tasks.ValueTask<System.Byte[]>(System.IO.Stream,System.Threading.CancellationToken)", "codegen", "System.Security.Cryptography.SHA1", "HashDataAsync", True, "benchmark" },
    { 1104, "System.Security.Cryptography/System.Security.Cryptography.SHA1::HashDataAsync:System.Threading.Tasks.ValueTask<System.Int32>(System.IO.Stream,System.Memory<System.Byte>,System.Threading.CancellationToken)", "codegen", "System.Security.Cryptography.SHA1", "HashDataAsync", True, "benchmark" },
    { 1105, "System.Security.Cryptography/System.Security.Cryptography.SHA1::HashDataAsync:System.Threading.Tasks.ValueTask<System.Int32>(System.IO.Stream,System.Memory<System.Byte>,System.Threading.CancellationToken)", "codegen", "System.Security.Cryptography.SHA1", "HashDataAsync", True, "benchmark" },
    { 1106, "System.Security.Cryptography/System.Security.Cryptography.SHA1CryptoServiceProvider::Initialize:System.Void()", "codegen", "System.Security.Cryptography.SHA1CryptoServiceProvider", "Initialize", True, "benchmark" },
    { 1107, "System.Security.Cryptography/System.Security.Cryptography.SHA1Managed::Initialize:System.Void()", "codegen", "System.Security.Cryptography.SHA1Managed", "Initialize", True, "benchmark" },
    { 1108, "System.Security.Cryptography/System.Security.Cryptography.SHA256::HashData:System.Byte[](System.Byte[])", "codegen", "System.Security.Cryptography.SHA256", "HashData", True, "benchmark" },
    { 1109, "System.Security.Cryptography/System.Security.Cryptography.SHA256::HashData:System.Byte[](System.Byte[])", "codegen", "System.Security.Cryptography.SHA256", "HashData", True, "benchmark" },
    { 1110, "System.Security.Cryptography/System.Security.Cryptography.SHA256::HashData:System.Byte[](System.Byte[])", "codegen", "System.Security.Cryptography.SHA256", "HashData", True, "benchmark" },
    { 1111, "System.Security.Cryptography/System.Security.Cryptography.SHA256::HashData:System.Byte[](System.IO.Stream)", "codegen", "System.Security.Cryptography.SHA256", "HashData", True, "benchmark" },
    { 1112, "System.Security.Cryptography/System.Security.Cryptography.SHA256::HashData:System.Byte[](System.IO.Stream)", "codegen", "System.Security.Cryptography.SHA256", "HashData", True, "benchmark" },
    { 1113, "System.Security.Cryptography/System.Security.Cryptography.SHA256::HashDataAsync:System.Threading.Tasks.ValueTask<System.Byte[]>(System.IO.Stream,System.Threading.CancellationToken)", "codegen", "System.Security.Cryptography.SHA256", "HashDataAsync", True, "benchmark" },
    { 1114, "System.Security.Cryptography/System.Security.Cryptography.SHA256::HashDataAsync:System.Threading.Tasks.ValueTask<System.Byte[]>(System.IO.Stream,System.Threading.CancellationToken)", "codegen", "System.Security.Cryptography.SHA256", "HashDataAsync", True, "benchmark" },
    { 1115, "System.Security.Cryptography/System.Security.Cryptography.SHA256::HashDataAsync:System.Threading.Tasks.ValueTask<System.Int32>(System.IO.Stream,System.Memory<System.Byte>,System.Threading.CancellationToken)", "codegen", "System.Security.Cryptography.SHA256", "HashDataAsync", True, "benchmark" },
    { 1116, "System.Security.Cryptography/System.Security.Cryptography.SHA256::HashDataAsync:System.Threading.Tasks.ValueTask<System.Int32>(System.IO.Stream,System.Memory<System.Byte>,System.Threading.CancellationToken)", "codegen", "System.Security.Cryptography.SHA256", "HashDataAsync", True, "benchmark" },
    { 1117, "System.Security.Cryptography/System.Security.Cryptography.SHA256CryptoServiceProvider::Initialize:System.Void()", "codegen", "System.Security.Cryptography.SHA256CryptoServiceProvider", "Initialize", True, "benchmark" },
    { 1118, "System.Security.Cryptography/System.Security.Cryptography.SHA256Managed::Initialize:System.Void()", "codegen", "System.Security.Cryptography.SHA256Managed", "Initialize", True, "benchmark" },
    { 1119, "System.Security.Cryptography/System.Security.Cryptography.SHA3_256::HashData:System.Byte[](System.Byte[])", "codegen", "System.Security.Cryptography.SHA3_256", "HashData", True, "benchmark" },
    { 1120, "System.Security.Cryptography/System.Security.Cryptography.SHA3_256::HashData:System.Byte[](System.Byte[])", "codegen", "System.Security.Cryptography.SHA3_256", "HashData", True, "benchmark" },
    { 1121, "System.Security.Cryptography/System.Security.Cryptography.SHA3_256::HashData:System.Byte[](System.Byte[])", "codegen", "System.Security.Cryptography.SHA3_256", "HashData", True, "benchmark" },
    { 1122, "System.Security.Cryptography/System.Security.Cryptography.SHA3_256::HashData:System.Byte[](System.IO.Stream)", "codegen", "System.Security.Cryptography.SHA3_256", "HashData", True, "benchmark" },
    { 1123, "System.Security.Cryptography/System.Security.Cryptography.SHA3_256::HashData:System.Byte[](System.IO.Stream)", "codegen", "System.Security.Cryptography.SHA3_256", "HashData", True, "benchmark" },
    { 1124, "System.Security.Cryptography/System.Security.Cryptography.SHA3_256::HashDataAsync:System.Threading.Tasks.ValueTask<System.Byte[]>(System.IO.Stream,System.Threading.CancellationToken)", "codegen", "System.Security.Cryptography.SHA3_256", "HashDataAsync", True, "benchmark" },
    { 1125, "System.Security.Cryptography/System.Security.Cryptography.SHA3_256::HashDataAsync:System.Threading.Tasks.ValueTask<System.Byte[]>(System.IO.Stream,System.Threading.CancellationToken)", "codegen", "System.Security.Cryptography.SHA3_256", "HashDataAsync", True, "benchmark" },
    { 1126, "System.Security.Cryptography/System.Security.Cryptography.SHA3_256::HashDataAsync:System.Threading.Tasks.ValueTask<System.Int32>(System.IO.Stream,System.Memory<System.Byte>,System.Threading.CancellationToken)", "codegen", "System.Security.Cryptography.SHA3_256", "HashDataAsync", True, "benchmark" },
    { 1127, "System.Security.Cryptography/System.Security.Cryptography.SHA3_256::HashDataAsync:System.Threading.Tasks.ValueTask<System.Int32>(System.IO.Stream,System.Memory<System.Byte>,System.Threading.CancellationToken)", "codegen", "System.Security.Cryptography.SHA3_256", "HashDataAsync", True, "benchmark" },
    { 1128, "System.Security.Cryptography/System.Security.Cryptography.SHA3_384::HashData:System.Byte[](System.Byte[])", "codegen", "System.Security.Cryptography.SHA3_384", "HashData", True, "benchmark" },
    { 1129, "System.Security.Cryptography/System.Security.Cryptography.SHA3_384::HashData:System.Byte[](System.Byte[])", "codegen", "System.Security.Cryptography.SHA3_384", "HashData", True, "benchmark" },
    { 1130, "System.Security.Cryptography/System.Security.Cryptography.SHA3_384::HashData:System.Byte[](System.Byte[])", "codegen", "System.Security.Cryptography.SHA3_384", "HashData", True, "benchmark" },
    { 1131, "System.Security.Cryptography/System.Security.Cryptography.SHA3_384::HashData:System.Byte[](System.IO.Stream)", "codegen", "System.Security.Cryptography.SHA3_384", "HashData", True, "benchmark" },
    { 1132, "System.Security.Cryptography/System.Security.Cryptography.SHA3_384::HashData:System.Byte[](System.IO.Stream)", "codegen", "System.Security.Cryptography.SHA3_384", "HashData", True, "benchmark" },
    { 1133, "System.Security.Cryptography/System.Security.Cryptography.SHA3_384::HashDataAsync:System.Threading.Tasks.ValueTask<System.Byte[]>(System.IO.Stream,System.Threading.CancellationToken)", "codegen", "System.Security.Cryptography.SHA3_384", "HashDataAsync", True, "benchmark" },
    { 1134, "System.Security.Cryptography/System.Security.Cryptography.SHA3_384::HashDataAsync:System.Threading.Tasks.ValueTask<System.Byte[]>(System.IO.Stream,System.Threading.CancellationToken)", "codegen", "System.Security.Cryptography.SHA3_384", "HashDataAsync", True, "benchmark" },
    { 1135, "System.Security.Cryptography/System.Security.Cryptography.SHA3_384::HashDataAsync:System.Threading.Tasks.ValueTask<System.Int32>(System.IO.Stream,System.Memory<System.Byte>,System.Threading.CancellationToken)", "codegen", "System.Security.Cryptography.SHA3_384", "HashDataAsync", True, "benchmark" },
    { 1136, "System.Security.Cryptography/System.Security.Cryptography.SHA3_384::HashDataAsync:System.Threading.Tasks.ValueTask<System.Int32>(System.IO.Stream,System.Memory<System.Byte>,System.Threading.CancellationToken)", "codegen", "System.Security.Cryptography.SHA3_384", "HashDataAsync", True, "benchmark" },
    { 1137, "System.Security.Cryptography/System.Security.Cryptography.SHA3_512::HashData:System.Byte[](System.Byte[])", "codegen", "System.Security.Cryptography.SHA3_512", "HashData", True, "benchmark" },
    { 1138, "System.Security.Cryptography/System.Security.Cryptography.SHA3_512::HashData:System.Byte[](System.Byte[])", "codegen", "System.Security.Cryptography.SHA3_512", "HashData", True, "benchmark" },
    { 1139, "System.Security.Cryptography/System.Security.Cryptography.SHA3_512::HashData:System.Byte[](System.Byte[])", "codegen", "System.Security.Cryptography.SHA3_512", "HashData", True, "benchmark" },
    { 1140, "System.Security.Cryptography/System.Security.Cryptography.SHA3_512::HashData:System.Byte[](System.IO.Stream)", "codegen", "System.Security.Cryptography.SHA3_512", "HashData", True, "benchmark" },
    { 1141, "System.Security.Cryptography/System.Security.Cryptography.SHA3_512::HashData:System.Byte[](System.IO.Stream)", "codegen", "System.Security.Cryptography.SHA3_512", "HashData", True, "benchmark" },
    { 1142, "System.Security.Cryptography/System.Security.Cryptography.SHA3_512::HashDataAsync:System.Threading.Tasks.ValueTask<System.Byte[]>(System.IO.Stream,System.Threading.CancellationToken)", "codegen", "System.Security.Cryptography.SHA3_512", "HashDataAsync", True, "benchmark" },
    { 1143, "System.Security.Cryptography/System.Security.Cryptography.SHA3_512::HashDataAsync:System.Threading.Tasks.ValueTask<System.Byte[]>(System.IO.Stream,System.Threading.CancellationToken)", "codegen", "System.Security.Cryptography.SHA3_512", "HashDataAsync", True, "benchmark" },
    { 1144, "System.Security.Cryptography/System.Security.Cryptography.SHA3_512::HashDataAsync:System.Threading.Tasks.ValueTask<System.Int32>(System.IO.Stream,System.Memory<System.Byte>,System.Threading.CancellationToken)", "codegen", "System.Security.Cryptography.SHA3_512", "HashDataAsync", True, "benchmark" },
    { 1145, "System.Security.Cryptography/System.Security.Cryptography.SHA3_512::HashDataAsync:System.Threading.Tasks.ValueTask<System.Int32>(System.IO.Stream,System.Memory<System.Byte>,System.Threading.CancellationToken)", "codegen", "System.Security.Cryptography.SHA3_512", "HashDataAsync", True, "benchmark" },
    { 1146, "System.Security.Cryptography/System.Security.Cryptography.SHA384::HashData:System.Byte[](System.Byte[])", "codegen", "System.Security.Cryptography.SHA384", "HashData", True, "benchmark" },
    { 1147, "System.Security.Cryptography/System.Security.Cryptography.SHA384::HashData:System.Byte[](System.Byte[])", "codegen", "System.Security.Cryptography.SHA384", "HashData", True, "benchmark" },
    { 1148, "System.Security.Cryptography/System.Security.Cryptography.SHA384::HashData:System.Byte[](System.Byte[])", "codegen", "System.Security.Cryptography.SHA384", "HashData", True, "benchmark" },
    { 1149, "System.Security.Cryptography/System.Security.Cryptography.SHA384::HashData:System.Byte[](System.IO.Stream)", "codegen", "System.Security.Cryptography.SHA384", "HashData", True, "benchmark" },
    { 1150, "System.Security.Cryptography/System.Security.Cryptography.SHA384::HashData:System.Byte[](System.IO.Stream)", "codegen", "System.Security.Cryptography.SHA384", "HashData", True, "benchmark" },
    { 1151, "System.Security.Cryptography/System.Security.Cryptography.SHA384::HashDataAsync:System.Threading.Tasks.ValueTask<System.Byte[]>(System.IO.Stream,System.Threading.CancellationToken)", "codegen", "System.Security.Cryptography.SHA384", "HashDataAsync", True, "benchmark" },
    { 1152, "System.Security.Cryptography/System.Security.Cryptography.SHA384::HashDataAsync:System.Threading.Tasks.ValueTask<System.Byte[]>(System.IO.Stream,System.Threading.CancellationToken)", "codegen", "System.Security.Cryptography.SHA384", "HashDataAsync", True, "benchmark" },
    { 1153, "System.Security.Cryptography/System.Security.Cryptography.SHA384::HashDataAsync:System.Threading.Tasks.ValueTask<System.Int32>(System.IO.Stream,System.Memory<System.Byte>,System.Threading.CancellationToken)", "codegen", "System.Security.Cryptography.SHA384", "HashDataAsync", True, "benchmark" },
    { 1154, "System.Security.Cryptography/System.Security.Cryptography.SHA384::HashDataAsync:System.Threading.Tasks.ValueTask<System.Int32>(System.IO.Stream,System.Memory<System.Byte>,System.Threading.CancellationToken)", "codegen", "System.Security.Cryptography.SHA384", "HashDataAsync", True, "benchmark" },
    { 1155, "System.Security.Cryptography/System.Security.Cryptography.SHA384CryptoServiceProvider::Initialize:System.Void()", "codegen", "System.Security.Cryptography.SHA384CryptoServiceProvider", "Initialize", True, "benchmark" },
    { 1156, "System.Security.Cryptography/System.Security.Cryptography.SHA384Managed::Initialize:System.Void()", "codegen", "System.Security.Cryptography.SHA384Managed", "Initialize", True, "benchmark" },
    { 1157, "System.Security.Cryptography/System.Security.Cryptography.SHA512::HashData:System.Byte[](System.Byte[])", "codegen", "System.Security.Cryptography.SHA512", "HashData", True, "benchmark" },
    { 1158, "System.Security.Cryptography/System.Security.Cryptography.SHA512::HashData:System.Byte[](System.Byte[])", "codegen", "System.Security.Cryptography.SHA512", "HashData", True, "benchmark" },
    { 1159, "System.Security.Cryptography/System.Security.Cryptography.SHA512::HashData:System.Byte[](System.Byte[])", "codegen", "System.Security.Cryptography.SHA512", "HashData", True, "benchmark" },
    { 1160, "System.Security.Cryptography/System.Security.Cryptography.SHA512::HashData:System.Byte[](System.IO.Stream)", "codegen", "System.Security.Cryptography.SHA512", "HashData", True, "benchmark" },
    { 1161, "System.Security.Cryptography/System.Security.Cryptography.SHA512::HashData:System.Byte[](System.IO.Stream)", "codegen", "System.Security.Cryptography.SHA512", "HashData", True, "benchmark" },
    { 1162, "System.Security.Cryptography/System.Security.Cryptography.SHA512::HashDataAsync:System.Threading.Tasks.ValueTask<System.Byte[]>(System.IO.Stream,System.Threading.CancellationToken)", "codegen", "System.Security.Cryptography.SHA512", "HashDataAsync", True, "benchmark" },
    { 1163, "System.Security.Cryptography/System.Security.Cryptography.SHA512::HashDataAsync:System.Threading.Tasks.ValueTask<System.Byte[]>(System.IO.Stream,System.Threading.CancellationToken)", "codegen", "System.Security.Cryptography.SHA512", "HashDataAsync", True, "benchmark" },
    { 1164, "System.Security.Cryptography/System.Security.Cryptography.SHA512::HashDataAsync:System.Threading.Tasks.ValueTask<System.Int32>(System.IO.Stream,System.Memory<System.Byte>,System.Threading.CancellationToken)", "codegen", "System.Security.Cryptography.SHA512", "HashDataAsync", True, "benchmark" },
    { 1165, "System.Security.Cryptography/System.Security.Cryptography.SHA512::HashDataAsync:System.Threading.Tasks.ValueTask<System.Int32>(System.IO.Stream,System.Memory<System.Byte>,System.Threading.CancellationToken)", "codegen", "System.Security.Cryptography.SHA512", "HashDataAsync", True, "benchmark" },
    { 1166, "System.Security.Cryptography/System.Security.Cryptography.SHA512CryptoServiceProvider::Initialize:System.Void()", "codegen", "System.Security.Cryptography.SHA512CryptoServiceProvider", "Initialize", True, "benchmark" },
    { 1167, "System.Security.Cryptography/System.Security.Cryptography.SHA512Managed::Initialize:System.Void()", "codegen", "System.Security.Cryptography.SHA512Managed", "Initialize", True, "benchmark" },
    { 1168, "System.Security.Cryptography/System.Security.Cryptography.Shake128::AppendData:System.Void(System.Byte[])", "codegen", "System.Security.Cryptography.Shake128", "AppendData", True, "benchmark" },
    { 1169, "System.Security.Cryptography/System.Security.Cryptography.Shake128::AppendData:System.Void(System.Byte[])", "codegen", "System.Security.Cryptography.Shake128", "AppendData", True, "benchmark" },
    { 1170, "System.Security.Cryptography/System.Security.Cryptography.Shake128::AppendData:System.Void(System.Byte[])", "codegen", "System.Security.Cryptography.Shake128", "AppendData", True, "benchmark" },
    { 1171, "System.Security.Cryptography/System.Security.Cryptography.Shake128::GetHashAndReset:System.Byte[](System.Int32)", "codegen", "System.Security.Cryptography.Shake128", "GetHashAndReset", True, "benchmark" },
    { 1172, "System.Security.Cryptography/System.Security.Cryptography.Shake128::GetHashAndReset:System.Byte[](System.Int32)", "codegen", "System.Security.Cryptography.Shake128", "GetHashAndReset", True, "benchmark" },
    { 1173, "System.Security.Cryptography/System.Security.Cryptography.Shake128::GetCurrentHash:System.Byte[](System.Int32)", "codegen", "System.Security.Cryptography.Shake128", "GetCurrentHash", True, "benchmark" },
    { 1174, "System.Security.Cryptography/System.Security.Cryptography.Shake128::GetCurrentHash:System.Byte[](System.Int32)", "codegen", "System.Security.Cryptography.Shake128", "GetCurrentHash", True, "benchmark" },
    { 1175, "System.Security.Cryptography/System.Security.Cryptography.Shake128::Read:System.Byte[](System.Int32)", "codegen", "System.Security.Cryptography.Shake128", "Read", True, "benchmark" },
    { 1176, "System.Security.Cryptography/System.Security.Cryptography.Shake128::Read:System.Byte[](System.Int32)", "codegen", "System.Security.Cryptography.Shake128", "Read", True, "benchmark" },
    { 1177, "System.Security.Cryptography/System.Security.Cryptography.Shake128::Clone:System.Security.Cryptography.Shake128()", "codegen", "System.Security.Cryptography.Shake128", "Clone", True, "benchmark" },
    { 1178, "System.Security.Cryptography/System.Security.Cryptography.Shake128::Dispose:System.Void()", "codegen", "System.Security.Cryptography.Shake128", "Dispose", True, "benchmark" },
    { 1179, "System.Security.Cryptography/System.Security.Cryptography.Shake128::HashData:System.Byte[](System.Byte[],System.Int32)", "codegen", "System.Security.Cryptography.Shake128", "HashData", True, "benchmark" },
    { 1180, "System.Security.Cryptography/System.Security.Cryptography.Shake128::HashData:System.Byte[](System.Byte[],System.Int32)", "codegen", "System.Security.Cryptography.Shake128", "HashData", True, "benchmark" },
    { 1181, "System.Security.Cryptography/System.Security.Cryptography.Shake128::HashData:System.Byte[](System.Byte[],System.Int32)", "codegen", "System.Security.Cryptography.Shake128", "HashData", True, "benchmark" },
    { 1182, "System.Security.Cryptography/System.Security.Cryptography.Shake128::HashData:System.Byte[](System.Byte[],System.Int32)", "codegen", "System.Security.Cryptography.Shake128", "HashData", True, "benchmark" },
    { 1183, "System.Security.Cryptography/System.Security.Cryptography.Shake128::HashData:System.Byte[](System.IO.Stream,System.Int32)", "codegen", "System.Security.Cryptography.Shake128", "HashData", True, "benchmark" },
    { 1184, "System.Security.Cryptography/System.Security.Cryptography.Shake128::HashData:System.Byte[](System.IO.Stream,System.Int32)", "codegen", "System.Security.Cryptography.Shake128", "HashData", True, "benchmark" },
    { 1185, "System.Security.Cryptography/System.Security.Cryptography.Shake128::HashData:System.Byte[](System.IO.Stream,System.Int32)", "codegen", "System.Security.Cryptography.Shake128", "HashData", True, "benchmark" },
    { 1186, "System.Security.Cryptography/System.Security.Cryptography.Shake128::HashDataAsync:System.Threading.Tasks.ValueTask(System.IO.Stream,System.Memory<System.Byte>,System.Threading.CancellationToken)", "codegen", "System.Security.Cryptography.Shake128", "HashDataAsync", True, "benchmark" },
    { 1187, "System.Security.Cryptography/System.Security.Cryptography.Shake128::HashDataAsync:System.Threading.Tasks.ValueTask(System.IO.Stream,System.Memory<System.Byte>,System.Threading.CancellationToken)", "codegen", "System.Security.Cryptography.Shake128", "HashDataAsync", True, "benchmark" },
    { 1188, "System.Security.Cryptography/System.Security.Cryptography.Shake128::HashDataAsync:System.Threading.Tasks.ValueTask<System.Byte[]>(System.IO.Stream,System.Int32,System.Threading.CancellationToken)", "codegen", "System.Security.Cryptography.Shake128", "HashDataAsync", True, "benchmark" },
    { 1189, "System.Security.Cryptography/System.Security.Cryptography.Shake128::HashDataAsync:System.Threading.Tasks.ValueTask<System.Byte[]>(System.IO.Stream,System.Int32,System.Threading.CancellationToken)", "codegen", "System.Security.Cryptography.Shake128", "HashDataAsync", True, "benchmark" },
    { 1190, "System.Security.Cryptography/System.Security.Cryptography.Shake128::HashDataAsync:System.Threading.Tasks.ValueTask<System.Byte[]>(System.IO.Stream,System.Int32,System.Threading.CancellationToken)", "codegen", "System.Security.Cryptography.Shake128", "HashDataAsync", True, "benchmark" },
    { 1191, "System.Security.Cryptography/System.Security.Cryptography.Shake256::AppendData:System.Void(System.Byte[])", "codegen", "System.Security.Cryptography.Shake256", "AppendData", True, "benchmark" },
    { 1192, "System.Security.Cryptography/System.Security.Cryptography.Shake256::AppendData:System.Void(System.Byte[])", "codegen", "System.Security.Cryptography.Shake256", "AppendData", True, "benchmark" },
    { 1193, "System.Security.Cryptography/System.Security.Cryptography.Shake256::AppendData:System.Void(System.Byte[])", "codegen", "System.Security.Cryptography.Shake256", "AppendData", True, "benchmark" },
    { 1194, "System.Security.Cryptography/System.Security.Cryptography.Shake256::GetHashAndReset:System.Byte[](System.Int32)", "codegen", "System.Security.Cryptography.Shake256", "GetHashAndReset", True, "benchmark" },
    { 1195, "System.Security.Cryptography/System.Security.Cryptography.Shake256::GetHashAndReset:System.Byte[](System.Int32)", "codegen", "System.Security.Cryptography.Shake256", "GetHashAndReset", True, "benchmark" },
    { 1196, "System.Security.Cryptography/System.Security.Cryptography.Shake256::GetCurrentHash:System.Byte[](System.Int32)", "codegen", "System.Security.Cryptography.Shake256", "GetCurrentHash", True, "benchmark" },
    { 1197, "System.Security.Cryptography/System.Security.Cryptography.Shake256::GetCurrentHash:System.Byte[](System.Int32)", "codegen", "System.Security.Cryptography.Shake256", "GetCurrentHash", True, "benchmark" },
    { 1198, "System.Security.Cryptography/System.Security.Cryptography.Shake256::Read:System.Byte[](System.Int32)", "codegen", "System.Security.Cryptography.Shake256", "Read", True, "benchmark" },
    { 1199, "System.Security.Cryptography/System.Security.Cryptography.Shake256::Read:System.Byte[](System.Int32)", "codegen", "System.Security.Cryptography.Shake256", "Read", True, "benchmark" },
    { 1200, "System.Security.Cryptography/System.Security.Cryptography.Shake256::Clone:System.Security.Cryptography.Shake256()", "codegen", "System.Security.Cryptography.Shake256", "Clone", True, "benchmark" },
    { 1201, "System.Security.Cryptography/System.Security.Cryptography.Shake256::Dispose:System.Void()", "codegen", "System.Security.Cryptography.Shake256", "Dispose", True, "benchmark" },
    { 1202, "System.Security.Cryptography/System.Security.Cryptography.Shake256::HashData:System.Byte[](System.Byte[],System.Int32)", "codegen", "System.Security.Cryptography.Shake256", "HashData", True, "benchmark" },
    { 1203, "System.Security.Cryptography/System.Security.Cryptography.Shake256::HashData:System.Byte[](System.Byte[],System.Int32)", "codegen", "System.Security.Cryptography.Shake256", "HashData", True, "benchmark" },
    { 1204, "System.Security.Cryptography/System.Security.Cryptography.Shake256::HashData:System.Byte[](System.Byte[],System.Int32)", "codegen", "System.Security.Cryptography.Shake256", "HashData", True, "benchmark" },
    { 1205, "System.Security.Cryptography/System.Security.Cryptography.Shake256::HashData:System.Byte[](System.Byte[],System.Int32)", "codegen", "System.Security.Cryptography.Shake256", "HashData", True, "benchmark" },
    { 1206, "System.Security.Cryptography/System.Security.Cryptography.Shake256::HashData:System.Byte[](System.IO.Stream,System.Int32)", "codegen", "System.Security.Cryptography.Shake256", "HashData", True, "benchmark" },
    { 1207, "System.Security.Cryptography/System.Security.Cryptography.Shake256::HashData:System.Byte[](System.IO.Stream,System.Int32)", "codegen", "System.Security.Cryptography.Shake256", "HashData", True, "benchmark" },
    { 1208, "System.Security.Cryptography/System.Security.Cryptography.Shake256::HashData:System.Byte[](System.IO.Stream,System.Int32)", "codegen", "System.Security.Cryptography.Shake256", "HashData", True, "benchmark" },
    { 1209, "System.Security.Cryptography/System.Security.Cryptography.Shake256::HashDataAsync:System.Threading.Tasks.ValueTask(System.IO.Stream,System.Memory<System.Byte>,System.Threading.CancellationToken)", "codegen", "System.Security.Cryptography.Shake256", "HashDataAsync", True, "benchmark" },
    { 1210, "System.Security.Cryptography/System.Security.Cryptography.Shake256::HashDataAsync:System.Threading.Tasks.ValueTask(System.IO.Stream,System.Memory<System.Byte>,System.Threading.CancellationToken)", "codegen", "System.Security.Cryptography.Shake256", "HashDataAsync", True, "benchmark" },
    { 1211, "System.Security.Cryptography/System.Security.Cryptography.Shake256::HashDataAsync:System.Threading.Tasks.ValueTask<System.Byte[]>(System.IO.Stream,System.Int32,System.Threading.CancellationToken)", "codegen", "System.Security.Cryptography.Shake256", "HashDataAsync", True, "benchmark" },
    { 1212, "System.Security.Cryptography/System.Security.Cryptography.Shake256::HashDataAsync:System.Threading.Tasks.ValueTask<System.Byte[]>(System.IO.Stream,System.Int32,System.Threading.CancellationToken)", "codegen", "System.Security.Cryptography.Shake256", "HashDataAsync", True, "benchmark" },
    { 1213, "System.Security.Cryptography/System.Security.Cryptography.Shake256::HashDataAsync:System.Threading.Tasks.ValueTask<System.Byte[]>(System.IO.Stream,System.Int32,System.Threading.CancellationToken)", "codegen", "System.Security.Cryptography.Shake256", "HashDataAsync", True, "benchmark" },
    { 1214, "System.Security.Cryptography/System.Security.Cryptography.SignatureDescription::CreateDeformatter:System.Security.Cryptography.AsymmetricSignatureDeformatter(System.Security.Cryptography.AsymmetricAlgorithm)", "codegen", "System.Security.Cryptography.SignatureDescription", "CreateDeformatter", True, "benchmark" },
    { 1215, "System.Security.Cryptography/System.Security.Cryptography.SignatureDescription::CreateFormatter:System.Security.Cryptography.AsymmetricSignatureFormatter(System.Security.Cryptography.AsymmetricAlgorithm)", "codegen", "System.Security.Cryptography.SignatureDescription", "CreateFormatter", True, "benchmark" },
    { 1216, "System.Security.Cryptography/System.Security.Cryptography.SignatureDescription::CreateDigest:System.Security.Cryptography.HashAlgorithm()", "codegen", "System.Security.Cryptography.SignatureDescription", "CreateDigest", True, "benchmark" },
    { 1217, "System.Security.Cryptography/System.Security.Cryptography.SP800108HmacCounterKdf::DeriveBytes:System.Byte[](System.Byte[],System.Security.Cryptography.HashAlgorithmName,System.Byte[],System.Byte[],System.Int32)", "codegen", "System.Security.Cryptography.SP800108HmacCounterKdf", "DeriveBytes", True, "benchmark" },
    { 1218, "System.Security.Cryptography/System.Security.Cryptography.SP800108HmacCounterKdf::DeriveBytes:System.Byte[](System.Byte[],System.Security.Cryptography.HashAlgorithmName,System.Byte[],System.Byte[],System.Int32)", "codegen", "System.Security.Cryptography.SP800108HmacCounterKdf", "DeriveBytes", True, "benchmark" },
    { 1219, "System.Security.Cryptography/System.Security.Cryptography.SP800108HmacCounterKdf::DeriveBytes:System.Byte[](System.Byte[],System.Security.Cryptography.HashAlgorithmName,System.Byte[],System.Byte[],System.Int32)", "codegen", "System.Security.Cryptography.SP800108HmacCounterKdf", "DeriveBytes", True, "benchmark" },
    { 1220, "System.Security.Cryptography/System.Security.Cryptography.SP800108HmacCounterKdf::DeriveBytes:System.Byte[](System.Byte[],System.Security.Cryptography.HashAlgorithmName,System.String,System.String,System.Int32)", "codegen", "System.Security.Cryptography.SP800108HmacCounterKdf", "DeriveBytes", True, "benchmark" },
    { 1221, "System.Security.Cryptography/System.Security.Cryptography.SP800108HmacCounterKdf::DeriveBytes:System.Byte[](System.Byte[],System.Security.Cryptography.HashAlgorithmName,System.String,System.String,System.Int32)", "codegen", "System.Security.Cryptography.SP800108HmacCounterKdf", "DeriveBytes", True, "benchmark" },
    { 1222, "System.Security.Cryptography/System.Security.Cryptography.SP800108HmacCounterKdf::DeriveBytes:System.Byte[](System.Byte[],System.Security.Cryptography.HashAlgorithmName,System.String,System.String,System.Int32)", "codegen", "System.Security.Cryptography.SP800108HmacCounterKdf", "DeriveBytes", True, "benchmark" },
    { 1223, "System.Security.Cryptography/System.Security.Cryptography.SP800108HmacCounterKdf::DeriveBytes:System.Byte[](System.Byte[],System.Security.Cryptography.HashAlgorithmName,System.String,System.String,System.Int32)", "codegen", "System.Security.Cryptography.SP800108HmacCounterKdf", "DeriveBytes", True, "benchmark" },
    { 1224, "System.Security.Cryptography/System.Security.Cryptography.SP800108HmacCounterKdf::DeriveKey:System.Byte[](System.Byte[],System.Byte[],System.Int32)", "codegen", "System.Security.Cryptography.SP800108HmacCounterKdf", "DeriveKey", True, "benchmark" },
    { 1225, "System.Security.Cryptography/System.Security.Cryptography.SP800108HmacCounterKdf::DeriveKey:System.Byte[](System.Byte[],System.Byte[],System.Int32)", "codegen", "System.Security.Cryptography.SP800108HmacCounterKdf", "DeriveKey", True, "benchmark" },
    { 1226, "System.Security.Cryptography/System.Security.Cryptography.SP800108HmacCounterKdf::DeriveKey:System.Byte[](System.Byte[],System.Byte[],System.Int32)", "codegen", "System.Security.Cryptography.SP800108HmacCounterKdf", "DeriveKey", True, "benchmark" },
    { 1227, "System.Security.Cryptography/System.Security.Cryptography.SP800108HmacCounterKdf::DeriveKey:System.Byte[](System.Byte[],System.Byte[],System.Int32)", "codegen", "System.Security.Cryptography.SP800108HmacCounterKdf", "DeriveKey", True, "benchmark" },
    { 1228, "System.Security.Cryptography/System.Security.Cryptography.SP800108HmacCounterKdf::DeriveKey:System.Byte[](System.String,System.String,System.Int32)", "codegen", "System.Security.Cryptography.SP800108HmacCounterKdf", "DeriveKey", True, "benchmark" },
    { 1229, "System.Security.Cryptography/System.Security.Cryptography.SP800108HmacCounterKdf::DeriveKey:System.Byte[](System.String,System.String,System.Int32)", "codegen", "System.Security.Cryptography.SP800108HmacCounterKdf", "DeriveKey", True, "benchmark" },
    { 1230, "System.Security.Cryptography/System.Security.Cryptography.SP800108HmacCounterKdf::DeriveKey:System.Byte[](System.String,System.String,System.Int32)", "codegen", "System.Security.Cryptography.SP800108HmacCounterKdf", "DeriveKey", True, "benchmark" },
    { 1231, "System.Security.Cryptography/System.Security.Cryptography.SP800108HmacCounterKdf::DeriveKey:System.Byte[](System.String,System.String,System.Int32)", "codegen", "System.Security.Cryptography.SP800108HmacCounterKdf", "DeriveKey", True, "benchmark" },
    { 1232, "System.Security.Cryptography/System.Security.Cryptography.SP800108HmacCounterKdf::Dispose:System.Void()", "codegen", "System.Security.Cryptography.SP800108HmacCounterKdf", "Dispose", True, "benchmark" },
    { 1233, "System.Security.Cryptography/System.Security.Cryptography.SymmetricAlgorithm::CreateDecryptor:System.Security.Cryptography.ICryptoTransform()", "codegen", "System.Security.Cryptography.SymmetricAlgorithm", "CreateDecryptor", True, "benchmark" },
    { 1234, "System.Security.Cryptography/System.Security.Cryptography.SymmetricAlgorithm::CreateDecryptor:System.Security.Cryptography.ICryptoTransform(System.Byte[],System.Byte[])", "codegen", "System.Security.Cryptography.SymmetricAlgorithm", "CreateDecryptor", True, "benchmark" },
    { 1235, "System.Security.Cryptography/System.Security.Cryptography.SymmetricAlgorithm::CreateDecryptor:System.Security.Cryptography.ICryptoTransform(System.Byte[],System.Byte[])", "codegen", "System.Security.Cryptography.SymmetricAlgorithm", "CreateDecryptor", True, "benchmark" },
    { 1236, "System.Security.Cryptography/System.Security.Cryptography.SymmetricAlgorithm::CreateDecryptor:System.Security.Cryptography.ICryptoTransform(System.Byte[],System.Byte[])", "codegen", "System.Security.Cryptography.SymmetricAlgorithm", "CreateDecryptor", True, "benchmark" },
    { 1237, "System.Security.Cryptography/System.Security.Cryptography.SymmetricAlgorithm::CreateEncryptor:System.Security.Cryptography.ICryptoTransform()", "codegen", "System.Security.Cryptography.SymmetricAlgorithm", "CreateEncryptor", True, "benchmark" },
    { 1238, "System.Security.Cryptography/System.Security.Cryptography.SymmetricAlgorithm::CreateEncryptor:System.Security.Cryptography.ICryptoTransform(System.Byte[],System.Byte[])", "codegen", "System.Security.Cryptography.SymmetricAlgorithm", "CreateEncryptor", True, "benchmark" },
    { 1239, "System.Security.Cryptography/System.Security.Cryptography.SymmetricAlgorithm::CreateEncryptor:System.Security.Cryptography.ICryptoTransform(System.Byte[],System.Byte[])", "codegen", "System.Security.Cryptography.SymmetricAlgorithm", "CreateEncryptor", True, "benchmark" },
    { 1240, "System.Security.Cryptography/System.Security.Cryptography.SymmetricAlgorithm::CreateEncryptor:System.Security.Cryptography.ICryptoTransform(System.Byte[],System.Byte[])", "codegen", "System.Security.Cryptography.SymmetricAlgorithm", "CreateEncryptor", True, "benchmark" },
    { 1241, "System.Security.Cryptography/System.Security.Cryptography.SymmetricAlgorithm::Dispose:System.Void()", "codegen", "System.Security.Cryptography.SymmetricAlgorithm", "Dispose", True, "benchmark" },
    { 1242, "System.Security.Cryptography/System.Security.Cryptography.SymmetricAlgorithm::Clear:System.Void()", "codegen", "System.Security.Cryptography.SymmetricAlgorithm", "Clear", True, "benchmark" },
    { 1243, "System.Security.Cryptography/System.Security.Cryptography.SymmetricAlgorithm::GenerateIV:System.Void()", "codegen", "System.Security.Cryptography.SymmetricAlgorithm", "GenerateIV", True, "benchmark" },
    { 1244, "System.Security.Cryptography/System.Security.Cryptography.SymmetricAlgorithm::GenerateKey:System.Void()", "codegen", "System.Security.Cryptography.SymmetricAlgorithm", "GenerateKey", True, "benchmark" },
    { 1245, "System.Security.Cryptography/System.Security.Cryptography.SymmetricAlgorithm::ValidKeySize:System.Boolean(System.Int32)", "codegen", "System.Security.Cryptography.SymmetricAlgorithm", "ValidKeySize", True, "benchmark" },
    { 1246, "System.Security.Cryptography/System.Security.Cryptography.SymmetricAlgorithm::ValidKeySize:System.Boolean(System.Int32)", "codegen", "System.Security.Cryptography.SymmetricAlgorithm", "ValidKeySize", True, "benchmark" },
    { 1247, "System.Security.Cryptography/System.Security.Cryptography.SymmetricAlgorithm::GetCiphertextLengthEcb:System.Int32(System.Int32,System.Security.Cryptography.PaddingMode)", "codegen", "System.Security.Cryptography.SymmetricAlgorithm", "GetCiphertextLengthEcb", True, "benchmark" },
    { 1248, "System.Security.Cryptography/System.Security.Cryptography.SymmetricAlgorithm::GetCiphertextLengthEcb:System.Int32(System.Int32,System.Security.Cryptography.PaddingMode)", "codegen", "System.Security.Cryptography.SymmetricAlgorithm", "GetCiphertextLengthEcb", True, "benchmark" },
    { 1249, "System.Security.Cryptography/System.Security.Cryptography.SymmetricAlgorithm::GetCiphertextLengthCbc:System.Int32(System.Int32,System.Security.Cryptography.PaddingMode)", "codegen", "System.Security.Cryptography.SymmetricAlgorithm", "GetCiphertextLengthCbc", True, "benchmark" },
    { 1250, "System.Security.Cryptography/System.Security.Cryptography.SymmetricAlgorithm::GetCiphertextLengthCbc:System.Int32(System.Int32,System.Security.Cryptography.PaddingMode)", "codegen", "System.Security.Cryptography.SymmetricAlgorithm", "GetCiphertextLengthCbc", True, "benchmark" },
    { 1251, "System.Security.Cryptography/System.Security.Cryptography.SymmetricAlgorithm::GetCiphertextLengthCfb:System.Int32(System.Int32,System.Security.Cryptography.PaddingMode,System.Int32)", "codegen", "System.Security.Cryptography.SymmetricAlgorithm", "GetCiphertextLengthCfb", True, "benchmark" },
    { 1252, "System.Security.Cryptography/System.Security.Cryptography.SymmetricAlgorithm::GetCiphertextLengthCfb:System.Int32(System.Int32,System.Security.Cryptography.PaddingMode,System.Int32)", "codegen", "System.Security.Cryptography.SymmetricAlgorithm", "GetCiphertextLengthCfb", True, "benchmark" },
    { 1253, "System.Security.Cryptography/System.Security.Cryptography.SymmetricAlgorithm::GetCiphertextLengthCfb:System.Int32(System.Int32,System.Security.Cryptography.PaddingMode,System.Int32)", "codegen", "System.Security.Cryptography.SymmetricAlgorithm", "GetCiphertextLengthCfb", True, "benchmark" },
    { 1254, "System.Security.Cryptography/System.Security.Cryptography.SymmetricAlgorithm::DecryptEcb:System.Byte[](System.Byte[],System.Security.Cryptography.PaddingMode)", "codegen", "System.Security.Cryptography.SymmetricAlgorithm", "DecryptEcb", True, "benchmark" },
    { 1255, "System.Security.Cryptography/System.Security.Cryptography.SymmetricAlgorithm::DecryptEcb:System.Byte[](System.Byte[],System.Security.Cryptography.PaddingMode)", "codegen", "System.Security.Cryptography.SymmetricAlgorithm", "DecryptEcb", True, "benchmark" },
    { 1256, "System.Security.Cryptography/System.Security.Cryptography.SymmetricAlgorithm::DecryptEcb:System.Byte[](System.Byte[],System.Security.Cryptography.PaddingMode)", "codegen", "System.Security.Cryptography.SymmetricAlgorithm", "DecryptEcb", True, "benchmark" },
    { 1257, "System.Security.Cryptography/System.Security.Cryptography.SymmetricAlgorithm::EncryptEcb:System.Byte[](System.Byte[],System.Security.Cryptography.PaddingMode)", "codegen", "System.Security.Cryptography.SymmetricAlgorithm", "EncryptEcb", True, "benchmark" },
    { 1258, "System.Security.Cryptography/System.Security.Cryptography.SymmetricAlgorithm::EncryptEcb:System.Byte[](System.Byte[],System.Security.Cryptography.PaddingMode)", "codegen", "System.Security.Cryptography.SymmetricAlgorithm", "EncryptEcb", True, "benchmark" },
    { 1259, "System.Security.Cryptography/System.Security.Cryptography.SymmetricAlgorithm::EncryptEcb:System.Byte[](System.Byte[],System.Security.Cryptography.PaddingMode)", "codegen", "System.Security.Cryptography.SymmetricAlgorithm", "EncryptEcb", True, "benchmark" },
    { 1260, "System.Security.Cryptography/System.Security.Cryptography.SymmetricAlgorithm::DecryptCbc:System.Byte[](System.Byte[],System.Byte[],System.Security.Cryptography.PaddingMode)", "codegen", "System.Security.Cryptography.SymmetricAlgorithm", "DecryptCbc", True, "benchmark" },
    { 1261, "System.Security.Cryptography/System.Security.Cryptography.SymmetricAlgorithm::DecryptCbc:System.Byte[](System.Byte[],System.Byte[],System.Security.Cryptography.PaddingMode)", "codegen", "System.Security.Cryptography.SymmetricAlgorithm", "DecryptCbc", True, "benchmark" },
    { 1262, "System.Security.Cryptography/System.Security.Cryptography.SymmetricAlgorithm::DecryptCbc:System.Byte[](System.Byte[],System.Byte[],System.Security.Cryptography.PaddingMode)", "codegen", "System.Security.Cryptography.SymmetricAlgorithm", "DecryptCbc", True, "benchmark" },
    { 1263, "System.Security.Cryptography/System.Security.Cryptography.SymmetricAlgorithm::EncryptCbc:System.Byte[](System.Byte[],System.Byte[],System.Security.Cryptography.PaddingMode)", "codegen", "System.Security.Cryptography.SymmetricAlgorithm", "EncryptCbc", True, "benchmark" },
    { 1264, "System.Security.Cryptography/System.Security.Cryptography.SymmetricAlgorithm::EncryptCbc:System.Byte[](System.Byte[],System.Byte[],System.Security.Cryptography.PaddingMode)", "codegen", "System.Security.Cryptography.SymmetricAlgorithm", "EncryptCbc", True, "benchmark" },
    { 1265, "System.Security.Cryptography/System.Security.Cryptography.SymmetricAlgorithm::EncryptCbc:System.Byte[](System.Byte[],System.Byte[],System.Security.Cryptography.PaddingMode)", "codegen", "System.Security.Cryptography.SymmetricAlgorithm", "EncryptCbc", True, "benchmark" },
    { 1266, "System.Security.Cryptography/System.Security.Cryptography.SymmetricAlgorithm::DecryptCfb:System.Byte[](System.Byte[],System.Byte[],System.Security.Cryptography.PaddingMode,System.Int32)", "codegen", "System.Security.Cryptography.SymmetricAlgorithm", "DecryptCfb", True, "benchmark" },
    { 1267, "System.Security.Cryptography/System.Security.Cryptography.SymmetricAlgorithm::DecryptCfb:System.Byte[](System.Byte[],System.Byte[],System.Security.Cryptography.PaddingMode,System.Int32)", "codegen", "System.Security.Cryptography.SymmetricAlgorithm", "DecryptCfb", True, "benchmark" },
    { 1268, "System.Security.Cryptography/System.Security.Cryptography.SymmetricAlgorithm::DecryptCfb:System.Byte[](System.Byte[],System.Byte[],System.Security.Cryptography.PaddingMode,System.Int32)", "codegen", "System.Security.Cryptography.SymmetricAlgorithm", "DecryptCfb", True, "benchmark" },
    { 1269, "System.Security.Cryptography/System.Security.Cryptography.SymmetricAlgorithm::EncryptCfb:System.Byte[](System.Byte[],System.Byte[],System.Security.Cryptography.PaddingMode,System.Int32)", "codegen", "System.Security.Cryptography.SymmetricAlgorithm", "EncryptCfb", True, "benchmark" },
    { 1270, "System.Security.Cryptography/System.Security.Cryptography.SymmetricAlgorithm::EncryptCfb:System.Byte[](System.Byte[],System.Byte[],System.Security.Cryptography.PaddingMode,System.Int32)", "codegen", "System.Security.Cryptography.SymmetricAlgorithm", "EncryptCfb", True, "benchmark" },
    { 1271, "System.Security.Cryptography/System.Security.Cryptography.SymmetricAlgorithm::EncryptCfb:System.Byte[](System.Byte[],System.Byte[],System.Security.Cryptography.PaddingMode,System.Int32)", "codegen", "System.Security.Cryptography.SymmetricAlgorithm", "EncryptCfb", True, "benchmark" },
    { 1272, "System.Security.Cryptography/System.Security.Cryptography.ToBase64Transform::TransformBlock:System.Int32(System.Byte[],System.Int32,System.Int32,System.Byte[],System.Int32)", "codegen", "System.Security.Cryptography.ToBase64Transform", "TransformBlock", True, "benchmark" },
    { 1273, "System.Security.Cryptography/System.Security.Cryptography.ToBase64Transform::TransformBlock:System.Int32(System.Byte[],System.Int32,System.Int32,System.Byte[],System.Int32)", "codegen", "System.Security.Cryptography.ToBase64Transform", "TransformBlock", True, "benchmark" },
    { 1274, "System.Security.Cryptography/System.Security.Cryptography.ToBase64Transform::TransformBlock:System.Int32(System.Byte[],System.Int32,System.Int32,System.Byte[],System.Int32)", "codegen", "System.Security.Cryptography.ToBase64Transform", "TransformBlock", True, "benchmark" },
    { 1275, "System.Security.Cryptography/System.Security.Cryptography.ToBase64Transform::TransformBlock:System.Int32(System.Byte[],System.Int32,System.Int32,System.Byte[],System.Int32)", "codegen", "System.Security.Cryptography.ToBase64Transform", "TransformBlock", True, "benchmark" },
    { 1276, "System.Security.Cryptography/System.Security.Cryptography.ToBase64Transform::TransformBlock:System.Int32(System.Byte[],System.Int32,System.Int32,System.Byte[],System.Int32)", "codegen", "System.Security.Cryptography.ToBase64Transform", "TransformBlock", True, "benchmark" },
    { 1277, "System.Security.Cryptography/System.Security.Cryptography.ToBase64Transform::TransformFinalBlock:System.Byte[](System.Byte[],System.Int32,System.Int32)", "codegen", "System.Security.Cryptography.ToBase64Transform", "TransformFinalBlock", True, "benchmark" },
    { 1278, "System.Security.Cryptography/System.Security.Cryptography.ToBase64Transform::TransformFinalBlock:System.Byte[](System.Byte[],System.Int32,System.Int32)", "codegen", "System.Security.Cryptography.ToBase64Transform", "TransformFinalBlock", True, "benchmark" },
    { 1279, "System.Security.Cryptography/System.Security.Cryptography.ToBase64Transform::TransformFinalBlock:System.Byte[](System.Byte[],System.Int32,System.Int32)", "codegen", "System.Security.Cryptography.ToBase64Transform", "TransformFinalBlock", True, "benchmark" },
    { 1280, "System.Security.Cryptography/System.Security.Cryptography.ToBase64Transform::TransformFinalBlock:System.Byte[](System.Byte[],System.Int32,System.Int32)", "codegen", "System.Security.Cryptography.ToBase64Transform", "TransformFinalBlock", True, "benchmark" },
    { 1281, "System.Security.Cryptography/System.Security.Cryptography.ToBase64Transform::TransformFinalBlock:System.Byte[](System.Byte[],System.Int32,System.Int32)", "codegen", "System.Security.Cryptography.ToBase64Transform", "TransformFinalBlock", True, "benchmark" },
    { 1282, "System.Security.Cryptography/System.Security.Cryptography.ToBase64Transform::Dispose:System.Void()", "codegen", "System.Security.Cryptography.ToBase64Transform", "Dispose", True, "benchmark" },
    { 1283, "System.Security.Cryptography/System.Security.Cryptography.ToBase64Transform::Clear:System.Void()", "codegen", "System.Security.Cryptography.ToBase64Transform", "Clear", True, "benchmark" },
    { 1284, "System.Security.Cryptography/System.Security.Cryptography.TripleDES::IsWeakKey:System.Boolean(System.Byte[])", "codegen", "System.Security.Cryptography.TripleDES", "IsWeakKey", True, "benchmark" },
    { 1285, "System.Security.Cryptography/System.Security.Cryptography.TripleDES::IsWeakKey:System.Boolean(System.Byte[])", "codegen", "System.Security.Cryptography.TripleDES", "IsWeakKey", True, "benchmark" },
    { 1286, "System.Security.Cryptography/System.Security.Cryptography.TripleDES::IsWeakKey:System.Boolean(System.Byte[])", "codegen", "System.Security.Cryptography.TripleDES", "IsWeakKey", True, "benchmark" },
    { 1287, "System.Security.Cryptography/System.Security.Cryptography.TripleDESCng::CreateDecryptor:System.Security.Cryptography.ICryptoTransform()", "codegen", "System.Security.Cryptography.TripleDESCng", "CreateDecryptor", True, "benchmark" },
    { 1288, "System.Security.Cryptography/System.Security.Cryptography.TripleDESCng::CreateDecryptor:System.Security.Cryptography.ICryptoTransform(System.Byte[],System.Byte[])", "codegen", "System.Security.Cryptography.TripleDESCng", "CreateDecryptor", True, "benchmark" },
    { 1289, "System.Security.Cryptography/System.Security.Cryptography.TripleDESCng::CreateDecryptor:System.Security.Cryptography.ICryptoTransform(System.Byte[],System.Byte[])", "codegen", "System.Security.Cryptography.TripleDESCng", "CreateDecryptor", True, "benchmark" },
    { 1290, "System.Security.Cryptography/System.Security.Cryptography.TripleDESCng::CreateDecryptor:System.Security.Cryptography.ICryptoTransform(System.Byte[],System.Byte[])", "codegen", "System.Security.Cryptography.TripleDESCng", "CreateDecryptor", True, "benchmark" },
    { 1291, "System.Security.Cryptography/System.Security.Cryptography.TripleDESCng::CreateEncryptor:System.Security.Cryptography.ICryptoTransform()", "codegen", "System.Security.Cryptography.TripleDESCng", "CreateEncryptor", True, "benchmark" },
    { 1292, "System.Security.Cryptography/System.Security.Cryptography.TripleDESCng::CreateEncryptor:System.Security.Cryptography.ICryptoTransform(System.Byte[],System.Byte[])", "codegen", "System.Security.Cryptography.TripleDESCng", "CreateEncryptor", True, "benchmark" },
    { 1293, "System.Security.Cryptography/System.Security.Cryptography.TripleDESCng::CreateEncryptor:System.Security.Cryptography.ICryptoTransform(System.Byte[],System.Byte[])", "codegen", "System.Security.Cryptography.TripleDESCng", "CreateEncryptor", True, "benchmark" },
    { 1294, "System.Security.Cryptography/System.Security.Cryptography.TripleDESCng::CreateEncryptor:System.Security.Cryptography.ICryptoTransform(System.Byte[],System.Byte[])", "codegen", "System.Security.Cryptography.TripleDESCng", "CreateEncryptor", True, "benchmark" },
    { 1295, "System.Security.Cryptography/System.Security.Cryptography.TripleDESCng::GenerateKey:System.Void()", "codegen", "System.Security.Cryptography.TripleDESCng", "GenerateKey", True, "benchmark" },
    { 1296, "System.Security.Cryptography/System.Security.Cryptography.TripleDESCng::GenerateIV:System.Void()", "codegen", "System.Security.Cryptography.TripleDESCng", "GenerateIV", True, "benchmark" },
    { 1297, "System.Security.Cryptography/System.Security.Cryptography.TripleDESCryptoServiceProvider::CreateEncryptor:System.Security.Cryptography.ICryptoTransform()", "codegen", "System.Security.Cryptography.TripleDESCryptoServiceProvider", "CreateEncryptor", True, "benchmark" },
    { 1298, "System.Security.Cryptography/System.Security.Cryptography.TripleDESCryptoServiceProvider::CreateDecryptor:System.Security.Cryptography.ICryptoTransform()", "codegen", "System.Security.Cryptography.TripleDESCryptoServiceProvider", "CreateDecryptor", True, "benchmark" },
    { 1299, "System.Security.Cryptography/System.Security.Cryptography.TripleDESCryptoServiceProvider::GenerateIV:System.Void()", "codegen", "System.Security.Cryptography.TripleDESCryptoServiceProvider", "GenerateIV", True, "benchmark" },
    { 1300, "System.Security.Cryptography/System.Security.Cryptography.TripleDESCryptoServiceProvider::GenerateKey:System.Void()", "codegen", "System.Security.Cryptography.TripleDESCryptoServiceProvider", "GenerateKey", True, "benchmark" },
    { 1301, "System.Security.Cryptography/System.Security.Cryptography.TripleDESCryptoServiceProvider::CreateEncryptor:System.Security.Cryptography.ICryptoTransform(System.Byte[],System.Byte[])", "codegen", "System.Security.Cryptography.TripleDESCryptoServiceProvider", "CreateEncryptor", True, "benchmark" },
    { 1302, "System.Security.Cryptography/System.Security.Cryptography.TripleDESCryptoServiceProvider::CreateEncryptor:System.Security.Cryptography.ICryptoTransform(System.Byte[],System.Byte[])", "codegen", "System.Security.Cryptography.TripleDESCryptoServiceProvider", "CreateEncryptor", True, "benchmark" },
    { 1303, "System.Security.Cryptography/System.Security.Cryptography.TripleDESCryptoServiceProvider::CreateEncryptor:System.Security.Cryptography.ICryptoTransform(System.Byte[],System.Byte[])", "codegen", "System.Security.Cryptography.TripleDESCryptoServiceProvider", "CreateEncryptor", True, "benchmark" },
    { 1304, "System.Security.Cryptography/System.Security.Cryptography.TripleDESCryptoServiceProvider::CreateDecryptor:System.Security.Cryptography.ICryptoTransform(System.Byte[],System.Byte[])", "codegen", "System.Security.Cryptography.TripleDESCryptoServiceProvider", "CreateDecryptor", True, "benchmark" },
    { 1305, "System.Security.Cryptography/System.Security.Cryptography.TripleDESCryptoServiceProvider::CreateDecryptor:System.Security.Cryptography.ICryptoTransform(System.Byte[],System.Byte[])", "codegen", "System.Security.Cryptography.TripleDESCryptoServiceProvider", "CreateDecryptor", True, "benchmark" },
    { 1306, "System.Security.Cryptography/System.Security.Cryptography.TripleDESCryptoServiceProvider::CreateDecryptor:System.Security.Cryptography.ICryptoTransform(System.Byte[],System.Byte[])", "codegen", "System.Security.Cryptography.TripleDESCryptoServiceProvider", "CreateDecryptor", True, "benchmark" },

};

static const int kSubjectEntryCount_Generated = 1307;

static chaos::il2cpp::runtime_core::PatchContext* ApplyHotpatchIfAvailable() {
    if (kPatchDataSize > 0u) {
        auto* patch_ctx = chaos::il2cpp::runtime_core::ApplyPatchFromMemory(
            kPatchData, kPatchDataSize, kPatchDataHostClassName);
        if (patch_ctx == nullptr) {
            std::fprintf(stderr, "WARN: ApplyPatchFromMemory returned null (no patches applied)\n");
        } else {
            std::fprintf(stderr, "INFO: Applied patches to %u methods\n",
                         static_cast<unsigned>(patch_ctx->method_count));
        }
        return patch_ctx;
    }
    return nullptr;
}

static int RunFactMode() {
    const int kCount = kSubjectEntryCount;
    int passed_count = 0;
    for (int si = 0; si < kCount; si++) {
        int i = kSubjectSlotMap[si];
{% if is_jit %}
        try {
            chaos::il2cpp::runtime_core::ChaosDispatchMethod(
                GetHotpatchEntries(), kAotMethodCount, i, CHAOS_USE_DEFAULT_THUNKS);
        } catch(...) {
            // caught — skip increment
        }
{% else %}
        CHAOS_EH_TRY
            chaos::il2cpp::runtime_core::ChaosDispatchMethod(
                GetHotpatchEntries(), kAotMethodCount, i, CHAOS_USE_DEFAULT_THUNKS);
            ++passed_count;
        CHAOS_EH_CATCH_BEGIN
            // caught — skip increment
        CHAOS_EH_END
{% endif %}
    }
    int failed_count = kCount - passed_count;
    printf("Passed: %d/%d\n", passed_count, kSubjectEntryCount);
    std::fflush(stdout);
    return failed_count;
}

static int RunFactJsonMode() {
    const int kCount = kSubjectEntryCount;
    printf("{\"factResults\":[");
    bool first = true;
    for (int si = 0; si < kCount; si++) {
        int i = kSubjectSlotMap[si];
        int64_t result = 0;
        bool caught = false;
{% if is_jit %}
        try {
            result = chaos::il2cpp::runtime_core::ChaosDispatchMethodGetValue(
                GetHotpatchEntries(), kAotMethodCount, i, CHAOS_USE_DEFAULT_THUNKS);
        } catch(...) {
            caught = true;
        }
{% else %}
        CHAOS_EH_TRY
            result = chaos::il2cpp::runtime_core::ChaosDispatchMethodGetValue(
                GetHotpatchEntries(), kAotMethodCount, i, CHAOS_USE_DEFAULT_THUNKS);
        CHAOS_EH_CATCH_BEGIN
            caught = true;
        CHAOS_EH_END
{% end %}
        if (!first) printf(",");
        printf("{\"si\":%d,\"methodIndex\":%d,\"passed\":%s,\"value\":%" PRId64 "}",
               si, i, caught ? "false" : "true", caught ? -1 : result);
        first = false;
    }
    printf("]}\n");
    std::fflush(stdout);
    return 0;
}

static int RunListMode() {
    printf("{\"subjects\":[");
    for (int i = 0; i < kSubjectEntryCount_Generated; i++) {
        if (i > 0) printf(",");
        printf("{\"index\":%d,\"subjectId\":\"%s\",\"assemblyName\":\"%s\","
               "\"typeName\":\"%s\",\"methodName\":\"%s\",\"isStatic\":%s,\"kind\":\"%s\"}",
               kSubjectEntries[i].index,
               kSubjectEntries[i].subjectId,
               kSubjectEntries[i].assemblyName,
               kSubjectEntries[i].typeName,
               kSubjectEntries[i].methodName,
               kSubjectEntries[i].isStatic ? "true" : "false",
               kSubjectEntries[i].kind);
    }
    printf("]}\n");
    std::fflush(stdout);
    return 0;
}

static int RunBenchmarkMode(int entry_index, int iterations) {
    CHAOS_IL2CPP_PROFILE_SCOPE("RunBenchmarkMode");
    auto result = RunBenchmark(entry_index, iterations);
    if (result.elapsed_ms < 0.0) {
        printf("{\"elapsedMilliseconds\":-1.0,\"error\":\"invalid index\"}\n");
        return 1;
    }
    double ns_per_op = (result.elapsed_ms * 1e6) / iterations;
    double ops_per_sec = (iterations / result.elapsed_ms) * 1000.0;
    double alloc_per_op = static_cast<double>(result.allocated_bytes) / iterations;
    printf(
        "{\"elapsedMilliseconds\":%.3f,\"calibratedMs\":%.3f,"
        "\"opsPerSecond\":%.0f,\"iterations\":%d,"
        "\"allocatedBytes\":%" PRId64 ",\"allocPerOp\":%.1f}\n",
        result.elapsed_ms, result.elapsed_ms, ops_per_sec, iterations,
        result.allocated_bytes, alloc_per_op);
    std::fflush(stdout);
    return 0;
}

// ── --benchmark-all: benchmark ALL subject methods ─────────────────
static int RunBenchmarkAllMode(int iterations) {
    const int kCount = kSubjectEntryCount;
    printf("{\"benchmarkAll\":[");
    bool first = true;
    for (int si = 0; si < kCount; si++) {
        int i = kSubjectSlotMap[si];
        auto result = RunBenchmark(i, iterations);
        if (!first) printf(",");
        first = false;
        if (result.elapsed_ms < 0.0) {
            printf("{\"methodIndex\":%d,\"elapsedMilliseconds\":-1.0,\"error\":\"invalid index\"}", i);
        } else {
            double ops_per_sec = (iterations / result.elapsed_ms) * 1000.0;
            double alloc_per_op = static_cast<double>(result.allocated_bytes) / iterations;
            printf(
                "{\"methodIndex\":%d,\"elapsedMilliseconds\":%.3f,"
                "\"opsPerSecond\":%.0f,\"iterations\":%d,"
                "\"allocatedBytes\":%" PRId64 ",\"allocPerOp\":%.1f}",
                i, result.elapsed_ms, ops_per_sec, iterations,
                result.allocated_bytes, alloc_per_op);
        }
    }
    printf("]}\n");
    std::fflush(stdout);
    return 0;
}

// ── --benchmark-range: benchmark a contiguous range of method table indices ─
// First argument: start index (inclusive), Second argument: end index (exclusive)
static int RunBenchmarkRangeMode(int iterations, int start_idx, int end_idx) {
    if (start_idx < 0) start_idx = 0;
    if (end_idx > kAotMethodCount) end_idx = kAotMethodCount;
    printf("{\"benchmarkAll\":[");
    bool first = true;
    for (int i = start_idx; i < end_idx; i++) {
        auto result = RunBenchmark(i, iterations);
        if (!first) printf(",");
        first = false;
        if (result.elapsed_ms < 0.0) {
            printf("{\"methodIndex\":%d,\"elapsedMilliseconds\":-1.0,\"error\":\"invalid index\"}", i);
        } else {
            double ops_per_sec = (iterations / result.elapsed_ms) * 1000.0;
            double alloc_per_op = static_cast<double>(result.allocated_bytes) / iterations;
            printf(
                "{\"methodIndex\":%d,\"elapsedMilliseconds\":%.3f,"
                "\"opsPerSecond\":%.0f,\"iterations\":%d,"
                "\"allocatedBytes\":%" PRId64 ",\"allocPerOp\":%.1f}",
                i, result.elapsed_ms, ops_per_sec, iterations,
                result.allocated_bytes, alloc_per_op);
        }
    }
    printf("]}\n");
    std::fflush(stdout);
    return 0;
}

// Load patch data from an external file (used by the verification pipeline).
// Returns nullptr if path is nullptr or file cannot be opened.
static chaos::il2cpp::runtime_core::PatchContext* ApplyHotpatchFromFile(const char* path) {
    if (path == nullptr) return nullptr;
    FILE* f = fopen(path, "rb");
    if (!f) { std::fprintf(stderr, "WARN: cannot open patch data: %s\n", path); return nullptr; }
    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    rewind(f);
    uint8_t* data = (uint8_t*)std::malloc(size > 0 ? size : 1);
    if (!data) { fclose(f); return nullptr; }
    size_t read_size = fread(data, 1, size, f);
    fclose(f);
    if (read_size == 0) { std::free(data); return nullptr; }
    auto* ctx = chaos::il2cpp::runtime_core::ApplyPatchFromMemory(
        data, read_size, nullptr);
    std::free(data);
    if (ctx == nullptr) {
        std::fprintf(stderr, "WARN: ApplyPatchFromMemory returned null (no patches applied)\n");
    } else {
        std::fprintf(stderr, "INFO: Applied patches to %u methods from %s\n",
                     static_cast<unsigned>(ctx->method_count), path);
    }
    return ctx;
}

static int RunHotupdateMode(const char* patchDataPath = nullptr) {
    const int kCount = kSubjectEntryCount;
    // Dynamic allocation to support arbitrary method counts
    auto baseline_ok = std::make_unique<bool[]>(kCount);
    auto baseline_caught = std::make_unique<bool[]>(kCount);
    auto baseline_value = std::make_unique<int64_t[]>(kCount);
    for (int si = 0; si < kCount; si++) {
        baseline_ok[si] = false;
        baseline_caught[si] = false;
        baseline_value[si] = 0;
    }

    printf("{\n");
    printf("\"baselineFact\":[");
    for (int si = 0; si < kCount; si++) {
        int i = kSubjectSlotMap[si];
        int64_t bv = 0;
        bool caught = false;
{% if is_jit %}
        try {
            bv = chaos::il2cpp::runtime_core::ChaosDispatchMethodGetValue(
                GetHotpatchEntries(), kAotMethodCount, i, CHAOS_USE_DEFAULT_THUNKS);
            baseline_ok[si] = true;
        } catch(...) {
            caught = true;
        }
{% else %}
        CHAOS_EH_TRY
            bv = chaos::il2cpp::runtime_core::ChaosDispatchMethodGetValue(
                GetHotpatchEntries(), kAotMethodCount, i, CHAOS_USE_DEFAULT_THUNKS);
            baseline_ok[si] = true;
        CHAOS_EH_CATCH_BEGIN
            caught = true;
        CHAOS_EH_END
{% end %}
        baseline_caught[si] = caught;
        baseline_value[si] = bv;
        if (si > 0) printf(",");
        printf("{\"si\":%d,\"passed\":%s,\"value\":%" PRId64 "}",
               si, caught ? "false" : "true", bv);
    }
    printf("],");
    auto* patch_ctx = patchDataPath ? ApplyHotpatchFromFile(patchDataPath) : ApplyHotpatchIfAvailable();
    int semantic_passed = 0;
    int semantic_changed_count = 0;
    printf("\"patchedFact\":[");
    for (int si = 0; si < kCount; si++) {
        if (!baseline_ok[si]) { continue; }
        int i = kSubjectSlotMap[si];
        int64_t patched_value = 0;
        bool patched_caught = false;
{% if is_jit %}
        try {
            patched_value = chaos::il2cpp::runtime_core::ChaosDispatchMethodGetValue(
                GetHotpatchEntries(), kAotMethodCount, i, CHAOS_USE_DEFAULT_THUNKS);
        } catch(...) {
            patched_caught = true;
        }
{% else %}
        CHAOS_EH_TRY
            patched_value = chaos::il2cpp::runtime_core::ChaosDispatchMethodGetValue(
                GetHotpatchEntries(), kAotMethodCount, i, CHAOS_USE_DEFAULT_THUNKS);
        CHAOS_EH_CATCH_BEGIN
            patched_caught = true;
        CHAOS_EH_END
{% end %}
        if (semantic_passed > 0) printf(",");
        printf("{\"si\":%d,\"passed\":%s,\"value\":%" PRId64 "}",
               si, patched_caught ? "false" : "true",
               patched_caught ? 0 : patched_value);
        semantic_passed++;
        if (baseline_caught[si] != patched_caught ||
            baseline_value[si] != patched_value) {
            semantic_changed_count++;
        }
    }
    printf("],");
    if (patch_ctx != nullptr) {
        chaos::il2cpp::runtime_core::Unpatch(patch_ctx);
    }
    bool all_revert = true;
    int revert_passed = 0;
    for (int si = 0; si < kCount; si++) {
        int i = kSubjectSlotMap[si];
{% if is_jit %}
        try {
            chaos::il2cpp::runtime_core::ChaosDispatchMethod(
                GetHotpatchEntries(), kAotMethodCount, i, CHAOS_USE_DEFAULT_THUNKS);
            revert_passed++;
        } catch(...) {
            all_revert = false;
        }
{% else %}
        CHAOS_EH_TRY
            chaos::il2cpp::runtime_core::ChaosDispatchMethod(
                GetHotpatchEntries(), kAotMethodCount, i, CHAOS_USE_DEFAULT_THUNKS);
            revert_passed++;
        CHAOS_EH_CATCH_BEGIN
            all_revert = false;
        CHAOS_EH_END
{% end %}
    }
    bool all_semantic = (semantic_passed > 0 && semantic_changed_count > 0);
    printf(
        "\"passedMethods\":%d,\"failedMethods\":0,"
        "\"totalMethods\":%d,\"allSemantic\":%s,\"allRevert\":%s,"
        "\"semanticChangedCount\":%d}\n",
        semantic_passed, kCount, all_semantic ? "true" : "false",
        all_revert ? "true" : "false", semantic_changed_count);
    std::fflush(stdout);
    return 0;
}

static int RunMicrobenchMode() {
    RunMicrobench();
    return 0;
}

static int RunHotupdateBenchmarkMode(int entry_index, int iterations) {
    auto* patch_ctx = ApplyHotpatchIfAvailable();
    auto result = RunHotpatchBenchmark(entry_index, iterations);
    if (result.elapsed_ms < 0.0) {
        printf("{\"elapsedMilliseconds\":-1.0,\"error\":\"invalid index\"}\n");
        return 1;
    }
    double ops_per_sec = (iterations / result.elapsed_ms) * 1000.0;
    double alloc_per_op = static_cast<double>(result.allocated_bytes) / iterations;
    printf(
        "{\"elapsedMilliseconds\":%.3f,\"calibratedMs\":%.3f,"
        "\"opsPerSecond\":%.0f,\"iterations\":%d,"
        "\"allocatedBytes\":%" PRId64 ",\"allocPerOp\":%.1f}\n",
        result.elapsed_ms, result.elapsed_ms, ops_per_sec, iterations,
        result.allocated_bytes, alloc_per_op);
    std::fflush(stdout);
    return 0;
}

// ── --profile: per-method GC/allocation/code-size profile ───────────
static int RunProfileMode() {
    const int kCount = kSubjectEntryCount;
    chaos::il2cpp::runtime_core::ProfileStoreInit(kCount);
    for (int si = 0; si < kCount; si++) {
        int i = kSubjectSlotMap[si];
        int64_t heap_before = chaos::il2cpp::runtime_core::chaos_gc_get_heap_size();
        chaos::il2cpp::runtime_core::GetThreadProfileData().heap_before = heap_before;

{% if is_jit %}
        try {
            chaos::il2cpp::runtime_core::ChaosDispatchMethod(
                GetHotpatchEntries(), kAotMethodCount, i, CHAOS_USE_DEFAULT_THUNKS);
        } catch(...) { }
{% else %}
        CHAOS_EH_TRY
            chaos::il2cpp::runtime_core::ChaosDispatchMethod(
                GetHotpatchEntries(), kAotMethodCount, i, CHAOS_USE_DEFAULT_THUNKS);
        CHAOS_EH_CATCH_BEGIN
        CHAOS_EH_END
{% endif %}

        int64_t heap_after = chaos::il2cpp::runtime_core::chaos_gc_get_heap_size();
        chaos::il2cpp::runtime_core::GetThreadProfileData().heap_after = heap_after;
        chaos::il2cpp::runtime_core::FlushThreadProfileData(i);
    }
    chaos::il2cpp::runtime_core::ProfileStoreFinalize();
    chaos::il2cpp::runtime_core::ProfileEmitJson();
    return 0;
}


// ── --profile: per-method GC/allocation/code-size profile ───────────
static int RunProfileMode() {
    const int kCount = kSubjectEntryCount;
    chaos::il2cpp::runtime_core::ProfileStoreInit(kCount);
    for (int si = 0; si < kCount; si++) {
        int i = kSubjectSlotMap[si];
{% if is_jit %}
        try {
            int64_t heap_before = chaos::il2cpp::runtime_core::chaos_gc_get_heap_size();
            chaos::il2cpp::runtime_core::GetThreadProfileData().heap_before = heap_before;
            chaos::il2cpp::runtime_core::ChaosDispatchMethod(
                GetHotpatchEntries(), kAotMethodCount, i, CHAOS_USE_DEFAULT_THUNKS);
            int64_t heap_after = chaos::il2cpp::runtime_core::chaos_gc_get_heap_size();
            chaos::il2cpp::runtime_core::GetThreadProfileData().heap_after = heap_after;
            chaos::il2cpp::runtime_core::FlushThreadProfileData(i);
        } catch(...) { }
{% else %}
#if defined(_WIN32)
        __try {
            int64_t heap_before = chaos::il2cpp::runtime_core::chaos_gc_get_heap_size();
            chaos::il2cpp::runtime_core::GetThreadProfileData().heap_before = heap_before;
            chaos::il2cpp::runtime_core::ChaosDispatchMethod(
                GetHotpatchEntries(), kAotMethodCount, i, CHAOS_USE_DEFAULT_THUNKS);
            int64_t heap_after = chaos::il2cpp::runtime_core::chaos_gc_get_heap_size();
            chaos::il2cpp::runtime_core::GetThreadProfileData().heap_after = heap_after;
            chaos::il2cpp::runtime_core::FlushThreadProfileData(i);
        } __except(EXCEPTION_EXECUTE_HANDLER) {
            // SEH caught — continue profiling
        }
#else
        CHAOS_EH_TRY
            int64_t heap_before = chaos::il2cpp::runtime_core::chaos_gc_get_heap_size();
            chaos::il2cpp::runtime_core::GetThreadProfileData().heap_before = heap_before;
            chaos::il2cpp::runtime_core::ChaosDispatchMethod(
                GetHotpatchEntries(), kAotMethodCount, i, CHAOS_USE_DEFAULT_THUNKS);
            int64_t heap_after = chaos::il2cpp::runtime_core::chaos_gc_get_heap_size();
            chaos::il2cpp::runtime_core::GetThreadProfileData().heap_after = heap_after;
            chaos::il2cpp::runtime_core::FlushThreadProfileData(i);
        CHAOS_EH_CATCH_BEGIN
        CHAOS_EH_END
#endif
{% endif %}
    }
    chaos::il2cpp::runtime_core::ProfileStoreFinalize();
    chaos::il2cpp::runtime_core::ProfileEmitJson();
    return 0;
}

int main(int argc, char* argv[]) {
    chaos::il2cpp::common::log_internal::g_log_use_stderr = true;
    chaos::il2cpp::runtime_core::g_bgc_enabled = false;

    auto* chaos_host = new ChaosRuntimeHost();
    if (!chaos_host->Initialize("verification-entry")) {
        std::fprintf(stderr, "FATAL: ChaosRuntimeHost::Initialize failed\n");
        return 1;
    }
    if (!chaos_host->RegisterModule(
            &chaos_codegen_code_registration,
            &chaos_codegen_metadata_registration,
            &chaos_codegen_options)) {
        std::fprintf(stderr, "FATAL: RegisterModule failed\n");
        return 1;
    }
    ChaosRegisterGcLayouts();

    int ret = 0;
    if (argc < 2) { ret = RunFactMode(); goto shutdown; }

    if (std::strcmp(argv[1], "--fact-json") == 0) { ret = RunFactJsonMode(); goto shutdown; }

    if (std::strcmp(argv[1], "--list") == 0) { ret = RunListMode(); goto shutdown; }

    if (std::strcmp(argv[1], "--benchmark") == 0) {
        if (argc < 4) { printf("Usage: entry.exe --benchmark <index> <iterations>\n"); return 1; }
        ret = RunBenchmarkMode(std::atoi(argv[2]), std::atoi(argv[3]));
        goto shutdown;
    }

    if (std::strcmp(argv[1], "--benchmark-all") == 0) {
        if (argc < 3) { printf("Usage: entry.exe --benchmark-all <iterations>\n"); return 1; }
        ret = RunBenchmarkAllMode(std::atoi(argv[2]));
        goto shutdown;
    }

    if (std::strcmp(argv[1], "--benchmark-range") == 0) {
        if (argc < 5) { printf("Usage: entry.exe --benchmark-range <start> <end> <iterations>\n"); return 1; }
        ret = RunBenchmarkRangeMode(std::atoi(argv[4]), std::atoi(argv[2]), std::atoi(argv[3]));
        goto shutdown;
    }

    if (std::strcmp(argv[1], "--hotupdate") == 0) {
        const char* patchDataPath = nullptr;
        if (argc >= 4 && std::strcmp(argv[2], "--patch-data") == 0) {
            patchDataPath = argv[3];
        }
        ret = RunHotupdateMode(patchDataPath);
        goto shutdown;
    }

    if (std::strcmp(argv[1], "--hotupdate-and-benchmark") == 0) {
        if (argc < 4) { printf("Usage: entry.exe --hotupdate-and-benchmark <index> <iterations>\n"); return 1; }
        ret = RunHotupdateBenchmarkMode(std::atoi(argv[2]), std::atoi(argv[3]));
        goto shutdown;
    }

    if (std::strcmp(argv[1], "--microbench") == 0) { ret = RunMicrobenchMode(); goto shutdown; }

    if (std::strcmp(argv[1], "--profile") == 0) { ret = RunProfileMode(); goto shutdown; }

    if (std::strcmp(argv[1], "--profile") == 0) { ret = RunProfileMode(); goto shutdown; }

    printf("Unknown flag: %s\n", argv[1]);
    ret = 1;

shutdown:
    std::fflush(stdout);
{% if is_windows %}
    TerminateProcess(GetCurrentProcess(), static_cast<DWORD>(ret));
{% else %}
    _exit(ret);
{% end %}
}