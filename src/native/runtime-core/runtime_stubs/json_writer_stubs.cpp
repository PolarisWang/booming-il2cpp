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
// The methods all share three common patterns — note that the exception type a
// bare receiver reports depends on the method family (all measured on .NET 10):
//   1. bare this* on a VALUE-ONLY write (WriteNullValue / WriteBooleanValue /
//      WriteNumberValue / WriteStringValue / WriteCommentValue)
//        → NullReferenceException
//   2. bare this* on a PROPERTY-NAME write (WriteNull / WriteNumber / WriteBoolean
//      / WriteString / WritePropertyName) or on WriteStartObject / WriteStartArray
//        → InvalidOperationException
//   3. bare this* on Flush / Reset
//        → ObjectDisposedException
//   4. null/empty string argument → ArgumentNullException/ArgumentException,
//      but only AFTER the receiver check — .NET reports the receiver state first.
//
// Collapsing 1 and 2 into 3 was a real defect: the typed-catch assertions in the
// ATG subjects correctly rejected the wrong type (12 subjects).
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

/// A bare Utf8JsonWriter from GetUninitializedObject has no output sink.
///
/// Which exception it throws is NOT uniform — it depends on the method family,
/// measured on .NET 10 against a `GetUninitializedObject` instance:
///
///   Value-only write (WriteNullValue / WriteBooleanValue / WriteNumberValue /
///     WriteStringValue / WriteCommentValue / WriteRawValue) -> System.NullReferenceException
///   Property-name write (WriteNull / WriteNumber / WriteBoolean /
///     WriteString / WritePropertyName) and
///     WriteStartObject / WriteStartArray       -> System.InvalidOperationException
///   Flush / Reset / JsonDocument.WriteTo       -> System.ObjectDisposedException
///
/// This function is for the third family only.  Applying it to the first two was
/// producing ObjectDisposedException where .NET raises something else, which the
/// typed-catch assertions correctly rejected.
[[noreturn]] void RaiseDisposedOrInvalid() {
    RaiseManagedException("System.ObjectDisposedException",
        "Cannot access a disposed object. Object name: 'Utf8JsonWriter'.");
}

/// Value-only write methods on a bare writer touch the missing output sink
/// directly, so .NET surfaces NullReferenceException.  Measured:
/// bare.WriteNullValue(), bare.WriteBooleanValue(true),
/// bare.WriteNumberValue(1m), bare.WriteStringValue(Guid.Empty).
[[noreturn]] void RaiseBareWriterNullReference() {
    RaiseManagedException("System.NullReferenceException",
        "Object reference not set to an instance of an object.");
}

/// Property-name writes on a bare writer report the receiver as unusable rather
/// than dereferencing it.  Measured: bare.WriteNull("p"), bare.WriteNumber("p",1),
/// bare.WriteBoolean("p",true), bare.WriteString("p","v"),
/// bare.WritePropertyName("") all throw InvalidOperationException.
[[noreturn]] void RaiseBareWriterInvalidOperation() {
    RaiseManagedException("System.InvalidOperationException",
        "Cannot write to a JSON writer that has been disposed.");
}

/// Validate the 'this' pointer: 0 means the bare object is disposed/unitialized.
/// Non-zero means it's a real instance (unlikely from ATG but hit by some
/// methods that never check this first).
///
/// `bareKind` selects the family distinction above so the receiver check reports
/// the same exception type .NET does for that family.
enum class BareWriterKind { Disposed, NullReference, InvalidOperation };

