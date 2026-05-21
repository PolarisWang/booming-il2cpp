// Auto-generated managed benchmark harness
// Family: nodes, Assembly: System.Text.Json
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
            { // [0] System.Text.Json/System.Text.Json.Nodes.JsonArray::Add{T}:System.Void(T) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 0,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.Nodes.JsonArray::Add{T}:System.Void(T)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [1] System.Text.Json/System.Text.Json.Nodes.JsonArray::Add:System.Void(System.Text.Json.Nodes.JsonNode) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 1,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.Nodes.JsonArray::Add:System.Void(System.Text.Json.Nodes.JsonNode)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [2] System.Text.Json/System.Text.Json.Nodes.JsonArray::Clear:System.Void() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 2,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.Nodes.JsonArray::Clear:System.Void()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [3] System.Text.Json/System.Text.Json.Nodes.JsonArray::Contains:System.Boolean(System.Text.Json.Nodes.JsonNode) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 3,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.Nodes.JsonArray::Contains:System.Boolean(System.Text.Json.Nodes.JsonNode)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [4] System.Text.Json/System.Text.Json.Nodes.JsonArray::Create:System.Text.Json.Nodes.JsonArray(System.Text.Json.JsonElement,System.Nullable{System.Text.Json.Nodes.JsonNodeOptions}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 4,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.Nodes.JsonArray::Create:System.Text.Json.Nodes.JsonArray(System.Text.Json.JsonElement,System.Nullable{System.Text.Json.Nodes.JsonNodeOptions})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [5] System.Text.Json/System.Text.Json.Nodes.JsonArray::get_Count:System.Int32() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 5,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.Nodes.JsonArray::get_Count:System.Int32()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [6] System.Text.Json/System.Text.Json.Nodes.JsonArray::GetEnumerator:System.Collections.Generic.IEnumerator{System.Text.Json.Nodes.JsonNode}() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 6,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.Nodes.JsonArray::GetEnumerator:System.Collections.Generic.IEnumerator{System.Text.Json.Nodes.JsonNode}()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [7] System.Text.Json/System.Text.Json.Nodes.JsonArray::GetValues{T}:System.Collections.Generic.IEnumerable{T}() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 7,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.Nodes.JsonArray::GetValues{T}:System.Collections.Generic.IEnumerable{T}()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [8] System.Text.Json/System.Text.Json.Nodes.JsonArray::IndexOf:System.Int32(System.Text.Json.Nodes.JsonNode) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 8,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.Nodes.JsonArray::IndexOf:System.Int32(System.Text.Json.Nodes.JsonNode)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [9] System.Text.Json/System.Text.Json.Nodes.JsonArray::Insert:System.Void(System.Int32,System.Text.Json.Nodes.JsonNode) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 9,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.Nodes.JsonArray::Insert:System.Void(System.Int32,System.Text.Json.Nodes.JsonNode)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [10] System.Text.Json/System.Text.Json.Nodes.JsonArray::Remove:System.Boolean(System.Text.Json.Nodes.JsonNode) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 10,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.Nodes.JsonArray::Remove:System.Boolean(System.Text.Json.Nodes.JsonNode)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [11] System.Text.Json/System.Text.Json.Nodes.JsonArray::RemoveAll:System.Int32(System.Func{System.Text.Json.Nodes.JsonNode,System.Boolean}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 11,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.Nodes.JsonArray::RemoveAll:System.Int32(System.Func{System.Text.Json.Nodes.JsonNode,System.Boolean})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [12] System.Text.Json/System.Text.Json.Nodes.JsonArray::RemoveAt:System.Void(System.Int32) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 12,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.Nodes.JsonArray::RemoveAt:System.Void(System.Int32)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [13] System.Text.Json/System.Text.Json.Nodes.JsonArray::RemoveRange:System.Void(System.Int32,System.Int32) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 13,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.Nodes.JsonArray::RemoveRange:System.Void(System.Int32,System.Int32)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [14] System.Text.Json/System.Text.Json.Nodes.JsonArray::WriteTo:System.Void(System.Text.Json.Utf8JsonWriter,System.Text.Json.JsonSerializerOptions) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 14,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.Nodes.JsonArray::WriteTo:System.Void(System.Text.Json.Utf8JsonWriter,System.Text.Json.JsonSerializerOptions)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [15] System.Text.Json/System.Text.Json.Nodes.JsonArray::.ctor:System.Void(System.Nullable{System.Text.Json.Nodes.JsonNodeOptions}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 15,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.Nodes.JsonArray::.ctor:System.Void(System.Nullable{System.Text.Json.Nodes.JsonNodeOptions})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [16] System.Text.Json/System.Text.Json.Nodes.JsonArray::.ctor:System.Void(System.Text.Json.Nodes.JsonNodeOptions,System.Text.Json.Nodes.JsonNode{}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 16,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.Nodes.JsonArray::.ctor:System.Void(System.Text.Json.Nodes.JsonNodeOptions,System.Text.Json.Nodes.JsonNode{})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [17] System.Text.Json/System.Text.Json.Nodes.JsonArray::.ctor:System.Void(System.Text.Json.Nodes.JsonNodeOptions,System.ReadOnlySpan{System.Text.Json.Nodes.JsonNode}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 17,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.Nodes.JsonArray::.ctor:System.Void(System.Text.Json.Nodes.JsonNodeOptions,System.ReadOnlySpan{System.Text.Json.Nodes.JsonNode})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [18] System.Text.Json/System.Text.Json.Nodes.JsonArray::.ctor:System.Void(System.Text.Json.Nodes.JsonNode{}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 18,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.Nodes.JsonArray::.ctor:System.Void(System.Text.Json.Nodes.JsonNode{})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [19] System.Text.Json/System.Text.Json.Nodes.JsonArray::.ctor:System.Void(System.ReadOnlySpan{System.Text.Json.Nodes.JsonNode}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 19,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.Nodes.JsonArray::.ctor:System.Void(System.ReadOnlySpan{System.Text.Json.Nodes.JsonNode})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [20] System.Text.Json/System.Text.Json.Nodes.JsonNode::AsArray:System.Text.Json.Nodes.JsonArray() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 20,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.Nodes.JsonNode::AsArray:System.Text.Json.Nodes.JsonArray()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [21] System.Text.Json/System.Text.Json.Nodes.JsonNode::AsObject:System.Text.Json.Nodes.JsonObject() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 21,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.Nodes.JsonNode::AsObject:System.Text.Json.Nodes.JsonObject()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [22] System.Text.Json/System.Text.Json.Nodes.JsonNode::AsValue:System.Text.Json.Nodes.JsonValue() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 22,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.Nodes.JsonNode::AsValue:System.Text.Json.Nodes.JsonValue()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [23] System.Text.Json/System.Text.Json.Nodes.JsonNode::DeepClone:System.Text.Json.Nodes.JsonNode() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 23,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.Nodes.JsonNode::DeepClone:System.Text.Json.Nodes.JsonNode()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [24] System.Text.Json/System.Text.Json.Nodes.JsonNode::DeepEquals:System.Boolean(System.Text.Json.Nodes.JsonNode,System.Text.Json.Nodes.JsonNode) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 24,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.Nodes.JsonNode::DeepEquals:System.Boolean(System.Text.Json.Nodes.JsonNode,System.Text.Json.Nodes.JsonNode)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [25] System.Text.Json/System.Text.Json.Nodes.JsonNode::get_Item:System.Text.Json.Nodes.JsonNode(System.Int32) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 25,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.Nodes.JsonNode::get_Item:System.Text.Json.Nodes.JsonNode(System.Int32)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [26] System.Text.Json/System.Text.Json.Nodes.JsonNode::get_Item:System.Text.Json.Nodes.JsonNode(System.String) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 26,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.Nodes.JsonNode::get_Item:System.Text.Json.Nodes.JsonNode(System.String)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [27] System.Text.Json/System.Text.Json.Nodes.JsonNode::get_Options:System.Nullable{System.Text.Json.Nodes.JsonNodeOptions}() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 27,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.Nodes.JsonNode::get_Options:System.Nullable{System.Text.Json.Nodes.JsonNodeOptions}()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [28] System.Text.Json/System.Text.Json.Nodes.JsonNode::get_Parent:System.Text.Json.Nodes.JsonNode() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 28,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.Nodes.JsonNode::get_Parent:System.Text.Json.Nodes.JsonNode()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [29] System.Text.Json/System.Text.Json.Nodes.JsonNode::get_Root:System.Text.Json.Nodes.JsonNode() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 29,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.Nodes.JsonNode::get_Root:System.Text.Json.Nodes.JsonNode()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [30] System.Text.Json/System.Text.Json.Nodes.JsonNode::GetElementIndex:System.Int32() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 30,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.Nodes.JsonNode::GetElementIndex:System.Int32()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [31] System.Text.Json/System.Text.Json.Nodes.JsonNode::GetPath:System.String() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 31,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.Nodes.JsonNode::GetPath:System.String()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [32] System.Text.Json/System.Text.Json.Nodes.JsonNode::GetPropertyName:System.String() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 32,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.Nodes.JsonNode::GetPropertyName:System.String()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [33] System.Text.Json/System.Text.Json.Nodes.JsonNode::GetValue{T}:T() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 33,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.Nodes.JsonNode::GetValue{T}:T()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [34] System.Text.Json/System.Text.Json.Nodes.JsonNode::GetValueKind:System.Text.Json.JsonValueKind() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 34,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.Nodes.JsonNode::GetValueKind:System.Text.Json.JsonValueKind()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [35] System.Text.Json/System.Text.Json.Nodes.JsonNode::op_Explicit:System.Boolean(System.Text.Json.Nodes.JsonNode) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 35,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.Nodes.JsonNode::op_Explicit:System.Boolean(System.Text.Json.Nodes.JsonNode)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [36] System.Text.Json/System.Text.Json.Nodes.JsonNode::op_Explicit:System.Nullable{System.Boolean}(System.Text.Json.Nodes.JsonNode) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 36,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.Nodes.JsonNode::op_Explicit:System.Nullable{System.Boolean}(System.Text.Json.Nodes.JsonNode)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [37] System.Text.Json/System.Text.Json.Nodes.JsonNode::op_Explicit:System.Byte(System.Text.Json.Nodes.JsonNode) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 37,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.Nodes.JsonNode::op_Explicit:System.Byte(System.Text.Json.Nodes.JsonNode)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [38] System.Text.Json/System.Text.Json.Nodes.JsonNode::op_Explicit:System.Nullable{System.Byte}(System.Text.Json.Nodes.JsonNode) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 38,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.Nodes.JsonNode::op_Explicit:System.Nullable{System.Byte}(System.Text.Json.Nodes.JsonNode)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [39] System.Text.Json/System.Text.Json.Nodes.JsonNode::op_Explicit:System.Char(System.Text.Json.Nodes.JsonNode) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 39,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.Nodes.JsonNode::op_Explicit:System.Char(System.Text.Json.Nodes.JsonNode)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [40] System.Text.Json/System.Text.Json.Nodes.JsonNode::op_Explicit:System.Nullable{System.Char}(System.Text.Json.Nodes.JsonNode) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 40,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.Nodes.JsonNode::op_Explicit:System.Nullable{System.Char}(System.Text.Json.Nodes.JsonNode)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [41] System.Text.Json/System.Text.Json.Nodes.JsonNode::op_Explicit:System.DateTime(System.Text.Json.Nodes.JsonNode) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 41,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.Nodes.JsonNode::op_Explicit:System.DateTime(System.Text.Json.Nodes.JsonNode)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [42] System.Text.Json/System.Text.Json.Nodes.JsonNode::op_Explicit:System.Nullable{System.DateTime}(System.Text.Json.Nodes.JsonNode) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 42,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.Nodes.JsonNode::op_Explicit:System.Nullable{System.DateTime}(System.Text.Json.Nodes.JsonNode)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [43] System.Text.Json/System.Text.Json.Nodes.JsonNode::op_Explicit:System.DateTimeOffset(System.Text.Json.Nodes.JsonNode) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 43,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.Nodes.JsonNode::op_Explicit:System.DateTimeOffset(System.Text.Json.Nodes.JsonNode)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [44] System.Text.Json/System.Text.Json.Nodes.JsonNode::op_Explicit:System.Nullable{System.DateTimeOffset}(System.Text.Json.Nodes.JsonNode) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 44,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.Nodes.JsonNode::op_Explicit:System.Nullable{System.DateTimeOffset}(System.Text.Json.Nodes.JsonNode)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [45] System.Text.Json/System.Text.Json.Nodes.JsonNode::op_Explicit:System.Decimal(System.Text.Json.Nodes.JsonNode) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 45,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.Nodes.JsonNode::op_Explicit:System.Decimal(System.Text.Json.Nodes.JsonNode)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [46] System.Text.Json/System.Text.Json.Nodes.JsonNode::op_Explicit:System.Nullable{System.Decimal}(System.Text.Json.Nodes.JsonNode) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 46,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.Nodes.JsonNode::op_Explicit:System.Nullable{System.Decimal}(System.Text.Json.Nodes.JsonNode)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [47] System.Text.Json/System.Text.Json.Nodes.JsonNode::op_Explicit:System.Double(System.Text.Json.Nodes.JsonNode) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 47,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.Nodes.JsonNode::op_Explicit:System.Double(System.Text.Json.Nodes.JsonNode)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [48] System.Text.Json/System.Text.Json.Nodes.JsonNode::op_Explicit:System.Nullable{System.Double}(System.Text.Json.Nodes.JsonNode) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 48,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.Nodes.JsonNode::op_Explicit:System.Nullable{System.Double}(System.Text.Json.Nodes.JsonNode)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [49] System.Text.Json/System.Text.Json.Nodes.JsonNode::op_Explicit:System.Guid(System.Text.Json.Nodes.JsonNode) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 49,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.Nodes.JsonNode::op_Explicit:System.Guid(System.Text.Json.Nodes.JsonNode)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [50] System.Text.Json/System.Text.Json.Nodes.JsonNode::op_Explicit:System.Nullable{System.Guid}(System.Text.Json.Nodes.JsonNode) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 50,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.Nodes.JsonNode::op_Explicit:System.Nullable{System.Guid}(System.Text.Json.Nodes.JsonNode)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [51] System.Text.Json/System.Text.Json.Nodes.JsonNode::op_Explicit:System.Int16(System.Text.Json.Nodes.JsonNode) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 51,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.Nodes.JsonNode::op_Explicit:System.Int16(System.Text.Json.Nodes.JsonNode)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [52] System.Text.Json/System.Text.Json.Nodes.JsonNode::op_Explicit:System.Nullable{System.Int16}(System.Text.Json.Nodes.JsonNode) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 52,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.Nodes.JsonNode::op_Explicit:System.Nullable{System.Int16}(System.Text.Json.Nodes.JsonNode)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [53] System.Text.Json/System.Text.Json.Nodes.JsonNode::op_Explicit:System.Int32(System.Text.Json.Nodes.JsonNode) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 53,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.Nodes.JsonNode::op_Explicit:System.Int32(System.Text.Json.Nodes.JsonNode)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [54] System.Text.Json/System.Text.Json.Nodes.JsonNode::op_Explicit:System.Nullable{System.Int32}(System.Text.Json.Nodes.JsonNode) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 54,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.Nodes.JsonNode::op_Explicit:System.Nullable{System.Int32}(System.Text.Json.Nodes.JsonNode)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [55] System.Text.Json/System.Text.Json.Nodes.JsonNode::op_Explicit:System.Int64(System.Text.Json.Nodes.JsonNode) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 55,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.Nodes.JsonNode::op_Explicit:System.Int64(System.Text.Json.Nodes.JsonNode)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [56] System.Text.Json/System.Text.Json.Nodes.JsonNode::op_Explicit:System.Nullable{System.Int64}(System.Text.Json.Nodes.JsonNode) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 56,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.Nodes.JsonNode::op_Explicit:System.Nullable{System.Int64}(System.Text.Json.Nodes.JsonNode)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [57] System.Text.Json/System.Text.Json.Nodes.JsonNode::op_Explicit:System.SByte(System.Text.Json.Nodes.JsonNode) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 57,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.Nodes.JsonNode::op_Explicit:System.SByte(System.Text.Json.Nodes.JsonNode)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [58] System.Text.Json/System.Text.Json.Nodes.JsonNode::op_Explicit:System.Nullable{System.SByte}(System.Text.Json.Nodes.JsonNode) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 58,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.Nodes.JsonNode::op_Explicit:System.Nullable{System.SByte}(System.Text.Json.Nodes.JsonNode)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [59] System.Text.Json/System.Text.Json.Nodes.JsonNode::op_Explicit:System.Single(System.Text.Json.Nodes.JsonNode) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 59,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.Nodes.JsonNode::op_Explicit:System.Single(System.Text.Json.Nodes.JsonNode)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [60] System.Text.Json/System.Text.Json.Nodes.JsonNode::op_Explicit:System.Nullable{System.Single}(System.Text.Json.Nodes.JsonNode) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 60,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.Nodes.JsonNode::op_Explicit:System.Nullable{System.Single}(System.Text.Json.Nodes.JsonNode)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [61] System.Text.Json/System.Text.Json.Nodes.JsonNode::op_Explicit:System.String(System.Text.Json.Nodes.JsonNode) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 61,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.Nodes.JsonNode::op_Explicit:System.String(System.Text.Json.Nodes.JsonNode)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [62] System.Text.Json/System.Text.Json.Nodes.JsonNode::op_Explicit:System.UInt16(System.Text.Json.Nodes.JsonNode) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 62,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.Nodes.JsonNode::op_Explicit:System.UInt16(System.Text.Json.Nodes.JsonNode)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [63] System.Text.Json/System.Text.Json.Nodes.JsonNode::op_Explicit:System.Nullable{System.UInt16}(System.Text.Json.Nodes.JsonNode) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 63,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.Nodes.JsonNode::op_Explicit:System.Nullable{System.UInt16}(System.Text.Json.Nodes.JsonNode)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [64] System.Text.Json/System.Text.Json.Nodes.JsonNode::op_Explicit:System.UInt32(System.Text.Json.Nodes.JsonNode) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 64,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.Nodes.JsonNode::op_Explicit:System.UInt32(System.Text.Json.Nodes.JsonNode)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [65] System.Text.Json/System.Text.Json.Nodes.JsonNode::op_Explicit:System.Nullable{System.UInt32}(System.Text.Json.Nodes.JsonNode) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 65,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.Nodes.JsonNode::op_Explicit:System.Nullable{System.UInt32}(System.Text.Json.Nodes.JsonNode)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [66] System.Text.Json/System.Text.Json.Nodes.JsonNode::op_Explicit:System.UInt64(System.Text.Json.Nodes.JsonNode) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 66,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.Nodes.JsonNode::op_Explicit:System.UInt64(System.Text.Json.Nodes.JsonNode)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [67] System.Text.Json/System.Text.Json.Nodes.JsonNode::op_Explicit:System.Nullable{System.UInt64}(System.Text.Json.Nodes.JsonNode) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 67,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.Nodes.JsonNode::op_Explicit:System.Nullable{System.UInt64}(System.Text.Json.Nodes.JsonNode)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [68] System.Text.Json/System.Text.Json.Nodes.JsonNode::op_Implicit:System.Text.Json.Nodes.JsonNode(System.Boolean) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 68,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.Nodes.JsonNode::op_Implicit:System.Text.Json.Nodes.JsonNode(System.Boolean)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [69] System.Text.Json/System.Text.Json.Nodes.JsonNode::op_Implicit:System.Text.Json.Nodes.JsonNode(System.Nullable{System.Boolean}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 69,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.Nodes.JsonNode::op_Implicit:System.Text.Json.Nodes.JsonNode(System.Nullable{System.Boolean})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [70] System.Text.Json/System.Text.Json.Nodes.JsonNode::op_Implicit:System.Text.Json.Nodes.JsonNode(System.Byte) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 70,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.Nodes.JsonNode::op_Implicit:System.Text.Json.Nodes.JsonNode(System.Byte)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [71] System.Text.Json/System.Text.Json.Nodes.JsonNode::op_Implicit:System.Text.Json.Nodes.JsonNode(System.Nullable{System.Byte}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 71,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.Nodes.JsonNode::op_Implicit:System.Text.Json.Nodes.JsonNode(System.Nullable{System.Byte})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [72] System.Text.Json/System.Text.Json.Nodes.JsonNode::op_Implicit:System.Text.Json.Nodes.JsonNode(System.Char) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 72,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.Nodes.JsonNode::op_Implicit:System.Text.Json.Nodes.JsonNode(System.Char)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [73] System.Text.Json/System.Text.Json.Nodes.JsonNode::op_Implicit:System.Text.Json.Nodes.JsonNode(System.Nullable{System.Char}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 73,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.Nodes.JsonNode::op_Implicit:System.Text.Json.Nodes.JsonNode(System.Nullable{System.Char})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [74] System.Text.Json/System.Text.Json.Nodes.JsonNode::op_Implicit:System.Text.Json.Nodes.JsonNode(System.DateTime) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 74,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.Nodes.JsonNode::op_Implicit:System.Text.Json.Nodes.JsonNode(System.DateTime)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [75] System.Text.Json/System.Text.Json.Nodes.JsonNode::op_Implicit:System.Text.Json.Nodes.JsonNode(System.Nullable{System.DateTime}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 75,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.Nodes.JsonNode::op_Implicit:System.Text.Json.Nodes.JsonNode(System.Nullable{System.DateTime})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [76] System.Text.Json/System.Text.Json.Nodes.JsonNode::op_Implicit:System.Text.Json.Nodes.JsonNode(System.DateTimeOffset) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 76,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.Nodes.JsonNode::op_Implicit:System.Text.Json.Nodes.JsonNode(System.DateTimeOffset)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [77] System.Text.Json/System.Text.Json.Nodes.JsonNode::op_Implicit:System.Text.Json.Nodes.JsonNode(System.Nullable{System.DateTimeOffset}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 77,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.Nodes.JsonNode::op_Implicit:System.Text.Json.Nodes.JsonNode(System.Nullable{System.DateTimeOffset})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [78] System.Text.Json/System.Text.Json.Nodes.JsonNode::op_Implicit:System.Text.Json.Nodes.JsonNode(System.Decimal) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 78,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.Nodes.JsonNode::op_Implicit:System.Text.Json.Nodes.JsonNode(System.Decimal)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [79] System.Text.Json/System.Text.Json.Nodes.JsonNode::op_Implicit:System.Text.Json.Nodes.JsonNode(System.Nullable{System.Decimal}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 79,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.Nodes.JsonNode::op_Implicit:System.Text.Json.Nodes.JsonNode(System.Nullable{System.Decimal})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [80] System.Text.Json/System.Text.Json.Nodes.JsonNode::op_Implicit:System.Text.Json.Nodes.JsonNode(System.Double) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 80,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.Nodes.JsonNode::op_Implicit:System.Text.Json.Nodes.JsonNode(System.Double)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [81] System.Text.Json/System.Text.Json.Nodes.JsonNode::op_Implicit:System.Text.Json.Nodes.JsonNode(System.Nullable{System.Double}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 81,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.Nodes.JsonNode::op_Implicit:System.Text.Json.Nodes.JsonNode(System.Nullable{System.Double})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [82] System.Text.Json/System.Text.Json.Nodes.JsonNode::op_Implicit:System.Text.Json.Nodes.JsonNode(System.Guid) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 82,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.Nodes.JsonNode::op_Implicit:System.Text.Json.Nodes.JsonNode(System.Guid)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [83] System.Text.Json/System.Text.Json.Nodes.JsonNode::op_Implicit:System.Text.Json.Nodes.JsonNode(System.Nullable{System.Guid}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 83,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.Nodes.JsonNode::op_Implicit:System.Text.Json.Nodes.JsonNode(System.Nullable{System.Guid})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [84] System.Text.Json/System.Text.Json.Nodes.JsonNode::op_Implicit:System.Text.Json.Nodes.JsonNode(System.Int16) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 84,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.Nodes.JsonNode::op_Implicit:System.Text.Json.Nodes.JsonNode(System.Int16)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [85] System.Text.Json/System.Text.Json.Nodes.JsonNode::op_Implicit:System.Text.Json.Nodes.JsonNode(System.Nullable{System.Int16}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 85,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.Nodes.JsonNode::op_Implicit:System.Text.Json.Nodes.JsonNode(System.Nullable{System.Int16})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [86] System.Text.Json/System.Text.Json.Nodes.JsonNode::op_Implicit:System.Text.Json.Nodes.JsonNode(System.Int32) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 86,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.Nodes.JsonNode::op_Implicit:System.Text.Json.Nodes.JsonNode(System.Int32)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [87] System.Text.Json/System.Text.Json.Nodes.JsonNode::op_Implicit:System.Text.Json.Nodes.JsonNode(System.Nullable{System.Int32}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 87,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.Nodes.JsonNode::op_Implicit:System.Text.Json.Nodes.JsonNode(System.Nullable{System.Int32})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [88] System.Text.Json/System.Text.Json.Nodes.JsonNode::op_Implicit:System.Text.Json.Nodes.JsonNode(System.Int64) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 88,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.Nodes.JsonNode::op_Implicit:System.Text.Json.Nodes.JsonNode(System.Int64)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [89] System.Text.Json/System.Text.Json.Nodes.JsonNode::op_Implicit:System.Text.Json.Nodes.JsonNode(System.Nullable{System.Int64}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 89,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.Nodes.JsonNode::op_Implicit:System.Text.Json.Nodes.JsonNode(System.Nullable{System.Int64})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [90] System.Text.Json/System.Text.Json.Nodes.JsonNode::op_Implicit:System.Text.Json.Nodes.JsonNode(System.SByte) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 90,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.Nodes.JsonNode::op_Implicit:System.Text.Json.Nodes.JsonNode(System.SByte)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [91] System.Text.Json/System.Text.Json.Nodes.JsonNode::op_Implicit:System.Text.Json.Nodes.JsonNode(System.Nullable{System.SByte}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 91,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.Nodes.JsonNode::op_Implicit:System.Text.Json.Nodes.JsonNode(System.Nullable{System.SByte})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [92] System.Text.Json/System.Text.Json.Nodes.JsonNode::op_Implicit:System.Text.Json.Nodes.JsonNode(System.Single) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 92,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.Nodes.JsonNode::op_Implicit:System.Text.Json.Nodes.JsonNode(System.Single)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [93] System.Text.Json/System.Text.Json.Nodes.JsonNode::op_Implicit:System.Text.Json.Nodes.JsonNode(System.Nullable{System.Single}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 93,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.Nodes.JsonNode::op_Implicit:System.Text.Json.Nodes.JsonNode(System.Nullable{System.Single})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [94] System.Text.Json/System.Text.Json.Nodes.JsonNode::op_Implicit:System.Text.Json.Nodes.JsonNode(System.String) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 94,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.Nodes.JsonNode::op_Implicit:System.Text.Json.Nodes.JsonNode(System.String)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [95] System.Text.Json/System.Text.Json.Nodes.JsonNode::op_Implicit:System.Text.Json.Nodes.JsonNode(System.UInt16) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 95,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.Nodes.JsonNode::op_Implicit:System.Text.Json.Nodes.JsonNode(System.UInt16)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [96] System.Text.Json/System.Text.Json.Nodes.JsonNode::op_Implicit:System.Text.Json.Nodes.JsonNode(System.Nullable{System.UInt16}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 96,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.Nodes.JsonNode::op_Implicit:System.Text.Json.Nodes.JsonNode(System.Nullable{System.UInt16})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [97] System.Text.Json/System.Text.Json.Nodes.JsonNode::op_Implicit:System.Text.Json.Nodes.JsonNode(System.UInt32) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 97,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.Nodes.JsonNode::op_Implicit:System.Text.Json.Nodes.JsonNode(System.UInt32)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [98] System.Text.Json/System.Text.Json.Nodes.JsonNode::op_Implicit:System.Text.Json.Nodes.JsonNode(System.Nullable{System.UInt32}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 98,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.Nodes.JsonNode::op_Implicit:System.Text.Json.Nodes.JsonNode(System.Nullable{System.UInt32})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [99] System.Text.Json/System.Text.Json.Nodes.JsonNode::op_Implicit:System.Text.Json.Nodes.JsonNode(System.UInt64) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 99,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.Nodes.JsonNode::op_Implicit:System.Text.Json.Nodes.JsonNode(System.UInt64)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [100] System.Text.Json/System.Text.Json.Nodes.JsonNode::op_Implicit:System.Text.Json.Nodes.JsonNode(System.Nullable{System.UInt64}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 100,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.Nodes.JsonNode::op_Implicit:System.Text.Json.Nodes.JsonNode(System.Nullable{System.UInt64})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [101] System.Text.Json/System.Text.Json.Nodes.JsonNode::Parse:System.Text.Json.Nodes.JsonNode(System.Text.Json.Utf8JsonReader&,System.Nullable{System.Text.Json.Nodes.JsonNodeOptions}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 101,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.Nodes.JsonNode::Parse:System.Text.Json.Nodes.JsonNode(System.Text.Json.Utf8JsonReader&,System.Nullable{System.Text.Json.Nodes.JsonNodeOptions})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [102] System.Text.Json/System.Text.Json.Nodes.JsonNode::Parse:System.Text.Json.Nodes.JsonNode(System.String,System.Nullable{System.Text.Json.Nodes.JsonNodeOptions},System.Text.Json.JsonDocumentOptions) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 102,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.Nodes.JsonNode::Parse:System.Text.Json.Nodes.JsonNode(System.String,System.Nullable{System.Text.Json.Nodes.JsonNodeOptions},System.Text.Json.JsonDocumentOptions)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [103] System.Text.Json/System.Text.Json.Nodes.JsonNode::Parse:System.Text.Json.Nodes.JsonNode(System.ReadOnlySpan{System.Byte},System.Nullable{System.Text.Json.Nodes.JsonNodeOptions},System.Text.Json.JsonDocumentOptions) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 103,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.Nodes.JsonNode::Parse:System.Text.Json.Nodes.JsonNode(System.ReadOnlySpan{System.Byte},System.Nullable{System.Text.Json.Nodes.JsonNodeOptions},System.Text.Json.JsonDocumentOptions)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [104] System.Text.Json/System.Text.Json.Nodes.JsonNode::Parse:System.Text.Json.Nodes.JsonNode(System.IO.Stream,System.Nullable{System.Text.Json.Nodes.JsonNodeOptions},System.Text.Json.JsonDocumentOptions) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 104,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.Nodes.JsonNode::Parse:System.Text.Json.Nodes.JsonNode(System.IO.Stream,System.Nullable{System.Text.Json.Nodes.JsonNodeOptions},System.Text.Json.JsonDocumentOptions)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [105] System.Text.Json/System.Text.Json.Nodes.JsonNode::ParseAsync:System.Threading.Tasks.Task{System.Text.Json.Nodes.JsonNode}(System.IO.Stream,System.Nullable{System.Text.Json.Nodes.JsonNodeOptions},System.Text.Json.JsonDocumentOptions,System.Threading.CancellationToken) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 105,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.Nodes.JsonNode::ParseAsync:System.Threading.Tasks.Task{System.Text.Json.Nodes.JsonNode}(System.IO.Stream,System.Nullable{System.Text.Json.Nodes.JsonNodeOptions},System.Text.Json.JsonDocumentOptions,System.Threading.CancellationToken)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [106] System.Text.Json/System.Text.Json.Nodes.JsonNode::ReplaceWith{T}:System.Void(T) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 106,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.Nodes.JsonNode::ReplaceWith{T}:System.Void(T)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [107] System.Text.Json/System.Text.Json.Nodes.JsonNode::set_Item:System.Void(System.Int32,System.Text.Json.Nodes.JsonNode) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 107,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.Nodes.JsonNode::set_Item:System.Void(System.Int32,System.Text.Json.Nodes.JsonNode)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [108] System.Text.Json/System.Text.Json.Nodes.JsonNode::set_Item:System.Void(System.String,System.Text.Json.Nodes.JsonNode) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 108,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.Nodes.JsonNode::set_Item:System.Void(System.String,System.Text.Json.Nodes.JsonNode)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [109] System.Text.Json/System.Text.Json.Nodes.JsonNode::ToJsonString:System.String(System.Text.Json.JsonSerializerOptions) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 109,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.Nodes.JsonNode::ToJsonString:System.String(System.Text.Json.JsonSerializerOptions)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [110] System.Text.Json/System.Text.Json.Nodes.JsonNode::ToString:System.String() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 110,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.Nodes.JsonNode::ToString:System.String()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [111] System.Text.Json/System.Text.Json.Nodes.JsonNode::WriteTo:System.Void(System.Text.Json.Utf8JsonWriter,System.Text.Json.JsonSerializerOptions) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 111,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.Nodes.JsonNode::WriteTo:System.Void(System.Text.Json.Utf8JsonWriter,System.Text.Json.JsonSerializerOptions)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [112] System.Text.Json/System.Text.Json.Nodes.JsonNode::set_Parent:System.Void(System.Text.Json.Nodes.JsonNode) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 112,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.Nodes.JsonNode::set_Parent:System.Void(System.Text.Json.Nodes.JsonNode)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [113] System.Text.Json/System.Text.Json.Nodes.JsonNode::get_Item:System.Text.Json.Nodes.JsonNode() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 113,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.Nodes.JsonNode::get_Item:System.Text.Json.Nodes.JsonNode()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [114] System.Text.Json/System.Text.Json.Nodes.JsonNode::set_Item:System.Void(System.Text.Json.Nodes.JsonNode) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 114,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.Nodes.JsonNode::set_Item:System.Void(System.Text.Json.Nodes.JsonNode)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [115] System.Text.Json/System.Text.Json.Nodes.JsonObject::Add:System.Void(System.String,System.Text.Json.Nodes.JsonNode) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 115,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.Nodes.JsonObject::Add:System.Void(System.String,System.Text.Json.Nodes.JsonNode)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [116] System.Text.Json/System.Text.Json.Nodes.JsonObject::Add:System.Void(System.Collections.Generic.KeyValuePair{System.String,System.Text.Json.Nodes.JsonNode}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 116,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.Nodes.JsonObject::Add:System.Void(System.Collections.Generic.KeyValuePair{System.String,System.Text.Json.Nodes.JsonNode})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [117] System.Text.Json/System.Text.Json.Nodes.JsonObject::Clear:System.Void() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 117,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.Nodes.JsonObject::Clear:System.Void()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [118] System.Text.Json/System.Text.Json.Nodes.JsonObject::ContainsKey:System.Boolean(System.String) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 118,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.Nodes.JsonObject::ContainsKey:System.Boolean(System.String)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [119] System.Text.Json/System.Text.Json.Nodes.JsonObject::Create:System.Text.Json.Nodes.JsonObject(System.Text.Json.JsonElement,System.Nullable{System.Text.Json.Nodes.JsonNodeOptions}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 119,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.Nodes.JsonObject::Create:System.Text.Json.Nodes.JsonObject(System.Text.Json.JsonElement,System.Nullable{System.Text.Json.Nodes.JsonNodeOptions})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [120] System.Text.Json/System.Text.Json.Nodes.JsonObject::get_Count:System.Int32() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 120,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.Nodes.JsonObject::get_Count:System.Int32()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [121] System.Text.Json/System.Text.Json.Nodes.JsonObject::GetAt:System.Collections.Generic.KeyValuePair{System.String,System.Text.Json.Nodes.JsonNode}(System.Int32) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 121,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.Nodes.JsonObject::GetAt:System.Collections.Generic.KeyValuePair{System.String,System.Text.Json.Nodes.JsonNode}(System.Int32)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [122] System.Text.Json/System.Text.Json.Nodes.JsonObject::GetEnumerator:System.Collections.Generic.IEnumerator{System.Collections.Generic.KeyValuePair{System.String,System.Text.Json.Nodes.JsonNode}}() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 122,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.Nodes.JsonObject::GetEnumerator:System.Collections.Generic.IEnumerator{System.Collections.Generic.KeyValuePair{System.String,System.Text.Json.Nodes.JsonNode}}()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [123] System.Text.Json/System.Text.Json.Nodes.JsonObject::IndexOf:System.Int32(System.String) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 123,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.Nodes.JsonObject::IndexOf:System.Int32(System.String)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [124] System.Text.Json/System.Text.Json.Nodes.JsonObject::Insert:System.Void(System.Int32,System.String,System.Text.Json.Nodes.JsonNode) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 124,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.Nodes.JsonObject::Insert:System.Void(System.Int32,System.String,System.Text.Json.Nodes.JsonNode)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [125] System.Text.Json/System.Text.Json.Nodes.JsonObject::Remove:System.Boolean(System.String) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 125,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.Nodes.JsonObject::Remove:System.Boolean(System.String)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [126] System.Text.Json/System.Text.Json.Nodes.JsonObject::RemoveAt:System.Void(System.Int32) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 126,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.Nodes.JsonObject::RemoveAt:System.Void(System.Int32)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [127] System.Text.Json/System.Text.Json.Nodes.JsonObject::SetAt:System.Void(System.Int32,System.String,System.Text.Json.Nodes.JsonNode) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 127,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.Nodes.JsonObject::SetAt:System.Void(System.Int32,System.String,System.Text.Json.Nodes.JsonNode)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [128] System.Text.Json/System.Text.Json.Nodes.JsonObject::SetAt:System.Void(System.Int32,System.Text.Json.Nodes.JsonNode) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 128,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.Nodes.JsonObject::SetAt:System.Void(System.Int32,System.Text.Json.Nodes.JsonNode)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [129] System.Text.Json/System.Text.Json.Nodes.JsonObject::TryAdd:System.Boolean(System.String,System.Text.Json.Nodes.JsonNode) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 129,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.Nodes.JsonObject::TryAdd:System.Boolean(System.String,System.Text.Json.Nodes.JsonNode)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [130] System.Text.Json/System.Text.Json.Nodes.JsonObject::TryAdd:System.Boolean(System.String,System.Text.Json.Nodes.JsonNode,System.Int32&) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 130,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.Nodes.JsonObject::TryAdd:System.Boolean(System.String,System.Text.Json.Nodes.JsonNode,System.Int32&)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [131] System.Text.Json/System.Text.Json.Nodes.JsonObject::TryGetPropertyValue:System.Boolean(System.String,System.Text.Json.Nodes.JsonNode&) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 131,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.Nodes.JsonObject::TryGetPropertyValue:System.Boolean(System.String,System.Text.Json.Nodes.JsonNode&)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [132] System.Text.Json/System.Text.Json.Nodes.JsonObject::TryGetPropertyValue:System.Boolean(System.String,System.Text.Json.Nodes.JsonNode&,System.Int32&) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 132,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.Nodes.JsonObject::TryGetPropertyValue:System.Boolean(System.String,System.Text.Json.Nodes.JsonNode&,System.Int32&)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [133] System.Text.Json/System.Text.Json.Nodes.JsonObject::WriteTo:System.Void(System.Text.Json.Utf8JsonWriter,System.Text.Json.JsonSerializerOptions) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 133,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.Nodes.JsonObject::WriteTo:System.Void(System.Text.Json.Utf8JsonWriter,System.Text.Json.JsonSerializerOptions)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [134] System.Text.Json/System.Text.Json.Nodes.JsonObject::.ctor:System.Void(System.Nullable{System.Text.Json.Nodes.JsonNodeOptions}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 134,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.Nodes.JsonObject::.ctor:System.Void(System.Nullable{System.Text.Json.Nodes.JsonNodeOptions})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [135] System.Text.Json/System.Text.Json.Nodes.JsonObject::.ctor:System.Void(System.Collections.Generic.IEnumerable{System.Collections.Generic.KeyValuePair{System.String,System.Text.Json.Nodes.JsonNode}},System.Nullable{System.Text.Json.Nodes.JsonNodeOptions}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 135,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.Nodes.JsonObject::.ctor:System.Void(System.Collections.Generic.IEnumerable{System.Collections.Generic.KeyValuePair{System.String,System.Text.Json.Nodes.JsonNode}},System.Nullable{System.Text.Json.Nodes.JsonNodeOptions})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [136] System.Text.Json/System.Text.Json.Nodes.JsonValue::Create:System.Text.Json.Nodes.JsonValue(System.Boolean,System.Nullable{System.Text.Json.Nodes.JsonNodeOptions}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 136,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.Nodes.JsonValue::Create:System.Text.Json.Nodes.JsonValue(System.Boolean,System.Nullable{System.Text.Json.Nodes.JsonNodeOptions})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [137] System.Text.Json/System.Text.Json.Nodes.JsonValue::Create:System.Text.Json.Nodes.JsonValue(System.Nullable{System.Boolean},System.Nullable{System.Text.Json.Nodes.JsonNodeOptions}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 137,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.Nodes.JsonValue::Create:System.Text.Json.Nodes.JsonValue(System.Nullable{System.Boolean},System.Nullable{System.Text.Json.Nodes.JsonNodeOptions})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [138] System.Text.Json/System.Text.Json.Nodes.JsonValue::Create:System.Text.Json.Nodes.JsonValue(System.Byte,System.Nullable{System.Text.Json.Nodes.JsonNodeOptions}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 138,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.Nodes.JsonValue::Create:System.Text.Json.Nodes.JsonValue(System.Byte,System.Nullable{System.Text.Json.Nodes.JsonNodeOptions})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [139] System.Text.Json/System.Text.Json.Nodes.JsonValue::Create:System.Text.Json.Nodes.JsonValue(System.Nullable{System.Byte},System.Nullable{System.Text.Json.Nodes.JsonNodeOptions}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 139,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.Nodes.JsonValue::Create:System.Text.Json.Nodes.JsonValue(System.Nullable{System.Byte},System.Nullable{System.Text.Json.Nodes.JsonNodeOptions})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [140] System.Text.Json/System.Text.Json.Nodes.JsonValue::Create:System.Text.Json.Nodes.JsonValue(System.Char,System.Nullable{System.Text.Json.Nodes.JsonNodeOptions}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 140,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.Nodes.JsonValue::Create:System.Text.Json.Nodes.JsonValue(System.Char,System.Nullable{System.Text.Json.Nodes.JsonNodeOptions})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [141] System.Text.Json/System.Text.Json.Nodes.JsonValue::Create:System.Text.Json.Nodes.JsonValue(System.Nullable{System.Char},System.Nullable{System.Text.Json.Nodes.JsonNodeOptions}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 141,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.Nodes.JsonValue::Create:System.Text.Json.Nodes.JsonValue(System.Nullable{System.Char},System.Nullable{System.Text.Json.Nodes.JsonNodeOptions})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [142] System.Text.Json/System.Text.Json.Nodes.JsonValue::Create:System.Text.Json.Nodes.JsonValue(System.DateTime,System.Nullable{System.Text.Json.Nodes.JsonNodeOptions}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 142,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.Nodes.JsonValue::Create:System.Text.Json.Nodes.JsonValue(System.DateTime,System.Nullable{System.Text.Json.Nodes.JsonNodeOptions})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [143] System.Text.Json/System.Text.Json.Nodes.JsonValue::Create:System.Text.Json.Nodes.JsonValue(System.Nullable{System.DateTime},System.Nullable{System.Text.Json.Nodes.JsonNodeOptions}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 143,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.Nodes.JsonValue::Create:System.Text.Json.Nodes.JsonValue(System.Nullable{System.DateTime},System.Nullable{System.Text.Json.Nodes.JsonNodeOptions})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [144] System.Text.Json/System.Text.Json.Nodes.JsonValue::Create:System.Text.Json.Nodes.JsonValue(System.DateTimeOffset,System.Nullable{System.Text.Json.Nodes.JsonNodeOptions}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 144,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.Nodes.JsonValue::Create:System.Text.Json.Nodes.JsonValue(System.DateTimeOffset,System.Nullable{System.Text.Json.Nodes.JsonNodeOptions})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [145] System.Text.Json/System.Text.Json.Nodes.JsonValue::Create:System.Text.Json.Nodes.JsonValue(System.Nullable{System.DateTimeOffset},System.Nullable{System.Text.Json.Nodes.JsonNodeOptions}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 145,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.Nodes.JsonValue::Create:System.Text.Json.Nodes.JsonValue(System.Nullable{System.DateTimeOffset},System.Nullable{System.Text.Json.Nodes.JsonNodeOptions})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [146] System.Text.Json/System.Text.Json.Nodes.JsonValue::Create:System.Text.Json.Nodes.JsonValue(System.Decimal,System.Nullable{System.Text.Json.Nodes.JsonNodeOptions}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 146,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.Nodes.JsonValue::Create:System.Text.Json.Nodes.JsonValue(System.Decimal,System.Nullable{System.Text.Json.Nodes.JsonNodeOptions})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [147] System.Text.Json/System.Text.Json.Nodes.JsonValue::Create:System.Text.Json.Nodes.JsonValue(System.Nullable{System.Decimal},System.Nullable{System.Text.Json.Nodes.JsonNodeOptions}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 147,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.Nodes.JsonValue::Create:System.Text.Json.Nodes.JsonValue(System.Nullable{System.Decimal},System.Nullable{System.Text.Json.Nodes.JsonNodeOptions})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [148] System.Text.Json/System.Text.Json.Nodes.JsonValue::Create:System.Text.Json.Nodes.JsonValue(System.Double,System.Nullable{System.Text.Json.Nodes.JsonNodeOptions}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 148,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.Nodes.JsonValue::Create:System.Text.Json.Nodes.JsonValue(System.Double,System.Nullable{System.Text.Json.Nodes.JsonNodeOptions})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [149] System.Text.Json/System.Text.Json.Nodes.JsonValue::Create:System.Text.Json.Nodes.JsonValue(System.Nullable{System.Double},System.Nullable{System.Text.Json.Nodes.JsonNodeOptions}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 149,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.Nodes.JsonValue::Create:System.Text.Json.Nodes.JsonValue(System.Nullable{System.Double},System.Nullable{System.Text.Json.Nodes.JsonNodeOptions})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [150] System.Text.Json/System.Text.Json.Nodes.JsonValue::Create:System.Text.Json.Nodes.JsonValue(System.Guid,System.Nullable{System.Text.Json.Nodes.JsonNodeOptions}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 150,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.Nodes.JsonValue::Create:System.Text.Json.Nodes.JsonValue(System.Guid,System.Nullable{System.Text.Json.Nodes.JsonNodeOptions})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [151] System.Text.Json/System.Text.Json.Nodes.JsonValue::Create:System.Text.Json.Nodes.JsonValue(System.Nullable{System.Guid},System.Nullable{System.Text.Json.Nodes.JsonNodeOptions}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 151,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.Nodes.JsonValue::Create:System.Text.Json.Nodes.JsonValue(System.Nullable{System.Guid},System.Nullable{System.Text.Json.Nodes.JsonNodeOptions})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [152] System.Text.Json/System.Text.Json.Nodes.JsonValue::Create:System.Text.Json.Nodes.JsonValue(System.Int16,System.Nullable{System.Text.Json.Nodes.JsonNodeOptions}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 152,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.Nodes.JsonValue::Create:System.Text.Json.Nodes.JsonValue(System.Int16,System.Nullable{System.Text.Json.Nodes.JsonNodeOptions})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [153] System.Text.Json/System.Text.Json.Nodes.JsonValue::Create:System.Text.Json.Nodes.JsonValue(System.Nullable{System.Int16},System.Nullable{System.Text.Json.Nodes.JsonNodeOptions}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 153,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.Nodes.JsonValue::Create:System.Text.Json.Nodes.JsonValue(System.Nullable{System.Int16},System.Nullable{System.Text.Json.Nodes.JsonNodeOptions})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [154] System.Text.Json/System.Text.Json.Nodes.JsonValue::Create:System.Text.Json.Nodes.JsonValue(System.Int32,System.Nullable{System.Text.Json.Nodes.JsonNodeOptions}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 154,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.Nodes.JsonValue::Create:System.Text.Json.Nodes.JsonValue(System.Int32,System.Nullable{System.Text.Json.Nodes.JsonNodeOptions})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [155] System.Text.Json/System.Text.Json.Nodes.JsonValue::Create:System.Text.Json.Nodes.JsonValue(System.Nullable{System.Int32},System.Nullable{System.Text.Json.Nodes.JsonNodeOptions}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 155,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.Nodes.JsonValue::Create:System.Text.Json.Nodes.JsonValue(System.Nullable{System.Int32},System.Nullable{System.Text.Json.Nodes.JsonNodeOptions})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [156] System.Text.Json/System.Text.Json.Nodes.JsonValue::Create:System.Text.Json.Nodes.JsonValue(System.Int64,System.Nullable{System.Text.Json.Nodes.JsonNodeOptions}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 156,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.Nodes.JsonValue::Create:System.Text.Json.Nodes.JsonValue(System.Int64,System.Nullable{System.Text.Json.Nodes.JsonNodeOptions})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [157] System.Text.Json/System.Text.Json.Nodes.JsonValue::Create:System.Text.Json.Nodes.JsonValue(System.Nullable{System.Int64},System.Nullable{System.Text.Json.Nodes.JsonNodeOptions}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 157,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.Nodes.JsonValue::Create:System.Text.Json.Nodes.JsonValue(System.Nullable{System.Int64},System.Nullable{System.Text.Json.Nodes.JsonNodeOptions})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [158] System.Text.Json/System.Text.Json.Nodes.JsonValue::Create:System.Text.Json.Nodes.JsonValue(System.SByte,System.Nullable{System.Text.Json.Nodes.JsonNodeOptions}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 158,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.Nodes.JsonValue::Create:System.Text.Json.Nodes.JsonValue(System.SByte,System.Nullable{System.Text.Json.Nodes.JsonNodeOptions})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [159] System.Text.Json/System.Text.Json.Nodes.JsonValue::Create:System.Text.Json.Nodes.JsonValue(System.Nullable{System.SByte},System.Nullable{System.Text.Json.Nodes.JsonNodeOptions}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 159,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.Nodes.JsonValue::Create:System.Text.Json.Nodes.JsonValue(System.Nullable{System.SByte},System.Nullable{System.Text.Json.Nodes.JsonNodeOptions})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [160] System.Text.Json/System.Text.Json.Nodes.JsonValue::Create:System.Text.Json.Nodes.JsonValue(System.Single,System.Nullable{System.Text.Json.Nodes.JsonNodeOptions}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 160,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.Nodes.JsonValue::Create:System.Text.Json.Nodes.JsonValue(System.Single,System.Nullable{System.Text.Json.Nodes.JsonNodeOptions})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [161] System.Text.Json/System.Text.Json.Nodes.JsonValue::Create:System.Text.Json.Nodes.JsonValue(System.Nullable{System.Single},System.Nullable{System.Text.Json.Nodes.JsonNodeOptions}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 161,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.Nodes.JsonValue::Create:System.Text.Json.Nodes.JsonValue(System.Nullable{System.Single},System.Nullable{System.Text.Json.Nodes.JsonNodeOptions})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [162] System.Text.Json/System.Text.Json.Nodes.JsonValue::Create:System.Text.Json.Nodes.JsonValue(System.String,System.Nullable{System.Text.Json.Nodes.JsonNodeOptions}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 162,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.Nodes.JsonValue::Create:System.Text.Json.Nodes.JsonValue(System.String,System.Nullable{System.Text.Json.Nodes.JsonNodeOptions})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [163] System.Text.Json/System.Text.Json.Nodes.JsonValue::Create:System.Text.Json.Nodes.JsonValue(System.UInt16,System.Nullable{System.Text.Json.Nodes.JsonNodeOptions}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 163,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.Nodes.JsonValue::Create:System.Text.Json.Nodes.JsonValue(System.UInt16,System.Nullable{System.Text.Json.Nodes.JsonNodeOptions})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [164] System.Text.Json/System.Text.Json.Nodes.JsonValue::Create:System.Text.Json.Nodes.JsonValue(System.Nullable{System.UInt16},System.Nullable{System.Text.Json.Nodes.JsonNodeOptions}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 164,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.Nodes.JsonValue::Create:System.Text.Json.Nodes.JsonValue(System.Nullable{System.UInt16},System.Nullable{System.Text.Json.Nodes.JsonNodeOptions})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [165] System.Text.Json/System.Text.Json.Nodes.JsonValue::Create:System.Text.Json.Nodes.JsonValue(System.UInt32,System.Nullable{System.Text.Json.Nodes.JsonNodeOptions}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 165,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.Nodes.JsonValue::Create:System.Text.Json.Nodes.JsonValue(System.UInt32,System.Nullable{System.Text.Json.Nodes.JsonNodeOptions})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [166] System.Text.Json/System.Text.Json.Nodes.JsonValue::Create:System.Text.Json.Nodes.JsonValue(System.Nullable{System.UInt32},System.Nullable{System.Text.Json.Nodes.JsonNodeOptions}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 166,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.Nodes.JsonValue::Create:System.Text.Json.Nodes.JsonValue(System.Nullable{System.UInt32},System.Nullable{System.Text.Json.Nodes.JsonNodeOptions})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [167] System.Text.Json/System.Text.Json.Nodes.JsonValue::Create:System.Text.Json.Nodes.JsonValue(System.UInt64,System.Nullable{System.Text.Json.Nodes.JsonNodeOptions}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 167,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.Nodes.JsonValue::Create:System.Text.Json.Nodes.JsonValue(System.UInt64,System.Nullable{System.Text.Json.Nodes.JsonNodeOptions})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [168] System.Text.Json/System.Text.Json.Nodes.JsonValue::Create:System.Text.Json.Nodes.JsonValue(System.Nullable{System.UInt64},System.Nullable{System.Text.Json.Nodes.JsonNodeOptions}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 168,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.Nodes.JsonValue::Create:System.Text.Json.Nodes.JsonValue(System.Nullable{System.UInt64},System.Nullable{System.Text.Json.Nodes.JsonNodeOptions})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [169] System.Text.Json/System.Text.Json.Nodes.JsonValue::Create:System.Text.Json.Nodes.JsonValue(System.Text.Json.JsonElement,System.Nullable{System.Text.Json.Nodes.JsonNodeOptions}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 169,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.Nodes.JsonValue::Create:System.Text.Json.Nodes.JsonValue(System.Text.Json.JsonElement,System.Nullable{System.Text.Json.Nodes.JsonNodeOptions})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [170] System.Text.Json/System.Text.Json.Nodes.JsonValue::Create:System.Text.Json.Nodes.JsonValue(System.Nullable{System.Text.Json.JsonElement},System.Nullable{System.Text.Json.Nodes.JsonNodeOptions}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 170,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.Nodes.JsonValue::Create:System.Text.Json.Nodes.JsonValue(System.Nullable{System.Text.Json.JsonElement},System.Nullable{System.Text.Json.Nodes.JsonNodeOptions})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [171] System.Text.Json/System.Text.Json.Nodes.JsonValue::Create{T}:System.Text.Json.Nodes.JsonValue(T,System.Nullable{System.Text.Json.Nodes.JsonNodeOptions}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 171,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.Nodes.JsonValue::Create{T}:System.Text.Json.Nodes.JsonValue(T,System.Nullable{System.Text.Json.Nodes.JsonNodeOptions})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [172] System.Text.Json/System.Text.Json.Nodes.JsonValue::Create{T}:System.Text.Json.Nodes.JsonValue(T,System.Text.Json.Serialization.Metadata.JsonTypeInfo{T},System.Nullable{System.Text.Json.Nodes.JsonNodeOptions}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 172,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.Nodes.JsonValue::Create{T}:System.Text.Json.Nodes.JsonValue(T,System.Text.Json.Serialization.Metadata.JsonTypeInfo{T},System.Nullable{System.Text.Json.Nodes.JsonNodeOptions})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [173] System.Text.Json/System.Text.Json.Nodes.JsonValue::TryGetValue{T}:System.Boolean(T&) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 173,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.Nodes.JsonValue::TryGetValue{T}:System.Boolean(T&)",
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
