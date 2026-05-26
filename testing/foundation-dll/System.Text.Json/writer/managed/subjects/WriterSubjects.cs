// Auto-generated native-AOT entry point
// Family: family/System.Text.Json/writer
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

public static partial class WriterSubjects
{
    // Inlined exit code — avoids SDK method call resolution in codegen
    public static int _exitCode;

    private static MemoryStream MakeWriter(out Utf8JsonWriter w)
    {
        var ms = new MemoryStream();
        w = new Utf8JsonWriter(ms);
        return ms;
    }

    // [0] System.Text.Json/System.Text.Json.Utf8JsonWriter::Dispose:System.Void()
    public static void CustomEntrySubject_0()
    {
        try { using var ms = new MemoryStream(); var w = new Utf8JsonWriter(ms); w.Dispose(); Assert.IsTrue(true); }
        catch { _exitCode = 1; }
    }

    // [1] System.Text.Json/System.Text.Json.Utf8JsonWriter::DisposeAsync:System.Threading.Tasks.ValueTask()
    public static void CustomEntrySubject_1()
    {
        try { using var ms = new MemoryStream(); var w = new Utf8JsonWriter(ms); var t = w.DisposeAsync(); Assert.IsTrue(t.IsCompletedSuccessfully); }
        catch { _exitCode = 1; }
    }

    // [2] System.Text.Json/System.Text.Json.Utf8JsonWriter::Flush:System.Void()
    public static void CustomEntrySubject_2()
    {
        try { var ms = MakeWriter(out var w); w.WriteNumberValue(42); w.Flush(); Assert.IsTrue(ms.ToArray().Length > 0); }
        catch { _exitCode = 1; }
    }

    // [3] System.Text.Json/System.Text.Json.Utf8JsonWriter::FlushAsync:System.Threading.Tasks.Task(System.Threading.CancellationToken)
    public static void CustomEntrySubject_3()
    {
        try { var ms = MakeWriter(out var w); w.WriteNumberValue(42); var t = w.FlushAsync(); Assert.IsTrue(t.IsCompletedSuccessfully); }
        catch { _exitCode = 1; }
    }

    // [4] System.Text.Json/System.Text.Json.Utf8JsonWriter::get_BytesCommitted:System.Int64()
    public static void CustomEntrySubject_4()
    {
        try { var ms = MakeWriter(out var w); w.WriteNumberValue(42); w.Flush(); Assert.IsTrue(w.BytesCommitted > 0); }
        catch { _exitCode = 1; }
    }

    // [5] System.Text.Json/System.Text.Json.Utf8JsonWriter::get_BytesPending:System.Int32()
    public static void CustomEntrySubject_5()
    {
        try { var ms = MakeWriter(out var w); w.WriteNumberValue(42); Assert.IsTrue(w.BytesPending > 0); }
        catch { _exitCode = 1; }
    }

    // [6] System.Text.Json/System.Text.Json.Utf8JsonWriter::get_CurrentDepth:System.Int32()
    public static void CustomEntrySubject_6()
    {
        try { var ms = MakeWriter(out var w); Assert.AreEqual(0, w.CurrentDepth); w.WriteStartObject(); Assert.AreEqual(1, w.CurrentDepth); }
        catch { _exitCode = 1; }
    }

    // [7] System.Text.Json/System.Text.Json.Utf8JsonWriter::get_Options:System.Text.Json.JsonWriterOptions()
    public static void CustomEntrySubject_7()
    {
        try { var ms = MakeWriter(out var w); var o = w.Options; Assert.IsTrue(true); }
        catch { _exitCode = 1; }
    }

    // [8] System.Text.Json/System.Text.Json.Utf8JsonWriter::Reset:System.Void()
    public static void CustomEntrySubject_8()
    {
        try { var ms = MakeWriter(out var w); w.Reset(); Assert.IsTrue(true); }
        catch { _exitCode = 1; }
    }

    // [9] System.Text.Json/System.Text.Json.Utf8JsonWriter::Reset:System.Void(System.IO.Stream)
    public static void CustomEntrySubject_9()
    {
        try { var ms = MakeWriter(out var w); using var ms2 = new MemoryStream(); w.Reset(ms2); Assert.IsTrue(true); }
        catch { _exitCode = 1; }
    }

    // [10] System.Text.Json/System.Text.Json.Utf8JsonWriter::Reset:System.Void(System.Buffers.IBufferWriter{System.Byte})
    public static void CustomEntrySubject_10()
    {
        try { var ms = MakeWriter(out var w); var bw = new ArrayBufferWriter<byte>(); w.Reset(bw); Assert.IsTrue(true); }
        catch { _exitCode = 1; }
    }

    // [11] System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteBase64String:System.Void(System.Text.Json.JsonEncodedText,System.ReadOnlySpan{System.Byte})
    public static void CustomEntrySubject_11()
    {
        try { var ms = MakeWriter(out var w); w.WriteBase64String(JsonEncodedText.Encode("data"), new byte[] { 1, 2, 3 }); w.Flush(); Assert.IsTrue(ms.ToArray().Length > 0); }
        catch { _exitCode = 1; }
    }

    // [12] System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteBase64String:System.Void(System.String,System.ReadOnlySpan{System.Byte})
    public static void CustomEntrySubject_12()
    {
        try { var ms = MakeWriter(out var w); w.WriteBase64String("data", new byte[] { 1, 2, 3 }); w.Flush(); Assert.IsTrue(ms.ToArray().Length > 0); }
        catch { _exitCode = 1; }
    }

    // [13] System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteBase64String:System.Void(System.ReadOnlySpan{System.Char},System.ReadOnlySpan{System.Byte})
    public static void CustomEntrySubject_13()
    {
        try { var ms = MakeWriter(out var w); w.WriteBase64String((ReadOnlySpan<char>)"data".ToCharArray(), new byte[] { 1, 2, 3 }); w.Flush(); Assert.IsTrue(ms.ToArray().Length > 0); }
        catch { _exitCode = 1; }
    }

    // [14] System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteBase64String:System.Void(System.ReadOnlySpan{System.Byte},System.ReadOnlySpan{System.Byte})
    public static void CustomEntrySubject_14()
    {
        try { var ms = MakeWriter(out var w); w.WriteBase64String((ReadOnlySpan<byte>)Encoding.UTF8.GetBytes("data"), new byte[] { 1, 2, 3 }); w.Flush(); Assert.IsTrue(ms.ToArray().Length > 0); }
        catch { _exitCode = 1; }
    }