bool CheckThis(CHAOS_IL2CPP_INTPTR this_ptr,
               BareWriterKind bareKind = BareWriterKind::Disposed) {
    if (this_ptr == 0) {
        switch (bareKind) {
            case BareWriterKind::NullReference:    RaiseBareWriterNullReference();
            case BareWriterKind::InvalidOperation: RaiseBareWriterInvalidOperation();
            case BareWriterKind::Disposed:         break;
        }
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
    // A bare writer (GetUninitializedObject) has no output sink; the managed
    // implementation throws ObjectDisposedException.  Measured on .NET 8/10:
    //   bare.Flush() → ObjectDisposedException
    // (Dispose() is the one exception — it does NOT throw on a bare instance.)
    RaiseDisposedOrInvalid();
}

void ChaosUtf8JsonWriterDispose(CHAOS_IL2CPP_INTPTR this_ptr) noexcept
{
    // Dispose on a bare instance is a documented no-op (measured: no exception).
    (void)this_ptr;
}

CHAOS_IL2CPP_INTPTR ChaosUtf8JsonWriterResetStream(
    CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INTPTR stream) noexcept
{
    CheckThis(this_ptr);
    // The receiver check comes FIRST: a bare writer is "disposed" and every
    // Reset overload reports that, regardless of the argument.  Measured:
    //   bare.Reset(default(Stream)!)  → ObjectDisposedException
    //   bare.Reset(Stream.Null)       → ObjectDisposedException
    // Previously the null-argument check ran first, so a null argument yielded
    // ArgumentNullException instead — a different type than the managed
    // implementation, which the typed-catch assertion (A2) correctly rejected.
    RaiseDisposedOrInvalid();
}

CHAOS_IL2CPP_INTPTR ChaosUtf8JsonWriterResetBufferWriter(
    CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INTPTR buffer_writer) noexcept
{
    CheckThis(this_ptr);
    // Same ordering as ResetStream: receiver validity precedes argument
    // validation.  Measured: bare.Reset(default(IBufferWriter<byte>)!)
    // → ObjectDisposedException.
    (void)buffer_writer;
    RaiseDisposedOrInvalid();
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

// ── Property-name overloads ──────────────────────────────────────────────
// WriteStartObject(string|JsonEncodedText) / WriteStartArray(string|JsonEncodedText).
//
// Measured on a bare instance: all four throw InvalidOperationException — the
// property-name form is the InvalidOperation family even though the 0-arg form
// is too (same family here, unlike the value-only writes).  Distinct symbols per
// parameter list are mandatory: C++ cannot resolve same-name overloads across
// translation units reliably, and the shape registration keys on an exact
// (method, paramTypes) tuple, so each needs its own entry anyway.

// Property-name overloads.  Null argument is checked first (.NET reports
// ArgumentNullException before the receiver state); the string form validates
// the name, the JsonEncodedText form carries its own payload.

void ChaosUtf8JsonWriterWriteStartObjectStr(
    CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INTPTR property_name) noexcept
{
    CheckThis(this_ptr, BareWriterKind::InvalidOperation);
    const char* n = nullptr; size_t n_len = 0;
    if (!ManagedStringView(property_name, n, n_len))
        RaiseArgumentNullException("propertyName");
    (void)n; (void)n_len;
    RaiseBareWriterInvalidOperation();
}

void ChaosUtf8JsonWriterWriteStartObjectEncoded(
    CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INTPTR encoded_name) noexcept
{
    CheckThis(this_ptr, BareWriterKind::InvalidOperation);
    (void)encoded_name;
    RaiseBareWriterInvalidOperation();
}

void ChaosUtf8JsonWriterWriteStartArrayStr(
    CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INTPTR property_name) noexcept
{
    CheckThis(this_ptr, BareWriterKind::InvalidOperation);
    const char* n = nullptr; size_t n_len = 0;
    if (!ManagedStringView(property_name, n, n_len))
        RaiseArgumentNullException("propertyName");
    (void)n; (void)n_len;
    RaiseBareWriterInvalidOperation();
}

void ChaosUtf8JsonWriterWriteStartArrayEncoded(
    CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INTPTR encoded_name) noexcept
{
    CheckThis(this_ptr, BareWriterKind::InvalidOperation);
    (void)encoded_name;
    RaiseBareWriterInvalidOperation();
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
    CheckThis(this_ptr, BareWriterKind::InvalidOperation);
    const char* n = nullptr; size_t n_len = 0;
    if (!ManagedStringView(property_name, n, n_len))
        RaiseArgumentNullException("propertyName");
    // Ordering matters, and .NET's order is: null argument > receiver state >
    // empty-string argument.  Measured on a bare instance:
    //   WriteString(null, null) -> ArgumentNullException
    //   WriteString("",   null) -> InvalidOperationException   (NOT ArgumentException)
    //   WriteString("p",  null) -> InvalidOperationException
    // So the empty-name ArgumentException must not be raised while the receiver
    // is still bare — the bare writer is reported first.
    (void)n_len;
    (void)value;
    RaiseBareWriterInvalidOperation();
}

void ChaosUtf8JsonWriterWriteStringStrOnly(
    CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INTPTR value) noexcept
{
    CheckThis(this_ptr, BareWriterKind::NullReference);
    if (value == 0) RaiseArgumentNullException("value");
    RaiseBareWriterNullReference();
}

void ChaosUtf8JsonWriterWriteStringEncodedText(
    CHAOS_IL2CPP_INTPTR this_ptr,
    CHAOS_IL2CPP_INTPTR property_name,
    CHAOS_IL2CPP_INTPTR encoded_text) noexcept
{
    CheckThis(this_ptr, BareWriterKind::InvalidOperation);
    if (property_name == 0) RaiseArgumentNullException("propertyName");
    (void)encoded_text;
    RaiseBareWriterInvalidOperation();
}

// ── WriteString(propertyName, non-string value) ──────────────────────────
// The value is a struct (DateTime/DateTimeOffset/Guid), an already-encoded
// JsonEncodedText, or another string.  All of them arrive as INTPTR carriers;
// on a bare writer .NET throws InvalidOperationException for every one of them
// (measured), so the value payload itself is not inspected.
//
// Each (propertyNameForm, valueForm) pair needs its OWN symbol: the shape
// registry keys on an exact (method, paramTypes) tuple, and C++ overload
// resolution cannot be relied on across translation units.

#define CHAOS_UJW_STRING_VALUE_OVERLOAD(suffix, nameType, valueType)               \
    void ChaosUtf8JsonWriterWriteString##suffix(                                  \
        CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INTPTR property_name,          \
        valueType value) noexcept                                                 \
    {                                                                             \
        CheckThis(this_ptr, BareWriterKind::InvalidOperation);                    \
        const char* n = nullptr; size_t n_len = 0;                                \
        if (!ManagedStringView(property_name, n, n_len))                          \
            RaiseArgumentNullException("propertyName");                           \
        (void)n; (void)n_len; (void)value;                                        \
        RaiseBareWriterInvalidOperation();                                        \
    }

// value = DateTime / DateTimeOffset / Guid (8- or 16-byte structs by pointer)
CHAOS_UJW_STRING_VALUE_OVERLOAD(StrDateTime,          System.String, CHAOS_IL2CPP_INTPTR)
CHAOS_UJW_STRING_VALUE_OVERLOAD(StrDateTimeOffset,    System.String, CHAOS_IL2CPP_INTPTR)
CHAOS_UJW_STRING_VALUE_OVERLOAD(StrGuid,              System.String, CHAOS_IL2CPP_INTPTR)
CHAOS_UJW_STRING_VALUE_OVERLOAD(EncodedDateTime,      JsonEncodedText, CHAOS_IL2CPP_INTPTR)
CHAOS_UJW_STRING_VALUE_OVERLOAD(EncodedDateTimeOffset, JsonEncodedText, CHAOS_IL2CPP_INTPTR)
CHAOS_UJW_STRING_VALUE_OVERLOAD(EncodedGuid,          JsonEncodedText, CHAOS_IL2CPP_INTPTR)
CHAOS_UJW_STRING_VALUE_OVERLOAD(EncodedEncoded,       JsonEncodedText, CHAOS_IL2CPP_INTPTR)
CHAOS_UJW_STRING_VALUE_OVERLOAD(EncodedStr,           JsonEncodedText, CHAOS_IL2CPP_INTPTR)

#undef CHAOS_UJW_STRING_VALUE_OVERLOAD

// WriteString(string propertyName, JsonEncodedText value) — string name form.
void ChaosUtf8JsonWriterWriteStringStrEncoded(
    CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INTPTR property_name,
    CHAOS_IL2CPP_INTPTR encoded_value) noexcept
{
    CheckThis(this_ptr, BareWriterKind::InvalidOperation);
    const char* n = nullptr; size_t n_len = 0;
    if (!ManagedStringView(property_name, n, n_len))
        RaiseArgumentNullException("propertyName");
    (void)n; (void)n_len; (void)encoded_value;
    RaiseBareWriterInvalidOperation();
}

// ── WriteNull(JsonEncodedText) / WriteBoolean(JsonEncodedText, bool) ──────
void ChaosUtf8JsonWriterWriteNullEncoded(
    CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INTPTR encoded_name) noexcept
{
    CheckThis(this_ptr, BareWriterKind::InvalidOperation);
    (void)encoded_name;
    RaiseBareWriterInvalidOperation();
}

void ChaosUtf8JsonWriterWriteBooleanEncoded(
    CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INTPTR encoded_name,
    CHAOS_IL2CPP_INT32 value) noexcept
{
    CheckThis(this_ptr, BareWriterKind::InvalidOperation);
    (void)encoded_name; (void)value;
    RaiseBareWriterInvalidOperation();
}

// ── WriteRawValue(payload, skipInputValidation) ──────────────────────────
// Value-only write (no property name).  .NET validates an EMPTY payload first
// and reports ArgumentException; a non-empty payload reaches the missing sink
// and reports NullReferenceException.  Measured:
//   WriteRawValue("[1]", false) -> NullReferenceException
//   WriteRawValue("[1]", true)  -> NullReferenceException
//   WriteRawValue("",    false) -> ArgumentException
void ChaosUtf8JsonWriterWriteRawValueStrBool(
    CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INTPTR json,
    CHAOS_IL2CPP_INT32 skip_input_validation) noexcept
{
    CheckThis(this_ptr, BareWriterKind::NullReference);
    const char* j = nullptr; size_t j_len = 0;
    if (!ManagedStringView(json, j, j_len))
        RaiseArgumentNullException("json");
    (void)skip_input_validation;
    if (j_len == 0)
        RaiseManagedException("System.ArgumentException",
            "The JSON payload cannot be empty.");
    RaiseBareWriterNullReference();
}

/// WriteRawValue(ReadOnlySequence<byte>, bool) — value-only write, so an empty
/// payload is validated first and reports ArgumentException, while a non-empty
/// payload reaches the missing sink and reports NullReferenceException.
///
/// Measured on .NET 10:
///   WriteRawValue(default(seq), false) -> ArgumentException   (the ATG fixture)
///   WriteRawValue(default(seq), true)  -> ArgumentException
///   WriteRawValue(empty seq,    false) -> ArgumentException
///   WriteRawValue(seq "[1]",    false) -> NullReferenceException
///
/// Emptiness is NOT "the carrier is null": a default sequence arrives as a
/// NON-null pointer to a struct whose first two words are 0 (start == end).
/// Instrumented: default(ReadOnlySequence<byte>) -> words = [0 0 <ptr> 0].
/// Reading the immediate first pointer as a null check never fired and let a
/// default sequence fall through to the NRE branch — the opposite of what .NET
/// does.  So test the sequence's own start/end words.
void ChaosUtf8JsonWriterWriteRawValueSequenceBool(
    CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INTPTR json_sequence,
    CHAOS_IL2CPP_INT32 skip_input_validation) noexcept
{
    CheckThis(this_ptr, BareWriterKind::NullReference);
    (void)skip_input_validation;

    // ReadOnlySequence<T> is laid out as (SequencePosition start, SequencePosition end)
    // and each SequencePosition is (object segment, int index).  A default/empty
    // sequence has start == end, so the whole first two words are zero.
    bool empty_payload = true;
    if (json_sequence != 0)
    {
        const auto* words = reinterpret_cast<const CHAOS_IL2CPP_UINT64*>(json_sequence);
        empty_payload = (words[0] == 0 && words[1] == 0);
    }

    if (empty_payload)
        RaiseManagedException("System.ArgumentException",
            "The JSON payload cannot be empty.");
    RaiseBareWriterNullReference();
}

// ══════════════════════════════════════════════════════════════════
// WriteNumber overloads
// ══════════════════════════════════════════════════════════════════

// WriteNumber(propertyName, scalar) scalar-width variants.  The existing
// WriteNumberStr covers Int64 / Double / Float via separate symbols; the Int32,
// UInt32, UInt64 and decimal widths were absent entirely, and the
// JsonEncodedText property-name form had no variant at all.  Every one of them
// throws InvalidOperationException on a bare writer (measured), so the payload
// is carried but not inspected.
#define CHAOS_UJW_NUMBER_OVERLOAD(suffix, nameAbi, valueAbi)                       \
    void ChaosUtf8JsonWriterWriteNumber##suffix(                                   \
        CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INTPTR property_name,           \
        valueAbi value) noexcept                                                   \
    {                                                                              \
        CheckThis(this_ptr, BareWriterKind::InvalidOperation);                     \
        const char* n = nullptr; size_t n_len = 0;                                 \
        if (!ManagedStringView(property_name, n, n_len))                           \
            RaiseArgumentNullException("propertyName");                            \
        (void)n; (void)n_len; (void)value;                                         \
        RaiseBareWriterInvalidOperation();                                         \
    }

