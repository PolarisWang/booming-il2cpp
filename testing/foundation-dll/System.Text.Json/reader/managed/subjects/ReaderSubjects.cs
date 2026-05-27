// Auto-generated native-AOT entry point
// Family: family/System.Text.Json/reader
// Assembly: System.Text.Json
// Variant: subjects

using System;
using System.Buffers;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Text.Json;
using Chaos.TestFramework;

public static partial class ReaderSubjects
{
    // Inlined exit code — avoids SDK method call resolution in codegen
    public static int _exitCode;

    private static Utf8JsonReader MakeReader(string json)
    {
        return new Utf8JsonReader(Encoding.UTF8.GetBytes(json));
    }

    private static Utf8JsonReader MakeBoolReader() { var r = MakeReader("true"); r.Read(); return r; }
    private static Utf8JsonReader MakeNumReader() { var r = MakeReader("42"); r.Read(); return r; }
    private static Utf8JsonReader MakeStrReader() { var r = MakeReader("\"hello\""); r.Read(); return r; }

    // [0] System.Text.Json/System.Text.Json.Utf8JsonReader::CopyString:System.Int32(System.Span{System.Byte})
    public static void CustomEntrySubject_0()
    {
        try { var r = MakeStrReader(); var buf = new byte[16]; var n = r.CopyString((Span<byte>)buf); Assert.IsTrue(n > 0); }
        catch { _exitCode = 1; }
    }

    // [1] System.Text.Json/System.Text.Json.Utf8JsonReader::CopyString:System.Int32(System.Span{System.Char})
    public static void CustomEntrySubject_1()
    {
        try { var r = MakeStrReader(); var buf = new char[16]; var n = r.CopyString((Span<char>)buf); Assert.IsTrue(n > 0); }
        catch { _exitCode = 1; }
    }

    // [2] System.Text.Json/System.Text.Json.Utf8JsonReader::get_BytesConsumed:System.Int64()
    public static void CustomEntrySubject_2()
    {
        try { var r = MakeStrReader(); Assert.IsTrue(r.BytesConsumed > 0); }
        catch { _exitCode = 1; }
    }

    // [3] System.Text.Json/System.Text.Json.Utf8JsonReader::get_CurrentDepth:System.Int32()
    public static void CustomEntrySubject_3()
    {
        try { var r = MakeBoolReader(); Assert.AreEqual(0, r.CurrentDepth); }
        catch { _exitCode = 1; }
    }

    // [4] System.Text.Json/System.Text.Json.Utf8JsonReader::get_CurrentState:System.Text.Json.JsonReaderState()
    public static void CustomEntrySubject_4()
    {
        try { var r = MakeBoolReader(); var s = r.CurrentState; Assert.IsTrue(true); }
        catch { _exitCode = 1; }
    }

    // [5] System.Text.Json/System.Text.Json.Utf8JsonReader::get_HasValueSequence:System.Boolean()
    public static void CustomEntrySubject_5()
    {
        try { var r = MakeBoolReader(); Assert.IsFalse(r.HasValueSequence); }
        catch { _exitCode = 1; }
    }

    // [6] System.Text.Json/System.Text.Json.Utf8JsonReader::get_IsFinalBlock:System.Boolean()
    public static void CustomEntrySubject_6()
    {
        try { var r = MakeBoolReader(); Assert.IsTrue(r.IsFinalBlock); }
        catch { _exitCode = 1; }
    }

    // [7] System.Text.Json/System.Text.Json.Utf8JsonReader::get_Position:System.SequencePosition()
    public static void CustomEntrySubject_7()
    {
        try { var r = MakeBoolReader(); var p = r.Position; Assert.IsTrue(true); }
        catch { _exitCode = 1; }
    }

    // [8] System.Text.Json/System.Text.Json.Utf8JsonReader::get_TokenStartIndex:System.Int64()
    public static void CustomEntrySubject_8()
    {
        try { var r = MakeBoolReader(); Assert.AreEqual(0L, r.TokenStartIndex); }
        catch { _exitCode = 1; }
    }

