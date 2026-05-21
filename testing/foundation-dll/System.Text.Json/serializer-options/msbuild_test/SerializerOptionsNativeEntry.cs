// Auto-generated native-AOT entry point
// Family: family/System.Text.Json/serializer-options
// Assembly: System.Text.Json
// Variant: benchmark

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text.Encodings.Web;
using System.Text.Json;
using System.Text.Json.Serialization;
using System.Text.Json.Serialization.Metadata;

public static partial class SerializerOptionsNativeEntry
{
    // Inlined exit code — avoids SDK method call resolution in codegen
    public static int _exitCode;

    // [0] System.Text.Json/System.Text.Json.JsonNamingPolicy::ConvertName:System.String(System.String)
    public static void Method0()
    {
        try { if (((JsonNamingPolicy.ConvertName("hello")).Length) != ((JsonNamingPolicy.ConvertName("hello")).Length)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [1] System.Text.Json/System.Text.Json.JsonNamingPolicy::get_CamelCase:System.Text.Json.JsonNamingPolicy()
    public static void Method1()
    {
        try { if ((int)(JsonNamingPolicy.CamelCase) != (int)(JsonNamingPolicy.CamelCase)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [2] System.Text.Json/System.Text.Json.JsonNamingPolicy::get_KebabCaseLower:System.Text.Json.JsonNamingPolicy()
    public static void Method2()
    {
        try { if ((int)(JsonNamingPolicy.KebabCaseLower) != (int)(JsonNamingPolicy.KebabCaseLower)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [3] System.Text.Json/System.Text.Json.JsonNamingPolicy::get_KebabCaseUpper:System.Text.Json.JsonNamingPolicy()
    public static void Method3()
    {
        try { if ((int)(JsonNamingPolicy.KebabCaseUpper) != (int)(JsonNamingPolicy.KebabCaseUpper)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [4] System.Text.Json/System.Text.Json.JsonNamingPolicy::get_SnakeCaseLower:System.Text.Json.JsonNamingPolicy()
    public static void Method4()
    {
        try { if ((int)(JsonNamingPolicy.SnakeCaseLower) != (int)(JsonNamingPolicy.SnakeCaseLower)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [5] System.Text.Json/System.Text.Json.JsonNamingPolicy::get_SnakeCaseUpper:System.Text.Json.JsonNamingPolicy()
    public static void Method5()
    {
        try { if ((int)(JsonNamingPolicy.SnakeCaseUpper) != (int)(JsonNamingPolicy.SnakeCaseUpper)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [6] System.Text.Json/System.Text.Json.JsonSerializerOptions::AddContext{TContext}:System.Void()
    public static void Method6()
    {
        try { JsonSerializerOptions.AddContext{TContext}(); }
        catch { _exitCode = 1; }
    }

    // [7] System.Text.Json/System.Text.Json.JsonSerializerOptions::get_AllowDuplicateProperties:System.Boolean()
    public static void Method7()
    {
        try { if (((JsonSerializerOptions.AllowDuplicateProperties) ? 1 : 0) != ((JsonSerializerOptions.AllowDuplicateProperties) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [8] System.Text.Json/System.Text.Json.JsonSerializerOptions::get_AllowOutOfOrderMetadataProperties:System.Boolean()
    public static void Method8()
    {
        try { if (((JsonSerializerOptions.AllowOutOfOrderMetadataProperties) ? 1 : 0) != ((JsonSerializerOptions.AllowOutOfOrderMetadataProperties) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [9] System.Text.Json/System.Text.Json.JsonSerializerOptions::get_AllowTrailingCommas:System.Boolean()
    public static void Method9()
    {
        try { if (((JsonSerializerOptions.AllowTrailingCommas) ? 1 : 0) != ((JsonSerializerOptions.AllowTrailingCommas) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [10] System.Text.Json/System.Text.Json.JsonSerializerOptions::get_Converters:System.Collections.Generic.IList{System.Text.Json.Serialization.JsonConverter}()
    public static void Method10()
    {
        try { if ((int)(JsonSerializerOptions.Converters) != (int)(JsonSerializerOptions.Converters)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [11] System.Text.Json/System.Text.Json.JsonSerializerOptions::get_Default:System.Text.Json.JsonSerializerOptions()

    // [12] System.Text.Json/System.Text.Json.JsonSerializerOptions::get_DefaultBufferSize:System.Int32()
    public static void Method12()
    {
        try { if (JsonSerializerOptions.DefaultBufferSize != JsonSerializerOptions.DefaultBufferSize) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [13] System.Text.Json/System.Text.Json.JsonSerializerOptions::get_DefaultIgnoreCondition:System.Text.Json.Serialization.JsonIgnoreCondition()
    public static void Method13()
    {
        try { if ((int)(JsonSerializerOptions.DefaultIgnoreCondition) != (int)(JsonSerializerOptions.DefaultIgnoreCondition)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [14] System.Text.Json/System.Text.Json.JsonSerializerOptions::get_DictionaryKeyPolicy:System.Text.Json.JsonNamingPolicy()
    public static void Method14()
    {
        try { if ((int)(JsonSerializerOptions.DictionaryKeyPolicy) != (int)(JsonSerializerOptions.DictionaryKeyPolicy)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [15] System.Text.Json/System.Text.Json.JsonSerializerOptions::get_Encoder:System.Text.Encodings.Web.JavaScriptEncoder()
    public static void Method15()
    {
        try { if ((int)(JsonSerializerOptions.Encoder) != (int)(JsonSerializerOptions.Encoder)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [16] System.Text.Json/System.Text.Json.JsonSerializerOptions::get_IgnoreNullValues:System.Boolean()
    public static void Method16()
    {
        try { if (((JsonSerializerOptions.IgnoreNullValues) ? 1 : 0) != ((JsonSerializerOptions.IgnoreNullValues) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [17] System.Text.Json/System.Text.Json.JsonSerializerOptions::get_IgnoreReadOnlyFields:System.Boolean()
    public static void Method17()
    {
        try { if (((JsonSerializerOptions.IgnoreReadOnlyFields) ? 1 : 0) != ((JsonSerializerOptions.IgnoreReadOnlyFields) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [18] System.Text.Json/System.Text.Json.JsonSerializerOptions::get_IgnoreReadOnlyProperties:System.Boolean()
    public static void Method18()
    {
        try { if (((JsonSerializerOptions.IgnoreReadOnlyProperties) ? 1 : 0) != ((JsonSerializerOptions.IgnoreReadOnlyProperties) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [19] System.Text.Json/System.Text.Json.JsonSerializerOptions::get_IncludeFields:System.Boolean()
    public static void Method19()
    {
        try { if (((JsonSerializerOptions.IncludeFields) ? 1 : 0) != ((JsonSerializerOptions.IncludeFields) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [20] System.Text.Json/System.Text.Json.JsonSerializerOptions::get_IndentCharacter:System.Char()
    public static void Method20()
    {
        try { if ((int)(JsonSerializerOptions.IndentCharacter) != (int)(JsonSerializerOptions.IndentCharacter)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [21] System.Text.Json/System.Text.Json.JsonSerializerOptions::get_IndentSize:System.Int32()
    public static void Method21()
    {
        try { if (JsonSerializerOptions.IndentSize != JsonSerializerOptions.IndentSize) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [22] System.Text.Json/System.Text.Json.JsonSerializerOptions::get_IsReadOnly:System.Boolean()
    public static void Method22()
    {
        try { if (((JsonSerializerOptions.IsReadOnly) ? 1 : 0) != ((JsonSerializerOptions.IsReadOnly) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [23] System.Text.Json/System.Text.Json.JsonSerializerOptions::get_MaxDepth:System.Int32()
    public static void Method23()
    {
        try { if (JsonSerializerOptions.MaxDepth != JsonSerializerOptions.MaxDepth) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [24] System.Text.Json/System.Text.Json.JsonSerializerOptions::get_NewLine:System.String()
    public static void Method24()
    {
        try { if (((JsonSerializerOptions.NewLine).Length) != ((JsonSerializerOptions.NewLine).Length)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [25] System.Text.Json/System.Text.Json.JsonSerializerOptions::get_NumberHandling:System.Text.Json.Serialization.JsonNumberHandling()
    public static void Method25()
    {
        try { if ((int)(JsonSerializerOptions.NumberHandling) != (int)(JsonSerializerOptions.NumberHandling)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [26] System.Text.Json/System.Text.Json.JsonSerializerOptions::get_PreferredObjectCreationHandling:System.Text.Json.Serialization.JsonObjectCreationHandling()
    public static void Method26()
    {
        try { if ((int)(JsonSerializerOptions.PreferredObjectCreationHandling) != (int)(JsonSerializerOptions.PreferredObjectCreationHandling)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [27] System.Text.Json/System.Text.Json.JsonSerializerOptions::get_PropertyNameCaseInsensitive:System.Boolean()
    public static void Method27()
    {
        try { if (((JsonSerializerOptions.PropertyNameCaseInsensitive) ? 1 : 0) != ((JsonSerializerOptions.PropertyNameCaseInsensitive) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [28] System.Text.Json/System.Text.Json.JsonSerializerOptions::get_PropertyNamingPolicy:System.Text.Json.JsonNamingPolicy()
    public static void Method28()
    {
        try { if ((int)(JsonSerializerOptions.PropertyNamingPolicy) != (int)(JsonSerializerOptions.PropertyNamingPolicy)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [29] System.Text.Json/System.Text.Json.JsonSerializerOptions::get_ReadCommentHandling:System.Text.Json.JsonCommentHandling()
    public static void Method29()
    {
        try { if ((int)(JsonSerializerOptions.ReadCommentHandling) != (int)(JsonSerializerOptions.ReadCommentHandling)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [30] System.Text.Json/System.Text.Json.JsonSerializerOptions::get_ReferenceHandler:System.Text.Json.Serialization.ReferenceHandler()
    public static void Method30()
    {
        try { if ((int)(JsonSerializerOptions.ReferenceHandler) != (int)(JsonSerializerOptions.ReferenceHandler)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [31] System.Text.Json/System.Text.Json.JsonSerializerOptions::get_RespectNullableAnnotations:System.Boolean()
    public static void Method31()
    {
        try { if (((JsonSerializerOptions.RespectNullableAnnotations) ? 1 : 0) != ((JsonSerializerOptions.RespectNullableAnnotations) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [32] System.Text.Json/System.Text.Json.JsonSerializerOptions::get_RespectRequiredConstructorParameters:System.Boolean()
    public static void Method32()
    {
        try { if (((JsonSerializerOptions.RespectRequiredConstructorParameters) ? 1 : 0) != ((JsonSerializerOptions.RespectRequiredConstructorParameters) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [33] System.Text.Json/System.Text.Json.JsonSerializerOptions::get_Strict:System.Text.Json.JsonSerializerOptions()
    public static void Method33()
    {
        try { if ((int)(JsonSerializerOptions.Strict) != (int)(JsonSerializerOptions.Strict)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [34] System.Text.Json/System.Text.Json.JsonSerializerOptions::get_TypeInfoResolver:System.Text.Json.Serialization.Metadata.IJsonTypeInfoResolver()
    public static void Method34()
    {
        try { if ((int)(JsonSerializerOptions.TypeInfoResolver) != (int)(JsonSerializerOptions.TypeInfoResolver)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [35] System.Text.Json/System.Text.Json.JsonSerializerOptions::get_TypeInfoResolverChain:System.Collections.Generic.IList{System.Text.Json.Serialization.Metadata.IJsonTypeInfoResolver}()
    public static void Method35()
    {
        try { if ((int)(JsonSerializerOptions.TypeInfoResolverChain) != (int)(JsonSerializerOptions.TypeInfoResolverChain)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [36] System.Text.Json/System.Text.Json.JsonSerializerOptions::get_UnknownTypeHandling:System.Text.Json.Serialization.JsonUnknownTypeHandling()
    public static void Method36()
    {
        try { if ((int)(JsonSerializerOptions.UnknownTypeHandling) != (int)(JsonSerializerOptions.UnknownTypeHandling)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [37] System.Text.Json/System.Text.Json.JsonSerializerOptions::get_UnmappedMemberHandling:System.Text.Json.Serialization.JsonUnmappedMemberHandling()
    public static void Method37()
    {
        try { if ((int)(JsonSerializerOptions.UnmappedMemberHandling) != (int)(JsonSerializerOptions.UnmappedMemberHandling)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [38] System.Text.Json/System.Text.Json.JsonSerializerOptions::get_Web:System.Text.Json.JsonSerializerOptions()
    public static void Method38()
    {
        try { if ((int)(JsonSerializerOptions.Web) != (int)(JsonSerializerOptions.Web)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [39] System.Text.Json/System.Text.Json.JsonSerializerOptions::get_WriteIndented:System.Boolean()
    public static void Method39()
    {
        try { if (((JsonSerializerOptions.WriteIndented) ? 1 : 0) != ((JsonSerializerOptions.WriteIndented) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [40] System.Text.Json/System.Text.Json.JsonSerializerOptions::GetConverter:System.Text.Json.Serialization.JsonConverter(System.Type)
    public static void Method40()
    {
        try { if ((int)(JsonSerializerOptions.GetConverter(typeof(byte))) != (int)(JsonSerializerOptions.GetConverter(typeof(byte)))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [41] System.Text.Json/System.Text.Json.JsonSerializerOptions::GetTypeInfo:System.Text.Json.Serialization.Metadata.JsonTypeInfo(System.Type)
    public static void Method41()
    {
        try { if ((int)(JsonSerializerOptions.GetTypeInfo(typeof(byte))) != (int)(JsonSerializerOptions.GetTypeInfo(typeof(byte)))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [42] System.Text.Json/System.Text.Json.JsonSerializerOptions::MakeReadOnly:System.Void()
    public static void Method42()
    {
        try { JsonSerializerOptions.MakeReadOnly(); }
        catch { _exitCode = 1; }
    }

    // [43] System.Text.Json/System.Text.Json.JsonSerializerOptions::MakeReadOnly:System.Void(System.Boolean)
    public static void Method43()
    {
        try { JsonSerializerOptions.MakeReadOnly(true); }
        catch { _exitCode = 1; }
    }

    // [44] System.Text.Json/System.Text.Json.JsonSerializerOptions::set_AllowDuplicateProperties:System.Void(System.Boolean)
    public static void Method44()
    {
        try { JsonSerializerOptions.AllowDuplicateProperties = true; }
        catch { _exitCode = 1; }
    }

    // [45] System.Text.Json/System.Text.Json.JsonSerializerOptions::set_AllowOutOfOrderMetadataProperties:System.Void(System.Boolean)
    public static void Method45()
    {
        try { JsonSerializerOptions.AllowOutOfOrderMetadataProperties = true; }
        catch { _exitCode = 1; }
    }

    // [46] System.Text.Json/System.Text.Json.JsonSerializerOptions::set_AllowTrailingCommas:System.Void(System.Boolean)
    public static void Method46()
    {
        try { JsonSerializerOptions.AllowTrailingCommas = true; }
        catch { _exitCode = 1; }
    }

    // [47] System.Text.Json/System.Text.Json.JsonSerializerOptions::set_DefaultBufferSize:System.Void(System.Int32)
    public static void Method47()
    {
        try { JsonSerializerOptions.DefaultBufferSize = 42; }
        catch { _exitCode = 1; }
    }

    // [48] System.Text.Json/System.Text.Json.JsonSerializerOptions::set_DefaultIgnoreCondition:System.Void(System.Text.Json.Serialization.JsonIgnoreCondition)
    public static void Method48()
    {
        try { JsonSerializerOptions.DefaultIgnoreCondition = null!; }
        catch { _exitCode = 1; }
    }

    // [49] System.Text.Json/System.Text.Json.JsonSerializerOptions::set_DictionaryKeyPolicy:System.Void(System.Text.Json.JsonNamingPolicy)
    public static void Method49()
    {
        try { JsonSerializerOptions.DictionaryKeyPolicy = null!; }
        catch { _exitCode = 1; }
    }

    // [50] System.Text.Json/System.Text.Json.JsonSerializerOptions::set_Encoder:System.Void(System.Text.Encodings.Web.JavaScriptEncoder)
    public static void Method50()
    {
        try { JsonSerializerOptions.Encoder = null!; }
        catch { _exitCode = 1; }
    }

    // [51] System.Text.Json/System.Text.Json.JsonSerializerOptions::set_IgnoreNullValues:System.Void(System.Boolean)
    public static void Method51()
    {
        try { JsonSerializerOptions.IgnoreNullValues = true; }
        catch { _exitCode = 1; }
    }

    // [52] System.Text.Json/System.Text.Json.JsonSerializerOptions::set_IgnoreReadOnlyFields:System.Void(System.Boolean)
    public static void Method52()
    {
        try { JsonSerializerOptions.IgnoreReadOnlyFields = true; }
        catch { _exitCode = 1; }
    }

    // [53] System.Text.Json/System.Text.Json.JsonSerializerOptions::set_IgnoreReadOnlyProperties:System.Void(System.Boolean)
    public static void Method53()
    {
        try { JsonSerializerOptions.IgnoreReadOnlyProperties = true; }
        catch { _exitCode = 1; }
    }

    // [54] System.Text.Json/System.Text.Json.JsonSerializerOptions::set_IncludeFields:System.Void(System.Boolean)
    public static void Method54()
    {
        try { JsonSerializerOptions.IncludeFields = true; }
        catch { _exitCode = 1; }
    }

    // [55] System.Text.Json/System.Text.Json.JsonSerializerOptions::set_IndentCharacter:System.Void(System.Char)
    public static void Method55()
    {
        try { JsonSerializerOptions.IndentCharacter = 'A'; }
        catch { _exitCode = 1; }
    }

    // [56] System.Text.Json/System.Text.Json.JsonSerializerOptions::set_IndentSize:System.Void(System.Int32)
    public static void Method56()
    {
        try { JsonSerializerOptions.IndentSize = 42; }
        catch { _exitCode = 1; }
    }

    // [57] System.Text.Json/System.Text.Json.JsonSerializerOptions::set_MaxDepth:System.Void(System.Int32)
    public static void Method57()
    {
        try { JsonSerializerOptions.MaxDepth = 42; }
        catch { _exitCode = 1; }
    }

    // [58] System.Text.Json/System.Text.Json.JsonSerializerOptions::set_NewLine:System.Void(System.String)
    public static void Method58()
    {
        try { JsonSerializerOptions.NewLine = "hello"; }
        catch { _exitCode = 1; }
    }

    // [59] System.Text.Json/System.Text.Json.JsonSerializerOptions::set_NumberHandling:System.Void(System.Text.Json.Serialization.JsonNumberHandling)
    public static void Method59()
    {
        try { JsonSerializerOptions.NumberHandling = null!; }
        catch { _exitCode = 1; }
    }

    // [60] System.Text.Json/System.Text.Json.JsonSerializerOptions::set_PreferredObjectCreationHandling:System.Void(System.Text.Json.Serialization.JsonObjectCreationHandling)
    public static void Method60()
    {
        try { JsonSerializerOptions.PreferredObjectCreationHandling = null!; }
        catch { _exitCode = 1; }
    }

    // [61] System.Text.Json/System.Text.Json.JsonSerializerOptions::set_PropertyNameCaseInsensitive:System.Void(System.Boolean)
    public static void Method61()
    {
        try { JsonSerializerOptions.PropertyNameCaseInsensitive = true; }
        catch { _exitCode = 1; }
    }

    // [62] System.Text.Json/System.Text.Json.JsonSerializerOptions::set_PropertyNamingPolicy:System.Void(System.Text.Json.JsonNamingPolicy)
    public static void Method62()
    {
        try { JsonSerializerOptions.PropertyNamingPolicy = null!; }
        catch { _exitCode = 1; }
    }

    // [63] System.Text.Json/System.Text.Json.JsonSerializerOptions::set_ReadCommentHandling:System.Void(System.Text.Json.JsonCommentHandling)
    public static void Method63()
    {
        try { JsonSerializerOptions.ReadCommentHandling = null!; }
        catch { _exitCode = 1; }
    }

    // [64] System.Text.Json/System.Text.Json.JsonSerializerOptions::set_ReferenceHandler:System.Void(System.Text.Json.Serialization.ReferenceHandler)
    public static void Method64()
    {
        try { JsonSerializerOptions.ReferenceHandler = null!; }
        catch { _exitCode = 1; }
    }

    // [65] System.Text.Json/System.Text.Json.JsonSerializerOptions::set_RespectNullableAnnotations:System.Void(System.Boolean)
    public static void Method65()
    {
        try { JsonSerializerOptions.RespectNullableAnnotations = true; }
        catch { _exitCode = 1; }
    }

    // [66] System.Text.Json/System.Text.Json.JsonSerializerOptions::set_RespectRequiredConstructorParameters:System.Void(System.Boolean)
    public static void Method66()
    {
        try { JsonSerializerOptions.RespectRequiredConstructorParameters = true; }
        catch { _exitCode = 1; }
    }

    // [67] System.Text.Json/System.Text.Json.JsonSerializerOptions::set_TypeInfoResolver:System.Void(System.Text.Json.Serialization.Metadata.IJsonTypeInfoResolver)
    public static void Method67()
    {
        try { JsonSerializerOptions.TypeInfoResolver = null!; }
        catch { _exitCode = 1; }
    }

    // [68] System.Text.Json/System.Text.Json.JsonSerializerOptions::set_UnknownTypeHandling:System.Void(System.Text.Json.Serialization.JsonUnknownTypeHandling)
    public static void Method68()
    {
        try { JsonSerializerOptions.UnknownTypeHandling = null!; }
        catch { _exitCode = 1; }
    }

    // [69] System.Text.Json/System.Text.Json.JsonSerializerOptions::set_UnmappedMemberHandling:System.Void(System.Text.Json.Serialization.JsonUnmappedMemberHandling)
    public static void Method69()
    {
        try { JsonSerializerOptions.UnmappedMemberHandling = null!; }
        catch { _exitCode = 1; }
    }

    // [70] System.Text.Json/System.Text.Json.JsonSerializerOptions::set_WriteIndented:System.Void(System.Boolean)
    public static void Method70()
    {
        try { JsonSerializerOptions.WriteIndented = true; }
        catch { _exitCode = 1; }
    }

    // [71] System.Text.Json/System.Text.Json.JsonSerializerOptions::TryGetTypeInfo:System.Boolean(System.Type,System.Text.Json.Serialization.Metadata.JsonTypeInfo&)
    public static void Method71()
    {
    var refLocal_1 = null!;
        try { if (((JsonSerializerOptions.TryGetTypeInfo(typeof(byte), ref refLocal_1)) ? 1 : 0) != ((JsonSerializerOptions.TryGetTypeInfo(typeof(byte), ref refLocal_1)) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [72] System.Text.Json/System.Text.Json.JsonSerializerOptions::.ctor:System.Void()
    public static void Method72()
    {
        try { new JsonSerializerOptions(); }
        catch { _exitCode = 1; }
    }

    // [73] System.Text.Json/System.Text.Json.JsonSerializerOptions::.ctor:System.Void(System.Text.Json.JsonSerializerOptions)
    public static void Method73()
    {
        try { new JsonSerializerOptions(null!); }
        catch { _exitCode = 1; }
    }

    // [74] System.Text.Json/System.Text.Json.JsonSerializerOptions::.ctor:System.Void(System.Text.Json.JsonSerializerDefaults)
    public static void Method74()
    {
        try { new JsonSerializerOptions(null!); }
        catch { _exitCode = 1; }
    }

    public static void Run(int entryIndex)
    {
        try
        {
            switch (entryIndex)
            {
                case 0: Method0(); break;
                case 1: Method1(); break;
                case 2: Method2(); break;
                case 3: Method3(); break;
                case 4: Method4(); break;
                case 5: Method5(); break;
                case 6: Method6(); break;
                case 7: Method7(); break;
                case 8: Method8(); break;
                case 9: Method9(); break;
                case 10: Method10(); break;
                case 11: CustomEntryMethod11(); break;
                case 12: Method12(); break;
                case 13: Method13(); break;
                case 14: Method14(); break;
                case 15: Method15(); break;
                case 16: Method16(); break;
                case 17: Method17(); break;
                case 18: Method18(); break;
                case 19: Method19(); break;
                case 20: Method20(); break;
                case 21: Method21(); break;
                case 22: Method22(); break;
                case 23: Method23(); break;
                case 24: Method24(); break;
                case 25: Method25(); break;
                case 26: Method26(); break;
                case 27: Method27(); break;
                case 28: Method28(); break;
                case 29: Method29(); break;
                case 30: Method30(); break;
                case 31: Method31(); break;
                case 32: Method32(); break;
                case 33: Method33(); break;
                case 34: Method34(); break;
                case 35: Method35(); break;
                case 36: Method36(); break;
                case 37: Method37(); break;
                case 38: Method38(); break;
                case 39: Method39(); break;
                case 40: Method40(); break;
                case 41: Method41(); break;
                case 42: Method42(); break;
                case 43: Method43(); break;
                case 44: Method44(); break;
                case 45: Method45(); break;
                case 46: Method46(); break;
                case 47: Method47(); break;
                case 48: Method48(); break;
                case 49: Method49(); break;
                case 50: Method50(); break;
                case 51: Method51(); break;
                case 52: Method52(); break;
                case 53: Method53(); break;
                case 54: Method54(); break;
                case 55: Method55(); break;
                case 56: Method56(); break;
                case 57: Method57(); break;
                case 58: Method58(); break;
                case 59: Method59(); break;
                case 60: Method60(); break;
                case 61: Method61(); break;
                case 62: Method62(); break;
                case 63: Method63(); break;
                case 64: Method64(); break;
                case 65: Method65(); break;
                case 66: Method66(); break;
                case 67: Method67(); break;
                case 68: Method68(); break;
                case 69: Method69(); break;
                case 70: Method70(); break;
                case 71: Method71(); break;
                case 72: Method72(); break;
                case 73: Method73(); break;
                case 74: Method74(); break;
            }
        }
        catch
        {
            _exitCode = 1;
        }
    }

}