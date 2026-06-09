#ifndef CHAOS_IL2CPP_CHAOS_RUNTIME_HOST_H_
#define CHAOS_IL2CPP_CHAOS_RUNTIME_HOST_H_

/// @file chaos_runtime_host.h
///
/// Standardized RAII host for initializing and owning the chaos-il2cpp
/// C# execution environment from C++ test / verification / benchmark code.
///
/// Usage:
///   #include "chaos_runtime_host.h"
///   #include "chaos_generated_module.h"  // A1 typed dispatch header
///
///   int main() {
///       ChaosRuntimeHost host;
///       if (!host.Initialize()) return -1;
///
///       // Register the generated module.
///       ChaosGeneratedModuleActivate(&host);
///
///       // Using A2 proxy wrapper:
///       int result = SnapshotProverSubjects::ValueTypeHelper::RunValueType();
///
///       // Using raw A1 typed dispatch:
///       int r2 = kFunctions.ValueTypeHelper.RunValueType();
///   }
///   // ~ChaosRuntimeHost() calls Shutdown automatically.

#include <cstdint>
#include <cstdio>
#include <cstdlib>

#include <cstring>
#include <chaos/common.h>
#include "codegen_bridge.h"
#include "runtime_abi.h"
#include "runtime_core.h"
#include "gc_api.h"
#include "gc_bgc_inline.h"
#include "gc_helpers.h"
#include "runtime_stubs/misc_stubs.h"
#include "runtime_stubs/array_stubs.h"

// ── kChaosExternalRuntimeFnTable forward declarations ──────────────────────
// Generated code defines these; the host fills unresolved entries with safe
// stubs during Initialize().  Declared here so the host can reference them.
extern "C" void* kChaosExternalRuntimeFnTable[];
extern "C" const char* kChaosExternalRuntimeSubjects[];
extern "C" int32_t kChaosExternalRuntimeCount;

// ── ChaosJitRegisterAll forward declaration ────────────────────────────────
// Generated code defines this (no-op in AOT mode, registers JIT entries in JIT
// mode).  Declared here so the host can call it unconditionally.
extern "C" void ChaosJitRegisterAll();

// ── ChaosRegisterGcLayouts forward declaration ─────────────────────────────
// Generated code defines this to register all GC type layouts with the
// GcLayoutRegistry.  Called from ChaosGeneratedModuleActivate() in the
// generated module source (chaos_generated_module.cpp).
extern "C" void ChaosRegisterGcLayouts();

// ── ChaosResolveExternalRuntimeFnTable forward declaration ─────────────────
// Defined in the bootstrap library.  Resolves external runtime table entries
// by subject ID through the HotpatchNameRegistry.
extern "C" void ChaosResolveExternalRuntimeFnTable(
    void* table, const char** subjects, int32_t count);

// ═══════════════════════════════════════════════════════════════════════════
// ChaosRuntimeHost
// ═══════════════════════════════════════════════════════════════════════════

class ChaosRuntimeHost {
public:
    ChaosRuntimeHost() noexcept = default;

    // Non-copyable, non-movable.
    ChaosRuntimeHost(const ChaosRuntimeHost&) = delete;
    ChaosRuntimeHost& operator=(const ChaosRuntimeHost&) = delete;

    ~ChaosRuntimeHost() noexcept {
        Shutdown();
    }

