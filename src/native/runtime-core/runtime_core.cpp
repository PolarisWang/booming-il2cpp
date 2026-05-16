// ABI exports: extern "C" linkage for managed/NativeAOT callability.
//
// Unity-file aggregator: runtime_core.cpp is the single translation unit.
// Each _core/*.cpp file is #included here and contributes to one combined
// compilation unit. Zero CMakeLists.txt changes required.

#include "runtime_core.h"

#include <chaos/trace.h>
#include <chaos/log.h>
#include <chaos/profile.h>

#include "memory_domain.h"
#include "gc_region.h"
#include "gc_old_gen.h"
#include "gc_loh.h"
#include "gc_young_collector.h"
#include "gc_stats.h"
#include "gc_events.h"
#include "reflection_query_model.h"
#include "generic_context.h"
#include "vtable_registry.h"
#include "runtime_vtable.h"
#include "bootstrap/bootstrap.h"
#include "instantiation_engine.h"
#include "reflection_query_model.h"
#include "module_registry.h"

#include <cmath>
#include <atomic>
#include <limits>
#include <cstdlib>
#include <condition_variable>
#include <thread>

#include "gc_transition.h"

#include <gc.h>

// Spin-loop hint (x86 PAUSE / ARM YIELD)
#if defined(_M_ARM64) || defined(__aarch64__)
    #define CHAOS_SPIN_HINT()  __yield()
#else
    #include <immintrin.h>
    #define CHAOS_SPIN_HINT()  _mm_pause()
#endif
#include <cstring>
#include <memory>
#include <mutex>
#include <new>
#include <string>
#include <thread>
#include <unordered_map>
#include <vector>

#if defined(_WIN32)
#include <Windows.h>
#endif

// Bring common namespaces into scope for all sub-files in this TU.
using namespace chaos::il2cpp::runtime_capability;
using namespace chaos::il2cpp::marshal_abi;

// Include sub-files in dependency order:
//   1. Foundational types, GC alloc, TLS
//   2. Engine lifecycle, config, marshal alloc, sync
//   3. Runtime init, object creation, reflection
//   4. Runtime mode, engine binding, monitor, GC safepoint
//   5. Marshal API, string marshaling, char classification
//   6. Hash utilities, value type kernels, numerics vectors
//   7. Delegate thunks, struct marshal descriptors, ABI export
#include "core/header_layouts.cpp"
#include "core/engine_lifecycle.cpp"
#include "core/state_tls.cpp"
#include "core/config_normalize.cpp"
#include "core/marshal_alloc.cpp"
#include "core/sync_mutex.cpp"
#include "core/type_capability.cpp"
#include "core/runtime_init.cpp"
#include "core/object_creation.cpp"
#include "core/hashcode.cpp"
#include "core/abi_reflection.cpp"
#include "core/runtime_mode.cpp"
#include "core/engine_binding.cpp"
#include "core/monitor.cpp"
#include "core/gc_safepoint.cpp"
#include "core/marshal_api.cpp"
#include "core/marshal_string.cpp"
#include "core/char_classification.cpp"
#include "core/value_type_kernels.cpp"
#include "core/numerics_vectors.cpp"
#include "core/delegate_thunks.cpp"
#include "core/struct_marshal_descriptors.cpp"
#include "core/abi_export.cpp"
