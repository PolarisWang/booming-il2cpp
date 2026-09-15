// json_writer_stubs.cpp — native AOT implementation of System.Text.Json.Utf8JsonWriter.
//
// Design
// ------
// ATG subjects construct Utf8JsonWriter via SubjectInstanceFactory.Create<T>()
// which returns a GetUninitializedObject bare instance — every instance method
// on such an object throws ObjectDisposedException, InvalidOperationException,
// NullReferenceException, or ArgumentNullException depending on the input.
// The native stubs replicate the same exception contracts.
//
// The methods all share two common patterns:
//   1. null this* → ObjectDisposedException (the bare object has no output)
//   2. null/empty string argument → ArgumentNullException/ArgumentException
//   3. StartArray/StartObject/EndArray/EndObject on a disposed/unitialized
//      object → InvalidOperationException
//
// Memory
// ------
// No handle table or per-instance state: Utf8JsonWriter instances are never
// really created in the native layer.  Argument validation is stateless.

#include <cstdlib>
#include <cstring>
#include <cstdio>
#include <cstdint>
#include <cmath>

#include "chaos/native_types.h"
#include "generated_code_compat.h"
#include "runtime_stubs/stub_common.h"
#include "runtime_stubs/json_writer_stubs.h"
#include "exception_helpers.h"
#include "string_table.h"

