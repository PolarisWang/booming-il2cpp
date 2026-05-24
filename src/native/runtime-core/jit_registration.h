#ifndef CHAOS_IL2CPP_JIT_REGISTRATION_H_
#define CHAOS_IL2CPP_JIT_REGISTRATION_H_

#include <cstdint>

// .-- JitMethodEntry ---------------------------------------------------------
// DEPRECATED: Use JitT4Entry + RegisterT4JitMethods() instead (T4 JIT dispatch).
// JitMethodEntry routes through the interpreter tier pipeline (old behavior).
// Kept for backward compatibility with pre-existing generated test files.
// New codegen runs (--mode jit) now emit JitT4Entry + RegisterT4JitMethods().
// Describes a single method's AotCoreIr JSON data for JIT/interpreter mode.
// Codegen emits an array of these entries and RegisterJitMethods() processes
// them at startup to activate interpreter dispatch for every method.
//
// The module_id field disambiguates which module's dispatch table the
// method belongs to. For single-assembly scenarios this is always 0.
extern "C" struct JitMethodEntry {
    const char* json;         // AotCoreIr JSON string (null-terminated)
    uint32_t    json_len;     // JSON string length (excluding null terminator)
    uint32_t    token;        // metadata token for slot lookup
    uint32_t    module_id;    // module index (0 for single-module)
};

// ── JitT4Entry ────────────────────────────────────────────────────────
// Describes a single method for T4 JIT compilation (JitPrecode dispatch).
// Codegen emits an array of these entries and RegisterT4JitMethods()
// processes them at startup to create JitPrecode + PrecodeArena trampoline
// for each method.  On first call, the trampoline triggers JitStubDispatchImpl
// → Compile() → direct_ptr patched to compiled native code.
//
// Unlike JitMethodEntry (which routes through the interpreter tier pipeline),
// JitT4Entry routes directly to JIT compilation via the Precode Stub dispatch.
extern "C" struct JitT4Entry {
    const char* json;         // AotCoreIr JSON string (null-terminated)
    uint32_t    json_len;     // JSON string length (excluding null terminator)
    uint32_t    token;        // metadata token for slot lookup
    uint32_t    module_id;    // module index (0 for single-module)
};

// .-- RegisterJitMethods -----------------------------------------------------
// DEPRECATED: Prefer RegisterT4JitMethods() for new --mode jit codegen runs.
// Kept for backward compatibility with pre-existing generated test files.
// Called once at startup (from runtime-entry.cpp) to register all methods'
// AotCoreIr data and activate interpreter dispatch entries.
//
// Each entry's JSON is stored in the PatchMethod's aot_core_ir_json field
// and the dispatch entry is flagged kHotpatchActive via SetPatchedBySlot.
// After this call, all methods in the array will execute through the
// interpreter instead of native AOT.
extern "C" void RegisterJitMethods(const JitMethodEntry* entries, uint32_t count) noexcept;

// ── HybridT4Entry ─────────────────────────────────────────────────────
// Describes a single method for Hybrid mode JIT compilation.
// Codegen emits AOT C++ function body + AotCoreIr JSON for each method.
// At startup, RegisterHybridMethods() creates a HybridPrecode with the
// AOT entry saved, call_counter = kJitUpgradeThreshold, and a PrecodeArena
// trampoline.  Cold calls go through AOT; when counter reaches 0, JIT
// compilation triggers and subsequent calls use JIT-compiled code.
extern "C" struct HybridT4Entry {
    const char* json;         // AotCoreIr JSON string (null-terminated)
    uint32_t    json_len;     // JSON string length (excluding null terminator)
    uint32_t    token;        // metadata token for slot lookup
    uint32_t    module_id;    // module index (0 for single-module)
};

// ── RegisterT4JitMethods ─────────────────────────────────────────────────
// Inline wrapper: forwards to RegisterJitMethods (the canonical implementation
// in chaos_jit.lib) with a cast since JitT4Entry and JitMethodEntry have the
// same memory layout ({json, json_len, token, module_id}).
extern "C" inline void RegisterT4JitMethods(const JitT4Entry* entries, uint32_t count) noexcept {
    RegisterJitMethods(reinterpret_cast<const JitMethodEntry*>(entries), count);
}

// ── RegisterHybridMethods ────────────────────────────────────────────────
// Called once at startup to register all methods for Hybrid mode.
// In Hybrid mode, AOT C++ code is already compiled into the binary.
// This function:
//   1. Deserializes AotCoreIr JSON → RegisterMethod
//   2. Heap-allocates HybridPrecode with counter = kJitUpgradeThreshold
//   3. Saves the AOT entry from HotpatchEntryV0::direct_ptr
//   4. Replaces direct_ptr with a PrecodeArena trampoline
//
// Cold: trampoline → HybridStubDispatchImpl → aot_entry (counter decrements)
// Hot:  direct_ptr patched to JIT-compiled code (counter reached 0)
extern "C" void RegisterHybridMethods(const HybridT4Entry* entries, uint32_t count) noexcept;

#endif // CHAOS_IL2CPP_JIT_REGISTRATION_H_