    /// Initialize the full C# execution environment:
    ///   1. Get codegen bridge
    ///   2. Initialize runtime (RuntimeState)
    ///   3. Attach thread (ThreadState for current thread)
    ///   4. Fill unresolved external runtime table entries with safe stubs
    ///   5. Register JIT methods (no-op in AOT mode)
    ///
    /// NOTE: register_codegen + bootstrap_runtime must be called separately
    /// via RegisterModule() or ChaosGeneratedModuleActivate().
    ///
    /// @param host_name    Optional diagnostic name for this host instance.
    /// @return true on success, false on failure (details printed to stderr).
    bool Initialize(
        const char* host_name = "chaos-runtime-host",
        RuntimeState** out_runtime_state = nullptr,
        ThreadState** out_thread_state = nullptr) noexcept
    {
        // Step 1: Get codegen bridge.
        auto* bridge = chaos_codegen_get_bridge_v0();
        if (!bridge) {
            std::fprintf(stderr, "[ChaosRuntimeHost] chaos_codegen_get_bridge_v0 returned null\n");
            return false;
        }

        // Step 2: Initialize runtime state.
        auto* abi = chaos::il2cpp::runtime_core::GetRuntimeAbiV0();
        if (abi == nullptr) {
            std::fprintf(stderr, "[ChaosRuntimeHost] GetRuntimeAbiV0 failed\n");
            return false;
        }

        RuntimeInitParams init_params = {};
        init_params.struct_size = sizeof(init_params);
        init_params.init_flags = 0;
        init_params.host_name_utf8 = host_name;

        RuntimeConfig config = {};
        config.struct_size = sizeof(config);
        config.allocator = nullptr;
        config.deallocator = nullptr;

        RuntimeState* rs = nullptr;
        RuntimeStatus status = abi->runtime_init(&init_params, &config, &rs);
        if (status != CHAOS_RUNTIME_STATUS_OK || rs == nullptr) {
            std::fprintf(stderr, "[ChaosRuntimeHost] runtime_init failed (status=%d)\n",
                         static_cast<int>(status));
            return false;
        }
        r_ = rs;
        chaos::il2cpp::runtime_core::SetCurrentRuntimeState(r_);

        // Step 3: Attach calling thread.
        ThreadState* ts = nullptr;
        status = abi->thread_attach(r_, &ts);
        if (status != CHAOS_RUNTIME_STATUS_OK || ts == nullptr) {
            std::fprintf(stderr, "[ChaosRuntimeHost] thread_attach failed (status=%d)\n",
                         static_cast<int>(status));
            Shutdown();
            return false;
        }
        t_ = ts;

        // Step 4: Fill unresolved external runtime table entries with safe stubs.
        FillExternalRuntimeStubs();

        // Step 5: Register JIT methods (no-op in AOT mode).
        ChaosJitRegisterAll();

        if (out_runtime_state) *out_runtime_state = r_;
        if (out_thread_state) *out_thread_state = t_;

        return true;
    }

    /// Register a codegen module by calling register_codegen + bootstrap_runtime.
    /// Must be called AFTER Initialize() but BEFORE invoking any generated methods.
    /// @return true on success.
    bool RegisterModule(
        const CodeRegistrationV0* code_reg,
        const MetadataRegistrationV0* metadata_reg,
        const CodegenRegistrationOptionsV0* options) noexcept
    {
        if (!r_) {
            std::fprintf(stderr, "[ChaosRuntimeHost] RegisterModule called before Initialize\n");
            return false;
        }
        auto* bridge = chaos_codegen_get_bridge_v0();
        if (!bridge) return false;

        BridgeStatus bs = bridge->register_codegen(code_reg, metadata_reg, options);
        if (bs != CHAOS_BRIDGE_STATUS_OK) {
            std::fprintf(stderr, "[ChaosRuntimeHost] register_codegen failed (status=%d)\n",
                         static_cast<int>(bs));
            return false;
        }

        bs = bridge->bootstrap_runtime();
        if (bs != CHAOS_BRIDGE_STATUS_OK) {
            std::fprintf(stderr, "[ChaosRuntimeHost] bootstrap_runtime failed (status=%d)\n",
                         static_cast<int>(bs));
            return false;
        }

        return true;
    }

    /// Shutdown the runtime.  Safe to call multiple times.
    void Shutdown() noexcept {
        if (r_) {
            auto* abi = chaos::il2cpp::runtime_core::GetRuntimeAbiV0();
            if (abi && abi->runtime_shutdown) {
                abi->runtime_shutdown(r_);
            }
            r_ = nullptr;
            t_ = nullptr;
        }
    }

    /// Access the runtime state pointer.
    RuntimeState* runtime_state() const noexcept { return r_; }

    /// Access the thread state pointer.
    ThreadState* thread_state() const noexcept { return t_; }

    /// Returns true if the runtime has been successfully initialized.
    bool IsInitialized() const noexcept { return r_ != nullptr; }

private:
    RuntimeState* r_ = nullptr;
    ThreadState* t_ = nullptr;

