// Auto-generated managed benchmark harness
// Family: reader, Assembly: System.Text.Json
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
            { // [0] System.Text.Json/System.Text.Json.Utf8JsonReader::CopyString:System.Int32(System.Span{System.Byte}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 0,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.Utf8JsonReader::CopyString:System.Int32(System.Span{System.Byte})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [1] System.Text.Json/System.Text.Json.Utf8JsonReader::CopyString:System.Int32(System.Span{System.Char}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 1,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.Utf8JsonReader::CopyString:System.Int32(System.Span{System.Char})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [2] System.Text.Json/System.Text.Json.Utf8JsonReader::get_BytesConsumed:System.Int64() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 2,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.Utf8JsonReader::get_BytesConsumed:System.Int64()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [3] System.Text.Json/System.Text.Json.Utf8JsonReader::get_CurrentDepth:System.Int32() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 3,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.Utf8JsonReader::get_CurrentDepth:System.Int32()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [4] System.Text.Json/System.Text.Json.Utf8JsonReader::get_CurrentState:System.Text.Json.JsonReaderState() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 4,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.Utf8JsonReader::get_CurrentState:System.Text.Json.JsonReaderState()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [5] System.Text.Json/System.Text.Json.Utf8JsonReader::get_HasValueSequence:System.Boolean() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 5,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.Utf8JsonReader::get_HasValueSequence:System.Boolean()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [6] System.Text.Json/System.Text.Json.Utf8JsonReader::get_IsFinalBlock:System.Boolean() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 6,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.Utf8JsonReader::get_IsFinalBlock:System.Boolean()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [7] System.Text.Json/System.Text.Json.Utf8JsonReader::get_Position:System.SequencePosition() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 7,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.Utf8JsonReader::get_Position:System.SequencePosition()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [8] System.Text.Json/System.Text.Json.Utf8JsonReader::get_TokenStartIndex:System.Int64() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 8,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.Utf8JsonReader::get_TokenStartIndex:System.Int64()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [9] System.Text.Json/System.Text.Json.Utf8JsonReader::get_TokenType:System.Text.Json.JsonTokenType() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 9,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.Utf8JsonReader::get_TokenType:System.Text.Json.JsonTokenType()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [10] System.Text.Json/System.Text.Json.Utf8JsonReader::get_ValueIsEscaped:System.Boolean() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 10,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.Utf8JsonReader::get_ValueIsEscaped:System.Boolean()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [11] System.Text.Json/System.Text.Json.Utf8JsonReader::get_ValueSequence:System.Buffers.ReadOnlySequence{System.Byte}() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 11,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.Utf8JsonReader::get_ValueSequence:System.Buffers.ReadOnlySequence{System.Byte}()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [12] System.Text.Json/System.Text.Json.Utf8JsonReader::get_ValueSpan:System.ReadOnlySpan{System.Byte}() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 12,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.Utf8JsonReader::get_ValueSpan:System.ReadOnlySpan{System.Byte}()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [13] System.Text.Json/System.Text.Json.Utf8JsonReader::GetBoolean:System.Boolean() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 13,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.Utf8JsonReader::GetBoolean:System.Boolean()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [14] System.Text.Json/System.Text.Json.Utf8JsonReader::GetByte:System.Byte() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 14,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.Utf8JsonReader::GetByte:System.Byte()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [15] System.Text.Json/System.Text.Json.Utf8JsonReader::GetBytesFromBase64:System.Byte{}() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 15,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.Utf8JsonReader::GetBytesFromBase64:System.Byte{}()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [16] System.Text.Json/System.Text.Json.Utf8JsonReader::GetComment:System.String() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 16,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.Utf8JsonReader::GetComment:System.String()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [17] System.Text.Json/System.Text.Json.Utf8JsonReader::GetDateTime:System.DateTime() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 17,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.Utf8JsonReader::GetDateTime:System.DateTime()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [18] System.Text.Json/System.Text.Json.Utf8JsonReader::GetDateTimeOffset:System.DateTimeOffset() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 18,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.Utf8JsonReader::GetDateTimeOffset:System.DateTimeOffset()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [19] System.Text.Json/System.Text.Json.Utf8JsonReader::GetDecimal:System.Decimal() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 19,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.Utf8JsonReader::GetDecimal:System.Decimal()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [20] System.Text.Json/System.Text.Json.Utf8JsonReader::GetDouble:System.Double() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 20,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.Utf8JsonReader::GetDouble:System.Double()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [21] System.Text.Json/System.Text.Json.Utf8JsonReader::GetGuid:System.Guid() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 21,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.Utf8JsonReader::GetGuid:System.Guid()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [22] System.Text.Json/System.Text.Json.Utf8JsonReader::GetInt16:System.Int16() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 22,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.Utf8JsonReader::GetInt16:System.Int16()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [23] System.Text.Json/System.Text.Json.Utf8JsonReader::GetInt32:System.Int32() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 23,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.Utf8JsonReader::GetInt32:System.Int32()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [24] System.Text.Json/System.Text.Json.Utf8JsonReader::GetInt64:System.Int64() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 24,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.Utf8JsonReader::GetInt64:System.Int64()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [25] System.Text.Json/System.Text.Json.Utf8JsonReader::GetSByte:System.SByte() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 25,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.Utf8JsonReader::GetSByte:System.SByte()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [26] System.Text.Json/System.Text.Json.Utf8JsonReader::GetSingle:System.Single() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 26,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.Utf8JsonReader::GetSingle:System.Single()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [27] System.Text.Json/System.Text.Json.Utf8JsonReader::GetString:System.String() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 27,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.Utf8JsonReader::GetString:System.String()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [28] System.Text.Json/System.Text.Json.Utf8JsonReader::GetUInt16:System.UInt16() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 28,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.Utf8JsonReader::GetUInt16:System.UInt16()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [29] System.Text.Json/System.Text.Json.Utf8JsonReader::GetUInt32:System.UInt32() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 29,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.Utf8JsonReader::GetUInt32:System.UInt32()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [30] System.Text.Json/System.Text.Json.Utf8JsonReader::GetUInt64:System.UInt64() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 30,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.Utf8JsonReader::GetUInt64:System.UInt64()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [31] System.Text.Json/System.Text.Json.Utf8JsonReader::Read:System.Boolean() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 31,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.Utf8JsonReader::Read:System.Boolean()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [32] System.Text.Json/System.Text.Json.Utf8JsonReader::Skip:System.Void() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 32,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.Utf8JsonReader::Skip:System.Void()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [33] System.Text.Json/System.Text.Json.Utf8JsonReader::TryGetByte:System.Boolean(System.Byte&) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 33,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.Utf8JsonReader::TryGetByte:System.Boolean(System.Byte&)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [34] System.Text.Json/System.Text.Json.Utf8JsonReader::TryGetBytesFromBase64:System.Boolean(System.Byte{}&) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 34,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.Utf8JsonReader::TryGetBytesFromBase64:System.Boolean(System.Byte{}&)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [35] System.Text.Json/System.Text.Json.Utf8JsonReader::TryGetDateTime:System.Boolean(System.DateTime&) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 35,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.Utf8JsonReader::TryGetDateTime:System.Boolean(System.DateTime&)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [36] System.Text.Json/System.Text.Json.Utf8JsonReader::TryGetDateTimeOffset:System.Boolean(System.DateTimeOffset&) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 36,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.Utf8JsonReader::TryGetDateTimeOffset:System.Boolean(System.DateTimeOffset&)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [37] System.Text.Json/System.Text.Json.Utf8JsonReader::TryGetDecimal:System.Boolean(System.Decimal&) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 37,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.Utf8JsonReader::TryGetDecimal:System.Boolean(System.Decimal&)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [38] System.Text.Json/System.Text.Json.Utf8JsonReader::TryGetDouble:System.Boolean(System.Double&) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 38,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.Utf8JsonReader::TryGetDouble:System.Boolean(System.Double&)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [39] System.Text.Json/System.Text.Json.Utf8JsonReader::TryGetGuid:System.Boolean(System.Guid&) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 39,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.Utf8JsonReader::TryGetGuid:System.Boolean(System.Guid&)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [40] System.Text.Json/System.Text.Json.Utf8JsonReader::TryGetInt16:System.Boolean(System.Int16&) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 40,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.Utf8JsonReader::TryGetInt16:System.Boolean(System.Int16&)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [41] System.Text.Json/System.Text.Json.Utf8JsonReader::TryGetInt32:System.Boolean(System.Int32&) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 41,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.Utf8JsonReader::TryGetInt32:System.Boolean(System.Int32&)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [42] System.Text.Json/System.Text.Json.Utf8JsonReader::TryGetInt64:System.Boolean(System.Int64&) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 42,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.Utf8JsonReader::TryGetInt64:System.Boolean(System.Int64&)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [43] System.Text.Json/System.Text.Json.Utf8JsonReader::TryGetSByte:System.Boolean(System.SByte&) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 43,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.Utf8JsonReader::TryGetSByte:System.Boolean(System.SByte&)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [44] System.Text.Json/System.Text.Json.Utf8JsonReader::TryGetSingle:System.Boolean(System.Single&) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 44,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.Utf8JsonReader::TryGetSingle:System.Boolean(System.Single&)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [45] System.Text.Json/System.Text.Json.Utf8JsonReader::TryGetUInt16:System.Boolean(System.UInt16&) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 45,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.Utf8JsonReader::TryGetUInt16:System.Boolean(System.UInt16&)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [46] System.Text.Json/System.Text.Json.Utf8JsonReader::TryGetUInt32:System.Boolean(System.UInt32&) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 46,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.Utf8JsonReader::TryGetUInt32:System.Boolean(System.UInt32&)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [47] System.Text.Json/System.Text.Json.Utf8JsonReader::TryGetUInt64:System.Boolean(System.UInt64&) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 47,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.Utf8JsonReader::TryGetUInt64:System.Boolean(System.UInt64&)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [48] System.Text.Json/System.Text.Json.Utf8JsonReader::TrySkip:System.Boolean() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 48,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.Utf8JsonReader::TrySkip:System.Boolean()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [49] System.Text.Json/System.Text.Json.Utf8JsonReader::ValueTextEquals:System.Boolean(System.ReadOnlySpan{System.Byte}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 49,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.Utf8JsonReader::ValueTextEquals:System.Boolean(System.ReadOnlySpan{System.Byte})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [50] System.Text.Json/System.Text.Json.Utf8JsonReader::ValueTextEquals:System.Boolean(System.String) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 50,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.Utf8JsonReader::ValueTextEquals:System.Boolean(System.String)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [51] System.Text.Json/System.Text.Json.Utf8JsonReader::ValueTextEquals:System.Boolean(System.ReadOnlySpan{System.Char}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 51,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.Utf8JsonReader::ValueTextEquals:System.Boolean(System.ReadOnlySpan{System.Char})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [52] System.Text.Json/System.Text.Json.Utf8JsonReader::.ctor:System.Void(System.ReadOnlySpan{System.Byte},System.Boolean,System.Text.Json.JsonReaderState) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 52,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.Utf8JsonReader::.ctor:System.Void(System.ReadOnlySpan{System.Byte},System.Boolean,System.Text.Json.JsonReaderState)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [53] System.Text.Json/System.Text.Json.Utf8JsonReader::.ctor:System.Void(System.ReadOnlySpan{System.Byte},System.Text.Json.JsonReaderOptions) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 53,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.Utf8JsonReader::.ctor:System.Void(System.ReadOnlySpan{System.Byte},System.Text.Json.JsonReaderOptions)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [54] System.Text.Json/System.Text.Json.Utf8JsonReader::.ctor:System.Void(System.Buffers.ReadOnlySequence{System.Byte},System.Boolean,System.Text.Json.JsonReaderState) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 54,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.Utf8JsonReader::.ctor:System.Void(System.Buffers.ReadOnlySequence{System.Byte},System.Boolean,System.Text.Json.JsonReaderState)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [55] System.Text.Json/System.Text.Json.Utf8JsonReader::.ctor:System.Void(System.Buffers.ReadOnlySequence{System.Byte},System.Text.Json.JsonReaderOptions) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 55,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.Utf8JsonReader::.ctor:System.Void(System.Buffers.ReadOnlySequence{System.Byte},System.Text.Json.JsonReaderOptions)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [56] System.Text.Json/System.Text.Json.Utf8JsonReader::set_ValueSpan:System.Void(System.ReadOnlySpan{System.Byte}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 56,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.Utf8JsonReader::set_ValueSpan:System.Void(System.ReadOnlySpan{System.Byte})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [57] System.Text.Json/System.Text.Json.Utf8JsonReader::set_TokenStartIndex:System.Void(System.Int64) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 57,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.Utf8JsonReader::set_TokenStartIndex:System.Void(System.Int64)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [58] System.Text.Json/System.Text.Json.Utf8JsonReader::set_HasValueSequence:System.Void(System.Boolean) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 58,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.Utf8JsonReader::set_HasValueSequence:System.Void(System.Boolean)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [59] System.Text.Json/System.Text.Json.Utf8JsonReader::set_ValueIsEscaped:System.Void(System.Boolean) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 59,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.Utf8JsonReader::set_ValueIsEscaped:System.Void(System.Boolean)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [60] System.Text.Json/System.Text.Json.Utf8JsonReader::set_ValueSequence:System.Void(System.Buffers.ReadOnlySequence{System.Byte}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 60,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.Utf8JsonReader::set_ValueSequence:System.Void(System.Buffers.ReadOnlySequence{System.Byte})",
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
