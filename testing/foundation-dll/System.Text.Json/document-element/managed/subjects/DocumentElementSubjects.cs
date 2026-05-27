// Auto-generated native-AOT entry point
// Family: family/System.Text.Json/document/element
// Assembly: System.Text.Json
// Variant: subjects

using System;
using System.Buffers;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Text.Json;
using System.Threading;
using System.Threading.Tasks;
using Chaos.TestFramework;

public static partial class DocumentElementSubjects
{
    // Inlined exit code — avoids SDK method call resolution in codegen
    public static int _exitCode;

    private static readonly string TestJson = "{\"X\":42,\"Y\":0,\"name\":\"hello\"}";

    // [0] System.Text.Json/System.Text.Json.JsonDocument::Dispose:System.Void()
    public static void CustomEntrySubject_0()
    {
        try { var doc = JsonDocument.Parse(TestJson); doc.Dispose(); Assert.IsTrue(true); }
        catch { _exitCode = 1; }
    }

    // [1] System.Text.Json/System.Text.Json.JsonDocument::get_RootElement:System.Text.Json.JsonElement()
    public static void CustomEntrySubject_1()
    {
        try { using var doc = JsonDocument.Parse(TestJson); var el = doc.RootElement; Assert.AreEqual((int)JsonValueKind.Object, (int)el.ValueKind); }
        catch { _exitCode = 1; }
    }

    // [2] System.Text.Json/System.Text.Json.JsonDocument::Parse:System.Text.Json.JsonDocument(System.ReadOnlyMemory{System.Byte},System.Text.Json.JsonDocumentOptions)
    public static void CustomEntrySubject_2()
    {
        try { using var doc = JsonDocument.Parse((ReadOnlyMemory<byte>)Encoding.UTF8.GetBytes(TestJson)); Assert.AreEqual(42, doc.RootElement.GetProperty("X").GetInt32()); }
        catch { _exitCode = 1; }
    }

    // [3] System.Text.Json/System.Text.Json.JsonDocument::Parse:System.Text.Json.JsonDocument(System.Buffers.ReadOnlySequence{System.Byte},System.Text.Json.JsonDocumentOptions)
    public static void CustomEntrySubject_3()
    {
        try { var seq = new ReadOnlySequence<byte>(Encoding.UTF8.GetBytes(TestJson)); using var doc = JsonDocument.Parse(seq); Assert.AreEqual(42, doc.RootElement.GetProperty("X").GetInt32()); }
        catch { _exitCode = 1; }
    }

    // [4] System.Text.Json/System.Text.Json.JsonDocument::Parse:System.Text.Json.JsonDocument(System.IO.Stream,System.Text.Json.JsonDocumentOptions)
    public static void CustomEntrySubject_4()
    {
        try { using var ms = new MemoryStream(Encoding.UTF8.GetBytes(TestJson)); using var doc = JsonDocument.Parse(ms); Assert.AreEqual(42, doc.RootElement.GetProperty("X").GetInt32()); }
        catch { _exitCode = 1; }
    }

    // [5] System.Text.Json/System.Text.Json.JsonDocument::Parse:System.Text.Json.JsonDocument(System.ReadOnlyMemory{System.Char},System.Text.Json.JsonDocumentOptions)
    public static void CustomEntrySubject_5()
    {
        try { using var doc = JsonDocument.Parse((ReadOnlyMemory<char>)TestJson.ToCharArray()); Assert.AreEqual(42, doc.RootElement.GetProperty("X").GetInt32()); }
        catch { _exitCode = 1; }
    }

    // [6] System.Text.Json/System.Text.Json.JsonDocument::Parse:System.Text.Json.JsonDocument(System.String,System.Text.Json.JsonDocumentOptions)
    public static void CustomEntrySubject_6()
    {
        try { using var doc = JsonDocument.Parse(TestJson); Assert.AreEqual(42, doc.RootElement.GetProperty("X").GetInt32()); }
        catch { _exitCode = 1; }
    }

