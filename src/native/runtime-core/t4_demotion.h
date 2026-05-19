#ifndef CHAOS_IL2CPP_RUNTIME_CORE_T4_DEMOTION_H_
#define CHAOS_IL2CPP_RUNTIME_CORE_T4_DEMOTION_H_

// ── T4 Demotion Callback Registry ──────────────────────────────────────────
//
// Breaks the circular dependency: hot_update needs to demote T4 entries,
// but the T4 code registry lives in chaos_codegen (which depends on
// chaos_interpreter → chaos_hot_update).
//
// Solution: runtime_core holds function pointer slots.  chaos_codegen
// registers its demotion functions at startup.  chaos_hot_update calls
// through the slots.
//
// Both slots default to nullptr — safe to call before T4 is initialized.

#include <cstdint>

namespace chaos::il2cpp::runtime_core {

/// Signature: uint32_t(uint32_t method_token)
using T4DemoteByTokenFn = uint32_t (*)(uint32_t);

/// Signature: uint32_t(uint32_t method_token)
using T4DemoteByCallSiteTokenFn = uint32_t (*)(uint32_t);

/// Register demotion callbacks (called by codegen/t4_seh_handler.cpp at init).
void RegisterT4DemotionCallbacks(
    T4DemoteByTokenFn demote_by_token,
    T4DemoteByCallSiteTokenFn demote_by_call_site_token) noexcept;

/// Call DemoteT4ByToken through the registered callback.
/// Returns 0 if no callback is registered (T4 not yet initialized).
uint32_t DemoteT4ByToken(uint32_t method_token) noexcept;

/// Call DemoteT4ByCallSiteToken through the registered callback.
/// Returns 0 if no callback is registered (T4 not yet initialized).
uint32_t DemoteT4ByCallSiteToken(uint32_t method_token) noexcept;

}  // namespace chaos::il2cpp::runtime_core

#endif  // CHAOS_IL2CPP_RUNTIME_CORE_T4_DEMOTION_H_
