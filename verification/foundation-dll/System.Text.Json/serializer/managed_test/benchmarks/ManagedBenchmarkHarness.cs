// Auto-generated managed benchmark harness
// Family: serializer, Assembly: System.Text.Json
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
            { // [0] System.Text.Json/System.Text.Json.JsonSerializer::Deserialize{TValue}:TValue(System.Text.Json.JsonDocument,System.Text.Json.JsonSerializerOptions) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 0,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.JsonSerializer::Deserialize{TValue}:TValue(System.Text.Json.JsonDocument,System.Text.Json.JsonSerializerOptions)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [1] System.Text.Json/System.Text.Json.JsonSerializer::Deserialize:System.Object(System.Text.Json.JsonDocument,System.Type,System.Text.Json.JsonSerializerOptions) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 1,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.JsonSerializer::Deserialize:System.Object(System.Text.Json.JsonDocument,System.Type,System.Text.Json.JsonSerializerOptions)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [2] System.Text.Json/System.Text.Json.JsonSerializer::Deserialize{TValue}:TValue(System.Text.Json.JsonDocument,System.Text.Json.Serialization.Metadata.JsonTypeInfo{TValue}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 2,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.JsonSerializer::Deserialize{TValue}:TValue(System.Text.Json.JsonDocument,System.Text.Json.Serialization.Metadata.JsonTypeInfo{TValue})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [3] System.Text.Json/System.Text.Json.JsonSerializer::Deserialize:System.Object(System.Text.Json.JsonDocument,System.Text.Json.Serialization.Metadata.JsonTypeInfo) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 3,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.JsonSerializer::Deserialize:System.Object(System.Text.Json.JsonDocument,System.Text.Json.Serialization.Metadata.JsonTypeInfo)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [4] System.Text.Json/System.Text.Json.JsonSerializer::Deserialize:System.Object(System.Text.Json.JsonDocument,System.Type,System.Text.Json.Serialization.JsonSerializerContext) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 4,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.JsonSerializer::Deserialize:System.Object(System.Text.Json.JsonDocument,System.Type,System.Text.Json.Serialization.JsonSerializerContext)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [5] System.Text.Json/System.Text.Json.JsonSerializer::Deserialize{TValue}:TValue(System.Text.Json.JsonElement,System.Text.Json.JsonSerializerOptions) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 5,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.JsonSerializer::Deserialize{TValue}:TValue(System.Text.Json.JsonElement,System.Text.Json.JsonSerializerOptions)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [6] System.Text.Json/System.Text.Json.JsonSerializer::Deserialize:System.Object(System.Text.Json.JsonElement,System.Type,System.Text.Json.JsonSerializerOptions) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 6,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.JsonSerializer::Deserialize:System.Object(System.Text.Json.JsonElement,System.Type,System.Text.Json.JsonSerializerOptions)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [7] System.Text.Json/System.Text.Json.JsonSerializer::Deserialize{TValue}:TValue(System.Text.Json.JsonElement,System.Text.Json.Serialization.Metadata.JsonTypeInfo{TValue}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 7,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.JsonSerializer::Deserialize{TValue}:TValue(System.Text.Json.JsonElement,System.Text.Json.Serialization.Metadata.JsonTypeInfo{TValue})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [8] System.Text.Json/System.Text.Json.JsonSerializer::Deserialize:System.Object(System.Text.Json.JsonElement,System.Text.Json.Serialization.Metadata.JsonTypeInfo) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 8,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.JsonSerializer::Deserialize:System.Object(System.Text.Json.JsonElement,System.Text.Json.Serialization.Metadata.JsonTypeInfo)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [9] System.Text.Json/System.Text.Json.JsonSerializer::Deserialize:System.Object(System.Text.Json.JsonElement,System.Type,System.Text.Json.Serialization.JsonSerializerContext) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 9,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.JsonSerializer::Deserialize:System.Object(System.Text.Json.JsonElement,System.Type,System.Text.Json.Serialization.JsonSerializerContext)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [10] System.Text.Json/System.Text.Json.JsonSerializer::Deserialize{TValue}:TValue(System.Text.Json.Nodes.JsonNode,System.Text.Json.JsonSerializerOptions) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 10,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.JsonSerializer::Deserialize{TValue}:TValue(System.Text.Json.Nodes.JsonNode,System.Text.Json.JsonSerializerOptions)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [11] System.Text.Json/System.Text.Json.JsonSerializer::Deserialize:System.Object(System.Text.Json.Nodes.JsonNode,System.Type,System.Text.Json.JsonSerializerOptions) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 11,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.JsonSerializer::Deserialize:System.Object(System.Text.Json.Nodes.JsonNode,System.Type,System.Text.Json.JsonSerializerOptions)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [12] System.Text.Json/System.Text.Json.JsonSerializer::Deserialize{TValue}:TValue(System.Text.Json.Nodes.JsonNode,System.Text.Json.Serialization.Metadata.JsonTypeInfo{TValue}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 12,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.JsonSerializer::Deserialize{TValue}:TValue(System.Text.Json.Nodes.JsonNode,System.Text.Json.Serialization.Metadata.JsonTypeInfo{TValue})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [13] System.Text.Json/System.Text.Json.JsonSerializer::Deserialize:System.Object(System.Text.Json.Nodes.JsonNode,System.Text.Json.Serialization.Metadata.JsonTypeInfo) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 13,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.JsonSerializer::Deserialize:System.Object(System.Text.Json.Nodes.JsonNode,System.Text.Json.Serialization.Metadata.JsonTypeInfo)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [14] System.Text.Json/System.Text.Json.JsonSerializer::Deserialize:System.Object(System.Text.Json.Nodes.JsonNode,System.Type,System.Text.Json.Serialization.JsonSerializerContext) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 14,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.JsonSerializer::Deserialize:System.Object(System.Text.Json.Nodes.JsonNode,System.Type,System.Text.Json.Serialization.JsonSerializerContext)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [15] System.Text.Json/System.Text.Json.JsonSerializer::Deserialize{TValue}:TValue(System.ReadOnlySpan{System.Byte},System.Text.Json.JsonSerializerOptions) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 15,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.JsonSerializer::Deserialize{TValue}:TValue(System.ReadOnlySpan{System.Byte},System.Text.Json.JsonSerializerOptions)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [16] System.Text.Json/System.Text.Json.JsonSerializer::Deserialize:System.Object(System.ReadOnlySpan{System.Byte},System.Type,System.Text.Json.JsonSerializerOptions) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 16,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.JsonSerializer::Deserialize:System.Object(System.ReadOnlySpan{System.Byte},System.Type,System.Text.Json.JsonSerializerOptions)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [17] System.Text.Json/System.Text.Json.JsonSerializer::Deserialize{TValue}:TValue(System.ReadOnlySpan{System.Byte},System.Text.Json.Serialization.Metadata.JsonTypeInfo{TValue}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 17,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.JsonSerializer::Deserialize{TValue}:TValue(System.ReadOnlySpan{System.Byte},System.Text.Json.Serialization.Metadata.JsonTypeInfo{TValue})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [18] System.Text.Json/System.Text.Json.JsonSerializer::Deserialize:System.Object(System.ReadOnlySpan{System.Byte},System.Text.Json.Serialization.Metadata.JsonTypeInfo) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 18,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.JsonSerializer::Deserialize:System.Object(System.ReadOnlySpan{System.Byte},System.Text.Json.Serialization.Metadata.JsonTypeInfo)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [19] System.Text.Json/System.Text.Json.JsonSerializer::Deserialize:System.Object(System.ReadOnlySpan{System.Byte},System.Type,System.Text.Json.Serialization.JsonSerializerContext) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 19,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.JsonSerializer::Deserialize:System.Object(System.ReadOnlySpan{System.Byte},System.Type,System.Text.Json.Serialization.JsonSerializerContext)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [20] System.Text.Json/System.Text.Json.JsonSerializer::Deserialize{TValue}:TValue(System.IO.Stream,System.Text.Json.JsonSerializerOptions) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 20,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.JsonSerializer::Deserialize{TValue}:TValue(System.IO.Stream,System.Text.Json.JsonSerializerOptions)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [21] System.Text.Json/System.Text.Json.JsonSerializer::Deserialize:System.Object(System.IO.Stream,System.Type,System.Text.Json.JsonSerializerOptions) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 21,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.JsonSerializer::Deserialize:System.Object(System.IO.Stream,System.Type,System.Text.Json.JsonSerializerOptions)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [22] System.Text.Json/System.Text.Json.JsonSerializer::Deserialize{TValue}:TValue(System.IO.Stream,System.Text.Json.Serialization.Metadata.JsonTypeInfo{TValue}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 22,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.JsonSerializer::Deserialize{TValue}:TValue(System.IO.Stream,System.Text.Json.Serialization.Metadata.JsonTypeInfo{TValue})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [23] System.Text.Json/System.Text.Json.JsonSerializer::Deserialize:System.Object(System.IO.Stream,System.Text.Json.Serialization.Metadata.JsonTypeInfo) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 23,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.JsonSerializer::Deserialize:System.Object(System.IO.Stream,System.Text.Json.Serialization.Metadata.JsonTypeInfo)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [24] System.Text.Json/System.Text.Json.JsonSerializer::Deserialize:System.Object(System.IO.Stream,System.Type,System.Text.Json.Serialization.JsonSerializerContext) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 24,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.JsonSerializer::Deserialize:System.Object(System.IO.Stream,System.Type,System.Text.Json.Serialization.JsonSerializerContext)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [25] System.Text.Json/System.Text.Json.JsonSerializer::Deserialize{TValue}:TValue(System.String,System.Text.Json.JsonSerializerOptions) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 25,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.JsonSerializer::Deserialize{TValue}:TValue(System.String,System.Text.Json.JsonSerializerOptions)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [26] System.Text.Json/System.Text.Json.JsonSerializer::Deserialize{TValue}:TValue(System.ReadOnlySpan{System.Char},System.Text.Json.JsonSerializerOptions) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 26,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.JsonSerializer::Deserialize{TValue}:TValue(System.ReadOnlySpan{System.Char},System.Text.Json.JsonSerializerOptions)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [27] System.Text.Json/System.Text.Json.JsonSerializer::Deserialize:System.Object(System.String,System.Type,System.Text.Json.JsonSerializerOptions) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 27,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.JsonSerializer::Deserialize:System.Object(System.String,System.Type,System.Text.Json.JsonSerializerOptions)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [28] System.Text.Json/System.Text.Json.JsonSerializer::Deserialize:System.Object(System.ReadOnlySpan{System.Char},System.Type,System.Text.Json.JsonSerializerOptions) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 28,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.JsonSerializer::Deserialize:System.Object(System.ReadOnlySpan{System.Char},System.Type,System.Text.Json.JsonSerializerOptions)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [29] System.Text.Json/System.Text.Json.JsonSerializer::Deserialize{TValue}:TValue(System.String,System.Text.Json.Serialization.Metadata.JsonTypeInfo{TValue}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 29,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.JsonSerializer::Deserialize{TValue}:TValue(System.String,System.Text.Json.Serialization.Metadata.JsonTypeInfo{TValue})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [30] System.Text.Json/System.Text.Json.JsonSerializer::Deserialize{TValue}:TValue(System.ReadOnlySpan{System.Char},System.Text.Json.Serialization.Metadata.JsonTypeInfo{TValue}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 30,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.JsonSerializer::Deserialize{TValue}:TValue(System.ReadOnlySpan{System.Char},System.Text.Json.Serialization.Metadata.JsonTypeInfo{TValue})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [31] System.Text.Json/System.Text.Json.JsonSerializer::Deserialize:System.Object(System.String,System.Text.Json.Serialization.Metadata.JsonTypeInfo) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 31,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.JsonSerializer::Deserialize:System.Object(System.String,System.Text.Json.Serialization.Metadata.JsonTypeInfo)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [32] System.Text.Json/System.Text.Json.JsonSerializer::Deserialize:System.Object(System.ReadOnlySpan{System.Char},System.Text.Json.Serialization.Metadata.JsonTypeInfo) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 32,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.JsonSerializer::Deserialize:System.Object(System.ReadOnlySpan{System.Char},System.Text.Json.Serialization.Metadata.JsonTypeInfo)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [33] System.Text.Json/System.Text.Json.JsonSerializer::Deserialize:System.Object(System.String,System.Type,System.Text.Json.Serialization.JsonSerializerContext) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 33,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.JsonSerializer::Deserialize:System.Object(System.String,System.Type,System.Text.Json.Serialization.JsonSerializerContext)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [34] System.Text.Json/System.Text.Json.JsonSerializer::Deserialize:System.Object(System.ReadOnlySpan{System.Char},System.Type,System.Text.Json.Serialization.JsonSerializerContext) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 34,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.JsonSerializer::Deserialize:System.Object(System.ReadOnlySpan{System.Char},System.Type,System.Text.Json.Serialization.JsonSerializerContext)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [35] System.Text.Json/System.Text.Json.JsonSerializer::Deserialize{TValue}:TValue(System.Text.Json.Utf8JsonReader&,System.Text.Json.JsonSerializerOptions) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 35,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.JsonSerializer::Deserialize{TValue}:TValue(System.Text.Json.Utf8JsonReader&,System.Text.Json.JsonSerializerOptions)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [36] System.Text.Json/System.Text.Json.JsonSerializer::Deserialize:System.Object(System.Text.Json.Utf8JsonReader&,System.Type,System.Text.Json.JsonSerializerOptions) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 36,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.JsonSerializer::Deserialize:System.Object(System.Text.Json.Utf8JsonReader&,System.Type,System.Text.Json.JsonSerializerOptions)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [37] System.Text.Json/System.Text.Json.JsonSerializer::Deserialize{TValue}:TValue(System.Text.Json.Utf8JsonReader&,System.Text.Json.Serialization.Metadata.JsonTypeInfo{TValue}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 37,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.JsonSerializer::Deserialize{TValue}:TValue(System.Text.Json.Utf8JsonReader&,System.Text.Json.Serialization.Metadata.JsonTypeInfo{TValue})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [38] System.Text.Json/System.Text.Json.JsonSerializer::Deserialize:System.Object(System.Text.Json.Utf8JsonReader&,System.Text.Json.Serialization.Metadata.JsonTypeInfo) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 38,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.JsonSerializer::Deserialize:System.Object(System.Text.Json.Utf8JsonReader&,System.Text.Json.Serialization.Metadata.JsonTypeInfo)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [39] System.Text.Json/System.Text.Json.JsonSerializer::Deserialize:System.Object(System.Text.Json.Utf8JsonReader&,System.Type,System.Text.Json.Serialization.JsonSerializerContext) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 39,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.JsonSerializer::Deserialize:System.Object(System.Text.Json.Utf8JsonReader&,System.Type,System.Text.Json.Serialization.JsonSerializerContext)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [40] System.Text.Json/System.Text.Json.JsonSerializer::DeserializeAsync{TValue}:System.Threading.Tasks.ValueTask{TValue}(System.IO.Pipelines.PipeReader,System.Text.Json.JsonSerializerOptions,System.Threading.CancellationToken) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 40,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.JsonSerializer::DeserializeAsync{TValue}:System.Threading.Tasks.ValueTask{TValue}(System.IO.Pipelines.PipeReader,System.Text.Json.JsonSerializerOptions,System.Threading.CancellationToken)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [41] System.Text.Json/System.Text.Json.JsonSerializer::DeserializeAsync{TValue}:System.Threading.Tasks.ValueTask{TValue}(System.IO.Pipelines.PipeReader,System.Text.Json.Serialization.Metadata.JsonTypeInfo{TValue},System.Threading.CancellationToken) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 41,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.JsonSerializer::DeserializeAsync{TValue}:System.Threading.Tasks.ValueTask{TValue}(System.IO.Pipelines.PipeReader,System.Text.Json.Serialization.Metadata.JsonTypeInfo{TValue},System.Threading.CancellationToken)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [42] System.Text.Json/System.Text.Json.JsonSerializer::DeserializeAsync:System.Threading.Tasks.ValueTask{System.Object}(System.IO.Pipelines.PipeReader,System.Text.Json.Serialization.Metadata.JsonTypeInfo,System.Threading.CancellationToken) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 42,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.JsonSerializer::DeserializeAsync:System.Threading.Tasks.ValueTask{System.Object}(System.IO.Pipelines.PipeReader,System.Text.Json.Serialization.Metadata.JsonTypeInfo,System.Threading.CancellationToken)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [43] System.Text.Json/System.Text.Json.JsonSerializer::DeserializeAsync:System.Threading.Tasks.ValueTask{System.Object}(System.IO.Pipelines.PipeReader,System.Type,System.Text.Json.Serialization.JsonSerializerContext,System.Threading.CancellationToken) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 43,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.JsonSerializer::DeserializeAsync:System.Threading.Tasks.ValueTask{System.Object}(System.IO.Pipelines.PipeReader,System.Type,System.Text.Json.Serialization.JsonSerializerContext,System.Threading.CancellationToken)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [44] System.Text.Json/System.Text.Json.JsonSerializer::DeserializeAsync:System.Threading.Tasks.ValueTask{System.Object}(System.IO.Pipelines.PipeReader,System.Type,System.Text.Json.JsonSerializerOptions,System.Threading.CancellationToken) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 44,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.JsonSerializer::DeserializeAsync:System.Threading.Tasks.ValueTask{System.Object}(System.IO.Pipelines.PipeReader,System.Type,System.Text.Json.JsonSerializerOptions,System.Threading.CancellationToken)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [45] System.Text.Json/System.Text.Json.JsonSerializer::DeserializeAsync{TValue}:System.Threading.Tasks.ValueTask{TValue}(System.IO.Stream,System.Text.Json.JsonSerializerOptions,System.Threading.CancellationToken) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 45,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.JsonSerializer::DeserializeAsync{TValue}:System.Threading.Tasks.ValueTask{TValue}(System.IO.Stream,System.Text.Json.JsonSerializerOptions,System.Threading.CancellationToken)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [46] System.Text.Json/System.Text.Json.JsonSerializer::DeserializeAsync:System.Threading.Tasks.ValueTask{System.Object}(System.IO.Stream,System.Type,System.Text.Json.JsonSerializerOptions,System.Threading.CancellationToken) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 46,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.JsonSerializer::DeserializeAsync:System.Threading.Tasks.ValueTask{System.Object}(System.IO.Stream,System.Type,System.Text.Json.JsonSerializerOptions,System.Threading.CancellationToken)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [47] System.Text.Json/System.Text.Json.JsonSerializer::DeserializeAsync{TValue}:System.Threading.Tasks.ValueTask{TValue}(System.IO.Stream,System.Text.Json.Serialization.Metadata.JsonTypeInfo{TValue},System.Threading.CancellationToken) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 47,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.JsonSerializer::DeserializeAsync{TValue}:System.Threading.Tasks.ValueTask{TValue}(System.IO.Stream,System.Text.Json.Serialization.Metadata.JsonTypeInfo{TValue},System.Threading.CancellationToken)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [48] System.Text.Json/System.Text.Json.JsonSerializer::DeserializeAsync:System.Threading.Tasks.ValueTask{System.Object}(System.IO.Stream,System.Text.Json.Serialization.Metadata.JsonTypeInfo,System.Threading.CancellationToken) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 48,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.JsonSerializer::DeserializeAsync:System.Threading.Tasks.ValueTask{System.Object}(System.IO.Stream,System.Text.Json.Serialization.Metadata.JsonTypeInfo,System.Threading.CancellationToken)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [49] System.Text.Json/System.Text.Json.JsonSerializer::DeserializeAsync:System.Threading.Tasks.ValueTask{System.Object}(System.IO.Stream,System.Type,System.Text.Json.Serialization.JsonSerializerContext,System.Threading.CancellationToken) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 49,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.JsonSerializer::DeserializeAsync:System.Threading.Tasks.ValueTask{System.Object}(System.IO.Stream,System.Type,System.Text.Json.Serialization.JsonSerializerContext,System.Threading.CancellationToken)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [50] System.Text.Json/System.Text.Json.JsonSerializer::DeserializeAsyncEnumerable{TValue}:System.Collections.Generic.IAsyncEnumerable{TValue}(System.IO.Pipelines.PipeReader,System.Text.Json.JsonSerializerOptions,System.Threading.CancellationToken) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 50,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.JsonSerializer::DeserializeAsyncEnumerable{TValue}:System.Collections.Generic.IAsyncEnumerable{TValue}(System.IO.Pipelines.PipeReader,System.Text.Json.JsonSerializerOptions,System.Threading.CancellationToken)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [51] System.Text.Json/System.Text.Json.JsonSerializer::DeserializeAsyncEnumerable{TValue}:System.Collections.Generic.IAsyncEnumerable{TValue}(System.IO.Pipelines.PipeReader,System.Text.Json.Serialization.Metadata.JsonTypeInfo{TValue},System.Threading.CancellationToken) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 51,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.JsonSerializer::DeserializeAsyncEnumerable{TValue}:System.Collections.Generic.IAsyncEnumerable{TValue}(System.IO.Pipelines.PipeReader,System.Text.Json.Serialization.Metadata.JsonTypeInfo{TValue},System.Threading.CancellationToken)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [52] System.Text.Json/System.Text.Json.JsonSerializer::DeserializeAsyncEnumerable{TValue}:System.Collections.Generic.IAsyncEnumerable{TValue}(System.IO.Pipelines.PipeReader,System.Text.Json.Serialization.Metadata.JsonTypeInfo{TValue},System.Boolean,System.Threading.CancellationToken) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 52,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.JsonSerializer::DeserializeAsyncEnumerable{TValue}:System.Collections.Generic.IAsyncEnumerable{TValue}(System.IO.Pipelines.PipeReader,System.Text.Json.Serialization.Metadata.JsonTypeInfo{TValue},System.Boolean,System.Threading.CancellationToken)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [53] System.Text.Json/System.Text.Json.JsonSerializer::DeserializeAsyncEnumerable{TValue}:System.Collections.Generic.IAsyncEnumerable{TValue}(System.IO.Pipelines.PipeReader,System.Boolean,System.Text.Json.JsonSerializerOptions,System.Threading.CancellationToken) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 53,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.JsonSerializer::DeserializeAsyncEnumerable{TValue}:System.Collections.Generic.IAsyncEnumerable{TValue}(System.IO.Pipelines.PipeReader,System.Boolean,System.Text.Json.JsonSerializerOptions,System.Threading.CancellationToken)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [54] System.Text.Json/System.Text.Json.JsonSerializer::DeserializeAsyncEnumerable{TValue}:System.Collections.Generic.IAsyncEnumerable{TValue}(System.IO.Stream,System.Text.Json.JsonSerializerOptions,System.Threading.CancellationToken) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 54,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.JsonSerializer::DeserializeAsyncEnumerable{TValue}:System.Collections.Generic.IAsyncEnumerable{TValue}(System.IO.Stream,System.Text.Json.JsonSerializerOptions,System.Threading.CancellationToken)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [55] System.Text.Json/System.Text.Json.JsonSerializer::DeserializeAsyncEnumerable{TValue}:System.Collections.Generic.IAsyncEnumerable{TValue}(System.IO.Stream,System.Boolean,System.Text.Json.JsonSerializerOptions,System.Threading.CancellationToken) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 55,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.JsonSerializer::DeserializeAsyncEnumerable{TValue}:System.Collections.Generic.IAsyncEnumerable{TValue}(System.IO.Stream,System.Boolean,System.Text.Json.JsonSerializerOptions,System.Threading.CancellationToken)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [56] System.Text.Json/System.Text.Json.JsonSerializer::DeserializeAsyncEnumerable{TValue}:System.Collections.Generic.IAsyncEnumerable{TValue}(System.IO.Stream,System.Text.Json.Serialization.Metadata.JsonTypeInfo{TValue},System.Threading.CancellationToken) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 56,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.JsonSerializer::DeserializeAsyncEnumerable{TValue}:System.Collections.Generic.IAsyncEnumerable{TValue}(System.IO.Stream,System.Text.Json.Serialization.Metadata.JsonTypeInfo{TValue},System.Threading.CancellationToken)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [57] System.Text.Json/System.Text.Json.JsonSerializer::DeserializeAsyncEnumerable{TValue}:System.Collections.Generic.IAsyncEnumerable{TValue}(System.IO.Stream,System.Text.Json.Serialization.Metadata.JsonTypeInfo{TValue},System.Boolean,System.Threading.CancellationToken) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 57,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.JsonSerializer::DeserializeAsyncEnumerable{TValue}:System.Collections.Generic.IAsyncEnumerable{TValue}(System.IO.Stream,System.Text.Json.Serialization.Metadata.JsonTypeInfo{TValue},System.Boolean,System.Threading.CancellationToken)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [58] System.Text.Json/System.Text.Json.JsonSerializer::get_IsReflectionEnabledByDefault:System.Boolean() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 58,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.JsonSerializer::get_IsReflectionEnabledByDefault:System.Boolean()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [59] System.Text.Json/System.Text.Json.JsonSerializer::Serialize{TValue}:System.Void(System.IO.Stream,TValue,System.Text.Json.JsonSerializerOptions) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 59,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.JsonSerializer::Serialize{TValue}:System.Void(System.IO.Stream,TValue,System.Text.Json.JsonSerializerOptions)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [60] System.Text.Json/System.Text.Json.JsonSerializer::Serialize:System.Void(System.IO.Stream,System.Object,System.Type,System.Text.Json.JsonSerializerOptions) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 60,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.JsonSerializer::Serialize:System.Void(System.IO.Stream,System.Object,System.Type,System.Text.Json.JsonSerializerOptions)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [61] System.Text.Json/System.Text.Json.JsonSerializer::Serialize{TValue}:System.Void(System.IO.Stream,TValue,System.Text.Json.Serialization.Metadata.JsonTypeInfo{TValue}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 61,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.JsonSerializer::Serialize{TValue}:System.Void(System.IO.Stream,TValue,System.Text.Json.Serialization.Metadata.JsonTypeInfo{TValue})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [62] System.Text.Json/System.Text.Json.JsonSerializer::Serialize:System.Void(System.IO.Stream,System.Object,System.Text.Json.Serialization.Metadata.JsonTypeInfo) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 62,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.JsonSerializer::Serialize:System.Void(System.IO.Stream,System.Object,System.Text.Json.Serialization.Metadata.JsonTypeInfo)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [63] System.Text.Json/System.Text.Json.JsonSerializer::Serialize:System.Void(System.IO.Stream,System.Object,System.Type,System.Text.Json.Serialization.JsonSerializerContext) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 63,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.JsonSerializer::Serialize:System.Void(System.IO.Stream,System.Object,System.Type,System.Text.Json.Serialization.JsonSerializerContext)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [64] System.Text.Json/System.Text.Json.JsonSerializer::Serialize{TValue}:System.String(TValue,System.Text.Json.JsonSerializerOptions) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 64,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.JsonSerializer::Serialize{TValue}:System.String(TValue,System.Text.Json.JsonSerializerOptions)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [65] System.Text.Json/System.Text.Json.JsonSerializer::Serialize:System.String(System.Object,System.Type,System.Text.Json.JsonSerializerOptions) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 65,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.JsonSerializer::Serialize:System.String(System.Object,System.Type,System.Text.Json.JsonSerializerOptions)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [66] System.Text.Json/System.Text.Json.JsonSerializer::Serialize{TValue}:System.String(TValue,System.Text.Json.Serialization.Metadata.JsonTypeInfo{TValue}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 66,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.JsonSerializer::Serialize{TValue}:System.String(TValue,System.Text.Json.Serialization.Metadata.JsonTypeInfo{TValue})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [67] System.Text.Json/System.Text.Json.JsonSerializer::Serialize:System.String(System.Object,System.Text.Json.Serialization.Metadata.JsonTypeInfo) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 67,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.JsonSerializer::Serialize:System.String(System.Object,System.Text.Json.Serialization.Metadata.JsonTypeInfo)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [68] System.Text.Json/System.Text.Json.JsonSerializer::Serialize:System.String(System.Object,System.Type,System.Text.Json.Serialization.JsonSerializerContext) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 68,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.JsonSerializer::Serialize:System.String(System.Object,System.Type,System.Text.Json.Serialization.JsonSerializerContext)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [69] System.Text.Json/System.Text.Json.JsonSerializer::Serialize{TValue}:System.Void(System.Text.Json.Utf8JsonWriter,TValue,System.Text.Json.JsonSerializerOptions) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 69,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.JsonSerializer::Serialize{TValue}:System.Void(System.Text.Json.Utf8JsonWriter,TValue,System.Text.Json.JsonSerializerOptions)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [70] System.Text.Json/System.Text.Json.JsonSerializer::Serialize:System.Void(System.Text.Json.Utf8JsonWriter,System.Object,System.Type,System.Text.Json.JsonSerializerOptions) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 70,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.JsonSerializer::Serialize:System.Void(System.Text.Json.Utf8JsonWriter,System.Object,System.Type,System.Text.Json.JsonSerializerOptions)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [71] System.Text.Json/System.Text.Json.JsonSerializer::Serialize{TValue}:System.Void(System.Text.Json.Utf8JsonWriter,TValue,System.Text.Json.Serialization.Metadata.JsonTypeInfo{TValue}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 71,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.JsonSerializer::Serialize{TValue}:System.Void(System.Text.Json.Utf8JsonWriter,TValue,System.Text.Json.Serialization.Metadata.JsonTypeInfo{TValue})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [72] System.Text.Json/System.Text.Json.JsonSerializer::Serialize:System.Void(System.Text.Json.Utf8JsonWriter,System.Object,System.Text.Json.Serialization.Metadata.JsonTypeInfo) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 72,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.JsonSerializer::Serialize:System.Void(System.Text.Json.Utf8JsonWriter,System.Object,System.Text.Json.Serialization.Metadata.JsonTypeInfo)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [73] System.Text.Json/System.Text.Json.JsonSerializer::Serialize:System.Void(System.Text.Json.Utf8JsonWriter,System.Object,System.Type,System.Text.Json.Serialization.JsonSerializerContext) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 73,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.JsonSerializer::Serialize:System.Void(System.Text.Json.Utf8JsonWriter,System.Object,System.Type,System.Text.Json.Serialization.JsonSerializerContext)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [74] System.Text.Json/System.Text.Json.JsonSerializer::SerializeAsync{TValue}:System.Threading.Tasks.Task(System.IO.Stream,TValue,System.Text.Json.JsonSerializerOptions,System.Threading.CancellationToken) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 74,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.JsonSerializer::SerializeAsync{TValue}:System.Threading.Tasks.Task(System.IO.Stream,TValue,System.Text.Json.JsonSerializerOptions,System.Threading.CancellationToken)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [75] System.Text.Json/System.Text.Json.JsonSerializer::SerializeAsync:System.Threading.Tasks.Task(System.IO.Stream,System.Object,System.Type,System.Text.Json.JsonSerializerOptions,System.Threading.CancellationToken) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 75,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.JsonSerializer::SerializeAsync:System.Threading.Tasks.Task(System.IO.Stream,System.Object,System.Type,System.Text.Json.JsonSerializerOptions,System.Threading.CancellationToken)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [76] System.Text.Json/System.Text.Json.JsonSerializer::SerializeAsync{TValue}:System.Threading.Tasks.Task(System.IO.Stream,TValue,System.Text.Json.Serialization.Metadata.JsonTypeInfo{TValue},System.Threading.CancellationToken) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 76,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.JsonSerializer::SerializeAsync{TValue}:System.Threading.Tasks.Task(System.IO.Stream,TValue,System.Text.Json.Serialization.Metadata.JsonTypeInfo{TValue},System.Threading.CancellationToken)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [77] System.Text.Json/System.Text.Json.JsonSerializer::SerializeAsync:System.Threading.Tasks.Task(System.IO.Stream,System.Object,System.Text.Json.Serialization.Metadata.JsonTypeInfo,System.Threading.CancellationToken) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 77,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.JsonSerializer::SerializeAsync:System.Threading.Tasks.Task(System.IO.Stream,System.Object,System.Text.Json.Serialization.Metadata.JsonTypeInfo,System.Threading.CancellationToken)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [78] System.Text.Json/System.Text.Json.JsonSerializer::SerializeAsync:System.Threading.Tasks.Task(System.IO.Stream,System.Object,System.Type,System.Text.Json.Serialization.JsonSerializerContext,System.Threading.CancellationToken) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 78,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.JsonSerializer::SerializeAsync:System.Threading.Tasks.Task(System.IO.Stream,System.Object,System.Type,System.Text.Json.Serialization.JsonSerializerContext,System.Threading.CancellationToken)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [79] System.Text.Json/System.Text.Json.JsonSerializer::SerializeAsync{TValue}:System.Threading.Tasks.Task(System.IO.Pipelines.PipeWriter,TValue,System.Text.Json.Serialization.Metadata.JsonTypeInfo{TValue},System.Threading.CancellationToken) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 79,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.JsonSerializer::SerializeAsync{TValue}:System.Threading.Tasks.Task(System.IO.Pipelines.PipeWriter,TValue,System.Text.Json.Serialization.Metadata.JsonTypeInfo{TValue},System.Threading.CancellationToken)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [80] System.Text.Json/System.Text.Json.JsonSerializer::SerializeAsync{TValue}:System.Threading.Tasks.Task(System.IO.Pipelines.PipeWriter,TValue,System.Text.Json.JsonSerializerOptions,System.Threading.CancellationToken) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 80,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.JsonSerializer::SerializeAsync{TValue}:System.Threading.Tasks.Task(System.IO.Pipelines.PipeWriter,TValue,System.Text.Json.JsonSerializerOptions,System.Threading.CancellationToken)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [81] System.Text.Json/System.Text.Json.JsonSerializer::SerializeAsync:System.Threading.Tasks.Task(System.IO.Pipelines.PipeWriter,System.Object,System.Text.Json.Serialization.Metadata.JsonTypeInfo,System.Threading.CancellationToken) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 81,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.JsonSerializer::SerializeAsync:System.Threading.Tasks.Task(System.IO.Pipelines.PipeWriter,System.Object,System.Text.Json.Serialization.Metadata.JsonTypeInfo,System.Threading.CancellationToken)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [82] System.Text.Json/System.Text.Json.JsonSerializer::SerializeAsync:System.Threading.Tasks.Task(System.IO.Pipelines.PipeWriter,System.Object,System.Type,System.Text.Json.Serialization.JsonSerializerContext,System.Threading.CancellationToken) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 82,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.JsonSerializer::SerializeAsync:System.Threading.Tasks.Task(System.IO.Pipelines.PipeWriter,System.Object,System.Type,System.Text.Json.Serialization.JsonSerializerContext,System.Threading.CancellationToken)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [83] System.Text.Json/System.Text.Json.JsonSerializer::SerializeAsync:System.Threading.Tasks.Task(System.IO.Pipelines.PipeWriter,System.Object,System.Type,System.Text.Json.JsonSerializerOptions,System.Threading.CancellationToken) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 83,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.JsonSerializer::SerializeAsync:System.Threading.Tasks.Task(System.IO.Pipelines.PipeWriter,System.Object,System.Type,System.Text.Json.JsonSerializerOptions,System.Threading.CancellationToken)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [84] System.Text.Json/System.Text.Json.JsonSerializer::SerializeToDocument{TValue}:System.Text.Json.JsonDocument(TValue,System.Text.Json.JsonSerializerOptions) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 84,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.JsonSerializer::SerializeToDocument{TValue}:System.Text.Json.JsonDocument(TValue,System.Text.Json.JsonSerializerOptions)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [85] System.Text.Json/System.Text.Json.JsonSerializer::SerializeToDocument:System.Text.Json.JsonDocument(System.Object,System.Type,System.Text.Json.JsonSerializerOptions) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 85,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.JsonSerializer::SerializeToDocument:System.Text.Json.JsonDocument(System.Object,System.Type,System.Text.Json.JsonSerializerOptions)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [86] System.Text.Json/System.Text.Json.JsonSerializer::SerializeToDocument{TValue}:System.Text.Json.JsonDocument(TValue,System.Text.Json.Serialization.Metadata.JsonTypeInfo{TValue}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 86,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.JsonSerializer::SerializeToDocument{TValue}:System.Text.Json.JsonDocument(TValue,System.Text.Json.Serialization.Metadata.JsonTypeInfo{TValue})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [87] System.Text.Json/System.Text.Json.JsonSerializer::SerializeToDocument:System.Text.Json.JsonDocument(System.Object,System.Text.Json.Serialization.Metadata.JsonTypeInfo) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 87,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.JsonSerializer::SerializeToDocument:System.Text.Json.JsonDocument(System.Object,System.Text.Json.Serialization.Metadata.JsonTypeInfo)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [88] System.Text.Json/System.Text.Json.JsonSerializer::SerializeToDocument:System.Text.Json.JsonDocument(System.Object,System.Type,System.Text.Json.Serialization.JsonSerializerContext) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 88,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.JsonSerializer::SerializeToDocument:System.Text.Json.JsonDocument(System.Object,System.Type,System.Text.Json.Serialization.JsonSerializerContext)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [89] System.Text.Json/System.Text.Json.JsonSerializer::SerializeToElement{TValue}:System.Text.Json.JsonElement(TValue,System.Text.Json.JsonSerializerOptions) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 89,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.JsonSerializer::SerializeToElement{TValue}:System.Text.Json.JsonElement(TValue,System.Text.Json.JsonSerializerOptions)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [90] System.Text.Json/System.Text.Json.JsonSerializer::SerializeToElement:System.Text.Json.JsonElement(System.Object,System.Type,System.Text.Json.JsonSerializerOptions) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 90,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.JsonSerializer::SerializeToElement:System.Text.Json.JsonElement(System.Object,System.Type,System.Text.Json.JsonSerializerOptions)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [91] System.Text.Json/System.Text.Json.JsonSerializer::SerializeToElement{TValue}:System.Text.Json.JsonElement(TValue,System.Text.Json.Serialization.Metadata.JsonTypeInfo{TValue}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 91,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.JsonSerializer::SerializeToElement{TValue}:System.Text.Json.JsonElement(TValue,System.Text.Json.Serialization.Metadata.JsonTypeInfo{TValue})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [92] System.Text.Json/System.Text.Json.JsonSerializer::SerializeToElement:System.Text.Json.JsonElement(System.Object,System.Text.Json.Serialization.Metadata.JsonTypeInfo) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 92,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.JsonSerializer::SerializeToElement:System.Text.Json.JsonElement(System.Object,System.Text.Json.Serialization.Metadata.JsonTypeInfo)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [93] System.Text.Json/System.Text.Json.JsonSerializer::SerializeToElement:System.Text.Json.JsonElement(System.Object,System.Type,System.Text.Json.Serialization.JsonSerializerContext) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 93,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.JsonSerializer::SerializeToElement:System.Text.Json.JsonElement(System.Object,System.Type,System.Text.Json.Serialization.JsonSerializerContext)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [94] System.Text.Json/System.Text.Json.JsonSerializer::SerializeToNode{TValue}:System.Text.Json.Nodes.JsonNode(TValue,System.Text.Json.JsonSerializerOptions) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 94,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.JsonSerializer::SerializeToNode{TValue}:System.Text.Json.Nodes.JsonNode(TValue,System.Text.Json.JsonSerializerOptions)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [95] System.Text.Json/System.Text.Json.JsonSerializer::SerializeToNode:System.Text.Json.Nodes.JsonNode(System.Object,System.Type,System.Text.Json.JsonSerializerOptions) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 95,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.JsonSerializer::SerializeToNode:System.Text.Json.Nodes.JsonNode(System.Object,System.Type,System.Text.Json.JsonSerializerOptions)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [96] System.Text.Json/System.Text.Json.JsonSerializer::SerializeToNode{TValue}:System.Text.Json.Nodes.JsonNode(TValue,System.Text.Json.Serialization.Metadata.JsonTypeInfo{TValue}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 96,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.JsonSerializer::SerializeToNode{TValue}:System.Text.Json.Nodes.JsonNode(TValue,System.Text.Json.Serialization.Metadata.JsonTypeInfo{TValue})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [97] System.Text.Json/System.Text.Json.JsonSerializer::SerializeToNode:System.Text.Json.Nodes.JsonNode(System.Object,System.Text.Json.Serialization.Metadata.JsonTypeInfo) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 97,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.JsonSerializer::SerializeToNode:System.Text.Json.Nodes.JsonNode(System.Object,System.Text.Json.Serialization.Metadata.JsonTypeInfo)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [98] System.Text.Json/System.Text.Json.JsonSerializer::SerializeToNode:System.Text.Json.Nodes.JsonNode(System.Object,System.Type,System.Text.Json.Serialization.JsonSerializerContext) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 98,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.JsonSerializer::SerializeToNode:System.Text.Json.Nodes.JsonNode(System.Object,System.Type,System.Text.Json.Serialization.JsonSerializerContext)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [99] System.Text.Json/System.Text.Json.JsonSerializer::SerializeToUtf8Bytes{TValue}:System.Byte{}(TValue,System.Text.Json.JsonSerializerOptions) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 99,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.JsonSerializer::SerializeToUtf8Bytes{TValue}:System.Byte{}(TValue,System.Text.Json.JsonSerializerOptions)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [100] System.Text.Json/System.Text.Json.JsonSerializer::SerializeToUtf8Bytes:System.Byte{}(System.Object,System.Type,System.Text.Json.JsonSerializerOptions) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 100,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.JsonSerializer::SerializeToUtf8Bytes:System.Byte{}(System.Object,System.Type,System.Text.Json.JsonSerializerOptions)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [101] System.Text.Json/System.Text.Json.JsonSerializer::SerializeToUtf8Bytes{TValue}:System.Byte{}(TValue,System.Text.Json.Serialization.Metadata.JsonTypeInfo{TValue}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 101,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.JsonSerializer::SerializeToUtf8Bytes{TValue}:System.Byte{}(TValue,System.Text.Json.Serialization.Metadata.JsonTypeInfo{TValue})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [102] System.Text.Json/System.Text.Json.JsonSerializer::SerializeToUtf8Bytes:System.Byte{}(System.Object,System.Text.Json.Serialization.Metadata.JsonTypeInfo) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 102,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.JsonSerializer::SerializeToUtf8Bytes:System.Byte{}(System.Object,System.Text.Json.Serialization.Metadata.JsonTypeInfo)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [103] System.Text.Json/System.Text.Json.JsonSerializer::SerializeToUtf8Bytes:System.Byte{}(System.Object,System.Type,System.Text.Json.Serialization.JsonSerializerContext) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 103,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.JsonSerializer::SerializeToUtf8Bytes:System.Byte{}(System.Object,System.Type,System.Text.Json.Serialization.JsonSerializerContext)",
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