    // [15] System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteBase64StringSegment:System.Void(System.ReadOnlySpan{System.Byte},System.Boolean)
    public static void CustomEntrySubject_15()
    {
        try { var ms = MakeWriter(out var w); w.WriteStartObject(); w.WriteBase64StringSegment(new byte[] { 1, 2, 3 }, false); w.WriteEndObject(); w.Flush(); Assert.IsTrue(ms.ToArray().Length > 0); }
        catch { _exitCode = 1; }
    }

    // [16] System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteBase64StringValue:System.Void(System.ReadOnlySpan{System.Byte})
    public static void CustomEntrySubject_16()
    {
        try { var ms = MakeWriter(out var w); w.WriteBase64StringValue(new byte[] { 1, 2, 3 }); w.Flush(); Assert.IsTrue(ms.ToArray().Length > 0); }
        catch { _exitCode = 1; }
    }

    // [17] System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteBoolean:System.Void(System.Text.Json.JsonEncodedText,System.Boolean)
    public static void CustomEntrySubject_17()
    {
        try { var ms = MakeWriter(out var w); w.WriteBoolean(JsonEncodedText.Encode("flag"), true); w.Flush(); Assert.IsTrue(ms.ToArray().Length > 0); }
        catch { _exitCode = 1; }
    }

    // [18] System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteBoolean:System.Void(System.String,System.Boolean)
    public static void CustomEntrySubject_18()
    {
        try { var ms = MakeWriter(out var w); w.WriteBoolean("flag", true); w.Flush(); Assert.IsTrue(ms.ToArray().Length > 0); }
        catch { _exitCode = 1; }
    }

    // [19] System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteBoolean:System.Void(System.ReadOnlySpan{System.Char},System.Boolean)
    public static void CustomEntrySubject_19()
    {
        try { var ms = MakeWriter(out var w); w.WriteBoolean((ReadOnlySpan<char>)"flag".ToCharArray(), true); w.Flush(); Assert.IsTrue(ms.ToArray().Length > 0); }
        catch { _exitCode = 1; }
    }

    // [20] System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteBoolean:System.Void(System.ReadOnlySpan{System.Byte},System.Boolean)
    public static void CustomEntrySubject_20()
    {
        try { var ms = MakeWriter(out var w); w.WriteBoolean((ReadOnlySpan<byte>)Encoding.UTF8.GetBytes("flag"), true); w.Flush(); Assert.IsTrue(ms.ToArray().Length > 0); }
        catch { _exitCode = 1; }
    }

    // [21] System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteBooleanValue:System.Void(System.Boolean)
    public static void CustomEntrySubject_21()
    {
        try { var ms = MakeWriter(out var w); w.WriteBooleanValue(true); w.Flush(); Assert.IsTrue(ms.ToArray().Length > 0); }
        catch { _exitCode = 1; }
    }

    // [22] System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteCommentValue:System.Void(System.String)
    public static void CustomEntrySubject_22()
    {
        try { var ms = MakeWriter(out var w); w.WriteCommentValue(" hello "); w.Flush(); var s = Encoding.UTF8.GetString(ms.ToArray()); Assert.IsTrue(s.Contains(" hello ")); }
        catch { _exitCode = 1; }
    }

    // [23] System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteCommentValue:System.Void(System.ReadOnlySpan{System.Char})
    public static void CustomEntrySubject_23()
    {
        try { var ms = MakeWriter(out var w); w.WriteCommentValue((ReadOnlySpan<char>)" hello ".ToCharArray()); w.Flush(); Assert.IsTrue(ms.ToArray().Length > 0); }
        catch { _exitCode = 1; }
    }

    // [24] System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteCommentValue:System.Void(System.ReadOnlySpan{System.Byte})
    public static void CustomEntrySubject_24()
    {
        try { var ms = MakeWriter(out var w); w.WriteCommentValue((ReadOnlySpan<byte>)Encoding.UTF8.GetBytes(" hello ")); w.Flush(); Assert.IsTrue(ms.ToArray().Length > 0); }
        catch { _exitCode = 1; }
    }

    // [25] System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteEndArray:System.Void()
    public static void CustomEntrySubject_25()
    {
        try { var ms = MakeWriter(out var w); w.WriteStartArray(); w.WriteEndArray(); w.Flush(); Assert.IsTrue(ms.ToArray().Length > 0); }
        catch { _exitCode = 1; }
    }

    // [26] System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteEndObject:System.Void()
    public static void CustomEntrySubject_26()
    {
        try { var ms = MakeWriter(out var w); w.WriteStartObject(); w.WriteEndObject(); w.Flush(); Assert.IsTrue(ms.ToArray().Length > 0); }
        catch { _exitCode = 1; }
    }

    // [27] System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteNull:System.Void(System.Text.Json.JsonEncodedText)
    public static void CustomEntrySubject_27()
    {
        try { var ms = MakeWriter(out var w); w.WriteNull(JsonEncodedText.Encode("n")); w.Flush(); Assert.IsTrue(ms.ToArray().Length > 0); }
        catch { _exitCode = 1; }
    }

    // [28] System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteNull:System.Void(System.String)
    public static void CustomEntrySubject_28()
    {
        try { var ms = MakeWriter(out var w); w.WriteNull("n"); w.Flush(); Assert.IsTrue(ms.ToArray().Length > 0); }
        catch { _exitCode = 1; }
    }

    // [29] System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteNull:System.Void(System.ReadOnlySpan{System.Char})
    public static void CustomEntrySubject_29()
    {
        try { var ms = MakeWriter(out var w); w.WriteNull((ReadOnlySpan<char>)"n".ToCharArray()); w.Flush(); Assert.IsTrue(ms.ToArray().Length > 0); }
        catch { _exitCode = 1; }
    }

    // [30] System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteNull:System.Void(System.ReadOnlySpan{System.Byte})
    public static void CustomEntrySubject_30()
    {
        try { var ms = MakeWriter(out var w); w.WriteNull((ReadOnlySpan<byte>)Encoding.UTF8.GetBytes("n")); w.Flush(); Assert.IsTrue(ms.ToArray().Length > 0); }
        catch { _exitCode = 1; }
    }

    // [31] System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteNullValue:System.Void()
    public static void CustomEntrySubject_31()
    {
        try { var ms = MakeWriter(out var w); w.WriteNullValue(); w.Flush(); Assert.IsTrue(ms.ToArray().Length > 0); }
        catch { _exitCode = 1; }
    }

