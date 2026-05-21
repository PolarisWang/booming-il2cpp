// Auto-generated native-AOT entry point
// Family: family/System.Text.Json/writer
// Assembly: System.Text.Json
// Variant: benchmark

using System;
using System.Buffers;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text.Json;
using System.Threading;
using System.Threading.Tasks;

public static partial class WriterNativeEntry
{
    // Inlined exit code — avoids SDK method call resolution in codegen
    public static int _exitCode;

    // [0] System.Text.Json/System.Text.Json.Utf8JsonWriter::Dispose:System.Void()
    public static void Method0()
    {
        try { Utf8JsonWriter.Dispose(); }
        catch { _exitCode = 1; }
    }

    // [1] System.Text.Json/System.Text.Json.Utf8JsonWriter::DisposeAsync:System.Threading.Tasks.ValueTask()
    public static void Method1()
    {
        try { if ((int)(Utf8JsonWriter.DisposeAsync()) != (int)(Utf8JsonWriter.DisposeAsync())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [2] System.Text.Json/System.Text.Json.Utf8JsonWriter::Flush:System.Void()
    public static void Method2()
    {
        try { Utf8JsonWriter.Flush(); }
        catch { _exitCode = 1; }
    }

    // [3] System.Text.Json/System.Text.Json.Utf8JsonWriter::FlushAsync:System.Threading.Tasks.Task(System.Threading.CancellationToken)
    public static void Method3()
    {
        try { if (((Utf8JsonWriter.FlushAsync(null!)).GetHashCode()) != ((Utf8JsonWriter.FlushAsync(null!)).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [4] System.Text.Json/System.Text.Json.Utf8JsonWriter::get_BytesCommitted:System.Int64()
    public static void Method4()
    {
        try { if ((int)(Utf8JsonWriter.BytesCommitted) != (int)(Utf8JsonWriter.BytesCommitted)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [5] System.Text.Json/System.Text.Json.Utf8JsonWriter::get_BytesPending:System.Int32()
    public static void Method5()
    {
        try { if (Utf8JsonWriter.BytesPending != Utf8JsonWriter.BytesPending) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [6] System.Text.Json/System.Text.Json.Utf8JsonWriter::get_CurrentDepth:System.Int32()
    public static void Method6()
    {
        try { if (Utf8JsonWriter.CurrentDepth != Utf8JsonWriter.CurrentDepth) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [7] System.Text.Json/System.Text.Json.Utf8JsonWriter::get_Options:System.Text.Json.JsonWriterOptions()
    public static void Method7()
    {
        try { if ((int)(Utf8JsonWriter.Options) != (int)(Utf8JsonWriter.Options)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [8] System.Text.Json/System.Text.Json.Utf8JsonWriter::Reset:System.Void()
    public static void Method8()
    {
        try { Utf8JsonWriter.Reset(); }
        catch { _exitCode = 1; }
    }

    // [9] System.Text.Json/System.Text.Json.Utf8JsonWriter::Reset:System.Void(System.IO.Stream)
    public static void Method9()
    {
        try { Utf8JsonWriter.Reset(null!); }
        catch { _exitCode = 1; }
    }

    // [10] System.Text.Json/System.Text.Json.Utf8JsonWriter::Reset:System.Void(System.Buffers.IBufferWriter{System.Byte})
    public static void Method10()
    {
        try { Utf8JsonWriter.Reset(null!); }
        catch { _exitCode = 1; }
    }

    // [11] System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteBase64String:System.Void(System.Text.Json.JsonEncodedText,System.ReadOnlySpan{System.Byte})
    public static void Method11()
    {
        try { Utf8JsonWriter.WriteBase64String(null!, null!); }
        catch { _exitCode = 1; }
    }

    // [12] System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteBase64String:System.Void(System.String,System.ReadOnlySpan{System.Byte})
    public static void Method12()
    {
        try { Utf8JsonWriter.WriteBase64String("hello", null!); }
        catch { _exitCode = 1; }
    }

    // [13] System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteBase64String:System.Void(System.ReadOnlySpan{System.Char},System.ReadOnlySpan{System.Byte})
    public static void Method13()
    {
        try { Utf8JsonWriter.WriteBase64String(null!, null!); }
        catch { _exitCode = 1; }
    }

    // [14] System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteBase64String:System.Void(System.ReadOnlySpan{System.Byte},System.ReadOnlySpan{System.Byte})
    public static void Method14()
    {
        try { Utf8JsonWriter.WriteBase64String(null!, null!); }
        catch { _exitCode = 1; }
    }

    // [15] System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteBase64StringSegment:System.Void(System.ReadOnlySpan{System.Byte},System.Boolean)
    public static void Method15()
    {
        try { Utf8JsonWriter.WriteBase64StringSegment(null!, true); }
        catch { _exitCode = 1; }
    }

    // [16] System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteBase64StringValue:System.Void(System.ReadOnlySpan{System.Byte})
    public static void Method16()
    {
        try { Utf8JsonWriter.WriteBase64StringValue(null!); }
        catch { _exitCode = 1; }
    }

    // [17] System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteBoolean:System.Void(System.Text.Json.JsonEncodedText,System.Boolean)
    public static void Method17()
    {
        try { Utf8JsonWriter.WriteBoolean(null!, true); }
        catch { _exitCode = 1; }
    }

    // [18] System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteBoolean:System.Void(System.String,System.Boolean)
    public static void Method18()
    {
        try { Utf8JsonWriter.WriteBoolean("hello", true); }
        catch { _exitCode = 1; }
    }

    // [19] System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteBoolean:System.Void(System.ReadOnlySpan{System.Char},System.Boolean)
    public static void Method19()
    {
        try { Utf8JsonWriter.WriteBoolean(null!, true); }
        catch { _exitCode = 1; }
    }

    // [20] System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteBoolean:System.Void(System.ReadOnlySpan{System.Byte},System.Boolean)
    public static void Method20()
    {
        try { Utf8JsonWriter.WriteBoolean(null!, true); }
        catch { _exitCode = 1; }
    }

    // [21] System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteBooleanValue:System.Void(System.Boolean)
    public static void Method21()
    {
        try { Utf8JsonWriter.WriteBooleanValue(true); }
        catch { _exitCode = 1; }
    }

    // [22] System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteCommentValue:System.Void(System.String)
    public static void Method22()
    {
        try { Utf8JsonWriter.WriteCommentValue("hello"); }
        catch { _exitCode = 1; }
    }

    // [23] System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteCommentValue:System.Void(System.ReadOnlySpan{System.Char})
    public static void Method23()
    {
        try { Utf8JsonWriter.WriteCommentValue(null!); }
        catch { _exitCode = 1; }
    }

    // [24] System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteCommentValue:System.Void(System.ReadOnlySpan{System.Byte})
    public static void Method24()
    {
        try { Utf8JsonWriter.WriteCommentValue(null!); }
        catch { _exitCode = 1; }
    }

    // [25] System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteEndArray:System.Void()
    public static void Method25()
    {
        try { Utf8JsonWriter.WriteEndArray(); }
        catch { _exitCode = 1; }
    }

    // [26] System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteEndObject:System.Void()
    public static void Method26()
    {
        try { Utf8JsonWriter.WriteEndObject(); }
        catch { _exitCode = 1; }
    }

    // [27] System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteNull:System.Void(System.Text.Json.JsonEncodedText)
    public static void Method27()
    {
        try { Utf8JsonWriter.WriteNull(null!); }
        catch { _exitCode = 1; }
    }

    // [28] System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteNull:System.Void(System.String)
    public static void Method28()
    {
        try { Utf8JsonWriter.WriteNull("hello"); }
        catch { _exitCode = 1; }
    }

    // [29] System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteNull:System.Void(System.ReadOnlySpan{System.Char})
    public static void Method29()
    {
        try { Utf8JsonWriter.WriteNull(null!); }
        catch { _exitCode = 1; }
    }

    // [30] System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteNull:System.Void(System.ReadOnlySpan{System.Byte})
    public static void Method30()
    {
        try { Utf8JsonWriter.WriteNull(null!); }
        catch { _exitCode = 1; }
    }

    // [31] System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteNullValue:System.Void()
    public static void Method31()
    {
        try { Utf8JsonWriter.WriteNullValue(); }
        catch { _exitCode = 1; }
    }

    // [32] System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteNumber:System.Void(System.Text.Json.JsonEncodedText,System.Decimal)
    public static void Method32()
    {
        try { Utf8JsonWriter.WriteNumber(null!, 42m); }
        catch { _exitCode = 1; }
    }

    // [33] System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteNumber:System.Void(System.String,System.Decimal)
    public static void Method33()
    {
        try { Utf8JsonWriter.WriteNumber("hello", 42m); }
        catch { _exitCode = 1; }
    }

    // [34] System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteNumber:System.Void(System.ReadOnlySpan{System.Char},System.Decimal)
    public static void Method34()
    {
        try { Utf8JsonWriter.WriteNumber(null!, 42m); }
        catch { _exitCode = 1; }
    }

    // [35] System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteNumber:System.Void(System.ReadOnlySpan{System.Byte},System.Decimal)
    public static void Method35()
    {
        try { Utf8JsonWriter.WriteNumber(null!, 42m); }
        catch { _exitCode = 1; }
    }

    // [36] System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteNumber:System.Void(System.Text.Json.JsonEncodedText,System.Double)
    public static void Method36()
    {
        try { Utf8JsonWriter.WriteNumber(null!, 42.0); }
        catch { _exitCode = 1; }
    }

    // [37] System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteNumber:System.Void(System.String,System.Double)
    public static void Method37()
    {
        try { Utf8JsonWriter.WriteNumber("hello", 42.0); }
        catch { _exitCode = 1; }
    }

    // [38] System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteNumber:System.Void(System.ReadOnlySpan{System.Char},System.Double)
    public static void Method38()
    {
        try { Utf8JsonWriter.WriteNumber(null!, 42.0); }
        catch { _exitCode = 1; }
    }

    // [39] System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteNumber:System.Void(System.ReadOnlySpan{System.Byte},System.Double)
    public static void Method39()
    {
        try { Utf8JsonWriter.WriteNumber(null!, 42.0); }
        catch { _exitCode = 1; }
    }

    // [40] System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteNumber:System.Void(System.Text.Json.JsonEncodedText,System.Single)
    public static void Method40()
    {
        try { Utf8JsonWriter.WriteNumber(null!, 42.0f); }
        catch { _exitCode = 1; }
    }

    // [41] System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteNumber:System.Void(System.String,System.Single)
    public static void Method41()
    {
        try { Utf8JsonWriter.WriteNumber("hello", 42.0f); }
        catch { _exitCode = 1; }
    }

    // [42] System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteNumber:System.Void(System.ReadOnlySpan{System.Char},System.Single)
    public static void Method42()
    {
        try { Utf8JsonWriter.WriteNumber(null!, 42.0f); }
        catch { _exitCode = 1; }
    }

    // [43] System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteNumber:System.Void(System.ReadOnlySpan{System.Byte},System.Single)
    public static void Method43()
    {
        try { Utf8JsonWriter.WriteNumber(null!, 42.0f); }
        catch { _exitCode = 1; }
    }

    // [44] System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteNumber:System.Void(System.Text.Json.JsonEncodedText,System.Int64)
    public static void Method44()
    {
        try { Utf8JsonWriter.WriteNumber(null!, 42L); }
        catch { _exitCode = 1; }
    }

    // [45] System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteNumber:System.Void(System.String,System.Int64)
    public static void Method45()
    {
        try { Utf8JsonWriter.WriteNumber("hello", 42L); }
        catch { _exitCode = 1; }
    }

    // [46] System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteNumber:System.Void(System.ReadOnlySpan{System.Char},System.Int64)
    public static void Method46()
    {
        try { Utf8JsonWriter.WriteNumber(null!, 42L); }
        catch { _exitCode = 1; }
    }

    // [47] System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteNumber:System.Void(System.ReadOnlySpan{System.Byte},System.Int64)
    public static void Method47()
    {
        try { Utf8JsonWriter.WriteNumber(null!, 42L); }
        catch { _exitCode = 1; }
    }

    // [48] System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteNumber:System.Void(System.Text.Json.JsonEncodedText,System.Int32)
    public static void Method48()
    {
        try { Utf8JsonWriter.WriteNumber(null!, 42); }
        catch { _exitCode = 1; }
    }

    // [49] System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteNumber:System.Void(System.String,System.Int32)
    public static void Method49()
    {
        try { Utf8JsonWriter.WriteNumber("hello", 42); }
        catch { _exitCode = 1; }
    }

    // [50] System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteNumber:System.Void(System.ReadOnlySpan{System.Char},System.Int32)
    public static void Method50()
    {
        try { Utf8JsonWriter.WriteNumber(null!, 42); }
        catch { _exitCode = 1; }
    }

    // [51] System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteNumber:System.Void(System.ReadOnlySpan{System.Byte},System.Int32)
    public static void Method51()
    {
        try { Utf8JsonWriter.WriteNumber(null!, 42); }
        catch { _exitCode = 1; }
    }

    // [52] System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteNumber:System.Void(System.Text.Json.JsonEncodedText,System.UInt64)
    public static void Method52()
    {
        try { Utf8JsonWriter.WriteNumber(null!, 42uL); }
        catch { _exitCode = 1; }
    }

    // [53] System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteNumber:System.Void(System.String,System.UInt64)
    public static void Method53()
    {
        try { Utf8JsonWriter.WriteNumber("hello", 42uL); }
        catch { _exitCode = 1; }
    }

    // [54] System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteNumber:System.Void(System.ReadOnlySpan{System.Char},System.UInt64)
    public static void Method54()
    {
        try { Utf8JsonWriter.WriteNumber(null!, 42uL); }
        catch { _exitCode = 1; }
    }

    // [55] System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteNumber:System.Void(System.ReadOnlySpan{System.Byte},System.UInt64)
    public static void Method55()
    {
        try { Utf8JsonWriter.WriteNumber(null!, 42uL); }
        catch { _exitCode = 1; }
    }

    // [56] System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteNumber:System.Void(System.Text.Json.JsonEncodedText,System.UInt32)
    public static void Method56()
    {
        try { Utf8JsonWriter.WriteNumber(null!, 42u); }
        catch { _exitCode = 1; }
    }

    // [57] System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteNumber:System.Void(System.String,System.UInt32)
    public static void Method57()
    {
        try { Utf8JsonWriter.WriteNumber("hello", 42u); }
        catch { _exitCode = 1; }
    }

    // [58] System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteNumber:System.Void(System.ReadOnlySpan{System.Char},System.UInt32)
    public static void Method58()
    {
        try { Utf8JsonWriter.WriteNumber(null!, 42u); }
        catch { _exitCode = 1; }
    }

    // [59] System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteNumber:System.Void(System.ReadOnlySpan{System.Byte},System.UInt32)
    public static void Method59()
    {
        try { Utf8JsonWriter.WriteNumber(null!, 42u); }
        catch { _exitCode = 1; }
    }

    // [60] System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteNumberValue:System.Void(System.Decimal)
    public static void Method60()
    {
        try { Utf8JsonWriter.WriteNumberValue(42m); }
        catch { _exitCode = 1; }
    }

    // [61] System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteNumberValue:System.Void(System.Double)
    public static void Method61()
    {
        try { Utf8JsonWriter.WriteNumberValue(42.0); }
        catch { _exitCode = 1; }
    }

    // [62] System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteNumberValue:System.Void(System.Single)
    public static void Method62()
    {
        try { Utf8JsonWriter.WriteNumberValue(42.0f); }
        catch { _exitCode = 1; }
    }

    // [63] System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteNumberValue:System.Void(System.Int32)
    public static void Method63()
    {
        try { Utf8JsonWriter.WriteNumberValue(42); }
        catch { _exitCode = 1; }
    }

    // [64] System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteNumberValue:System.Void(System.Int64)
    public static void Method64()
    {
        try { Utf8JsonWriter.WriteNumberValue(42L); }
        catch { _exitCode = 1; }
    }

    // [65] System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteNumberValue:System.Void(System.UInt32)
    public static void Method65()
    {
        try { Utf8JsonWriter.WriteNumberValue(42u); }
        catch { _exitCode = 1; }
    }

    // [66] System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteNumberValue:System.Void(System.UInt64)
    public static void Method66()
    {
        try { Utf8JsonWriter.WriteNumberValue(42uL); }
        catch { _exitCode = 1; }
    }

    // [67] System.Text.Json/System.Text.Json.Utf8JsonWriter::WritePropertyName:System.Void(System.Text.Json.JsonEncodedText)
    public static void Method67()
    {
        try { Utf8JsonWriter.WritePropertyName(null!); }
        catch { _exitCode = 1; }
    }

    // [68] System.Text.Json/System.Text.Json.Utf8JsonWriter::WritePropertyName:System.Void(System.String)
    public static void Method68()
    {
        try { Utf8JsonWriter.WritePropertyName("hello"); }
        catch { _exitCode = 1; }
    }

    // [69] System.Text.Json/System.Text.Json.Utf8JsonWriter::WritePropertyName:System.Void(System.ReadOnlySpan{System.Char})
    public static void Method69()
    {
        try { Utf8JsonWriter.WritePropertyName(null!); }
        catch { _exitCode = 1; }
    }

    // [70] System.Text.Json/System.Text.Json.Utf8JsonWriter::WritePropertyName:System.Void(System.ReadOnlySpan{System.Byte})
    public static void Method70()
    {
        try { Utf8JsonWriter.WritePropertyName(null!); }
        catch { _exitCode = 1; }
    }

    // [71] System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteRawValue:System.Void(System.String,System.Boolean)
    public static void Method71()
    {
        try { Utf8JsonWriter.WriteRawValue("hello", true); }
        catch { _exitCode = 1; }
    }

    // [72] System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteRawValue:System.Void(System.ReadOnlySpan{System.Char},System.Boolean)
    public static void Method72()
    {
        try { Utf8JsonWriter.WriteRawValue(null!, true); }
        catch { _exitCode = 1; }
    }

    // [73] System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteRawValue:System.Void(System.ReadOnlySpan{System.Byte},System.Boolean)
    public static void Method73()
    {
        try { Utf8JsonWriter.WriteRawValue(null!, true); }
        catch { _exitCode = 1; }
    }

    // [74] System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteRawValue:System.Void(System.Buffers.ReadOnlySequence{System.Byte},System.Boolean)
    public static void Method74()
    {
        try { Utf8JsonWriter.WriteRawValue(null!, true); }
        catch { _exitCode = 1; }
    }

    // [75] System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteStartArray:System.Void()
    public static void Method75()
    {
        try { Utf8JsonWriter.WriteStartArray(); }
        catch { _exitCode = 1; }
    }

    // [76] System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteStartArray:System.Void(System.Text.Json.JsonEncodedText)
    public static void Method76()
    {
        try { Utf8JsonWriter.WriteStartArray(null!); }
        catch { _exitCode = 1; }
    }

    // [77] System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteStartArray:System.Void(System.ReadOnlySpan{System.Byte})
    public static void Method77()
    {
        try { Utf8JsonWriter.WriteStartArray(null!); }
        catch { _exitCode = 1; }
    }

    // [78] System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteStartArray:System.Void(System.String)
    public static void Method78()
    {
        try { Utf8JsonWriter.WriteStartArray("hello"); }
        catch { _exitCode = 1; }
    }

    // [79] System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteStartArray:System.Void(System.ReadOnlySpan{System.Char})
    public static void Method79()
    {
        try { Utf8JsonWriter.WriteStartArray(null!); }
        catch { _exitCode = 1; }
    }

    // [80] System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteStartObject:System.Void()
    public static void Method80()
    {
        try { Utf8JsonWriter.WriteStartObject(); }
        catch { _exitCode = 1; }
    }

    // [81] System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteStartObject:System.Void(System.Text.Json.JsonEncodedText)
    public static void Method81()
    {
        try { Utf8JsonWriter.WriteStartObject(null!); }
        catch { _exitCode = 1; }
    }

    // [82] System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteStartObject:System.Void(System.ReadOnlySpan{System.Byte})
    public static void Method82()
    {
        try { Utf8JsonWriter.WriteStartObject(null!); }
        catch { _exitCode = 1; }
    }

    // [83] System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteStartObject:System.Void(System.String)
    public static void Method83()
    {
        try { Utf8JsonWriter.WriteStartObject("hello"); }
        catch { _exitCode = 1; }
    }

    // [84] System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteStartObject:System.Void(System.ReadOnlySpan{System.Char})
    public static void Method84()
    {
        try { Utf8JsonWriter.WriteStartObject(null!); }
        catch { _exitCode = 1; }
    }

    // [85] System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteString:System.Void(System.Text.Json.JsonEncodedText,System.DateTime)
    public static void Method85()
    {
        try { Utf8JsonWriter.WriteString(null!, DateTime.UtcNow); }
        catch { _exitCode = 1; }
    }

    // [86] System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteString:System.Void(System.String,System.DateTime)
    public static void Method86()
    {
        try { Utf8JsonWriter.WriteString("hello", DateTime.UtcNow); }
        catch { _exitCode = 1; }
    }

    // [87] System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteString:System.Void(System.ReadOnlySpan{System.Char},System.DateTime)
    public static void Method87()
    {
        try { Utf8JsonWriter.WriteString(null!, DateTime.UtcNow); }
        catch { _exitCode = 1; }
    }

    // [88] System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteString:System.Void(System.ReadOnlySpan{System.Byte},System.DateTime)
    public static void Method88()
    {
        try { Utf8JsonWriter.WriteString(null!, DateTime.UtcNow); }
        catch { _exitCode = 1; }
    }

    // [89] System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteString:System.Void(System.Text.Json.JsonEncodedText,System.DateTimeOffset)
    public static void Method89()
    {
        try { Utf8JsonWriter.WriteString(null!, null!); }
        catch { _exitCode = 1; }
    }

    // [90] System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteString:System.Void(System.String,System.DateTimeOffset)
    public static void Method90()
    {
        try { Utf8JsonWriter.WriteString("hello", null!); }
        catch { _exitCode = 1; }
    }

    // [91] System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteString:System.Void(System.ReadOnlySpan{System.Char},System.DateTimeOffset)
    public static void Method91()
    {
        try { Utf8JsonWriter.WriteString(null!, null!); }
        catch { _exitCode = 1; }
    }

    // [92] System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteString:System.Void(System.ReadOnlySpan{System.Byte},System.DateTimeOffset)
    public static void Method92()
    {
        try { Utf8JsonWriter.WriteString(null!, null!); }
        catch { _exitCode = 1; }
    }

    // [93] System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteString:System.Void(System.Text.Json.JsonEncodedText,System.Guid)
    public static void Method93()
    {
        try { Utf8JsonWriter.WriteString(null!, Guid.Empty); }
        catch { _exitCode = 1; }
    }

    // [94] System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteString:System.Void(System.String,System.Guid)
    public static void Method94()
    {
        try { Utf8JsonWriter.WriteString("hello", Guid.Empty); }
        catch { _exitCode = 1; }
    }

    // [95] System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteString:System.Void(System.ReadOnlySpan{System.Char},System.Guid)
    public static void Method95()
    {
        try { Utf8JsonWriter.WriteString(null!, Guid.Empty); }
        catch { _exitCode = 1; }
    }

    // [96] System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteString:System.Void(System.ReadOnlySpan{System.Byte},System.Guid)
    public static void Method96()
    {
        try { Utf8JsonWriter.WriteString(null!, Guid.Empty); }
        catch { _exitCode = 1; }
    }

    // [97] System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteString:System.Void(System.Text.Json.JsonEncodedText,System.Text.Json.JsonEncodedText)
    public static void Method97()
    {
        try { Utf8JsonWriter.WriteString(null!, null!); }
        catch { _exitCode = 1; }
    }

    // [98] System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteString:System.Void(System.String,System.Text.Json.JsonEncodedText)
    public static void Method98()
    {
        try { Utf8JsonWriter.WriteString("hello", null!); }
        catch { _exitCode = 1; }
    }

    // [99] System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteString:System.Void(System.String,System.String)
    public static void Method99()
    {
        try { Utf8JsonWriter.WriteString("hello", "hello"); }
        catch { _exitCode = 1; }
    }

    // [100] System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteString:System.Void(System.ReadOnlySpan{System.Char},System.ReadOnlySpan{System.Char})
    public static void Method100()
    {
        try { Utf8JsonWriter.WriteString(null!, null!); }
        catch { _exitCode = 1; }
    }

    // [101] System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteString:System.Void(System.ReadOnlySpan{System.Byte},System.ReadOnlySpan{System.Byte})
    public static void Method101()
    {
        try { Utf8JsonWriter.WriteString(null!, null!); }
        catch { _exitCode = 1; }
    }

    // [102] System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteString:System.Void(System.Text.Json.JsonEncodedText,System.String)
    public static void Method102()
    {
        try { Utf8JsonWriter.WriteString(null!, "hello"); }
        catch { _exitCode = 1; }
    }

    // [103] System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteString:System.Void(System.Text.Json.JsonEncodedText,System.ReadOnlySpan{System.Char})
    public static void Method103()
    {
        try { Utf8JsonWriter.WriteString(null!, null!); }
        catch { _exitCode = 1; }
    }

    // [104] System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteString:System.Void(System.String,System.ReadOnlySpan{System.Char})
    public static void Method104()
    {
        try { Utf8JsonWriter.WriteString("hello", null!); }
        catch { _exitCode = 1; }
    }

    // [105] System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteString:System.Void(System.ReadOnlySpan{System.Byte},System.ReadOnlySpan{System.Char})
    public static void Method105()
    {
        try { Utf8JsonWriter.WriteString(null!, null!); }
        catch { _exitCode = 1; }
    }

    // [106] System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteString:System.Void(System.Text.Json.JsonEncodedText,System.ReadOnlySpan{System.Byte})
    public static void Method106()
    {
        try { Utf8JsonWriter.WriteString(null!, null!); }
        catch { _exitCode = 1; }
    }

    // [107] System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteString:System.Void(System.String,System.ReadOnlySpan{System.Byte})
    public static void Method107()
    {
        try { Utf8JsonWriter.WriteString("hello", null!); }
        catch { _exitCode = 1; }
    }

    // [108] System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteString:System.Void(System.ReadOnlySpan{System.Char},System.ReadOnlySpan{System.Byte})
    public static void Method108()
    {
        try { Utf8JsonWriter.WriteString(null!, null!); }
        catch { _exitCode = 1; }
    }

    // [109] System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteString:System.Void(System.ReadOnlySpan{System.Char},System.Text.Json.JsonEncodedText)
    public static void Method109()
    {
        try { Utf8JsonWriter.WriteString(null!, null!); }
        catch { _exitCode = 1; }
    }

    // [110] System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteString:System.Void(System.ReadOnlySpan{System.Char},System.String)
    public static void Method110()
    {
        try { Utf8JsonWriter.WriteString(null!, "hello"); }
        catch { _exitCode = 1; }
    }

    // [111] System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteString:System.Void(System.ReadOnlySpan{System.Byte},System.Text.Json.JsonEncodedText)
    public static void Method111()
    {
        try { Utf8JsonWriter.WriteString(null!, null!); }
        catch { _exitCode = 1; }
    }

    // [112] System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteString:System.Void(System.ReadOnlySpan{System.Byte},System.String)
    public static void Method112()
    {
        try { Utf8JsonWriter.WriteString(null!, "hello"); }
        catch { _exitCode = 1; }
    }

    // [113] System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteStringValue:System.Void(System.DateTime)
    public static void Method113()
    {
        try { Utf8JsonWriter.WriteStringValue(DateTime.UtcNow); }
        catch { _exitCode = 1; }
    }

    // [114] System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteStringValue:System.Void(System.DateTimeOffset)
    public static void Method114()
    {
        try { Utf8JsonWriter.WriteStringValue(null!); }
        catch { _exitCode = 1; }
    }

    // [115] System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteStringValue:System.Void(System.Guid)
    public static void Method115()
    {
        try { Utf8JsonWriter.WriteStringValue(Guid.Empty); }
        catch { _exitCode = 1; }
    }

    // [116] System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteStringValue:System.Void(System.Text.Json.JsonEncodedText)
    public static void Method116()
    {
        try { Utf8JsonWriter.WriteStringValue(null!); }
        catch { _exitCode = 1; }
    }

    // [117] System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteStringValue:System.Void(System.String)
    public static void Method117()
    {
        try { Utf8JsonWriter.WriteStringValue("hello"); }
        catch { _exitCode = 1; }
    }

    // [118] System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteStringValue:System.Void(System.ReadOnlySpan{System.Char})
    public static void Method118()
    {
        try { Utf8JsonWriter.WriteStringValue(null!); }
        catch { _exitCode = 1; }
    }

    // [119] System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteStringValue:System.Void(System.ReadOnlySpan{System.Byte})
    public static void Method119()
    {
        try { Utf8JsonWriter.WriteStringValue(null!); }
        catch { _exitCode = 1; }
    }

    // [120] System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteStringValueSegment:System.Void(System.ReadOnlySpan{System.Char},System.Boolean)
    public static void Method120()
    {
        try { Utf8JsonWriter.WriteStringValueSegment(null!, true); }
        catch { _exitCode = 1; }
    }

    // [121] System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteStringValueSegment:System.Void(System.ReadOnlySpan{System.Byte},System.Boolean)
    public static void Method121()
    {
        try { Utf8JsonWriter.WriteStringValueSegment(null!, true); }
        catch { _exitCode = 1; }
    }

    // [122] System.Text.Json/System.Text.Json.Utf8JsonWriter::.ctor:System.Void(System.Buffers.IBufferWriter{System.Byte},System.Text.Json.JsonWriterOptions)
    public static void Method122()
    {
        try { new Utf8JsonWriter(null!, null!); }
        catch { _exitCode = 1; }
    }

    // [123] System.Text.Json/System.Text.Json.Utf8JsonWriter::.ctor:System.Void(System.IO.Stream,System.Text.Json.JsonWriterOptions)
    public static void Method123()
    {
        try { new Utf8JsonWriter(null!, null!); }
        catch { _exitCode = 1; }
    }

    // [124] System.Text.Json/System.Text.Json.Utf8JsonWriter::set_BytesPending:System.Void(System.Int32)
    public static void Method124()
    {
        try { Utf8JsonWriter.BytesPending = 42; }
        catch { _exitCode = 1; }
    }

    // [125] System.Text.Json/System.Text.Json.Utf8JsonWriter::set_BytesCommitted:System.Void(System.Int64)
    public static void Method125()
    {
        try { Utf8JsonWriter.BytesCommitted = 42L; }
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
                case 11: Method11(); break;
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
                case 75: Method75(); break;
                case 76: Method76(); break;
                case 77: Method77(); break;
                case 78: Method78(); break;
                case 79: Method79(); break;
                case 80: Method80(); break;
                case 81: Method81(); break;
                case 82: Method82(); break;
                case 83: Method83(); break;
                case 84: Method84(); break;
                case 85: Method85(); break;
                case 86: Method86(); break;
                case 87: Method87(); break;
                case 88: Method88(); break;
                case 89: Method89(); break;
                case 90: Method90(); break;
                case 91: Method91(); break;
                case 92: Method92(); break;
                case 93: Method93(); break;
                case 94: Method94(); break;
                case 95: Method95(); break;
                case 96: Method96(); break;
                case 97: Method97(); break;
                case 98: Method98(); break;
                case 99: Method99(); break;
                case 100: Method100(); break;
                case 101: Method101(); break;
                case 102: Method102(); break;
                case 103: Method103(); break;
                case 104: Method104(); break;
                case 105: Method105(); break;
                case 106: Method106(); break;
                case 107: Method107(); break;
                case 108: Method108(); break;
                case 109: Method109(); break;
                case 110: Method110(); break;
                case 111: Method111(); break;
                case 112: Method112(); break;
                case 113: Method113(); break;
                case 114: Method114(); break;
                case 115: Method115(); break;
                case 116: Method116(); break;
                case 117: Method117(); break;
                case 118: Method118(); break;
                case 119: Method119(); break;
                case 120: Method120(); break;
                case 121: Method121(); break;
                case 122: Method122(); break;
                case 123: Method123(); break;
                case 124: Method124(); break;
                case 125: Method125(); break;
            }
        }
        catch
        {
            _exitCode = 1;
        }
    }

}