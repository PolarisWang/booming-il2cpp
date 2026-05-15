// ABI exports: extern "C" linkage for managed/NativeAOT callability.
//
// Unity-file aggregator: reflection_api.cpp is the single translation unit.
// Each reflection/*.cpp file is #included here and contributes to one combined
// compilation unit. Zero CMakeLists.txt changes required.

// reflection_api.cpp — Native AOT reflection API implementation
//
// Provides extern "C" implementations for chaos_reflection_* functions
// called from generated C++ code. Uses Module Registry + Two-Tier Metadata
#include <chaos/trace.h>
//
// for type/method/field queries:
//
//   - Given a (module_id, token) encoded TypeInfoHandle, the registry
//     locates the module descriptor, then the Tier 2 image descriptor
//     for full type/method/field metadata.
//   - Legacy ReflectionQuery encoded handles (tag bit 63) are decoded
//     directly to constexpr ReflectionQueryTypeDescriptor pointers.
//   - Raw metadata tokens fall back to the aot_metadata shared tables.
//
// These functions are called via SimpleForward dispatch from generated code,
// NOT through the bridge vtable.

#include "runtime_core.h"
#include "module_registry.h"
#include "reflection_query_model.h"
#include "reflection_metadata_impl.h"
#include "string_table.h"
#include "instantiation_engine.h"
#include "generic_context.h"

#include <cstring>
#include <fmt/format.h>

#include "reflection/internal_helpers.cpp"
#include "reflection/type_resolve.cpp"
#include "reflection/type_properties.cpp"
#include "reflection/hierarchy.cpp"
#include "reflection/members.cpp"
#include "reflection/invoke.cpp"
#include "reflection/attributes.cpp"
#include "reflection/module.cpp"
#include "reflection/misc.cpp"
#include "reflection/assembly_types.cpp"
#include "reflection/string_concat.cpp"
#include "reflection/type_stubs.cpp"
#include "reflection/remaining_stubs.cpp"