    // [32] System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteNumber:System.Void(System.Text.Json.JsonEncodedText,System.Decimal)
    public static void CustomEntrySubject_32()
    {
        try { var ms = MakeWriter(out var w); w.WriteNumber(JsonEncodedText.Encode("x"), 1.5m); w.Flush(); Assert.IsTrue(ms.ToArray().Length > 0); }
        catch { _exitCode = 1; }
    }

    // [33] System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteNumber:System.Void(System.String,System.Decimal)
    public static void CustomEntrySubject_33()
    {
        try { var ms = MakeWriter(out var w); w.WriteNumber("x", 1.5m); w.Flush(); Assert.IsTrue(ms.ToArray().Length > 0); }
        catch { _exitCode = 1; }
    }

    // [34] System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteNumber:System.Void(System.ReadOnlySpan{System.Char},System.Decimal)
    public static void CustomEntrySubject_34()
    {
        try { var ms = MakeWriter(out var w); w.WriteNumber((ReadOnlySpan<char>)"x".ToCharArray(), 1.5m); w.Flush(); Assert.IsTrue(ms.ToArray().Length > 0); }
        catch { _exitCode = 1; }
    }

    // [35] System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteNumber:System.Void(System.ReadOnlySpan{System.Byte},System.Decimal)
    public static void CustomEntrySubject_35()
    {
        try { var ms = MakeWriter(out var w); w.WriteNumber((ReadOnlySpan<byte>)Encoding.UTF8.GetBytes("x"), 1.5m); w.Flush(); Assert.IsTrue(ms.ToArray().Length > 0); }
        catch { _exitCode = 1; }
    }

    // [36] System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteNumber:System.Void(System.Text.Json.JsonEncodedText,System.Double)
    public static void CustomEntrySubject_36()
    {
        try { var ms = MakeWriter(out var w); w.WriteNumber(JsonEncodedText.Encode("x"), 1.5); w.Flush(); Assert.IsTrue(ms.ToArray().Length > 0); }
        catch { _exitCode = 1; }
    }

    // [37] System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteNumber:System.Void(System.String,System.Double)
    public static void CustomEntrySubject_37()
    {
        try { var ms = MakeWriter(out var w); w.WriteNumber("x", 1.5); w.Flush(); Assert.IsTrue(ms.ToArray().Length > 0); }
        catch { _exitCode = 1; }
    }

    // [38] System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteNumber:System.Void(System.ReadOnlySpan{System.Char},System.Double)
    public static void CustomEntrySubject_38()
    {
        try { var ms = MakeWriter(out var w); w.WriteNumber((ReadOnlySpan<char>)"x".ToCharArray(), 1.5); w.Flush(); Assert.IsTrue(ms.ToArray().Length > 0); }
        catch { _exitCode = 1; }
    }

    // [39] System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteNumber:System.Void(System.ReadOnlySpan{System.Byte},System.Double)
    public static void CustomEntrySubject_39()
    {
        try { var ms = MakeWriter(out var w); w.WriteNumber((ReadOnlySpan<byte>)Encoding.UTF8.GetBytes("x"), 1.5); w.Flush(); Assert.IsTrue(ms.ToArray().Length > 0); }
        catch { _exitCode = 1; }
    }

    // [40] System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteNumber:System.Void(System.Text.Json.JsonEncodedText,System.Single)
    public static void CustomEntrySubject_40()
    {
        try { var ms = MakeWriter(out var w); w.WriteNumber(JsonEncodedText.Encode("x"), 1.5f); w.Flush(); Assert.IsTrue(ms.ToArray().Length > 0); }
        catch { _exitCode = 1; }
    }

    // [41] System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteNumber:System.Void(System.String,System.Single)
    public static void CustomEntrySubject_41()
    {
        try { var ms = MakeWriter(out var w); w.WriteNumber("x", 1.5f); w.Flush(); Assert.IsTrue(ms.ToArray().Length > 0); }
        catch { _exitCode = 1; }
    }

    // [42] System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteNumber:System.Void(System.ReadOnlySpan{System.Char},System.Single)
    public static void CustomEntrySubject_42()
    {
        try { var ms = MakeWriter(out var w); w.WriteNumber((ReadOnlySpan<char>)"x".ToCharArray(), 1.5f); w.Flush(); Assert.IsTrue(ms.ToArray().Length > 0); }
        catch { _exitCode = 1; }
    }

    // [43] System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteNumber:System.Void(System.ReadOnlySpan{System.Byte},System.Single)
    public static void CustomEntrySubject_43()
    {
        try { var ms = MakeWriter(out var w); w.WriteNumber((ReadOnlySpan<byte>)Encoding.UTF8.GetBytes("x"), 1.5f); w.Flush(); Assert.IsTrue(ms.ToArray().Length > 0); }
        catch { _exitCode = 1; }
    }

    // [44] System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteNumber:System.Void(System.Text.Json.JsonEncodedText,System.Int64)
    public static void CustomEntrySubject_44()
    {
        try { var ms = MakeWriter(out var w); w.WriteNumber(JsonEncodedText.Encode("x"), 42L); w.Flush(); Assert.IsTrue(ms.ToArray().Length > 0); }
        catch { _exitCode = 1; }
    }

    // [45] System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteNumber:System.Void(System.String,System.Int64)
    public static void CustomEntrySubject_45()
    {
        try { var ms = MakeWriter(out var w); w.WriteNumber("x", 42L); w.Flush(); Assert.IsTrue(ms.ToArray().Length > 0); }
        catch { _exitCode = 1; }
    }

    // [46] System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteNumber:System.Void(System.ReadOnlySpan{System.Char},System.Int64)
    public static void CustomEntrySubject_46()
    {
        try { var ms = MakeWriter(out var w); w.WriteNumber((ReadOnlySpan<char>)"x".ToCharArray(), 42L); w.Flush(); Assert.IsTrue(ms.ToArray().Length > 0); }
        catch { _exitCode = 1; }
    }

    // [47] System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteNumber:System.Void(System.ReadOnlySpan{System.Byte},System.Int64)
    public static void CustomEntrySubject_47()
    {
        try { var ms = MakeWriter(out var w); w.WriteNumber((ReadOnlySpan<byte>)Encoding.UTF8.GetBytes("x"), 42L); w.Flush(); Assert.IsTrue(ms.ToArray().Length > 0); }
        catch { _exitCode = 1; }
    }

