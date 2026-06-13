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

// ── JitEntry (file-offset-based) ──────────────────────────────────────
// Describes a single method for JIT compilation (JitPrecode dispatch).
// Codegen emits an array of these entries. The AotCoreIr JSON data is
// stored in a separate .jdata binary file, referenced by offset + length.
// During startup, the JIT data is loaded via ChaosJitDataLoad() and a
// pointer to the mapped data is passed to RegisterJitEntryMethods().
// On first call, the trampoline triggers JitStubDispatchImpl
// → Compile() → direct_ptr patched to compiled native code.
//
// Unlike JitMethodEntry (which routes through the interpreter tier pipeline),
// JitEntry routes directly to JIT compilation via the Precode Stub dispatch.
extern "C" struct JitEntry {
    uint32_t    json_offset;  // byte offset into .jdata file (was: const char* json_ptr)
    uint32_t    json_len;     // JSON string length
    uint32_t    token;        // metadata token for slot lookup
    uint32_t    module_id;    // module index (0 for single-module)
};

// ── RegisterJitMethods (interpreter dispatch) ──────────────────────────
// Kept for backward compatibility with pre-existing generated test files.
// Called once at startup to register AotCoreIr data for interpreter dispatch.
// New codegen runs (--mode jit) should use RegisterJitEntryMethods() instead.
extern "C" void RegisterJitMethods(const JitMethodEntry* entries, uint32_t count) noexcept;

// ── JitEntry registration ─────────────────────────────────────────────
// Declared here (implementation in chaos_jit / jit_precode.cpp).
// Processes an array of JitEntry descriptors to create JitPrecode +
// PrecodeArena trampolines for each method.  On first call, the trampoline
// triggers JitStubDispatchImpl → Compile() → direct_ptr patched to compiled code.
// JitT4Entry is an alias for JitEntry — codegen targeting T4 (JIT Tier 4)
// emits arrays of JitT4Entry for forward compatibility with tiered dispatch.
//
// The jit_data parameter points to the loaded .jdata file content in memory.
// It is the caller's responsibility to keep the data alive for the program
// lifetime. Pass nullptr to use embedded JSON strings (backward compat).
extern "C" void RegisterJitEntryMethods(const JitEntry* entries, uint32_t count,
                                        const char* jit_data = nullptr) noexcept;
typedef JitEntry JitT4Entry;

// T4 alias — codegen targeting T4 (JIT Tier 4) emits RegisterT4JitMethods()
// for forward compatibility with tiered dispatch. Maps to RegisterJitEntryMethods
// since T4 is currently the only JIT tier.
inline void RegisterT4JitMethods(const JitT4Entry* entries, uint32_t count,
                                 const char* jit_data = nullptr) noexcept {
    RegisterJitEntryMethods(entries, count, jit_data);
}

#endif // CHAOS_IL2CPP_JIT_REGISTRATION_H_