    // [7] System.Text.Json/System.Text.Json.JsonDocument::ParseAsync:System.Threading.Tasks.Task{System.Text.Json.JsonDocument}(System.IO.Stream,System.Text.Json.JsonDocumentOptions,System.Threading.CancellationToken)
    public static void CustomEntrySubject_7()
    {
        try { using var ms = new MemoryStream(Encoding.UTF8.GetBytes(TestJson)); var t = JsonDocument.ParseAsync(ms); Assert.IsTrue(t.IsCompletedSuccessfully); }
        catch { _exitCode = 1; }
    }

    // [8] System.Text.Json/System.Text.Json.JsonDocument::ParseValue:System.Text.Json.JsonDocument(System.Text.Json.Utf8JsonReader&)
    public static void CustomEntrySubject_8()
    {
        try { var reader = new Utf8JsonReader(Encoding.UTF8.GetBytes(TestJson)); using var doc = JsonDocument.ParseValue(ref reader); Assert.AreEqual(42, doc.RootElement.GetProperty("X").GetInt32()); }
        catch { _exitCode = 1; }
    }

    // [9] System.Text.Json/System.Text.Json.JsonDocument::TryParseValue:System.Boolean(System.Text.Json.Utf8JsonReader&,System.Text.Json.JsonDocument&)
    public static void CustomEntrySubject_9()
    {
        try { var reader = new Utf8JsonReader(Encoding.UTF8.GetBytes(TestJson)); Assert.IsTrue(JsonDocument.TryParseValue(ref reader, out JsonDocument? doc)); doc!.Dispose(); }
        catch { _exitCode = 1; }
    }

    // [10] System.Text.Json/System.Text.Json.JsonDocument::WriteTo:System.Void(System.Text.Json.Utf8JsonWriter)
    public static void CustomEntrySubject_10()
    {
        try { using var doc = JsonDocument.Parse(TestJson); var ms = new MemoryStream(); using var writer = new Utf8JsonWriter(ms); doc.WriteTo(writer); writer.Flush(); Assert.IsTrue(ms.ToArray().Length > 0); }
        catch { _exitCode = 1; }
    }

    // [11] System.Text.Json/System.Text.Json.JsonElement::Clone:System.Text.Json.JsonElement()
    public static void CustomEntrySubject_11()
    {
        try { using var doc = JsonDocument.Parse(TestJson); var el = doc.RootElement.Clone(); Assert.AreEqual(42, el.GetProperty("X").GetInt32()); }
        catch { _exitCode = 1; }
    }

    // [12] System.Text.Json/System.Text.Json.JsonElement::DeepEquals:System.Boolean(System.Text.Json.JsonElement,System.Text.Json.JsonElement)
    public static void CustomEntrySubject_12()
    {
        try { using var doc = JsonDocument.Parse(TestJson); Assert.IsTrue(JsonElement.DeepEquals(doc.RootElement, doc.RootElement)); }
        catch { _exitCode = 1; }
    }

    // [13] System.Text.Json/System.Text.Json.JsonElement::EnumerateArray:System.Text.Json.JsonElement+ArrayEnumerator()
    public static void CustomEntrySubject_13()
    {
        try { using var doc = JsonDocument.Parse("[1,2,3]"); var e = doc.RootElement.EnumerateArray(); Assert.IsTrue(e.Count() == 3); }
        catch { _exitCode = 1; }
    }

    // [14] System.Text.Json/System.Text.Json.JsonElement::EnumerateObject:System.Text.Json.JsonElement+ObjectEnumerator()
    public static void CustomEntrySubject_14()
    {
        try { using var doc = JsonDocument.Parse(TestJson); var e = doc.RootElement.EnumerateObject(); Assert.IsTrue(e.Count() > 0); }
        catch { _exitCode = 1; }
    }

    // [15] System.Text.Json/System.Text.Json.JsonElement::get_Item:System.Text.Json.JsonElement(System.Int32)
    public static void CustomEntrySubject_15()
    {
        try { using var doc = JsonDocument.Parse("[10,20,30]"); Assert.AreEqual(20, doc.RootElement[1].GetInt32()); }
        catch { _exitCode = 1; }
    }

    // [16] System.Text.Json/System.Text.Json.JsonElement::get_ValueKind:System.Text.Json.JsonValueKind()
    public static void CustomEntrySubject_16()
    {
        try { using var doc = JsonDocument.Parse(TestJson); Assert.AreEqual((int)JsonValueKind.Object, (int)doc.RootElement.ValueKind); }
        catch { _exitCode = 1; }
    }

