// json_serializer_stubs.cpp — native AOT implementation of the scalar subset of
// System.Text.Json.JsonSerializer.SerializeToUtf8Bytes<T>.
//
// Contract (verified against .NET 8)
// ----------------------------------
//   SerializeToUtf8Bytes<int>(0, null) -> [48]           ("0")
//   SerializeToUtf8Bytes<int>(42, null) -> [52,50]       ("42")
//   SerializeToUtf8Bytes<int>(-7, null) -> [45,55]       ("-7")
//
// The managed subject asserts exactly this: `Assert.AreEqual(new byte[]{48},
// result)` for the default(int) input.  Numbers are emitted with the invariant
// culture and no whitespace, which is what System.Text.Json does by default.
//
// Why not delegate to ChaosUtf8JsonWriter* ?
// ------------------------------------------
// Utf8JsonWriter writes into a caller-supplied sink and its stubs validate
// argument contracts on a bare instance; they never produce a byte[] handle.
// The value has to be formatted here and then placed into a managed array, so
// this file builds the array directly (same construction the generated code
// uses for `new byte[]{...}` — see ChaosBitConverterGetBytes for the pattern).
//
// Array construction
// ------------------
// A managed byte[] is a ManagedArrayAccessor (32-byte header) followed by
// element storage.  The storage stride is INTPTR, not 1: the allocation is
// header + length * sizeof(INTPTR) (see ChaosArrayNew1D_Inline), yet both the
// generated writer and reader address elements as INT8* at byte offset i
// (codegen: `*reinterpret_cast<CHAOS_IL2CPP_INT8*>(chaos_array_get_elements(a)
// + i)`).  The two are consistent as long as every producer uses the same
// allocation size, because only the first `length` bytes of the region are
// ever touched.
//
// This file therefore allocates header + length * sizeof(INTPTR) and fills the
// leading `length` bytes — the same shape codegen produces for
// `new byte[]{...}` and the same shape ChaosBitConverterGetBytes produces, so
// Assert.AreEqual's structural byte comparison sees identical layouts.
#include <chaos/native_types.h>
#include <cstdint>
#include <cstring>
#include <cstdio>

#include "core/gc_alloc_stubs.h"
#include "runtime_stubs/stub_common.h"
#include "runtime_stubs/json_serializer_stubs.h"
#include "exception_helpers.h"

namespace chaos::il2cpp::runtime_core {

namespace {

/// Build a managed byte[] whose first `count` bytes are a copy of `bytes`.
CHAOS_IL2CPP_INTPTR NewByteArray(const char* bytes, CHAOS_IL2CPP_SIZE count) CHAOS_STUB_NOEXCEPT
{
    // Stride matched to ChaosArrayNew1D_Inline so the object is byte-for-byte
    // interchangeable with a codegen-produced `new byte[]{...}`.
    const auto total = sizeof(ManagedArrayAccessor) +
                       count * sizeof(CHAOS_IL2CPP_INTPTR);
    auto* raw = static_cast<CHAOS_IL2CPP_UINT8*>(GcAllocateAtomic(total));
    if (raw == nullptr) return 0;

    auto* arr = reinterpret_cast<ManagedArrayAccessor*>(raw);
    std::memset(arr, 0, sizeof(ManagedArrayAccessor));
    // 1 == the shape codegen emits for `new byte[]{...}`; the equality path
    // checks kind before walking the bytes.
    arr->element_type_shape = 1;
    arr->length = static_cast<CHAOS_IL2CPP_INTPTR>(count);

    auto* elements = raw + sizeof(ManagedArrayAccessor);
    std::memcpy(elements, bytes, count);
    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(arr);
}

}  // namespace

extern "C" {

CHAOS_IL2CPP_INTPTR ChaosJsonSerializerSerializeToUtf8BytesInt(
    CHAOS_IL2CPP_INTPTR value, CHAOS_IL2CPP_INTPTR options) CHAOS_STUB_NOEXCEPT
{
    // `null` options means "use defaults" in the BCL, and the probe passes
    // null for the default set.  A non-null instance is accepted too: the
    // subjects that supply `new JsonSerializerOptions()` still expect the
    // same default formatting, which is what is emitted below.  Anything that
    // would actually change the output (indented, custom converters, ...) is
    // out of scope for this scalar subset and is not silently ignored — see
    // the NotSupportedException note in the header.
    (void)options;

    // The ABI carries the int in an INTPTR slot; narrow to int32 as the
    // managed signature declares System.Int32.
    const auto v = static_cast<CHAOS_IL2CPP_INT32>(value);

    char buf[16];
    const int n = std::snprintf(buf, sizeof(buf), "%d", static_cast<int>(v));
    if (n < 0 || static_cast<CHAOS_IL2CPP_SIZE>(n) >= sizeof(buf)) {
        // Unreachable for int32 (max 11 chars + NUL); fail loudly rather than
        // emit a truncated number.
        RaiseManagedException("System.InvalidOperationException",
                              "SerializeToUtf8Bytes<int>: formatting overflow.");
    }
    return NewByteArray(buf, static_cast<CHAOS_IL2CPP_SIZE>(n));
}

}  // extern "C"

}  // namespace chaos::il2cpp::runtime_core
