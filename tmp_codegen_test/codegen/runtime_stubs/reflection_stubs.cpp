// ABI exports: extern "C" linkage for managed/NativeAOT callability.
//
// reflection_stubs.cpp — FieldInfo/MethodBase flag stubs compiled from source
// in foundation-dll entry builds (bypasses prebuilt chaos_runtime_core.lib).
//
// These stubs are normally defined in reflection/remaining_stubs.cpp which is
// #included from reflection_api.cpp, but that TU is not compiled from source
// in entry builds — it ships in chaos_runtime_core.lib.  When adding new
// reflection stubs that are referenced by generated code (native-aot.generated.cpp),
// place them here so CMakeLists.txt picks them up via runtime_stubs/*.cpp glob.

#include <chaos/native_types.h>
#include "runtime_core.h"

extern "C" {
namespace chaos::il2cpp::runtime_core {

CHAOS_IL2CPP_INT32 ChaosReflectionFieldGetIsStatic(CHAOS_IL2CPP_INTPTR field) noexcept {
    auto* decoded = TryDecodeReflectionQueryHandle<ReflectionQueryFieldDescriptor>(
        static_cast<FieldInfoHandle>(field));
    if (decoded == nullptr) return 0;
    return (decoded->flags & kFieldFlagIsStatic) ? 1 : 0;
}

CHAOS_IL2CPP_INTPTR chaos_reflection_get_is_static(CHAOS_IL2CPP_INTPTR member) noexcept {
    auto* method = TryDecodeReflectionQueryHandle<ReflectionQueryMethodDescriptor>(
        static_cast<MethodInfoHandle>(member));
    if (method == nullptr) return 0;
    return (method->flags & kMethodFlagIsStatic) ? 1 : 0;
}

}  // namespace chaos::il2cpp::runtime_core
}  // extern "C"
