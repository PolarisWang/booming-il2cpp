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
//
// char[] overloads (Add/Get(char[],int,int)) decode the UTF-16 window to UTF-8
// before interning, so the same content reached via either overload yields the
// same interned reference.
#include <cstdint>
#include <cstring>
#include <cstdlib>

#include "generated_code_compat.h"
#include "runtime_stubs/stub_common.h"
#include "exception_helpers.h"
#include "string_table.h"

namespace chaos::il2cpp::runtime_core {

// ── UTF-16 → UTF-8 conversion helpers ─────────────────────────────

/// Simple worst-case buffer for UTF-16-to-UTF-8 conversion.
/// The char[] window is at most INT32_MAX characters; UTF-8 expansion is
/// at most 3 bytes per BMP code unit (no surrogates), so 3×len + 1 suffices.
static char* utf16_window_to_utf8(const CHAOS_IL2CPP_UINT16* src,
                                   CHAOS_IL2CPP_INT32 start,
                                   CHAOS_IL2CPP_INT32 len)
{
    if (len <= 0) return nullptr;
    const auto cap = static_cast<size_t>(len) * 3 + 1;
    auto* buf = static_cast<char*>(CHAOS_IL2CPP_MALLOC(cap));
    if (buf == nullptr) return nullptr;

    size_t wi = 0;
    for (CHAOS_IL2CPP_INT32 i = 0; i < len && wi + 3 < cap; ++i)
    {
        const auto u = static_cast<uint32_t>(src[static_cast<size_t>(start) + i]);
        if (u < 0x80U)       { buf[wi++] = static_cast<char>(u); }
        else if (u < 0x800U) { buf[wi++] = static_cast<char>(0xC0 | (u >> 6));
                               buf[wi++] = static_cast<char>(0x80 | (u & 0x3F)); }
        else                 { buf[wi++] = static_cast<char>(0xE0 | (u >> 12));
                               buf[wi++] = static_cast<char>(0x80 | ((u >> 6) & 0x3F));
                               buf[wi++] = static_cast<char>(0x80 | (u & 0x3F)); }
    }
    buf[wi] = '\0';

    return buf;  // Caller frees via CHAOS_IL2CPP_FREE. Returns the oversized
                  // buffer (at most 3×len+1) rather than reallocating to exact
                  // size, avoiding a copy + a null-terminator bug.
}

/// Decode the source char[] into its UTF-16 element array.
static const CHAOS_IL2CPP_UINT16* resolve_char_array_data(CHAOS_IL2CPP_INTPTR key)
{
    // Could be a codegen StubArrayHeader* or a real ManagedArrayAccessor*.
    // Both have `length` as the second field; element data is contiguous
    // after the header.  For char[], elements are 2-byte UTF-16 code units.
    const auto* hdr = reinterpret_cast<const StubArrayHeader*>(key);
    (void)hdr->length;  // validate the pointer is readable
    return reinterpret_cast<const CHAOS_IL2CPP_UINT16*>(
        reinterpret_cast<const uint8_t*>(key) + sizeof(StubArrayHeader));
}

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

// ── char[] overloads (AddChars / GetChars) ───────────────────────

}  // namespace chaos::il2cpp::runtime_core

namespace chaos::il2cpp::runtime_core {
extern "C" {

CHAOS_IL2CPP_INTPTR ChaosXmlNameTableAddChars(
    CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INTPTR key,
    CHAOS_IL2CPP_INT32 start, CHAOS_IL2CPP_INT32 len) noexcept
{
    if (this_ptr == 0) { RaiseNullReferenceException(); return 0; }
    if (key == 0)
    {
        RaiseManagedException("System.ArgumentNullException",
                              "Value cannot be null. (Parameter 'key')");
        return 0;
    }
    if (start < 0 || len < 0)
    {
        RaiseManagedException("System.ArgumentOutOfRangeException",
                              "start and len must be non-negative.");
        return 0;
    }

    const auto* u16 = resolve_char_array_data(key);
    auto* utf8 = utf16_window_to_utf8(u16, start, len);
    if (utf8 == nullptr) return 0;

    const auto byte_len = static_cast<CHAOS_IL2CPP_UINT32>(std::strlen(utf8));
    string_table::StringId id = 0;

    if (byte_len == 0)
    {
        constexpr uint64_t kFnvOffsetBasis = 14695981039346656037ULL;
        constexpr uint64_t kFnvEmpty = (kFnvOffsetBasis & ~(1ULL << 63)) | 1ULL;
        id = kFnvEmpty;
    }
    else
    {
        id = string_table::Intern(utf8, byte_len);
    }

    CHAOS_IL2CPP_FREE(utf8);
    if (id == string_table::kStringIdNull) return 0;
    return static_cast<CHAOS_IL2CPP_INTPTR>(id | CHAOS_STRING_ID_TAG);
}

CHAOS_IL2CPP_INTPTR ChaosXmlNameTableGetChars(
    CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INTPTR key,
    CHAOS_IL2CPP_INT32 start, CHAOS_IL2CPP_INT32 len) noexcept
{
    using namespace chaos::il2cpp::runtime_core;
    if (this_ptr == 0) { RaiseNullReferenceException(); return 0; }
    if (key == 0) return 0;  // null array → null result (the string overload
                              // also returns 0 for null input — contrast with
                              // Add which throws ArgumentNullException).

    const auto* u16 = resolve_char_array_data(key);
    auto* utf8 = utf16_window_to_utf8(u16, start, len);
    if (utf8 == nullptr) return 0;

    const auto byte_len = static_cast<CHAOS_IL2CPP_UINT32>(std::strlen(utf8));
    string_table::StringId id = 0;

    if (byte_len == 0)
    {
        constexpr uint64_t kFnvOffsetBasis = 14695981039346656037ULL;
        constexpr uint64_t kFnvEmpty = (kFnvOffsetBasis & ~(1ULL << 63)) | 1ULL;
        id = kFnvEmpty;
    }
    else
    {
        id = string_table::Find(utf8, byte_len);
    }

    CHAOS_IL2CPP_FREE(utf8);
    if (id == string_table::kStringIdNull) return 0;
    return static_cast<CHAOS_IL2CPP_INTPTR>(id | CHAOS_STRING_ID_TAG);
}

}  // extern "C"

}  // namespace chaos::il2cpp::runtime_core