    // [17] System.Text.Json/System.Text.Json.JsonElement::GetArrayLength:System.Int32()
    public static void CustomEntrySubject_17()
    {
        try { using var doc = JsonDocument.Parse("[1,2,3]"); Assert.AreEqual(3, doc.RootElement.GetArrayLength()); }
        catch { _exitCode = 1; }
    }

    // [18] System.Text.Json/System.Text.Json.JsonElement::GetBoolean:System.Boolean()
    public static void CustomEntrySubject_18()
    {
        try { using var doc = JsonDocument.Parse("true"); Assert.IsTrue(doc.RootElement.GetBoolean()); }
        catch { _exitCode = 1; }
    }

    // [19] System.Text.Json/System.Text.Json.JsonElement::GetByte:System.Byte()
    public static void CustomEntrySubject_19()
    {
        try { using var doc = JsonDocument.Parse("42"); Assert.AreEqual((byte)42, doc.RootElement.GetByte()); }
        catch { _exitCode = 1; }
    }

    // [20] System.Text.Json/System.Text.Json.JsonElement::GetBytesFromBase64:System.Byte{}()
    public static void CustomEntrySubject_20()
    {
        try { using var doc = JsonDocument.Parse("\"AQUIDBAUG\""); var b = doc.RootElement.GetBytesFromBase64(); Assert.IsTrue(b.Length > 0); }
        catch { _exitCode = 1; }
    }

    // [21] System.Text.Json/System.Text.Json.JsonElement::GetDateTime:System.DateTime()
    public static void CustomEntrySubject_21()
    {
        try { using var doc = JsonDocument.Parse("\"2024-01-01T00:00:00\""); Assert.AreEqual(2024, doc.RootElement.GetDateTime().Year); }
        catch { _exitCode = 1; }
    }

    // [22] System.Text.Json/System.Text.Json.JsonElement::GetDateTimeOffset:System.DateTimeOffset()
    public static void CustomEntrySubject_22()
    {
        try { using var doc = JsonDocument.Parse("\"2024-01-01T00:00:00Z\""); Assert.AreEqual(2024, doc.RootElement.GetDateTimeOffset().Year); }
        catch { _exitCode = 1; }
    }

    // [23] System.Text.Json/System.Text.Json.JsonElement::GetDecimal:System.Decimal()
    public static void CustomEntrySubject_23()
    {
        try { using var doc = JsonDocument.Parse("3.14"); Assert.AreEqual(3.14m, doc.RootElement.GetDecimal()); }
        catch { _exitCode = 1; }
    }

    // [24] System.Text.Json/System.Text.Json.JsonElement::GetDouble:System.Double()
    public static void CustomEntrySubject_24()
    {
        try { using var doc = JsonDocument.Parse("3.14"); Assert.AreEqual(3.14, doc.RootElement.GetDouble()); }
        catch { _exitCode = 1; }
    }

    // [25] System.Text.Json/System.Text.Json.JsonElement::GetGuid:System.Guid()
    public static void CustomEntrySubject_25()
    {
        try { using var doc = JsonDocument.Parse("\"00000001-0002-0003-0004-000000000005\""); var g = doc.RootElement.GetGuid(); Assert.IsTrue(true); }
        catch { _exitCode = 1; }
    }

    // [26] System.Text.Json/System.Text.Json.JsonElement::GetInt16:System.Int16()
    public static void CustomEntrySubject_26()
    {
        try { using var doc = JsonDocument.Parse("42"); Assert.AreEqual((short)42, doc.RootElement.GetInt16()); }
        catch { _exitCode = 1; }
    }

    // [27] System.Text.Json/System.Text.Json.JsonElement::GetInt32:System.Int32()
    public static void CustomEntrySubject_27()
    {
        try { using var doc = JsonDocument.Parse(TestJson); Assert.AreEqual(42, doc.RootElement.GetProperty("X").GetInt32()); }
        catch { _exitCode = 1; }
    }

    // [28] System.Text.Json/System.Text.Json.JsonElement::GetInt64:System.Int64()
    public static void CustomEntrySubject_28()
    {
        try { using var doc = JsonDocument.Parse("42"); Assert.AreEqual(42L, doc.RootElement.GetInt64()); }
        catch { _exitCode = 1; }
    }

