// test_codegen_subsystem_stubs.cpp — standalone definitions for codegen-generated
// extern symbols referenced by the runtime libs (chaos_bootstrap / chaos_interpreter)
// that the contracts-native test targets link against.
//
// In a full foundation-dll build, native-aot.generated.cpp defines these symbols
// (hotpatch module + external-runtime dispatch table) as a side effect of codegen.
// Standalone CMake builds of the contracts-* test tier have no generated output,
// so these externs are otherwise unresolved at link time (LNK2001).
//
// The runtime paths that consume them are null/count-0 safe:
//   - ChaosResolveExternalRuntimeFnTable() early-returns when count <= 0.
//   - ModuleLifecycleManager::RegisterAotModuleData() skips a null hotpatch module.
// These contracts-native tests exercise runtime-core thread/sync primitives, not
// external dispatch, so empty stubs are correct.

#include <hotpatch_table.h>

#include <cstdint>

extern "C" const HotpatchModuleV0* chaos_il2cpp_aot_hotpatch_module = nullptr;

// MSVC disallows zero-size arrays; a single null sentinel satisfies the linker
// while the runtime only ever reads index < kChaosExternalRuntimeCount (0 here).
extern "C" const char* const kChaosExternalRuntimeSubjects[] = {nullptr};

extern "C" void* kChaosExternalRuntimeFnTable[] = {nullptr};

extern "C" std::int32_t kChaosExternalRuntimeCount = 0;
