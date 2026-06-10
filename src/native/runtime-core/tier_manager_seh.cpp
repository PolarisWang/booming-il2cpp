// tier_manager_seh.cpp — SEH-safe helpers for tier_manager background thread.
// MSVC C2712 prohibits __try/__except in any .cpp that also contains C++
// EH (std::unique_lock, etc.), even in different functions within the same TU.
// This file is compiled as a separate translation unit to isolate SEH blocks.

#include "tier_manager.h"
#include "patch_loader.h"
#include <jit/jit_precode.h>

#if defined(_MSC_VER)
#include <excpt.h>

void* SafeJitRecompile(chaos::il2cpp::jit::JitPrecode* p) noexcept {
    __try {
        return chaos::il2cpp::jit::JitRecompileToTier1(p);
    } __except(EXCEPTION_EXECUTE_HANDLER) {
        // Logging intentionally omitted — the CHAOS_LOG family uses C++
        // temporary objects whose destructors trigger C2712.
        return nullptr;
    }
}

extern "C" void SafeT3Promotion(
    chaos::il2cpp::runtime_core::PatchMethod* pm) noexcept
{
    auto fn = chaos::il2cpp::runtime_core::GetTier3PromotionCallback();
    if (fn == nullptr) return;
    __try {
        fn(pm);
    } __except(EXCEPTION_EXECUTE_HANDLER) {
        uint32_t expected = chaos::il2cpp::runtime_core::PatchMethod::kOptimizeLowering;
        pm->tier_state.compare_exchange_strong(
            expected, chaos::il2cpp::runtime_core::PatchMethod::kOptimizedRegister,
            std::memory_order_release);
    }
}

#else
void* SafeJitRecompile(chaos::il2cpp::jit::JitPrecode* p) noexcept {
    return chaos::il2cpp::jit::JitRecompileToTier1(p);
}

void SafeT3Promotion(
    chaos::il2cpp::runtime_core::PatchMethod* pm) noexcept
{
    auto fn = chaos::il2cpp::runtime_core::GetTier3PromotionCallback();
    if (fn == nullptr) return;
    fn(pm);
}
#endif