    // [29] System.Text.Json/System.Text.Json.JsonElement::GetProperty:System.Text.Json.JsonElement(System.String)
    public static void CustomEntrySubject_29()
    {
        try { using var doc = JsonDocument.Parse(TestJson); Assert.AreEqual(42, doc.RootElement.GetProperty("X").GetInt32()); }
        catch { _exitCode = 1; }
    }

    // [30] System.Text.Json/System.Text.Json.JsonElement::GetProperty:System.Text.Json.JsonElement(System.ReadOnlySpan{System.Char})
    public static void CustomEntrySubject_30()
    {
        try { using var doc = JsonDocument.Parse(TestJson); Assert.AreEqual(42, doc.RootElement.GetProperty((ReadOnlySpan<char>)"X".ToCharArray()).GetInt32()); }
        catch { _exitCode = 1; }
    }

    // [31] System.Text.Json/System.Text.Json.JsonElement::GetProperty:System.Text.Json.JsonElement(System.ReadOnlySpan{System.Byte})
    public static void CustomEntrySubject_31()
    {
        try { using var doc = JsonDocument.Parse(TestJson); Assert.AreEqual(42, doc.RootElement.GetProperty((ReadOnlySpan<byte>)Encoding.UTF8.GetBytes("X")).GetInt32()); }
        catch { _exitCode = 1; }
    }

    // [32] System.Text.Json/System.Text.Json.JsonElement::GetPropertyCount:System.Int32()
    public static void CustomEntrySubject_32()
    {
        try { using var doc = JsonDocument.Parse(TestJson); Assert.AreEqual(3, doc.RootElement.GetPropertyCount()); }
        catch { _exitCode = 1; }
    }

    // [33] System.Text.Json/System.Text.Json.JsonElement::GetRawText:System.String()
    public static void CustomEntrySubject_33()
    {
        try { using var doc = JsonDocument.Parse(TestJson); var s = doc.RootElement.GetRawText(); Assert.IsTrue(s.Contains("X")); }
        catch { _exitCode = 1; }
    }

    // [34] System.Text.Json/System.Text.Json.JsonElement::GetSByte:System.SByte()
    public static void CustomEntrySubject_34()
    {
        try { using var doc = JsonDocument.Parse("42"); Assert.AreEqual((sbyte)42, doc.RootElement.GetSByte()); }
        catch { _exitCode = 1; }
    }

    // [35] System.Text.Json/System.Text.Json.JsonElement::GetSingle:System.Single()
    public static void CustomEntrySubject_35()
    {
        try { using var doc = JsonDocument.Parse("3.14"); Assert.AreEqual(3.14f, doc.RootElement.GetSingle()); }
        catch { _exitCode = 1; }
    }

    // [36] System.Text.Json/System.Text.Json.JsonElement::GetString:System.String()
    public static void CustomEntrySubject_36()
    {
        try { using var doc = JsonDocument.Parse(TestJson); Assert.AreEqual("hello", doc.RootElement.GetProperty("name").GetString()); }
        catch { _exitCode = 1; }
    }

    // [37] System.Text.Json/System.Text.Json.JsonElement::GetUInt16:System.UInt16()
    public static void CustomEntrySubject_37()
    {
        try { using var doc = JsonDocument.Parse("42"); Assert.AreEqual((ushort)42, doc.RootElement.GetUInt16()); }
        catch { _exitCode = 1; }
    }

    // [38] System.Text.Json/System.Text.Json.JsonElement::GetUInt32:System.UInt32()
    public static void CustomEntrySubject_38()
    {
        try { using var doc = JsonDocument.Parse("42"); Assert.AreEqual(42u, doc.RootElement.GetUInt32()); }
        catch { _exitCode = 1; }
    }

    // [39] System.Text.Json/System.Text.Json.JsonElement::GetUInt64:System.UInt64()
    public static void CustomEntrySubject_39()
    {
        try { using var doc = JsonDocument.Parse("42"); Assert.AreEqual(42UL, doc.RootElement.GetUInt64()); }
        catch { _exitCode = 1; }
    }

