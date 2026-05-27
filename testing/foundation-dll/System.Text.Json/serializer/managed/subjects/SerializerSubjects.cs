// Auto-generated native-AOT entry point
// Family: family/System.Text.Json/serializer
// Assembly: System.Text.Json
// Variant: subjects

using System;
using System.Collections.Generic;
using System.IO;
using System.IO.Pipelines;
using System.Linq;
using System.Text;
using System.Text.Json;
using System.Text.Json.Nodes;
using System.Text.Json.Serialization;
using System.Text.Json.Serialization.Metadata;
using System.Threading;
using System.Threading.Tasks;
using Chaos.TestFramework;

public static partial class SerializerSubjects
{
    // Inlined exit code — avoids SDK method call resolution in codegen
    public static int _exitCode;

    private class TestObj { public int X { get; set; } public int Y { get; set; } }
    private const string TestJson = "{\"X\":42,\"Y\":0}";

    // [0] System.Text.Json/System.Text.Json.JsonSerializer::Deserialize{TValue}:TValue(System.Text.Json.JsonDocument,System.Text.Json.JsonSerializerOptions)
    public static void CustomEntrySubject_0()
    {
        try { using var doc = JsonDocument.Parse(TestJson); Assert.AreEqual(42, JsonSerializer.Deserialize<TestObj>(doc, (JsonSerializerOptions)null!)!.X); }
        catch { _exitCode = 1; }
    }

    // [1] System.Text.Json/System.Text.Json.JsonSerializer::Deserialize:System.Object(System.Text.Json.JsonDocument,System.Type,System.Text.Json.JsonSerializerOptions)
    public static void CustomEntrySubject_1()
    {
        try { using var doc = JsonDocument.Parse(TestJson); var r = (TestObj)JsonSerializer.Deserialize(doc, typeof(TestObj), (JsonSerializerOptions)null!)!; Assert.AreEqual(42, r!.X); }
        catch { _exitCode = 1; }
    }

    // [2] System.Text.Json/System.Text.Json.JsonSerializer::Deserialize{TValue}:TValue(System.Text.Json.JsonDocument,System.Text.Json.Serialization.Metadata.JsonTypeInfo{TValue})
    public static void CustomEntrySubject_2()
    {
        try { using var doc = JsonDocument.Parse(TestJson); var ti = (JsonTypeInfo<TestObj>)null!; Assert.Throws(() => JsonSerializer.Deserialize<TestObj>(doc, ti)); }
        catch { _exitCode = 1; }
    }

    // [3] System.Text.Json/System.Text.Json.JsonSerializer::Deserialize:System.Object(System.Text.Json.JsonDocument,System.Text.Json.Serialization.Metadata.JsonTypeInfo)
    public static void CustomEntrySubject_3()
    {
        try { using var doc = JsonDocument.Parse(TestJson); var ti = (JsonTypeInfo)null!; Assert.Throws(() => JsonSerializer.Deserialize(doc, ti)); }
        catch { _exitCode = 1; }
    }

    // [4] System.Text.Json/System.Text.Json.JsonSerializer::Deserialize:System.Object(System.Text.Json.JsonDocument,System.Type,System.Text.Json.Serialization.JsonSerializerContext)
    public static void CustomEntrySubject_4()
    {
        try { using var doc = JsonDocument.Parse(TestJson); var ctx = (JsonSerializerContext)null!; Assert.Throws(() => JsonSerializer.Deserialize(doc, typeof(TestObj), ctx)); }
        catch { _exitCode = 1; }
    }

    // [5] System.Text.Json/System.Text.Json.JsonSerializer::Deserialize{TValue}:TValue(System.Text.Json.JsonElement,System.Text.Json.JsonSerializerOptions)
    public static void CustomEntrySubject_5()
    {
        try { using var doc = JsonDocument.Parse(TestJson); Assert.AreEqual(42, JsonSerializer.Deserialize<TestObj>(doc.RootElement, (JsonSerializerOptions)null!)!.X); }
        catch { _exitCode = 1; }
    }

    // [6] System.Text.Json/System.Text.Json.JsonSerializer::Deserialize:System.Object(System.Text.Json.JsonElement,System.Type,System.Text.Json.JsonSerializerOptions)
    public static void CustomEntrySubject_6()
    {
        try { using var doc = JsonDocument.Parse(TestJson); var r = (TestObj)JsonSerializer.Deserialize(doc.RootElement, typeof(TestObj), (JsonSerializerOptions)null!)!; Assert.AreEqual(42, r!.X); }
        catch { _exitCode = 1; }
    }

    // [7] System.Text.Json/System.Text.Json.JsonSerializer::Deserialize{TValue}:TValue(System.Text.Json.JsonElement,System.Text.Json.Serialization.Metadata.JsonTypeInfo{TValue})
    public static void CustomEntrySubject_7()
    {
        try { using var doc = JsonDocument.Parse(TestJson); var ti = (JsonTypeInfo<TestObj>)null!; Assert.Throws(() => JsonSerializer.Deserialize<TestObj>(doc.RootElement, ti)); }
        catch { _exitCode = 1; }
    }

    // [8] System.Text.Json/System.Text.Json.JsonSerializer::Deserialize:System.Object(System.Text.Json.JsonElement,System.Text.Json.Serialization.Metadata.JsonTypeInfo)
    public static void CustomEntrySubject_8()
    {
        try { using var doc = JsonDocument.Parse(TestJson); var ti = (JsonTypeInfo)null!; Assert.Throws(() => JsonSerializer.Deserialize(doc.RootElement, ti)); }
        catch { _exitCode = 1; }
    }

    // [9] System.Text.Json/System.Text.Json.JsonSerializer::Deserialize:System.Object(System.Text.Json.JsonElement,System.Type,System.Text.Json.Serialization.JsonSerializerContext)
    public static void CustomEntrySubject_9()
    {
        try { using var doc = JsonDocument.Parse(TestJson); var ctx = (JsonSerializerContext)null!; Assert.Throws(() => JsonSerializer.Deserialize(doc.RootElement, typeof(TestObj), ctx)); }
        catch { _exitCode = 1; }
    }

    // [10] System.Text.Json/System.Text.Json.JsonSerializer::Deserialize{TValue}:TValue(System.Text.Json.Nodes.JsonNode,System.Text.Json.JsonSerializerOptions)
    public static void CustomEntrySubject_10()
    {
        try { var node = JsonNode.Parse(TestJson)!; Assert.AreEqual(42, JsonSerializer.Deserialize<TestObj>(node, (JsonSerializerOptions)null!)!.X); }
        catch { _exitCode = 1; }
    }

    // [11] System.Text.Json/System.Text.Json.JsonSerializer::Deserialize:System.Object(System.Text.Json.Nodes.JsonNode,System.Type,System.Text.Json.JsonSerializerOptions)
    public static void CustomEntrySubject_11()
    {
        try { var node = JsonNode.Parse(TestJson)!; var r = (TestObj)JsonSerializer.Deserialize(node, typeof(TestObj), (JsonSerializerOptions)null!)!; Assert.AreEqual(42, r!.X); }
        catch { _exitCode = 1; }
    }

