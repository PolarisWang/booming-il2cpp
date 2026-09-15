// xml_convert_stubs.h — native AOT declarations for System.Xml.XmlConvert's
// scalar conversion and XSD-name-validation surface.
//
// Return-slot convention: XmlConvert.ToX for a value type returns the value in
// the appropriate scalar slot (ToInt32 → INT32, ToInt64/ToDateTime/ToTimeSpan →
// INT64, ToDouble/ToSingle → float).  Types that do not fit a scalar slot
// (Decimal, Guid) are declared as INTPTR and currently raise
// NotSupportedException rather than fabricating a mismatched box.
//
// The Verify* predicates are void: they raise XmlException on invalid input and
// return normally when the input satisfies the corresponding XML production.
#pragma once

#include <chaos/native_types.h>

extern "C" {

// ── XSD name / content validation ──
void ChaosXmlConvertVerifyName(CHAOS_IL2CPP_INTPTR name) noexcept;
void ChaosXmlConvertVerifyNCName(CHAOS_IL2CPP_INTPTR name) noexcept;
void ChaosXmlConvertVerifyNMTOKEN(CHAOS_IL2CPP_INTPTR name) noexcept;
void ChaosXmlConvertVerifyPublicId(CHAOS_IL2CPP_INTPTR id) noexcept;
void ChaosXmlConvertVerifyWhitespace(CHAOS_IL2CPP_INTPTR text) noexcept;
void ChaosXmlConvertVerifyXmlChars(CHAOS_IL2CPP_INTPTR text) noexcept;

// ── Scalar conversions ──
CHAOS_IL2CPP_INT32 ChaosXmlConvertToBoolean(CHAOS_IL2CPP_INTPTR str) noexcept;
CHAOS_IL2CPP_INT32 ChaosXmlConvertToByte(CHAOS_IL2CPP_INTPTR str) noexcept;
CHAOS_IL2CPP_INT32 ChaosXmlConvertToSByte(CHAOS_IL2CPP_INTPTR str) noexcept;
CHAOS_IL2CPP_INT32 ChaosXmlConvertToInt16(CHAOS_IL2CPP_INTPTR str) noexcept;
CHAOS_IL2CPP_INT32 ChaosXmlConvertToUInt16(CHAOS_IL2CPP_INTPTR str) noexcept;
CHAOS_IL2CPP_INT32 ChaosXmlConvertToInt32(CHAOS_IL2CPP_INTPTR str) noexcept;
CHAOS_IL2CPP_INT32 ChaosXmlConvertToUInt32(CHAOS_IL2CPP_INTPTR str) noexcept;
CHAOS_IL2CPP_INT64 ChaosXmlConvertToInt64(CHAOS_IL2CPP_INTPTR str) noexcept;
CHAOS_IL2CPP_INT64 ChaosXmlConvertToUInt64(CHAOS_IL2CPP_INTPTR str) noexcept;
float ChaosXmlConvertToSingle(CHAOS_IL2CPP_INTPTR str) noexcept;
double ChaosXmlConvertToDouble(CHAOS_IL2CPP_INTPTR str) noexcept;
CHAOS_IL2CPP_INT32 ChaosXmlConvertToChar(CHAOS_IL2CPP_INTPTR str) noexcept;
CHAOS_IL2CPP_INT64 ChaosXmlConvertToTimeSpan(CHAOS_IL2CPP_INTPTR str) noexcept;
CHAOS_IL2CPP_INT64 ChaosXmlConvertToDateTime(CHAOS_IL2CPP_INTPTR str) noexcept;
CHAOS_IL2CPP_INT64 ChaosXmlConvertToDateTimeOffset(CHAOS_IL2CPP_INTPTR str) noexcept;

/// Decimal does not fit a scalar return slot — currently raises
/// NotSupportedException (see the .cpp for why a fabricated box would be wrong).
CHAOS_IL2CPP_INTPTR ChaosXmlConvertToDecimal(CHAOS_IL2CPP_INTPTR str) noexcept;

/// Guid likewise needs a 16-byte box; currently raises NotSupportedException.
CHAOS_IL2CPP_INTPTR ChaosXmlConvertToGuid(CHAOS_IL2CPP_INTPTR str) noexcept;

}  // extern "C"