    // [40] System.Text.Json/System.Text.Json.JsonElement::Parse:System.Text.Json.JsonElement(System.ReadOnlySpan{System.Byte},System.Text.Json.JsonDocumentOptions)
    public static void CustomEntrySubject_40()
    {
        try { var el = JsonElement.Parse(Encoding.UTF8.GetBytes(TestJson)); Assert.AreEqual(42, el.GetProperty("X").GetInt32()); }
        catch { _exitCode = 1; }
    }

    // [41] System.Text.Json/System.Text.Json.JsonElement::Parse:System.Text.Json.JsonElement(System.ReadOnlySpan{System.Char},System.Text.Json.JsonDocumentOptions)
    public static void CustomEntrySubject_41()
    {
        try { var el = JsonElement.Parse((ReadOnlySpan<char>)TestJson.ToCharArray()); Assert.AreEqual(42, el.GetProperty("X").GetInt32()); }
        catch { _exitCode = 1; }
    }

    // [42] System.Text.Json/System.Text.Json.JsonElement::Parse:System.Text.Json.JsonElement(System.String,System.Text.Json.JsonDocumentOptions)
    public static void CustomEntrySubject_42()
    {
        try { var el = JsonElement.Parse(TestJson); Assert.AreEqual(42, el.GetProperty("X").GetInt32()); }
        catch { _exitCode = 1; }
    }

    // [43] System.Text.Json/System.Text.Json.JsonElement::ParseValue:System.Text.Json.JsonElement(System.Text.Json.Utf8JsonReader&)
    public static void CustomEntrySubject_43()
    {
        try { var reader = new Utf8JsonReader(Encoding.UTF8.GetBytes("42")); var el = JsonElement.ParseValue(ref reader); Assert.AreEqual(42, el.GetInt32()); }
        catch { _exitCode = 1; }
    }

    // [44] System.Text.Json/System.Text.Json.JsonElement::ToString:System.String()
    public static void CustomEntrySubject_44()
    {
        try { using var doc = JsonDocument.Parse("42"); Assert.AreEqual("42", doc.RootElement.ToString()); }
        catch { _exitCode = 1; }
    }

    // [45] System.Text.Json/System.Text.Json.JsonElement::TryGetByte:System.Boolean(System.Byte&)
    public static void CustomEntrySubject_45()
    {
        try { using var doc = JsonDocument.Parse("42"); Assert.IsTrue(doc.RootElement.TryGetByte(out byte v)); Assert.AreEqual((byte)42, v); }
        catch { _exitCode = 1; }
    }

    // [46] System.Text.Json/System.Text.Json.JsonElement::TryGetBytesFromBase64:System.Boolean(System.Byte{}&)
    public static void CustomEntrySubject_46()
    {
        try { using var doc = JsonDocument.Parse("\"AQUIDBAUG\""); Assert.IsTrue(doc.RootElement.TryGetBytesFromBase64(out byte[]? b)); Assert.IsTrue(b!.Length > 0); }
        catch { _exitCode = 1; }
    }

    // [47] System.Text.Json/System.Text.Json.JsonElement::TryGetDateTime:System.Boolean(System.DateTime&)
    public static void CustomEntrySubject_47()
    {
        try { using var doc = JsonDocument.Parse("\"2024-01-01T00:00:00\""); Assert.IsTrue(doc.RootElement.TryGetDateTime(out DateTime d)); Assert.AreEqual(2024, d.Year); }
        catch { _exitCode = 1; }
    }

    // [48] System.Text.Json/System.Text.Json.JsonElement::TryGetDateTimeOffset:System.Boolean(System.DateTimeOffset&)
    public static void CustomEntrySubject_48()
    {
        try { using var doc = JsonDocument.Parse("\"2024-01-01T00:00:00Z\""); Assert.IsTrue(doc.RootElement.TryGetDateTimeOffset(out DateTimeOffset d)); Assert.AreEqual(2024, d.Year); }
        catch { _exitCode = 1; }
    }

    // [49] System.Text.Json/System.Text.Json.JsonElement::TryGetDecimal:System.Boolean(System.Decimal&)
    public static void CustomEntrySubject_49()
    {
        try { using var doc = JsonDocument.Parse("3.14"); Assert.IsTrue(doc.RootElement.TryGetDecimal(out decimal v)); Assert.AreEqual(3.14m, v); }
        catch { _exitCode = 1; }
    }