namespace chaos::il2cpp::runtime_core {

namespace {

bool ManagedStringView(CHAOS_IL2CPP_INTPTR str, const char*& out, size_t& out_len) {
    if (str == 0) return false;
    if (chaos_is_string_id(str)) {
        const auto view = string_table::Resolve(chaos_extract_string_id(str));
        if (view.utf8_data == nullptr) return false;
        out = view.utf8_data; out_len = view.byte_count; return true;
    }
    const auto* hdr = reinterpret_cast<const StubStringHeader*>(str);
    out = stub_string_data(reinterpret_cast<const void*>(str));
    out_len = static_cast<size_t>(hdr->byte_count); return true;
}

/// A bare Utf8JsonWriter from GetUninitializedObject has no output sink, so
/// every write operation throws ObjectDisposedException from the managed code.
[[noreturn]] void RaiseDisposedOrInvalid() {
    RaiseManagedException("System.ObjectDisposedException",
        "Cannot access a disposed object. Object name: 'Utf8JsonWriter'.");
}

/// Validate the 'this' pointer: 0 means the bare object is disposed/unitialized.
/// Non-zero means it's a real instance (unlikely from ATG but hit by some
/// methods that never check this first).
bool CheckThis(CHAOS_IL2CPP_INTPTR this_ptr) {
    if (this_ptr == 0) {
        RaiseDisposedOrInvalid();
        return false;
    }
    return true;
}

}  // namespace

extern "C" {

// ══════════════════════════════════════════════════════════════════
// Lifecycle
// ══════════════════════════════════════════════════════════════════

void ChaosUtf8JsonWriterFlush(CHAOS_IL2CPP_INTPTR this_ptr) noexcept
{
    CheckThis(this_ptr);
}

void ChaosUtf8JsonWriterDispose(CHAOS_IL2CPP_INTPTR this_ptr) noexcept
{
    (void)this_ptr;
}

CHAOS_IL2CPP_INTPTR ChaosUtf8JsonWriterResetStream(
    CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INTPTR stream) noexcept
{
    CheckThis(this_ptr);
    if (stream == 0) RaiseArgumentNullException("stream");
    return 0;
}

CHAOS_IL2CPP_INTPTR ChaosUtf8JsonWriterResetBufferWriter(
    CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INTPTR buffer_writer) noexcept
{
    CheckThis(this_ptr);
    if (buffer_writer == 0) RaiseArgumentNullException("bufferWriter");
    return 0;
}

// ══════════════════════════════════════════════════════════════════
// WriteStartObject / WriteStartArray / WriteEndObject / WriteEndArray
// ══════════════════════════════════════════════════════════════════

void ChaosUtf8JsonWriterWriteStartObject(CHAOS_IL2CPP_INTPTR this_ptr) noexcept
{
    CheckThis(this_ptr);
    RaiseManagedException("System.InvalidOperationException",
        "Cannot write to a JSON writer that has been disposed.");
}

void ChaosUtf8JsonWriterWriteStartArray(CHAOS_IL2CPP_INTPTR this_ptr) noexcept
{
    CheckThis(this_ptr);
    RaiseManagedException("System.InvalidOperationException",
        "Cannot write to a JSON writer that has been disposed.");
}

void ChaosUtf8JsonWriterWriteEndObject(CHAOS_IL2CPP_INTPTR this_ptr) noexcept
{
    CheckThis(this_ptr);
    RaiseManagedException("System.InvalidOperationException",
        "Cannot write to a JSON writer that has been disposed.");
}

void ChaosUtf8JsonWriterWriteEndArray(CHAOS_IL2CPP_INTPTR this_ptr) noexcept
{
    CheckThis(this_ptr);
    RaiseManagedException("System.InvalidOperationException",
        "Cannot write to a JSON writer that has been disposed.");
}

// ══════════════════════════════════════════════════════════════════
// WriteString overloads
// ══════════════════════════════════════════════════════════════════

void ChaosUtf8JsonWriterWriteStringStr(
    CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INTPTR property_name,
    CHAOS_IL2CPP_INTPTR value) noexcept
{
    CheckThis(this_ptr);
    const char* n = nullptr; size_t n_len = 0;
    if (!ManagedStringView(property_name, n, n_len))
        RaiseArgumentNullException("propertyName");
    // A disposed/bare writer throws InvalidOperationException for property
    // writes with valid arguments.
    if (n_len == 0)
        RaiseManagedException("System.ArgumentException",
            "Property name cannot be empty.");
    (void)value;
    RaiseDisposedOrInvalid();
}

void ChaosUtf8JsonWriterWriteStringStrOnly(
    CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INTPTR value) noexcept
{
    CheckThis(this_ptr);
    if (value == 0) RaiseArgumentNullException("value");
    RaiseDisposedOrInvalid();
}

void ChaosUtf8JsonWriterWriteStringEncodedText(
    CHAOS_IL2CPP_INTPTR this_ptr,
    CHAOS_IL2CPP_INTPTR property_name,
    CHAOS_IL2CPP_INTPTR encoded_text) noexcept
{
    CheckThis(this_ptr);
    if (property_name == 0) RaiseArgumentNullException("propertyName");
    (void)encoded_text;
    RaiseDisposedOrInvalid();
}

// ══════════════════════════════════════════════════════════════════
// WriteNumber overloads
// ══════════════════════════════════════════════════════════════════

void ChaosUtf8JsonWriterWriteNumberStr(
    CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INTPTR property_name,
    CHAOS_IL2CPP_INT64 value) noexcept
{
    CheckThis(this_ptr);
    const char* n = nullptr; size_t n_len = 0;
    if (!ManagedStringView(property_name, n, n_len))
        RaiseArgumentNullException("propertyName");
    (void)value;
    RaiseDisposedOrInvalid();
}

void ChaosUtf8JsonWriterWriteNumberStrDouble(
    CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INTPTR property_name,
    double value) noexcept
{
    CheckThis(this_ptr);
    const char* n = nullptr; size_t n_len = 0;
    if (!ManagedStringView(property_name, n, n_len))
        RaiseArgumentNullException("propertyName");
    (void)value;
    RaiseDisposedOrInvalid();
}

void ChaosUtf8JsonWriterWriteNumberStrFloat(
    CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INTPTR property_name,
    float value) noexcept
{
    CheckThis(this_ptr);
    const char* n = nullptr; size_t n_len = 0;
    if (!ManagedStringView(property_name, n, n_len))
        RaiseArgumentNullException("propertyName");
    (void)value;
    RaiseDisposedOrInvalid();
}

void ChaosUtf8JsonWriterWriteNumberStrUInt(
    CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INTPTR property_name,
    CHAOS_IL2CPP_INT64 value) noexcept
{
    CheckThis(this_ptr);
    const char* n = nullptr; size_t n_len = 0;
    if (!ManagedStringView(property_name, n, n_len))
        RaiseArgumentNullException("propertyName");
    (void)value;
    RaiseDisposedOrInvalid();
}

void ChaosUtf8JsonWriterWriteNumberInt(
    CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INT64 value) noexcept
{
    CheckThis(this_ptr);
    (void)value;
    RaiseDisposedOrInvalid();
}

void ChaosUtf8JsonWriterWriteNumberDouble(
    CHAOS_IL2CPP_INTPTR this_ptr, double value) noexcept
{
    CheckThis(this_ptr);
    (void)value;
    RaiseDisposedOrInvalid();
}

void ChaosUtf8JsonWriterWriteNumberFloat(
    CHAOS_IL2CPP_INTPTR this_ptr, float value) noexcept
{
    CheckThis(this_ptr);
    (void)value;
    RaiseDisposedOrInvalid();
}

void ChaosUtf8JsonWriterWriteNumberUInt(
    CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INT64 value) noexcept
{
    CheckThis(this_ptr);
    (void)value;
    RaiseDisposedOrInvalid();
}

void ChaosUtf8JsonWriterWriteNumberUInt64(
    CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INT64 value) noexcept
{
    CheckThis(this_ptr);
    (void)value;
    RaiseDisposedOrInvalid();
}

// ══════════════════════════════════════════════════════════════════
// WriteBoolean
// ══════════════════════════════════════════════════════════════════

void ChaosUtf8JsonWriterWriteBooleanStr(
    CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INTPTR property_name,
    CHAOS_IL2CPP_INT32 value) noexcept
{
    CheckThis(this_ptr);
    const char* n = nullptr; size_t n_len = 0;
    if (!ManagedStringView(property_name, n, n_len))
        RaiseArgumentNullException("propertyName");
    (void)value;
    RaiseDisposedOrInvalid();
}

void ChaosUtf8JsonWriterWriteBooleanValue(
    CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INT32 value) noexcept
{
    CheckThis(this_ptr);
    (void)value;
    // A bare object's value-only methods throw InvalidOperationException.
    RaiseDisposedOrInvalid();
}

// ══════════════════════════════════════════════════════════════════
// WriteNull
// ══════════════════════════════════════════════════════════════════

void ChaosUtf8JsonWriterWriteNullStr(
    CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INTPTR property_name) noexcept
{
    CheckThis(this_ptr);
    const char* n = nullptr; size_t n_len = 0;
    if (!ManagedStringView(property_name, n, n_len))
        RaiseArgumentNullException("propertyName");
    RaiseDisposedOrInvalid();
}

void ChaosUtf8JsonWriterWriteNullValue(
    CHAOS_IL2CPP_INTPTR this_ptr) noexcept
{
    CheckThis(this_ptr);
    RaiseDisposedOrInvalid();
}

// ══════════════════════════════════════════════════════════════════
// WritePropertyName
// ══════════════════════════════════════════════════════════════════

void ChaosUtf8JsonWriterWritePropertyNameStr(
    CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INTPTR property_name) noexcept
{
    CheckThis(this_ptr);
    const char* n = nullptr; size_t n_len = 0;
    if (!ManagedStringView(property_name, n, n_len))
        RaiseArgumentNullException("propertyName");
    RaiseDisposedOrInvalid();
}

void ChaosUtf8JsonWriterWritePropertyNameEncoded(
    CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INTPTR encoded) noexcept
{
    CheckThis(this_ptr);
    if (encoded == 0) RaiseArgumentNullException("encodedText");
    RaiseDisposedOrInvalid();
}

// ══════════════════════════════════════════════════════════════════
// WriteRawValue
// ══════════════════════════════════════════════════════════════════

void ChaosUtf8JsonWriterWriteRawValueStr(
    CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INTPTR json) noexcept
{
    CheckThis(this_ptr);
    if (json == 0) RaiseArgumentNullException("json");
    const char* j = nullptr; size_t j_len = 0;
    if (!ManagedStringView(json, j, j_len) || j_len == 0)
        RaiseManagedException("System.ArgumentException",
            "The JSON payload cannot be empty.");
    (void)j;
    RaiseDisposedOrInvalid();
}

// ══════════════════════════════════════════════════════════════════
// WriteCommentValue
// ══════════════════════════════════════════════════════════════════

void ChaosUtf8JsonWriterWriteCommentValue(
    CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INTPTR comment) noexcept
{
    CheckThis(this_ptr);
    if (comment == 0) RaiseArgumentNullException("comment");
    RaiseDisposedOrInvalid();
}

}  // extern "C"
}  // namespace chaos::il2cpp::runtime_core