    // [9] System.Text.Json/System.Text.Json.Utf8JsonReader::get_TokenType:System.Text.Json.JsonTokenType()
    public static void CustomEntrySubject_9()
    {
        try { var r = MakeBoolReader(); Assert.AreEqual((int)JsonTokenType.True, (int)r.TokenType); }
        catch { _exitCode = 1; }
    }

    // [10] System.Text.Json/System.Text.Json.Utf8JsonReader::get_ValueIsEscaped:System.Boolean()
    public static void CustomEntrySubject_10()
    {
        try { var r = MakeStrReader(); Assert.IsFalse(r.ValueIsEscaped); }
        catch { _exitCode = 1; }
    }

    // [11] System.Text.Json/System.Text.Json.Utf8JsonReader::get_ValueSequence:System.Buffers.ReadOnlySequence{System.Byte}()
    public static void CustomEntrySubject_11()
    {
        try { var r = MakeBoolReader(); var vs = r.ValueSequence; Assert.IsTrue(true); }
        catch { _exitCode = 1; }
    }

    // [12] System.Text.Json/System.Text.Json.Utf8JsonReader::get_ValueSpan:System.ReadOnlySpan{System.Byte}()
    public static void CustomEntrySubject_12()
    {
        try { var r = MakeStrReader(); Assert.IsTrue(r.ValueSpan.Length > 0); }
        catch { _exitCode = 1; }
    }

    // [13] System.Text.Json/System.Text.Json.Utf8JsonReader::GetBoolean:System.Boolean()
    public static void CustomEntrySubject_13()
    {
        try { var r = MakeBoolReader(); Assert.IsTrue(r.GetBoolean()); }
        catch { _exitCode = 1; }
    }

    // [14] System.Text.Json/System.Text.Json.Utf8JsonReader::GetByte:System.Byte()
    public static void CustomEntrySubject_14()
    {
        try { var r = MakeNumReader(); Assert.AreEqual((byte)42, r.GetByte()); }
        catch { _exitCode = 1; }
    }

    // [15] System.Text.Json/System.Text.Json.Utf8JsonReader::GetBytesFromBase64:System.Byte{}()
    public static void CustomEntrySubject_15()
    {
        try { var r = MakeReader("\"AQUIDBAUG\""); r.Read(); var b = r.GetBytesFromBase64(); Assert.IsTrue(b.Length > 0); }
        catch { _exitCode = 1; }
    }

    // [16] System.Text.Json/System.Text.Json.Utf8JsonReader::GetComment:System.String()
    public static void CustomEntrySubject_16()
    {
        try { var r = new Utf8JsonReader(Encoding.UTF8.GetBytes("/* comment */42")); r.Read(); Assert.AreEqual(" comment ", r.GetComment()); }
        catch { _exitCode = 1; }
    }

    // [17] System.Text.Json/System.Text.Json.Utf8JsonReader::GetDateTime:System.DateTime()
    public static void CustomEntrySubject_17()
    {
        try { var r = MakeReader("\"2024-01-01T00:00:00\""); r.Read(); var d = r.GetDateTime(); Assert.IsTrue(d.Year == 2024); }
        catch { _exitCode = 1; }
    }

    // [18] System.Text.Json/System.Text.Json.Utf8JsonReader::GetDateTimeOffset:System.DateTimeOffset()
    public static void CustomEntrySubject_18()
    {
        try { var r = MakeReader("\"2024-01-01T00:00:00Z\""); r.Read(); var d = r.GetDateTimeOffset(); Assert.IsTrue(d.Year == 2024); }
        catch { _exitCode = 1; }
    }

    // [19] System.Text.Json/System.Text.Json.Utf8JsonReader::GetDecimal:System.Decimal()
    public static void CustomEntrySubject_19()
    {
        try { var r = MakeReader("3.14"); r.Read(); Assert.AreEqual(3.14m, r.GetDecimal()); }
        catch { _exitCode = 1; }
    }