    // [12] System.Text.Json/System.Text.Json.JsonSerializer::Deserialize{TValue}:TValue(System.Text.Json.Nodes.JsonNode,System.Text.Json.Serialization.Metadata.JsonTypeInfo{TValue})
    public static void CustomEntrySubject_12()
    {
        try { var node = JsonNode.Parse(TestJson)!; var ti = (JsonTypeInfo<TestObj>)null!; Assert.Throws(() => JsonSerializer.Deserialize<TestObj>(node, ti)); }
        catch { _exitCode = 1; }
    }

    // [13] System.Text.Json/System.Text.Json.JsonSerializer::Deserialize:System.Object(System.Text.Json.Nodes.JsonNode,System.Text.Json.Serialization.Metadata.JsonTypeInfo)
    public static void CustomEntrySubject_13()
    {
        try { var node = JsonNode.Parse(TestJson)!; var ti = (JsonTypeInfo)null!; Assert.Throws(() => JsonSerializer.Deserialize(node, ti)); }
        catch { _exitCode = 1; }
    }

    // [14] System.Text.Json/System.Text.Json.JsonSerializer::Deserialize:System.Object(System.Text.Json.Nodes.JsonNode,System.Type,System.Text.Json.Serialization.JsonSerializerContext)
    public static void CustomEntrySubject_14()
    {
        try { var node = JsonNode.Parse(TestJson)!; var ctx = (JsonSerializerContext)null!; Assert.Throws(() => JsonSerializer.Deserialize(node, typeof(TestObj), ctx)); }
        catch { _exitCode = 1; }
    }

    // [15] System.Text.Json/System.Text.Json.JsonSerializer::Deserialize{TValue}:TValue(System.ReadOnlySpan{System.Byte},System.Text.Json.JsonSerializerOptions)
    public static void CustomEntrySubject_15()
    {
        try { var utf8 = Encoding.UTF8.GetBytes(TestJson); Assert.AreEqual(42, JsonSerializer.Deserialize<TestObj>((ReadOnlySpan<byte>)utf8, (JsonSerializerOptions)null!)!.X); }
        catch { _exitCode = 1; }
    }

    // [16] System.Text.Json/System.Text.Json.JsonSerializer::Deserialize:System.Object(System.ReadOnlySpan{System.Byte},System.Type,System.Text.Json.JsonSerializerOptions)
    public static void CustomEntrySubject_16()
    {
        try { var utf8 = Encoding.UTF8.GetBytes(TestJson); var r = (TestObj)JsonSerializer.Deserialize((ReadOnlySpan<byte>)utf8, typeof(TestObj), (JsonSerializerOptions)null!)!; Assert.AreEqual(42, r!.X); }
        catch { _exitCode = 1; }
    }

    // [17] System.Text.Json/System.Text.Json.JsonSerializer::Deserialize{TValue}:TValue(System.ReadOnlySpan{System.Byte},System.Text.Json.Serialization.Metadata.JsonTypeInfo{TValue})
    public static void CustomEntrySubject_17()
    {
        try { var utf8 = Encoding.UTF8.GetBytes(TestJson); var ti = (JsonTypeInfo<TestObj>)null!; Assert.Throws(() => JsonSerializer.Deserialize<TestObj>((ReadOnlySpan<byte>)utf8, ti)); }
        catch { _exitCode = 1; }
    }

    // [18] System.Text.Json/System.Text.Json.JsonSerializer::Deserialize:System.Object(System.ReadOnlySpan{System.Byte},System.Text.Json.Serialization.Metadata.JsonTypeInfo)
    public static void CustomEntrySubject_18()
    {
        try { var utf8 = Encoding.UTF8.GetBytes(TestJson); var ti = (JsonTypeInfo)null!; Assert.Throws(() => JsonSerializer.Deserialize((ReadOnlySpan<byte>)utf8, ti)); }
        catch { _exitCode = 1; }
    }

    // [19] System.Text.Json/System.Text.Json.JsonSerializer::Deserialize:System.Object(System.ReadOnlySpan{System.Byte},System.Type,System.Text.Json.Serialization.JsonSerializerContext)
    public static void CustomEntrySubject_19()
    {
        try { var utf8 = Encoding.UTF8.GetBytes(TestJson); var ctx = (JsonSerializerContext)null!; Assert.Throws(() => JsonSerializer.Deserialize((ReadOnlySpan<byte>)utf8, typeof(TestObj), ctx)); }
        catch { _exitCode = 1; }
    }

    // [20] System.Text.Json/System.Text.Json.JsonSerializer::Deserialize{TValue}:TValue(System.IO.Stream,System.Text.Json.JsonSerializerOptions)
    public static void CustomEntrySubject_20()
    {
        try { using var ms = new MemoryStream(Encoding.UTF8.GetBytes(TestJson)); Assert.AreEqual(42, JsonSerializer.Deserialize<TestObj>(ms, (JsonSerializerOptions)null!)!.X); }
        catch { _exitCode = 1; }
    }

    // [21] System.Text.Json/System.Text.Json.JsonSerializer::Deserialize:System.Object(System.IO.Stream,System.Type,System.Text.Json.JsonSerializerOptions)
    public static void CustomEntrySubject_21()
    {
        try { using var ms = new MemoryStream(Encoding.UTF8.GetBytes(TestJson)); var r = (TestObj)JsonSerializer.Deserialize(ms, typeof(TestObj), (JsonSerializerOptions)null!)!; Assert.AreEqual(42, r!.X); }
        catch { _exitCode = 1; }
    }

    // [22] System.Text.Json/System.Text.Json.JsonSerializer::Deserialize{TValue}:TValue(System.IO.Stream,System.Text.Json.Serialization.Metadata.JsonTypeInfo{TValue})
    public static void CustomEntrySubject_22()
    {
        try { using var ms = new MemoryStream(Encoding.UTF8.GetBytes(TestJson)); var ti = (JsonTypeInfo<TestObj>)null!; Assert.Throws(() => JsonSerializer.Deserialize<TestObj>(ms, ti)); }
        catch { _exitCode = 1; }
    }

    // [23] System.Text.Json/System.Text.Json.JsonSerializer::Deserialize:System.Object(System.IO.Stream,System.Text.Json.Serialization.Metadata.JsonTypeInfo)
    public static void CustomEntrySubject_23()
    {
        try { using var ms = new MemoryStream(Encoding.UTF8.GetBytes(TestJson)); var ti = (JsonTypeInfo)null!; Assert.Throws(() => JsonSerializer.Deserialize(ms, ti)); }
        catch { _exitCode = 1; }
    }

    // [24] System.Text.Json/System.Text.Json.JsonSerializer::Deserialize:System.Object(System.IO.Stream,System.Type,System.Text.Json.Serialization.JsonSerializerContext)
    public static void CustomEntrySubject_24()
    {
        try { using var ms = new MemoryStream(Encoding.UTF8.GetBytes(TestJson)); var ctx = (JsonSerializerContext)null!; Assert.Throws(() => JsonSerializer.Deserialize(ms, typeof(TestObj), ctx)); }
        catch { _exitCode = 1; }
    }