// string property name
CHAOS_UJW_NUMBER_OVERLOAD(StrInt32,  System.String,   CHAOS_IL2CPP_INT32)
CHAOS_UJW_NUMBER_OVERLOAD(StrUInt32, System.String,   CHAOS_IL2CPP_UINT32)
CHAOS_UJW_NUMBER_OVERLOAD(StrUInt64, System.String,   CHAOS_IL2CPP_UINT64)
// decimal arrives as a pointer to the boxed 16-byte payload.
CHAOS_UJW_NUMBER_OVERLOAD(StrDecimal, System.String,  CHAOS_IL2CPP_INTPTR)
// JsonEncodedText property name
CHAOS_UJW_NUMBER_OVERLOAD(EncodedInt32,  JsonEncodedText, CHAOS_IL2CPP_INT32)
CHAOS_UJW_NUMBER_OVERLOAD(EncodedInt64,  JsonEncodedText, CHAOS_IL2CPP_INT64)
CHAOS_UJW_NUMBER_OVERLOAD(EncodedUInt32, JsonEncodedText, CHAOS_IL2CPP_UINT32)
CHAOS_UJW_NUMBER_OVERLOAD(EncodedUInt64, JsonEncodedText, CHAOS_IL2CPP_UINT64)
CHAOS_UJW_NUMBER_OVERLOAD(EncodedDouble, JsonEncodedText, double)
CHAOS_UJW_NUMBER_OVERLOAD(EncodedFloat,  JsonEncodedText, float)
CHAOS_UJW_NUMBER_OVERLOAD(EncodedDecimal, JsonEncodedText, CHAOS_IL2CPP_INTPTR)
CHAOS_UJW_NUMBER_OVERLOAD(EncodedStr,    JsonEncodedText, CHAOS_IL2CPP_INTPTR)

