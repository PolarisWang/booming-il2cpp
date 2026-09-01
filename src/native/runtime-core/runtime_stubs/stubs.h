#pragma once

// ── Meta-include: all per-domain stub declarations ───────────────
// Included by generated_code_compat.h to replace the old monolithic
// extern "C" declaration block.

#include "stub_common.h"

// Include exception helpers before extern "C" so inline stubs
// (e.g. ChaosConvertToInt32FromDouble) can use RaiseManagedException.
#include "exception_helpers.h"

// Include thread_state.h before extern "C" so its namespaced inline
// functions (GetCurrentThreadId, GetCurrentThread) get C++ linkage.
// When first seen inside extern "C", MSVC gives them C linkage which
// conflicts with Win32 API functions of the same name.
#include "thread_state.h"

// exception_stubs.h must be outside extern "C" because it defines
// static inline functions using C++ casts (static_cast).
#include "exception_stubs.h"

// threading_stubs.h pulls <chaos/thread.h> → <thread>/<mutex> (C++ std headers).
// It MUST be included OUTSIDE extern "C": MSVC 14.44+ fails to declare the global
// ::terminate (→ C2039) when a C++ std header is first seen inside an extern "C"
// block.
#include "threading_stubs.h"

extern "C" {

#include "array_stubs.h"
#include "math_stubs.h"
#include "datetime_stubs.h"
#include "char_stubs.h"
#include "convert_stubs.h"
#include "guid_stubs.h"
#include "random_stubs.h"
#include "hashcode_stubs.h"
#include "interlocked_stubs.h"
#include "string_stubs.h"
#include "stream_stubs.h"
#include "object_stubs.h"
#include "collection_stubs.h"
#include "misc_stubs.h"
#include "mutex_stubs.h"
#include "enum_stubs.h"
#include "async_stubs.h"
#include "web_stubs.h"
#include "interop_stubs.h"
#include "simd_stubs.h"
#include "xml_nametable_stubs.h"

}  // extern "C"