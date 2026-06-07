// Auto-generated bridge redirect registration — Phase 2
// Patches kChaosExternalRuntimeFnTable with compiled function pointers.
#include <cstdint>

extern "C" void* kChaosExternalRuntimeFnTable[];
extern "C" const char* kChaosExternalRuntimeSubjects[];
extern "C" int32_t kChaosExternalRuntimeCount;
extern "C" void* ResolveBridge(const char* subjectId) noexcept;

namespace {
    struct BridgeRegistrar {
        BridgeRegistrar() noexcept {
            for (int32_t i = 0; i < kChaosExternalRuntimeCount; i++) {
                const auto* sub = kChaosExternalRuntimeSubjects[i];
                if (sub == nullptr) continue;
                auto* fn = ResolveBridge(sub);
                if (fn != nullptr) kChaosExternalRuntimeFnTable[i] = fn;
            }
        }
    } _reg;
}
