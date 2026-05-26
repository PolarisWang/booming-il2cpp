// Auto-generated native-AOT entry point
// Family: family/System.Text.Json/converters
// Assembly: System.Text.Json
// Variant: subjects

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Text.Json;
using System.Text.Json.Serialization;
using Chaos.TestFramework;

public static partial class ConvertersSubjects
{
    // Inlined exit code — avoids SDK method call resolution in codegen
    public static int _exitCode;

    // [0] System.Text.Json/System.Text.Json.Serialization.JsonConverter::CanConvert:System.Boolean(System.Type)
    public static void Subject_0()
    {
        try { if (((default(JsonConverter)!.CanConvert(typeof(byte))) ? 1 : 0) != ((default(JsonConverter)!.CanConvert(typeof(byte))) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [1] System.Text.Json/System.Text.Json.Serialization.JsonConverter::get_Type:System.Type()
    public static void Subject_1()
    {
        try { if (((default(JsonConverter)!.Type).GetHashCode()) != ((default(JsonConverter)!.Type).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [2] System.Text.Json/System.Text.Json.Serialization.JsonConverter`1::CanConvert:System.Boolean(System.Type)
    public static void Subject_2()
    {
        try { if (((default(JsonConverter)!.CanConvert(typeof(byte))) ? 1 : 0) != ((default(JsonConverter)!.CanConvert(typeof(byte))) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [3] System.Text.Json/System.Text.Json.Serialization.JsonConverter`1::get_HandleNull:System.Boolean()
    public static void Subject_3()
    {
        try { if (((default(JsonConverter<byte>)!.HandleNull) ? 1 : 0) != ((default(JsonConverter<byte>)!.HandleNull) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [4] System.Text.Json/System.Text.Json.Serialization.JsonConverter`1::get_Type:System.Type()
    public static void Subject_4()
    {
        try { if (((default(JsonConverter)!.Type).GetHashCode()) != ((default(JsonConverter)!.Type).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [5] System.Text.Json/System.Text.Json.Serialization.JsonConverter`1::Read:T(System.Text.Json.Utf8JsonReader&,System.Type,System.Text.Json.JsonSerializerOptions)
    public static void Subject_5()
    {
        try { var opts = new JsonSerializerOptions(); var converter = (JsonConverter<int>)opts.GetConverter(typeof(int)); var bytes = Encoding.UTF8.GetBytes("42"); var reader = new Utf8JsonReader(bytes); reader.Read(); var result = converter.Read(ref reader, typeof(int), opts); Assert.AreEqual(42, result); }
        catch { _exitCode = 1; }
    }

    // [6] System.Text.Json/System.Text.Json.Serialization.JsonConverter`1::ReadAsPropertyName:T(System.Text.Json.Utf8JsonReader&,System.Type,System.Text.Json.JsonSerializerOptions)
    public static void Subject_6()
    {
        try { var opts = new JsonSerializerOptions(); var converter = (JsonConverter<int>)opts.GetConverter(typeof(int)); var bytes = Encoding.UTF8.GetBytes("\"42\""); var reader = new Utf8JsonReader(bytes); reader.Read(); var result = converter.ReadAsPropertyName(ref reader, typeof(int), opts); Assert.AreEqual(42, result); }
        catch { _exitCode = 1; }
    }

    // [7] System.Text.Json/System.Text.Json.Serialization.JsonConverter`1::Write:System.Void(System.Text.Json.Utf8JsonWriter,T,System.Text.Json.JsonSerializerOptions)
    public static void Subject_7()
    {
        try { var opts = new JsonSerializerOptions(); var converter = (JsonConverter<int>)opts.GetConverter(typeof(int)); using var ms = new System.IO.MemoryStream(); using var writer = new Utf8JsonWriter(ms); converter.Write(writer, 42, opts); writer.Flush(); Assert.IsTrue(ms.Length > 0); }
        catch { _exitCode = 1; }
    }

    // [8] System.Text.Json/System.Text.Json.Serialization.JsonConverter`1::WriteAsPropertyName:System.Void(System.Text.Json.Utf8JsonWriter,T,System.Text.Json.JsonSerializerOptions)
    public static void Subject_8()
    {
        try { var opts = new JsonSerializerOptions(); var converter = (JsonConverter<int>)opts.GetConverter(typeof(int)); using var ms = new System.IO.MemoryStream(); using var writer = new Utf8JsonWriter(ms); converter.WriteAsPropertyName(writer, 42, opts); writer.Flush(); Assert.IsTrue(ms.Length > 0); }
        catch { _exitCode = 1; }
    }

}