    // [25] System.Text.Json/System.Text.Json.JsonSerializer::Deserialize{TValue}:TValue(System.String,System.Text.Json.JsonSerializerOptions)
    public static void CustomEntrySubject_25()
    {
        try { Assert.AreEqual(42, JsonSerializer.Deserialize<TestObj>(TestJson, (JsonSerializerOptions)null!)!.X); }
        catch { _exitCode = 1; }
    }

    // [26] System.Text.Json/System.Text.Json.JsonSerializer::Deserialize{TValue}:TValue(System.ReadOnlySpan{System.Char},System.Text.Json.JsonSerializerOptions)
    public static void CustomEntrySubject_26()
    {
        try { Assert.AreEqual(42, JsonSerializer.Deserialize<TestObj>((ReadOnlySpan<char>)TestJson.ToCharArray(), (JsonSerializerOptions)null!)!.X); }
        catch { _exitCode = 1; }
    }

    // [27] System.Text.Json/System.Text.Json.JsonSerializer::Deserialize:System.Object(System.String,System.Type,System.Text.Json.JsonSerializerOptions)
    public static void CustomEntrySubject_27()
    {
        try { var r = (TestObj)JsonSerializer.Deserialize(TestJson, typeof(TestObj), (JsonSerializerOptions)null!)!; Assert.AreEqual(42, r!.X); }
        catch { _exitCode = 1; }
    }

    // [28] System.Text.Json/System.Text.Json.JsonSerializer::Deserialize:System.Object(System.ReadOnlySpan{System.Char},System.Type,System.Text.Json.JsonSerializerOptions)
    public static void CustomEntrySubject_28()
    {
        try { var r = (TestObj)JsonSerializer.Deserialize((ReadOnlySpan<char>)TestJson.ToCharArray(), typeof(TestObj), (JsonSerializerOptions)null!)!; Assert.AreEqual(42, r!.X); }
        catch { _exitCode = 1; }
    }

    // [29] System.Text.Json/System.Text.Json.JsonSerializer::Deserialize{TValue}:TValue(System.String,System.Text.Json.Serialization.Metadata.JsonTypeInfo{TValue})
    public static void CustomEntrySubject_29()
    {
        try { var ti = (JsonTypeInfo<TestObj>)null!; Assert.Throws(() => JsonSerializer.Deserialize<TestObj>(TestJson, ti)); }
        catch { _exitCode = 1; }
    }

    // [30] System.Text.Json/System.Text.Json.JsonSerializer::Deserialize{TValue}:TValue(System.ReadOnlySpan{System.Char},System.Text.Json.Serialization.Metadata.JsonTypeInfo{TValue})
    public static void CustomEntrySubject_30()
    {
        try { var ti = (JsonTypeInfo<TestObj>)null!; Assert.Throws(() => JsonSerializer.Deserialize<TestObj>((ReadOnlySpan<char>)TestJson.ToCharArray(), ti)); }
        catch { _exitCode = 1; }
    }

    // [31] System.Text.Json/System.Text.Json.JsonSerializer::Deserialize:System.Object(System.String,System.Text.Json.Serialization.Metadata.JsonTypeInfo)
    public static void CustomEntrySubject_31()
    {
        try { var ti = (JsonTypeInfo)null!; Assert.Throws(() => JsonSerializer.Deserialize(TestJson, ti)); }
        catch { _exitCode = 1; }
    }

    // [32] System.Text.Json/System.Text.Json.JsonSerializer::Deserialize:System.Object(System.ReadOnlySpan{System.Char},System.Text.Json.Serialization.Metadata.JsonTypeInfo)
    public static void CustomEntrySubject_32()
    {
        try { var ti = (JsonTypeInfo)null!; Assert.Throws(() => JsonSerializer.Deserialize((ReadOnlySpan<char>)TestJson.ToCharArray(), ti)); }
        catch { _exitCode = 1; }
    }

    // [33] System.Text.Json/System.Text.Json.JsonSerializer::Deserialize:System.Object(System.String,System.Type,System.Text.Json.Serialization.JsonSerializerContext)
    public static void CustomEntrySubject_33()
    {
        try { var ctx = (JsonSerializerContext)null!; Assert.Throws(() => JsonSerializer.Deserialize(TestJson, typeof(TestObj), ctx)); }
        catch { _exitCode = 1; }
    }

    // [34] System.Text.Json/System.Text.Json.JsonSerializer::Deserialize:System.Object(System.ReadOnlySpan{System.Char},System.Type,System.Text.Json.Serialization.JsonSerializerContext)
    public static void CustomEntrySubject_34()
    {
        try { var ctx = (JsonSerializerContext)null!; Assert.Throws(() => JsonSerializer.Deserialize((ReadOnlySpan<char>)TestJson.ToCharArray(), typeof(TestObj), ctx)); }
        catch { _exitCode = 1; }
    }

    // [35] System.Text.Json/System.Text.Json.JsonSerializer::Deserialize{TValue}:TValue(System.Text.Json.Utf8JsonReader&,System.Text.Json.JsonSerializerOptions)
    public static void CustomEntrySubject_35()
    {
        try { var utf8 = Encoding.UTF8.GetBytes(TestJson); var reader = new Utf8JsonReader(utf8); reader.Read(); Assert.AreEqual(42, JsonSerializer.Deserialize<TestObj>(ref reader, (JsonSerializerOptions)null!)!.X); }
        catch { _exitCode = 1; }
    }

    // [36] System.Text.Json/System.Text.Json.JsonSerializer::Deserialize:System.Object(System.Text.Json.Utf8JsonReader&,System.Type,System.Text.Json.JsonSerializerOptions)
    public static void CustomEntrySubject_36()
    {
        try { var utf8 = Encoding.UTF8.GetBytes(TestJson); var reader = new Utf8JsonReader(utf8); reader.Read(); var r = (TestObj)JsonSerializer.Deserialize(ref reader, typeof(TestObj), (JsonSerializerOptions)null!)!; Assert.AreEqual(42, r!.X); }
        catch { _exitCode = 1; }
    }

    // [37] System.Text.Json/System.Text.Json.JsonSerializer::Deserialize{TValue}:TValue(System.Text.Json.Utf8JsonReader&,System.Text.Json.Serialization.Metadata.JsonTypeInfo{TValue})
    public static void CustomEntrySubject_37()
    {
        try { Assert.Throws(() => { var utf8b = Encoding.UTF8.GetBytes(TestJson); var r = new Utf8JsonReader(utf8b); r.Read(); JsonSerializer.Deserialize<TestObj>(ref r, (JsonTypeInfo<TestObj>)null!); }); }
        catch { _exitCode = 1; }
    }

    // [38] System.Text.Json/System.Text.Json.JsonSerializer::Deserialize:System.Object(System.Text.Json.Utf8JsonReader&,System.Text.Json.Serialization.Metadata.JsonTypeInfo)
    public static void CustomEntrySubject_38()
    {
        try { Assert.Throws(() => { var utf8b = Encoding.UTF8.GetBytes(TestJson); var r = new Utf8JsonReader(utf8b); r.Read(); JsonSerializer.Deserialize(ref r, (JsonTypeInfo)null!); }); }
        catch { _exitCode = 1; }
    }

