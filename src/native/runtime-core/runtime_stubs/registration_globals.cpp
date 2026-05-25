// registration_globals.cpp — Global function pointers set by generated code static init
//
// These extern "C" global function pointers are set by generated code via
// static initializers. g_chaos_populate_generic_registration is called at
// boot to populate generic method registrations.

extern "C" void (*g_chaos_populate_generic_registration)(void) = nullptr;

// TierManager::EnqueueJitRecompilation — stub for entry builds.
// The SDK's chaos_runtime_core.lib is too old to contain this symbol,
// but entry-jit.exe only registers JIT methods and never tier-compiles,
// so a no-op returning false is sufficient.
#include "tier_manager.h"
namespace chaos::il2cpp::jit { struct JitPrecode; }
bool chaos::il2cpp::runtime_core::TierManager::EnqueueJitRecompilation(
    chaos::il2cpp::jit::JitPrecode* /*precode*/) noexcept {
    return false;
}