    // [20] System.Text.Json/System.Text.Json.Utf8JsonReader::GetDouble:System.Double()
    public static void CustomEntrySubject_20()
    {
        try { var r = MakeReader("3.14"); r.Read(); Assert.AreEqual(3.14, r.GetDouble()); }
        catch { _exitCode = 1; }
    }

    // [21] System.Text.Json/System.Text.Json.Utf8JsonReader::GetGuid:System.Guid()
    public static void CustomEntrySubject_21()
    {
        try { var r = MakeReader("\"00000001-0002-0003-0004-000000000005\""); r.Read(); var g = r.GetGuid(); Assert.IsTrue(true); }
        catch { _exitCode = 1; }
    }

    // [22] System.Text.Json/System.Text.Json.Utf8JsonReader::GetInt16:System.Int16()
    public static void CustomEntrySubject_22()
    {
        try { var r = MakeReader("42"); r.Read(); Assert.AreEqual((short)42, r.GetInt16()); }
        catch { _exitCode = 1; }
    }

    // [23] System.Text.Json/System.Text.Json.Utf8JsonReader::GetInt32:System.Int32()
    public static void CustomEntrySubject_23()
    {
        try { var r = MakeNumReader(); Assert.AreEqual(42, r.GetInt32()); }
        catch { _exitCode = 1; }
    }

    // [24] System.Text.Json/System.Text.Json.Utf8JsonReader::GetInt64:System.Int64()
    public static void CustomEntrySubject_24()
    {
        try { var r = MakeReader("42"); r.Read(); Assert.AreEqual(42L, r.GetInt64()); }
        catch { _exitCode = 1; }
    }

    // [25] System.Text.Json/System.Text.Json.Utf8JsonReader::GetSByte:System.SByte()
    public static void CustomEntrySubject_25()
    {
        try { var r = MakeReader("42"); r.Read(); Assert.AreEqual((sbyte)42, r.GetSByte()); }
        catch { _exitCode = 1; }
    }

    // [26] System.Text.Json/System.Text.Json.Utf8JsonReader::GetSingle:System.Single()
    public static void CustomEntrySubject_26()
    {
        try { var r = MakeReader("3.14"); r.Read(); Assert.AreEqual(3.14f, r.GetSingle()); }
        catch { _exitCode = 1; }
    }

    // [27] System.Text.Json/System.Text.Json.Utf8JsonReader::GetString:System.String()
    public static void CustomEntrySubject_27()
    {
        try { var r = MakeStrReader(); Assert.AreEqual("hello", r.GetString()); }
        catch { _exitCode = 1; }
    }

    // [28] System.Text.Json/System.Text.Json.Utf8JsonReader::GetUInt16:System.UInt16()
    public static void CustomEntrySubject_28()
    {
        try { var r = MakeReader("42"); r.Read(); Assert.AreEqual((ushort)42, r.GetUInt16()); }
        catch { _exitCode = 1; }
    }

    // [29] System.Text.Json/System.Text.Json.Utf8JsonReader::GetUInt32:System.UInt32()
    public static void CustomEntrySubject_29()
    {
        try { var r = MakeReader("42"); r.Read(); Assert.AreEqual(42u, r.GetUInt32()); }
        catch { _exitCode = 1; }
    }

    // [30] System.Text.Json/System.Text.Json.Utf8JsonReader::GetUInt64:System.UInt64()
    public static void CustomEntrySubject_30()
    {
        try { var r = MakeReader("42"); r.Read(); Assert.AreEqual(42UL, r.GetUInt64()); }
        catch { _exitCode = 1; }
    }

    // [31] System.Text.Json/System.Text.Json.Utf8JsonReader::Read:System.Boolean()
    public static void CustomEntrySubject_31()
    {
        try { var r = MakeReader("true"); Assert.IsTrue(r.Read()); Assert.IsFalse(r.Read()); }
        catch { _exitCode = 1; }
    }

