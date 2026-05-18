#ifndef CHAOS_IL2CPP_DELEGATE_HELPERS_H_
#define CHAOS_IL2CPP_DELEGATE_HELPERS_H_

#include <chaos/native_types.h>
#include <core/delegate_object.h>
#include <cstddef>
#include <vector>

namespace chaos::il2cpp::runtime_core {

/// Compare two single-entry delegate values for structural equality
/// (same type_info, target, method_ptr).
bool DelegateSingleEntryEquals(CHAOS_IL2CPP_INTPTR left, CHAOS_IL2CPP_INTPTR right);

/// Flatten a delegate value into an invocation list (recursively expanding
/// multicast wrappers). Appends to the provided vector.
void DelegateAppendFlattenedEntries(
    std::vector<CHAOS_IL2CPP_INTPTR>& entries,
    CHAOS_IL2CPP_INTPTR delegate_value);

/// Validate that all entries in the flattened list share the same type_info.
void DelegateValidateEntryTypes(const std::vector<CHAOS_IL2CPP_INTPTR>& entries);

/// Create a multicast wrapper delegate containing the given entries.
/// Uses template_delegate_value to obtain the type_info for allocation.
/// Returns 0 when entries is empty, or the single entry when size == 1.
CHAOS_IL2CPP_INTPTR DelegateCreateMulticast(
    CHAOS_IL2CPP_INTPTR template_delegate_value,
    const std::vector<CHAOS_IL2CPP_INTPTR>& entries);

/// Combine two delegate values (Delegate.Combine semantics).
CHAOS_IL2CPP_INTPTR DelegateCombine(
    CHAOS_IL2CPP_INTPTR left, CHAOS_IL2CPP_INTPTR right);

/// Remove one delegate value from another (Delegate.Remove semantics).
CHAOS_IL2CPP_INTPTR DelegateRemove(
    CHAOS_IL2CPP_INTPTR source, CHAOS_IL2CPP_INTPTR value_to_remove);

}  // namespace chaos::il2cpp::runtime_core

#endif  // CHAOS_IL2CPP_DELEGATE_HELPERS_H_
