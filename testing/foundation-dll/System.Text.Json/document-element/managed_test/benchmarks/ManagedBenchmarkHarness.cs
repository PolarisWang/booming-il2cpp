// Auto-generated managed benchmark harness
// Family: document-element, Assembly: System.Text.Json
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
            { // [0] System.Text.Json/System.Text.Json.JsonDocument::Dispose:System.Void() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 0,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.JsonDocument::Dispose:System.Void()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [1] System.Text.Json/System.Text.Json.JsonDocument::get_RootElement:System.Text.Json.JsonElement() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 1,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.JsonDocument::get_RootElement:System.Text.Json.JsonElement()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [2] System.Text.Json/System.Text.Json.JsonDocument::Parse:System.Text.Json.JsonDocument(System.ReadOnlyMemory{System.Byte},System.Text.Json.JsonDocumentOptions) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 2,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.JsonDocument::Parse:System.Text.Json.JsonDocument(System.ReadOnlyMemory{System.Byte},System.Text.Json.JsonDocumentOptions)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [3] System.Text.Json/System.Text.Json.JsonDocument::Parse:System.Text.Json.JsonDocument(System.Buffers.ReadOnlySequence{System.Byte},System.Text.Json.JsonDocumentOptions) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 3,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.JsonDocument::Parse:System.Text.Json.JsonDocument(System.Buffers.ReadOnlySequence{System.Byte},System.Text.Json.JsonDocumentOptions)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [4] System.Text.Json/System.Text.Json.JsonDocument::Parse:System.Text.Json.JsonDocument(System.IO.Stream,System.Text.Json.JsonDocumentOptions) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 4,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.JsonDocument::Parse:System.Text.Json.JsonDocument(System.IO.Stream,System.Text.Json.JsonDocumentOptions)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [5] System.Text.Json/System.Text.Json.JsonDocument::Parse:System.Text.Json.JsonDocument(System.ReadOnlyMemory{System.Char},System.Text.Json.JsonDocumentOptions) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 5,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.JsonDocument::Parse:System.Text.Json.JsonDocument(System.ReadOnlyMemory{System.Char},System.Text.Json.JsonDocumentOptions)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [6] System.Text.Json/System.Text.Json.JsonDocument::Parse:System.Text.Json.JsonDocument(System.String,System.Text.Json.JsonDocumentOptions) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 6,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.JsonDocument::Parse:System.Text.Json.JsonDocument(System.String,System.Text.Json.JsonDocumentOptions)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [7] System.Text.Json/System.Text.Json.JsonDocument::ParseAsync:System.Threading.Tasks.Task{System.Text.Json.JsonDocument}(System.IO.Stream,System.Text.Json.JsonDocumentOptions,System.Threading.CancellationToken) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 7,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.JsonDocument::ParseAsync:System.Threading.Tasks.Task{System.Text.Json.JsonDocument}(System.IO.Stream,System.Text.Json.JsonDocumentOptions,System.Threading.CancellationToken)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [8] System.Text.Json/System.Text.Json.JsonDocument::ParseValue:System.Text.Json.JsonDocument(System.Text.Json.Utf8JsonReader&) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 8,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.JsonDocument::ParseValue:System.Text.Json.JsonDocument(System.Text.Json.Utf8JsonReader&)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [9] System.Text.Json/System.Text.Json.JsonDocument::TryParseValue:System.Boolean(System.Text.Json.Utf8JsonReader&,System.Text.Json.JsonDocument&) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 9,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.JsonDocument::TryParseValue:System.Boolean(System.Text.Json.Utf8JsonReader&,System.Text.Json.JsonDocument&)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [10] System.Text.Json/System.Text.Json.JsonDocument::WriteTo:System.Void(System.Text.Json.Utf8JsonWriter) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 10,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.JsonDocument::WriteTo:System.Void(System.Text.Json.Utf8JsonWriter)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [11] System.Text.Json/System.Text.Json.JsonElement::Clone:System.Text.Json.JsonElement() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 11,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.JsonElement::Clone:System.Text.Json.JsonElement()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [12] System.Text.Json/System.Text.Json.JsonElement::DeepEquals:System.Boolean(System.Text.Json.JsonElement,System.Text.Json.JsonElement) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 12,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.JsonElement::DeepEquals:System.Boolean(System.Text.Json.JsonElement,System.Text.Json.JsonElement)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [13] System.Text.Json/System.Text.Json.JsonElement::EnumerateArray:System.Text.Json.JsonElement+ArrayEnumerator() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 13,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.JsonElement::EnumerateArray:System.Text.Json.JsonElement+ArrayEnumerator()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [14] System.Text.Json/System.Text.Json.JsonElement::EnumerateObject:System.Text.Json.JsonElement+ObjectEnumerator() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 14,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.JsonElement::EnumerateObject:System.Text.Json.JsonElement+ObjectEnumerator()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [15] System.Text.Json/System.Text.Json.JsonElement::get_Item:System.Text.Json.JsonElement(System.Int32) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 15,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.JsonElement::get_Item:System.Text.Json.JsonElement(System.Int32)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [16] System.Text.Json/System.Text.Json.JsonElement::get_ValueKind:System.Text.Json.JsonValueKind() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 16,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.JsonElement::get_ValueKind:System.Text.Json.JsonValueKind()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [17] System.Text.Json/System.Text.Json.JsonElement::GetArrayLength:System.Int32() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 17,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.JsonElement::GetArrayLength:System.Int32()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [18] System.Text.Json/System.Text.Json.JsonElement::GetBoolean:System.Boolean() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 18,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.JsonElement::GetBoolean:System.Boolean()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [19] System.Text.Json/System.Text.Json.JsonElement::GetByte:System.Byte() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 19,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.JsonElement::GetByte:System.Byte()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [20] System.Text.Json/System.Text.Json.JsonElement::GetBytesFromBase64:System.Byte{}() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 20,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.JsonElement::GetBytesFromBase64:System.Byte{}()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [21] System.Text.Json/System.Text.Json.JsonElement::GetDateTime:System.DateTime() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 21,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.JsonElement::GetDateTime:System.DateTime()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [22] System.Text.Json/System.Text.Json.JsonElement::GetDateTimeOffset:System.DateTimeOffset() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 22,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.JsonElement::GetDateTimeOffset:System.DateTimeOffset()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [23] System.Text.Json/System.Text.Json.JsonElement::GetDecimal:System.Decimal() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 23,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.JsonElement::GetDecimal:System.Decimal()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [24] System.Text.Json/System.Text.Json.JsonElement::GetDouble:System.Double() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 24,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.JsonElement::GetDouble:System.Double()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [25] System.Text.Json/System.Text.Json.JsonElement::GetGuid:System.Guid() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 25,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.JsonElement::GetGuid:System.Guid()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [26] System.Text.Json/System.Text.Json.JsonElement::GetInt16:System.Int16() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 26,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.JsonElement::GetInt16:System.Int16()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [27] System.Text.Json/System.Text.Json.JsonElement::GetInt32:System.Int32() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 27,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.JsonElement::GetInt32:System.Int32()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [28] System.Text.Json/System.Text.Json.JsonElement::GetInt64:System.Int64() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 28,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.JsonElement::GetInt64:System.Int64()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [29] System.Text.Json/System.Text.Json.JsonElement::GetProperty:System.Text.Json.JsonElement(System.String) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 29,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.JsonElement::GetProperty:System.Text.Json.JsonElement(System.String)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [30] System.Text.Json/System.Text.Json.JsonElement::GetProperty:System.Text.Json.JsonElement(System.ReadOnlySpan{System.Char}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 30,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.JsonElement::GetProperty:System.Text.Json.JsonElement(System.ReadOnlySpan{System.Char})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [31] System.Text.Json/System.Text.Json.JsonElement::GetProperty:System.Text.Json.JsonElement(System.ReadOnlySpan{System.Byte}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 31,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.JsonElement::GetProperty:System.Text.Json.JsonElement(System.ReadOnlySpan{System.Byte})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [32] System.Text.Json/System.Text.Json.JsonElement::GetPropertyCount:System.Int32() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 32,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.JsonElement::GetPropertyCount:System.Int32()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [33] System.Text.Json/System.Text.Json.JsonElement::GetRawText:System.String() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 33,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.JsonElement::GetRawText:System.String()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [34] System.Text.Json/System.Text.Json.JsonElement::GetSByte:System.SByte() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 34,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.JsonElement::GetSByte:System.SByte()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [35] System.Text.Json/System.Text.Json.JsonElement::GetSingle:System.Single() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 35,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.JsonElement::GetSingle:System.Single()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [36] System.Text.Json/System.Text.Json.JsonElement::GetString:System.String() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 36,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.JsonElement::GetString:System.String()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [37] System.Text.Json/System.Text.Json.JsonElement::GetUInt16:System.UInt16() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 37,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.JsonElement::GetUInt16:System.UInt16()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [38] System.Text.Json/System.Text.Json.JsonElement::GetUInt32:System.UInt32() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 38,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.JsonElement::GetUInt32:System.UInt32()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [39] System.Text.Json/System.Text.Json.JsonElement::GetUInt64:System.UInt64() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 39,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.JsonElement::GetUInt64:System.UInt64()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [40] System.Text.Json/System.Text.Json.JsonElement::Parse:System.Text.Json.JsonElement(System.ReadOnlySpan{System.Byte},System.Text.Json.JsonDocumentOptions) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 40,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.JsonElement::Parse:System.Text.Json.JsonElement(System.ReadOnlySpan{System.Byte},System.Text.Json.JsonDocumentOptions)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [41] System.Text.Json/System.Text.Json.JsonElement::Parse:System.Text.Json.JsonElement(System.ReadOnlySpan{System.Char},System.Text.Json.JsonDocumentOptions) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 41,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.JsonElement::Parse:System.Text.Json.JsonElement(System.ReadOnlySpan{System.Char},System.Text.Json.JsonDocumentOptions)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [42] System.Text.Json/System.Text.Json.JsonElement::Parse:System.Text.Json.JsonElement(System.String,System.Text.Json.JsonDocumentOptions) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 42,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.JsonElement::Parse:System.Text.Json.JsonElement(System.String,System.Text.Json.JsonDocumentOptions)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [43] System.Text.Json/System.Text.Json.JsonElement::ParseValue:System.Text.Json.JsonElement(System.Text.Json.Utf8JsonReader&) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 43,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.JsonElement::ParseValue:System.Text.Json.JsonElement(System.Text.Json.Utf8JsonReader&)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [44] System.Text.Json/System.Text.Json.JsonElement::ToString:System.String() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 44,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.JsonElement::ToString:System.String()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [45] System.Text.Json/System.Text.Json.JsonElement::TryGetByte:System.Boolean(System.Byte&) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 45,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.JsonElement::TryGetByte:System.Boolean(System.Byte&)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [46] System.Text.Json/System.Text.Json.JsonElement::TryGetBytesFromBase64:System.Boolean(System.Byte{}&) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 46,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.JsonElement::TryGetBytesFromBase64:System.Boolean(System.Byte{}&)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [47] System.Text.Json/System.Text.Json.JsonElement::TryGetDateTime:System.Boolean(System.DateTime&) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 47,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.JsonElement::TryGetDateTime:System.Boolean(System.DateTime&)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [48] System.Text.Json/System.Text.Json.JsonElement::TryGetDateTimeOffset:System.Boolean(System.DateTimeOffset&) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 48,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.JsonElement::TryGetDateTimeOffset:System.Boolean(System.DateTimeOffset&)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [49] System.Text.Json/System.Text.Json.JsonElement::TryGetDecimal:System.Boolean(System.Decimal&) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 49,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.JsonElement::TryGetDecimal:System.Boolean(System.Decimal&)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [50] System.Text.Json/System.Text.Json.JsonElement::TryGetDouble:System.Boolean(System.Double&) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 50,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.JsonElement::TryGetDouble:System.Boolean(System.Double&)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [51] System.Text.Json/System.Text.Json.JsonElement::TryGetGuid:System.Boolean(System.Guid&) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 51,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.JsonElement::TryGetGuid:System.Boolean(System.Guid&)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [52] System.Text.Json/System.Text.Json.JsonElement::TryGetInt16:System.Boolean(System.Int16&) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 52,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.JsonElement::TryGetInt16:System.Boolean(System.Int16&)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [53] System.Text.Json/System.Text.Json.JsonElement::TryGetInt32:System.Boolean(System.Int32&) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 53,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.JsonElement::TryGetInt32:System.Boolean(System.Int32&)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [54] System.Text.Json/System.Text.Json.JsonElement::TryGetInt64:System.Boolean(System.Int64&) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 54,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.JsonElement::TryGetInt64:System.Boolean(System.Int64&)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [55] System.Text.Json/System.Text.Json.JsonElement::TryGetProperty:System.Boolean(System.String,System.Text.Json.JsonElement&) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 55,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.JsonElement::TryGetProperty:System.Boolean(System.String,System.Text.Json.JsonElement&)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [56] System.Text.Json/System.Text.Json.JsonElement::TryGetProperty:System.Boolean(System.ReadOnlySpan{System.Char},System.Text.Json.JsonElement&) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 56,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.JsonElement::TryGetProperty:System.Boolean(System.ReadOnlySpan{System.Char},System.Text.Json.JsonElement&)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [57] System.Text.Json/System.Text.Json.JsonElement::TryGetProperty:System.Boolean(System.ReadOnlySpan{System.Byte},System.Text.Json.JsonElement&) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 57,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.JsonElement::TryGetProperty:System.Boolean(System.ReadOnlySpan{System.Byte},System.Text.Json.JsonElement&)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [58] System.Text.Json/System.Text.Json.JsonElement::TryGetSByte:System.Boolean(System.SByte&) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 58,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.JsonElement::TryGetSByte:System.Boolean(System.SByte&)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [59] System.Text.Json/System.Text.Json.JsonElement::TryGetSingle:System.Boolean(System.Single&) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 59,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.JsonElement::TryGetSingle:System.Boolean(System.Single&)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [60] System.Text.Json/System.Text.Json.JsonElement::TryGetUInt16:System.Boolean(System.UInt16&) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 60,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.JsonElement::TryGetUInt16:System.Boolean(System.UInt16&)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [61] System.Text.Json/System.Text.Json.JsonElement::TryGetUInt32:System.Boolean(System.UInt32&) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 61,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.JsonElement::TryGetUInt32:System.Boolean(System.UInt32&)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [62] System.Text.Json/System.Text.Json.JsonElement::TryGetUInt64:System.Boolean(System.UInt64&) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 62,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.JsonElement::TryGetUInt64:System.Boolean(System.UInt64&)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [63] System.Text.Json/System.Text.Json.JsonElement::TryParseValue:System.Boolean(System.Text.Json.Utf8JsonReader&,System.Nullable{System.Text.Json.JsonElement}&) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 63,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.JsonElement::TryParseValue:System.Boolean(System.Text.Json.Utf8JsonReader&,System.Nullable{System.Text.Json.JsonElement}&)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [64] System.Text.Json/System.Text.Json.JsonElement::ValueEquals:System.Boolean(System.String) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 64,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.JsonElement::ValueEquals:System.Boolean(System.String)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [65] System.Text.Json/System.Text.Json.JsonElement::ValueEquals:System.Boolean(System.ReadOnlySpan{System.Byte}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 65,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.JsonElement::ValueEquals:System.Boolean(System.ReadOnlySpan{System.Byte})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [66] System.Text.Json/System.Text.Json.JsonElement::ValueEquals:System.Boolean(System.ReadOnlySpan{System.Char}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 66,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.JsonElement::ValueEquals:System.Boolean(System.ReadOnlySpan{System.Char})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [67] System.Text.Json/System.Text.Json.JsonElement::WriteTo:System.Void(System.Text.Json.Utf8JsonWriter) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 67,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.JsonElement::WriteTo:System.Void(System.Text.Json.Utf8JsonWriter)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [68] System.Text.Json/System.Text.Json.JsonElement::get_Item:System.Text.Json.JsonElement() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 68,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.JsonElement::get_Item:System.Text.Json.JsonElement()",
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