    // [39] System.Text.Json/System.Text.Json.JsonSerializer::Deserialize:System.Object(System.Text.Json.Utf8JsonReader&,System.Type,System.Text.Json.Serialization.JsonSerializerContext)
    public static void CustomEntrySubject_39()
    {
        try { Assert.Throws(() => { var utf8b = Encoding.UTF8.GetBytes(TestJson); var r = new Utf8JsonReader(utf8b); r.Read(); JsonSerializer.Deserialize(ref r, typeof(TestObj), (JsonSerializerContext)null!); }); }
        catch { _exitCode = 1; }
    }

    // [40] System.Text.Json/System.Text.Json.JsonSerializer::DeserializeAsync{TValue}:System.Threading.Tasks.ValueTask{TValue}(System.IO.Pipelines.PipeReader,System.Text.Json.JsonSerializerOptions,System.Threading.CancellationToken)
    public static void CustomEntrySubject_40()
    {
        try { var pipe = new Pipe(); var ti = (JsonTypeInfo<TestObj>)null!; Assert.Throws(() => pipe.Reader.TryRead(out _)); }
        catch { _exitCode = 1; }
    }

    // [41] System.Text.Json/System.Text.Json.JsonSerializer::DeserializeAsync{TValue}:System.Threading.Tasks.ValueTask{TValue}(System.IO.Pipelines.PipeReader,System.Text.Json.Serialization.Metadata.JsonTypeInfo{TValue},System.Threading.CancellationToken)
    public static void CustomEntrySubject_41()
    {
        try { var pipe = new Pipe(); var ti = (JsonTypeInfo<TestObj>)null!; Assert.Throws(() => pipe.Reader.TryRead(out _)); }
        catch { _exitCode = 1; }
    }

    // [42] System.Text.Json/System.Text.Json.JsonSerializer::DeserializeAsync:System.Threading.Tasks.ValueTask{System.Object}(System.IO.Pipelines.PipeReader,System.Text.Json.Serialization.Metadata.JsonTypeInfo,System.Threading.CancellationToken)
    public static void CustomEntrySubject_42()
    {
        try { var pipe = new Pipe(); var ti = (JsonTypeInfo)null!; Assert.Throws(() => pipe.Reader.TryRead(out _)); }
        catch { _exitCode = 1; }
    }

    // [43] System.Text.Json/System.Text.Json.JsonSerializer::DeserializeAsync:System.Threading.Tasks.ValueTask{System.Object}(System.IO.Pipelines.PipeReader,System.Type,System.Text.Json.Serialization.JsonSerializerContext,System.Threading.CancellationToken)
    public static void CustomEntrySubject_43()
    {
        try { var pipe = new Pipe(); var ctx = (JsonSerializerContext)null!; Assert.Throws(() => pipe.Reader.TryRead(out _)); }
        catch { _exitCode = 1; }
    }

    // [44] System.Text.Json/System.Text.Json.JsonSerializer::DeserializeAsync:System.Threading.Tasks.ValueTask{System.Object}(System.IO.Pipelines.PipeReader,System.Type,System.Text.Json.JsonSerializerOptions,System.Threading.CancellationToken)
    public static void CustomEntrySubject_44()
    {
        try { var pipe = new Pipe(); Assert.Throws(() => pipe.Reader.TryRead(out _)); }
        catch { _exitCode = 1; }
    }

    // [45] System.Text.Json/System.Text.Json.JsonSerializer::DeserializeAsync{TValue}:System.Threading.Tasks.ValueTask{TValue}(System.IO.Stream,System.Text.Json.JsonSerializerOptions,System.Threading.CancellationToken)
    public static void CustomEntrySubject_45()
    {
        try { using var ms = new MemoryStream(Encoding.UTF8.GetBytes(TestJson)); var v = JsonSerializer.DeserializeAsync<TestObj>(ms, (JsonSerializerOptions)null!); Assert.IsTrue(v.IsCompletedSuccessfully); }
        catch { _exitCode = 1; }
    }

    // [46] System.Text.Json/System.Text.Json.JsonSerializer::DeserializeAsync:System.Threading.Tasks.ValueTask{System.Object}(System.IO.Stream,System.Type,System.Text.Json.JsonSerializerOptions,System.Threading.CancellationToken)
    public static void CustomEntrySubject_46()
    {
        try { using var ms = new MemoryStream(Encoding.UTF8.GetBytes(TestJson)); var v = JsonSerializer.DeserializeAsync(ms, typeof(TestObj), (JsonSerializerOptions)null!); Assert.IsTrue(v.IsCompletedSuccessfully); }
        catch { _exitCode = 1; }
    }

    // [47] System.Text.Json/System.Text.Json.JsonSerializer::DeserializeAsync{TValue}:System.Threading.Tasks.ValueTask{TValue}(System.IO.Stream,System.Text.Json.Serialization.Metadata.JsonTypeInfo{TValue},System.Threading.CancellationToken)
    public static void CustomEntrySubject_47()
    {
        try { using var ms = new MemoryStream(Encoding.UTF8.GetBytes(TestJson)); var ti = (JsonTypeInfo<TestObj>)null!; Assert.Throws(() => JsonSerializer.DeserializeAsync<TestObj>(ms, ti)); }
        catch { _exitCode = 1; }
    }

    // [48] System.Text.Json/System.Text.Json.JsonSerializer::DeserializeAsync:System.Threading.Tasks.ValueTask{System.Object}(System.IO.Stream,System.Text.Json.Serialization.Metadata.JsonTypeInfo,System.Threading.CancellationToken)
    public static void CustomEntrySubject_48()
    {
        try { using var ms = new MemoryStream(Encoding.UTF8.GetBytes(TestJson)); var ti = (JsonTypeInfo)null!; Assert.Throws(() => JsonSerializer.DeserializeAsync(ms, ti)); }
        catch { _exitCode = 1; }
    }

    // [49] System.Text.Json/System.Text.Json.JsonSerializer::DeserializeAsync:System.Threading.Tasks.ValueTask{System.Object}(System.IO.Stream,System.Type,System.Text.Json.Serialization.JsonSerializerContext,System.Threading.CancellationToken)
    public static void CustomEntrySubject_49()
    {
        try { using var ms = new MemoryStream(Encoding.UTF8.GetBytes(TestJson)); var ctx = (JsonSerializerContext)null!; Assert.Throws(() => JsonSerializer.DeserializeAsync(ms, typeof(TestObj), ctx)); }
        catch { _exitCode = 1; }
    }

    // [50] System.Text.Json/System.Text.Json.JsonSerializer::DeserializeAsyncEnumerable{TValue}:System.Collections.Generic.IAsyncEnumerable{TValue}(System.IO.Pipelines.PipeReader,System.Text.Json.JsonSerializerOptions,System.Threading.CancellationToken)
    public static void CustomEntrySubject_50()
    {
        try { var pipe = new Pipe(); var enumerable = JsonSerializer.DeserializeAsyncEnumerable<TestObj>(pipe.Reader); Assert.IsNotNull(enumerable); }
        catch { _exitCode = 1; }
    }

