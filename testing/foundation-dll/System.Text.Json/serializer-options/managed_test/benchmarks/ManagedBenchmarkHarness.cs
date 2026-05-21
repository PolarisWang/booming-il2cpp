// Auto-generated managed benchmark harness
// Family: serializer-options, Assembly: System.Text.Json
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
            { // [0] System.Text.Json/System.Text.Json.JsonNamingPolicy::ConvertName:System.String(System.String) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 0,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.JsonNamingPolicy::ConvertName:System.String(System.String)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [1] System.Text.Json/System.Text.Json.JsonNamingPolicy::get_CamelCase:System.Text.Json.JsonNamingPolicy() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 1,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.JsonNamingPolicy::get_CamelCase:System.Text.Json.JsonNamingPolicy()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [2] System.Text.Json/System.Text.Json.JsonNamingPolicy::get_KebabCaseLower:System.Text.Json.JsonNamingPolicy() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 2,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.JsonNamingPolicy::get_KebabCaseLower:System.Text.Json.JsonNamingPolicy()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [3] System.Text.Json/System.Text.Json.JsonNamingPolicy::get_KebabCaseUpper:System.Text.Json.JsonNamingPolicy() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 3,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.JsonNamingPolicy::get_KebabCaseUpper:System.Text.Json.JsonNamingPolicy()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [4] System.Text.Json/System.Text.Json.JsonNamingPolicy::get_SnakeCaseLower:System.Text.Json.JsonNamingPolicy() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 4,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.JsonNamingPolicy::get_SnakeCaseLower:System.Text.Json.JsonNamingPolicy()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [5] System.Text.Json/System.Text.Json.JsonNamingPolicy::get_SnakeCaseUpper:System.Text.Json.JsonNamingPolicy() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 5,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.JsonNamingPolicy::get_SnakeCaseUpper:System.Text.Json.JsonNamingPolicy()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [6] System.Text.Json/System.Text.Json.JsonSerializerOptions::AddContext{TContext}:System.Void() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 6,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.JsonSerializerOptions::AddContext{TContext}:System.Void()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [7] System.Text.Json/System.Text.Json.JsonSerializerOptions::get_AllowDuplicateProperties:System.Boolean() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 7,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.JsonSerializerOptions::get_AllowDuplicateProperties:System.Boolean()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [8] System.Text.Json/System.Text.Json.JsonSerializerOptions::get_AllowOutOfOrderMetadataProperties:System.Boolean() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 8,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.JsonSerializerOptions::get_AllowOutOfOrderMetadataProperties:System.Boolean()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [9] System.Text.Json/System.Text.Json.JsonSerializerOptions::get_AllowTrailingCommas:System.Boolean() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 9,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.JsonSerializerOptions::get_AllowTrailingCommas:System.Boolean()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [10] System.Text.Json/System.Text.Json.JsonSerializerOptions::get_Converters:System.Collections.Generic.IList{System.Text.Json.Serialization.JsonConverter}() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 10,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.JsonSerializerOptions::get_Converters:System.Collections.Generic.IList{System.Text.Json.Serialization.JsonConverter}()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [11] System.Text.Json/System.Text.Json.JsonSerializerOptions::get_Default:System.Text.Json.JsonSerializerOptions() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 11,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.JsonSerializerOptions::get_Default:System.Text.Json.JsonSerializerOptions()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [12] System.Text.Json/System.Text.Json.JsonSerializerOptions::get_DefaultBufferSize:System.Int32() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 12,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.JsonSerializerOptions::get_DefaultBufferSize:System.Int32()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [13] System.Text.Json/System.Text.Json.JsonSerializerOptions::get_DefaultIgnoreCondition:System.Text.Json.Serialization.JsonIgnoreCondition() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 13,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.JsonSerializerOptions::get_DefaultIgnoreCondition:System.Text.Json.Serialization.JsonIgnoreCondition()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [14] System.Text.Json/System.Text.Json.JsonSerializerOptions::get_DictionaryKeyPolicy:System.Text.Json.JsonNamingPolicy() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 14,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.JsonSerializerOptions::get_DictionaryKeyPolicy:System.Text.Json.JsonNamingPolicy()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [15] System.Text.Json/System.Text.Json.JsonSerializerOptions::get_Encoder:System.Text.Encodings.Web.JavaScriptEncoder() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 15,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.JsonSerializerOptions::get_Encoder:System.Text.Encodings.Web.JavaScriptEncoder()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [16] System.Text.Json/System.Text.Json.JsonSerializerOptions::get_IgnoreNullValues:System.Boolean() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 16,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.JsonSerializerOptions::get_IgnoreNullValues:System.Boolean()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [17] System.Text.Json/System.Text.Json.JsonSerializerOptions::get_IgnoreReadOnlyFields:System.Boolean() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 17,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.JsonSerializerOptions::get_IgnoreReadOnlyFields:System.Boolean()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [18] System.Text.Json/System.Text.Json.JsonSerializerOptions::get_IgnoreReadOnlyProperties:System.Boolean() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 18,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.JsonSerializerOptions::get_IgnoreReadOnlyProperties:System.Boolean()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [19] System.Text.Json/System.Text.Json.JsonSerializerOptions::get_IncludeFields:System.Boolean() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 19,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.JsonSerializerOptions::get_IncludeFields:System.Boolean()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [20] System.Text.Json/System.Text.Json.JsonSerializerOptions::get_IndentCharacter:System.Char() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 20,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.JsonSerializerOptions::get_IndentCharacter:System.Char()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [21] System.Text.Json/System.Text.Json.JsonSerializerOptions::get_IndentSize:System.Int32() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 21,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.JsonSerializerOptions::get_IndentSize:System.Int32()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [22] System.Text.Json/System.Text.Json.JsonSerializerOptions::get_IsReadOnly:System.Boolean() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 22,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.JsonSerializerOptions::get_IsReadOnly:System.Boolean()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [23] System.Text.Json/System.Text.Json.JsonSerializerOptions::get_MaxDepth:System.Int32() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 23,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.JsonSerializerOptions::get_MaxDepth:System.Int32()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [24] System.Text.Json/System.Text.Json.JsonSerializerOptions::get_NewLine:System.String() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 24,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.JsonSerializerOptions::get_NewLine:System.String()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [25] System.Text.Json/System.Text.Json.JsonSerializerOptions::get_NumberHandling:System.Text.Json.Serialization.JsonNumberHandling() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 25,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.JsonSerializerOptions::get_NumberHandling:System.Text.Json.Serialization.JsonNumberHandling()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [26] System.Text.Json/System.Text.Json.JsonSerializerOptions::get_PreferredObjectCreationHandling:System.Text.Json.Serialization.JsonObjectCreationHandling() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 26,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.JsonSerializerOptions::get_PreferredObjectCreationHandling:System.Text.Json.Serialization.JsonObjectCreationHandling()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [27] System.Text.Json/System.Text.Json.JsonSerializerOptions::get_PropertyNameCaseInsensitive:System.Boolean() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 27,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.JsonSerializerOptions::get_PropertyNameCaseInsensitive:System.Boolean()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [28] System.Text.Json/System.Text.Json.JsonSerializerOptions::get_PropertyNamingPolicy:System.Text.Json.JsonNamingPolicy() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 28,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.JsonSerializerOptions::get_PropertyNamingPolicy:System.Text.Json.JsonNamingPolicy()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [29] System.Text.Json/System.Text.Json.JsonSerializerOptions::get_ReadCommentHandling:System.Text.Json.JsonCommentHandling() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 29,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.JsonSerializerOptions::get_ReadCommentHandling:System.Text.Json.JsonCommentHandling()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [30] System.Text.Json/System.Text.Json.JsonSerializerOptions::get_ReferenceHandler:System.Text.Json.Serialization.ReferenceHandler() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 30,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.JsonSerializerOptions::get_ReferenceHandler:System.Text.Json.Serialization.ReferenceHandler()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [31] System.Text.Json/System.Text.Json.JsonSerializerOptions::get_RespectNullableAnnotations:System.Boolean() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 31,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.JsonSerializerOptions::get_RespectNullableAnnotations:System.Boolean()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [32] System.Text.Json/System.Text.Json.JsonSerializerOptions::get_RespectRequiredConstructorParameters:System.Boolean() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 32,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.JsonSerializerOptions::get_RespectRequiredConstructorParameters:System.Boolean()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [33] System.Text.Json/System.Text.Json.JsonSerializerOptions::get_Strict:System.Text.Json.JsonSerializerOptions() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 33,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.JsonSerializerOptions::get_Strict:System.Text.Json.JsonSerializerOptions()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [34] System.Text.Json/System.Text.Json.JsonSerializerOptions::get_TypeInfoResolver:System.Text.Json.Serialization.Metadata.IJsonTypeInfoResolver() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 34,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.JsonSerializerOptions::get_TypeInfoResolver:System.Text.Json.Serialization.Metadata.IJsonTypeInfoResolver()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [35] System.Text.Json/System.Text.Json.JsonSerializerOptions::get_TypeInfoResolverChain:System.Collections.Generic.IList{System.Text.Json.Serialization.Metadata.IJsonTypeInfoResolver}() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 35,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.JsonSerializerOptions::get_TypeInfoResolverChain:System.Collections.Generic.IList{System.Text.Json.Serialization.Metadata.IJsonTypeInfoResolver}()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [36] System.Text.Json/System.Text.Json.JsonSerializerOptions::get_UnknownTypeHandling:System.Text.Json.Serialization.JsonUnknownTypeHandling() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 36,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.JsonSerializerOptions::get_UnknownTypeHandling:System.Text.Json.Serialization.JsonUnknownTypeHandling()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [37] System.Text.Json/System.Text.Json.JsonSerializerOptions::get_UnmappedMemberHandling:System.Text.Json.Serialization.JsonUnmappedMemberHandling() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 37,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.JsonSerializerOptions::get_UnmappedMemberHandling:System.Text.Json.Serialization.JsonUnmappedMemberHandling()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [38] System.Text.Json/System.Text.Json.JsonSerializerOptions::get_Web:System.Text.Json.JsonSerializerOptions() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 38,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.JsonSerializerOptions::get_Web:System.Text.Json.JsonSerializerOptions()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [39] System.Text.Json/System.Text.Json.JsonSerializerOptions::get_WriteIndented:System.Boolean() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 39,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.JsonSerializerOptions::get_WriteIndented:System.Boolean()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [40] System.Text.Json/System.Text.Json.JsonSerializerOptions::GetConverter:System.Text.Json.Serialization.JsonConverter(System.Type) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 40,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.JsonSerializerOptions::GetConverter:System.Text.Json.Serialization.JsonConverter(System.Type)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [41] System.Text.Json/System.Text.Json.JsonSerializerOptions::GetTypeInfo:System.Text.Json.Serialization.Metadata.JsonTypeInfo(System.Type) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 41,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.JsonSerializerOptions::GetTypeInfo:System.Text.Json.Serialization.Metadata.JsonTypeInfo(System.Type)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [42] System.Text.Json/System.Text.Json.JsonSerializerOptions::MakeReadOnly:System.Void() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 42,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.JsonSerializerOptions::MakeReadOnly:System.Void()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [43] System.Text.Json/System.Text.Json.JsonSerializerOptions::MakeReadOnly:System.Void(System.Boolean) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 43,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.JsonSerializerOptions::MakeReadOnly:System.Void(System.Boolean)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [44] System.Text.Json/System.Text.Json.JsonSerializerOptions::set_AllowDuplicateProperties:System.Void(System.Boolean) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 44,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.JsonSerializerOptions::set_AllowDuplicateProperties:System.Void(System.Boolean)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [45] System.Text.Json/System.Text.Json.JsonSerializerOptions::set_AllowOutOfOrderMetadataProperties:System.Void(System.Boolean) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 45,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.JsonSerializerOptions::set_AllowOutOfOrderMetadataProperties:System.Void(System.Boolean)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [46] System.Text.Json/System.Text.Json.JsonSerializerOptions::set_AllowTrailingCommas:System.Void(System.Boolean) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 46,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.JsonSerializerOptions::set_AllowTrailingCommas:System.Void(System.Boolean)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [47] System.Text.Json/System.Text.Json.JsonSerializerOptions::set_DefaultBufferSize:System.Void(System.Int32) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 47,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.JsonSerializerOptions::set_DefaultBufferSize:System.Void(System.Int32)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [48] System.Text.Json/System.Text.Json.JsonSerializerOptions::set_DefaultIgnoreCondition:System.Void(System.Text.Json.Serialization.JsonIgnoreCondition) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 48,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.JsonSerializerOptions::set_DefaultIgnoreCondition:System.Void(System.Text.Json.Serialization.JsonIgnoreCondition)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [49] System.Text.Json/System.Text.Json.JsonSerializerOptions::set_DictionaryKeyPolicy:System.Void(System.Text.Json.JsonNamingPolicy) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 49,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.JsonSerializerOptions::set_DictionaryKeyPolicy:System.Void(System.Text.Json.JsonNamingPolicy)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [50] System.Text.Json/System.Text.Json.JsonSerializerOptions::set_Encoder:System.Void(System.Text.Encodings.Web.JavaScriptEncoder) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 50,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.JsonSerializerOptions::set_Encoder:System.Void(System.Text.Encodings.Web.JavaScriptEncoder)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [51] System.Text.Json/System.Text.Json.JsonSerializerOptions::set_IgnoreNullValues:System.Void(System.Boolean) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 51,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.JsonSerializerOptions::set_IgnoreNullValues:System.Void(System.Boolean)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [52] System.Text.Json/System.Text.Json.JsonSerializerOptions::set_IgnoreReadOnlyFields:System.Void(System.Boolean) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 52,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.JsonSerializerOptions::set_IgnoreReadOnlyFields:System.Void(System.Boolean)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [53] System.Text.Json/System.Text.Json.JsonSerializerOptions::set_IgnoreReadOnlyProperties:System.Void(System.Boolean) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 53,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.JsonSerializerOptions::set_IgnoreReadOnlyProperties:System.Void(System.Boolean)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [54] System.Text.Json/System.Text.Json.JsonSerializerOptions::set_IncludeFields:System.Void(System.Boolean) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 54,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.JsonSerializerOptions::set_IncludeFields:System.Void(System.Boolean)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [55] System.Text.Json/System.Text.Json.JsonSerializerOptions::set_IndentCharacter:System.Void(System.Char) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 55,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.JsonSerializerOptions::set_IndentCharacter:System.Void(System.Char)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [56] System.Text.Json/System.Text.Json.JsonSerializerOptions::set_IndentSize:System.Void(System.Int32) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 56,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.JsonSerializerOptions::set_IndentSize:System.Void(System.Int32)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [57] System.Text.Json/System.Text.Json.JsonSerializerOptions::set_MaxDepth:System.Void(System.Int32) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 57,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.JsonSerializerOptions::set_MaxDepth:System.Void(System.Int32)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [58] System.Text.Json/System.Text.Json.JsonSerializerOptions::set_NewLine:System.Void(System.String) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 58,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.JsonSerializerOptions::set_NewLine:System.Void(System.String)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [59] System.Text.Json/System.Text.Json.JsonSerializerOptions::set_NumberHandling:System.Void(System.Text.Json.Serialization.JsonNumberHandling) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 59,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.JsonSerializerOptions::set_NumberHandling:System.Void(System.Text.Json.Serialization.JsonNumberHandling)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [60] System.Text.Json/System.Text.Json.JsonSerializerOptions::set_PreferredObjectCreationHandling:System.Void(System.Text.Json.Serialization.JsonObjectCreationHandling) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 60,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.JsonSerializerOptions::set_PreferredObjectCreationHandling:System.Void(System.Text.Json.Serialization.JsonObjectCreationHandling)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [61] System.Text.Json/System.Text.Json.JsonSerializerOptions::set_PropertyNameCaseInsensitive:System.Void(System.Boolean) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 61,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.JsonSerializerOptions::set_PropertyNameCaseInsensitive:System.Void(System.Boolean)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [62] System.Text.Json/System.Text.Json.JsonSerializerOptions::set_PropertyNamingPolicy:System.Void(System.Text.Json.JsonNamingPolicy) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 62,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.JsonSerializerOptions::set_PropertyNamingPolicy:System.Void(System.Text.Json.JsonNamingPolicy)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [63] System.Text.Json/System.Text.Json.JsonSerializerOptions::set_ReadCommentHandling:System.Void(System.Text.Json.JsonCommentHandling) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 63,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.JsonSerializerOptions::set_ReadCommentHandling:System.Void(System.Text.Json.JsonCommentHandling)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [64] System.Text.Json/System.Text.Json.JsonSerializerOptions::set_ReferenceHandler:System.Void(System.Text.Json.Serialization.ReferenceHandler) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 64,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.JsonSerializerOptions::set_ReferenceHandler:System.Void(System.Text.Json.Serialization.ReferenceHandler)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [65] System.Text.Json/System.Text.Json.JsonSerializerOptions::set_RespectNullableAnnotations:System.Void(System.Boolean) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 65,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.JsonSerializerOptions::set_RespectNullableAnnotations:System.Void(System.Boolean)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [66] System.Text.Json/System.Text.Json.JsonSerializerOptions::set_RespectRequiredConstructorParameters:System.Void(System.Boolean) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 66,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.JsonSerializerOptions::set_RespectRequiredConstructorParameters:System.Void(System.Boolean)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [67] System.Text.Json/System.Text.Json.JsonSerializerOptions::set_TypeInfoResolver:System.Void(System.Text.Json.Serialization.Metadata.IJsonTypeInfoResolver) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 67,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.JsonSerializerOptions::set_TypeInfoResolver:System.Void(System.Text.Json.Serialization.Metadata.IJsonTypeInfoResolver)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [68] System.Text.Json/System.Text.Json.JsonSerializerOptions::set_UnknownTypeHandling:System.Void(System.Text.Json.Serialization.JsonUnknownTypeHandling) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 68,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.JsonSerializerOptions::set_UnknownTypeHandling:System.Void(System.Text.Json.Serialization.JsonUnknownTypeHandling)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [69] System.Text.Json/System.Text.Json.JsonSerializerOptions::set_UnmappedMemberHandling:System.Void(System.Text.Json.Serialization.JsonUnmappedMemberHandling) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 69,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.JsonSerializerOptions::set_UnmappedMemberHandling:System.Void(System.Text.Json.Serialization.JsonUnmappedMemberHandling)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [70] System.Text.Json/System.Text.Json.JsonSerializerOptions::set_WriteIndented:System.Void(System.Boolean) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 70,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.JsonSerializerOptions::set_WriteIndented:System.Void(System.Boolean)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [71] System.Text.Json/System.Text.Json.JsonSerializerOptions::TryGetTypeInfo:System.Boolean(System.Type,System.Text.Json.Serialization.Metadata.JsonTypeInfo&) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 71,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.JsonSerializerOptions::TryGetTypeInfo:System.Boolean(System.Type,System.Text.Json.Serialization.Metadata.JsonTypeInfo&)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [72] System.Text.Json/System.Text.Json.JsonSerializerOptions::.ctor:System.Void() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 72,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.JsonSerializerOptions::.ctor:System.Void()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [73] System.Text.Json/System.Text.Json.JsonSerializerOptions::.ctor:System.Void(System.Text.Json.JsonSerializerOptions) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 73,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.JsonSerializerOptions::.ctor:System.Void(System.Text.Json.JsonSerializerOptions)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [74] System.Text.Json/System.Text.Json.JsonSerializerOptions::.ctor:System.Void(System.Text.Json.JsonSerializerDefaults) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 74,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.JsonSerializerOptions::.ctor:System.Void(System.Text.Json.JsonSerializerDefaults)",
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
