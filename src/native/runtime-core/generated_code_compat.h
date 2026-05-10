#ifndef CHAOS_IL2CPP_GENERATED_CODE_COMPAT_H_
#define CHAOS_IL2CPP_GENERATED_CODE_COMPAT_H_

// ── Runtime API declarations exposed to generated code ──
//
// This header provides declarations used by generated .cpp files via
// runtime_core.h. After the A4-Dual+V2 migration, generated code uses
// FatHeader (16B: type_info + vtable) for all object headers.

#include <chaos/native_types.h>
#include <chaos/type_info.h>
#include "arithmetic_chaos_bridge.h"
#include "codegen_bridge.h"       // HotpatchEntryV0, Hotpatch structs, kHotpatchActive
#include <gc.h>                   // GC_END_STUBBORN_CHANGE for write barriers
#include "debug_sink.h"           // DebugEventSink ring buffer

// ═══════════════════════════════════════════════════════════════════
// A5-Trinity Object Header Architecture
// ═══════════════════════════════════════════════════════════════════
// Three header variants discriminated by TypeInfo.flags[1:0]:
//
//   PureType (00):  8B  {TypeInfo* type_info}
//                       — no sync, no vtable
//   ThinLockable (01): 16B {TypeInfo* type_info, uint64_t sync_state}
//                       — thin-lock capable, no vtable
//   Fat (10):          24B {TypeInfo* type_info, void** vtable, uint64_t sync_state}
//                       — full dispatch + sync
//
// All three store TypeInfo* at offset [0], so chaos_object_get_type_info()
// is a single *(TypeInfoHot**)obj read — no bit magic needed.
//
// PureType: value-type boxes, sealed types with 0 virtual methods,
//           compiler-verified no-sync.
// ThinLockable: most reference types (no virtual dispatch, can sync).
// Fat: types with virtual methods (vtable dispatch + sync).
// ═══════════════════════════════════════════════════════════════════

// ── PureType header (8B) ──────────────────────────────────────────
// Used for value-type boxes, sealed types with no virtual methods.
// No sync_state — compiler-verified no-sync.
struct PureTypeHeader {
    const TypeInfoHot* type_info = nullptr;
};

// ── ThinLockable header (16B) ─────────────────────────────────────
// Used for most reference types. sync_state at [8] for thin locking.
struct ThinLockableHeader {
    const TypeInfoHot* type_info   = nullptr;  // [0]
    uint64_t        sync_state  = 0;        // [8] — thin lock / sync block index
};

// ── Fat header (24B) ──────────────────────────────────────────────
// Full-featured: virtual dispatch table + type identity + sync.
struct FatHeader {
    const TypeInfoHot* type_info   = nullptr;  // [0]
    const void**    vtable      = nullptr;  // [8]
    uint64_t        sync_state  = 0;        // [16] — thin lock / sync block index
};

// ── Runtime ObjectHeader (24B) ────────────────────────────────────
// Runtime-internal full layout. Matches FatHeader fields at [0..15]
// so reinterpret_cast between them is safe.
// struct RuntimeObjectHeader {
//     FatHeader   header;         // 16B {type_info, vtable}
//     uint64_t    sync_state = 0; // 8B  — thin lock / sync block index
// };  // 24B (defined in runtime_core.cpp)

// ── Unified type_info accessor ─────────────────────────────────────
// All three header kinds store TypeInfoHot* at offset [0].
inline const TypeInfoHot* chaos_object_get_type_info(const void* obj) noexcept {
    return *static_cast<const TypeInfoHot* const*>(obj);
}

// ── Managed string type ──────────────────────────────────────────
// Used by generated code for reinterpret_cast access to string length.
struct chaos_managed_string {
    ThinLockableHeader header{};
    CHAOS_IL2CPP_INT32 length = 0;
    const char* utf8_data = nullptr;
    CHAOS_IL2CPP_UINT64 string_id = 0u;
};

// ── Managed exception type ───────────────────────────────────────
// Used by generated code for catch(chaos_managed_exception&) blocks
// and throw chaos_managed_exception{obj} statements.
struct chaos_managed_exception
{
    CHAOS_IL2CPP_INTPTR object_value = 0;
};

#define CHAOS_IL2CPP_STRING_TYPE chaos_managed_string

// ── Exception metadata functions (extern C declarations) ──
// Implementations live in exception_api.cpp.
namespace chaos::il2cpp::runtime_core {
extern "C" {
void ChaosReflectionSetExceptionMetadata(CHAOS_IL2CPP_INTPTR exception_obj, CHAOS_IL2CPP_INTPTR message_value);
CHAOS_IL2CPP_INTPTR ChaosReflectionGetExceptionMessage(CHAOS_IL2CPP_INTPTR exception_obj);
}  // extern "C"

// ── Runtime stub declarations ───────────────────────────────────
// Stub implementations in runtime_stubs/*.cpp. Each per-domain header
// declares its functions in namespace+extern "C" context.
//
// NOTE: Functions already declared in reflection_api.h (included via
// runtime_core.h) are NOT duplicated here to avoid redefinition errors.
#include "runtime_stubs/stubs.h"

}  // namespace chaos::il2cpp::runtime_core

#endif // CHAOS_IL2CPP_GENERATED_CODE_COMPAT_H_