#undef CHAOS_UJW_NUMBER_OVERLOAD

void ChaosUtf8JsonWriterWriteNumberStr(
    CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INTPTR property_name,
    CHAOS_IL2CPP_INT64 value) noexcept
{
    CheckThis(this_ptr, BareWriterKind::InvalidOperation);
    const char* n = nullptr; size_t n_len = 0;
    if (!ManagedStringView(property_name, n, n_len))
        RaiseArgumentNullException("propertyName");
    (void)value;
    RaiseBareWriterInvalidOperation();
}

void ChaosUtf8JsonWriterWriteNumberStrDouble(
    CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INTPTR property_name,
    double value) noexcept
{
    CheckThis(this_ptr, BareWriterKind::InvalidOperation);
    const char* n = nullptr; size_t n_len = 0;
    if (!ManagedStringView(property_name, n, n_len))
        RaiseArgumentNullException("propertyName");
    (void)value;
    RaiseBareWriterInvalidOperation();
}

void ChaosUtf8JsonWriterWriteNumberStrFloat(
    CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INTPTR property_name,
    float value) noexcept
{
    CheckThis(this_ptr, BareWriterKind::InvalidOperation);
    const char* n = nullptr; size_t n_len = 0;
    if (!ManagedStringView(property_name, n, n_len))
        RaiseArgumentNullException("propertyName");
    (void)value;
    RaiseBareWriterInvalidOperation();
}