    // [32] System.Text.Json/System.Text.Json.Utf8JsonReader::Skip:System.Void()
    public static void CustomEntrySubject_32()
    {
        try { var r = MakeReader("true"); r.Skip(); Assert.IsFalse(r.Read()); }
        catch { _exitCode = 1; }
    }

    // [33] System.Text.Json/System.Text.Json.Utf8JsonReader::TryGetByte:System.Boolean(System.Byte&)
    public static void CustomEntrySubject_33()
    {
        try { var r = MakeNumReader(); Assert.IsTrue(r.TryGetByte(out byte v)); Assert.AreEqual((byte)42, v); }
        catch { _exitCode = 1; }
    }

    // [34] System.Text.Json/System.Text.Json.Utf8JsonReader::TryGetBytesFromBase64:System.Boolean(System.Byte{}&)
    public static void CustomEntrySubject_34()
    {
        try { var r = MakeReader("\"AQUIDBAUG\""); r.Read(); Assert.IsTrue(r.TryGetBytesFromBase64(out byte[]? b)); Assert.IsTrue(b!.Length > 0); }
        catch { _exitCode = 1; }
    }

    // [35] System.Text.Json/System.Text.Json.Utf8JsonReader::TryGetDateTime:System.Boolean(System.DateTime&)
    public static void CustomEntrySubject_35()
    {
        try { var r = MakeReader("\"2024-01-01T00:00:00\""); r.Read(); Assert.IsTrue(r.TryGetDateTime(out DateTime d)); Assert.AreEqual(2024, d.Year); }
        catch { _exitCode = 1; }
    }

    // [36] System.Text.Json/System.Text.Json.Utf8JsonReader::TryGetDateTimeOffset:System.Boolean(System.DateTimeOffset&)
    public static void CustomEntrySubject_36()
    {
        try { var r = MakeReader("\"2024-01-01T00:00:00Z\""); r.Read(); Assert.IsTrue(r.TryGetDateTimeOffset(out DateTimeOffset d)); Assert.AreEqual(2024, d.Year); }
        catch { _exitCode = 1; }
    }

    // [37] System.Text.Json/System.Text.Json.Utf8JsonReader::TryGetDecimal:System.Boolean(System.Decimal&)
    public static void CustomEntrySubject_37()
    {
        try { var r = MakeReader("3.14"); r.Read(); Assert.IsTrue(r.TryGetDecimal(out decimal v)); Assert.AreEqual(3.14m, v); }
        catch { _exitCode = 1; }
    }

    // [38] System.Text.Json/System.Text.Json.Utf8JsonReader::TryGetDouble:System.Boolean(System.Double&)
    public static void CustomEntrySubject_38()
    {
        try { var r = MakeReader("3.14"); r.Read(); Assert.IsTrue(r.TryGetDouble(out double v)); Assert.AreEqual(3.14, v); }
        catch { _exitCode = 1; }
    }

    // [39] System.Text.Json/System.Text.Json.Utf8JsonReader::TryGetGuid:System.Boolean(System.Guid&)
    public static void CustomEntrySubject_39()
    {
        try { var r = MakeReader("\"00000001-0002-0003-0004-000000000005\""); r.Read(); Assert.IsTrue(r.TryGetGuid(out Guid g)); }
        catch { _exitCode = 1; }
    }

    // [40] System.Text.Json/System.Text.Json.Utf8JsonReader::TryGetInt16:System.Boolean(System.Int16&)
    public static void CustomEntrySubject_40()
    {
        try { var r = MakeReader("42"); r.Read(); Assert.IsTrue(r.TryGetInt16(out short v)); Assert.AreEqual((short)42, v); }
        catch { _exitCode = 1; }
    }

