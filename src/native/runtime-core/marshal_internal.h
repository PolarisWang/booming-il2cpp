#ifndef CHAOS_IL2CPP_MARSHAL_INTERNAL_H_
#define CHAOS_IL2CPP_MARSHAL_INTERNAL_H_

// marshal_internal.h — Internal symbols shared across marshal module TUs.
//
// Extracts allocation structures, helpers, and accessor declarations that
// currently live in anonymous namespaces within individual marshal .cpp files
// (shared only via Unity build). When marshal files are split into independent
// translation units (Phase D4), this header provides the necessary declarations.
//
// Types that depend on header_layouts.cpp struct definitions (RuntimeState,
// ThreadState, RuntimeInternalState, ThreadInternalState) remain opaque;
// accessor functions are declared here but defined in the Unity build until
// the struct layouts are also extracted.

#include <chaos/common.h>

// TypeInfoHandle (uint64_t from runtime_abi.h) is used in StringObjectHeader.
#include <runtime_abi.h>

#include <cstdint>
#include <cstring>

// ── StringObjectHeader ────────────────────────────────────────────────────
// Header layout for managed string objects.
// Defined here (was in header_layouts.cpp anonymous ns) so standalone marshal
// TUs can access string data without depending on the Unity build.
struct StringObjectHeader {
    TypeInfoHandle type;
    CHAOS_IL2CPP_UINTPTR byte_count;
};

// ── Forward declarations (opaque types from header_layouts.cpp) ─────────
// NOTE: These must be at file scope (not inside a namespace) because
// header_layouts.cpp defines them at file scope too. When this header is
// included in the Unity build (via runtime_core.cpp), putting them inside
// namespace chaos::il2cpp::runtime_core would create incompatible types.
struct RuntimeState;
struct ThreadState;
struct RuntimeInternalState;
struct ThreadInternalState;
struct RuntimeConfig;

#include <marshal_abi.h>
#include "memory_domain.h"

namespace chaos::il2cpp::runtime_core {

// ── MarshalAllocationHeader ─────────────────────────────────────────────
// Must precede every marshal allocation. sizeof(header) + user_size per block.
// Defined here for standalone TUs. When compiled as part of the Unity build,
// header_layouts.cpp provides the same type inside the anonymous namespace,
// so this definition is skipped via the CHAOS_IL2CPP_MARSHAL_ALLOC_HEADER_ALREADY_DEFINED guard.

#ifndef CHAOS_IL2CPP_MARSHAL_ALLOC_HEADER_ALREADY_DEFINED
#define CHAOS_IL2CPP_MARSHAL_ALLOC_HEADER_ALREADY_DEFINED

struct MarshalAllocationHeader {
    CHAOS_IL2CPP_UINT64              magic;
    memory_domain::DomainId          domain_id;
    marshal_abi::MarshalAllocationKind kind;
    CHAOS_IL2CPP_SIZE                requested_size;
};

constexpr CHAOS_IL2CPP_UINT64 kMarshalAllocationMagic = 0x43484D4152534841ull;

#endif  // CHAOS_IL2CPP_MARSHAL_ALLOC_HEADER_ALREADY_DEFINED

// ── Config allocation forwarding (declared) ─────────────────────────────
// Defined in config_normalize.cpp. Needed by marshal allocators when no
// memory domain is active.

void* AllocateBytes(const RuntimeConfig& config, CHAOS_IL2CPP_SIZE size);
void  FreeBytes(const RuntimeConfig& config, void* ptr);

// ── Allocation helpers ───────────────────────────────────────────────────
// Currently in marshal_alloc.cpp anonymous namespace. Declared here so
// marshal_api.cpp (and future independent TUs) can call them.

/// Cast raw CHAOS_IL2CPP_INTPTR to usable void* pointer.
inline void* GetMarshalAllocationStorage(CHAOS_IL2CPP_INTPTR memory) {
    return reinterpret_cast<void*>(memory);
}

/// Validate header magic and return the header pointer, or nullptr.
MarshalAllocationHeader* TryGetMarshalAllocationHeader(CHAOS_IL2CPP_INTPTR memory);

/// Allocate a marshal block with header (no domain fallback → config allocator).
CHAOS_IL2CPP_INTPTR AllocateMarshalBlock(
    RuntimeState* runtime_state,
    CHAOS_IL2CPP_SIZE requested_size,
    marshal_abi::MarshalAllocationKind kind);

/// Reallocate a marshal block, preserving existing content.
CHAOS_IL2CPP_INTPTR ReallocateMarshalBlock(
    RuntimeState* runtime_state,
    CHAOS_IL2CPP_INTPTR memory,
    CHAOS_IL2CPP_SIZE requested_size,
    marshal_abi::MarshalAllocationKind kind);

/// Free a marshal block (zero_before_free for secure string cleanup).
bool FreeMarshalBlock(
    RuntimeState* runtime_state,
    CHAOS_IL2CPP_INTPTR memory,
    marshal_abi::MarshalAllocationKind kind,
    bool zero_before_free);

// ── Read/Write value templates ───────────────────────────────────────────
// Inline templates for type-safe marshal memory read/write operations.

template <typename TValue>
inline TValue MarshalReadValue(CHAOS_IL2CPP_INTPTR address, CHAOS_IL2CPP_INT32 offset) {
    TValue value = {};
    if (address == 0) return value;
    const auto* source = static_cast<const unsigned char*>(
        GetMarshalAllocationStorage(address)) + offset;
    CHAOS_IL2CPP_MEMCPY(&value, source, sizeof(TValue));
    return value;
}

template <typename TValue>
inline void MarshalWriteValue(
    CHAOS_IL2CPP_INTPTR address,
    CHAOS_IL2CPP_INT32 offset,
    TValue value) {
    if (address == 0) return;
    auto* destination = static_cast<unsigned char*>(
        GetMarshalAllocationStorage(address)) + offset;
    CHAOS_IL2CPP_MEMCPY(destination, &value, sizeof(TValue));
}

// ── Thread/Runtime state accessors ───────────────────────────────────────
// Defined in marshal_alloc.cpp (anonymous ns currently). These access opaque
// struct fields and require the full struct definition from header_layouts.cpp.
// Until independent TU split, they compile within the Unity build.

bool IsAttached(RuntimeState* runtime_state, ThreadState* thread_state);
RuntimeInternalState* GetRuntimeInternalState(RuntimeState* runtime_state);
ThreadInternalState* GetThreadInternalState(ThreadState* thread_state);

// ── String creation helper ────────────────────────────────────────────────
// Defined in object_creation.cpp (Unity build). Needed by marshal_api.cpp and
// marshal_string.cpp when compiled as standalone TUs.

void* StringNewUtf8(
    RuntimeState* runtime_state,
    ThreadState* thread_state,
    const char* utf8_bytes,
    CHAOS_IL2CPP_UINTPTR byte_count);

}  // namespace chaos::il2cpp::runtime_core

#endif  // CHAOS_IL2CPP_MARSHAL_INTERNAL_H_