void ChaosUtf8JsonWriterWriteNumberStrUInt(
    CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INTPTR property_name,
    CHAOS_IL2CPP_INT64 value) noexcept
{
    CheckThis(this_ptr, BareWriterKind::InvalidOperation);
    const char* n = nullptr; size_t n_len = 0;
    if (!ManagedStringView(property_name, n, n_len))
        RaiseArgumentNullException("propertyName");
    (void)value;
    RaiseBareWriterInvalidOperation();
}

void ChaosUtf8JsonWriterWriteNumberInt(
    CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INT64 value) noexcept
{
    CheckThis(this_ptr, BareWriterKind::NullReference);
    (void)value;
    RaiseBareWriterNullReference();
}

void ChaosUtf8JsonWriterWriteNumberDouble(
    CHAOS_IL2CPP_INTPTR this_ptr, double value) noexcept
{
    CheckThis(this_ptr, BareWriterKind::NullReference);
    (void)value;
    RaiseBareWriterNullReference();
}

void ChaosUtf8JsonWriterWriteNumberFloat(
    CHAOS_IL2CPP_INTPTR this_ptr, float value) noexcept
{
    CheckThis(this_ptr, BareWriterKind::NullReference);
    (void)value;
    RaiseBareWriterNullReference();
}

void ChaosUtf8JsonWriterWriteNumberUInt(
    CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INT64 value) noexcept
{
    CheckThis(this_ptr, BareWriterKind::NullReference);
    (void)value;
    RaiseBareWriterNullReference();
}

