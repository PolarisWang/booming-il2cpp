// registration_globals.cpp — Global function pointers set by generated code static init
//
// These extern "C" global function pointers are set by generated code via
// static initializers. g_chaos_populate_generic_registration is called at
// boot to populate generic method registrations.

extern "C" void (*g_chaos_populate_generic_registration)(void) = nullptr;

// TierManager::EnqueueJitRecompilation — stub for entry builds where
// chaos_runtime_core.lib is pre-built without tier_manager.cpp compiled.
// In workspace build (all source compiled locally), tier_manager.cpp provides
// the real implementation and this stub must be skipped to avoid duplicate.
#if !defined(CHAOS_IL2CPP_HAS_TIER_MANAGER)
#include "tier_manager.h"
namespace chaos::il2cpp::jit { struct JitPrecode; }
bool chaos::il2cpp::runtime_core::TierManager::EnqueueJitRecompilation(
    chaos::il2cpp::jit::JitPrecode* /*precode*/) noexcept {
    return false;
}
#endif