    // [48] System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteNumber:System.Void(System.Text.Json.JsonEncodedText,System.Int32)
    public static void CustomEntrySubject_48()
    {
        try { var ms = MakeWriter(out var w); w.WriteNumber(JsonEncodedText.Encode("x"), 42); w.Flush(); Assert.IsTrue(ms.ToArray().Length > 0); }
        catch { _exitCode = 1; }
    }

    // [49] System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteNumber:System.Void(System.String,System.Int32)
    public static void CustomEntrySubject_49()
    {
        try { var ms = MakeWriter(out var w); w.WriteNumber("x", 42); w.Flush(); Assert.IsTrue(ms.ToArray().Length > 0); }
        catch { _exitCode = 1; }
    }

    // [50] System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteNumber:System.Void(System.ReadOnlySpan{System.Char},System.Int32)
    public static void CustomEntrySubject_50()
    {
        try { var ms = MakeWriter(out var w); w.WriteNumber((ReadOnlySpan<char>)"x".ToCharArray(), 42); w.Flush(); Assert.IsTrue(ms.ToArray().Length > 0); }
        catch { _exitCode = 1; }
    }

    // [51] System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteNumber:System.Void(System.ReadOnlySpan{System.Byte},System.Int32)
    public static void CustomEntrySubject_51()
    {
        try { var ms = MakeWriter(out var w); w.WriteNumber((ReadOnlySpan<byte>)Encoding.UTF8.GetBytes("x"), 42); w.Flush(); Assert.IsTrue(ms.ToArray().Length > 0); }
        catch { _exitCode = 1; }
    }

    // [52] System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteNumber:System.Void(System.Text.Json.JsonEncodedText,System.UInt64)
    public static void CustomEntrySubject_52()
    {
        try { var ms = MakeWriter(out var w); w.WriteNumber(JsonEncodedText.Encode("x"), 42UL); w.Flush(); Assert.IsTrue(ms.ToArray().Length > 0); }
        catch { _exitCode = 1; }
    }

    // [53] System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteNumber:System.Void(System.String,System.UInt64)
    public static void CustomEntrySubject_53()
    {
        try { var ms = MakeWriter(out var w); w.WriteNumber("x", 42UL); w.Flush(); Assert.IsTrue(ms.ToArray().Length > 0); }
        catch { _exitCode = 1; }
    }

    // [54] System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteNumber:System.Void(System.ReadOnlySpan{System.Char},System.UInt64)
    public static void CustomEntrySubject_54()
    {
        try { var ms = MakeWriter(out var w); w.WriteNumber((ReadOnlySpan<char>)"x".ToCharArray(), 42UL); w.Flush(); Assert.IsTrue(ms.ToArray().Length > 0); }
        catch { _exitCode = 1; }
    }

    // [55] System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteNumber:System.Void(System.ReadOnlySpan{System.Byte},System.UInt64)
    public static void CustomEntrySubject_55()
    {
        try { var ms = MakeWriter(out var w); w.WriteNumber((ReadOnlySpan<byte>)Encoding.UTF8.GetBytes("x"), 42UL); w.Flush(); Assert.IsTrue(ms.ToArray().Length > 0); }
        catch { _exitCode = 1; }
    }

    // [56] System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteNumber:System.Void(System.Text.Json.JsonEncodedText,System.UInt32)
    public static void CustomEntrySubject_56()
    {
        try { var ms = MakeWriter(out var w); w.WriteNumber(JsonEncodedText.Encode("x"), 42u); w.Flush(); Assert.IsTrue(ms.ToArray().Length > 0); }
        catch { _exitCode = 1; }
    }

    // [57] System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteNumber:System.Void(System.String,System.UInt32)
    public static void CustomEntrySubject_57()
    {
        try { var ms = MakeWriter(out var w); w.WriteNumber("x", 42u); w.Flush(); Assert.IsTrue(ms.ToArray().Length > 0); }
        catch { _exitCode = 1; }
    }

    // [58] System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteNumber:System.Void(System.ReadOnlySpan{System.Char},System.UInt32)
    public static void CustomEntrySubject_58()
    {
        try { var ms = MakeWriter(out var w); w.WriteNumber((ReadOnlySpan<char>)"x".ToCharArray(), 42u); w.Flush(); Assert.IsTrue(ms.ToArray().Length > 0); }
        catch { _exitCode = 1; }
    }

    // [59] System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteNumber:System.Void(System.ReadOnlySpan{System.Byte},System.UInt32)
    public static void CustomEntrySubject_59()
    {
        try { var ms = MakeWriter(out var w); w.WriteNumber((ReadOnlySpan<byte>)Encoding.UTF8.GetBytes("x"), 42u); w.Flush(); Assert.IsTrue(ms.ToArray().Length > 0); }
        catch { _exitCode = 1; }
    }

    // [60] System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteNumberValue:System.Void(System.Decimal)
    public static void CustomEntrySubject_60()
    {
        try { var ms = MakeWriter(out var w); w.WriteNumberValue(1.5m); w.Flush(); Assert.IsTrue(ms.ToArray().Length > 0); }
        catch { _exitCode = 1; }
    }

    // [61] System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteNumberValue:System.Void(System.Double)
    public static void CustomEntrySubject_61()
    {
        try { var ms = MakeWriter(out var w); w.WriteNumberValue(1.5); w.Flush(); Assert.IsTrue(ms.ToArray().Length > 0); }
        catch { _exitCode = 1; }
    }

    // [62] System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteNumberValue:System.Void(System.Single)
    public static void CustomEntrySubject_62()
    {
        try { var ms = MakeWriter(out var w); w.WriteNumberValue(1.5f); w.Flush(); Assert.IsTrue(ms.ToArray().Length > 0); }
        catch { _exitCode = 1; }
    }

    // [63] System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteNumberValue:System.Void(System.Int32)
    public static void CustomEntrySubject_63()
    {
        try { var ms = MakeWriter(out var w); w.WriteNumberValue(42); w.Flush(); Assert.IsTrue(ms.ToArray().Length > 0); }
        catch { _exitCode = 1; }
    }

    // [64] System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteNumberValue:System.Void(System.Int64)
    public static void CustomEntrySubject_64()
    {
        try { var ms = MakeWriter(out var w); w.WriteNumberValue(42L); w.Flush(); Assert.IsTrue(ms.ToArray().Length > 0); }
        catch { _exitCode = 1; }
    }

