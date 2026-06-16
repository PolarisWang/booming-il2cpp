#include <cstring>
#include "runtime_stubs/crypto_stubs.h"

namespace chaos::il2cpp::runtime_core {

// -- External Runtime Dispatch Table Resolution ----------------------------
// Resolves subjectIds -> function pointers for the codegen-emitted
// kChaosExternalRuntimeFnTable.  Uses the HotpatchNameRegistry which is
// already populated during bootstrap with all AOT modules' dispatch tables.
//
// Called from BootstrapRuntime() after all hotpatch modules are registered.
// The function pointers come from HotpatchEntryV0::direct_ptr, giving O(1)
// dispatch at call sites without per-method C++ wrapper stubs.

static void ParseSubjectIdForHotpatchLookup(
    const char* subject_id,
    std::string& out_ns,
    std::string& out_type_name,
    std::string& out_method_name) noexcept
{
    out_ns.clear();
    out_type_name.clear();
    out_method_name.clear();

    if (subject_id == nullptr) return;

    // Format: "AssemblyName/Namespace.TypeName:MethodName(Params...)"
    // Find '/' separator between assembly name and type path
    const char* type_start = std::strchr(subject_id, '/');
    if (type_start == nullptr) return;
    ++type_start; // skip '/'

    // Find "::" between type name and method name
    const char* method_start = std::strstr(type_start, "::");
    if (method_start == nullptr) return;

    // Find namespace boundary: last '.' before "::"
    const char* type_name_begin = type_start;
    for (const char* p = type_start; p < method_start; ++p) {
        if (*p == '.') type_name_begin = p + 1;
    }

    // Namespace: everything between '/' and last '.' before type name
    if (type_name_begin > type_start + 1) {
        // +1 for the '/', -1 for the trailing '.'
        out_ns.assign(type_start, type_name_begin - type_start - 1);
    } else {
        out_ns.assign(type_start, type_name_begin - type_start);
    }

    // Type name: from type_name_begin to "::"
    out_type_name.assign(type_name_begin, method_start - type_name_begin);

    // Method name: from after "::" to '('
    const char* paren = std::strchr(method_start + 2, '(');
    const char* method_name_end = (paren == nullptr)
        ? (method_start + 2 + std::strlen(method_start + 2))
        : paren;

    // Strip return-type suffix (":ReturnType") if present -- the hotpatch
    // registry stores bare method names (e.g. "Run") while subject IDs
    // include the return type (e.g. "Run:System.Int32").
    const char* colon = nullptr;
    for (const char* p = method_start + 2; p < method_name_end; ++p) {
        if (*p == ':') { colon = p; break; }
    }
    if (colon != nullptr) {
        out_method_name.assign(method_start + 2, colon - method_start - 2);
    } else if (paren == nullptr) {
        out_method_name.assign(method_start + 2);
    } else {
        out_method_name.assign(method_start + 2, paren - method_start - 2);
    }
}

// ── Interop stub declarations (extern "C" must be at namespace scope) ──
extern "C" int ChaosMarshalGetHRForLastWin32Error() noexcept;
extern "C" int ChaosMarshalGetLastPInvokeError() noexcept;
extern "C" CHAOS_IL2CPP_INTPTR ChaosBitOperationsIsPow2Impl(CHAOS_IL2CPP_INTPTR value) noexcept;
// SIMD stubs for Vector128/256 methods (defined in simd_stubs.cpp)
#include "runtime_stubs/simd_stubs.h"
extern "C" CHAOS_IL2CPP_INTPTR ChaosBitOperationsPopCount(CHAOS_IL2CPP_INTPTR value) noexcept;
extern "C" CHAOS_IL2CPP_INTPTR ChaosBitOperationsLeadingZeroCount(CHAOS_IL2CPP_INTPTR value) noexcept;
extern "C" CHAOS_IL2CPP_INTPTR ChaosBitOperationsLog2(CHAOS_IL2CPP_INTPTR value) noexcept;
extern "C" const char* const kChaosExternalRuntimeSubjects[];

extern "C" void ChaosResolveExternalRuntimeFnTable() noexcept
{
    if (kChaosExternalRuntimeCount <= 0) return;

    auto& registry = chaos::il2cpp::runtime_core::GetHotpatchNameRegistry();

    std::string ns, type_name, method_name;

    for (int32_t i = 0; i < kChaosExternalRuntimeCount; ++i) {
        const char* subject_id = kChaosExternalRuntimeSubjects[i];
        if (subject_id == nullptr || subject_id[0] == '\0') continue;

        // Parse subjectId into hotpatch lookup components
        ParseSubjectIdForHotpatchLookup(subject_id, ns, type_name, method_name);
        if (type_name.empty() || method_name.empty()) continue;

        // Look up across all registered hotpatch modules
        uint64_t result = registry.LookupMethod(
            ns.c_str(), type_name.c_str(), method_name.c_str());
        if (result == 0) continue;

        uint32_t module_index = ExtractModuleId(result);
        uint32_t token = ExtractToken(result);

        if (module_index >= registry.ModuleCount()) continue;

        uint32_t slot = registry.TokenToSlot(module_index, token);
        if (slot == ~0u) continue;

        auto* entry = registry.GetDispatchEntryBySlot(module_index, slot);
        if (entry != nullptr && entry->direct_ptr != nullptr) {
            kChaosExternalRuntimeFnTable[i] = entry->direct_ptr;
        }
    }

    // ── Fallback: hardcoded interop stubs for unresolvable entries ──
    // The codegen may fail to produce AOT code for some managed methods,
    // leaving their kChaosExternalRuntimeFnTable entry as nullptr.  Calling
    // through a null entry causes AV.  Provide fallback implementations
    // for well-known methods that have native stubs in interop_stubs.cpp.
    for (int32_t i = 0; i < kChaosExternalRuntimeCount; ++i) {
        if (kChaosExternalRuntimeFnTable[i] != nullptr)
            continue;
        const char* sid = static_cast<const char*>(kChaosExternalRuntimeSubjects[i]);
        if (sid == nullptr) continue;

        if (std::strstr(sid, "::GetHRForLastWin32Error") != nullptr) {
            kChaosExternalRuntimeFnTable[i] =
                reinterpret_cast<void*>(ChaosMarshalGetHRForLastWin32Error);
        } else if (std::strstr(sid, "::GetLastPInvokeError") != nullptr) {
            kChaosExternalRuntimeFnTable[i] =
                reinterpret_cast<void*>(ChaosMarshalGetLastPInvokeError);
        } else if (std::strstr(sid, "Interop+BCrypt") != nullptr ||
                   std::strstr(sid, "Interop+NCrypt") != nullptr) {
            // Route BCrypt/NCrypt P/Invoke calls to native stub implementations.
            // Only BCrypt stubs are currently implemented (see crypto_stubs.cpp).
            // NCrypt entries without stubs remain nullptr -> will throw at runtime.
            #define BCROUTE(name) do { \
                if (std::strstr(sid, "::" #name ":")) { \
                    kChaosExternalRuntimeFnTable[i] = \
                        reinterpret_cast<void*>(Chaos##name); \
                    break; \
                } \
            } while(0)

            // ── Algorithm provider ──
            BCROUTE(BCryptOpenAlgorithmProvider);
            BCROUTE(BCryptCloseAlgorithmProvider);

            // ── Hash ──
            BCROUTE(BCryptCreateHash);
            BCROUTE(BCryptDestroyHash);
            BCROUTE(BCryptHashData);
            BCROUTE(BCryptFinishHash);
            BCROUTE(BCryptHash);

            // ── Symmetric key ──
            BCROUTE(BCryptGenerateSymmetricKey);
            BCROUTE(BCryptDestroyKey);
            BCROUTE(BCryptEncrypt);
            BCROUTE(BCryptDecrypt);
            BCROUTE(BCryptImportKey);
            BCROUTE(BCryptExportKey);

            // ── Key properties ──
            BCROUTE(BCryptGetProperty);
            BCROUTE(BCryptSetProperty);

            // ── Asymmetric key ──
            BCROUTE(BCryptGenerateKeyPair);
            BCROUTE(BCryptFinalizeKeyPair);
            BCROUTE(BCryptImportKeyPair);

            // ── Signatures ──
            BCROUTE(BCryptSignHash);
            BCROUTE(BCryptVerifySignature);

            // ── Secret agreement (ECDH) ──
            BCROUTE(BCryptSecretAgreement);
            BCROUTE(BCryptDestroySecret);
            BCROUTE(BCryptDeriveKey);
            BCROUTE(BCryptKeyDerivation);

            // ── RNG ──
            BCROUTE(BCryptGenRandom);

            // ── Capabilities ──
            BCROUTE(BCryptIsAvailable);

            #undef BCROUTE
        }

        // ── BitOperations stubs ─────────────────────────────────────────
        // IsPow2 uses popcount/LZCNT in .NET JIT; our AOT falls through to
        // the interpreter.  Provide a native stub for ~270x faster execution.
        if (std::strstr(sid, "BitOperations::IsPow2:") != nullptr) {
            kChaosExternalRuntimeFnTable[i] =
                reinterpret_cast<void*>(ChaosBitOperationsIsPow2Impl);
        }
        if (std::strstr(sid, "BitOperations::PopCount:") != nullptr) {
            kChaosExternalRuntimeFnTable[i] =
                reinterpret_cast<void*>(ChaosBitOperationsPopCount);
        }
        if (std::strstr(sid, "BitOperations::LeadingZeroCount:") != nullptr) {
            kChaosExternalRuntimeFnTable[i] =
                reinterpret_cast<void*>(ChaosBitOperationsLeadingZeroCount);
        }
        if (std::strstr(sid, "BitOperations::Log2:") != nullptr) {
            kChaosExternalRuntimeFnTable[i] =
                reinterpret_cast<void*>(ChaosBitOperationsLog2);
        }
    }
}

}  // namespace chaos::il2cpp::runtime_core
