// Auto-generated managed benchmark harness
// Family: reader, Assembly: System.Formats.Asn1
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Text.Json;

class ManagedBenchmarkHarness
{
    static volatile int _g;  // volatile side-effect prevents JIT DCE

    struct MethodResult
    {
        public int MethodIndex { get; set; }
        public string MethodSubjectId { get; set; }
        public double ElapsedMilliseconds { get; set; }
        public int Iterations { get; set; }
        public bool IsBodyReal { get; set; }
        public bool IsException { get; set; }
    }



    static void Main()
    {
        var results = new List<MethodResult>();
            { // [0] System.Formats.Asn1/System.Formats.Asn1.AsnReader::Clone:System.Formats.Asn1.AsnReader() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 0,
                    MethodSubjectId = "System.Formats.Asn1/System.Formats.Asn1.AsnReader::Clone:System.Formats.Asn1.AsnReader()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [1] System.Formats.Asn1/System.Formats.Asn1.AsnReader::get_HasData:System.Boolean() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 1,
                    MethodSubjectId = "System.Formats.Asn1/System.Formats.Asn1.AsnReader::get_HasData:System.Boolean()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [2] System.Formats.Asn1/System.Formats.Asn1.AsnReader::get_RuleSet:System.Formats.Asn1.AsnEncodingRules() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 2,
                    MethodSubjectId = "System.Formats.Asn1/System.Formats.Asn1.AsnReader::get_RuleSet:System.Formats.Asn1.AsnEncodingRules()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [3] System.Formats.Asn1/System.Formats.Asn1.AsnReader::PeekContentBytes:System.ReadOnlyMemory{System.Byte}() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 3,
                    MethodSubjectId = "System.Formats.Asn1/System.Formats.Asn1.AsnReader::PeekContentBytes:System.ReadOnlyMemory{System.Byte}()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [4] System.Formats.Asn1/System.Formats.Asn1.AsnReader::PeekEncodedValue:System.ReadOnlyMemory{System.Byte}() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 4,
                    MethodSubjectId = "System.Formats.Asn1/System.Formats.Asn1.AsnReader::PeekEncodedValue:System.ReadOnlyMemory{System.Byte}()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [5] System.Formats.Asn1/System.Formats.Asn1.AsnReader::PeekTag:System.Formats.Asn1.Asn1Tag() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 5,
                    MethodSubjectId = "System.Formats.Asn1/System.Formats.Asn1.AsnReader::PeekTag:System.Formats.Asn1.Asn1Tag()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [6] System.Formats.Asn1/System.Formats.Asn1.AsnReader::ReadBitString:System.Byte{}(System.Int32&,System.Nullable{System.Formats.Asn1.Asn1Tag}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 6,
                    MethodSubjectId = "System.Formats.Asn1/System.Formats.Asn1.AsnReader::ReadBitString:System.Byte{}(System.Int32&,System.Nullable{System.Formats.Asn1.Asn1Tag})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [7] System.Formats.Asn1/System.Formats.Asn1.AsnReader::ReadBoolean:System.Boolean(System.Nullable{System.Formats.Asn1.Asn1Tag}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 7,
                    MethodSubjectId = "System.Formats.Asn1/System.Formats.Asn1.AsnReader::ReadBoolean:System.Boolean(System.Nullable{System.Formats.Asn1.Asn1Tag})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [8] System.Formats.Asn1/System.Formats.Asn1.AsnReader::ReadCharacterString:System.String(System.Formats.Asn1.UniversalTagNumber,System.Nullable{System.Formats.Asn1.Asn1Tag}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 8,
                    MethodSubjectId = "System.Formats.Asn1/System.Formats.Asn1.AsnReader::ReadCharacterString:System.String(System.Formats.Asn1.UniversalTagNumber,System.Nullable{System.Formats.Asn1.Asn1Tag})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [9] System.Formats.Asn1/System.Formats.Asn1.AsnReader::ReadEncodedValue:System.ReadOnlyMemory{System.Byte}() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 9,
                    MethodSubjectId = "System.Formats.Asn1/System.Formats.Asn1.AsnReader::ReadEncodedValue:System.ReadOnlyMemory{System.Byte}()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [10] System.Formats.Asn1/System.Formats.Asn1.AsnReader::ReadEnumeratedBytes:System.ReadOnlyMemory{System.Byte}(System.Nullable{System.Formats.Asn1.Asn1Tag}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 10,
                    MethodSubjectId = "System.Formats.Asn1/System.Formats.Asn1.AsnReader::ReadEnumeratedBytes:System.ReadOnlyMemory{System.Byte}(System.Nullable{System.Formats.Asn1.Asn1Tag})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [11] System.Formats.Asn1/System.Formats.Asn1.AsnReader::ReadEnumeratedValue{TEnum}:TEnum(System.Nullable{System.Formats.Asn1.Asn1Tag}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 11,
                    MethodSubjectId = "System.Formats.Asn1/System.Formats.Asn1.AsnReader::ReadEnumeratedValue{TEnum}:TEnum(System.Nullable{System.Formats.Asn1.Asn1Tag})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [12] System.Formats.Asn1/System.Formats.Asn1.AsnReader::ReadEnumeratedValue:System.Enum(System.Type,System.Nullable{System.Formats.Asn1.Asn1Tag}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 12,
                    MethodSubjectId = "System.Formats.Asn1/System.Formats.Asn1.AsnReader::ReadEnumeratedValue:System.Enum(System.Type,System.Nullable{System.Formats.Asn1.Asn1Tag})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [13] System.Formats.Asn1/System.Formats.Asn1.AsnReader::ReadGeneralizedTime:System.DateTimeOffset(System.Nullable{System.Formats.Asn1.Asn1Tag}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 13,
                    MethodSubjectId = "System.Formats.Asn1/System.Formats.Asn1.AsnReader::ReadGeneralizedTime:System.DateTimeOffset(System.Nullable{System.Formats.Asn1.Asn1Tag})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [14] System.Formats.Asn1/System.Formats.Asn1.AsnReader::ReadInteger:System.Numerics.BigInteger(System.Nullable{System.Formats.Asn1.Asn1Tag}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 14,
                    MethodSubjectId = "System.Formats.Asn1/System.Formats.Asn1.AsnReader::ReadInteger:System.Numerics.BigInteger(System.Nullable{System.Formats.Asn1.Asn1Tag})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [15] System.Formats.Asn1/System.Formats.Asn1.AsnReader::ReadIntegerBytes:System.ReadOnlyMemory{System.Byte}(System.Nullable{System.Formats.Asn1.Asn1Tag}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 15,
                    MethodSubjectId = "System.Formats.Asn1/System.Formats.Asn1.AsnReader::ReadIntegerBytes:System.ReadOnlyMemory{System.Byte}(System.Nullable{System.Formats.Asn1.Asn1Tag})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [16] System.Formats.Asn1/System.Formats.Asn1.AsnReader::ReadNamedBitList:System.Collections.BitArray(System.Nullable{System.Formats.Asn1.Asn1Tag}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 16,
                    MethodSubjectId = "System.Formats.Asn1/System.Formats.Asn1.AsnReader::ReadNamedBitList:System.Collections.BitArray(System.Nullable{System.Formats.Asn1.Asn1Tag})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [17] System.Formats.Asn1/System.Formats.Asn1.AsnReader::ReadNamedBitListValue{TFlagsEnum}:TFlagsEnum(System.Nullable{System.Formats.Asn1.Asn1Tag}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 17,
                    MethodSubjectId = "System.Formats.Asn1/System.Formats.Asn1.AsnReader::ReadNamedBitListValue{TFlagsEnum}:TFlagsEnum(System.Nullable{System.Formats.Asn1.Asn1Tag})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [18] System.Formats.Asn1/System.Formats.Asn1.AsnReader::ReadNamedBitListValue:System.Enum(System.Type,System.Nullable{System.Formats.Asn1.Asn1Tag}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 18,
                    MethodSubjectId = "System.Formats.Asn1/System.Formats.Asn1.AsnReader::ReadNamedBitListValue:System.Enum(System.Type,System.Nullable{System.Formats.Asn1.Asn1Tag})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [19] System.Formats.Asn1/System.Formats.Asn1.AsnReader::ReadNull:System.Void(System.Nullable{System.Formats.Asn1.Asn1Tag}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 19,
                    MethodSubjectId = "System.Formats.Asn1/System.Formats.Asn1.AsnReader::ReadNull:System.Void(System.Nullable{System.Formats.Asn1.Asn1Tag})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [20] System.Formats.Asn1/System.Formats.Asn1.AsnReader::ReadObjectIdentifier:System.String(System.Nullable{System.Formats.Asn1.Asn1Tag}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 20,
                    MethodSubjectId = "System.Formats.Asn1/System.Formats.Asn1.AsnReader::ReadObjectIdentifier:System.String(System.Nullable{System.Formats.Asn1.Asn1Tag})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [21] System.Formats.Asn1/System.Formats.Asn1.AsnReader::ReadOctetString:System.Byte{}(System.Nullable{System.Formats.Asn1.Asn1Tag}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 21,
                    MethodSubjectId = "System.Formats.Asn1/System.Formats.Asn1.AsnReader::ReadOctetString:System.Byte{}(System.Nullable{System.Formats.Asn1.Asn1Tag})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [22] System.Formats.Asn1/System.Formats.Asn1.AsnReader::ReadSequence:System.Formats.Asn1.AsnReader(System.Nullable{System.Formats.Asn1.Asn1Tag}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 22,
                    MethodSubjectId = "System.Formats.Asn1/System.Formats.Asn1.AsnReader::ReadSequence:System.Formats.Asn1.AsnReader(System.Nullable{System.Formats.Asn1.Asn1Tag})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [23] System.Formats.Asn1/System.Formats.Asn1.AsnReader::ReadSetOf:System.Formats.Asn1.AsnReader(System.Nullable{System.Formats.Asn1.Asn1Tag}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 23,
                    MethodSubjectId = "System.Formats.Asn1/System.Formats.Asn1.AsnReader::ReadSetOf:System.Formats.Asn1.AsnReader(System.Nullable{System.Formats.Asn1.Asn1Tag})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [24] System.Formats.Asn1/System.Formats.Asn1.AsnReader::ReadSetOf:System.Formats.Asn1.AsnReader(System.Boolean,System.Nullable{System.Formats.Asn1.Asn1Tag}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 24,
                    MethodSubjectId = "System.Formats.Asn1/System.Formats.Asn1.AsnReader::ReadSetOf:System.Formats.Asn1.AsnReader(System.Boolean,System.Nullable{System.Formats.Asn1.Asn1Tag})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [25] System.Formats.Asn1/System.Formats.Asn1.AsnReader::ReadUtcTime:System.DateTimeOffset(System.Nullable{System.Formats.Asn1.Asn1Tag}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 25,
                    MethodSubjectId = "System.Formats.Asn1/System.Formats.Asn1.AsnReader::ReadUtcTime:System.DateTimeOffset(System.Nullable{System.Formats.Asn1.Asn1Tag})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [26] System.Formats.Asn1/System.Formats.Asn1.AsnReader::ReadUtcTime:System.DateTimeOffset(System.Int32,System.Nullable{System.Formats.Asn1.Asn1Tag}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 26,
                    MethodSubjectId = "System.Formats.Asn1/System.Formats.Asn1.AsnReader::ReadUtcTime:System.DateTimeOffset(System.Int32,System.Nullable{System.Formats.Asn1.Asn1Tag})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [27] System.Formats.Asn1/System.Formats.Asn1.AsnReader::ThrowIfNotEmpty:System.Void() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 27,
                    MethodSubjectId = "System.Formats.Asn1/System.Formats.Asn1.AsnReader::ThrowIfNotEmpty:System.Void()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [28] System.Formats.Asn1/System.Formats.Asn1.AsnReader::TryReadBitString:System.Boolean(System.Span{System.Byte},System.Int32&,System.Int32&,System.Nullable{System.Formats.Asn1.Asn1Tag}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 28,
                    MethodSubjectId = "System.Formats.Asn1/System.Formats.Asn1.AsnReader::TryReadBitString:System.Boolean(System.Span{System.Byte},System.Int32&,System.Int32&,System.Nullable{System.Formats.Asn1.Asn1Tag})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [29] System.Formats.Asn1/System.Formats.Asn1.AsnReader::TryReadCharacterString:System.Boolean(System.Span{System.Char},System.Formats.Asn1.UniversalTagNumber,System.Int32&,System.Nullable{System.Formats.Asn1.Asn1Tag}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 29,
                    MethodSubjectId = "System.Formats.Asn1/System.Formats.Asn1.AsnReader::TryReadCharacterString:System.Boolean(System.Span{System.Char},System.Formats.Asn1.UniversalTagNumber,System.Int32&,System.Nullable{System.Formats.Asn1.Asn1Tag})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [30] System.Formats.Asn1/System.Formats.Asn1.AsnReader::TryReadCharacterStringBytes:System.Boolean(System.Span{System.Byte},System.Formats.Asn1.Asn1Tag,System.Int32&) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 30,
                    MethodSubjectId = "System.Formats.Asn1/System.Formats.Asn1.AsnReader::TryReadCharacterStringBytes:System.Boolean(System.Span{System.Byte},System.Formats.Asn1.Asn1Tag,System.Int32&)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [31] System.Formats.Asn1/System.Formats.Asn1.AsnReader::TryReadInt32:System.Boolean(System.Int32&,System.Nullable{System.Formats.Asn1.Asn1Tag}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 31,
                    MethodSubjectId = "System.Formats.Asn1/System.Formats.Asn1.AsnReader::TryReadInt32:System.Boolean(System.Int32&,System.Nullable{System.Formats.Asn1.Asn1Tag})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [32] System.Formats.Asn1/System.Formats.Asn1.AsnReader::TryReadInt64:System.Boolean(System.Int64&,System.Nullable{System.Formats.Asn1.Asn1Tag}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 32,
                    MethodSubjectId = "System.Formats.Asn1/System.Formats.Asn1.AsnReader::TryReadInt64:System.Boolean(System.Int64&,System.Nullable{System.Formats.Asn1.Asn1Tag})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [33] System.Formats.Asn1/System.Formats.Asn1.AsnReader::TryReadOctetString:System.Boolean(System.Span{System.Byte},System.Int32&,System.Nullable{System.Formats.Asn1.Asn1Tag}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 33,
                    MethodSubjectId = "System.Formats.Asn1/System.Formats.Asn1.AsnReader::TryReadOctetString:System.Boolean(System.Span{System.Byte},System.Int32&,System.Nullable{System.Formats.Asn1.Asn1Tag})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [34] System.Formats.Asn1/System.Formats.Asn1.AsnReader::TryReadPrimitiveBitString:System.Boolean(System.Int32&,System.ReadOnlyMemory{System.Byte}&,System.Nullable{System.Formats.Asn1.Asn1Tag}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 34,
                    MethodSubjectId = "System.Formats.Asn1/System.Formats.Asn1.AsnReader::TryReadPrimitiveBitString:System.Boolean(System.Int32&,System.ReadOnlyMemory{System.Byte}&,System.Nullable{System.Formats.Asn1.Asn1Tag})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [35] System.Formats.Asn1/System.Formats.Asn1.AsnReader::TryReadPrimitiveCharacterStringBytes:System.Boolean(System.Formats.Asn1.Asn1Tag,System.ReadOnlyMemory{System.Byte}&) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 35,
                    MethodSubjectId = "System.Formats.Asn1/System.Formats.Asn1.AsnReader::TryReadPrimitiveCharacterStringBytes:System.Boolean(System.Formats.Asn1.Asn1Tag,System.ReadOnlyMemory{System.Byte}&)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [36] System.Formats.Asn1/System.Formats.Asn1.AsnReader::TryReadPrimitiveOctetString:System.Boolean(System.ReadOnlyMemory{System.Byte}&,System.Nullable{System.Formats.Asn1.Asn1Tag}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 36,
                    MethodSubjectId = "System.Formats.Asn1/System.Formats.Asn1.AsnReader::TryReadPrimitiveOctetString:System.Boolean(System.ReadOnlyMemory{System.Byte}&,System.Nullable{System.Formats.Asn1.Asn1Tag})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [37] System.Formats.Asn1/System.Formats.Asn1.AsnReader::TryReadUInt32:System.Boolean(System.UInt32&,System.Nullable{System.Formats.Asn1.Asn1Tag}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 37,
                    MethodSubjectId = "System.Formats.Asn1/System.Formats.Asn1.AsnReader::TryReadUInt32:System.Boolean(System.UInt32&,System.Nullable{System.Formats.Asn1.Asn1Tag})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [38] System.Formats.Asn1/System.Formats.Asn1.AsnReader::TryReadUInt64:System.Boolean(System.UInt64&,System.Nullable{System.Formats.Asn1.Asn1Tag}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 38,
                    MethodSubjectId = "System.Formats.Asn1/System.Formats.Asn1.AsnReader::TryReadUInt64:System.Boolean(System.UInt64&,System.Nullable{System.Formats.Asn1.Asn1Tag})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [39] System.Formats.Asn1/System.Formats.Asn1.AsnReader::.ctor:System.Void(System.ReadOnlyMemory{System.Byte},System.Formats.Asn1.AsnEncodingRules,System.Formats.Asn1.AsnReaderOptions) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 39,
                    MethodSubjectId = "System.Formats.Asn1/System.Formats.Asn1.AsnReader::.ctor:System.Void(System.ReadOnlyMemory{System.Byte},System.Formats.Asn1.AsnEncodingRules,System.Formats.Asn1.AsnReaderOptions)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
        // Consume accum so JIT cannot elide the computation
        string json = JsonSerializer.Serialize(new { results }, new JsonSerializerOptions { PropertyNamingPolicy = JsonNamingPolicy.CamelCase });
        Console.WriteLine(json);
    }
}