    // [51] System.Text.Json/System.Text.Json.JsonSerializer::DeserializeAsyncEnumerable{TValue}:System.Collections.Generic.IAsyncEnumerable{TValue}(System.IO.Pipelines.PipeReader,System.Text.Json.Serialization.Metadata.JsonTypeInfo{TValue},System.Threading.CancellationToken)
    public static void CustomEntrySubject_51()
    {
        try { var pipe = new Pipe(); var ti = (JsonTypeInfo<TestObj>)null!; Assert.Throws(() => JsonSerializer.DeserializeAsyncEnumerable<TestObj>(pipe.Reader, ti)); }
        catch { _exitCode = 1; }
    }

    // [52] System.Text.Json/System.Text.Json.JsonSerializer::DeserializeAsyncEnumerable{TValue}:System.Collections.Generic.IAsyncEnumerable{TValue}(System.IO.Pipelines.PipeReader,System.Text.Json.Serialization.Metadata.JsonTypeInfo{TValue},System.Boolean,System.Threading.CancellationToken)
    public static void CustomEntrySubject_52()
    {
        try { var pipe = new Pipe(); var ti = (JsonTypeInfo<TestObj>)null!; Assert.Throws(() => JsonSerializer.DeserializeAsyncEnumerable<TestObj>(pipe.Reader, ti, false)); }
        catch { _exitCode = 1; }
    }

    // [53] System.Text.Json/System.Text.Json.JsonSerializer::DeserializeAsyncEnumerable{TValue}:System.Collections.Generic.IAsyncEnumerable{TValue}(System.IO.Pipelines.PipeReader,System.Boolean,System.Text.Json.JsonSerializerOptions,System.Threading.CancellationToken)
    public static void CustomEntrySubject_53()
    {
        try { var pipe = new Pipe(); var enumerable = JsonSerializer.DeserializeAsyncEnumerable<TestObj>(pipe.Reader, false); Assert.IsNotNull(enumerable); }
        catch { _exitCode = 1; }
    }

    // [54] System.Text.Json/System.Text.Json.JsonSerializer::DeserializeAsyncEnumerable{TValue}:System.Collections.Generic.IAsyncEnumerable{TValue}(System.IO.Stream,System.Text.Json.JsonSerializerOptions,System.Threading.CancellationToken)
    public static void CustomEntrySubject_54()
    {
        try { using var ms = new MemoryStream(Encoding.UTF8.GetBytes(TestJson)); var enumerable = JsonSerializer.DeserializeAsyncEnumerable<TestObj>(ms); Assert.IsNotNull(enumerable); }
        catch { _exitCode = 1; }
    }

    // [55] System.Text.Json/System.Text.Json.JsonSerializer::DeserializeAsyncEnumerable{TValue}:System.Collections.Generic.IAsyncEnumerable{TValue}(System.IO.Stream,System.Boolean,System.Text.Json.JsonSerializerOptions,System.Threading.CancellationToken)
    public static void CustomEntrySubject_55()
    {
        try { using var ms = new MemoryStream(Encoding.UTF8.GetBytes(TestJson)); var enumerable = JsonSerializer.DeserializeAsyncEnumerable<TestObj>(ms, false); Assert.IsNotNull(enumerable); }
        catch { _exitCode = 1; }
    }

    // [56] System.Text.Json/System.Text.Json.JsonSerializer::DeserializeAsyncEnumerable{TValue}:System.Collections.Generic.IAsyncEnumerable{TValue}(System.IO.Stream,System.Text.Json.Serialization.Metadata.JsonTypeInfo{TValue},System.Threading.CancellationToken)
    public static void CustomEntrySubject_56()
    {
        try { using var ms = new MemoryStream(Encoding.UTF8.GetBytes(TestJson)); var ti = (JsonTypeInfo<TestObj>)null!; Assert.Throws(() => JsonSerializer.DeserializeAsyncEnumerable<TestObj>(ms, ti)); }
        catch { _exitCode = 1; }
    }

    // [57] System.Text.Json/System.Text.Json.JsonSerializer::DeserializeAsyncEnumerable{TValue}:System.Collections.Generic.IAsyncEnumerable{TValue}(System.IO.Stream,System.Text.Json.Serialization.Metadata.JsonTypeInfo{TValue},System.Boolean,System.Threading.CancellationToken)
    public static void CustomEntrySubject_57()
    {
        try { using var ms = new MemoryStream(Encoding.UTF8.GetBytes(TestJson)); var ti = (JsonTypeInfo<TestObj>)null!; Assert.Throws(() => JsonSerializer.DeserializeAsyncEnumerable<TestObj>(ms, ti, false)); }
        catch { _exitCode = 1; }
    }

    // [58] System.Text.Json/System.Text.Json.JsonSerializer::get_IsReflectionEnabledByDefault:System.Boolean()
    public static void CustomEntrySubject_58()
    {
        try { Assert.IsFalse(JsonSerializer.IsReflectionEnabledByDefault); }
        catch { _exitCode = 1; }
    }

    // [59] System.Text.Json/System.Text.Json.JsonSerializer::Serialize{TValue}:System.Void(System.IO.Stream,TValue,System.Text.Json.JsonSerializerOptions)
    public static void CustomEntrySubject_59()
    {
        try { using var ms = new MemoryStream(); JsonSerializer.Serialize(ms, new TestObj { X = 42 }, (JsonSerializerOptions)null!); var bytes = ms.ToArray(); Assert.IsTrue(bytes.Length > 0); }
        catch { _exitCode = 1; }
    }

    // [60] System.Text.Json/System.Text.Json.JsonSerializer::Serialize:System.Void(System.IO.Stream,System.Object,System.Type,System.Text.Json.JsonSerializerOptions)
    public static void CustomEntrySubject_60()
    {
        try { using var ms = new MemoryStream(); JsonSerializer.Serialize(ms, new TestObj { X = 42 }, typeof(TestObj), (JsonSerializerOptions)null!); var bytes = ms.ToArray(); Assert.IsTrue(bytes.Length > 0); }
        catch { _exitCode = 1; }
    }

    // [61] System.Text.Json/System.Text.Json.JsonSerializer::Serialize{TValue}:System.Void(System.IO.Stream,TValue,System.Text.Json.Serialization.Metadata.JsonTypeInfo{TValue})
    public static void CustomEntrySubject_61()
    {
        try { using var ms = new MemoryStream(); var ti = (JsonTypeInfo<TestObj>)null!; Assert.Throws(() => JsonSerializer.Serialize(ms, new TestObj { X = 42 }, ti)); }
        catch { _exitCode = 1; }
    }

    // [62] System.Text.Json/System.Text.Json.JsonSerializer::Serialize:System.Void(System.IO.Stream,System.Object,System.Text.Json.Serialization.Metadata.JsonTypeInfo)
    public static void CustomEntrySubject_62()
    {
        try { using var ms = new MemoryStream(); var ti = (JsonTypeInfo)null!; Assert.Throws(() => JsonSerializer.Serialize(ms, new TestObj { X = 42 }, ti)); }
        catch { _exitCode = 1; }
    }

    // [63] System.Text.Json/System.Text.Json.JsonSerializer::Serialize:System.Void(System.IO.Stream,System.Object,System.Type,System.Text.Json.Serialization.JsonSerializerContext)
    public static void CustomEntrySubject_63()
    {
        try { using var ms = new MemoryStream(); var ctx = (JsonSerializerContext)null!; Assert.Throws(() => JsonSerializer.Serialize(ms, new TestObj { X = 42 }, typeof(TestObj), ctx)); }
        catch { _exitCode = 1; }
    }

