// ABI exports: extern "C" linkage for managed/NativeAOT callability.
//
// Unity-file aggregator: runtime_instantiation.cpp is the single translation unit.
// Each instantiation/*.cpp file is #included here and contributes to one combined
// compilation unit. Zero CMakeLists.txt changes required.

#include "instantiation_engine.h"
#include "generic_context.h"
#include "generic_method_instantiation.h"
#include "layout_engine.h"
#include "module_registry.h"    // kMaxModules, GetTypeToken
#include "hotpatch_table.h"    // Dispatch entry lookup for A2
#include "reflection_query_model.h"
#include "runtime_core.h"
#include "runtime_vtable.h"
#include "bootstrap/bootstrap.h"  // GetCodegenBridgeV0
#include <chaos/profile.h>

#include <chaos/native_types.h>

#include <atomic>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <new>
#include <string>
#include <vector>

#include "instantiation/token_allocator.cpp"
#include "instantiation/resolve_instantiate.cpp"
#include "instantiation/interpreter_dispatch.cpp"
#include "instantiation/precache_dispatch_raw.cpp"
#include "instantiation/interpret_method_call.cpp"
#include "instantiation/type_descriptor.cpp"
#include "instantiation/aot_method_map.cpp"

