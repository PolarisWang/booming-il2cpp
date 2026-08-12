// xml_nametable_stubs.cpp — native AOT implementation of
// System.Xml.NameTable.Add(string) / XmlNameTable.Get(string) interned-string
// semantics.
//
// Design (see xml_nametable_stubs.h for the contract):
//   A managed String is either a tagged StringId (CHAOS_STRING_ID_TAG | id)
//   backed by the permanent string_table, or a real StubStringHeader*.
//   * Add  → string_table::Intern(bytes): atomic dedupe; same content yields
//            the same StringId → the same tagged String reference is returned,
//            satisfying the reference-equality contract.
//   * Get  → string_table::Find(bytes): query-only, never inserts.
//   Both are allocated in the CHAOS_IL2CPP_MALLOC string_table pool, which is
//   immutable/permanent → permanently rooted, no GC collection, no new
//   allocation domain introduced.
#include <chaos/native_types.h>
#include <cstring>

#include "generated_code_compat.h"
#include "runtime_stubs/stub_common.h"
#include "exception_helpers.h"  // RaiseNullReferenceException / RaiseManagedException
#include "string_table.h"

namespace chaos::il2cpp::runtime_core {

/// Resolve a managed String argument to its {utf8_data, byte_count} view without
/// allocating. Handles both representations:
///   * tagged StringId → resolved through string_table.
///   * real StubStringHeader* → inline bytes via stub_string_data.
/// Returns true when the argument holds a non-null, resolvable string.
static bool utf8_view_of_string(
    CHAOS_IL2CPP_INTPTR str, string_table::StringView& out) noexcept
{
    if (str == 0)
    {
        return false;
    }

    if (chaos_is_string_id(str))
    {
        // Tagged StringId path — resolve through string_table.
        const auto view = string_table::Resolve(chaos_extract_string_id(str));
        if (view.utf8_data == nullptr)
        {
            return false;
        }
        out = view;
        return true;
    }

    // Real StubStringHeader* — bytes are contiguous after the header.
    const auto* header = reinterpret_cast<const StubStringHeader*>(str);
    out.utf8_data = stub_string_data(reinterpret_cast<const void*>(str));
    out.byte_count = static_cast<CHAOS_IL2CPP_UINT32>(header->byte_count);
    return true;
}

extern "C" {

CHAOS_IL2CPP_INTPTR ChaosXmlNameTableAddString(
    CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INTPTR name) noexcept
{
    // .NET NameTable.Add on a null receiver throws NullReferenceException.
    if (this_ptr == 0)
    {
        RaiseNullReferenceException();
        return 0;  // unreachable (noreturn), but keeps control flow explicit.
    }

    string_table::StringView view;
    if (name == 0)
    {
        // .NET NameTable.Add(null) throws ArgumentNullException.
        RaiseManagedException("System.ArgumentNullException",
                              "Value cannot be null. (Parameter 'name')");
        return 0;  // unreachable (noreturn), keeps control flow explicit.
    }
    if (!utf8_view_of_string(name, view))
    {
        // Unresolvable string-id (shouldn't happen for valid managed strings).
        return 0;
    }

    // Interning the empty string: string_table::Intern explicitly returns
    // kStringIdNull for byte_count==0, so the empty literal cannot be interned
    // that way. The compiler's ldstr "" emits CHAOS_IL2CPP_STRING_ID(""), whose
    // hash is chaos_constexpr_string_hash(seed, 0) = (seed & ~bit63) | 1.
    // Mirror that EXACT value so Add("") and a compiled "" literal share one
    // reference. (The FNV seed has bit 63 set; it must be cleared before OR-ing
    // the tag, or the returned tagged value collides with the tag bit.)
    if (view.byte_count == 0u)
    {
        constexpr uint64_t kFnvOffsetBasis = 14695981039346656037ULL;
        constexpr uint64_t kFnvEmpty = (kFnvOffsetBasis & ~(1ULL << 63)) | 1ULL;
        return static_cast<CHAOS_IL2CPP_INTPTR>(kFnvEmpty | CHAOS_STRING_ID_TAG);
    }

    const auto id =
        string_table::Intern(view.utf8_data, view.byte_count);
    if (id == string_table::kStringIdNull)
    {
        return 0;  // OOM/guard: unrepresentable.
    }
    return static_cast<CHAOS_IL2CPP_INTPTR>(id | CHAOS_STRING_ID_TAG);
}

CHAOS_IL2CPP_INTPTR ChaosXmlNameTableGetString(
    CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INTPTR name) noexcept
{
    // XmlNameTable.Get on a null receiver throws NullReferenceException.
    if (this_ptr == 0)
    {
        RaiseNullReferenceException();
        return 0;
    }

    string_table::StringView view;
    if (!utf8_view_of_string(name, view))
    {
        return 0;  // null string → null result.
    }

    // Query-only: never inserts. The empty string is a well-formed reference
    // (matches the compiled "" literal), so Get("") returns it rather than null.
    if (view.byte_count == 0u)
    {
        constexpr uint64_t kFnvOffsetBasis = 14695981039346656037ULL;
        constexpr uint64_t kFnvEmpty = (kFnvOffsetBasis & ~(1ULL << 63)) | 1ULL;
        return static_cast<CHAOS_IL2CPP_INTPTR>(kFnvEmpty | CHAOS_STRING_ID_TAG);
    }

    const auto id = string_table::Find(view.utf8_data, view.byte_count);
    if (id == string_table::kStringIdNull)
    {
        return 0;  // absent → null.
    }
    return static_cast<CHAOS_IL2CPP_INTPTR>(id | CHAOS_STRING_ID_TAG);
}

}  // extern "C"

}  // namespace chaos::il2cpp::runtime_core