    // [64] System.Text.Json/System.Text.Json.JsonSerializer::Serialize{TValue}:System.String(TValue,System.Text.Json.JsonSerializerOptions)
    public static void CustomEntrySubject_64()
    {
        try { var json = JsonSerializer.Serialize(new TestObj { X = 42 }, (JsonSerializerOptions)null!); Assert.IsTrue(json.Contains("42")); }
        catch { _exitCode = 1; }
    }

    // [65] System.Text.Json/System.Text.Json.JsonSerializer::Serialize:System.String(System.Object,System.Type,System.Text.Json.JsonSerializerOptions)
    public static void CustomEntrySubject_65()
    {
        try { var json = JsonSerializer.Serialize(new TestObj { X = 42 }, typeof(TestObj), (JsonSerializerOptions)null!); Assert.IsTrue(json.Contains("42")); }
        catch { _exitCode = 1; }
    }

    // [66] System.Text.Json/System.Text.Json.JsonSerializer::Serialize{TValue}:System.String(TValue,System.Text.Json.Serialization.Metadata.JsonTypeInfo{TValue})
    public static void CustomEntrySubject_66()
    {
        try { var ti = (JsonTypeInfo<TestObj>)null!; Assert.Throws(() => JsonSerializer.Serialize(new TestObj { X = 42 }, ti)); }
        catch { _exitCode = 1; }
    }

    // [67] System.Text.Json/System.Text.Json.JsonSerializer::Serialize:System.String(System.Object,System.Text.Json.Serialization.Metadata.JsonTypeInfo)
    public static void CustomEntrySubject_67()
    {
        try { var ti = (JsonTypeInfo)null!; Assert.Throws(() => JsonSerializer.Serialize(new TestObj { X = 42 }, ti)); }
        catch { _exitCode = 1; }
    }

    // [68] System.Text.Json/System.Text.Json.JsonSerializer::Serialize:System.String(System.Object,System.Type,System.Text.Json.Serialization.JsonSerializerContext)
    public static void CustomEntrySubject_68()
    {
        try { var ctx = (JsonSerializerContext)null!; Assert.Throws(() => JsonSerializer.Serialize(new TestObj { X = 42 }, typeof(TestObj), ctx)); }
        catch { _exitCode = 1; }
    }

    // [69] System.Text.Json/System.Text.Json.JsonSerializer::Serialize{TValue}:System.Void(System.Text.Json.Utf8JsonWriter,TValue,System.Text.Json.JsonSerializerOptions)
    public static void CustomEntrySubject_69()
    {
        try { using var ms = new MemoryStream(); using var writer = new Utf8JsonWriter(ms); JsonSerializer.Serialize(writer, new TestObj { X = 42 }, (JsonSerializerOptions)null!); writer.Flush(); Assert.IsTrue(ms.ToArray().Length > 0); }
        catch { _exitCode = 1; }
    }

    // [70] System.Text.Json/System.Text.Json.JsonSerializer::Serialize:System.Void(System.Text.Json.Utf8JsonWriter,System.Object,System.Type,System.Text.Json.JsonSerializerOptions)
    public static void CustomEntrySubject_70()
    {
        try { using var ms = new MemoryStream(); using var writer = new Utf8JsonWriter(ms); JsonSerializer.Serialize(writer, new TestObj { X = 42 }, typeof(TestObj), (JsonSerializerOptions)null!); writer.Flush(); Assert.IsTrue(ms.ToArray().Length > 0); }
        catch { _exitCode = 1; }
    }

    // [71] System.Text.Json/System.Text.Json.JsonSerializer::Serialize{TValue}:System.Void(System.Text.Json.Utf8JsonWriter,TValue,System.Text.Json.Serialization.Metadata.JsonTypeInfo{TValue})
    public static void CustomEntrySubject_71()
    {
        try { using var ms = new MemoryStream(); using var writer = new Utf8JsonWriter(ms); var ti = (JsonTypeInfo<TestObj>)null!; Assert.Throws(() => JsonSerializer.Serialize(writer, new TestObj { X = 42 }, ti)); }
        catch { _exitCode = 1; }
    }

    // [72] System.Text.Json/System.Text.Json.JsonSerializer::Serialize:System.Void(System.Text.Json.Utf8JsonWriter,System.Object,System.Text.Json.Serialization.Metadata.JsonTypeInfo)
    public static void CustomEntrySubject_72()
    {
        try { using var ms = new MemoryStream(); using var writer = new Utf8JsonWriter(ms); var ti = (JsonTypeInfo)null!; Assert.Throws(() => JsonSerializer.Serialize(writer, new TestObj { X = 42 }, ti)); }
        catch { _exitCode = 1; }
    }

    // [73] System.Text.Json/System.Text.Json.JsonSerializer::Serialize:System.Void(System.Text.Json.Utf8JsonWriter,System.Object,System.Type,System.Text.Json.Serialization.JsonSerializerContext)
    public static void CustomEntrySubject_73()
    {
        try { using var ms = new MemoryStream(); using var writer = new Utf8JsonWriter(ms); var ctx = (JsonSerializerContext)null!; Assert.Throws(() => JsonSerializer.Serialize(writer, new TestObj { X = 42 }, typeof(TestObj), ctx)); }
        catch { _exitCode = 1; }
    }

    // [74] System.Text.Json/System.Text.Json.JsonSerializer::SerializeAsync{TValue}:System.Threading.Tasks.Task(System.IO.Stream,TValue,System.Text.Json.JsonSerializerOptions,System.Threading.CancellationToken)
    public static void CustomEntrySubject_74()
    {
        try { using var ms = new MemoryStream(); var task = JsonSerializer.SerializeAsync(ms, new TestObj { X = 42 }, (JsonSerializerOptions)null!); Assert.IsTrue(task.Status == TaskStatus.RanToCompletion || task.Status == TaskStatus.WaitingForActivation); }
        catch { _exitCode = 1; }
    }

    // [75] System.Text.Json/System.Text.Json.JsonSerializer::SerializeAsync:System.Threading.Tasks.Task(System.IO.Stream,System.Object,System.Type,System.Text.Json.JsonSerializerOptions,System.Threading.CancellationToken)
    public static void CustomEntrySubject_75()
    {
        try { using var ms = new MemoryStream(); var task = JsonSerializer.SerializeAsync(ms, new TestObj { X = 42 }, typeof(TestObj), (JsonSerializerOptions)null!); Assert.IsTrue(task.Status == TaskStatus.RanToCompletion || task.Status == TaskStatus.WaitingForActivation); }
        catch { _exitCode = 1; }
    }

    // [76] System.Text.Json/System.Text.Json.JsonSerializer::SerializeAsync{TValue}:System.Threading.Tasks.Task(System.IO.Stream,TValue,System.Text.Json.Serialization.Metadata.JsonTypeInfo{TValue},System.Threading.CancellationToken)
    public static void CustomEntrySubject_76()
    {
        try { using var ms = new MemoryStream(); var ti = (JsonTypeInfo<TestObj>)null!; Assert.Throws(() => JsonSerializer.SerializeAsync(ms, new TestObj { X = 42 }, ti)); }
        catch { _exitCode = 1; }
    }

