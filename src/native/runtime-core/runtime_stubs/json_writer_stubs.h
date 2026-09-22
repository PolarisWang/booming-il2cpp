// json_writer_stubs.h — native AOT declarations for
// System.Text.Json.Utf8JsonWriter's write surface.
#pragma once

#include <chaos/eh.h>  // CHAOS_STUB_NOEXCEPT
#include <chaos/native_types.h>

extern "C" {

// ── Lifecycle ──
void ChaosUtf8JsonWriterFlush(CHAOS_IL2CPP_INTPTR this_ptr) CHAOS_STUB_NOEXCEPT;
void ChaosUtf8JsonWriterDispose(CHAOS_IL2CPP_INTPTR this_ptr) CHAOS_STUB_NOEXCEPT;
CHAOS_IL2CPP_INTPTR ChaosUtf8JsonWriterResetStream(CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INTPTR stream) CHAOS_STUB_NOEXCEPT;
CHAOS_IL2CPP_INTPTR ChaosUtf8JsonWriterResetBufferWriter(CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INTPTR buffer_writer) CHAOS_STUB_NOEXCEPT;

// ── Structure ──
void ChaosUtf8JsonWriterWriteStartObject(CHAOS_IL2CPP_INTPTR this_ptr) CHAOS_STUB_NOEXCEPT;
void ChaosUtf8JsonWriterWriteStartArray(CHAOS_IL2CPP_INTPTR this_ptr) CHAOS_STUB_NOEXCEPT;
void ChaosUtf8JsonWriterWriteEndObject(CHAOS_IL2CPP_INTPTR this_ptr) CHAOS_STUB_NOEXCEPT;
void ChaosUtf8JsonWriterWriteEndArray(CHAOS_IL2CPP_INTPTR this_ptr) CHAOS_STUB_NOEXCEPT;

// Property-name forms.  Each parameter list gets its own symbol — the shape
// registry keys on an exact (method, paramTypes) tuple, and same-name C++
// overloads across translation units cannot be relied on to resolve.
void ChaosUtf8JsonWriterWriteStartObjectStr(CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INTPTR property_name) CHAOS_STUB_NOEXCEPT;
void ChaosUtf8JsonWriterWriteStartObjectEncoded(CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INTPTR encoded_name) CHAOS_STUB_NOEXCEPT;
void ChaosUtf8JsonWriterWriteStartArrayStr(CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INTPTR property_name) CHAOS_STUB_NOEXCEPT;
void ChaosUtf8JsonWriterWriteStartArrayEncoded(CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INTPTR encoded_name) CHAOS_STUB_NOEXCEPT;

// ── WriteString ──
void ChaosUtf8JsonWriterWriteStringStr(CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INTPTR property_name, CHAOS_IL2CPP_INTPTR value) CHAOS_STUB_NOEXCEPT;
void ChaosUtf8JsonWriterWriteStringStrOnly(CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INTPTR value) CHAOS_STUB_NOEXCEPT;
void ChaosUtf8JsonWriterWriteStringEncodedText(CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INTPTR property_name, CHAOS_IL2CPP_INTPTR encoded_text) CHAOS_STUB_NOEXCEPT;

// WriteString(propertyName, non-string value).  Struct values (DateTime /
// DateTimeOffset / Guid), JsonEncodedText and string values all arrive as
// INTPTR carriers.
void ChaosUtf8JsonWriterWriteStringStrDateTime(CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INTPTR property_name, CHAOS_IL2CPP_INTPTR value) CHAOS_STUB_NOEXCEPT;
void ChaosUtf8JsonWriterWriteStringStrDateTimeOffset(CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INTPTR property_name, CHAOS_IL2CPP_INTPTR value) CHAOS_STUB_NOEXCEPT;
void ChaosUtf8JsonWriterWriteStringStrGuid(CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INTPTR property_name, CHAOS_IL2CPP_INTPTR value) CHAOS_STUB_NOEXCEPT;
void ChaosUtf8JsonWriterWriteStringStrEncoded(CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INTPTR property_name, CHAOS_IL2CPP_INTPTR encoded_value) CHAOS_STUB_NOEXCEPT;
void ChaosUtf8JsonWriterWriteStringEncodedDateTime(CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INTPTR property_name, CHAOS_IL2CPP_INTPTR value) CHAOS_STUB_NOEXCEPT;
void ChaosUtf8JsonWriterWriteStringEncodedDateTimeOffset(CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INTPTR property_name, CHAOS_IL2CPP_INTPTR value) CHAOS_STUB_NOEXCEPT;
void ChaosUtf8JsonWriterWriteStringEncodedGuid(CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INTPTR property_name, CHAOS_IL2CPP_INTPTR value) CHAOS_STUB_NOEXCEPT;
void ChaosUtf8JsonWriterWriteStringEncodedEncoded(CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INTPTR property_name, CHAOS_IL2CPP_INTPTR value) CHAOS_STUB_NOEXCEPT;
void ChaosUtf8JsonWriterWriteStringEncodedStr(CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INTPTR property_name, CHAOS_IL2CPP_INTPTR value) CHAOS_STUB_NOEXCEPT;

// ── WriteNumber ──
void ChaosUtf8JsonWriterWriteNumberStr(CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INTPTR property_name, CHAOS_IL2CPP_INT64 value) CHAOS_STUB_NOEXCEPT;
void ChaosUtf8JsonWriterWriteNumberStrDouble(CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INTPTR property_name, double value) CHAOS_STUB_NOEXCEPT;
void ChaosUtf8JsonWriterWriteNumberStrFloat(CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INTPTR property_name, float value) CHAOS_STUB_NOEXCEPT;
void ChaosUtf8JsonWriterWriteNumberStrUInt(CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INTPTR property_name, CHAOS_IL2CPP_INT64 value) CHAOS_STUB_NOEXCEPT;
void ChaosUtf8JsonWriterWriteNumberStrInt32(CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INTPTR property_name, CHAOS_IL2CPP_INT32 value) CHAOS_STUB_NOEXCEPT;
void ChaosUtf8JsonWriterWriteNumberStrUInt32(CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INTPTR property_name, CHAOS_IL2CPP_UINT32 value) CHAOS_STUB_NOEXCEPT;
void ChaosUtf8JsonWriterWriteNumberStrUInt64(CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INTPTR property_name, CHAOS_IL2CPP_UINT64 value) CHAOS_STUB_NOEXCEPT;
void ChaosUtf8JsonWriterWriteNumberStrDecimal(CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INTPTR property_name, CHAOS_IL2CPP_INTPTR value) CHAOS_STUB_NOEXCEPT;
void ChaosUtf8JsonWriterWriteNumberEncodedInt32(CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INTPTR property_name, CHAOS_IL2CPP_INT32 value) CHAOS_STUB_NOEXCEPT;
void ChaosUtf8JsonWriterWriteNumberEncodedInt64(CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INTPTR property_name, CHAOS_IL2CPP_INT64 value) CHAOS_STUB_NOEXCEPT;
void ChaosUtf8JsonWriterWriteNumberEncodedUInt32(CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INTPTR property_name, CHAOS_IL2CPP_UINT32 value) CHAOS_STUB_NOEXCEPT;
void ChaosUtf8JsonWriterWriteNumberEncodedUInt64(CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INTPTR property_name, CHAOS_IL2CPP_UINT64 value) CHAOS_STUB_NOEXCEPT;
void ChaosUtf8JsonWriterWriteNumberEncodedDouble(CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INTPTR property_name, double value) CHAOS_STUB_NOEXCEPT;
void ChaosUtf8JsonWriterWriteNumberEncodedFloat(CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INTPTR property_name, float value) CHAOS_STUB_NOEXCEPT;
void ChaosUtf8JsonWriterWriteNumberEncodedDecimal(CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INTPTR property_name, CHAOS_IL2CPP_INTPTR value) CHAOS_STUB_NOEXCEPT;
void ChaosUtf8JsonWriterWriteNumberEncodedStr(CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INTPTR property_name, CHAOS_IL2CPP_INTPTR value) CHAOS_STUB_NOEXCEPT;
void ChaosUtf8JsonWriterWriteNumberInt(CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INT64 value) CHAOS_STUB_NOEXCEPT;
void ChaosUtf8JsonWriterWriteNumberDouble(CHAOS_IL2CPP_INTPTR this_ptr, double value) CHAOS_STUB_NOEXCEPT;
void ChaosUtf8JsonWriterWriteNumberFloat(CHAOS_IL2CPP_INTPTR this_ptr, float value) CHAOS_STUB_NOEXCEPT;
void ChaosUtf8JsonWriterWriteNumberUInt(CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INT64 value) CHAOS_STUB_NOEXCEPT;
void ChaosUtf8JsonWriterWriteNumberUInt64(CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INT64 value) CHAOS_STUB_NOEXCEPT;

// ── WriteNumberValue (value-only overloads) ──
void ChaosUtf8JsonWriterWriteNumberValueInt(CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INT64 value) CHAOS_STUB_NOEXCEPT;
void ChaosUtf8JsonWriterWriteNumberValueInt32(CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INT32 value) CHAOS_STUB_NOEXCEPT;
void ChaosUtf8JsonWriterWriteNumberValueUInt32(CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_UINT32 value) CHAOS_STUB_NOEXCEPT;
void ChaosUtf8JsonWriterWriteNumberValueDouble(CHAOS_IL2CPP_INTPTR this_ptr, double value) CHAOS_STUB_NOEXCEPT;
void ChaosUtf8JsonWriterWriteNumberValueFloat(CHAOS_IL2CPP_INTPTR this_ptr, float value) CHAOS_STUB_NOEXCEPT;
void ChaosUtf8JsonWriterWriteNumberValueDecimal(CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INTPTR value) CHAOS_STUB_NOEXCEPT;
void ChaosUtf8JsonWriterWriteNumberValueUInt(CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INT64 value) CHAOS_STUB_NOEXCEPT;

// ── WriteStringValue (value-only overloads) ──
void ChaosUtf8JsonWriterWriteStringValueStr(CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INTPTR value) CHAOS_STUB_NOEXCEPT;
void ChaosUtf8JsonWriterWriteStringValueStruct(CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INTPTR value) CHAOS_STUB_NOEXCEPT;
void ChaosUtf8JsonWriterWriteStringValueEncoded(CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INTPTR encoded) CHAOS_STUB_NOEXCEPT;

// ── WriteBoolean ──
void ChaosUtf8JsonWriterWriteBooleanStr(CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INTPTR property_name, CHAOS_IL2CPP_INT32 value) CHAOS_STUB_NOEXCEPT;
void ChaosUtf8JsonWriterWriteBooleanValue(CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INT32 value) CHAOS_STUB_NOEXCEPT;
void ChaosUtf8JsonWriterWriteBooleanEncoded(CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INTPTR encoded_name, CHAOS_IL2CPP_INT32 value) CHAOS_STUB_NOEXCEPT;

// ── WriteNull ──
void ChaosUtf8JsonWriterWriteNullStr(CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INTPTR property_name) CHAOS_STUB_NOEXCEPT;
void ChaosUtf8JsonWriterWriteNullValue(CHAOS_IL2CPP_INTPTR this_ptr) CHAOS_STUB_NOEXCEPT;
void ChaosUtf8JsonWriterWriteNullEncoded(CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INTPTR encoded_name) CHAOS_STUB_NOEXCEPT;

// ── WritePropertyName ──
void ChaosUtf8JsonWriterWritePropertyNameStr(CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INTPTR property_name) CHAOS_STUB_NOEXCEPT;
void ChaosUtf8JsonWriterWritePropertyNameEncoded(CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INTPTR encoded) CHAOS_STUB_NOEXCEPT;

// ── WriteRawValue / WriteCommentValue / WriteTo ──
void ChaosUtf8JsonWriterWriteRawValueStr(CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INTPTR json) CHAOS_STUB_NOEXCEPT;
void ChaosUtf8JsonWriterWriteRawValueStrBool(CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INTPTR json, CHAOS_IL2CPP_INT32 skip_input_validation) CHAOS_STUB_NOEXCEPT;
void ChaosUtf8JsonWriterWriteRawValueSequenceBool(CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INTPTR json_sequence, CHAOS_IL2CPP_INT32 skip_input_validation) CHAOS_STUB_NOEXCEPT;
void ChaosUtf8JsonWriterWriteCommentValue(CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INTPTR comment) CHAOS_STUB_NOEXCEPT;

/// WriteTo(Utf8JsonWriter) — JsonDocument/JsonElement/JsonProperty surface.
void ChaosUtf8JsonWriterWriteTo(CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INTPTR writer) CHAOS_STUB_NOEXCEPT;

}  // extern "C"