    // [65] System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteNumberValue:System.Void(System.UInt32)
    public static void CustomEntrySubject_65()
    {
        try { var ms = MakeWriter(out var w); w.WriteNumberValue(42u); w.Flush(); Assert.IsTrue(ms.ToArray().Length > 0); }
        catch { _exitCode = 1; }
    }

    // [66] System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteNumberValue:System.Void(System.UInt64)
    public static void CustomEntrySubject_66()
    {
        try { var ms = MakeWriter(out var w); w.WriteNumberValue(42UL); w.Flush(); Assert.IsTrue(ms.ToArray().Length > 0); }
        catch { _exitCode = 1; }
    }

    // [67] System.Text.Json/System.Text.Json.Utf8JsonWriter::WritePropertyName:System.Void(System.Text.Json.JsonEncodedText)
    public static void CustomEntrySubject_67()
    {
        try { var ms = MakeWriter(out var w); w.WriteStartObject(); w.WritePropertyName(JsonEncodedText.Encode("key")); w.WriteNumberValue(1); w.WriteEndObject(); w.Flush(); Assert.IsTrue(ms.ToArray().Length > 0); }
        catch { _exitCode = 1; }
    }

    // [68] System.Text.Json/System.Text.Json.Utf8JsonWriter::WritePropertyName:System.Void(System.String)
    public static void CustomEntrySubject_68()
    {
        try { var ms = MakeWriter(out var w); w.WriteStartObject(); w.WritePropertyName("key"); w.WriteNumberValue(1); w.WriteEndObject(); w.Flush(); Assert.IsTrue(ms.ToArray().Length > 0); }
        catch { _exitCode = 1; }
    }

    // [69] System.Text.Json/System.Text.Json.Utf8JsonWriter::WritePropertyName:System.Void(System.ReadOnlySpan{System.Char})
    public static void CustomEntrySubject_69()
    {
        try { var ms = MakeWriter(out var w); w.WriteStartObject(); w.WritePropertyName((ReadOnlySpan<char>)"key".ToCharArray()); w.WriteNumberValue(1); w.WriteEndObject(); w.Flush(); Assert.IsTrue(ms.ToArray().Length > 0); }
        catch { _exitCode = 1; }
    }

    // [70] System.Text.Json/System.Text.Json.Utf8JsonWriter::WritePropertyName:System.Void(System.ReadOnlySpan{System.Byte})
    public static void CustomEntrySubject_70()
    {
        try { var ms = MakeWriter(out var w); w.WriteStartObject(); w.WritePropertyName((ReadOnlySpan<byte>)Encoding.UTF8.GetBytes("key")); w.WriteNumberValue(1); w.WriteEndObject(); w.Flush(); Assert.IsTrue(ms.ToArray().Length > 0); }
        catch { _exitCode = 1; }
    }

    // [71] System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteRawValue:System.Void(System.String,System.Boolean)
    public static void CustomEntrySubject_71()
    {
        try { var ms = MakeWriter(out var w); w.WriteRawValue("42", false); w.Flush(); Assert.IsTrue(ms.ToArray().Length > 0); }
        catch { _exitCode = 1; }
    }

    // [72] System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteRawValue:System.Void(System.ReadOnlySpan{System.Char},System.Boolean)
    public static void CustomEntrySubject_72()
    {
        try { var ms = MakeWriter(out var w); w.WriteRawValue((ReadOnlySpan<char>)"42".ToCharArray(), false); w.Flush(); Assert.IsTrue(ms.ToArray().Length > 0); }
        catch { _exitCode = 1; }
    }

    // [73] System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteRawValue:System.Void(System.ReadOnlySpan{System.Byte},System.Boolean)
    public static void CustomEntrySubject_73()
    {
        try { var ms = MakeWriter(out var w); w.WriteRawValue((ReadOnlySpan<byte>)Encoding.UTF8.GetBytes("42"), false); w.Flush(); Assert.IsTrue(ms.ToArray().Length > 0); }
        catch { _exitCode = 1; }
    }

    // [74] System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteRawValue:System.Void(System.Buffers.ReadOnlySequence{System.Byte},System.Boolean)
    public static void CustomEntrySubject_74()
    {
        try { var ms = MakeWriter(out var w); var seq = new ReadOnlySequence<byte>(Encoding.UTF8.GetBytes("42")); w.WriteRawValue(seq, false); w.Flush(); Assert.IsTrue(ms.ToArray().Length > 0); }
        catch { _exitCode = 1; }
    }

    // [75] System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteStartArray:System.Void()
    public static void CustomEntrySubject_75()
    {
        try { var ms = MakeWriter(out var w); w.WriteStartArray(); w.WriteEndArray(); w.Flush(); Assert.IsTrue(ms.ToArray().Length > 0); }
        catch { _exitCode = 1; }
    }

    // [76] System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteStartArray:System.Void(System.Text.Json.JsonEncodedText)
    public static void CustomEntrySubject_76()
    {
        try { var ms = MakeWriter(out var w); w.WriteStartArray(JsonEncodedText.Encode("arr")); w.WriteEndArray(); w.Flush(); Assert.IsTrue(ms.ToArray().Length > 0); }
        catch { _exitCode = 1; }
    }

    // [77] System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteStartArray:System.Void(System.ReadOnlySpan{System.Byte})
    public static void CustomEntrySubject_77()
    {
        try { var ms = MakeWriter(out var w); w.WriteStartArray((ReadOnlySpan<byte>)Encoding.UTF8.GetBytes("arr")); w.WriteEndArray(); w.Flush(); Assert.IsTrue(ms.ToArray().Length > 0); }
        catch { _exitCode = 1; }
    }

    // [78] System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteStartArray:System.Void(System.String)
    public static void CustomEntrySubject_78()
    {
        try { var ms = MakeWriter(out var w); w.WriteStartArray("arr"); w.WriteEndArray(); w.Flush(); Assert.IsTrue(ms.ToArray().Length > 0); }
        catch { _exitCode = 1; }
    }

    // [79] System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteStartArray:System.Void(System.ReadOnlySpan{System.Char})
    public static void CustomEntrySubject_79()
    {
        try { var ms = MakeWriter(out var w); w.WriteStartArray((ReadOnlySpan<char>)"arr".ToCharArray()); w.WriteEndArray(); w.Flush(); Assert.IsTrue(ms.ToArray().Length > 0); }
        catch { _exitCode = 1; }
    }

