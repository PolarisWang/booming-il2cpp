// Auto-generated handwritten test stubs for pre-verification audit.
// TODO: Replace each stub body with a real test invocation.

using System.Collections.Generic;
using System.Runtime.InteropServices;
using System;
using Chaos.TestFramework;

public static partial class ConvertersSubjects
{
    // [0] System.Text.Json/System.Text.Json.Serialization.JsonConverter::CanConvert:System.Boolean(System.Type)
    [Fact]
    public static void CustomEntrySubject_0()
    {        try { _ = default(JsonConverter)!.CanConvert(typeof(byte)); } catch { }    }

    // [1] System.Text.Json/System.Text.Json.Serialization.JsonConverter::get_Type:System.Type()
    [Fact]
    public static void CustomEntrySubject_1()
    {        try { _ = default(JsonConverter)!.Type; } catch { }    }

    // [2] System.Text.Json/System.Text.Json.Serialization.JsonConverter`1::CanConvert:System.Boolean(System.Type)
    [Fact]
    public static void CustomEntrySubject_2()
    {        try { _ = default(JsonConverter)!.CanConvert(typeof(byte)); } catch { }    }

    // [4] System.Text.Json/System.Text.Json.Serialization.JsonConverter`1::get_Type:System.Type()
    [Fact]
    public static void CustomEntrySubject_4()
    {        try { _ = default(JsonConverter)!.Type; } catch { }    }

    // [5] System.Text.Json/System.Text.Json.Serialization.JsonConverter`1::Read:T(System.Text.Json.Utf8JsonReader&,System.Type,System.Text.Json.JsonSerializerOptions)
    [Fact]
    public static void CustomEntrySubject_5()
    {        try { _ = default(JsonConverter)!.Read(out default, typeof(byte), null!); } catch { }    }

    // [6] System.Text.Json/System.Text.Json.Serialization.JsonConverter`1::ReadAsPropertyName:T(System.Text.Json.Utf8JsonReader&,System.Type,System.Text.Json.JsonSerializerOptions)
    [Fact]
    public static void CustomEntrySubject_6()
    {        try { _ = default(JsonConverter)!.ReadAsPropertyName(out default, typeof(byte), null!); } catch { }    }

    // [7] System.Text.Json/System.Text.Json.Serialization.JsonConverter`1::Write:System.Void(System.Text.Json.Utf8JsonWriter,T,System.Text.Json.JsonSerializerOptions)
    [Fact]
    public static void CustomEntrySubject_7()
    {        try { default(JsonConverter)!.Write(default, default, null!); } catch { }    }

    // [8] System.Text.Json/System.Text.Json.Serialization.JsonConverter`1::WriteAsPropertyName:System.Void(System.Text.Json.Utf8JsonWriter,T,System.Text.Json.JsonSerializerOptions)
    [Fact]
    public static void CustomEntrySubject_8()
    {        try { default(JsonConverter)!.WriteAsPropertyName(default, default, null!); } catch { }    }

}
