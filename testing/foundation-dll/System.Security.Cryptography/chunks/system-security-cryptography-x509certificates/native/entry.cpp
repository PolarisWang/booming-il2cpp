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
    { 0, "System.Security.Cryptography/System.Security.Cryptography.X509Certificates.CertificateRequest::CreateSigningRequest:System.Byte[]()", "codegen", "System.Security.Cryptography.X509Certificates.CertificateRequest", "CreateSigningRequest", True, "benchmark" },
    { 1, "System.Security.Cryptography/System.Security.Cryptography.X509Certificates.CertificateRequest::CreateSigningRequest:System.Byte[](System.Security.Cryptography.X509Certificates.X509SignatureGenerator)", "codegen", "System.Security.Cryptography.X509Certificates.CertificateRequest", "CreateSigningRequest", True, "benchmark" },
    { 2, "System.Security.Cryptography/System.Security.Cryptography.X509Certificates.CertificateRequest::CreateSigningRequestPem:System.String()", "codegen", "System.Security.Cryptography.X509Certificates.CertificateRequest", "CreateSigningRequestPem", True, "benchmark" },
    { 3, "System.Security.Cryptography/System.Security.Cryptography.X509Certificates.CertificateRequest::CreateSigningRequestPem:System.String(System.Security.Cryptography.X509Certificates.X509SignatureGenerator)", "codegen", "System.Security.Cryptography.X509Certificates.CertificateRequest", "CreateSigningRequestPem", True, "benchmark" },
    { 4, "System.Security.Cryptography/System.Security.Cryptography.X509Certificates.CertificateRequest::CreateSelfSigned:System.Security.Cryptography.X509Certificates.X509Certificate2(System.DateTimeOffset,System.DateTimeOffset)", "codegen", "System.Security.Cryptography.X509Certificates.CertificateRequest", "CreateSelfSigned", True, "benchmark" },
    { 5, "System.Security.Cryptography/System.Security.Cryptography.X509Certificates.CertificateRequest::LoadSigningRequestPem:System.Security.Cryptography.X509Certificates.CertificateRequest(System.String,System.Security.Cryptography.HashAlgorithmName,System.Security.Cryptography.X509Certificates.CertificateRequestLoadOptions,System.Security.Cryptography.RSASignaturePadding)", "codegen", "System.Security.Cryptography.X509Certificates.CertificateRequest", "LoadSigningRequestPem", True, "benchmark" },
    { 6, "System.Security.Cryptography/System.Security.Cryptography.X509Certificates.CertificateRequest::LoadSigningRequestPem:System.Security.Cryptography.X509Certificates.CertificateRequest(System.String,System.Security.Cryptography.HashAlgorithmName,System.Security.Cryptography.X509Certificates.CertificateRequestLoadOptions,System.Security.Cryptography.RSASignaturePadding)", "codegen", "System.Security.Cryptography.X509Certificates.CertificateRequest", "LoadSigningRequestPem", True, "benchmark" },
    { 7, "System.Security.Cryptography/System.Security.Cryptography.X509Certificates.CertificateRequest::LoadSigningRequest:System.Security.Cryptography.X509Certificates.CertificateRequest(System.Byte[],System.Security.Cryptography.HashAlgorithmName,System.Security.Cryptography.X509Certificates.CertificateRequestLoadOptions,System.Security.Cryptography.RSASignaturePadding)", "codegen", "System.Security.Cryptography.X509Certificates.CertificateRequest", "LoadSigningRequest", True, "benchmark" },
    { 8, "System.Security.Cryptography/System.Security.Cryptography.X509Certificates.CertificateRequest::LoadSigningRequest:System.Security.Cryptography.X509Certificates.CertificateRequest(System.Byte[],System.Security.Cryptography.HashAlgorithmName,System.Security.Cryptography.X509Certificates.CertificateRequestLoadOptions,System.Security.Cryptography.RSASignaturePadding)", "codegen", "System.Security.Cryptography.X509Certificates.CertificateRequest", "LoadSigningRequest", True, "benchmark" },
    { 9, "System.Security.Cryptography/System.Security.Cryptography.X509Certificates.CertificateRequest::LoadSigningRequest:System.Security.Cryptography.X509Certificates.CertificateRequest(System.Byte[],System.Security.Cryptography.HashAlgorithmName,System.Security.Cryptography.X509Certificates.CertificateRequestLoadOptions,System.Security.Cryptography.RSASignaturePadding)", "codegen", "System.Security.Cryptography.X509Certificates.CertificateRequest", "LoadSigningRequest", True, "benchmark" },
    { 10, "System.Security.Cryptography/System.Security.Cryptography.X509Certificates.CertificateRevocationListBuilder::AddEntry:System.Void(System.Security.Cryptography.X509Certificates.X509Certificate2,System.Nullable<System.DateTimeOffset>,System.Nullable<System.Security.Cryptography.X509Certificates.X509RevocationReason>)", "codegen", "System.Security.Cryptography.X509Certificates.CertificateRevocationListBuilder", "AddEntry", True, "benchmark" },
    { 11, "System.Security.Cryptography/System.Security.Cryptography.X509Certificates.CertificateRevocationListBuilder::AddEntry:System.Void(System.Byte[],System.Nullable<System.DateTimeOffset>,System.Nullable<System.Security.Cryptography.X509Certificates.X509RevocationReason>)", "codegen", "System.Security.Cryptography.X509Certificates.CertificateRevocationListBuilder", "AddEntry", True, "benchmark" },
    { 12, "System.Security.Cryptography/System.Security.Cryptography.X509Certificates.CertificateRevocationListBuilder::AddEntry:System.Void(System.Byte[],System.Nullable<System.DateTimeOffset>,System.Nullable<System.Security.Cryptography.X509Certificates.X509RevocationReason>)", "codegen", "System.Security.Cryptography.X509Certificates.CertificateRevocationListBuilder", "AddEntry", True, "benchmark" },
    { 13, "System.Security.Cryptography/System.Security.Cryptography.X509Certificates.CertificateRevocationListBuilder::AddEntry:System.Void(System.Byte[],System.Nullable<System.DateTimeOffset>,System.Nullable<System.Security.Cryptography.X509Certificates.X509RevocationReason>)", "codegen", "System.Security.Cryptography.X509Certificates.CertificateRevocationListBuilder", "AddEntry", True, "benchmark" },
    { 14, "System.Security.Cryptography/System.Security.Cryptography.X509Certificates.CertificateRevocationListBuilder::RemoveEntry:System.Boolean(System.Byte[])", "codegen", "System.Security.Cryptography.X509Certificates.CertificateRevocationListBuilder", "RemoveEntry", True, "benchmark" },
    { 15, "System.Security.Cryptography/System.Security.Cryptography.X509Certificates.CertificateRevocationListBuilder::RemoveEntry:System.Boolean(System.Byte[])", "codegen", "System.Security.Cryptography.X509Certificates.CertificateRevocationListBuilder", "RemoveEntry", True, "benchmark" },
    { 16, "System.Security.Cryptography/System.Security.Cryptography.X509Certificates.CertificateRevocationListBuilder::RemoveEntry:System.Boolean(System.Byte[])", "codegen", "System.Security.Cryptography.X509Certificates.CertificateRevocationListBuilder", "RemoveEntry", True, "benchmark" },
    { 17, "System.Security.Cryptography/System.Security.Cryptography.X509Certificates.CertificateRevocationListBuilder::Build:System.Byte[](System.Security.Cryptography.X509Certificates.X509Certificate2,System.Numerics.BigInteger,System.DateTimeOffset,System.Security.Cryptography.HashAlgorithmName,System.Security.Cryptography.RSASignaturePadding,System.Nullable<System.DateTimeOffset>)", "codegen", "System.Security.Cryptography.X509Certificates.CertificateRevocationListBuilder", "Build", True, "benchmark" },
    { 18, "System.Security.Cryptography/System.Security.Cryptography.X509Certificates.CertificateRevocationListBuilder::Build:System.Byte[](System.Security.Cryptography.X509Certificates.X509Certificate2,System.Numerics.BigInteger,System.DateTimeOffset,System.Security.Cryptography.HashAlgorithmName,System.Security.Cryptography.RSASignaturePadding,System.Nullable<System.DateTimeOffset>)", "codegen", "System.Security.Cryptography.X509Certificates.CertificateRevocationListBuilder", "Build", True, "benchmark" },
    { 19, "System.Security.Cryptography/System.Security.Cryptography.X509Certificates.CertificateRevocationListBuilder::Build:System.Byte[](System.Security.Cryptography.X509Certificates.X500DistinguishedName,System.Security.Cryptography.X509Certificates.X509SignatureGenerator,System.Numerics.BigInteger,System.DateTimeOffset,System.Security.Cryptography.HashAlgorithmName,System.Security.Cryptography.X509Certificates.X509AuthorityKeyIdentifierExtension,System.Nullable<System.DateTimeOffset>)", "codegen", "System.Security.Cryptography.X509Certificates.CertificateRevocationListBuilder", "Build", True, "benchmark" },
    { 20, "System.Security.Cryptography/System.Security.Cryptography.X509Certificates.CertificateRevocationListBuilder::Build:System.Byte[](System.Security.Cryptography.X509Certificates.X500DistinguishedName,System.Security.Cryptography.X509Certificates.X509SignatureGenerator,System.Numerics.BigInteger,System.DateTimeOffset,System.Security.Cryptography.HashAlgorithmName,System.Security.Cryptography.X509Certificates.X509AuthorityKeyIdentifierExtension,System.Nullable<System.DateTimeOffset>)", "codegen", "System.Security.Cryptography.X509Certificates.CertificateRevocationListBuilder", "Build", True, "benchmark" },
    { 21, "System.Security.Cryptography/System.Security.Cryptography.X509Certificates.CertificateRevocationListBuilder::BuildCrlDistributionPointExtension:System.Security.Cryptography.X509Certificates.X509Extension(System.Collections.Generic.IEnumerable<System.String>,System.Boolean)", "codegen", "System.Security.Cryptography.X509Certificates.CertificateRevocationListBuilder", "BuildCrlDistributionPointExtension", True, "benchmark" },
    { 22, "System.Security.Cryptography/System.Security.Cryptography.X509Certificates.CertificateRevocationListBuilder::BuildCrlDistributionPointExtension:System.Security.Cryptography.X509Certificates.X509Extension(System.Collections.Generic.IEnumerable<System.String>,System.Boolean)", "codegen", "System.Security.Cryptography.X509Certificates.CertificateRevocationListBuilder", "BuildCrlDistributionPointExtension", True, "benchmark" },
    { 23, "System.Security.Cryptography/System.Security.Cryptography.X509Certificates.CertificateRevocationListBuilder::BuildCrlDistributionPointExtension:System.Security.Cryptography.X509Certificates.X509Extension(System.Collections.Generic.IEnumerable<System.String>,System.Boolean)", "codegen", "System.Security.Cryptography.X509Certificates.CertificateRevocationListBuilder", "BuildCrlDistributionPointExtension", True, "benchmark" },
    { 24, "System.Security.Cryptography/System.Security.Cryptography.X509Certificates.CertificateRevocationListBuilder::Load:System.Security.Cryptography.X509Certificates.CertificateRevocationListBuilder(System.Byte[],System.Numerics.BigInteger)", "codegen", "System.Security.Cryptography.X509Certificates.CertificateRevocationListBuilder", "Load", True, "benchmark" },
    { 25, "System.Security.Cryptography/System.Security.Cryptography.X509Certificates.CertificateRevocationListBuilder::Load:System.Security.Cryptography.X509Certificates.CertificateRevocationListBuilder(System.Byte[],System.Numerics.BigInteger)", "codegen", "System.Security.Cryptography.X509Certificates.CertificateRevocationListBuilder", "Load", True, "benchmark" },
    { 26, "System.Security.Cryptography/System.Security.Cryptography.X509Certificates.CertificateRevocationListBuilder::Load:System.Security.Cryptography.X509Certificates.CertificateRevocationListBuilder(System.Byte[],System.Numerics.BigInteger)", "codegen", "System.Security.Cryptography.X509Certificates.CertificateRevocationListBuilder", "Load", True, "benchmark" },
    { 27, "System.Security.Cryptography/System.Security.Cryptography.X509Certificates.CertificateRevocationListBuilder::Load:System.Security.Cryptography.X509Certificates.CertificateRevocationListBuilder(System.Byte[],System.Numerics.BigInteger)", "codegen", "System.Security.Cryptography.X509Certificates.CertificateRevocationListBuilder", "Load", True, "benchmark" },
    { 28, "System.Security.Cryptography/System.Security.Cryptography.X509Certificates.CertificateRevocationListBuilder::LoadPem:System.Security.Cryptography.X509Certificates.CertificateRevocationListBuilder(System.String,System.Numerics.BigInteger)", "codegen", "System.Security.Cryptography.X509Certificates.CertificateRevocationListBuilder", "LoadPem", True, "benchmark" },
    { 29, "System.Security.Cryptography/System.Security.Cryptography.X509Certificates.CertificateRevocationListBuilder::LoadPem:System.Security.Cryptography.X509Certificates.CertificateRevocationListBuilder(System.String,System.Numerics.BigInteger)", "codegen", "System.Security.Cryptography.X509Certificates.CertificateRevocationListBuilder", "LoadPem", True, "benchmark" },
    { 30, "System.Security.Cryptography/System.Security.Cryptography.X509Certificates.CertificateRevocationListBuilder::LoadPem:System.Security.Cryptography.X509Certificates.CertificateRevocationListBuilder(System.String,System.Numerics.BigInteger)", "codegen", "System.Security.Cryptography.X509Certificates.CertificateRevocationListBuilder", "LoadPem", True, "benchmark" },
    { 31, "System.Security.Cryptography/System.Security.Cryptography.X509Certificates.DSACertificateExtensions::GetDSAPublicKey:System.Security.Cryptography.DSA(System.Security.Cryptography.X509Certificates.X509Certificate2)", "codegen", "System.Security.Cryptography.X509Certificates.DSACertificateExtensions", "GetDSAPublicKey", True, "benchmark" },
    { 32, "System.Security.Cryptography/System.Security.Cryptography.X509Certificates.DSACertificateExtensions::GetDSAPrivateKey:System.Security.Cryptography.DSA(System.Security.Cryptography.X509Certificates.X509Certificate2)", "codegen", "System.Security.Cryptography.X509Certificates.DSACertificateExtensions", "GetDSAPrivateKey", True, "benchmark" },
    { 33, "System.Security.Cryptography/System.Security.Cryptography.X509Certificates.DSACertificateExtensions::CopyWithPrivateKey:System.Security.Cryptography.X509Certificates.X509Certificate2(System.Security.Cryptography.X509Certificates.X509Certificate2,System.Security.Cryptography.DSA)", "codegen", "System.Security.Cryptography.X509Certificates.DSACertificateExtensions", "CopyWithPrivateKey", True, "benchmark" },
    { 34, "System.Security.Cryptography/System.Security.Cryptography.X509Certificates.ECDsaCertificateExtensions::GetECDsaPublicKey:System.Security.Cryptography.ECDsa(System.Security.Cryptography.X509Certificates.X509Certificate2)", "codegen", "System.Security.Cryptography.X509Certificates.ECDsaCertificateExtensions", "GetECDsaPublicKey", True, "benchmark" },
    { 35, "System.Security.Cryptography/System.Security.Cryptography.X509Certificates.ECDsaCertificateExtensions::GetECDsaPrivateKey:System.Security.Cryptography.ECDsa(System.Security.Cryptography.X509Certificates.X509Certificate2)", "codegen", "System.Security.Cryptography.X509Certificates.ECDsaCertificateExtensions", "GetECDsaPrivateKey", True, "benchmark" },
    { 36, "System.Security.Cryptography/System.Security.Cryptography.X509Certificates.ECDsaCertificateExtensions::CopyWithPrivateKey:System.Security.Cryptography.X509Certificates.X509Certificate2(System.Security.Cryptography.X509Certificates.X509Certificate2,System.Security.Cryptography.ECDsa)", "codegen", "System.Security.Cryptography.X509Certificates.ECDsaCertificateExtensions", "CopyWithPrivateKey", True, "benchmark" },
    { 37, "System.Security.Cryptography/System.Security.Cryptography.X509Certificates.Pkcs12LoaderLimits::MakeReadOnly:System.Void()", "codegen", "System.Security.Cryptography.X509Certificates.Pkcs12LoaderLimits", "MakeReadOnly", True, "benchmark" },
    { 38, "System.Security.Cryptography/System.Security.Cryptography.X509Certificates.PublicKey::GetRSAPublicKey:System.Security.Cryptography.RSA()", "codegen", "System.Security.Cryptography.X509Certificates.PublicKey", "GetRSAPublicKey", True, "benchmark" },
    { 39, "System.Security.Cryptography/System.Security.Cryptography.X509Certificates.PublicKey::GetDSAPublicKey:System.Security.Cryptography.DSA()", "codegen", "System.Security.Cryptography.X509Certificates.PublicKey", "GetDSAPublicKey", True, "benchmark" },
    { 40, "System.Security.Cryptography/System.Security.Cryptography.X509Certificates.PublicKey::GetECDsaPublicKey:System.Security.Cryptography.ECDsa()", "codegen", "System.Security.Cryptography.X509Certificates.PublicKey", "GetECDsaPublicKey", True, "benchmark" },
    { 41, "System.Security.Cryptography/System.Security.Cryptography.X509Certificates.PublicKey::GetECDiffieHellmanPublicKey:System.Security.Cryptography.ECDiffieHellman()", "codegen", "System.Security.Cryptography.X509Certificates.PublicKey", "GetECDiffieHellmanPublicKey", True, "benchmark" },
    { 42, "System.Security.Cryptography/System.Security.Cryptography.X509Certificates.RSACertificateExtensions::GetRSAPublicKey:System.Security.Cryptography.RSA(System.Security.Cryptography.X509Certificates.X509Certificate2)", "codegen", "System.Security.Cryptography.X509Certificates.RSACertificateExtensions", "GetRSAPublicKey", True, "benchmark" },
    { 43, "System.Security.Cryptography/System.Security.Cryptography.X509Certificates.RSACertificateExtensions::GetRSAPrivateKey:System.Security.Cryptography.RSA(System.Security.Cryptography.X509Certificates.X509Certificate2)", "codegen", "System.Security.Cryptography.X509Certificates.RSACertificateExtensions", "GetRSAPrivateKey", True, "benchmark" },
    { 44, "System.Security.Cryptography/System.Security.Cryptography.X509Certificates.RSACertificateExtensions::CopyWithPrivateKey:System.Security.Cryptography.X509Certificates.X509Certificate2(System.Security.Cryptography.X509Certificates.X509Certificate2,System.Security.Cryptography.RSA)", "codegen", "System.Security.Cryptography.X509Certificates.RSACertificateExtensions", "CopyWithPrivateKey", True, "benchmark" },
    { 45, "System.Security.Cryptography/System.Security.Cryptography.X509Certificates.SubjectAlternativeNameBuilder::AddEmailAddress:System.Void(System.String)", "codegen", "System.Security.Cryptography.X509Certificates.SubjectAlternativeNameBuilder", "AddEmailAddress", True, "benchmark" },
    { 46, "System.Security.Cryptography/System.Security.Cryptography.X509Certificates.SubjectAlternativeNameBuilder::AddEmailAddress:System.Void(System.String)", "codegen", "System.Security.Cryptography.X509Certificates.SubjectAlternativeNameBuilder", "AddEmailAddress", True, "benchmark" },
    { 47, "System.Security.Cryptography/System.Security.Cryptography.X509Certificates.SubjectAlternativeNameBuilder::AddDnsName:System.Void(System.String)", "codegen", "System.Security.Cryptography.X509Certificates.SubjectAlternativeNameBuilder", "AddDnsName", True, "benchmark" },
    { 48, "System.Security.Cryptography/System.Security.Cryptography.X509Certificates.SubjectAlternativeNameBuilder::AddDnsName:System.Void(System.String)", "codegen", "System.Security.Cryptography.X509Certificates.SubjectAlternativeNameBuilder", "AddDnsName", True, "benchmark" },
    { 49, "System.Security.Cryptography/System.Security.Cryptography.X509Certificates.SubjectAlternativeNameBuilder::AddUri:System.Void(System.Uri)", "codegen", "System.Security.Cryptography.X509Certificates.SubjectAlternativeNameBuilder", "AddUri", True, "benchmark" },
    { 50, "System.Security.Cryptography/System.Security.Cryptography.X509Certificates.SubjectAlternativeNameBuilder::AddIpAddress:System.Void(System.Net.IPAddress)", "codegen", "System.Security.Cryptography.X509Certificates.SubjectAlternativeNameBuilder", "AddIpAddress", True, "benchmark" },
    { 51, "System.Security.Cryptography/System.Security.Cryptography.X509Certificates.SubjectAlternativeNameBuilder::AddUserPrincipalName:System.Void(System.String)", "codegen", "System.Security.Cryptography.X509Certificates.SubjectAlternativeNameBuilder", "AddUserPrincipalName", True, "benchmark" },
    { 52, "System.Security.Cryptography/System.Security.Cryptography.X509Certificates.SubjectAlternativeNameBuilder::AddUserPrincipalName:System.Void(System.String)", "codegen", "System.Security.Cryptography.X509Certificates.SubjectAlternativeNameBuilder", "AddUserPrincipalName", True, "benchmark" },
    { 53, "System.Security.Cryptography/System.Security.Cryptography.X509Certificates.SubjectAlternativeNameBuilder::Build:System.Security.Cryptography.X509Certificates.X509Extension(System.Boolean)", "codegen", "System.Security.Cryptography.X509Certificates.SubjectAlternativeNameBuilder", "Build", True, "benchmark" },
    { 54, "System.Security.Cryptography/System.Security.Cryptography.X509Certificates.SubjectAlternativeNameBuilder::Build:System.Security.Cryptography.X509Certificates.X509Extension(System.Boolean)", "codegen", "System.Security.Cryptography.X509Certificates.SubjectAlternativeNameBuilder", "Build", True, "benchmark" },
    { 55, "System.Security.Cryptography/System.Security.Cryptography.X509Certificates.X500DistinguishedName::Decode:System.String(System.Security.Cryptography.X509Certificates.X500DistinguishedNameFlags)", "codegen", "System.Security.Cryptography.X509Certificates.X500DistinguishedName", "Decode", True, "benchmark" },
    { 56, "System.Security.Cryptography/System.Security.Cryptography.X509Certificates.X500DistinguishedName::Format:System.String(System.Boolean)", "codegen", "System.Security.Cryptography.X509Certificates.X500DistinguishedName", "Format", True, "benchmark" },
    { 57, "System.Security.Cryptography/System.Security.Cryptography.X509Certificates.X500DistinguishedName::Format:System.String(System.Boolean)", "codegen", "System.Security.Cryptography.X509Certificates.X500DistinguishedName", "Format", True, "benchmark" },
    { 58, "System.Security.Cryptography/System.Security.Cryptography.X509Certificates.X500DistinguishedName::EnumerateRelativeDistinguishedNames:System.Collections.Generic.IEnumerable<System.Security.Cryptography.X509Certificates.X500RelativeDistinguishedName>(System.Boolean)", "codegen", "System.Security.Cryptography.X509Certificates.X500DistinguishedName", "EnumerateRelativeDistinguishedNames", True, "benchmark" },
    { 59, "System.Security.Cryptography/System.Security.Cryptography.X509Certificates.X500DistinguishedName::EnumerateRelativeDistinguishedNames:System.Collections.Generic.IEnumerable<System.Security.Cryptography.X509Certificates.X500RelativeDistinguishedName>(System.Boolean)", "codegen", "System.Security.Cryptography.X509Certificates.X500DistinguishedName", "EnumerateRelativeDistinguishedNames", True, "benchmark" },
    { 60, "System.Security.Cryptography/System.Security.Cryptography.X509Certificates.X500DistinguishedNameBuilder::Add:System.Void(System.String,System.String,System.Nullable<System.Formats.Asn1.UniversalTagNumber>)", "codegen", "System.Security.Cryptography.X509Certificates.X500DistinguishedNameBuilder", "Add", True, "benchmark" },
    { 61, "System.Security.Cryptography/System.Security.Cryptography.X509Certificates.X500DistinguishedNameBuilder::Add:System.Void(System.String,System.String,System.Nullable<System.Formats.Asn1.UniversalTagNumber>)", "codegen", "System.Security.Cryptography.X509Certificates.X500DistinguishedNameBuilder", "Add", True, "benchmark" },
    { 62, "System.Security.Cryptography/System.Security.Cryptography.X509Certificates.X500DistinguishedNameBuilder::Add:System.Void(System.String,System.String,System.Nullable<System.Formats.Asn1.UniversalTagNumber>)", "codegen", "System.Security.Cryptography.X509Certificates.X500DistinguishedNameBuilder", "Add", True, "benchmark" },
    { 63, "System.Security.Cryptography/System.Security.Cryptography.X509Certificates.X500DistinguishedNameBuilder::Add:System.Void(System.Security.Cryptography.Oid,System.String,System.Nullable<System.Formats.Asn1.UniversalTagNumber>)", "codegen", "System.Security.Cryptography.X509Certificates.X500DistinguishedNameBuilder", "Add", True, "benchmark" },
    { 64, "System.Security.Cryptography/System.Security.Cryptography.X509Certificates.X500DistinguishedNameBuilder::Add:System.Void(System.Security.Cryptography.Oid,System.String,System.Nullable<System.Formats.Asn1.UniversalTagNumber>)", "codegen", "System.Security.Cryptography.X509Certificates.X500DistinguishedNameBuilder", "Add", True, "benchmark" },
    { 65, "System.Security.Cryptography/System.Security.Cryptography.X509Certificates.X500DistinguishedNameBuilder::AddEmailAddress:System.Void(System.String)", "codegen", "System.Security.Cryptography.X509Certificates.X500DistinguishedNameBuilder", "AddEmailAddress", True, "benchmark" },
    { 66, "System.Security.Cryptography/System.Security.Cryptography.X509Certificates.X500DistinguishedNameBuilder::AddEmailAddress:System.Void(System.String)", "codegen", "System.Security.Cryptography.X509Certificates.X500DistinguishedNameBuilder", "AddEmailAddress", True, "benchmark" },
    { 67, "System.Security.Cryptography/System.Security.Cryptography.X509Certificates.X500DistinguishedNameBuilder::AddCommonName:System.Void(System.String)", "codegen", "System.Security.Cryptography.X509Certificates.X500DistinguishedNameBuilder", "AddCommonName", True, "benchmark" },
    { 68, "System.Security.Cryptography/System.Security.Cryptography.X509Certificates.X500DistinguishedNameBuilder::AddCommonName:System.Void(System.String)", "codegen", "System.Security.Cryptography.X509Certificates.X500DistinguishedNameBuilder", "AddCommonName", True, "benchmark" },
    { 69, "System.Security.Cryptography/System.Security.Cryptography.X509Certificates.X500DistinguishedNameBuilder::AddLocalityName:System.Void(System.String)", "codegen", "System.Security.Cryptography.X509Certificates.X500DistinguishedNameBuilder", "AddLocalityName", True, "benchmark" },
    { 70, "System.Security.Cryptography/System.Security.Cryptography.X509Certificates.X500DistinguishedNameBuilder::AddLocalityName:System.Void(System.String)", "codegen", "System.Security.Cryptography.X509Certificates.X500DistinguishedNameBuilder", "AddLocalityName", True, "benchmark" },
    { 71, "System.Security.Cryptography/System.Security.Cryptography.X509Certificates.X500DistinguishedNameBuilder::AddCountryOrRegion:System.Void(System.String)", "codegen", "System.Security.Cryptography.X509Certificates.X500DistinguishedNameBuilder", "AddCountryOrRegion", True, "benchmark" },
    { 72, "System.Security.Cryptography/System.Security.Cryptography.X509Certificates.X500DistinguishedNameBuilder::AddCountryOrRegion:System.Void(System.String)", "codegen", "System.Security.Cryptography.X509Certificates.X500DistinguishedNameBuilder", "AddCountryOrRegion", True, "benchmark" },
    { 73, "System.Security.Cryptography/System.Security.Cryptography.X509Certificates.X500DistinguishedNameBuilder::AddOrganizationName:System.Void(System.String)", "codegen", "System.Security.Cryptography.X509Certificates.X500DistinguishedNameBuilder", "AddOrganizationName", True, "benchmark" },
    { 74, "System.Security.Cryptography/System.Security.Cryptography.X509Certificates.X500DistinguishedNameBuilder::AddOrganizationName:System.Void(System.String)", "codegen", "System.Security.Cryptography.X509Certificates.X500DistinguishedNameBuilder", "AddOrganizationName", True, "benchmark" },
    { 75, "System.Security.Cryptography/System.Security.Cryptography.X509Certificates.X500DistinguishedNameBuilder::AddOrganizationalUnitName:System.Void(System.String)", "codegen", "System.Security.Cryptography.X509Certificates.X500DistinguishedNameBuilder", "AddOrganizationalUnitName", True, "benchmark" },
    { 76, "System.Security.Cryptography/System.Security.Cryptography.X509Certificates.X500DistinguishedNameBuilder::AddOrganizationalUnitName:System.Void(System.String)", "codegen", "System.Security.Cryptography.X509Certificates.X500DistinguishedNameBuilder", "AddOrganizationalUnitName", True, "benchmark" },
    { 77, "System.Security.Cryptography/System.Security.Cryptography.X509Certificates.X500DistinguishedNameBuilder::AddStateOrProvinceName:System.Void(System.String)", "codegen", "System.Security.Cryptography.X509Certificates.X500DistinguishedNameBuilder", "AddStateOrProvinceName", True, "benchmark" },
    { 78, "System.Security.Cryptography/System.Security.Cryptography.X509Certificates.X500DistinguishedNameBuilder::AddStateOrProvinceName:System.Void(System.String)", "codegen", "System.Security.Cryptography.X509Certificates.X500DistinguishedNameBuilder", "AddStateOrProvinceName", True, "benchmark" },
    { 79, "System.Security.Cryptography/System.Security.Cryptography.X509Certificates.X500DistinguishedNameBuilder::AddDomainComponent:System.Void(System.String)", "codegen", "System.Security.Cryptography.X509Certificates.X500DistinguishedNameBuilder", "AddDomainComponent", True, "benchmark" },
    { 80, "System.Security.Cryptography/System.Security.Cryptography.X509Certificates.X500DistinguishedNameBuilder::AddDomainComponent:System.Void(System.String)", "codegen", "System.Security.Cryptography.X509Certificates.X500DistinguishedNameBuilder", "AddDomainComponent", True, "benchmark" },
    { 81, "System.Security.Cryptography/System.Security.Cryptography.X509Certificates.X500DistinguishedNameBuilder::Build:System.Security.Cryptography.X509Certificates.X500DistinguishedName()", "codegen", "System.Security.Cryptography.X509Certificates.X500DistinguishedNameBuilder", "Build", True, "benchmark" },
    { 82, "System.Security.Cryptography/System.Security.Cryptography.X509Certificates.X500RelativeDistinguishedName::GetSingleElementType:System.Security.Cryptography.Oid()", "codegen", "System.Security.Cryptography.X509Certificates.X500RelativeDistinguishedName", "GetSingleElementType", True, "benchmark" },
    { 83, "System.Security.Cryptography/System.Security.Cryptography.X509Certificates.X500RelativeDistinguishedName::GetSingleElementValue:System.String()", "codegen", "System.Security.Cryptography.X509Certificates.X500RelativeDistinguishedName", "GetSingleElementValue", True, "benchmark" },
    { 84, "System.Security.Cryptography/System.Security.Cryptography.X509Certificates.X509AuthorityInformationAccessExtension::CopyFrom:System.Void(System.Security.Cryptography.AsnEncodedData)", "codegen", "System.Security.Cryptography.X509Certificates.X509AuthorityInformationAccessExtension", "CopyFrom", True, "benchmark" },
    { 85, "System.Security.Cryptography/System.Security.Cryptography.X509Certificates.X509AuthorityInformationAccessExtension::EnumerateUris:System.Collections.Generic.IEnumerable<System.String>(System.String)", "codegen", "System.Security.Cryptography.X509Certificates.X509AuthorityInformationAccessExtension", "EnumerateUris", True, "benchmark" },
    { 86, "System.Security.Cryptography/System.Security.Cryptography.X509Certificates.X509AuthorityInformationAccessExtension::EnumerateUris:System.Collections.Generic.IEnumerable<System.String>(System.String)", "codegen", "System.Security.Cryptography.X509Certificates.X509AuthorityInformationAccessExtension", "EnumerateUris", True, "benchmark" },
    { 87, "System.Security.Cryptography/System.Security.Cryptography.X509Certificates.X509AuthorityInformationAccessExtension::EnumerateUris:System.Collections.Generic.IEnumerable<System.String>(System.Security.Cryptography.Oid)", "codegen", "System.Security.Cryptography.X509Certificates.X509AuthorityInformationAccessExtension", "EnumerateUris", True, "benchmark" },
    { 88, "System.Security.Cryptography/System.Security.Cryptography.X509Certificates.X509AuthorityInformationAccessExtension::EnumerateCAIssuersUris:System.Collections.Generic.IEnumerable<System.String>()", "codegen", "System.Security.Cryptography.X509Certificates.X509AuthorityInformationAccessExtension", "EnumerateCAIssuersUris", True, "benchmark" },
    { 89, "System.Security.Cryptography/System.Security.Cryptography.X509Certificates.X509AuthorityInformationAccessExtension::EnumerateOcspUris:System.Collections.Generic.IEnumerable<System.String>()", "codegen", "System.Security.Cryptography.X509Certificates.X509AuthorityInformationAccessExtension", "EnumerateOcspUris", True, "benchmark" },
    { 90, "System.Security.Cryptography/System.Security.Cryptography.X509Certificates.X509AuthorityKeyIdentifierExtension::CopyFrom:System.Void(System.Security.Cryptography.AsnEncodedData)", "codegen", "System.Security.Cryptography.X509Certificates.X509AuthorityKeyIdentifierExtension", "CopyFrom", True, "benchmark" },
    { 91, "System.Security.Cryptography/System.Security.Cryptography.X509Certificates.X509AuthorityKeyIdentifierExtension::CreateFromSubjectKeyIdentifier:System.Security.Cryptography.X509Certificates.X509AuthorityKeyIdentifierExtension(System.Security.Cryptography.X509Certificates.X509SubjectKeyIdentifierExtension)", "codegen", "System.Security.Cryptography.X509Certificates.X509AuthorityKeyIdentifierExtension", "CreateFromSubjectKeyIdentifier", True, "benchmark" },
    { 92, "System.Security.Cryptography/System.Security.Cryptography.X509Certificates.X509AuthorityKeyIdentifierExtension::CreateFromSubjectKeyIdentifier:System.Security.Cryptography.X509Certificates.X509AuthorityKeyIdentifierExtension(System.Byte[])", "codegen", "System.Security.Cryptography.X509Certificates.X509AuthorityKeyIdentifierExtension", "CreateFromSubjectKeyIdentifier", True, "benchmark" },
    { 93, "System.Security.Cryptography/System.Security.Cryptography.X509Certificates.X509AuthorityKeyIdentifierExtension::CreateFromSubjectKeyIdentifier:System.Security.Cryptography.X509Certificates.X509AuthorityKeyIdentifierExtension(System.Byte[])", "codegen", "System.Security.Cryptography.X509Certificates.X509AuthorityKeyIdentifierExtension", "CreateFromSubjectKeyIdentifier", True, "benchmark" },
    { 94, "System.Security.Cryptography/System.Security.Cryptography.X509Certificates.X509AuthorityKeyIdentifierExtension::CreateFromSubjectKeyIdentifier:System.Security.Cryptography.X509Certificates.X509AuthorityKeyIdentifierExtension(System.Byte[])", "codegen", "System.Security.Cryptography.X509Certificates.X509AuthorityKeyIdentifierExtension", "CreateFromSubjectKeyIdentifier", True, "benchmark" },
    { 95, "System.Security.Cryptography/System.Security.Cryptography.X509Certificates.X509AuthorityKeyIdentifierExtension::CreateFromIssuerNameAndSerialNumber:System.Security.Cryptography.X509Certificates.X509AuthorityKeyIdentifierExtension(System.Security.Cryptography.X509Certificates.X500DistinguishedName,System.Byte[])", "codegen", "System.Security.Cryptography.X509Certificates.X509AuthorityKeyIdentifierExtension", "CreateFromIssuerNameAndSerialNumber", True, "benchmark" },
    { 96, "System.Security.Cryptography/System.Security.Cryptography.X509Certificates.X509AuthorityKeyIdentifierExtension::CreateFromIssuerNameAndSerialNumber:System.Security.Cryptography.X509Certificates.X509AuthorityKeyIdentifierExtension(System.Security.Cryptography.X509Certificates.X500DistinguishedName,System.Byte[])", "codegen", "System.Security.Cryptography.X509Certificates.X509AuthorityKeyIdentifierExtension", "CreateFromIssuerNameAndSerialNumber", True, "benchmark" },
    { 97, "System.Security.Cryptography/System.Security.Cryptography.X509Certificates.X509AuthorityKeyIdentifierExtension::CreateFromIssuerNameAndSerialNumber:System.Security.Cryptography.X509Certificates.X509AuthorityKeyIdentifierExtension(System.Security.Cryptography.X509Certificates.X500DistinguishedName,System.Byte[])", "codegen", "System.Security.Cryptography.X509Certificates.X509AuthorityKeyIdentifierExtension", "CreateFromIssuerNameAndSerialNumber", True, "benchmark" },
    { 98, "System.Security.Cryptography/System.Security.Cryptography.X509Certificates.X509AuthorityKeyIdentifierExtension::CreateFromCertificate:System.Security.Cryptography.X509Certificates.X509AuthorityKeyIdentifierExtension(System.Security.Cryptography.X509Certificates.X509Certificate2,System.Boolean,System.Boolean)", "codegen", "System.Security.Cryptography.X509Certificates.X509AuthorityKeyIdentifierExtension", "CreateFromCertificate", True, "benchmark" },
    { 99, "System.Security.Cryptography/System.Security.Cryptography.X509Certificates.X509AuthorityKeyIdentifierExtension::CreateFromCertificate:System.Security.Cryptography.X509Certificates.X509AuthorityKeyIdentifierExtension(System.Security.Cryptography.X509Certificates.X509Certificate2,System.Boolean,System.Boolean)", "codegen", "System.Security.Cryptography.X509Certificates.X509AuthorityKeyIdentifierExtension", "CreateFromCertificate", True, "benchmark" },
    { 100, "System.Security.Cryptography/System.Security.Cryptography.X509Certificates.X509AuthorityKeyIdentifierExtension::CreateFromCertificate:System.Security.Cryptography.X509Certificates.X509AuthorityKeyIdentifierExtension(System.Security.Cryptography.X509Certificates.X509Certificate2,System.Boolean,System.Boolean)", "codegen", "System.Security.Cryptography.X509Certificates.X509AuthorityKeyIdentifierExtension", "CreateFromCertificate", True, "benchmark" },
    { 101, "System.Security.Cryptography/System.Security.Cryptography.X509Certificates.X509BasicConstraintsExtension::CopyFrom:System.Void(System.Security.Cryptography.AsnEncodedData)", "codegen", "System.Security.Cryptography.X509Certificates.X509BasicConstraintsExtension", "CopyFrom", True, "benchmark" },
    { 102, "System.Security.Cryptography/System.Security.Cryptography.X509Certificates.X509BasicConstraintsExtension::CreateForCertificateAuthority:System.Security.Cryptography.X509Certificates.X509BasicConstraintsExtension(System.Nullable<System.Int32>)", "codegen", "System.Security.Cryptography.X509Certificates.X509BasicConstraintsExtension", "CreateForCertificateAuthority", True, "benchmark" },
    { 103, "System.Security.Cryptography/System.Security.Cryptography.X509Certificates.X509BasicConstraintsExtension::CreateForEndEntity:System.Security.Cryptography.X509Certificates.X509BasicConstraintsExtension(System.Boolean)", "codegen", "System.Security.Cryptography.X509Certificates.X509BasicConstraintsExtension", "CreateForEndEntity", True, "benchmark" },
    { 104, "System.Security.Cryptography/System.Security.Cryptography.X509Certificates.X509BasicConstraintsExtension::CreateForEndEntity:System.Security.Cryptography.X509Certificates.X509BasicConstraintsExtension(System.Boolean)", "codegen", "System.Security.Cryptography.X509Certificates.X509BasicConstraintsExtension", "CreateForEndEntity", True, "benchmark" },
    { 105, "System.Security.Cryptography/System.Security.Cryptography.X509Certificates.X509Certificate::CreateFromCertFile:System.Security.Cryptography.X509Certificates.X509Certificate(System.String)", "codegen", "System.Security.Cryptography.X509Certificates.X509Certificate", "CreateFromCertFile", True, "benchmark" },
    { 106, "System.Security.Cryptography/System.Security.Cryptography.X509Certificates.X509Certificate::CreateFromCertFile:System.Security.Cryptography.X509Certificates.X509Certificate(System.String)", "codegen", "System.Security.Cryptography.X509Certificates.X509Certificate", "CreateFromCertFile", True, "benchmark" },
    { 107, "System.Security.Cryptography/System.Security.Cryptography.X509Certificates.X509Certificate::CreateFromSignedFile:System.Security.Cryptography.X509Certificates.X509Certificate(System.String)", "codegen", "System.Security.Cryptography.X509Certificates.X509Certificate", "CreateFromSignedFile", True, "benchmark" },
    { 108, "System.Security.Cryptography/System.Security.Cryptography.X509Certificates.X509Certificate::CreateFromSignedFile:System.Security.Cryptography.X509Certificates.X509Certificate(System.String)", "codegen", "System.Security.Cryptography.X509Certificates.X509Certificate", "CreateFromSignedFile", True, "benchmark" },
    { 109, "System.Security.Cryptography/System.Security.Cryptography.X509Certificates.X509Certificate::Dispose:System.Void()", "codegen", "System.Security.Cryptography.X509Certificates.X509Certificate", "Dispose", True, "benchmark" },
    { 110, "System.Security.Cryptography/System.Security.Cryptography.X509Certificates.X509Certificate::Export:System.Byte[](System.Security.Cryptography.X509Certificates.X509ContentType)", "codegen", "System.Security.Cryptography.X509Certificates.X509Certificate", "Export", True, "benchmark" },
    { 111, "System.Security.Cryptography/System.Security.Cryptography.X509Certificates.X509Certificate::Export:System.Byte[](System.Security.Cryptography.X509Certificates.X509ContentType,System.String)", "codegen", "System.Security.Cryptography.X509Certificates.X509Certificate", "Export", True, "benchmark" },
    { 112, "System.Security.Cryptography/System.Security.Cryptography.X509Certificates.X509Certificate::Export:System.Byte[](System.Security.Cryptography.X509Certificates.X509ContentType,System.String)", "codegen", "System.Security.Cryptography.X509Certificates.X509Certificate", "Export", True, "benchmark" },
    { 113, "System.Security.Cryptography/System.Security.Cryptography.X509Certificates.X509Certificate::Export:System.Byte[](System.Security.Cryptography.X509Certificates.X509ContentType,System.Security.SecureString)", "codegen", "System.Security.Cryptography.X509Certificates.X509Certificate", "Export", True, "benchmark" },
    { 114, "System.Security.Cryptography/System.Security.Cryptography.X509Certificates.X509Certificate::GetRawCertDataString:System.String()", "codegen", "System.Security.Cryptography.X509Certificates.X509Certificate", "GetRawCertDataString", True, "benchmark" },
    { 115, "System.Security.Cryptography/System.Security.Cryptography.X509Certificates.X509Certificate::GetCertHash:System.Byte[]()", "codegen", "System.Security.Cryptography.X509Certificates.X509Certificate", "GetCertHash", True, "benchmark" },
    { 116, "System.Security.Cryptography/System.Security.Cryptography.X509Certificates.X509Certificate::GetCertHash:System.Byte[](System.Security.Cryptography.HashAlgorithmName)", "codegen", "System.Security.Cryptography.X509Certificates.X509Certificate", "GetCertHash", True, "benchmark" },
    { 117, "System.Security.Cryptography/System.Security.Cryptography.X509Certificates.X509Certificate::GetCertHashString:System.String()", "codegen", "System.Security.Cryptography.X509Certificates.X509Certificate", "GetCertHashString", True, "benchmark" },
    { 118, "System.Security.Cryptography/System.Security.Cryptography.X509Certificates.X509Certificate::GetCertHashString:System.String(System.Security.Cryptography.HashAlgorithmName)", "codegen", "System.Security.Cryptography.X509Certificates.X509Certificate", "GetCertHashString", True, "benchmark" },
    { 119, "System.Security.Cryptography/System.Security.Cryptography.X509Certificates.X509Certificate::GetEffectiveDateString:System.String()", "codegen", "System.Security.Cryptography.X509Certificates.X509Certificate", "GetEffectiveDateString", True, "benchmark" },
    { 120, "System.Security.Cryptography/System.Security.Cryptography.X509Certificates.X509Certificate::GetExpirationDateString:System.String()", "codegen", "System.Security.Cryptography.X509Certificates.X509Certificate", "GetExpirationDateString", True, "benchmark" },
    { 121, "System.Security.Cryptography/System.Security.Cryptography.X509Certificates.X509Certificate::GetFormat:System.String()", "codegen", "System.Security.Cryptography.X509Certificates.X509Certificate", "GetFormat", True, "benchmark" },
    { 122, "System.Security.Cryptography/System.Security.Cryptography.X509Certificates.X509Certificate::GetPublicKeyString:System.String()", "codegen", "System.Security.Cryptography.X509Certificates.X509Certificate", "GetPublicKeyString", True, "benchmark" },
    { 123, "System.Security.Cryptography/System.Security.Cryptography.X509Certificates.X509Certificate::GetRawCertData:System.Byte[]()", "codegen", "System.Security.Cryptography.X509Certificates.X509Certificate", "GetRawCertData", True, "benchmark" },
    { 124, "System.Security.Cryptography/System.Security.Cryptography.X509Certificates.X509Certificate::GetKeyAlgorithm:System.String()", "codegen", "System.Security.Cryptography.X509Certificates.X509Certificate", "GetKeyAlgorithm", True, "benchmark" },
    { 125, "System.Security.Cryptography/System.Security.Cryptography.X509Certificates.X509Certificate::GetKeyAlgorithmParameters:System.Byte[]()", "codegen", "System.Security.Cryptography.X509Certificates.X509Certificate", "GetKeyAlgorithmParameters", True, "benchmark" },
    { 126, "System.Security.Cryptography/System.Security.Cryptography.X509Certificates.X509Certificate::GetKeyAlgorithmParametersString:System.String()", "codegen", "System.Security.Cryptography.X509Certificates.X509Certificate", "GetKeyAlgorithmParametersString", True, "benchmark" },
    { 127, "System.Security.Cryptography/System.Security.Cryptography.X509Certificates.X509Certificate::GetPublicKey:System.Byte[]()", "codegen", "System.Security.Cryptography.X509Certificates.X509Certificate", "GetPublicKey", True, "benchmark" },
    { 128, "System.Security.Cryptography/System.Security.Cryptography.X509Certificates.X509Certificate::GetSerialNumber:System.Byte[]()", "codegen", "System.Security.Cryptography.X509Certificates.X509Certificate", "GetSerialNumber", True, "benchmark" },
    { 129, "System.Security.Cryptography/System.Security.Cryptography.X509Certificates.X509Certificate::GetSerialNumberString:System.String()", "codegen", "System.Security.Cryptography.X509Certificates.X509Certificate", "GetSerialNumberString", True, "benchmark" },
    { 130, "System.Security.Cryptography/System.Security.Cryptography.X509Certificates.X509Certificate::GetName:System.String()", "codegen", "System.Security.Cryptography.X509Certificates.X509Certificate", "GetName", True, "benchmark" },
    { 131, "System.Security.Cryptography/System.Security.Cryptography.X509Certificates.X509Certificate::GetIssuerName:System.String()", "codegen", "System.Security.Cryptography.X509Certificates.X509Certificate", "GetIssuerName", True, "benchmark" },
    { 132, "System.Security.Cryptography/System.Security.Cryptography.X509Certificates.X509Certificate::Import:System.Void(System.Byte[])", "codegen", "System.Security.Cryptography.X509Certificates.X509Certificate", "Import", True, "benchmark" },
    { 133, "System.Security.Cryptography/System.Security.Cryptography.X509Certificates.X509Certificate::Import:System.Void(System.Byte[])", "codegen", "System.Security.Cryptography.X509Certificates.X509Certificate", "Import", True, "benchmark" },
    { 134, "System.Security.Cryptography/System.Security.Cryptography.X509Certificates.X509Certificate::Import:System.Void(System.Byte[])", "codegen", "System.Security.Cryptography.X509Certificates.X509Certificate", "Import", True, "benchmark" },
    { 135, "System.Security.Cryptography/System.Security.Cryptography.X509Certificates.X509Certificate::Import:System.Void(System.Byte[],System.String,System.Security.Cryptography.X509Certificates.X509KeyStorageFlags)", "codegen", "System.Security.Cryptography.X509Certificates.X509Certificate", "Import", True, "benchmark" },
    { 136, "System.Security.Cryptography/System.Security.Cryptography.X509Certificates.X509Certificate::Import:System.Void(System.Byte[],System.String,System.Security.Cryptography.X509Certificates.X509KeyStorageFlags)", "codegen", "System.Security.Cryptography.X509Certificates.X509Certificate", "Import", True, "benchmark" },
    { 137, "System.Security.Cryptography/System.Security.Cryptography.X509Certificates.X509Certificate::Import:System.Void(System.Byte[],System.String,System.Security.Cryptography.X509Certificates.X509KeyStorageFlags)", "codegen", "System.Security.Cryptography.X509Certificates.X509Certificate", "Import", True, "benchmark" },
    { 138, "System.Security.Cryptography/System.Security.Cryptography.X509Certificates.X509Certificate::Import:System.Void(System.Byte[],System.String,System.Security.Cryptography.X509Certificates.X509KeyStorageFlags)", "codegen", "System.Security.Cryptography.X509Certificates.X509Certificate", "Import", True, "benchmark" },
    { 139, "System.Security.Cryptography/System.Security.Cryptography.X509Certificates.X509Certificate::Import:System.Void(System.Byte[],System.Security.SecureString,System.Security.Cryptography.X509Certificates.X509KeyStorageFlags)", "codegen", "System.Security.Cryptography.X509Certificates.X509Certificate", "Import", True, "benchmark" },
    { 140, "System.Security.Cryptography/System.Security.Cryptography.X509Certificates.X509Certificate::Import:System.Void(System.Byte[],System.Security.SecureString,System.Security.Cryptography.X509Certificates.X509KeyStorageFlags)", "codegen", "System.Security.Cryptography.X509Certificates.X509Certificate", "Import", True, "benchmark" },
    { 141, "System.Security.Cryptography/System.Security.Cryptography.X509Certificates.X509Certificate::Import:System.Void(System.Byte[],System.Security.SecureString,System.Security.Cryptography.X509Certificates.X509KeyStorageFlags)", "codegen", "System.Security.Cryptography.X509Certificates.X509Certificate", "Import", True, "benchmark" },
    { 142, "System.Security.Cryptography/System.Security.Cryptography.X509Certificates.X509Certificate::Import:System.Void(System.String)", "codegen", "System.Security.Cryptography.X509Certificates.X509Certificate", "Import", True, "benchmark" },
    { 143, "System.Security.Cryptography/System.Security.Cryptography.X509Certificates.X509Certificate::Import:System.Void(System.String)", "codegen", "System.Security.Cryptography.X509Certificates.X509Certificate", "Import", True, "benchmark" },
    { 144, "System.Security.Cryptography/System.Security.Cryptography.X509Certificates.X509Certificate::Import:System.Void(System.String,System.String,System.Security.Cryptography.X509Certificates.X509KeyStorageFlags)", "codegen", "System.Security.Cryptography.X509Certificates.X509Certificate", "Import", True, "benchmark" },
    { 145, "System.Security.Cryptography/System.Security.Cryptography.X509Certificates.X509Certificate::Import:System.Void(System.String,System.String,System.Security.Cryptography.X509Certificates.X509KeyStorageFlags)", "codegen", "System.Security.Cryptography.X509Certificates.X509Certificate", "Import", True, "benchmark" },
    { 146, "System.Security.Cryptography/System.Security.Cryptography.X509Certificates.X509Certificate::Import:System.Void(System.String,System.String,System.Security.Cryptography.X509Certificates.X509KeyStorageFlags)", "codegen", "System.Security.Cryptography.X509Certificates.X509Certificate", "Import", True, "benchmark" },
    { 147, "System.Security.Cryptography/System.Security.Cryptography.X509Certificates.X509Certificate::Import:System.Void(System.String,System.Security.SecureString,System.Security.Cryptography.X509Certificates.X509KeyStorageFlags)", "codegen", "System.Security.Cryptography.X509Certificates.X509Certificate", "Import", True, "benchmark" },
    { 148, "System.Security.Cryptography/System.Security.Cryptography.X509Certificates.X509Certificate::Import:System.Void(System.String,System.Security.SecureString,System.Security.Cryptography.X509Certificates.X509KeyStorageFlags)", "codegen", "System.Security.Cryptography.X509Certificates.X509Certificate", "Import", True, "benchmark" },
    { 149, "System.Security.Cryptography/System.Security.Cryptography.X509Certificates.X509Certificate2::GetCertContentType:System.Security.Cryptography.X509Certificates.X509ContentType(System.Byte[])", "codegen", "System.Security.Cryptography.X509Certificates.X509Certificate2", "GetCertContentType", True, "benchmark" },
    { 150, "System.Security.Cryptography/System.Security.Cryptography.X509Certificates.X509Certificate2::GetCertContentType:System.Security.Cryptography.X509Certificates.X509ContentType(System.Byte[])", "codegen", "System.Security.Cryptography.X509Certificates.X509Certificate2", "GetCertContentType", True, "benchmark" },
    { 151, "System.Security.Cryptography/System.Security.Cryptography.X509Certificates.X509Certificate2::GetCertContentType:System.Security.Cryptography.X509Certificates.X509ContentType(System.Byte[])", "codegen", "System.Security.Cryptography.X509Certificates.X509Certificate2", "GetCertContentType", True, "benchmark" },
    { 152, "System.Security.Cryptography/System.Security.Cryptography.X509Certificates.X509Certificate2::GetCertContentType:System.Security.Cryptography.X509Certificates.X509ContentType(System.String)", "codegen", "System.Security.Cryptography.X509Certificates.X509Certificate2", "GetCertContentType", True, "benchmark" },
    { 153, "System.Security.Cryptography/System.Security.Cryptography.X509Certificates.X509Certificate2::GetCertContentType:System.Security.Cryptography.X509Certificates.X509ContentType(System.String)", "codegen", "System.Security.Cryptography.X509Certificates.X509Certificate2", "GetCertContentType", True, "benchmark" },
    { 154, "System.Security.Cryptography/System.Security.Cryptography.X509Certificates.X509Certificate2::GetNameInfo:System.String(System.Security.Cryptography.X509Certificates.X509NameType,System.Boolean)", "codegen", "System.Security.Cryptography.X509Certificates.X509Certificate2", "GetNameInfo", True, "benchmark" },
    { 155, "System.Security.Cryptography/System.Security.Cryptography.X509Certificates.X509Certificate2::GetNameInfo:System.String(System.Security.Cryptography.X509Certificates.X509NameType,System.Boolean)", "codegen", "System.Security.Cryptography.X509Certificates.X509Certificate2", "GetNameInfo", True, "benchmark" },
    { 156, "System.Security.Cryptography/System.Security.Cryptography.X509Certificates.X509Certificate2::Import:System.Void(System.Byte[])", "codegen", "System.Security.Cryptography.X509Certificates.X509Certificate2", "Import", True, "benchmark" },
    { 157, "System.Security.Cryptography/System.Security.Cryptography.X509Certificates.X509Certificate2::Import:System.Void(System.Byte[])", "codegen", "System.Security.Cryptography.X509Certificates.X509Certificate2", "Import", True, "benchmark" },
    { 158, "System.Security.Cryptography/System.Security.Cryptography.X509Certificates.X509Certificate2::Import:System.Void(System.Byte[])", "codegen", "System.Security.Cryptography.X509Certificates.X509Certificate2", "Import", True, "benchmark" },
    { 159, "System.Security.Cryptography/System.Security.Cryptography.X509Certificates.X509Certificate2::Import:System.Void(System.Byte[],System.String,System.Security.Cryptography.X509Certificates.X509KeyStorageFlags)", "codegen", "System.Security.Cryptography.X509Certificates.X509Certificate2", "Import", True, "benchmark" },
    { 160, "System.Security.Cryptography/System.Security.Cryptography.X509Certificates.X509Certificate2::Import:System.Void(System.Byte[],System.String,System.Security.Cryptography.X509Certificates.X509KeyStorageFlags)", "codegen", "System.Security.Cryptography.X509Certificates.X509Certificate2", "Import", True, "benchmark" },
    { 161, "System.Security.Cryptography/System.Security.Cryptography.X509Certificates.X509Certificate2::Import:System.Void(System.Byte[],System.String,System.Security.Cryptography.X509Certificates.X509KeyStorageFlags)", "codegen", "System.Security.Cryptography.X509Certificates.X509Certificate2", "Import", True, "benchmark" },
    { 162, "System.Security.Cryptography/System.Security.Cryptography.X509Certificates.X509Certificate2::Import:System.Void(System.Byte[],System.String,System.Security.Cryptography.X509Certificates.X509KeyStorageFlags)", "codegen", "System.Security.Cryptography.X509Certificates.X509Certificate2", "Import", True, "benchmark" },
    { 163, "System.Security.Cryptography/System.Security.Cryptography.X509Certificates.X509Certificate2::Import:System.Void(System.Byte[],System.Security.SecureString,System.Security.Cryptography.X509Certificates.X509KeyStorageFlags)", "codegen", "System.Security.Cryptography.X509Certificates.X509Certificate2", "Import", True, "benchmark" },
    { 164, "System.Security.Cryptography/System.Security.Cryptography.X509Certificates.X509Certificate2::Import:System.Void(System.Byte[],System.Security.SecureString,System.Security.Cryptography.X509Certificates.X509KeyStorageFlags)", "codegen", "System.Security.Cryptography.X509Certificates.X509Certificate2", "Import", True, "benchmark" },
    { 165, "System.Security.Cryptography/System.Security.Cryptography.X509Certificates.X509Certificate2::Import:System.Void(System.Byte[],System.Security.SecureString,System.Security.Cryptography.X509Certificates.X509KeyStorageFlags)", "codegen", "System.Security.Cryptography.X509Certificates.X509Certificate2", "Import", True, "benchmark" },
    { 166, "System.Security.Cryptography/System.Security.Cryptography.X509Certificates.X509Certificate2::Import:System.Void(System.String)", "codegen", "System.Security.Cryptography.X509Certificates.X509Certificate2", "Import", True, "benchmark" },
    { 167, "System.Security.Cryptography/System.Security.Cryptography.X509Certificates.X509Certificate2::Import:System.Void(System.String)", "codegen", "System.Security.Cryptography.X509Certificates.X509Certificate2", "Import", True, "benchmark" },
    { 168, "System.Security.Cryptography/System.Security.Cryptography.X509Certificates.X509Certificate2::Import:System.Void(System.String,System.String,System.Security.Cryptography.X509Certificates.X509KeyStorageFlags)", "codegen", "System.Security.Cryptography.X509Certificates.X509Certificate2", "Import", True, "benchmark" },
    { 169, "System.Security.Cryptography/System.Security.Cryptography.X509Certificates.X509Certificate2::Import:System.Void(System.String,System.String,System.Security.Cryptography.X509Certificates.X509KeyStorageFlags)", "codegen", "System.Security.Cryptography.X509Certificates.X509Certificate2", "Import", True, "benchmark" },
    { 170, "System.Security.Cryptography/System.Security.Cryptography.X509Certificates.X509Certificate2::Import:System.Void(System.String,System.String,System.Security.Cryptography.X509Certificates.X509KeyStorageFlags)", "codegen", "System.Security.Cryptography.X509Certificates.X509Certificate2", "Import", True, "benchmark" },
    { 171, "System.Security.Cryptography/System.Security.Cryptography.X509Certificates.X509Certificate2::Import:System.Void(System.String,System.Security.SecureString,System.Security.Cryptography.X509Certificates.X509KeyStorageFlags)", "codegen", "System.Security.Cryptography.X509Certificates.X509Certificate2", "Import", True, "benchmark" },
    { 172, "System.Security.Cryptography/System.Security.Cryptography.X509Certificates.X509Certificate2::Import:System.Void(System.String,System.Security.SecureString,System.Security.Cryptography.X509Certificates.X509KeyStorageFlags)", "codegen", "System.Security.Cryptography.X509Certificates.X509Certificate2", "Import", True, "benchmark" },
    { 173, "System.Security.Cryptography/System.Security.Cryptography.X509Certificates.X509Certificate2::Verify:System.Boolean()", "codegen", "System.Security.Cryptography.X509Certificates.X509Certificate2", "Verify", True, "benchmark" },
    { 174, "System.Security.Cryptography/System.Security.Cryptography.X509Certificates.X509Certificate2::GetECDiffieHellmanPublicKey:System.Security.Cryptography.ECDiffieHellman()", "codegen", "System.Security.Cryptography.X509Certificates.X509Certificate2", "GetECDiffieHellmanPublicKey", True, "benchmark" },
    { 175, "System.Security.Cryptography/System.Security.Cryptography.X509Certificates.X509Certificate2::GetECDiffieHellmanPrivateKey:System.Security.Cryptography.ECDiffieHellman()", "codegen", "System.Security.Cryptography.X509Certificates.X509Certificate2", "GetECDiffieHellmanPrivateKey", True, "benchmark" },
    { 176, "System.Security.Cryptography/System.Security.Cryptography.X509Certificates.X509Certificate2::CopyWithPrivateKey:System.Security.Cryptography.X509Certificates.X509Certificate2(System.Security.Cryptography.ECDiffieHellman)", "codegen", "System.Security.Cryptography.X509Certificates.X509Certificate2", "CopyWithPrivateKey", True, "benchmark" },
    { 177, "System.Security.Cryptography/System.Security.Cryptography.X509Certificates.X509Certificate2::CreateFromPemFile:System.Security.Cryptography.X509Certificates.X509Certificate2(System.String,System.String)", "codegen", "System.Security.Cryptography.X509Certificates.X509Certificate2", "CreateFromPemFile", True, "benchmark" },
    { 178, "System.Security.Cryptography/System.Security.Cryptography.X509Certificates.X509Certificate2::CreateFromPemFile:System.Security.Cryptography.X509Certificates.X509Certificate2(System.String,System.String)", "codegen", "System.Security.Cryptography.X509Certificates.X509Certificate2", "CreateFromPemFile", True, "benchmark" },
    { 179, "System.Security.Cryptography/System.Security.Cryptography.X509Certificates.X509Certificate2::CreateFromPemFile:System.Security.Cryptography.X509Certificates.X509Certificate2(System.String,System.String)", "codegen", "System.Security.Cryptography.X509Certificates.X509Certificate2", "CreateFromPemFile", True, "benchmark" },
    { 180, "System.Security.Cryptography/System.Security.Cryptography.X509Certificates.X509Certificate2::ExportCertificatePem:System.String()", "codegen", "System.Security.Cryptography.X509Certificates.X509Certificate2", "ExportCertificatePem", True, "benchmark" },
    { 181, "System.Security.Cryptography/System.Security.Cryptography.X509Certificates.X509Certificate2::MatchesHostname:System.Boolean(System.String,System.Boolean,System.Boolean)", "codegen", "System.Security.Cryptography.X509Certificates.X509Certificate2", "MatchesHostname", True, "benchmark" },
    { 182, "System.Security.Cryptography/System.Security.Cryptography.X509Certificates.X509Certificate2::MatchesHostname:System.Boolean(System.String,System.Boolean,System.Boolean)", "codegen", "System.Security.Cryptography.X509Certificates.X509Certificate2", "MatchesHostname", True, "benchmark" },
    { 183, "System.Security.Cryptography/System.Security.Cryptography.X509Certificates.X509Certificate2::MatchesHostname:System.Boolean(System.String,System.Boolean,System.Boolean)", "codegen", "System.Security.Cryptography.X509Certificates.X509Certificate2", "MatchesHostname", True, "benchmark" },
    { 184, "System.Security.Cryptography/System.Security.Cryptography.X509Certificates.X509Certificate2::MatchesHostname:System.Boolean(System.String,System.Boolean,System.Boolean)", "codegen", "System.Security.Cryptography.X509Certificates.X509Certificate2", "MatchesHostname", True, "benchmark" },
    { 185, "System.Security.Cryptography/System.Security.Cryptography.X509Certificates.X509Certificate2Collection::Add:System.Int32(System.Security.Cryptography.X509Certificates.X509Certificate2)", "codegen", "System.Security.Cryptography.X509Certificates.X509Certificate2Collection", "Add", True, "benchmark" },
    { 186, "System.Security.Cryptography/System.Security.Cryptography.X509Certificates.X509Certificate2Collection::AddRange:System.Void(System.Security.Cryptography.X509Certificates.X509Certificate2[])", "codegen", "System.Security.Cryptography.X509Certificates.X509Certificate2Collection", "AddRange", True, "benchmark" },
    { 187, "System.Security.Cryptography/System.Security.Cryptography.X509Certificates.X509Certificate2Collection::AddRange:System.Void(System.Security.Cryptography.X509Certificates.X509Certificate2[])", "codegen", "System.Security.Cryptography.X509Certificates.X509Certificate2Collection", "AddRange", True, "benchmark" },
    { 188, "System.Security.Cryptography/System.Security.Cryptography.X509Certificates.X509Certificate2Collection::AddRange:System.Void(System.Security.Cryptography.X509Certificates.X509Certificate2[])", "codegen", "System.Security.Cryptography.X509Certificates.X509Certificate2Collection", "AddRange", True, "benchmark" },
    { 189, "System.Security.Cryptography/System.Security.Cryptography.X509Certificates.X509Certificate2Collection::AddRange:System.Void(System.Security.Cryptography.X509Certificates.X509Certificate2Collection)", "codegen", "System.Security.Cryptography.X509Certificates.X509Certificate2Collection", "AddRange", True, "benchmark" },
    { 190, "System.Security.Cryptography/System.Security.Cryptography.X509Certificates.X509Certificate2Collection::Contains:System.Boolean(System.Security.Cryptography.X509Certificates.X509Certificate2)", "codegen", "System.Security.Cryptography.X509Certificates.X509Certificate2Collection", "Contains", True, "benchmark" },
    { 191, "System.Security.Cryptography/System.Security.Cryptography.X509Certificates.X509Certificate2Collection::Export:System.Byte[](System.Security.Cryptography.X509Certificates.X509ContentType)", "codegen", "System.Security.Cryptography.X509Certificates.X509Certificate2Collection", "Export", True, "benchmark" },
    { 192, "System.Security.Cryptography/System.Security.Cryptography.X509Certificates.X509Certificate2Collection::Export:System.Byte[](System.Security.Cryptography.X509Certificates.X509ContentType,System.String)", "codegen", "System.Security.Cryptography.X509Certificates.X509Certificate2Collection", "Export", True, "benchmark" },
    { 193, "System.Security.Cryptography/System.Security.Cryptography.X509Certificates.X509Certificate2Collection::Export:System.Byte[](System.Security.Cryptography.X509Certificates.X509ContentType,System.String)", "codegen", "System.Security.Cryptography.X509Certificates.X509Certificate2Collection", "Export", True, "benchmark" },
    { 194, "System.Security.Cryptography/System.Security.Cryptography.X509Certificates.X509Certificate2Collection::Find:System.Security.Cryptography.X509Certificates.X509Certificate2Collection(System.Security.Cryptography.X509Certificates.X509FindType,System.Object,System.Boolean)", "codegen", "System.Security.Cryptography.X509Certificates.X509Certificate2Collection", "Find", True, "benchmark" },
    { 195, "System.Security.Cryptography/System.Security.Cryptography.X509Certificates.X509Certificate2Collection::Find:System.Security.Cryptography.X509Certificates.X509Certificate2Collection(System.Security.Cryptography.X509Certificates.X509FindType,System.Object,System.Boolean)", "codegen", "System.Security.Cryptography.X509Certificates.X509Certificate2Collection", "Find", True, "benchmark" },
    { 196, "System.Security.Cryptography/System.Security.Cryptography.X509Certificates.X509Certificate2Collection::GetEnumerator:System.Security.Cryptography.X509Certificates.X509Certificate2Enumerator()", "codegen", "System.Security.Cryptography.X509Certificates.X509Certificate2Collection", "GetEnumerator", True, "benchmark" },
    { 197, "System.Security.Cryptography/System.Security.Cryptography.X509Certificates.X509Certificate2Collection::Import:System.Void(System.Byte[])", "codegen", "System.Security.Cryptography.X509Certificates.X509Certificate2Collection", "Import", True, "benchmark" },
    { 198, "System.Security.Cryptography/System.Security.Cryptography.X509Certificates.X509Certificate2Collection::Import:System.Void(System.Byte[])", "codegen", "System.Security.Cryptography.X509Certificates.X509Certificate2Collection", "Import", True, "benchmark" },
    { 199, "System.Security.Cryptography/System.Security.Cryptography.X509Certificates.X509Certificate2Collection::Import:System.Void(System.Byte[])", "codegen", "System.Security.Cryptography.X509Certificates.X509Certificate2Collection", "Import", True, "benchmark" },
    { 200, "System.Security.Cryptography/System.Security.Cryptography.X509Certificates.X509Certificate2Collection::Import:System.Void(System.Byte[],System.String,System.Security.Cryptography.X509Certificates.X509KeyStorageFlags)", "codegen", "System.Security.Cryptography.X509Certificates.X509Certificate2Collection", "Import", True, "benchmark" },
    { 201, "System.Security.Cryptography/System.Security.Cryptography.X509Certificates.X509Certificate2Collection::Import:System.Void(System.Byte[],System.String,System.Security.Cryptography.X509Certificates.X509KeyStorageFlags)", "codegen", "System.Security.Cryptography.X509Certificates.X509Certificate2Collection", "Import", True, "benchmark" },
    { 202, "System.Security.Cryptography/System.Security.Cryptography.X509Certificates.X509Certificate2Collection::Import:System.Void(System.Byte[],System.String,System.Security.Cryptography.X509Certificates.X509KeyStorageFlags)", "codegen", "System.Security.Cryptography.X509Certificates.X509Certificate2Collection", "Import", True, "benchmark" },
    { 203, "System.Security.Cryptography/System.Security.Cryptography.X509Certificates.X509Certificate2Collection::Import:System.Void(System.Byte[],System.String,System.Security.Cryptography.X509Certificates.X509KeyStorageFlags)", "codegen", "System.Security.Cryptography.X509Certificates.X509Certificate2Collection", "Import", True, "benchmark" },
    { 204, "System.Security.Cryptography/System.Security.Cryptography.X509Certificates.X509Certificate2Collection::Import:System.Void(System.String)", "codegen", "System.Security.Cryptography.X509Certificates.X509Certificate2Collection", "Import", True, "benchmark" },
    { 205, "System.Security.Cryptography/System.Security.Cryptography.X509Certificates.X509Certificate2Collection::Import:System.Void(System.String)", "codegen", "System.Security.Cryptography.X509Certificates.X509Certificate2Collection", "Import", True, "benchmark" },
    { 206, "System.Security.Cryptography/System.Security.Cryptography.X509Certificates.X509Certificate2Collection::Import:System.Void(System.String,System.String,System.Security.Cryptography.X509Certificates.X509KeyStorageFlags)", "codegen", "System.Security.Cryptography.X509Certificates.X509Certificate2Collection", "Import", True, "benchmark" },
    { 207, "System.Security.Cryptography/System.Security.Cryptography.X509Certificates.X509Certificate2Collection::Import:System.Void(System.String,System.String,System.Security.Cryptography.X509Certificates.X509KeyStorageFlags)", "codegen", "System.Security.Cryptography.X509Certificates.X509Certificate2Collection", "Import", True, "benchmark" },
    { 208, "System.Security.Cryptography/System.Security.Cryptography.X509Certificates.X509Certificate2Collection::Import:System.Void(System.String,System.String,System.Security.Cryptography.X509Certificates.X509KeyStorageFlags)", "codegen", "System.Security.Cryptography.X509Certificates.X509Certificate2Collection", "Import", True, "benchmark" },
    { 209, "System.Security.Cryptography/System.Security.Cryptography.X509Certificates.X509Certificate2Collection::Insert:System.Void(System.Int32,System.Security.Cryptography.X509Certificates.X509Certificate2)", "codegen", "System.Security.Cryptography.X509Certificates.X509Certificate2Collection", "Insert", True, "benchmark" },
    { 210, "System.Security.Cryptography/System.Security.Cryptography.X509Certificates.X509Certificate2Collection::Insert:System.Void(System.Int32,System.Security.Cryptography.X509Certificates.X509Certificate2)", "codegen", "System.Security.Cryptography.X509Certificates.X509Certificate2Collection", "Insert", True, "benchmark" },
    { 211, "System.Security.Cryptography/System.Security.Cryptography.X509Certificates.X509Certificate2Collection::Remove:System.Void(System.Security.Cryptography.X509Certificates.X509Certificate2)", "codegen", "System.Security.Cryptography.X509Certificates.X509Certificate2Collection", "Remove", True, "benchmark" },
    { 212, "System.Security.Cryptography/System.Security.Cryptography.X509Certificates.X509Certificate2Collection::RemoveRange:System.Void(System.Security.Cryptography.X509Certificates.X509Certificate2[])", "codegen", "System.Security.Cryptography.X509Certificates.X509Certificate2Collection", "RemoveRange", True, "benchmark" },
    { 213, "System.Security.Cryptography/System.Security.Cryptography.X509Certificates.X509Certificate2Collection::RemoveRange:System.Void(System.Security.Cryptography.X509Certificates.X509Certificate2[])", "codegen", "System.Security.Cryptography.X509Certificates.X509Certificate2Collection", "RemoveRange", True, "benchmark" },
    { 214, "System.Security.Cryptography/System.Security.Cryptography.X509Certificates.X509Certificate2Collection::RemoveRange:System.Void(System.Security.Cryptography.X509Certificates.X509Certificate2[])", "codegen", "System.Security.Cryptography.X509Certificates.X509Certificate2Collection", "RemoveRange", True, "benchmark" },
    { 215, "System.Security.Cryptography/System.Security.Cryptography.X509Certificates.X509Certificate2Collection::RemoveRange:System.Void(System.Security.Cryptography.X509Certificates.X509Certificate2Collection)", "codegen", "System.Security.Cryptography.X509Certificates.X509Certificate2Collection", "RemoveRange", True, "benchmark" },
    { 216, "System.Security.Cryptography/System.Security.Cryptography.X509Certificates.X509Certificate2Collection::ImportFromPemFile:System.Void(System.String)", "codegen", "System.Security.Cryptography.X509Certificates.X509Certificate2Collection", "ImportFromPemFile", True, "benchmark" },
    { 217, "System.Security.Cryptography/System.Security.Cryptography.X509Certificates.X509Certificate2Collection::ImportFromPemFile:System.Void(System.String)", "codegen", "System.Security.Cryptography.X509Certificates.X509Certificate2Collection", "ImportFromPemFile", True, "benchmark" },
    { 218, "System.Security.Cryptography/System.Security.Cryptography.X509Certificates.X509Certificate2Collection::ExportPkcs7Pem:System.String()", "codegen", "System.Security.Cryptography.X509Certificates.X509Certificate2Collection", "ExportPkcs7Pem", True, "benchmark" },
    { 219, "System.Security.Cryptography/System.Security.Cryptography.X509Certificates.X509Certificate2Collection::ExportCertificatePems:System.String()", "codegen", "System.Security.Cryptography.X509Certificates.X509Certificate2Collection", "ExportCertificatePems", True, "benchmark" },
    { 220, "System.Security.Cryptography/System.Security.Cryptography.X509Certificates.X509CertificateCollection::Add:System.Int32(System.Security.Cryptography.X509Certificates.X509Certificate)", "codegen", "System.Security.Cryptography.X509Certificates.X509CertificateCollection", "Add", True, "benchmark" },
    { 221, "System.Security.Cryptography/System.Security.Cryptography.X509Certificates.X509CertificateCollection::AddRange:System.Void(System.Security.Cryptography.X509Certificates.X509Certificate[])", "codegen", "System.Security.Cryptography.X509Certificates.X509CertificateCollection", "AddRange", True, "benchmark" },
    { 222, "System.Security.Cryptography/System.Security.Cryptography.X509Certificates.X509CertificateCollection::AddRange:System.Void(System.Security.Cryptography.X509Certificates.X509Certificate[])", "codegen", "System.Security.Cryptography.X509Certificates.X509CertificateCollection", "AddRange", True, "benchmark" },
    { 223, "System.Security.Cryptography/System.Security.Cryptography.X509Certificates.X509CertificateCollection::AddRange:System.Void(System.Security.Cryptography.X509Certificates.X509Certificate[])", "codegen", "System.Security.Cryptography.X509Certificates.X509CertificateCollection", "AddRange", True, "benchmark" },
    { 224, "System.Security.Cryptography/System.Security.Cryptography.X509Certificates.X509CertificateCollection::AddRange:System.Void(System.Security.Cryptography.X509Certificates.X509CertificateCollection)", "codegen", "System.Security.Cryptography.X509Certificates.X509CertificateCollection", "AddRange", True, "benchmark" },
    { 225, "System.Security.Cryptography/System.Security.Cryptography.X509Certificates.X509CertificateCollection::Contains:System.Boolean(System.Security.Cryptography.X509Certificates.X509Certificate)", "codegen", "System.Security.Cryptography.X509Certificates.X509CertificateCollection", "Contains", True, "benchmark" },
    { 226, "System.Security.Cryptography/System.Security.Cryptography.X509Certificates.X509CertificateCollection::GetEnumerator:X509CertificateCollection.X509CertificateEnumerator()", "codegen", "System.Security.Cryptography.X509Certificates.X509CertificateCollection", "GetEnumerator", True, "benchmark" },
    { 227, "System.Security.Cryptography/System.Security.Cryptography.X509Certificates.X509CertificateCollection::IndexOf:System.Int32(System.Security.Cryptography.X509Certificates.X509Certificate)", "codegen", "System.Security.Cryptography.X509Certificates.X509CertificateCollection", "IndexOf", True, "benchmark" },
    { 228, "System.Security.Cryptography/System.Security.Cryptography.X509Certificates.X509CertificateCollection::Insert:System.Void(System.Int32,System.Security.Cryptography.X509Certificates.X509Certificate)", "codegen", "System.Security.Cryptography.X509Certificates.X509CertificateCollection", "Insert", True, "benchmark" },
    { 229, "System.Security.Cryptography/System.Security.Cryptography.X509Certificates.X509CertificateCollection::Insert:System.Void(System.Int32,System.Security.Cryptography.X509Certificates.X509Certificate)", "codegen", "System.Security.Cryptography.X509Certificates.X509CertificateCollection", "Insert", True, "benchmark" },
    { 230, "System.Security.Cryptography/System.Security.Cryptography.X509Certificates.X509CertificateCollection::Remove:System.Void(System.Security.Cryptography.X509Certificates.X509Certificate)", "codegen", "System.Security.Cryptography.X509Certificates.X509CertificateCollection", "Remove", True, "benchmark" },
    { 231, "System.Security.Cryptography/System.Security.Cryptography.X509Certificates.X509CertificateLoader::LoadCertificate:System.Security.Cryptography.X509Certificates.X509Certificate2(System.Byte[])", "codegen", "System.Security.Cryptography.X509Certificates.X509CertificateLoader", "LoadCertificate", True, "benchmark" },
    { 232, "System.Security.Cryptography/System.Security.Cryptography.X509Certificates.X509CertificateLoader::LoadCertificate:System.Security.Cryptography.X509Certificates.X509Certificate2(System.Byte[])", "codegen", "System.Security.Cryptography.X509Certificates.X509CertificateLoader", "LoadCertificate", True, "benchmark" },
    { 233, "System.Security.Cryptography/System.Security.Cryptography.X509Certificates.X509CertificateLoader::LoadCertificate:System.Security.Cryptography.X509Certificates.X509Certificate2(System.Byte[])", "codegen", "System.Security.Cryptography.X509Certificates.X509CertificateLoader", "LoadCertificate", True, "benchmark" },
    { 234, "System.Security.Cryptography/System.Security.Cryptography.X509Certificates.X509CertificateLoader::LoadCertificateFromFile:System.Security.Cryptography.X509Certificates.X509Certificate2(System.String)", "codegen", "System.Security.Cryptography.X509Certificates.X509CertificateLoader", "LoadCertificateFromFile", True, "benchmark" },
    { 235, "System.Security.Cryptography/System.Security.Cryptography.X509Certificates.X509CertificateLoader::LoadCertificateFromFile:System.Security.Cryptography.X509Certificates.X509Certificate2(System.String)", "codegen", "System.Security.Cryptography.X509Certificates.X509CertificateLoader", "LoadCertificateFromFile", True, "benchmark" },
    { 236, "System.Security.Cryptography/System.Security.Cryptography.X509Certificates.X509CertificateLoader::LoadPkcs12:System.Security.Cryptography.X509Certificates.X509Certificate2(System.Byte[],System.String,System.Security.Cryptography.X509Certificates.X509KeyStorageFlags,System.Security.Cryptography.X509Certificates.Pkcs12LoaderLimits)", "codegen", "System.Security.Cryptography.X509Certificates.X509CertificateLoader", "LoadPkcs12", True, "benchmark" },
    { 237, "System.Security.Cryptography/System.Security.Cryptography.X509Certificates.X509CertificateLoader::LoadPkcs12:System.Security.Cryptography.X509Certificates.X509Certificate2(System.Byte[],System.String,System.Security.Cryptography.X509Certificates.X509KeyStorageFlags,System.Security.Cryptography.X509Certificates.Pkcs12LoaderLimits)", "codegen", "System.Security.Cryptography.X509Certificates.X509CertificateLoader", "LoadPkcs12", True, "benchmark" },
    { 238, "System.Security.Cryptography/System.Security.Cryptography.X509Certificates.X509CertificateLoader::LoadPkcs12:System.Security.Cryptography.X509Certificates.X509Certificate2(System.Byte[],System.String,System.Security.Cryptography.X509Certificates.X509KeyStorageFlags,System.Security.Cryptography.X509Certificates.Pkcs12LoaderLimits)", "codegen", "System.Security.Cryptography.X509Certificates.X509CertificateLoader", "LoadPkcs12", True, "benchmark" },
    { 239, "System.Security.Cryptography/System.Security.Cryptography.X509Certificates.X509CertificateLoader::LoadPkcs12:System.Security.Cryptography.X509Certificates.X509Certificate2(System.Byte[],System.String,System.Security.Cryptography.X509Certificates.X509KeyStorageFlags,System.Security.Cryptography.X509Certificates.Pkcs12LoaderLimits)", "codegen", "System.Security.Cryptography.X509Certificates.X509CertificateLoader", "LoadPkcs12", True, "benchmark" },
    { 240, "System.Security.Cryptography/System.Security.Cryptography.X509Certificates.X509CertificateLoader::LoadPkcs12FromFile:System.Security.Cryptography.X509Certificates.X509Certificate2(System.String,System.String,System.Security.Cryptography.X509Certificates.X509KeyStorageFlags,System.Security.Cryptography.X509Certificates.Pkcs12LoaderLimits)", "codegen", "System.Security.Cryptography.X509Certificates.X509CertificateLoader", "LoadPkcs12FromFile", True, "benchmark" },
    { 241, "System.Security.Cryptography/System.Security.Cryptography.X509Certificates.X509CertificateLoader::LoadPkcs12FromFile:System.Security.Cryptography.X509Certificates.X509Certificate2(System.String,System.String,System.Security.Cryptography.X509Certificates.X509KeyStorageFlags,System.Security.Cryptography.X509Certificates.Pkcs12LoaderLimits)", "codegen", "System.Security.Cryptography.X509Certificates.X509CertificateLoader", "LoadPkcs12FromFile", True, "benchmark" },
    { 242, "System.Security.Cryptography/System.Security.Cryptography.X509Certificates.X509CertificateLoader::LoadPkcs12FromFile:System.Security.Cryptography.X509Certificates.X509Certificate2(System.String,System.String,System.Security.Cryptography.X509Certificates.X509KeyStorageFlags,System.Security.Cryptography.X509Certificates.Pkcs12LoaderLimits)", "codegen", "System.Security.Cryptography.X509Certificates.X509CertificateLoader", "LoadPkcs12FromFile", True, "benchmark" },
    { 243, "System.Security.Cryptography/System.Security.Cryptography.X509Certificates.X509CertificateLoader::LoadPkcs12Collection:System.Security.Cryptography.X509Certificates.X509Certificate2Collection(System.Byte[],System.String,System.Security.Cryptography.X509Certificates.X509KeyStorageFlags,System.Security.Cryptography.X509Certificates.Pkcs12LoaderLimits)", "codegen", "System.Security.Cryptography.X509Certificates.X509CertificateLoader", "LoadPkcs12Collection", True, "benchmark" },
    { 244, "System.Security.Cryptography/System.Security.Cryptography.X509Certificates.X509CertificateLoader::LoadPkcs12Collection:System.Security.Cryptography.X509Certificates.X509Certificate2Collection(System.Byte[],System.String,System.Security.Cryptography.X509Certificates.X509KeyStorageFlags,System.Security.Cryptography.X509Certificates.Pkcs12LoaderLimits)", "codegen", "System.Security.Cryptography.X509Certificates.X509CertificateLoader", "LoadPkcs12Collection", True, "benchmark" },
    { 245, "System.Security.Cryptography/System.Security.Cryptography.X509Certificates.X509CertificateLoader::LoadPkcs12Collection:System.Security.Cryptography.X509Certificates.X509Certificate2Collection(System.Byte[],System.String,System.Security.Cryptography.X509Certificates.X509KeyStorageFlags,System.Security.Cryptography.X509Certificates.Pkcs12LoaderLimits)", "codegen", "System.Security.Cryptography.X509Certificates.X509CertificateLoader", "LoadPkcs12Collection", True, "benchmark" },
    { 246, "System.Security.Cryptography/System.Security.Cryptography.X509Certificates.X509CertificateLoader::LoadPkcs12Collection:System.Security.Cryptography.X509Certificates.X509Certificate2Collection(System.Byte[],System.String,System.Security.Cryptography.X509Certificates.X509KeyStorageFlags,System.Security.Cryptography.X509Certificates.Pkcs12LoaderLimits)", "codegen", "System.Security.Cryptography.X509Certificates.X509CertificateLoader", "LoadPkcs12Collection", True, "benchmark" },
    { 247, "System.Security.Cryptography/System.Security.Cryptography.X509Certificates.X509CertificateLoader::LoadPkcs12CollectionFromFile:System.Security.Cryptography.X509Certificates.X509Certificate2Collection(System.String,System.String,System.Security.Cryptography.X509Certificates.X509KeyStorageFlags,System.Security.Cryptography.X509Certificates.Pkcs12LoaderLimits)", "codegen", "System.Security.Cryptography.X509Certificates.X509CertificateLoader", "LoadPkcs12CollectionFromFile", True, "benchmark" },
    { 248, "System.Security.Cryptography/System.Security.Cryptography.X509Certificates.X509CertificateLoader::LoadPkcs12CollectionFromFile:System.Security.Cryptography.X509Certificates.X509Certificate2Collection(System.String,System.String,System.Security.Cryptography.X509Certificates.X509KeyStorageFlags,System.Security.Cryptography.X509Certificates.Pkcs12LoaderLimits)", "codegen", "System.Security.Cryptography.X509Certificates.X509CertificateLoader", "LoadPkcs12CollectionFromFile", True, "benchmark" },
    { 249, "System.Security.Cryptography/System.Security.Cryptography.X509Certificates.X509CertificateLoader::LoadPkcs12CollectionFromFile:System.Security.Cryptography.X509Certificates.X509Certificate2Collection(System.String,System.String,System.Security.Cryptography.X509Certificates.X509KeyStorageFlags,System.Security.Cryptography.X509Certificates.Pkcs12LoaderLimits)", "codegen", "System.Security.Cryptography.X509Certificates.X509CertificateLoader", "LoadPkcs12CollectionFromFile", True, "benchmark" },
    { 250, "System.Security.Cryptography/System.Security.Cryptography.X509Certificates.X509Chain::Build:System.Boolean(System.Security.Cryptography.X509Certificates.X509Certificate2)", "codegen", "System.Security.Cryptography.X509Certificates.X509Chain", "Build", True, "benchmark" },
    { 251, "System.Security.Cryptography/System.Security.Cryptography.X509Certificates.X509Chain::Dispose:System.Void()", "codegen", "System.Security.Cryptography.X509Certificates.X509Chain", "Dispose", True, "benchmark" },
    { 252, "System.Security.Cryptography/System.Security.Cryptography.X509Certificates.X509ChainElementCollection::GetEnumerator:System.Security.Cryptography.X509Certificates.X509ChainElementEnumerator()", "codegen", "System.Security.Cryptography.X509Certificates.X509ChainElementCollection", "GetEnumerator", True, "benchmark" },
    { 253, "System.Security.Cryptography/System.Security.Cryptography.X509Certificates.X509ChainPolicy::Clone:System.Security.Cryptography.X509Certificates.X509ChainPolicy()", "codegen", "System.Security.Cryptography.X509Certificates.X509ChainPolicy", "Clone", True, "benchmark" },
    { 254, "System.Security.Cryptography/System.Security.Cryptography.X509Certificates.X509EnhancedKeyUsageExtension::CopyFrom:System.Void(System.Security.Cryptography.AsnEncodedData)", "codegen", "System.Security.Cryptography.X509Certificates.X509EnhancedKeyUsageExtension", "CopyFrom", True, "benchmark" },
    { 255, "System.Security.Cryptography/System.Security.Cryptography.X509Certificates.X509Extension::CopyFrom:System.Void(System.Security.Cryptography.AsnEncodedData)", "codegen", "System.Security.Cryptography.X509Certificates.X509Extension", "CopyFrom", True, "benchmark" },
    { 256, "System.Security.Cryptography/System.Security.Cryptography.X509Certificates.X509ExtensionCollection::Add:System.Int32(System.Security.Cryptography.X509Certificates.X509Extension)", "codegen", "System.Security.Cryptography.X509Certificates.X509ExtensionCollection", "Add", True, "benchmark" },
    { 257, "System.Security.Cryptography/System.Security.Cryptography.X509Certificates.X509ExtensionCollection::GetEnumerator:System.Security.Cryptography.X509Certificates.X509ExtensionEnumerator()", "codegen", "System.Security.Cryptography.X509Certificates.X509ExtensionCollection", "GetEnumerator", True, "benchmark" },
    { 258, "System.Security.Cryptography/System.Security.Cryptography.X509Certificates.X509KeyUsageExtension::CopyFrom:System.Void(System.Security.Cryptography.AsnEncodedData)", "codegen", "System.Security.Cryptography.X509Certificates.X509KeyUsageExtension", "CopyFrom", True, "benchmark" },
    { 259, "System.Security.Cryptography/System.Security.Cryptography.X509Certificates.X509SignatureGenerator::GetSignatureAlgorithmIdentifier:System.Byte[](System.Security.Cryptography.HashAlgorithmName)", "codegen", "System.Security.Cryptography.X509Certificates.X509SignatureGenerator", "GetSignatureAlgorithmIdentifier", True, "benchmark" },
    { 260, "System.Security.Cryptography/System.Security.Cryptography.X509Certificates.X509SignatureGenerator::SignData:System.Byte[](System.Byte[],System.Security.Cryptography.HashAlgorithmName)", "codegen", "System.Security.Cryptography.X509Certificates.X509SignatureGenerator", "SignData", True, "benchmark" },
    { 261, "System.Security.Cryptography/System.Security.Cryptography.X509Certificates.X509SignatureGenerator::SignData:System.Byte[](System.Byte[],System.Security.Cryptography.HashAlgorithmName)", "codegen", "System.Security.Cryptography.X509Certificates.X509SignatureGenerator", "SignData", True, "benchmark" },
    { 262, "System.Security.Cryptography/System.Security.Cryptography.X509Certificates.X509SignatureGenerator::SignData:System.Byte[](System.Byte[],System.Security.Cryptography.HashAlgorithmName)", "codegen", "System.Security.Cryptography.X509Certificates.X509SignatureGenerator", "SignData", True, "benchmark" },
    { 263, "System.Security.Cryptography/System.Security.Cryptography.X509Certificates.X509SignatureGenerator::CreateForECDsa:System.Security.Cryptography.X509Certificates.X509SignatureGenerator(System.Security.Cryptography.ECDsa)", "codegen", "System.Security.Cryptography.X509Certificates.X509SignatureGenerator", "CreateForECDsa", True, "benchmark" },
    { 264, "System.Security.Cryptography/System.Security.Cryptography.X509Certificates.X509SignatureGenerator::CreateForRSA:System.Security.Cryptography.X509Certificates.X509SignatureGenerator(System.Security.Cryptography.RSA,System.Security.Cryptography.RSASignaturePadding)", "codegen", "System.Security.Cryptography.X509Certificates.X509SignatureGenerator", "CreateForRSA", True, "benchmark" },
    { 265, "System.Security.Cryptography/System.Security.Cryptography.X509Certificates.X509Store::Open:System.Void(System.Security.Cryptography.X509Certificates.OpenFlags)", "codegen", "System.Security.Cryptography.X509Certificates.X509Store", "Open", True, "benchmark" },
    { 266, "System.Security.Cryptography/System.Security.Cryptography.X509Certificates.X509Store::Add:System.Void(System.Security.Cryptography.X509Certificates.X509Certificate2)", "codegen", "System.Security.Cryptography.X509Certificates.X509Store", "Add", True, "benchmark" },
    { 267, "System.Security.Cryptography/System.Security.Cryptography.X509Certificates.X509Store::AddRange:System.Void(System.Security.Cryptography.X509Certificates.X509Certificate2Collection)", "codegen", "System.Security.Cryptography.X509Certificates.X509Store", "AddRange", True, "benchmark" },
    { 268, "System.Security.Cryptography/System.Security.Cryptography.X509Certificates.X509Store::Remove:System.Void(System.Security.Cryptography.X509Certificates.X509Certificate2)", "codegen", "System.Security.Cryptography.X509Certificates.X509Store", "Remove", True, "benchmark" },
    { 269, "System.Security.Cryptography/System.Security.Cryptography.X509Certificates.X509Store::RemoveRange:System.Void(System.Security.Cryptography.X509Certificates.X509Certificate2Collection)", "codegen", "System.Security.Cryptography.X509Certificates.X509Store", "RemoveRange", True, "benchmark" },
    { 270, "System.Security.Cryptography/System.Security.Cryptography.X509Certificates.X509Store::Dispose:System.Void()", "codegen", "System.Security.Cryptography.X509Certificates.X509Store", "Dispose", True, "benchmark" },
    { 271, "System.Security.Cryptography/System.Security.Cryptography.X509Certificates.X509Store::Close:System.Void()", "codegen", "System.Security.Cryptography.X509Certificates.X509Store", "Close", True, "benchmark" },
    { 272, "System.Security.Cryptography/System.Security.Cryptography.X509Certificates.X509SubjectAlternativeNameExtension::CopyFrom:System.Void(System.Security.Cryptography.AsnEncodedData)", "codegen", "System.Security.Cryptography.X509Certificates.X509SubjectAlternativeNameExtension", "CopyFrom", True, "benchmark" },
    { 273, "System.Security.Cryptography/System.Security.Cryptography.X509Certificates.X509SubjectAlternativeNameExtension::EnumerateDnsNames:System.Collections.Generic.IEnumerable<System.String>()", "codegen", "System.Security.Cryptography.X509Certificates.X509SubjectAlternativeNameExtension", "EnumerateDnsNames", True, "benchmark" },
    { 274, "System.Security.Cryptography/System.Security.Cryptography.X509Certificates.X509SubjectAlternativeNameExtension::EnumerateIPAddresses:System.Collections.Generic.IEnumerable<System.Net.IPAddress>()", "codegen", "System.Security.Cryptography.X509Certificates.X509SubjectAlternativeNameExtension", "EnumerateIPAddresses", True, "benchmark" },
    { 275, "System.Security.Cryptography/System.Security.Cryptography.X509Certificates.X509SubjectKeyIdentifierExtension::CopyFrom:System.Void(System.Security.Cryptography.AsnEncodedData)", "codegen", "System.Security.Cryptography.X509Certificates.X509SubjectKeyIdentifierExtension", "CopyFrom", True, "benchmark" },

};

static const int kSubjectEntryCount_Generated = 276;

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