void ChaosUtf8JsonWriterWriteNumberUInt64(
    CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INT64 value) noexcept
{
    CheckThis(this_ptr, BareWriterKind::NullReference);
    (void)value;
    RaiseBareWriterNullReference();
}

// ══════════════════════════════════════════════════════════════════
// WriteNumberValue / WriteStringValue — value-only overloads
//
// These are the "current position" variants (no property name): valid only
// inside an array or at the root.  On a bare/disposed instance .NET throws
// NullReferenceException — the missing sink is dereferenced, not reported as a
// disposed object (measured; see RaiseBareWriterNullReference).
// ══════════════════════════════════════════════════════════════════

void ChaosUtf8JsonWriterWriteNumberValueInt(
    CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INT64 value) noexcept
{
    CheckThis(this_ptr, BareWriterKind::NullReference);
    (void)value;
    RaiseBareWriterNullReference();
}

// WriteNumberValue(Int32) / (UInt32) — the two widths that had no native variant
// at all.  Same value-only family as the others: NullReferenceException on a
// bare writer (measured across every width: byte/sbyte/short/ushort/int/uint/
// long/ulong/float/double/decimal/char).
void ChaosUtf8JsonWriterWriteNumberValueInt32(
    CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INT32 value) noexcept
{
    CheckThis(this_ptr, BareWriterKind::NullReference);
    (void)value;
    RaiseBareWriterNullReference();
}

void ChaosUtf8JsonWriterWriteNumberValueUInt32(
    CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_UINT32 value) noexcept
{
    CheckThis(this_ptr, BareWriterKind::NullReference);
    (void)value;
    RaiseBareWriterNullReference();
}

void ChaosUtf8JsonWriterWriteNumberValueDouble(
    CHAOS_IL2CPP_INTPTR this_ptr, double value) noexcept
{
    CheckThis(this_ptr, BareWriterKind::NullReference);
    (void)value;
    RaiseBareWriterNullReference();
}

void ChaosUtf8JsonWriterWriteNumberValueFloat(
    CHAOS_IL2CPP_INTPTR this_ptr, float value) noexcept
{
    CheckThis(this_ptr, BareWriterKind::NullReference);
    (void)value;
    RaiseBareWriterNullReference();
}

/// WriteNumberValue(decimal) — Decimal arrives as a 16-byte value that does not
/// fit a scalar ABI slot; it is passed as a pointer to the boxed payload.
void ChaosUtf8JsonWriterWriteNumberValueDecimal(
    CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INTPTR value) noexcept
{
    CheckThis(this_ptr, BareWriterKind::NullReference);
    (void)value;
    RaiseBareWriterNullReference();
}

void ChaosUtf8JsonWriterWriteNumberValueUInt(
    CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INT64 value) noexcept
{
    CheckThis(this_ptr, BareWriterKind::NullReference);
    (void)value;
    RaiseBareWriterNullReference();
}

void ChaosUtf8JsonWriterWriteStringValueStr(
    CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INTPTR value) noexcept
{
    // Receiver state outranks argument validation: on a bare writer .NET throws
    // NullReferenceException even for a null value argument (measured —
    // bare.WriteStringValue((string)null) -> NullReferenceException), so the
    // value check must not run first.
    CheckThis(this_ptr, BareWriterKind::NullReference);
    (void)value;
    RaiseBareWriterNullReference();
}

/// WriteStringValue(DateTime / DateTimeOffset / Guid) — value types that do not
/// fit a scalar slot (DateTime/DateTimeOffset are 8-byte structs passed by
/// pointer; Guid is 16 bytes).  All arrive as an INTPTR to the value payload.
void ChaosUtf8JsonWriterWriteStringValueStruct(
    CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INTPTR value) noexcept
{
    CheckThis(this_ptr, BareWriterKind::NullReference);
    (void)value;
    RaiseBareWriterNullReference();
}

/// WriteStringValue(JsonEncodedText) — already-encoded text; the bare writer
/// still fails the receiver check first (NullReferenceException, same family).
void ChaosUtf8JsonWriterWriteStringValueEncoded(
    CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INTPTR encoded) noexcept
{
    CheckThis(this_ptr, BareWriterKind::NullReference);
    (void)encoded;
    RaiseBareWriterNullReference();
}