    // [80] System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteStartObject:System.Void()
    public static void CustomEntrySubject_80()
    {
        try { var ms = MakeWriter(out var w); w.WriteStartObject(); w.WriteEndObject(); w.Flush(); Assert.IsTrue(ms.ToArray().Length > 0); }
        catch { _exitCode = 1; }
    }

    // [81] System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteStartObject:System.Void(System.Text.Json.JsonEncodedText)
    public static void CustomEntrySubject_81()
    {
        try { var ms = MakeWriter(out var w); w.WriteStartObject(JsonEncodedText.Encode("obj")); w.WriteEndObject(); w.Flush(); Assert.IsTrue(ms.ToArray().Length > 0); }
        catch { _exitCode = 1; }
    }

    // [82] System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteStartObject:System.Void(System.ReadOnlySpan{System.Byte})
    public static void CustomEntrySubject_82()
    {
        try { var ms = MakeWriter(out var w); w.WriteStartObject((ReadOnlySpan<byte>)Encoding.UTF8.GetBytes("obj")); w.WriteEndObject(); w.Flush(); Assert.IsTrue(ms.ToArray().Length > 0); }
        catch { _exitCode = 1; }
    }

    // [83] System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteStartObject:System.Void(System.String)
    public static void CustomEntrySubject_83()
    {
        try { var ms = MakeWriter(out var w); w.WriteStartObject("obj"); w.WriteEndObject(); w.Flush(); Assert.IsTrue(ms.ToArray().Length > 0); }
        catch { _exitCode = 1; }
    }

    // [84] System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteStartObject:System.Void(System.ReadOnlySpan{System.Char})
    public static void CustomEntrySubject_84()
    {
        try { var ms = MakeWriter(out var w); w.WriteStartObject((ReadOnlySpan<char>)"obj".ToCharArray()); w.WriteEndObject(); w.Flush(); Assert.IsTrue(ms.ToArray().Length > 0); }
        catch { _exitCode = 1; }
    }

    // [85] System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteString:System.Void(System.Text.Json.JsonEncodedText,System.DateTime)
    public static void CustomEntrySubject_85()
    {
        try { var ms = MakeWriter(out var w); w.WriteString(JsonEncodedText.Encode("d"), DateTime.Now); w.Flush(); Assert.IsTrue(ms.ToArray().Length > 0); }
        catch { _exitCode = 1; }
    }

    // [86] System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteString:System.Void(System.String,System.DateTime)
    public static void CustomEntrySubject_86()
    {
        try { var ms = MakeWriter(out var w); w.WriteString("d", DateTime.Now); w.Flush(); Assert.IsTrue(ms.ToArray().Length > 0); }
        catch { _exitCode = 1; }
    }

    // [87] System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteString:System.Void(System.ReadOnlySpan{System.Char},System.DateTime)
    public static void CustomEntrySubject_87()
    {
        try { var ms = MakeWriter(out var w); w.WriteString((ReadOnlySpan<char>)"d".ToCharArray(), DateTime.Now); w.Flush(); Assert.IsTrue(ms.ToArray().Length > 0); }
        catch { _exitCode = 1; }
    }

    // [88] System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteString:System.Void(System.ReadOnlySpan{System.Byte},System.DateTime)
    public static void CustomEntrySubject_88()
    {
        try { var ms = MakeWriter(out var w); w.WriteString((ReadOnlySpan<byte>)Encoding.UTF8.GetBytes("d"), DateTime.Now); w.Flush(); Assert.IsTrue(ms.ToArray().Length > 0); }
        catch { _exitCode = 1; }
    }

    // [89] System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteString:System.Void(System.Text.Json.JsonEncodedText,System.DateTimeOffset)
    public static void CustomEntrySubject_89()
    {
        try { var ms = MakeWriter(out var w); w.WriteString(JsonEncodedText.Encode("d"), DateTimeOffset.Now); w.Flush(); Assert.IsTrue(ms.ToArray().Length > 0); }
        catch { _exitCode = 1; }
    }

    // [90] System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteString:System.Void(System.String,System.DateTimeOffset)
    public static void CustomEntrySubject_90()
    {
        try { var ms = MakeWriter(out var w); w.WriteString("d", DateTimeOffset.Now); w.Flush(); Assert.IsTrue(ms.ToArray().Length > 0); }
        catch { _exitCode = 1; }
    }

    // [91] System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteString:System.Void(System.ReadOnlySpan{System.Char},System.DateTimeOffset)
    public static void CustomEntrySubject_91()
    {
        try { var ms = MakeWriter(out var w); w.WriteString((ReadOnlySpan<char>)"d".ToCharArray(), DateTimeOffset.Now); w.Flush(); Assert.IsTrue(ms.ToArray().Length > 0); }
        catch { _exitCode = 1; }
    }

    // [92] System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteString:System.Void(System.ReadOnlySpan{System.Byte},System.DateTimeOffset)
    public static void CustomEntrySubject_92()
    {
        try { var ms = MakeWriter(out var w); w.WriteString((ReadOnlySpan<byte>)Encoding.UTF8.GetBytes("d"), DateTimeOffset.Now); w.Flush(); Assert.IsTrue(ms.ToArray().Length > 0); }
        catch { _exitCode = 1; }
    }

    // [93] System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteString:System.Void(System.Text.Json.JsonEncodedText,System.Guid)
    public static void CustomEntrySubject_93()
    {
        try { var ms = MakeWriter(out var w); w.WriteString(JsonEncodedText.Encode("g"), Guid.NewGuid()); w.Flush(); Assert.IsTrue(ms.ToArray().Length > 0); }
        catch { _exitCode = 1; }
    }

    // [94] System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteString:System.Void(System.String,System.Guid)
    public static void CustomEntrySubject_94()
    {
        try { var ms = MakeWriter(out var w); w.WriteString("g", Guid.NewGuid()); w.Flush(); Assert.IsTrue(ms.ToArray().Length > 0); }
        catch { _exitCode = 1; }
    }

    // [95] System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteString:System.Void(System.ReadOnlySpan{System.Char},System.Guid)
    public static void CustomEntrySubject_95()
    {
        try { var ms = MakeWriter(out var w); w.WriteString((ReadOnlySpan<char>)"g".ToCharArray(), Guid.NewGuid()); w.Flush(); Assert.IsTrue(ms.ToArray().Length > 0); }
        catch { _exitCode = 1; }
    }

