// HotUpdate test configuration header.
//
// The CodeGen pipeline emits C++ that references these symbols but doesn't
// define them. This header provides the missing definitions so that the
// CodeGen-generated translation units compile correctly.
//
// TODO: Move these definitions into the CodeGen pipeline's C++ emission
//       (NativeAotLoweringPlanner) so they are emitted by the pipeline
//       itself, not supplied externally.

#ifndef CHAOS_NATIVE_HOTUPDATE_CONFIG_H_
#define CHAOS_NATIVE_HOTUPDATE_CONFIG_H_

#include <chaos/common.h>
#include "string_table.h"
#include "codegen_bridge.h"
#include <cstdint>
#include <cstring>

// ---------------------------------------------------------------------------
// chaos_managed_pointer_local_slot_tag
//
// NOT defined via #define. Each CodeGen-generated TU defines its own
// anonymous-namespace constexpr alias — TU-local, no ODR violation when
// multiple TUs (host + patch + semantic-patch) are linked together.
// The generate_hotupdate_test.py _rename_and_fix_patch_file step
// uncomments the stripped constexpr line in patch/semantic-patch copies.
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// chaos_require_dictionary_runtime_storage
//
// CodeGen external helpers for Dictionary<TKey,TValue> emit this call, but
// collection.h defines it inside namespace ChaosIl2cpp::Common. Map it to
// the namespaced function so the anonymous-namespace preamble can find it.
// ---------------------------------------------------------------------------
#define chaos_require_dictionary_runtime_storage ChaosIl2cpp::Common::require_dictionary_runtime_storage
#define chaos_require_list_runtime_storage ChaosIl2cpp::Common::require_list_runtime_storage

#endif // CHAOS_NATIVE_HOTUPDATE_CONFIG_H_