    // [41] System.Text.Json/System.Text.Json.Utf8JsonReader::TryGetInt32:System.Boolean(System.Int32&)
    public static void CustomEntrySubject_41()
    {
        try { var r = MakeNumReader(); Assert.IsTrue(r.TryGetInt32(out int v)); Assert.AreEqual(42, v); }
        catch { _exitCode = 1; }
    }

    // [42] System.Text.Json/System.Text.Json.Utf8JsonReader::TryGetInt64:System.Boolean(System.Int64&)
    public static void CustomEntrySubject_42()
    {
        try { var r = MakeReader("42"); r.Read(); Assert.IsTrue(r.TryGetInt64(out long v)); Assert.AreEqual(42L, v); }
        catch { _exitCode = 1; }
    }

    // [43] System.Text.Json/System.Text.Json.Utf8JsonReader::TryGetSByte:System.Boolean(System.SByte&)
    public static void CustomEntrySubject_43()
    {
        try { var r = MakeReader("42"); r.Read(); Assert.IsTrue(r.TryGetSByte(out sbyte v)); Assert.AreEqual((sbyte)42, v); }
        catch { _exitCode = 1; }
    }

    // [44] System.Text.Json/System.Text.Json.Utf8JsonReader::TryGetSingle:System.Boolean(System.Single&)
    public static void CustomEntrySubject_44()
    {
        try { var r = MakeReader("3.14"); r.Read(); Assert.IsTrue(r.TryGetSingle(out float v)); Assert.AreEqual(3.14f, v); }
        catch { _exitCode = 1; }
    }

    // [45] System.Text.Json/System.Text.Json.Utf8JsonReader::TryGetUInt16:System.Boolean(System.UInt16&)
    public static void CustomEntrySubject_45()
    {
        try { var r = MakeReader("42"); r.Read(); Assert.IsTrue(r.TryGetUInt16(out ushort v)); Assert.AreEqual((ushort)42, v); }
        catch { _exitCode = 1; }
    }

    // [46] System.Text.Json/System.Text.Json.Utf8JsonReader::TryGetUInt32:System.Boolean(System.UInt32&)
    public static void CustomEntrySubject_46()
    {
        try { var r = MakeReader("42"); r.Read(); Assert.IsTrue(r.TryGetUInt32(out uint v)); Assert.AreEqual(42u, v); }
        catch { _exitCode = 1; }
    }

    // [47] System.Text.Json/System.Text.Json.Utf8JsonReader::TryGetUInt64:System.Boolean(System.UInt64&)
    public static void CustomEntrySubject_47()
    {
        try { var r = MakeReader("42"); r.Read(); Assert.IsTrue(r.TryGetUInt64(out ulong v)); Assert.AreEqual(42UL, v); }
        catch { _exitCode = 1; }
    }

    // [48] System.Text.Json/System.Text.Json.Utf8JsonReader::TrySkip:System.Boolean()
    public static void CustomEntrySubject_48()
    {
        try { var r = MakeReader("true"); Assert.IsTrue(r.TrySkip()); }
        catch { _exitCode = 1; }
    }

    // [49] System.Text.Json/System.Text.Json.Utf8JsonReader::ValueTextEquals:System.Boolean(System.ReadOnlySpan{System.Byte})
    public static void CustomEntrySubject_49()
    {
        try { var r = MakeStrReader(); Assert.IsTrue(r.ValueTextEquals((ReadOnlySpan<byte>)Encoding.UTF8.GetBytes("hello"))); }
        catch { _exitCode = 1; }
    }

    // [50] System.Text.Json/System.Text.Json.Utf8JsonReader::ValueTextEquals:System.Boolean(System.String)
    public static void CustomEntrySubject_50()
    {
        try { var r = MakeStrReader(); Assert.IsTrue(r.ValueTextEquals("hello")); }
        catch { _exitCode = 1; }
    }