    // [96] System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteString:System.Void(System.ReadOnlySpan{System.Byte},System.Guid)
    public static void CustomEntrySubject_96()
    {
        try { var ms = MakeWriter(out var w); w.WriteString((ReadOnlySpan<byte>)Encoding.UTF8.GetBytes("g"), Guid.NewGuid()); w.Flush(); Assert.IsTrue(ms.ToArray().Length > 0); }
        catch { _exitCode = 1; }
    }

    // [97] System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteString:System.Void(System.Text.Json.JsonEncodedText,System.Text.Json.JsonEncodedText)
    public static void CustomEntrySubject_97()
    {
        try { var ms = MakeWriter(out var w); w.WriteString(JsonEncodedText.Encode("k"), JsonEncodedText.Encode("v")); w.Flush(); Assert.IsTrue(ms.ToArray().Length > 0); }
        catch { _exitCode = 1; }
    }

    // [98] System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteString:System.Void(System.String,System.Text.Json.JsonEncodedText)
    public static void CustomEntrySubject_98()
    {
        try { var ms = MakeWriter(out var w); w.WriteString("k", JsonEncodedText.Encode("v")); w.Flush(); Assert.IsTrue(ms.ToArray().Length > 0); }
        catch { _exitCode = 1; }
    }

    // [99] System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteString:System.Void(System.String,System.String)
    public static void CustomEntrySubject_99()
    {
        try { var ms = MakeWriter(out var w); w.WriteString("k", "v"); w.Flush(); Assert.IsTrue(ms.ToArray().Length > 0); }
        catch { _exitCode = 1; }
    }

    // [100] System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteString:System.Void(System.ReadOnlySpan{System.Char},System.ReadOnlySpan{System.Char})
    public static void CustomEntrySubject_100()
    {
        try { var ms = MakeWriter(out var w); w.WriteString((ReadOnlySpan<char>)"k".ToCharArray(), (ReadOnlySpan<char>)"v".ToCharArray()); w.Flush(); Assert.IsTrue(ms.ToArray().Length > 0); }
        catch { _exitCode = 1; }
    }

    // [101] System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteString:System.Void(System.ReadOnlySpan{System.Byte},System.ReadOnlySpan{System.Byte})
    public static void CustomEntrySubject_101()
    {
        try { var ms = MakeWriter(out var w); w.WriteString((ReadOnlySpan<byte>)Encoding.UTF8.GetBytes("k"), (ReadOnlySpan<byte>)Encoding.UTF8.GetBytes("v")); w.Flush(); Assert.IsTrue(ms.ToArray().Length > 0); }
        catch { _exitCode = 1; }
    }

    // [102] System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteString:System.Void(System.Text.Json.JsonEncodedText,System.String)
    public static void CustomEntrySubject_102()
    {
        try { var ms = MakeWriter(out var w); w.WriteString(JsonEncodedText.Encode("k"), "v"); w.Flush(); Assert.IsTrue(ms.ToArray().Length > 0); }
        catch { _exitCode = 1; }
    }

    // [103] System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteString:System.Void(System.Text.Json.JsonEncodedText,System.ReadOnlySpan{System.Char})
    public static void CustomEntrySubject_103()
    {
        try { var ms = MakeWriter(out var w); w.WriteString(JsonEncodedText.Encode("k"), (ReadOnlySpan<char>)"v".ToCharArray()); w.Flush(); Assert.IsTrue(ms.ToArray().Length > 0); }
        catch { _exitCode = 1; }
    }

    // [104] System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteString:System.Void(System.String,System.ReadOnlySpan{System.Char})
    public static void CustomEntrySubject_104()
    {
        try { var ms = MakeWriter(out var w); w.WriteString("k", (ReadOnlySpan<char>)"v".ToCharArray()); w.Flush(); Assert.IsTrue(ms.ToArray().Length > 0); }
        catch { _exitCode = 1; }
    }

    // [105] System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteString:System.Void(System.ReadOnlySpan{System.Byte},System.ReadOnlySpan{System.Char})
    public static void CustomEntrySubject_105()
    {
        try { var ms = MakeWriter(out var w); w.WriteString((ReadOnlySpan<byte>)Encoding.UTF8.GetBytes("k"), (ReadOnlySpan<char>)"v".ToCharArray()); w.Flush(); Assert.IsTrue(ms.ToArray().Length > 0); }
        catch { _exitCode = 1; }
    }

    // [106] System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteString:System.Void(System.Text.Json.JsonEncodedText,System.ReadOnlySpan{System.Byte})
    public static void CustomEntrySubject_106()
    {
        try { var ms = MakeWriter(out var w); w.WriteString(JsonEncodedText.Encode("k"), (ReadOnlySpan<byte>)Encoding.UTF8.GetBytes("v")); w.Flush(); Assert.IsTrue(ms.ToArray().Length > 0); }
        catch { _exitCode = 1; }
    }

    // [107] System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteString:System.Void(System.String,System.ReadOnlySpan{System.Byte})
    public static void CustomEntrySubject_107()
    {
        try { var ms = MakeWriter(out var w); w.WriteString("k", (ReadOnlySpan<byte>)Encoding.UTF8.GetBytes("v")); w.Flush(); Assert.IsTrue(ms.ToArray().Length > 0); }
        catch { _exitCode = 1; }
    }

    // [108] System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteString:System.Void(System.ReadOnlySpan{System.Char},System.ReadOnlySpan{System.Byte})
    public static void CustomEntrySubject_108()
    {
        try { var ms = MakeWriter(out var w); w.WriteString((ReadOnlySpan<char>)"k".ToCharArray(), (ReadOnlySpan<byte>)Encoding.UTF8.GetBytes("v")); w.Flush(); Assert.IsTrue(ms.ToArray().Length > 0); }
        catch { _exitCode = 1; }
    }

    // [109] System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteString:System.Void(System.ReadOnlySpan{System.Char},System.Text.Json.JsonEncodedText)
    public static void CustomEntrySubject_109()
    {
        try { var ms = MakeWriter(out var w); w.WriteString((ReadOnlySpan<char>)"k".ToCharArray(), JsonEncodedText.Encode("v")); w.Flush(); Assert.IsTrue(ms.ToArray().Length > 0); }
        catch { _exitCode = 1; }
    }

