// Auto-generated managed benchmark harness
// Family: metadata-typeinfo, Assembly: System.Text.Json
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
            { // [0] System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::CreateJsonPropertyInfo:System.Text.Json.Serialization.Metadata.JsonPropertyInfo(System.Type,System.String) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 0,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::CreateJsonPropertyInfo:System.Text.Json.Serialization.Metadata.JsonPropertyInfo(System.Type,System.String)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [1] System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::CreateJsonTypeInfo{T}:System.Text.Json.Serialization.Metadata.JsonTypeInfo{T}(System.Text.Json.JsonSerializerOptions) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 1,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::CreateJsonTypeInfo{T}:System.Text.Json.Serialization.Metadata.JsonTypeInfo{T}(System.Text.Json.JsonSerializerOptions)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [2] System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::CreateJsonTypeInfo:System.Text.Json.Serialization.Metadata.JsonTypeInfo(System.Type,System.Text.Json.JsonSerializerOptions) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 2,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::CreateJsonTypeInfo:System.Text.Json.Serialization.Metadata.JsonTypeInfo(System.Type,System.Text.Json.JsonSerializerOptions)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [3] System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::get_ConstructorAttributeProvider:System.Reflection.ICustomAttributeProvider() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 3,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::get_ConstructorAttributeProvider:System.Reflection.ICustomAttributeProvider()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [4] System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::get_Converter:System.Text.Json.Serialization.JsonConverter() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 4,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::get_Converter:System.Text.Json.Serialization.JsonConverter()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [5] System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::get_CreateObject:System.Func{System.Object}() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 5,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::get_CreateObject:System.Func{System.Object}()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [6] System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::get_ElementType:System.Type() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 6,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::get_ElementType:System.Type()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [7] System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::get_IsReadOnly:System.Boolean() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 7,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::get_IsReadOnly:System.Boolean()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [8] System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::get_KeyType:System.Type() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 8,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::get_KeyType:System.Type()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [9] System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::get_Kind:System.Text.Json.Serialization.Metadata.JsonTypeInfoKind() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 9,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::get_Kind:System.Text.Json.Serialization.Metadata.JsonTypeInfoKind()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [10] System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::get_NumberHandling:System.Nullable{System.Text.Json.Serialization.JsonNumberHandling}() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 10,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::get_NumberHandling:System.Nullable{System.Text.Json.Serialization.JsonNumberHandling}()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [11] System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::get_OnDeserialized:System.Action{System.Object}() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 11,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::get_OnDeserialized:System.Action{System.Object}()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [12] System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::get_OnDeserializing:System.Action{System.Object}() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 12,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::get_OnDeserializing:System.Action{System.Object}()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [13] System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::get_OnSerialized:System.Action{System.Object}() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 13,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::get_OnSerialized:System.Action{System.Object}()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [14] System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::get_OnSerializing:System.Action{System.Object}() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 14,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::get_OnSerializing:System.Action{System.Object}()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [15] System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::get_Options:System.Text.Json.JsonSerializerOptions() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 15,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::get_Options:System.Text.Json.JsonSerializerOptions()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [16] System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::get_OriginatingResolver:System.Text.Json.Serialization.Metadata.IJsonTypeInfoResolver() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 16,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::get_OriginatingResolver:System.Text.Json.Serialization.Metadata.IJsonTypeInfoResolver()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [17] System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::get_PolymorphismOptions:System.Text.Json.Serialization.Metadata.JsonPolymorphismOptions() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 17,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::get_PolymorphismOptions:System.Text.Json.Serialization.Metadata.JsonPolymorphismOptions()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [18] System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::get_PreferredPropertyObjectCreationHandling:System.Nullable{System.Text.Json.Serialization.JsonObjectCreationHandling}() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 18,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::get_PreferredPropertyObjectCreationHandling:System.Nullable{System.Text.Json.Serialization.JsonObjectCreationHandling}()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [19] System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::get_Properties:System.Collections.Generic.IList{System.Text.Json.Serialization.Metadata.JsonPropertyInfo}() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 19,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::get_Properties:System.Collections.Generic.IList{System.Text.Json.Serialization.Metadata.JsonPropertyInfo}()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [20] System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::get_Type:System.Type() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 20,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::get_Type:System.Type()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [21] System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::get_UnmappedMemberHandling:System.Nullable{System.Text.Json.Serialization.JsonUnmappedMemberHandling}() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 21,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::get_UnmappedMemberHandling:System.Nullable{System.Text.Json.Serialization.JsonUnmappedMemberHandling}()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [22] System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::MakeReadOnly:System.Void() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 22,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::MakeReadOnly:System.Void()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [23] System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::set_CreateObject:System.Void(System.Func{System.Object}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 23,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::set_CreateObject:System.Void(System.Func{System.Object})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [24] System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::set_NumberHandling:System.Void(System.Nullable{System.Text.Json.Serialization.JsonNumberHandling}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 24,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::set_NumberHandling:System.Void(System.Nullable{System.Text.Json.Serialization.JsonNumberHandling})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [25] System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::set_OnDeserialized:System.Void(System.Action{System.Object}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 25,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::set_OnDeserialized:System.Void(System.Action{System.Object})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [26] System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::set_OnDeserializing:System.Void(System.Action{System.Object}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 26,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::set_OnDeserializing:System.Void(System.Action{System.Object})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [27] System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::set_OnSerialized:System.Void(System.Action{System.Object}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 27,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::set_OnSerialized:System.Void(System.Action{System.Object})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [28] System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::set_OnSerializing:System.Void(System.Action{System.Object}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 28,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::set_OnSerializing:System.Void(System.Action{System.Object})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [29] System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::set_OriginatingResolver:System.Void(System.Text.Json.Serialization.Metadata.IJsonTypeInfoResolver) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 29,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::set_OriginatingResolver:System.Void(System.Text.Json.Serialization.Metadata.IJsonTypeInfoResolver)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [30] System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::set_PolymorphismOptions:System.Void(System.Text.Json.Serialization.Metadata.JsonPolymorphismOptions) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 30,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::set_PolymorphismOptions:System.Void(System.Text.Json.Serialization.Metadata.JsonPolymorphismOptions)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [31] System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::set_PreferredPropertyObjectCreationHandling:System.Void(System.Nullable{System.Text.Json.Serialization.JsonObjectCreationHandling}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 31,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::set_PreferredPropertyObjectCreationHandling:System.Void(System.Nullable{System.Text.Json.Serialization.JsonObjectCreationHandling})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [32] System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::set_UnmappedMemberHandling:System.Void(System.Nullable{System.Text.Json.Serialization.JsonUnmappedMemberHandling}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 32,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::set_UnmappedMemberHandling:System.Void(System.Nullable{System.Text.Json.Serialization.JsonUnmappedMemberHandling})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [33] System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::set_IsReadOnly:System.Void(System.Boolean) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 33,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::set_IsReadOnly:System.Void(System.Boolean)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [34] System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::set_ConstructorAttributeProvider:System.Void(System.Reflection.ICustomAttributeProvider) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 34,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::set_ConstructorAttributeProvider:System.Void(System.Reflection.ICustomAttributeProvider)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [35] System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo`1::get_CreateObject:System.Func{T}() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 35,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo`1::get_CreateObject:System.Func{T}()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [36] System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo`1::get_SerializeHandler:System.Action{System.Text.Json.Utf8JsonWriter,T}() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 36,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo`1::get_SerializeHandler:System.Action{System.Text.Json.Utf8JsonWriter,T}()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [37] System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo`1::set_CreateObject:System.Void(System.Func{T}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 37,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo`1::set_CreateObject:System.Void(System.Func{T})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [38] System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo`1::set_SerializeHandler:System.Void(System.Action{System.Text.Json.Utf8JsonWriter,T}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 38,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo`1::set_SerializeHandler:System.Void(System.Action{System.Text.Json.Utf8JsonWriter,T})",
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
