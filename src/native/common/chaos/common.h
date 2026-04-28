#ifndef CHAOS_IL2CPP_COMMON_H_
#define CHAOS_IL2CPP_COMMON_H_

// ChaosIl2cpp::Common — shared foundation library for all generated C++ code.
// Provides high-performance, header-only utilities and minimal compiled modules.
//
// Usage: #include <chaos/common.h>
//
// Namespace: ChaosIl2cpp::Common
// Types: PascalCase
// Functions: snake_case
// Constants: k_ + snake_case

#include "checked_conv.h"
#include "arithmetic.h"
#include "load_store.h"
#include "finally.h"
#include "ptr_tag.h"
#include "status.h"
#include "static_init.h"
#include "span.h"
#include "collection.h"
#include "monitor.h"
#include "thread.h"
#include "async.h"

// Umbrella header — all symbols are in ChaosIl2cpp::Common via individual headers.
namespace ChaosIl2cpp::Common {
} // namespace ChaosIl2cpp::Common

#endif // CHAOS_IL2CPP_COMMON_H_
