#ifndef CHAOS_IL2CPP_JIT_REGISTRATION_H_
#define CHAOS_IL2CPP_JIT_REGISTRATION_H_

#include <cstdint>

// ── JitMethodEntry ───────────────────────────────────────────────────────
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

// ── RegisterJitMethods ───────────────────────────────────────────────────
// Called once at startup (from runtime-entry.cpp) to register all methods'
// AotCoreIr data and activate interpreter dispatch entries.
//
// Each entry's JSON is stored in the PatchMethod's aot_core_ir_json field
// and the dispatch entry is flagged kHotpatchActive via SetPatchedBySlot.
// After this call, all methods in the array will execute through the
// interpreter instead of native AOT.
extern "C" void RegisterJitMethods(const JitMethodEntry* entries, uint32_t count) noexcept;

#endif // CHAOS_IL2CPP_JIT_REGISTRATION_H_
