#ifndef CHAOS_IL2CPP_JIT_REGISTRATION_H_
#define CHAOS_IL2CPP_JIT_REGISTRATION_H_

#include <cstdint>

// .-- JitMethodEntry ---------------------------------------------------------
// DEPRECATED: Use JitEntry + RegisterJitEntryMethods() instead (JIT dispatch).
// JitMethodEntry routes through the interpreter tier pipeline (old behavior).
// Kept for backward compatibility with pre-existing generated test files.
// New codegen runs (--mode jit) now emit JitEntry + RegisterJitEntryMethods().
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

// ── JitEntry ────────────────────────────────────────────────────────
// Describes a single method for JIT compilation (JitPrecode dispatch).
// Codegen emits an array of these entries and RegisterJitEntryMethods()
// processes them at startup to create JitPrecode + PrecodeArena trampoline
// for each method.  On first call, the trampoline triggers JitStubDispatchImpl
// → Compile() → direct_ptr patched to compiled native code.
//
// Unlike JitMethodEntry (which routes through the interpreter tier pipeline),
// JitEntry routes directly to JIT compilation via the Precode Stub dispatch.
extern "C" struct JitEntry {
    const char* json;         // AotCoreIr JSON string (null-terminated)
    uint32_t    json_len;     // JSON string length (excluding null terminator)
    uint32_t    token;        // metadata token for slot lookup
    uint32_t    module_id;    // module index (0 for single-module)
};

// ── RegisterJitMethods (interpreter dispatch) ──────────────────────────
// Kept for backward compatibility with pre-existing generated test files.
// Called once at startup to register AotCoreIr data for interpreter dispatch.
// New codegen runs (--mode jit) should use RegisterJitEntryMethods() instead.
extern "C" void RegisterJitMethods(const JitMethodEntry* entries, uint32_t count) noexcept;

// ── HybridEntry ─────────────────────────────────────────────────────
// Describes a single method for Hybrid mode JIT compilation.
// Codegen emits AOT C++ function body + AotCoreIr JSON for each method.
// At startup, RegisterHybridMethods() creates a HybridPrecode with the
// AOT entry saved, call_counter = kJitUpgradeThreshold, and a PrecodeArena
// trampoline.  Cold calls go through AOT; when counter reaches 0, JIT
// compilation triggers and subsequent calls use JIT-compiled code.
extern "C" struct HybridEntry {
    const char* json;         // AotCoreIr JSON string (null-terminated)
    uint32_t    json_len;     // JSON string length (excluding null terminator)
    uint32_t    token;        // metadata token for slot lookup
    uint32_t    module_id;    // module index (0 for single-module)
};

// ── JitEntry registration ─────────────────────────────────────────────
// Declared here (implementation in chaos_jit / jit_precode.cpp).
// Processes an array of JitEntry descriptors to create JitPrecode +
// PrecodeArena trampolines for each method.  On first call, the trampoline
// triggers JitStubDispatchImpl → Compile() → direct_ptr patched to compiled code.
// JitT4Entry is an alias for JitEntry — codegen targeting T4 (JIT Tier 4)
// emits arrays of JitT4Entry for forward compatibility with tiered dispatch.
extern "C" void RegisterJitEntryMethods(const JitEntry* entries, uint32_t count) noexcept;
typedef JitEntry JitT4Entry;

// T4 alias — codegen targeting T4 (JIT Tier 4) emits RegisterT4JitMethods()
// for forward compatibility with tiered dispatch. Maps to RegisterJitEntryMethods
// since T4 is currently the only JIT tier.
inline void RegisterT4JitMethods(const JitT4Entry* entries, uint32_t count) noexcept {
    RegisterJitEntryMethods(entries, count);
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
extern "C" void RegisterHybridMethods(const HybridEntry* entries, uint32_t count) noexcept;

#endif // CHAOS_IL2CPP_JIT_REGISTRATION_H_