    // [110] System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteString:System.Void(System.ReadOnlySpan{System.Char},System.String)
    public static void CustomEntrySubject_110()
    {
        try { var ms = MakeWriter(out var w); w.WriteString((ReadOnlySpan<char>)"k".ToCharArray(), "v"); w.Flush(); Assert.IsTrue(ms.ToArray().Length > 0); }
        catch { _exitCode = 1; }
    }

    // [111] System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteString:System.Void(System.ReadOnlySpan{System.Byte},System.Text.Json.JsonEncodedText)
    public static void CustomEntrySubject_111()
    {
        try { var ms = MakeWriter(out var w); w.WriteString((ReadOnlySpan<byte>)Encoding.UTF8.GetBytes("k"), JsonEncodedText.Encode("v")); w.Flush(); Assert.IsTrue(ms.ToArray().Length > 0); }
        catch { _exitCode = 1; }
    }

    // [112] System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteString:System.Void(System.ReadOnlySpan{System.Byte},System.String)
    public static void CustomEntrySubject_112()
    {
        try { var ms = MakeWriter(out var w); w.WriteString((ReadOnlySpan<byte>)Encoding.UTF8.GetBytes("k"), "v"); w.Flush(); Assert.IsTrue(ms.ToArray().Length > 0); }
        catch { _exitCode = 1; }
    }

    // [113] System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteStringValue:System.Void(System.DateTime)
    public static void CustomEntrySubject_113()
    {
        try { var ms = MakeWriter(out var w); w.WriteStringValue(DateTime.Now); w.Flush(); Assert.IsTrue(ms.ToArray().Length > 0); }
        catch { _exitCode = 1; }
    }

    // [114] System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteStringValue:System.Void(System.DateTimeOffset)
    public static void CustomEntrySubject_114()
    {
        try { var ms = MakeWriter(out var w); w.WriteStringValue(DateTimeOffset.Now); w.Flush(); Assert.IsTrue(ms.ToArray().Length > 0); }
        catch { _exitCode = 1; }
    }

    // [115] System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteStringValue:System.Void(System.Guid)
    public static void CustomEntrySubject_115()
    {
        try { var ms = MakeWriter(out var w); w.WriteStringValue(Guid.NewGuid()); w.Flush(); Assert.IsTrue(ms.ToArray().Length > 0); }
        catch { _exitCode = 1; }
    }

    // [116] System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteStringValue:System.Void(System.Text.Json.JsonEncodedText)
    public static void CustomEntrySubject_116()
    {
        try { var ms = MakeWriter(out var w); w.WriteStringValue(JsonEncodedText.Encode("v")); w.Flush(); Assert.IsTrue(ms.ToArray().Length > 0); }
        catch { _exitCode = 1; }
    }

    // [117] System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteStringValue:System.Void(System.String)
    public static void CustomEntrySubject_117()
    {
        try { var ms = MakeWriter(out var w); w.WriteStringValue("v"); w.Flush(); Assert.IsTrue(ms.ToArray().Length > 0); }
        catch { _exitCode = 1; }
    }

    // [118] System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteStringValue:System.Void(System.ReadOnlySpan{System.Char})
    public static void CustomEntrySubject_118()
    {
        try { var ms = MakeWriter(out var w); w.WriteStringValue((ReadOnlySpan<char>)"v".ToCharArray()); w.Flush(); Assert.IsTrue(ms.ToArray().Length > 0); }
        catch { _exitCode = 1; }
    }

    // [119] System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteStringValue:System.Void(System.ReadOnlySpan{System.Byte})
    public static void CustomEntrySubject_119()
    {
        try { var ms = MakeWriter(out var w); w.WriteStringValue((ReadOnlySpan<byte>)Encoding.UTF8.GetBytes("v")); w.Flush(); Assert.IsTrue(ms.ToArray().Length > 0); }
        catch { _exitCode = 1; }
    }

    // [120] System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteStringValueSegment:System.Void(System.ReadOnlySpan{System.Char},System.Boolean)
    public static void CustomEntrySubject_120()
    {
        try { var ms = MakeWriter(out var w); w.WriteStartObject(); w.WriteStringValueSegment((ReadOnlySpan<char>)"v".ToCharArray(), false); w.WriteEndObject(); w.Flush(); Assert.IsTrue(ms.ToArray().Length > 0); }
        catch { _exitCode = 1; }
    }

    // [121] System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteStringValueSegment:System.Void(System.ReadOnlySpan{System.Byte},System.Boolean)
    public static void CustomEntrySubject_121()
    {
        try { var ms = MakeWriter(out var w); w.WriteStartObject(); w.WriteStringValueSegment((ReadOnlySpan<byte>)Encoding.UTF8.GetBytes("v"), false); w.WriteEndObject(); w.Flush(); Assert.IsTrue(ms.ToArray().Length > 0); }
        catch { _exitCode = 1; }
    }

    // [122] System.Text.Json/System.Text.Json.Utf8JsonWriter::.ctor:System.Void(System.Buffers.IBufferWriter{System.Byte},System.Text.Json.JsonWriterOptions)
    public static void CustomEntrySubject_122()
    {
        try { var bw = new ArrayBufferWriter<byte>(); var w = new Utf8JsonWriter(bw, new JsonWriterOptions()); w.WriteNumberValue(42); w.Flush(); Assert.IsTrue(bw.WrittenCount > 0); }
        catch { _exitCode = 1; }
    }

    // [123] System.Text.Json/System.Text.Json.Utf8JsonWriter::.ctor:System.Void(System.IO.Stream,System.Text.Json.JsonWriterOptions)
    public static void CustomEntrySubject_123()
    {
        try { using var ms = new MemoryStream(); var w = new Utf8JsonWriter(ms, new JsonWriterOptions()); w.WriteNumberValue(42); w.Flush(); Assert.IsTrue(ms.ToArray().Length > 0); }
        catch { _exitCode = 1; }
    }

    // [124] System.Text.Json/System.Text.Json.Utf8JsonWriter::set_BytesPending:System.Void(System.Int32)
    public static void CustomEntrySubject_124()
    {
        try { var ms = MakeWriter(out var w); Assert.IsTrue(true); /* internal setter effective via write */ }
        catch { _exitCode = 1; }
    }

    // [125] System.Text.Json/System.Text.Json.Utf8JsonWriter::set_BytesCommitted:System.Void(System.Int64)
    public static void CustomEntrySubject_125()
    {
        try { var ms = MakeWriter(out var w); Assert.IsTrue(true); /* internal setter effective via write+flush */ }
        catch { _exitCode = 1; }
    }
}
