// Auto-generated managed benchmark harness
// Family: decoder, Assembly: System.Formats.Asn1
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
            { // [0] System.Formats.Asn1/System.Formats.Asn1.AsnDecoder::DecodeLength:System.Nullable{System.Int32}(System.ReadOnlySpan{System.Byte},System.Formats.Asn1.AsnEncodingRules,System.Int32&) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 0,
                    MethodSubjectId = "System.Formats.Asn1/System.Formats.Asn1.AsnDecoder::DecodeLength:System.Nullable{System.Int32}(System.ReadOnlySpan{System.Byte},System.Formats.Asn1.AsnEncodingRules,System.Int32&)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [1] System.Formats.Asn1/System.Formats.Asn1.AsnDecoder::ReadBitString:System.Byte{}(System.ReadOnlySpan{System.Byte},System.Formats.Asn1.AsnEncodingRules,System.Int32&,System.Int32&,System.Nullable{System.Formats.Asn1.Asn1Tag}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 1,
                    MethodSubjectId = "System.Formats.Asn1/System.Formats.Asn1.AsnDecoder::ReadBitString:System.Byte{}(System.ReadOnlySpan{System.Byte},System.Formats.Asn1.AsnEncodingRules,System.Int32&,System.Int32&,System.Nullable{System.Formats.Asn1.Asn1Tag})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [2] System.Formats.Asn1/System.Formats.Asn1.AsnDecoder::ReadBoolean:System.Boolean(System.ReadOnlySpan{System.Byte},System.Formats.Asn1.AsnEncodingRules,System.Int32&,System.Nullable{System.Formats.Asn1.Asn1Tag}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 2,
                    MethodSubjectId = "System.Formats.Asn1/System.Formats.Asn1.AsnDecoder::ReadBoolean:System.Boolean(System.ReadOnlySpan{System.Byte},System.Formats.Asn1.AsnEncodingRules,System.Int32&,System.Nullable{System.Formats.Asn1.Asn1Tag})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [3] System.Formats.Asn1/System.Formats.Asn1.AsnDecoder::ReadCharacterString:System.String(System.ReadOnlySpan{System.Byte},System.Formats.Asn1.AsnEncodingRules,System.Formats.Asn1.UniversalTagNumber,System.Int32&,System.Nullable{System.Formats.Asn1.Asn1Tag}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 3,
                    MethodSubjectId = "System.Formats.Asn1/System.Formats.Asn1.AsnDecoder::ReadCharacterString:System.String(System.ReadOnlySpan{System.Byte},System.Formats.Asn1.AsnEncodingRules,System.Formats.Asn1.UniversalTagNumber,System.Int32&,System.Nullable{System.Formats.Asn1.Asn1Tag})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [4] System.Formats.Asn1/System.Formats.Asn1.AsnDecoder::ReadEncodedValue:System.Formats.Asn1.Asn1Tag(System.ReadOnlySpan{System.Byte},System.Formats.Asn1.AsnEncodingRules,System.Int32&,System.Int32&,System.Int32&) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 4,
                    MethodSubjectId = "System.Formats.Asn1/System.Formats.Asn1.AsnDecoder::ReadEncodedValue:System.Formats.Asn1.Asn1Tag(System.ReadOnlySpan{System.Byte},System.Formats.Asn1.AsnEncodingRules,System.Int32&,System.Int32&,System.Int32&)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [5] System.Formats.Asn1/System.Formats.Asn1.AsnDecoder::ReadEnumeratedBytes:System.ReadOnlySpan{System.Byte}(System.ReadOnlySpan{System.Byte},System.Formats.Asn1.AsnEncodingRules,System.Int32&,System.Nullable{System.Formats.Asn1.Asn1Tag}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 5,
                    MethodSubjectId = "System.Formats.Asn1/System.Formats.Asn1.AsnDecoder::ReadEnumeratedBytes:System.ReadOnlySpan{System.Byte}(System.ReadOnlySpan{System.Byte},System.Formats.Asn1.AsnEncodingRules,System.Int32&,System.Nullable{System.Formats.Asn1.Asn1Tag})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [6] System.Formats.Asn1/System.Formats.Asn1.AsnDecoder::ReadEnumeratedValue{TEnum}:TEnum(System.ReadOnlySpan{System.Byte},System.Formats.Asn1.AsnEncodingRules,System.Int32&,System.Nullable{System.Formats.Asn1.Asn1Tag}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 6,
                    MethodSubjectId = "System.Formats.Asn1/System.Formats.Asn1.AsnDecoder::ReadEnumeratedValue{TEnum}:TEnum(System.ReadOnlySpan{System.Byte},System.Formats.Asn1.AsnEncodingRules,System.Int32&,System.Nullable{System.Formats.Asn1.Asn1Tag})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [7] System.Formats.Asn1/System.Formats.Asn1.AsnDecoder::ReadEnumeratedValue:System.Enum(System.ReadOnlySpan{System.Byte},System.Formats.Asn1.AsnEncodingRules,System.Type,System.Int32&,System.Nullable{System.Formats.Asn1.Asn1Tag}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 7,
                    MethodSubjectId = "System.Formats.Asn1/System.Formats.Asn1.AsnDecoder::ReadEnumeratedValue:System.Enum(System.ReadOnlySpan{System.Byte},System.Formats.Asn1.AsnEncodingRules,System.Type,System.Int32&,System.Nullable{System.Formats.Asn1.Asn1Tag})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [8] System.Formats.Asn1/System.Formats.Asn1.AsnDecoder::ReadGeneralizedTime:System.DateTimeOffset(System.ReadOnlySpan{System.Byte},System.Formats.Asn1.AsnEncodingRules,System.Int32&,System.Nullable{System.Formats.Asn1.Asn1Tag}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 8,
                    MethodSubjectId = "System.Formats.Asn1/System.Formats.Asn1.AsnDecoder::ReadGeneralizedTime:System.DateTimeOffset(System.ReadOnlySpan{System.Byte},System.Formats.Asn1.AsnEncodingRules,System.Int32&,System.Nullable{System.Formats.Asn1.Asn1Tag})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [9] System.Formats.Asn1/System.Formats.Asn1.AsnDecoder::ReadInteger:System.Numerics.BigInteger(System.ReadOnlySpan{System.Byte},System.Formats.Asn1.AsnEncodingRules,System.Int32&,System.Nullable{System.Formats.Asn1.Asn1Tag}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 9,
                    MethodSubjectId = "System.Formats.Asn1/System.Formats.Asn1.AsnDecoder::ReadInteger:System.Numerics.BigInteger(System.ReadOnlySpan{System.Byte},System.Formats.Asn1.AsnEncodingRules,System.Int32&,System.Nullable{System.Formats.Asn1.Asn1Tag})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [10] System.Formats.Asn1/System.Formats.Asn1.AsnDecoder::ReadIntegerBytes:System.ReadOnlySpan{System.Byte}(System.ReadOnlySpan{System.Byte},System.Formats.Asn1.AsnEncodingRules,System.Int32&,System.Nullable{System.Formats.Asn1.Asn1Tag}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 10,
                    MethodSubjectId = "System.Formats.Asn1/System.Formats.Asn1.AsnDecoder::ReadIntegerBytes:System.ReadOnlySpan{System.Byte}(System.ReadOnlySpan{System.Byte},System.Formats.Asn1.AsnEncodingRules,System.Int32&,System.Nullable{System.Formats.Asn1.Asn1Tag})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [11] System.Formats.Asn1/System.Formats.Asn1.AsnDecoder::ReadNamedBitList:System.Collections.BitArray(System.ReadOnlySpan{System.Byte},System.Formats.Asn1.AsnEncodingRules,System.Int32&,System.Nullable{System.Formats.Asn1.Asn1Tag}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 11,
                    MethodSubjectId = "System.Formats.Asn1/System.Formats.Asn1.AsnDecoder::ReadNamedBitList:System.Collections.BitArray(System.ReadOnlySpan{System.Byte},System.Formats.Asn1.AsnEncodingRules,System.Int32&,System.Nullable{System.Formats.Asn1.Asn1Tag})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [12] System.Formats.Asn1/System.Formats.Asn1.AsnDecoder::ReadNamedBitListValue{TFlagsEnum}:TFlagsEnum(System.ReadOnlySpan{System.Byte},System.Formats.Asn1.AsnEncodingRules,System.Int32&,System.Nullable{System.Formats.Asn1.Asn1Tag}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 12,
                    MethodSubjectId = "System.Formats.Asn1/System.Formats.Asn1.AsnDecoder::ReadNamedBitListValue{TFlagsEnum}:TFlagsEnum(System.ReadOnlySpan{System.Byte},System.Formats.Asn1.AsnEncodingRules,System.Int32&,System.Nullable{System.Formats.Asn1.Asn1Tag})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [13] System.Formats.Asn1/System.Formats.Asn1.AsnDecoder::ReadNamedBitListValue:System.Enum(System.ReadOnlySpan{System.Byte},System.Formats.Asn1.AsnEncodingRules,System.Type,System.Int32&,System.Nullable{System.Formats.Asn1.Asn1Tag}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 13,
                    MethodSubjectId = "System.Formats.Asn1/System.Formats.Asn1.AsnDecoder::ReadNamedBitListValue:System.Enum(System.ReadOnlySpan{System.Byte},System.Formats.Asn1.AsnEncodingRules,System.Type,System.Int32&,System.Nullable{System.Formats.Asn1.Asn1Tag})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [14] System.Formats.Asn1/System.Formats.Asn1.AsnDecoder::ReadNull:System.Void(System.ReadOnlySpan{System.Byte},System.Formats.Asn1.AsnEncodingRules,System.Int32&,System.Nullable{System.Formats.Asn1.Asn1Tag}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 14,
                    MethodSubjectId = "System.Formats.Asn1/System.Formats.Asn1.AsnDecoder::ReadNull:System.Void(System.ReadOnlySpan{System.Byte},System.Formats.Asn1.AsnEncodingRules,System.Int32&,System.Nullable{System.Formats.Asn1.Asn1Tag})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [15] System.Formats.Asn1/System.Formats.Asn1.AsnDecoder::ReadObjectIdentifier:System.String(System.ReadOnlySpan{System.Byte},System.Formats.Asn1.AsnEncodingRules,System.Int32&,System.Nullable{System.Formats.Asn1.Asn1Tag}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 15,
                    MethodSubjectId = "System.Formats.Asn1/System.Formats.Asn1.AsnDecoder::ReadObjectIdentifier:System.String(System.ReadOnlySpan{System.Byte},System.Formats.Asn1.AsnEncodingRules,System.Int32&,System.Nullable{System.Formats.Asn1.Asn1Tag})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [16] System.Formats.Asn1/System.Formats.Asn1.AsnDecoder::ReadOctetString:System.Byte{}(System.ReadOnlySpan{System.Byte},System.Formats.Asn1.AsnEncodingRules,System.Int32&,System.Nullable{System.Formats.Asn1.Asn1Tag}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 16,
                    MethodSubjectId = "System.Formats.Asn1/System.Formats.Asn1.AsnDecoder::ReadOctetString:System.Byte{}(System.ReadOnlySpan{System.Byte},System.Formats.Asn1.AsnEncodingRules,System.Int32&,System.Nullable{System.Formats.Asn1.Asn1Tag})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [17] System.Formats.Asn1/System.Formats.Asn1.AsnDecoder::ReadSequence:System.Void(System.ReadOnlySpan{System.Byte},System.Formats.Asn1.AsnEncodingRules,System.Int32&,System.Int32&,System.Int32&,System.Nullable{System.Formats.Asn1.Asn1Tag}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 17,
                    MethodSubjectId = "System.Formats.Asn1/System.Formats.Asn1.AsnDecoder::ReadSequence:System.Void(System.ReadOnlySpan{System.Byte},System.Formats.Asn1.AsnEncodingRules,System.Int32&,System.Int32&,System.Int32&,System.Nullable{System.Formats.Asn1.Asn1Tag})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [18] System.Formats.Asn1/System.Formats.Asn1.AsnDecoder::ReadSetOf:System.Void(System.ReadOnlySpan{System.Byte},System.Formats.Asn1.AsnEncodingRules,System.Int32&,System.Int32&,System.Int32&,System.Boolean,System.Nullable{System.Formats.Asn1.Asn1Tag}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 18,
                    MethodSubjectId = "System.Formats.Asn1/System.Formats.Asn1.AsnDecoder::ReadSetOf:System.Void(System.ReadOnlySpan{System.Byte},System.Formats.Asn1.AsnEncodingRules,System.Int32&,System.Int32&,System.Int32&,System.Boolean,System.Nullable{System.Formats.Asn1.Asn1Tag})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [19] System.Formats.Asn1/System.Formats.Asn1.AsnDecoder::ReadUtcTime:System.DateTimeOffset(System.ReadOnlySpan{System.Byte},System.Formats.Asn1.AsnEncodingRules,System.Int32&,System.Int32,System.Nullable{System.Formats.Asn1.Asn1Tag}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 19,
                    MethodSubjectId = "System.Formats.Asn1/System.Formats.Asn1.AsnDecoder::ReadUtcTime:System.DateTimeOffset(System.ReadOnlySpan{System.Byte},System.Formats.Asn1.AsnEncodingRules,System.Int32&,System.Int32,System.Nullable{System.Formats.Asn1.Asn1Tag})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [20] System.Formats.Asn1/System.Formats.Asn1.AsnDecoder::TryDecodeLength:System.Boolean(System.ReadOnlySpan{System.Byte},System.Formats.Asn1.AsnEncodingRules,System.Nullable{System.Int32}&,System.Int32&) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 20,
                    MethodSubjectId = "System.Formats.Asn1/System.Formats.Asn1.AsnDecoder::TryDecodeLength:System.Boolean(System.ReadOnlySpan{System.Byte},System.Formats.Asn1.AsnEncodingRules,System.Nullable{System.Int32}&,System.Int32&)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [21] System.Formats.Asn1/System.Formats.Asn1.AsnDecoder::TryReadBitString:System.Boolean(System.ReadOnlySpan{System.Byte},System.Span{System.Byte},System.Formats.Asn1.AsnEncodingRules,System.Int32&,System.Int32&,System.Int32&,System.Nullable{System.Formats.Asn1.Asn1Tag}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 21,
                    MethodSubjectId = "System.Formats.Asn1/System.Formats.Asn1.AsnDecoder::TryReadBitString:System.Boolean(System.ReadOnlySpan{System.Byte},System.Span{System.Byte},System.Formats.Asn1.AsnEncodingRules,System.Int32&,System.Int32&,System.Int32&,System.Nullable{System.Formats.Asn1.Asn1Tag})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [22] System.Formats.Asn1/System.Formats.Asn1.AsnDecoder::TryReadCharacterString:System.Boolean(System.ReadOnlySpan{System.Byte},System.Span{System.Char},System.Formats.Asn1.AsnEncodingRules,System.Formats.Asn1.UniversalTagNumber,System.Int32&,System.Int32&,System.Nullable{System.Formats.Asn1.Asn1Tag}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 22,
                    MethodSubjectId = "System.Formats.Asn1/System.Formats.Asn1.AsnDecoder::TryReadCharacterString:System.Boolean(System.ReadOnlySpan{System.Byte},System.Span{System.Char},System.Formats.Asn1.AsnEncodingRules,System.Formats.Asn1.UniversalTagNumber,System.Int32&,System.Int32&,System.Nullable{System.Formats.Asn1.Asn1Tag})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [23] System.Formats.Asn1/System.Formats.Asn1.AsnDecoder::TryReadCharacterStringBytes:System.Boolean(System.ReadOnlySpan{System.Byte},System.Span{System.Byte},System.Formats.Asn1.AsnEncodingRules,System.Formats.Asn1.Asn1Tag,System.Int32&,System.Int32&) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 23,
                    MethodSubjectId = "System.Formats.Asn1/System.Formats.Asn1.AsnDecoder::TryReadCharacterStringBytes:System.Boolean(System.ReadOnlySpan{System.Byte},System.Span{System.Byte},System.Formats.Asn1.AsnEncodingRules,System.Formats.Asn1.Asn1Tag,System.Int32&,System.Int32&)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [24] System.Formats.Asn1/System.Formats.Asn1.AsnDecoder::TryReadEncodedValue:System.Boolean(System.ReadOnlySpan{System.Byte},System.Formats.Asn1.AsnEncodingRules,System.Formats.Asn1.Asn1Tag&,System.Int32&,System.Int32&,System.Int32&) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 24,
                    MethodSubjectId = "System.Formats.Asn1/System.Formats.Asn1.AsnDecoder::TryReadEncodedValue:System.Boolean(System.ReadOnlySpan{System.Byte},System.Formats.Asn1.AsnEncodingRules,System.Formats.Asn1.Asn1Tag&,System.Int32&,System.Int32&,System.Int32&)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [25] System.Formats.Asn1/System.Formats.Asn1.AsnDecoder::TryReadInt32:System.Boolean(System.ReadOnlySpan{System.Byte},System.Formats.Asn1.AsnEncodingRules,System.Int32&,System.Int32&,System.Nullable{System.Formats.Asn1.Asn1Tag}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 25,
                    MethodSubjectId = "System.Formats.Asn1/System.Formats.Asn1.AsnDecoder::TryReadInt32:System.Boolean(System.ReadOnlySpan{System.Byte},System.Formats.Asn1.AsnEncodingRules,System.Int32&,System.Int32&,System.Nullable{System.Formats.Asn1.Asn1Tag})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [26] System.Formats.Asn1/System.Formats.Asn1.AsnDecoder::TryReadInt64:System.Boolean(System.ReadOnlySpan{System.Byte},System.Formats.Asn1.AsnEncodingRules,System.Int64&,System.Int32&,System.Nullable{System.Formats.Asn1.Asn1Tag}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 26,
                    MethodSubjectId = "System.Formats.Asn1/System.Formats.Asn1.AsnDecoder::TryReadInt64:System.Boolean(System.ReadOnlySpan{System.Byte},System.Formats.Asn1.AsnEncodingRules,System.Int64&,System.Int32&,System.Nullable{System.Formats.Asn1.Asn1Tag})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [27] System.Formats.Asn1/System.Formats.Asn1.AsnDecoder::TryReadOctetString:System.Boolean(System.ReadOnlySpan{System.Byte},System.Span{System.Byte},System.Formats.Asn1.AsnEncodingRules,System.Int32&,System.Int32&,System.Nullable{System.Formats.Asn1.Asn1Tag}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 27,
                    MethodSubjectId = "System.Formats.Asn1/System.Formats.Asn1.AsnDecoder::TryReadOctetString:System.Boolean(System.ReadOnlySpan{System.Byte},System.Span{System.Byte},System.Formats.Asn1.AsnEncodingRules,System.Int32&,System.Int32&,System.Nullable{System.Formats.Asn1.Asn1Tag})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [28] System.Formats.Asn1/System.Formats.Asn1.AsnDecoder::TryReadPrimitiveBitString:System.Boolean(System.ReadOnlySpan{System.Byte},System.Formats.Asn1.AsnEncodingRules,System.Int32&,System.ReadOnlySpan{System.Byte}&,System.Int32&,System.Nullable{System.Formats.Asn1.Asn1Tag}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 28,
                    MethodSubjectId = "System.Formats.Asn1/System.Formats.Asn1.AsnDecoder::TryReadPrimitiveBitString:System.Boolean(System.ReadOnlySpan{System.Byte},System.Formats.Asn1.AsnEncodingRules,System.Int32&,System.ReadOnlySpan{System.Byte}&,System.Int32&,System.Nullable{System.Formats.Asn1.Asn1Tag})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [29] System.Formats.Asn1/System.Formats.Asn1.AsnDecoder::TryReadPrimitiveCharacterStringBytes:System.Boolean(System.ReadOnlySpan{System.Byte},System.Formats.Asn1.AsnEncodingRules,System.Formats.Asn1.Asn1Tag,System.ReadOnlySpan{System.Byte}&,System.Int32&) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 29,
                    MethodSubjectId = "System.Formats.Asn1/System.Formats.Asn1.AsnDecoder::TryReadPrimitiveCharacterStringBytes:System.Boolean(System.ReadOnlySpan{System.Byte},System.Formats.Asn1.AsnEncodingRules,System.Formats.Asn1.Asn1Tag,System.ReadOnlySpan{System.Byte}&,System.Int32&)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [30] System.Formats.Asn1/System.Formats.Asn1.AsnDecoder::TryReadPrimitiveOctetString:System.Boolean(System.ReadOnlySpan{System.Byte},System.Formats.Asn1.AsnEncodingRules,System.ReadOnlySpan{System.Byte}&,System.Int32&,System.Nullable{System.Formats.Asn1.Asn1Tag}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 30,
                    MethodSubjectId = "System.Formats.Asn1/System.Formats.Asn1.AsnDecoder::TryReadPrimitiveOctetString:System.Boolean(System.ReadOnlySpan{System.Byte},System.Formats.Asn1.AsnEncodingRules,System.ReadOnlySpan{System.Byte}&,System.Int32&,System.Nullable{System.Formats.Asn1.Asn1Tag})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [31] System.Formats.Asn1/System.Formats.Asn1.AsnDecoder::TryReadUInt32:System.Boolean(System.ReadOnlySpan{System.Byte},System.Formats.Asn1.AsnEncodingRules,System.UInt32&,System.Int32&,System.Nullable{System.Formats.Asn1.Asn1Tag}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 31,
                    MethodSubjectId = "System.Formats.Asn1/System.Formats.Asn1.AsnDecoder::TryReadUInt32:System.Boolean(System.ReadOnlySpan{System.Byte},System.Formats.Asn1.AsnEncodingRules,System.UInt32&,System.Int32&,System.Nullable{System.Formats.Asn1.Asn1Tag})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [32] System.Formats.Asn1/System.Formats.Asn1.AsnDecoder::TryReadUInt64:System.Boolean(System.ReadOnlySpan{System.Byte},System.Formats.Asn1.AsnEncodingRules,System.UInt64&,System.Int32&,System.Nullable{System.Formats.Asn1.Asn1Tag}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 32,
                    MethodSubjectId = "System.Formats.Asn1/System.Formats.Asn1.AsnDecoder::TryReadUInt64:System.Boolean(System.ReadOnlySpan{System.Byte},System.Formats.Asn1.AsnEncodingRules,System.UInt64&,System.Int32&,System.Nullable{System.Formats.Asn1.Asn1Tag})",
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