    // [50] System.Text.Json/System.Text.Json.JsonElement::TryGetDouble:System.Boolean(System.Double&)
    public static void CustomEntrySubject_50()
    {
        try { using var doc = JsonDocument.Parse("3.14"); Assert.IsTrue(doc.RootElement.TryGetDouble(out double v)); Assert.AreEqual(3.14, v); }
        catch { _exitCode = 1; }
    }

    // [51] System.Text.Json/System.Text.Json.JsonElement::TryGetGuid:System.Boolean(System.Guid&)
    public static void CustomEntrySubject_51()
    {
        try { using var doc = JsonDocument.Parse("\"00000001-0002-0003-0004-000000000005\""); Assert.IsTrue(doc.RootElement.TryGetGuid(out Guid g)); }
        catch { _exitCode = 1; }
    }

    // [52] System.Text.Json/System.Text.Json.JsonElement::TryGetInt16:System.Boolean(System.Int16&)
    public static void CustomEntrySubject_52()
    {
        try { using var doc = JsonDocument.Parse("42"); Assert.IsTrue(doc.RootElement.TryGetInt16(out short v)); Assert.AreEqual((short)42, v); }
        catch { _exitCode = 1; }
    }

    // [53] System.Text.Json/System.Text.Json.JsonElement::TryGetInt32:System.Boolean(System.Int32&)
    public static void CustomEntrySubject_53()
    {
        try { using var doc = JsonDocument.Parse(TestJson); Assert.IsTrue(doc.RootElement.GetProperty("X").TryGetInt32(out int v)); Assert.AreEqual(42, v); }
        catch { _exitCode = 1; }
    }

    // [54] System.Text.Json/System.Text.Json.JsonElement::TryGetInt64:System.Boolean(System.Int64&)
    public static void CustomEntrySubject_54()
    {
        try { using var doc = JsonDocument.Parse("42"); Assert.IsTrue(doc.RootElement.TryGetInt64(out long v)); Assert.AreEqual(42L, v); }
        catch { _exitCode = 1; }
    }

    // [55] System.Text.Json/System.Text.Json.JsonElement::TryGetProperty:System.Boolean(System.String,System.Text.Json.JsonElement&)
    public static void CustomEntrySubject_55()
    {
        try { using var doc = JsonDocument.Parse(TestJson); Assert.IsTrue(doc.RootElement.TryGetProperty("X", out JsonElement el)); Assert.AreEqual(42, el.GetInt32()); }
        catch { _exitCode = 1; }
    }

    // [56] System.Text.Json/System.Text.Json.JsonElement::TryGetProperty:System.Boolean(System.ReadOnlySpan{System.Char},System.Text.Json.JsonElement&)
    public static void CustomEntrySubject_56()
    {
        try { using var doc = JsonDocument.Parse(TestJson); Assert.IsTrue(doc.RootElement.TryGetProperty((ReadOnlySpan<char>)"X".ToCharArray(), out JsonElement el)); Assert.AreEqual(42, el.GetInt32()); }
        catch { _exitCode = 1; }
    }

    // [57] System.Text.Json/System.Text.Json.JsonElement::TryGetProperty:System.Boolean(System.ReadOnlySpan{System.Byte},System.Text.Json.JsonElement&)
    public static void CustomEntrySubject_57()
    {
        try { using var doc = JsonDocument.Parse(TestJson); Assert.IsTrue(doc.RootElement.TryGetProperty((ReadOnlySpan<byte>)Encoding.UTF8.GetBytes("X"), out JsonElement el)); Assert.AreEqual(42, el.GetInt32()); }
        catch { _exitCode = 1; }
    }

    // [58] System.Text.Json/System.Text.Json.JsonElement::TryGetSByte:System.Boolean(System.SByte&)
    public static void CustomEntrySubject_58()
    {
        try { using var doc = JsonDocument.Parse("42"); Assert.IsTrue(doc.RootElement.TryGetSByte(out sbyte v)); Assert.AreEqual((sbyte)42, v); }
        catch { _exitCode = 1; }
    }

