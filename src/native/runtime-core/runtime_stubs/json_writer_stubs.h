// json_writer_stubs.h — native AOT declarations for
// System.Text.Json.Utf8JsonWriter's write surface.
#pragma once

#include <chaos/native_types.h>

extern "C" {

// ── Lifecycle ──
void ChaosUtf8JsonWriterFlush(CHAOS_IL2CPP_INTPTR this_ptr) noexcept;
void ChaosUtf8JsonWriterDispose(CHAOS_IL2CPP_INTPTR this_ptr) noexcept;
CHAOS_IL2CPP_INTPTR ChaosUtf8JsonWriterResetStream(CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INTPTR stream) noexcept;
CHAOS_IL2CPP_INTPTR ChaosUtf8JsonWriterResetBufferWriter(CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INTPTR buffer_writer) noexcept;

// ── Structure ──
void ChaosUtf8JsonWriterWriteStartObject(CHAOS_IL2CPP_INTPTR this_ptr) noexcept;
void ChaosUtf8JsonWriterWriteStartArray(CHAOS_IL2CPP_INTPTR this_ptr) noexcept;
void ChaosUtf8JsonWriterWriteEndObject(CHAOS_IL2CPP_INTPTR this_ptr) noexcept;
void ChaosUtf8JsonWriterWriteEndArray(CHAOS_IL2CPP_INTPTR this_ptr) noexcept;

// ── WriteString ──
void ChaosUtf8JsonWriterWriteStringStr(CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INTPTR property_name, CHAOS_IL2CPP_INTPTR value) noexcept;
void ChaosUtf8JsonWriterWriteStringStrOnly(CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INTPTR value) noexcept;
void ChaosUtf8JsonWriterWriteStringEncodedText(CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INTPTR property_name, CHAOS_IL2CPP_INTPTR encoded_text) noexcept;

// ── WriteNumber ──
void ChaosUtf8JsonWriterWriteNumberStr(CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INTPTR property_name, CHAOS_IL2CPP_INT64 value) noexcept;
void ChaosUtf8JsonWriterWriteNumberStrDouble(CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INTPTR property_name, double value) noexcept;
void ChaosUtf8JsonWriterWriteNumberStrFloat(CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INTPTR property_name, float value) noexcept;
void ChaosUtf8JsonWriterWriteNumberStrUInt(CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INTPTR property_name, CHAOS_IL2CPP_INT64 value) noexcept;
void ChaosUtf8JsonWriterWriteNumberInt(CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INT64 value) noexcept;
void ChaosUtf8JsonWriterWriteNumberDouble(CHAOS_IL2CPP_INTPTR this_ptr, double value) noexcept;
void ChaosUtf8JsonWriterWriteNumberFloat(CHAOS_IL2CPP_INTPTR this_ptr, float value) noexcept;
void ChaosUtf8JsonWriterWriteNumberUInt(CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INT64 value) noexcept;
void ChaosUtf8JsonWriterWriteNumberUInt64(CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INT64 value) noexcept;

// ── WriteBoolean ──
void ChaosUtf8JsonWriterWriteBooleanStr(CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INTPTR property_name, CHAOS_IL2CPP_INT32 value) noexcept;
void ChaosUtf8JsonWriterWriteBooleanValue(CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INT32 value) noexcept;

// ── WriteNull ──
void ChaosUtf8JsonWriterWriteNullStr(CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INTPTR property_name) noexcept;
void ChaosUtf8JsonWriterWriteNullValue(CHAOS_IL2CPP_INTPTR this_ptr) noexcept;

// ── WritePropertyName ──
void ChaosUtf8JsonWriterWritePropertyNameStr(CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INTPTR property_name) noexcept;
void ChaosUtf8JsonWriterWritePropertyNameEncoded(CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INTPTR encoded) noexcept;

// ── WriteRawValue / WriteCommentValue ──
void ChaosUtf8JsonWriterWriteRawValueStr(CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INTPTR json) noexcept;
void ChaosUtf8JsonWriterWriteCommentValue(CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INTPTR comment) noexcept;

}  // extern "C"