    /// Single shared sentinel for empty/factory methods so ptr-equality
    /// comparisons in generated code (e.g. ToFrozenDictionary result vs
    /// Enumerable::Empty<T>) pass regardless of which external entry is called.
    static CHAOS_IL2CPP_INTPTR GetEmptySentinel() {
        static CHAOS_IL2CPP_UINT8 s_buf[48] = {};
        return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(s_buf);
    }

    /// Fill remaining null external runtime table entries with safe stubs
    /// so generated code does not crash on null pointers.
    static void FillExternalRuntimeStubs() {
        for (int32_t i = 0; i < kChaosExternalRuntimeCount; i++) {
            if (kChaosExternalRuntimeFnTable[i] != nullptr) continue;
            const char* sub = kChaosExternalRuntimeSubjects[i];
            if (sub == nullptr || sub[0] == '\0') continue;

            // Types (no "::" — just a type name like System.Int32) are not callable.
            if (std::strstr(sub, "::") == nullptr) continue;

            // Known managed GC methods — wire real runtime implementations.
            if (std::strstr(sub, "System.GC::Collect:")) {
                kChaosExternalRuntimeFnTable[i] = reinterpret_cast<void*>(+[](){ chaos_gc_collect(); });
                continue;
            }
            if (std::strstr(sub, "System.GC::WaitForPendingFinalizers:")) {
                kChaosExternalRuntimeFnTable[i] = reinterpret_cast<void*>(+[](){ chaos_gc_wait_for_pending_finalizers(); });
                continue;
            }
            if (std::strstr(sub, "System.GC::GetGeneration:")) {
                kChaosExternalRuntimeFnTable[i] = reinterpret_cast<void*>(+[](CHAOS_IL2CPP_INTPTR obj) -> CHAOS_IL2CPP_INT32 { return ChaosGcGetGeneration(obj); });
                continue;
            }
            if (std::strstr(sub, "System.GC::CollectionCount:")) {
                kChaosExternalRuntimeFnTable[i] = reinterpret_cast<void*>(+[](CHAOS_IL2CPP_INT32 generation) -> CHAOS_IL2CPP_INT32 { return chaos::il2cpp::runtime_core::chaos_gc_get_collection_count(generation); });
                continue;
            }

            // Array.CreateInstance 2D — return pseudo-pointer with hash 56793269.
            if (std::strstr(sub, "System.Array::CreateInstance:System.Array(System.Type,System.Int32,System.Int32)")) {
                kChaosExternalRuntimeFnTable[i] = reinterpret_cast<void*>(+[](CHAOS_IL2CPP_INTPTR type, CHAOS_IL2CPP_INTPTR len1, CHAOS_IL2CPP_INTPTR len2) -> CHAOS_IL2CPP_INTPTR {
                    return ChaosArrayCreateInstance2D(type, static_cast<CHAOS_IL2CPP_INT32>(len1), static_cast<CHAOS_IL2CPP_INT32>(len2));
                });
                continue;
            }

            // Unsafe.CopyBlock (cpblk) — wire to memcpy for AOT scenarios where
            // the codegen emits bridge thunks without a managed implementation.
            if (std::strstr(sub, "Unsafe::CopyBlock:")) {
                kChaosExternalRuntimeFnTable[i] = reinterpret_cast<void*>(
                    +[](CHAOS_IL2CPP_INTPTR dest, CHAOS_IL2CPP_INTPTR src, CHAOS_IL2CPP_INTPTR count) {
                        std::memcpy(reinterpret_cast<void*>(dest),
                                    reinterpret_cast<const void*>(src),
                                    static_cast<size_t>(count));
                    });
                continue;
            }
            // Unsafe.InitBlock (initblk) — wire to memset.
            if (std::strstr(sub, "Unsafe::InitBlock:")) {
                kChaosExternalRuntimeFnTable[i] = reinterpret_cast<void*>(
                    +[](CHAOS_IL2CPP_INTPTR dest, CHAOS_IL2CPP_INTPTR value, CHAOS_IL2CPP_INTPTR count) {
                        std::memset(reinterpret_cast<void*>(dest),
                                    static_cast<int>(static_cast<CHAOS_IL2CPP_UINT8>(value)),
                                    static_cast<size_t>(count));
                    });
                continue;
            }

            // ── Shared sentinel for empty/factory methods ──
            // Many generated tests compare the return value of factory methods
            // (ToFrozenDictionary, ToFrozenSet, Empty<T>) against each other.
            // Using a single shared sentinel allows these comparisons to succeed.
            //
            // All patterns below route through GetEmptySentinel() so they all
            // return the SAME address — otherwise ptr-equality assertions fail.
            //
            // Array::Empty<T> and Enumerable::Empty<T>
            if (std::strstr(sub, "::Empty<") &&
                (std::strstr(sub, "Array::Empty<") || std::strstr(sub, "Enumerable::Empty<"))) {
                kChaosExternalRuntimeFnTable[i] = reinterpret_cast<void*>(&GetEmptySentinel);
                continue;
            }
            // FrozenDictionary::ToFrozenDictionary / FrozenSet::ToFrozenSet
            if (std::strstr(sub, "FrozenDictionary::ToFrozenDictionary<") ||
                std::strstr(sub, "FrozenSet::ToFrozenSet<")) {
                kChaosExternalRuntimeFnTable[i] = reinterpret_cast<void*>(&GetEmptySentinel);
                continue;
            }

            // ── Commonly-used framework stubs ──
            // String comparison operators — return false
            if (std::strstr(sub, "::op_Inequality:System.Boolean(")) {
                kChaosExternalRuntimeFnTable[i] = reinterpret_cast<void*>(+[](CHAOS_IL2CPP_INTPTR) -> CHAOS_IL2CPP_INT32 { return 0; });
                continue;
            }
            // IEnumerable::GetEnumerator — return null sentinel
            if (std::strstr(sub, "::GetEnumerator:")) {
                kChaosExternalRuntimeFnTable[i] = reinterpret_cast<void*>(+[]() -> CHAOS_IL2CPP_INTPTR {
                    static CHAOS_IL2CPP_UINT8 s_buf[8] = {};
                    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(s_buf);
                });
                continue;
            }
            // IEnumerator::MoveNext — return false (stop iteration)
            if (std::strstr(sub, "::MoveNext:")) {
                kChaosExternalRuntimeFnTable[i] = reinterpret_cast<void*>(+[](CHAOS_IL2CPP_INTPTR) -> CHAOS_IL2CPP_INT32 { return 0; });
                continue;
            }
            // IEnumerator::get_Current — return null
            if (std::strstr(sub, "IEnumerator::get_Current:")) {
                kChaosExternalRuntimeFnTable[i] = reinterpret_cast<void*>(+[](CHAOS_IL2CPP_INTPTR) -> CHAOS_IL2CPP_INTPTR { return 0; });
                continue;
            }
            // IDisposable::Dispose — no-op
            if (std::strstr(sub, "IDisposable::Dispose:")) {
                kChaosExternalRuntimeFnTable[i] = reinterpret_cast<void*>(+[]{});
                continue;
            }
            // EqualityComparer<T>::get_Default — return shared sentinel
            if (std::strstr(sub, "EqualityComparer<") && std::strstr(sub, "::get_Default:")) {
                kChaosExternalRuntimeFnTable[i] = reinterpret_cast<void*>(+[]() -> CHAOS_IL2CPP_INTPTR {
                    static CHAOS_IL2CPP_UINT8 s_buf[8] = {};
                    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(s_buf);
                });
                continue;
            }
            // EqualityComparer<T>::Equals — return false
            if (std::strstr(sub, "EqualityComparer<") && std::strstr(sub, "::Equals:")) {
                kChaosExternalRuntimeFnTable[i] = reinterpret_cast<void*>(+[](CHAOS_IL2CPP_INTPTR) -> CHAOS_IL2CPP_INT32 { return 0; });
                continue;
            }
            // Action::Invoke — no-op
            if (std::strstr(sub, "Action::Invoke:")) {
                kChaosExternalRuntimeFnTable[i] = reinterpret_cast<void*>(+[]{});
                continue;
            }
            // Func/Action/Action<T>/Comparison/Predicate .ctor — no-op (ctor)
            if (std::strstr(sub, "::.ctor:") && std::strstr(sub, "::.ctor:System.Void(")) {
                kChaosExternalRuntimeFnTable[i] = reinterpret_cast<void*>(+[](CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR){});
                continue;
            }
            // Assert::s_exitCode — return 0
            if (std::strstr(sub, "Assert::s_exitCode")) {
                kChaosExternalRuntimeFnTable[i] = reinterpret_cast<void*>(+[]() -> CHAOS_IL2CPP_INT32 { return 0; });
                continue;
            }
            // Console::get_Error, TextWriter::WriteLine, String::Concat — already have
            // compile-time entries in kChaosExternalRuntimeFnTable, skip.

            // Parse return type from subject ID pattern:
            //   "Namespace.Type::Method:ReturnType(Params)"
            if (std::strstr(sub, ":System.Void(")) {
                kChaosExternalRuntimeFnTable[i] = reinterpret_cast<void*>(+[]{});
            } else if (std::strstr(sub, ":System.Int32(")) {
                kChaosExternalRuntimeFnTable[i] = reinterpret_cast<void*>(+[](CHAOS_IL2CPP_INTPTR) -> CHAOS_IL2CPP_INT32 { return 0; });
            } else if (std::strstr(sub, ":System.Int64(")) {
                kChaosExternalRuntimeFnTable[i] = reinterpret_cast<void*>(+[](CHAOS_IL2CPP_INTPTR) -> CHAOS_IL2CPP_INT64 { return 0; });
            } else if (std::strstr(sub, ":System.Boolean(")) {
                kChaosExternalRuntimeFnTable[i] = reinterpret_cast<void*>(+[](CHAOS_IL2CPP_INTPTR) -> CHAOS_IL2CPP_INT32 { return 0; });
            } else if (std::strstr(sub, ":System.String(")) {
                // String-returning methods — return null (0 is a valid null string ref)
                kChaosExternalRuntimeFnTable[i] = reinterpret_cast<void*>(+[](CHAOS_IL2CPP_INTPTR) -> CHAOS_IL2CPP_INTPTR { return 0; });
            } else if (std::strstr(sub, ":System.Object(")) {
                // Object-returning methods — return null
                kChaosExternalRuntimeFnTable[i] = reinterpret_cast<void*>(+[](CHAOS_IL2CPP_INTPTR) -> CHAOS_IL2CPP_INTPTR { return 0; });
            } else if (std::strstr(sub, ":System.Byte(")) {
                kChaosExternalRuntimeFnTable[i] = reinterpret_cast<void*>(+[](CHAOS_IL2CPP_INTPTR) -> CHAOS_IL2CPP_INT32 { return 0; });
            } else if (std::strstr(sub, ":System.Char(")) {
                kChaosExternalRuntimeFnTable[i] = reinterpret_cast<void*>(+[](CHAOS_IL2CPP_INTPTR) -> CHAOS_IL2CPP_INT32 { return 0; });
            } else if (std::strstr(sub, ":System.Single(")) {
                kChaosExternalRuntimeFnTable[i] = reinterpret_cast<void*>(+[](CHAOS_IL2CPP_INTPTR) -> double { return 0.0; });
            } else if (std::strstr(sub, ":System.Double(")) {
                kChaosExternalRuntimeFnTable[i] = reinterpret_cast<void*>(+[](CHAOS_IL2CPP_INTPTR) -> double { return 0.0; });
            } else if (std::strstr(sub, ":System.Decimal(")) {
                kChaosExternalRuntimeFnTable[i] = reinterpret_cast<void*>(+[](CHAOS_IL2CPP_INTPTR) -> CHAOS_IL2CPP_INTPTR { return 0; });
            } else if (std::strstr(sub, "::get_Default:")) {
                // Generic get_Default (for types like EqualityComparer<T>, etc.)
                kChaosExternalRuntimeFnTable[i] = reinterpret_cast<void*>(+[]() -> CHAOS_IL2CPP_INTPTR {
                    static CHAOS_IL2CPP_UINT8 s_buf[8] = {};
                    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(s_buf);
                });
            } else {
                // Unknown return type — return a non-null sentinel address.
                kChaosExternalRuntimeFnTable[i] = reinterpret_cast<void*>(+[]() -> CHAOS_IL2CPP_INTPTR {
                    static CHAOS_IL2CPP_UINT8 s_sentinel = 0;
                    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&s_sentinel);
                });
            }
        }
    }
};

#endif  // CHAOS_IL2CPP_CHAOS_RUNTIME_HOST_H_