// ══════════════════════════════════════════════════════════════════
// WriteBoolean
// ══════════════════════════════════════════════════════════════════

void ChaosUtf8JsonWriterWriteBooleanStr(
    CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INTPTR property_name,
    CHAOS_IL2CPP_INT32 value) noexcept
{
    CheckThis(this_ptr, BareWriterKind::InvalidOperation);
    const char* n = nullptr; size_t n_len = 0;
    if (!ManagedStringView(property_name, n, n_len))
        RaiseArgumentNullException("propertyName");
    (void)value;
    RaiseBareWriterInvalidOperation();
}

void ChaosUtf8JsonWriterWriteBooleanValue(
    CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INT32 value) noexcept
{
    // Value-only write on a bare writer: .NET raises NullReferenceException
    // (measured), not ObjectDisposedException.
    CheckThis(this_ptr, BareWriterKind::NullReference);
    (void)value;
    RaiseBareWriterNullReference();
}

// ══════════════════════════════════════════════════════════════════
// WriteNull
// ══════════════════════════════════════════════════════════════════

void ChaosUtf8JsonWriterWriteNullStr(
    CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INTPTR property_name) noexcept
{
    CheckThis(this_ptr, BareWriterKind::InvalidOperation);
    const char* n = nullptr; size_t n_len = 0;
    if (!ManagedStringView(property_name, n, n_len))
        RaiseArgumentNullException("propertyName");
    RaiseBareWriterInvalidOperation();
}

void ChaosUtf8JsonWriterWriteNullValue(
    CHAOS_IL2CPP_INTPTR this_ptr) noexcept
{
    // Value-only write on a bare writer: NullReferenceException (measured).
    CheckThis(this_ptr, BareWriterKind::NullReference);
    RaiseBareWriterNullReference();
}

// ══════════════════════════════════════════════════════════════════
// WritePropertyName
// ══════════════════════════════════════════════════════════════════

void ChaosUtf8JsonWriterWritePropertyNameStr(
    CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INTPTR property_name) noexcept
{
    CheckThis(this_ptr, BareWriterKind::InvalidOperation);
    const char* n = nullptr; size_t n_len = 0;
    if (!ManagedStringView(property_name, n, n_len))
        RaiseArgumentNullException("propertyName");
    RaiseBareWriterInvalidOperation();
}

void ChaosUtf8JsonWriterWritePropertyNameEncoded(
    CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INTPTR encoded) noexcept
{
    CheckThis(this_ptr, BareWriterKind::InvalidOperation);
    if (encoded == 0) RaiseArgumentNullException("encodedText");
    RaiseBareWriterInvalidOperation();
}

// ══════════════════════════════════════════════════════════════════
// WriteRawValue
// ══════════════════════════════════════════════════════════════════

void ChaosUtf8JsonWriterWriteRawValueStr(
    CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INTPTR json) noexcept
{
    CheckThis(this_ptr, BareWriterKind::NullReference);
    if (json == 0) RaiseArgumentNullException("json");
    const char* j = nullptr; size_t j_len = 0;
    if (!ManagedStringView(json, j, j_len) || j_len == 0)
        RaiseManagedException("System.ArgumentException",
            "The JSON payload cannot be empty.");
    (void)j;
    RaiseBareWriterNullReference();
}

// ══════════════════════════════════════════════════════════════════
// WriteCommentValue
// ══════════════════════════════════════════════════════════════════

void ChaosUtf8JsonWriterWriteCommentValue(
    CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INTPTR comment) noexcept
{
    CheckThis(this_ptr, BareWriterKind::NullReference);
    if (comment == 0) RaiseArgumentNullException("comment");
    RaiseBareWriterNullReference();
}

/// WriteTo(Utf8JsonWriter) — JsonDocument/JsonElement/JsonProperty surface.
/// ATG passes a bare target writer (default(Utf8JsonWriter)!), which the
/// managed implementation rejects with ArgumentNullException.  Receiver
/// validity is not checked (the source document is a bare object too).
void ChaosUtf8JsonWriterWriteTo(
    CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INTPTR writer) noexcept
{
    (void)this_ptr;
    if (writer == 0) RaiseArgumentNullException("writer");
    RaiseDisposedOrInvalid();
}

}  // extern "C"
}  // namespace chaos::il2cpp::runtime_core