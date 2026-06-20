// chaos_pch.h — Precompiled header for native-aot generated translation units.
//
// Includes all stable runtime headers used by every generated page file.
// Only headers that NEVER change between codegen runs belong here.
// Headers that are regenerated per-run (enum_metadata.generated.h,
// native-aot.generated.header.h) are included directly in each page.
//
// Usage: #include "chaos_pch.h"  (replaces 20+ individual #includes)
//
// This PCH is compiled once by the build system and reused across all
// page translation units, reducing total C++ compilation time by ~60%.

#ifndef CHAOS_IL2CPP_CHAOS_PCH_H_
#define CHAOS_IL2CPP_CHAOS_PCH_H_

// ── Standard C/C++ headers (needed by all generated files) ─────────
// Must be in PCH because MSVC /Yu requires the PCH include to be first;
// any includes before #include "chaos_pch.h" are IGNORED.
// Suppress C2362: codegen emits goto past local var init (structurally safe).
#pragma warning(disable : 2362)
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <chrono>
#include <cstdint>
#include <cinttypes>
#include <csignal>
#include <csetjmp>

// ── ChaOS runtime core headers ─────────────────────────────────────
#include <chaos/common.h>
#include <chaos/type_info.h>

// Unified exception-handling macros (CHAOS_EH_TRY / CHAOS_EH_CATCH_BEGIN / etc.)
// Must appear after runtime_core.h which provides the EH backend type definitions.
#include <chaos/eh.h>

#include "runtime_core.h"
#include "codegen_bridge.h"
#include "module_registry.h"
#include "abi_manifest.h"
#include "hotpatch_table.h"
#include "runtime_vtable.h"
#include "runtime_instantiation.h"
#include "reflection_query_model.h"
#include "load_store_chaos_bridge.h"

// Interpreter dispatch for hotpatch-kept-native & flat-goto fallback
#include "interpreter_entry.h"

// GC write barrier (SATB pre-write barrier for reference-type field stores).
#include <gc/gc_bgc_inline.h>

// GC card table (post-write dirty card marking for generational GC).
#include <gc/gc_card_table.h>

// GC root change tracking (BGC concurrent mark — records root slot
// overwrites so the re-mark phase can re-scan newly-unreachable objects).
#include <gc/gc_root_change.h>

// Common generated runtime prelude (shared header, ~200 lines
// of helper functions previously emitted inline in every file).
#include <ChaosGeneratedRuntimePrelude.h>

// Runtime stubs for Environment, Console, Culture, GC and delegate helpers
#include "runtime_stubs/misc_stubs.h"

// Cryptography stubs (ChaosSha256Hash, ChaosHmacSha256, etc.)
#include "runtime_stubs/crypto_stubs.h"

// Vector<T> comparison stubs (chaos_vector_greater_than_any, etc.)
#include "runtime_stubs/vector_stubs.h"

// ── Runtime host (ChaosRuntimeHost, Module registration) ───────────
#include "chaos_runtime_host.h"

// GC layout registration for generated types
#include <gc/gc_layout.h>

#endif // CHAOS_IL2CPP_CHAOS_PCH_H_