    // [77] System.Text.Json/System.Text.Json.JsonSerializer::SerializeAsync:System.Threading.Tasks.Task(System.IO.Stream,System.Object,System.Text.Json.Serialization.Metadata.JsonTypeInfo,System.Threading.CancellationToken)
    public static void CustomEntrySubject_77()
    {
        try { using var ms = new MemoryStream(); var ti = (JsonTypeInfo)null!; Assert.Throws(() => JsonSerializer.SerializeAsync(ms, new TestObj { X = 42 }, ti)); }
        catch { _exitCode = 1; }
    }

    // [78] System.Text.Json/System.Text.Json.JsonSerializer::SerializeAsync:System.Threading.Tasks.Task(System.IO.Stream,System.Object,System.Type,System.Text.Json.Serialization.JsonSerializerContext,System.Threading.CancellationToken)
    public static void CustomEntrySubject_78()
    {
        try { using var ms = new MemoryStream(); var ctx = (JsonSerializerContext)null!; Assert.Throws(() => JsonSerializer.SerializeAsync(ms, new TestObj { X = 42 }, typeof(TestObj), ctx)); }
        catch { _exitCode = 1; }
    }

    // [79] System.Text.Json/System.Text.Json.JsonSerializer::SerializeAsync{TValue}:System.Threading.Tasks.Task(System.IO.Pipelines.PipeWriter,TValue,System.Text.Json.Serialization.Metadata.JsonTypeInfo{TValue},System.Threading.CancellationToken)
    public static void CustomEntrySubject_79()
    {
        try { var pipe = new Pipe(); var ti = (JsonTypeInfo<TestObj>)null!; Assert.Throws(() => JsonSerializer.SerializeAsync(pipe.Writer, new TestObj { X = 42 }, ti)); }
        catch { _exitCode = 1; }
    }

    // [80] System.Text.Json/System.Text.Json.JsonSerializer::SerializeAsync{TValue}:System.Threading.Tasks.Task(System.IO.Pipelines.PipeWriter,TValue,System.Text.Json.JsonSerializerOptions,System.Threading.CancellationToken)
    public static void CustomEntrySubject_80()
    {
        try { var pipe = new Pipe(); var task = JsonSerializer.SerializeAsync(pipe.Writer, new TestObj { X = 42 }, (JsonSerializerOptions)null!); Assert.IsNotNull(task); }
        catch { _exitCode = 1; }
    }

    // [81] System.Text.Json/System.Text.Json.JsonSerializer::SerializeAsync:System.Threading.Tasks.Task(System.IO.Pipelines.PipeWriter,System.Object,System.Text.Json.Serialization.Metadata.JsonTypeInfo,System.Threading.CancellationToken)
    public static void CustomEntrySubject_81()
    {
        try { var pipe = new Pipe(); var ti = (JsonTypeInfo)null!; Assert.Throws(() => JsonSerializer.SerializeAsync(pipe.Writer, new TestObj { X = 42 }, ti)); }
        catch { _exitCode = 1; }
    }

    // [82] System.Text.Json/System.Text.Json.JsonSerializer::SerializeAsync:System.Threading.Tasks.Task(System.IO.Pipelines.PipeWriter,System.Object,System.Type,System.Text.Json.Serialization.JsonSerializerContext,System.Threading.CancellationToken)
    public static void CustomEntrySubject_82()
    {
        try { var pipe = new Pipe(); var ctx = (JsonSerializerContext)null!; Assert.Throws(() => JsonSerializer.SerializeAsync(pipe.Writer, new TestObj { X = 42 }, typeof(TestObj), ctx)); }
        catch { _exitCode = 1; }
    }

    // [83] System.Text.Json/System.Text.Json.JsonSerializer::SerializeAsync:System.Threading.Tasks.Task(System.IO.Pipelines.PipeWriter,System.Object,System.Type,System.Text.Json.JsonSerializerOptions,System.Threading.CancellationToken)
    public static void CustomEntrySubject_83()
    {
        try { var pipe = new Pipe(); var task = JsonSerializer.SerializeAsync(pipe.Writer, new TestObj { X = 42 }, typeof(TestObj), (JsonSerializerOptions)null!); Assert.IsNotNull(task); }
        catch { _exitCode = 1; }
    }

    // [84] System.Text.Json/System.Text.Json.JsonSerializer::SerializeToDocument{TValue}:System.Text.Json.JsonDocument(TValue,System.Text.Json.JsonSerializerOptions)
    public static void CustomEntrySubject_84()
    {
        try { using var doc = JsonSerializer.SerializeToDocument(new TestObj { X = 42 }, (JsonSerializerOptions)null!); Assert.AreEqual(42, doc.RootElement.GetProperty("X").GetInt32()); }
        catch { _exitCode = 1; }
    }

    // [85] System.Text.Json/System.Text.Json.JsonSerializer::SerializeToDocument:System.Text.Json.JsonDocument(System.Object,System.Type,System.Text.Json.JsonSerializerOptions)
    public static void CustomEntrySubject_85()
    {
        try { using var doc = JsonSerializer.SerializeToDocument(new TestObj { X = 42 }, typeof(TestObj), (JsonSerializerOptions)null!); Assert.AreEqual(42, doc.RootElement.GetProperty("X").GetInt32()); }
        catch { _exitCode = 1; }
    }

    // [86] System.Text.Json/System.Text.Json.JsonSerializer::SerializeToDocument{TValue}:System.Text.Json.JsonDocument(TValue,System.Text.Json.Serialization.Metadata.JsonTypeInfo{TValue})
    public static void CustomEntrySubject_86()
    {
        try { var ti = (JsonTypeInfo<TestObj>)null!; Assert.Throws(() => JsonSerializer.SerializeToDocument(new TestObj { X = 42 }, ti)); }
        catch { _exitCode = 1; }
    }

    // [87] System.Text.Json/System.Text.Json.JsonSerializer::SerializeToDocument:System.Text.Json.JsonDocument(System.Object,System.Text.Json.Serialization.Metadata.JsonTypeInfo)
    public static void CustomEntrySubject_87()
    {
        try { var ti = (JsonTypeInfo)null!; Assert.Throws(() => JsonSerializer.SerializeToDocument(new TestObj { X = 42 }, ti)); }
        catch { _exitCode = 1; }
    }

    // [88] System.Text.Json/System.Text.Json.JsonSerializer::SerializeToDocument:System.Text.Json.JsonDocument(System.Object,System.Type,System.Text.Json.Serialization.JsonSerializerContext)
    public static void CustomEntrySubject_88()
    {
        try { var ctx = (JsonSerializerContext)null!; Assert.Throws(() => JsonSerializer.SerializeToDocument(new TestObj { X = 42 }, typeof(TestObj), ctx)); }
        catch { _exitCode = 1; }
    }

    // [89] System.Text.Json/System.Text.Json.JsonSerializer::SerializeToElement{TValue}:System.Text.Json.JsonElement(TValue,System.Text.Json.JsonSerializerOptions)
    public static void CustomEntrySubject_89()
    {
        try { var el = JsonSerializer.SerializeToElement(new TestObj { X = 42 }, (JsonSerializerOptions)null!); Assert.AreEqual(42, el.GetProperty("X").GetInt32()); }
        catch { _exitCode = 1; }
    }

