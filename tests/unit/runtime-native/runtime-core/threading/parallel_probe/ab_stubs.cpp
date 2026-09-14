// Minimal definitions for codegen-emitted symbols.
//
// The A/B harness links the chaos static libraries but no generated chunk, so
// it must supply the four symbols a real chunk would define: the external
// runtime dispatch table and the AOT hotpatch module.  An empty table is the
// correct value here — the probe calls Parallel.For directly and never routes
// through external-runtime fallback.
#include <cstdint>

extern "C" void* kChaosExternalRuntimeFnTable[1] = {nullptr};
extern "C" const char* kChaosExternalRuntimeSubjects[1] = {nullptr};
extern "C" int32_t kChaosExternalRuntimeCount = 0;

struct HotpatchModuleV0;
extern "C" const HotpatchModuleV0* chaos_il2cpp_aot_hotpatch_module = nullptr;
