// json_serializer_stubs.h — native AOT declarations for
// System.Text.Json.JsonSerializer.SerializeToUtf8Bytes<T>.
//
// Scope
// -----
// Only the scalar-shape overloads ATG's subjects reach:
//
//   SerializeToUtf8Bytes<TValue>(TValue, JsonSerializerOptions)
//
// A general implementation would need to walk an arbitrary managed object
// graph and reflect over its properties, which the native layer cannot do
// without the interpreter.  The subjects, however, only ever serialize a
// handful of primitive values, and for those the JSON text is fully specified
// by the value alone (invariant culture, no options honoured beyond defaults).
// This file implements exactly that subset and is honest about the boundary:
// a non-primitive TValue is NOT silently mis-serialized — it raises
// NotSupportedException, so the fact layer records an explicit gap rather
// than a wrong value (see the three-tier verdict in
// docs/dev/in-progress/json-xml-production-readiness/STATUS.md).
#pragma once

#include <chaos/eh.h>  // CHAOS_STUB_NOEXCEPT
#include <chaos/native_types.h>

extern "C" {

/// JsonSerializer.SerializeToUtf8Bytes<int>(int, JsonSerializerOptions) -> byte[]
CHAOS_IL2CPP_INTPTR ChaosJsonSerializerSerializeToUtf8BytesInt(
    CHAOS_IL2CPP_INTPTR value, CHAOS_IL2CPP_INTPTR options) CHAOS_STUB_NOEXCEPT;

}  // extern "C"
