#include "jit_debug_contract.h"
#include "../metadata_interface.h"

#include <cstdint>
#include <cstring>

#if defined(_MSC_VER)
  #include <intrin.h>
#endif

namespace rc = chaos::il2cpp::runtime_core;

// ── JIT Debug Entry (POD, stable layout for cross-process SOS reading) ──
// Mirrors the key fields of WinSehHandler::JitCodeEntry for the SOS extension.
// SOS reads these structs from the target process via IDebugDataSpaces::ReadVirtual.
// The layout must remain stable across builds.
struct JitDebugEntry {
    void*             code_start;           // RX code entry point
    uint32_t          code_size;            // bytes
    uint32_t          patch_method_token;   // PatchMethod token for hotpatch demotion
    const void*       nm;                   // const JitMethod* (for reading debug_info_offset)
};

static constexpr uint32_t kMaxJitDebugEntries = 2048;
static JitDebugEntry g_jit_debug_entries[kMaxJitDebugEntries];
static uint32_t      g_jit_debug_entry_count = 0;

extern "C" {

void* g_chaos_jit_debug_entries_ptr         = &g_jit_debug_entries;
void* g_chaos_jit_debug_entry_count_ptr     = &g_jit_debug_entry_count;
int32_t g_chaos_jit_debug_max_entries       = static_cast<int32_t>(kMaxJitDebugEntries);
void* g_chaos_metadata_registry_ptr         = nullptr;

}  // extern "C"

// ── Public API for jit_seh.cpp to sync the debug mirror ──────────────────
// Called from RegisterNativeCodeSection, typically under JitRegistryLockGuard.
// The caller holds the lock, so the increment is thread-safe.
void JitDebugContractAddEntry(void* code_start, uint32_t code_size,
                              const void* nm, uint32_t patch_method_token) noexcept {
    if (code_start == nullptr || nm == nullptr) return;
#if defined(_MSC_VER)
    uint32_t idx = static_cast<uint32_t>(_InterlockedExchangeAdd(
        reinterpret_cast<volatile long*>(&g_jit_debug_entry_count), 1));
#else
    uint32_t idx = __atomic_fetch_add(&g_jit_debug_entry_count, 1u, __ATOMIC_RELAXED);
#endif
    if (idx >= kMaxJitDebugEntries) {
        g_jit_debug_entry_count = kMaxJitDebugEntries;
        return;
    }
    g_jit_debug_entries[idx].code_start         = code_start;
    g_jit_debug_entries[idx].code_size          = code_size;
    g_jit_debug_entries[idx].nm                 = nm;
    g_jit_debug_entries[idx].patch_method_token = patch_method_token;
}

void JitDebugContractInitMetadataRegistry(const void* unified_registry) noexcept {
    g_chaos_metadata_registry_ptr = const_cast<void*>(unified_registry);
}