    // [59] System.Text.Json/System.Text.Json.JsonElement::TryGetSingle:System.Boolean(System.Single&)
    public static void CustomEntrySubject_59()
    {
        try { using var doc = JsonDocument.Parse("3.14"); Assert.IsTrue(doc.RootElement.TryGetSingle(out float v)); Assert.AreEqual(3.14f, v); }
        catch { _exitCode = 1; }
    }

    // [60] System.Text.Json/System.Text.Json.JsonElement::TryGetUInt16:System.Boolean(System.UInt16&)
    public static void CustomEntrySubject_60()
    {
        try { using var doc = JsonDocument.Parse("42"); Assert.IsTrue(doc.RootElement.TryGetUInt16(out ushort v)); Assert.AreEqual((ushort)42, v); }
        catch { _exitCode = 1; }
    }

    // [61] System.Text.Json/System.Text.Json.JsonElement::TryGetUInt32:System.Boolean(System.UInt32&)
    public static void CustomEntrySubject_61()
    {
        try { using var doc = JsonDocument.Parse("42"); Assert.IsTrue(doc.RootElement.TryGetUInt32(out uint v)); Assert.AreEqual(42u, v); }
        catch { _exitCode = 1; }
    }

    // [62] System.Text.Json/System.Text.Json.JsonElement::TryGetUInt64:System.Boolean(System.UInt64&)
    public static void CustomEntrySubject_62()
    {
        try { using var doc = JsonDocument.Parse("42"); Assert.IsTrue(doc.RootElement.TryGetUInt64(out ulong v)); Assert.AreEqual(42UL, v); }
        catch { _exitCode = 1; }
    }

    // [63] System.Text.Json/System.Text.Json.JsonElement::TryParseValue:System.Boolean(System.Text.Json.Utf8JsonReader&,System.Nullable{System.Text.Json.JsonElement}&)
    public static void CustomEntrySubject_63()
    {
        try { var reader = new Utf8JsonReader(Encoding.UTF8.GetBytes("42")); Assert.IsTrue(JsonElement.TryParseValue(ref reader, out JsonElement? el)); Assert.AreEqual(42, el!.Value.GetInt32()); }
        catch { _exitCode = 1; }
    }

    // [64] System.Text.Json/System.Text.Json.JsonElement::ValueEquals:System.Boolean(System.String)
    public static void CustomEntrySubject_64()
    {
        try { using var doc = JsonDocument.Parse(TestJson); Assert.IsTrue(doc.RootElement.GetProperty("name").ValueEquals("hello")); }
        catch { _exitCode = 1; }
    }

    // [65] System.Text.Json/System.Text.Json.JsonElement::ValueEquals:System.Boolean(System.ReadOnlySpan{System.Byte})
    public static void CustomEntrySubject_65()
    {
        try { using var doc = JsonDocument.Parse(TestJson); Assert.IsTrue(doc.RootElement.GetProperty("name").ValueEquals((ReadOnlySpan<byte>)Encoding.UTF8.GetBytes("hello"))); }
        catch { _exitCode = 1; }
    }

    // [66] System.Text.Json/System.Text.Json.JsonElement::ValueEquals:System.Boolean(System.ReadOnlySpan{System.Char})
    public static void CustomEntrySubject_66()
    {
        try { using var doc = JsonDocument.Parse(TestJson); Assert.IsTrue(doc.RootElement.GetProperty("name").ValueEquals((ReadOnlySpan<char>)"hello".ToCharArray())); }
        catch { _exitCode = 1; }
    }

    // [67] System.Text.Json/System.Text.Json.JsonElement::WriteTo:System.Void(System.Text.Json.Utf8JsonWriter)
    public static void CustomEntrySubject_67()
    {
        try { using var doc = JsonDocument.Parse(TestJson); var ms = new MemoryStream(); using var writer = new Utf8JsonWriter(ms); doc.RootElement.WriteTo(writer); writer.Flush(); Assert.IsTrue(ms.ToArray().Length > 0); }
        catch { _exitCode = 1; }
    }

    // [68] System.Text.Json/System.Text.Json.JsonElement::get_Item:System.Text.Json.JsonElement()
    public static void CustomEntrySubject_68()
    {
        try { using var doc = JsonDocument.Parse("[10]"); var el = doc.RootElement[0]; Assert.AreEqual(10, el.GetInt32()); }
        catch { _exitCode = 1; }
    }
}
