#ifndef CHAOS_IL2CPP_REFLECTION_BINDING_FLAGS_H_
#define CHAOS_IL2CPP_REFLECTION_BINDING_FLAGS_H_

#include "reflection_query_model.h"

namespace chaos::il2cpp::runtime_core {

// BindingFlags values matching System.Reflection.BindingFlags (passed from managed code)
static constexpr int kBindingFlagsPublic           = 0x10;
static constexpr int kBindingFlagsNonPublic        = 0x20;
static constexpr int kBindingFlagsInstance         = 0x04;
static constexpr int kBindingFlagsStatic           = 0x08;
static constexpr int kBindingFlagsDeclaredOnly     = 0x02;
static constexpr int kBindingFlagsFlattenHierarchy = 0x40;

// Default binding flags when 0 is passed (matches .NET default: Instance | Public)
inline int DefaultBindingFlags() noexcept {
    return kBindingFlagsInstance | kBindingFlagsPublic;
}

// Apply default if flags == 0
inline int NormalizeBindingFlags(int flags) noexcept {
    return flags == 0 ? DefaultBindingFlags() : flags;
}

// ── BindingFlags matching helpers ──────────────────────────────────────
// These check whether a member with given descriptor flags matches the
// BindingFlags requested by the caller.

// Match a field descriptor against binding flags
inline bool MatchFieldFlags(uint32_t field_flags, int binding_flags) noexcept {
    // Access check: if any of Public/NonPublic is specified, apply the filter
    if ((binding_flags & (kBindingFlagsPublic | kBindingFlagsNonPublic)) != 0) {
        bool is_public = (field_flags & kFieldFlagIsPublic) != 0u;
        if (is_public && !(binding_flags & kBindingFlagsPublic)) return false;
        if (!is_public && !(binding_flags & kBindingFlagsNonPublic)) return false;
    }

    // Static/Instance check: if any is specified, apply the filter
    if ((binding_flags & (kBindingFlagsStatic | kBindingFlagsInstance)) != 0) {
        bool is_static = (field_flags & kFieldFlagIsStatic) != 0u;
        if (is_static && !(binding_flags & kBindingFlagsStatic)) return false;
        if (!is_static && !(binding_flags & kBindingFlagsInstance)) return false;
    }

    return true;
}

// Match a method descriptor against binding flags
// Method flags use the same bit layout as field flags for Public/Static
inline bool MatchMethodFlags(uint32_t method_flags, int binding_flags) noexcept {
    return MatchFieldFlags(method_flags, binding_flags);
}

// Match a property descriptor against binding flags
inline bool MatchPropertyFlags(uint32_t property_flags, int binding_flags) noexcept {
    // Static/Instance check: if any is specified, apply the filter
    if ((binding_flags & (kBindingFlagsStatic | kBindingFlagsInstance)) != 0) {
        bool is_static = (property_flags & kPropertyFlagIsStatic) != 0u;
        if (is_static && !(binding_flags & kBindingFlagsStatic)) return false;
        if (!is_static && !(binding_flags & kBindingFlagsInstance)) return false;
    }
    return true;
}

}  // namespace chaos::il2cpp::runtime_core

#endif  // CHAOS_IL2CPP_REFLECTION_BINDING_FLAGS_H_