    // [51] System.Text.Json/System.Text.Json.Utf8JsonReader::ValueTextEquals:System.Boolean(System.ReadOnlySpan{System.Char})
    public static void CustomEntrySubject_51()
    {
        try { var r = MakeStrReader(); Assert.IsTrue(r.ValueTextEquals((ReadOnlySpan<char>)"hello".ToCharArray())); }
        catch { _exitCode = 1; }
    }

    // [52] System.Text.Json/System.Text.Json.Utf8JsonReader::.ctor:System.Void(System.ReadOnlySpan{System.Byte},System.Boolean,System.Text.Json.JsonReaderState)
    public static void CustomEntrySubject_52()
    {
        try { var r = new Utf8JsonReader(Encoding.UTF8.GetBytes("true"), true, default(JsonReaderState)); Assert.IsTrue(true); }
        catch { _exitCode = 1; }
    }

    // [53] System.Text.Json/System.Text.Json.Utf8JsonReader::.ctor:System.Void(System.ReadOnlySpan{System.Byte},System.Text.Json.JsonReaderOptions)
    public static void CustomEntrySubject_53()
    {
        try { var r = new Utf8JsonReader(Encoding.UTF8.GetBytes("true"), new JsonReaderOptions()); Assert.IsTrue(true); }
        catch { _exitCode = 1; }
    }

    // [54] System.Text.Json/System.Text.Json.Utf8JsonReader::.ctor:System.Void(System.Buffers.ReadOnlySequence{System.Byte},System.Boolean,System.Text.Json.JsonReaderState)
    public static void CustomEntrySubject_54()
    {
        try { var seq = new ReadOnlySequence<byte>(Encoding.UTF8.GetBytes("true")); var r = new Utf8JsonReader(seq, true, default(JsonReaderState)); Assert.IsTrue(true); }
        catch { _exitCode = 1; }
    }

    // [55] System.Text.Json/System.Text.Json.Utf8JsonReader::.ctor:System.Void(System.Buffers.ReadOnlySequence{System.Byte},System.Text.Json.JsonReaderOptions)
    public static void CustomEntrySubject_55()
    {
        try { var seq = new ReadOnlySequence<byte>(Encoding.UTF8.GetBytes("true")); var r = new Utf8JsonReader(seq, new JsonReaderOptions()); Assert.IsTrue(true); }
        catch { _exitCode = 1; }
    }

    // [56] System.Text.Json/System.Text.Json.Utf8JsonReader::set_ValueSpan:System.Void(System.ReadOnlySpan{System.Byte})
    public static void CustomEntrySubject_56()
    {
        try { var r = MakeStrReader(); Assert.IsTrue(true); /* setter tested via constructor */ }
        catch { _exitCode = 1; }
    }

    // [57] System.Text.Json/System.Text.Json.Utf8JsonReader::set_TokenStartIndex:System.Void(System.Int64)
    public static void CustomEntrySubject_57()
    {
        try { var r = MakeBoolReader(); Assert.IsTrue(true); /* setter tested via constructor */ }
        catch { _exitCode = 1; }
    }

    // [58] System.Text.Json/System.Text.Json.Utf8JsonReader::set_HasValueSequence:System.Void(System.Boolean)
    public static void CustomEntrySubject_58()
    {
        try { var r = MakeBoolReader(); Assert.IsTrue(true); /* setter tested via constructor */ }
        catch { _exitCode = 1; }
    }

    // [59] System.Text.Json/System.Text.Json.Utf8JsonReader::set_ValueIsEscaped:System.Void(System.Boolean)
    public static void CustomEntrySubject_59()
    {
        try { var r = MakeStrReader(); Assert.IsTrue(true); /* setter tested via constructor */ }
        catch { _exitCode = 1; }
    }

    // [60] System.Text.Json/System.Text.Json.Utf8JsonReader::set_ValueSequence:System.Void(System.Buffers.ReadOnlySequence{System.Byte})
    public static void CustomEntrySubject_60()
    {
        try { var r = MakeBoolReader(); Assert.IsTrue(true); /* setter tested via constructor */ }
        catch { _exitCode = 1; }
    }
}
