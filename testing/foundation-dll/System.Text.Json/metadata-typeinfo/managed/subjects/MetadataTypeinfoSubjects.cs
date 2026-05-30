// Auto-generated native-AOT entry point
// Family: family/System.Text.Json/metadata/typeinfo
// Assembly: System.Text.Json
// Variant: subjects

using System;
using System.Collections.Generic;
using System.Linq;
using System.Reflection;
using System.Text.Json;
using System.Text.Json.Serialization.Metadata;
using Chaos.TestFramework;
using System.Text.Json.Serialization;
using System.Text.Json.Serialization.Metadata;

public static partial class MetadataTypeinfoSubjects
{
    // Inlined exit code — avoids SDK method call resolution in codegen
    public static int _exitCode;

    // [0] System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::CreateJsonPropertyInfo:System.Text.Json.Serialization.Metadata.JsonPropertyInfo(System.Type,System.String)
    public static void Subject_0()
    {
        try { if (((default(JsonTypeInfo)!.CreateJsonPropertyInfo(typeof(byte), "hello")).GetHashCode()) != ((default(JsonTypeInfo)!.CreateJsonPropertyInfo(typeof(byte), "hello")).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [1] System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::CreateJsonTypeInfo{T}:System.Text.Json.Serialization.Metadata.JsonTypeInfo{T}(System.Text.Json.JsonSerializerOptions)
    public static void CustomEntrySubject_1()
    {
        try { var info = JsonTypeInfo.CreateJsonTypeInfo<int>(null!); Assert.IsNotNull(info); }
        catch { _exitCode = 1; }
    }

    // [2] System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::CreateJsonTypeInfo:System.Text.Json.Serialization.Metadata.JsonTypeInfo(System.Type,System.Text.Json.JsonSerializerOptions)
    public static void Subject_2()
    {
        try { if (((JsonTypeInfo.CreateJsonTypeInfo(typeof(byte), null!)).GetHashCode()) != ((JsonTypeInfo.CreateJsonTypeInfo(typeof(byte), null!)).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [3] System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::get_ConstructorAttributeProvider:System.Reflection.ICustomAttributeProvider()
    public static void Subject_3()
    {
        try { if (((default(JsonTypeInfo)!.ConstructorAttributeProvider).GetHashCode()) != ((default(JsonTypeInfo)!.ConstructorAttributeProvider).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [4] System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::get_Converter:System.Text.Json.Serialization.JsonConverter()
    public static void Subject_4()
    {
        try { if (((default(JsonTypeInfo)!.Converter).GetHashCode()) != ((default(JsonTypeInfo)!.Converter).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [5] System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::get_CreateObject:System.Func{System.Object}()
    public static void Subject_5()
    {
        try { if (((default(JsonTypeInfo)!.CreateObject).GetHashCode()) != ((default(JsonTypeInfo)!.CreateObject).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [6] System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::get_ElementType:System.Type()
    public static void Subject_6()
    {
        try { if (((default(JsonTypeInfo)!.ElementType).GetHashCode()) != ((default(JsonTypeInfo)!.ElementType).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [7] System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::get_IsReadOnly:System.Boolean()
    public static void Subject_7()
    {
        try { if (((default(JsonTypeInfo)!.IsReadOnly) ? 1 : 0) != ((default(JsonTypeInfo)!.IsReadOnly) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [8] System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::get_KeyType:System.Type()
    public static void Subject_8()
    {
        try { if (((default(JsonTypeInfo)!.KeyType).GetHashCode()) != ((default(JsonTypeInfo)!.KeyType).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [9] System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::get_Kind:System.Text.Json.Serialization.Metadata.JsonTypeInfoKind()
    public static void Subject_9()
    {
        try { if (((default(JsonTypeInfo)!.Kind).GetHashCode()) != ((default(JsonTypeInfo)!.Kind).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [10] System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::get_NumberHandling:System.Nullable{System.Text.Json.Serialization.JsonNumberHandling}()
    public static void Subject_10()
    {
        try { if (((default(JsonTypeInfo)!.NumberHandling).GetHashCode()) != ((default(JsonTypeInfo)!.NumberHandling).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [11] System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::get_OnDeserialized:System.Action{System.Object}()
    public static void Subject_11()
    {
        try { if (((default(JsonTypeInfo)!.OnDeserialized).GetHashCode()) != ((default(JsonTypeInfo)!.OnDeserialized).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [12] System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::get_OnDeserializing:System.Action{System.Object}()
    public static void Subject_12()
    {
        try { if (((default(JsonTypeInfo)!.OnDeserializing).GetHashCode()) != ((default(JsonTypeInfo)!.OnDeserializing).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [13] System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::get_OnSerialized:System.Action{System.Object}()
    public static void Subject_13()
    {
        try { if (((default(JsonTypeInfo)!.OnSerialized).GetHashCode()) != ((default(JsonTypeInfo)!.OnSerialized).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [14] System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::get_OnSerializing:System.Action{System.Object}()
    public static void Subject_14()
    {
        try { if (((default(JsonTypeInfo)!.OnSerializing).GetHashCode()) != ((default(JsonTypeInfo)!.OnSerializing).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [15] System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::get_Options:System.Text.Json.JsonSerializerOptions()
    public static void Subject_15()
    {
        try { if (((default(JsonTypeInfo)!.Options).GetHashCode()) != ((default(JsonTypeInfo)!.Options).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [16] System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::get_OriginatingResolver:System.Text.Json.Serialization.Metadata.IJsonTypeInfoResolver()
    public static void Subject_16()
    {
        try { if (((default(JsonTypeInfo)!.OriginatingResolver).GetHashCode()) != ((default(JsonTypeInfo)!.OriginatingResolver).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [17] System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::get_PolymorphismOptions:System.Text.Json.Serialization.Metadata.JsonPolymorphismOptions()
    public static void Subject_17()
    {
        try { if (((default(JsonTypeInfo)!.PolymorphismOptions).GetHashCode()) != ((default(JsonTypeInfo)!.PolymorphismOptions).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [18] System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::get_PreferredPropertyObjectCreationHandling:System.Nullable{System.Text.Json.Serialization.JsonObjectCreationHandling}()
    public static void Subject_18()
    {
        try { if (((default(JsonTypeInfo)!.PreferredPropertyObjectCreationHandling).GetHashCode()) != ((default(JsonTypeInfo)!.PreferredPropertyObjectCreationHandling).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [19] System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::get_Properties:System.Collections.Generic.IList{System.Text.Json.Serialization.Metadata.JsonPropertyInfo}()
    public static void Subject_19()
    {
        try { if (((default(JsonTypeInfo)!.Properties).GetHashCode()) != ((default(JsonTypeInfo)!.Properties).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [20] System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::get_Type:System.Type()
    public static void Subject_20()
    {
        try { if (((default(JsonTypeInfo)!.Type).GetHashCode()) != ((default(JsonTypeInfo)!.Type).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [21] System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::get_UnmappedMemberHandling:System.Nullable{System.Text.Json.Serialization.JsonUnmappedMemberHandling}()
    public static void Subject_21()
    {
        try { if (((default(JsonTypeInfo)!.UnmappedMemberHandling).GetHashCode()) != ((default(JsonTypeInfo)!.UnmappedMemberHandling).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [22] System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::MakeReadOnly:System.Void()
    public static void Subject_22()
    {
        try { default(JsonTypeInfo)!.MakeReadOnly(); }
        catch { _exitCode = 1; }
    }

    // [23] System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::set_CreateObject:System.Void(System.Func{System.Object})
    public static void CustomEntrySubject_23()
    {
        try { Assert.Throws<NullReferenceException>(() => default(JsonTypeInfo)!.CreateObject = default); }
        catch { _exitCode = 1; }
    }

    // [24] System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::set_NumberHandling:System.Void(System.Nullable{System.Text.Json.Serialization.JsonNumberHandling})
    public static void Subject_24()
    {
        try { default(JsonTypeInfo)!.NumberHandling = default; }
        catch { _exitCode = 1; }
    }

    // [25] System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::set_OnDeserialized:System.Void(System.Action{System.Object})
    public static void CustomEntrySubject_25()
    {
        try { Assert.Throws<NullReferenceException>(() => default(JsonTypeInfo)!.OnDeserialized = default); }
        catch { _exitCode = 1; }
    }

    // [26] System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::set_OnDeserializing:System.Void(System.Action{System.Object})
    public static void CustomEntrySubject_26()
    {
        try { Assert.Throws<NullReferenceException>(() => default(JsonTypeInfo)!.OnDeserializing = default); }
        catch { _exitCode = 1; }
    }

    // [27] System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::set_OnSerialized:System.Void(System.Action{System.Object})
    public static void CustomEntrySubject_27()
    {
        try { Assert.Throws<NullReferenceException>(() => default(JsonTypeInfo)!.OnSerialized = default); }
        catch { _exitCode = 1; }
    }

    // [28] System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::set_OnSerializing:System.Void(System.Action{System.Object})
    public static void CustomEntrySubject_28()
    {
        try { Assert.Throws<NullReferenceException>(() => default(JsonTypeInfo)!.OnSerializing = default); }
        catch { _exitCode = 1; }
    }

    // [29] System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::set_OriginatingResolver:System.Void(System.Text.Json.Serialization.Metadata.IJsonTypeInfoResolver)
    public static void Subject_29()
    {
        try { default(JsonTypeInfo)!.OriginatingResolver = default; }
        catch { _exitCode = 1; }
    }

    // [30] System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::set_PolymorphismOptions:System.Void(System.Text.Json.Serialization.Metadata.JsonPolymorphismOptions)
    public static void Subject_30()
    {
        try { default(JsonTypeInfo)!.PolymorphismOptions = default; }
        catch { _exitCode = 1; }
    }

    // [31] System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::set_PreferredPropertyObjectCreationHandling:System.Void(System.Nullable{System.Text.Json.Serialization.JsonObjectCreationHandling})
    public static void Subject_31()
    {
        try { default(JsonTypeInfo)!.PreferredPropertyObjectCreationHandling = default; }
        catch { _exitCode = 1; }
    }

    // [32] System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::set_UnmappedMemberHandling:System.Void(System.Nullable{System.Text.Json.Serialization.JsonUnmappedMemberHandling})
    public static void Subject_32()
    {
        try { default(JsonTypeInfo)!.UnmappedMemberHandling = default; }
        catch { _exitCode = 1; }
    }

    // [33] System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::set_IsReadOnly:System.Void(System.Boolean)
    public static void Subject_33()
    {
        try { var pi = typeof(JsonTypeInfo).GetProperty("IsReadOnly"); if (pi != null) pi.SetValue(default(JsonTypeInfo), true); }
        catch { _exitCode = 1; }
    }

    // [34] System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::set_ConstructorAttributeProvider:System.Void(System.Reflection.ICustomAttributeProvider)
    public static void Subject_34()
    {
        try { var pi = typeof(JsonTypeInfo).GetProperty("ConstructorAttributeProvider"); if (pi != null) pi.SetValue(default(JsonTypeInfo), default); }
        catch { _exitCode = 1; }
    }

    // [35] System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo`1::get_CreateObject:System.Func{T}()
    public static void Subject_35()
    {
        try { if (((default(JsonTypeInfo)!.CreateObject).GetHashCode()) != ((default(JsonTypeInfo)!.CreateObject).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [36] System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo`1::get_SerializeHandler:System.Action{System.Text.Json.Utf8JsonWriter,T}()
    public static void CustomEntrySubject_36()
    {
        try { var h = default(JsonTypeInfo<int>)!.SerializeHandler; Assert.IsNull(h); }
        catch { _exitCode = 1; }
    }

    // [37] System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo`1::set_CreateObject:System.Void(System.Func{T})
    public static void Subject_37()
    {
        try { default(JsonTypeInfo)!.CreateObject = default; }
        catch { _exitCode = 1; }
    }

    // [38] System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo`1::set_SerializeHandler:System.Void(System.Action{System.Text.Json.Utf8JsonWriter,T})
    public static void CustomEntrySubject_38()
    {
        try { var pi = typeof(JsonTypeInfo<int>).GetProperty("SerializeHandler"); if (pi != null) pi.SetValue(default(JsonTypeInfo<int>), default); }
        catch { _exitCode = 1; }
    }

}