    // [90] System.Text.Json/System.Text.Json.JsonSerializer::SerializeToElement:System.Text.Json.JsonElement(System.Object,System.Type,System.Text.Json.JsonSerializerOptions)
    public static void CustomEntrySubject_90()
    {
        try { var el = JsonSerializer.SerializeToElement(new TestObj { X = 42 }, typeof(TestObj), (JsonSerializerOptions)null!); Assert.AreEqual(42, el.GetProperty("X").GetInt32()); }
        catch { _exitCode = 1; }
    }

    // [91] System.Text.Json/System.Text.Json.JsonSerializer::SerializeToElement{TValue}:System.Text.Json.JsonElement(TValue,System.Text.Json.Serialization.Metadata.JsonTypeInfo{TValue})
    public static void CustomEntrySubject_91()
    {
        try { var ti = (JsonTypeInfo<TestObj>)null!; Assert.Throws(() => JsonSerializer.SerializeToElement(new TestObj { X = 42 }, ti)); }
        catch { _exitCode = 1; }
    }

    // [92] System.Text.Json/System.Text.Json.JsonSerializer::SerializeToElement:System.Text.Json.JsonElement(System.Object,System.Text.Json.Serialization.Metadata.JsonTypeInfo)
    public static void CustomEntrySubject_92()
    {
        try { var ti = (JsonTypeInfo)null!; Assert.Throws(() => JsonSerializer.SerializeToElement(new TestObj { X = 42 }, ti)); }
        catch { _exitCode = 1; }
    }

    // [93] System.Text.Json/System.Text.Json.JsonSerializer::SerializeToElement:System.Text.Json.JsonElement(System.Object,System.Type,System.Text.Json.Serialization.JsonSerializerContext)
    public static void CustomEntrySubject_93()
    {
        try { var ctx = (JsonSerializerContext)null!; Assert.Throws(() => JsonSerializer.SerializeToElement(new TestObj { X = 42 }, typeof(TestObj), ctx)); }
        catch { _exitCode = 1; }
    }

    // [94] System.Text.Json/System.Text.Json.JsonSerializer::SerializeToNode{TValue}:System.Text.Json.Nodes.JsonNode(TValue,System.Text.Json.JsonSerializerOptions)
    public static void CustomEntrySubject_94()
    {
        try { var node = JsonSerializer.SerializeToNode(new TestObj { X = 42 }, (JsonSerializerOptions)null!); Assert.AreEqual(42, (int)node!["X"]!); }
        catch { _exitCode = 1; }
    }

    // [95] System.Text.Json/System.Text.Json.JsonSerializer::SerializeToNode:System.Text.Json.Nodes.JsonNode(System.Object,System.Type,System.Text.Json.JsonSerializerOptions)
    public static void CustomEntrySubject_95()
    {
        try { var node = JsonSerializer.SerializeToNode(new TestObj { X = 42 }, typeof(TestObj), (JsonSerializerOptions)null!); Assert.AreEqual(42, (int)node!["X"]!); }
        catch { _exitCode = 1; }
    }

    // [96] System.Text.Json/System.Text.Json.JsonSerializer::SerializeToNode{TValue}:System.Text.Json.Nodes.JsonNode(TValue,System.Text.Json.Serialization.Metadata.JsonTypeInfo{TValue})
    public static void CustomEntrySubject_96()
    {
        try { var ti = (JsonTypeInfo<TestObj>)null!; Assert.Throws(() => JsonSerializer.SerializeToNode(new TestObj { X = 42 }, ti)); }
        catch { _exitCode = 1; }
    }

    // [97] System.Text.Json/System.Text.Json.JsonSerializer::SerializeToNode:System.Text.Json.Nodes.JsonNode(System.Object,System.Text.Json.Serialization.Metadata.JsonTypeInfo)
    public static void CustomEntrySubject_97()
    {
        try { var ti = (JsonTypeInfo)null!; Assert.Throws(() => JsonSerializer.SerializeToNode(new TestObj { X = 42 }, ti)); }
        catch { _exitCode = 1; }
    }

    // [98] System.Text.Json/System.Text.Json.JsonSerializer::SerializeToNode:System.Text.Json.Nodes.JsonNode(System.Object,System.Type,System.Text.Json.Serialization.JsonSerializerContext)
    public static void CustomEntrySubject_98()
    {
        try { var ctx = (JsonSerializerContext)null!; Assert.Throws(() => JsonSerializer.SerializeToNode(new TestObj { X = 42 }, typeof(TestObj), ctx)); }
        catch { _exitCode = 1; }
    }

    // [99] System.Text.Json/System.Text.Json.JsonSerializer::SerializeToUtf8Bytes{TValue}:System.Byte{}(TValue,System.Text.Json.JsonSerializerOptions)
    public static void CustomEntrySubject_99()
    {
        try { var bytes = JsonSerializer.SerializeToUtf8Bytes(new TestObj { X = 42 }, (JsonSerializerOptions)null!); Assert.IsTrue(bytes.Length > 0); }
        catch { _exitCode = 1; }
    }

    // [100] System.Text.Json/System.Text.Json.JsonSerializer::SerializeToUtf8Bytes:System.Byte{}(System.Object,System.Type,System.Text.Json.JsonSerializerOptions)
    public static void CustomEntrySubject_100()
    {
        try { var bytes = JsonSerializer.SerializeToUtf8Bytes(new TestObj { X = 42 }, typeof(TestObj), (JsonSerializerOptions)null!); Assert.IsTrue(bytes.Length > 0); }
        catch { _exitCode = 1; }
    }

    // [101] System.Text.Json/System.Text.Json.JsonSerializer::SerializeToUtf8Bytes{TValue}:System.Byte{}(TValue,System.Text.Json.Serialization.Metadata.JsonTypeInfo{TValue})
    public static void CustomEntrySubject_101()
    {
        try { var ti = (JsonTypeInfo<TestObj>)null!; Assert.Throws(() => JsonSerializer.SerializeToUtf8Bytes(new TestObj { X = 42 }, ti)); }
        catch { _exitCode = 1; }
    }

    // [102] System.Text.Json/System.Text.Json.JsonSerializer::SerializeToUtf8Bytes:System.Byte{}(System.Object,System.Text.Json.Serialization.Metadata.JsonTypeInfo)
    public static void CustomEntrySubject_102()
    {
        try { var ti = (JsonTypeInfo)null!; Assert.Throws(() => JsonSerializer.SerializeToUtf8Bytes(new TestObj { X = 42 }, ti)); }
        catch { _exitCode = 1; }
    }

    // [103] System.Text.Json/System.Text.Json.JsonSerializer::SerializeToUtf8Bytes:System.Byte{}(System.Object,System.Type,System.Text.Json.Serialization.JsonSerializerContext)
    public static void CustomEntrySubject_103()
    {
        try { var ctx = (JsonSerializerContext)null!; Assert.Throws(() => JsonSerializer.SerializeToUtf8Bytes(new TestObj { X = 42 }, typeof(TestObj), ctx)); }
        catch { _exitCode = 1; }
    }
}
