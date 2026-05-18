#ifndef CHAOS_IL2CPP_COMMON_H_
#define CHAOS_IL2CPP_COMMON_H_

// chaos::il2cpp::common — shared foundation library for all generated C++ code.
// Provides high-performance, header-only utilities and minimal compiled modules.
//
// Usage: #include <chaos/common.h>
//
// Namespace: chaos::il2cpp::common
// Types: PascalCase
// Functions: snake_case
// Constants: k_ + snake_case

#include "native_types.h"
#include "format.h"
#include "checked_conv.h"
#include "arithmetic.h"
#include "load_store.h"
#include "finally.h"
#include "ptr_tag.h"
#include "status.h"
#include "static_init.h"
#include "span.h"
#include "collection.h"
#include "unordered_dense.h"
#include "monitor.h"
#include "thread.h"
#include "async.h"
#include "log.h"

// Umbrella header — all symbols are in chaos::il2cpp::common via individual headers.
namespace chaos::il2cpp::common {

/// Fail hook (setjmp/longjmp fallback for generated code verification).
/// Set by runtime-entry.cpp Fact/Benchmark/HotUpdate modes to catch managed
/// exceptions.  When non-null, CHAOS_IL2CPP_FAIL calls this instead of abort().
extern void (*g_chaos_fail_hook)();

} // namespace chaos::il2cpp::common

// Backward-compat alias for generated code using the old namespace name.
namespace ChaosIl2cpp { namespace Common = chaos::il2cpp::common; }

#endif // CHAOS_IL2CPP_COMMON_H_
