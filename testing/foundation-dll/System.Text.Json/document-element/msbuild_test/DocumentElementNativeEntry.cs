// Auto-generated native-AOT entry point
// Family: family/System.Text.Json/document-element
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

public static partial class DocumentElementNativeEntry
{
    // Inlined exit code — avoids SDK method call resolution in codegen
    public static int _exitCode;

    // [0] System.Text.Json/System.Text.Json.JsonDocument::Dispose:System.Void()
    public static void Method0()
    {
        try { JsonDocument.Dispose(); }
        catch { _exitCode = 1; }
    }

    // [1] System.Text.Json/System.Text.Json.JsonDocument::get_RootElement:System.Text.Json.JsonElement()
    public static void Method1()
    {
        try { if ((int)(JsonDocument.RootElement) != (int)(JsonDocument.RootElement)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [2] System.Text.Json/System.Text.Json.JsonDocument::Parse:System.Text.Json.JsonDocument(System.ReadOnlyMemory{System.Byte},System.Text.Json.JsonDocumentOptions)
    public static void Method2()
    {
        try { if ((int)(JsonDocument.Parse(null!, null!)) != (int)(JsonDocument.Parse(null!, null!))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [3] System.Text.Json/System.Text.Json.JsonDocument::Parse:System.Text.Json.JsonDocument(System.Buffers.ReadOnlySequence{System.Byte},System.Text.Json.JsonDocumentOptions)
    public static void Method3()
    {
        try { if ((int)(JsonDocument.Parse(null!, null!)) != (int)(JsonDocument.Parse(null!, null!))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [4] System.Text.Json/System.Text.Json.JsonDocument::Parse:System.Text.Json.JsonDocument(System.IO.Stream,System.Text.Json.JsonDocumentOptions)
    public static void Method4()
    {
        try { if ((int)(JsonDocument.Parse(null!, null!)) != (int)(JsonDocument.Parse(null!, null!))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [5] System.Text.Json/System.Text.Json.JsonDocument::Parse:System.Text.Json.JsonDocument(System.ReadOnlyMemory{System.Char},System.Text.Json.JsonDocumentOptions)
    public static void Method5()
    {
        try { if ((int)(JsonDocument.Parse(null!, null!)) != (int)(JsonDocument.Parse(null!, null!))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [6] System.Text.Json/System.Text.Json.JsonDocument::Parse:System.Text.Json.JsonDocument(System.String,System.Text.Json.JsonDocumentOptions)
    public static void Method6()
    {
        try { if ((int)(JsonDocument.Parse("hello", null!)) != (int)(JsonDocument.Parse("hello", null!))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [7] System.Text.Json/System.Text.Json.JsonDocument::ParseAsync:System.Threading.Tasks.Task{System.Text.Json.JsonDocument}(System.IO.Stream,System.Text.Json.JsonDocumentOptions,System.Threading.CancellationToken)
    public static void Method7()
    {
        try { if (((JsonDocument.ParseAsync(null!, null!, null!)).GetHashCode()) != ((JsonDocument.ParseAsync(null!, null!, null!)).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [8] System.Text.Json/System.Text.Json.JsonDocument::ParseValue:System.Text.Json.JsonDocument(System.Text.Json.Utf8JsonReader&)
    public static void Method8()
    {
    var refLocal_0 = null!;
        try { if ((int)(JsonDocument.ParseValue(ref refLocal_0)) != (int)(JsonDocument.ParseValue(ref refLocal_0))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [9] System.Text.Json/System.Text.Json.JsonDocument::TryParseValue:System.Boolean(System.Text.Json.Utf8JsonReader&,System.Text.Json.JsonDocument&)
    public static void Method9()
    {
    var refLocal_0 = null!;
    var refLocal_1 = null!;
        try { if (((JsonDocument.TryParseValue(ref refLocal_0, ref refLocal_1)) ? 1 : 0) != ((JsonDocument.TryParseValue(ref refLocal_0, ref refLocal_1)) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [10] System.Text.Json/System.Text.Json.JsonDocument::WriteTo:System.Void(System.Text.Json.Utf8JsonWriter)
    public static void Method10()
    {
        try { JsonDocument.WriteTo(null!); }
        catch { _exitCode = 1; }
    }

    // [11] System.Text.Json/System.Text.Json.JsonElement::Clone:System.Text.Json.JsonElement()
    public static void Method11()
    {
        try { if ((int)(JsonElement.Clone()) != (int)(JsonElement.Clone())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [12] System.Text.Json/System.Text.Json.JsonElement::DeepEquals:System.Boolean(System.Text.Json.JsonElement,System.Text.Json.JsonElement)
    public static void Method12()
    {
        try { if (((JsonElement.DeepEquals(null!, null!)) ? 1 : 0) != ((JsonElement.DeepEquals(null!, null!)) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [13] System.Text.Json/System.Text.Json.JsonElement::EnumerateArray:System.Text.Json.JsonElement+ArrayEnumerator()
    public static void Method13()
    {
        try { if ((int)(JsonElement.EnumerateArray()) != (int)(JsonElement.EnumerateArray())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [14] System.Text.Json/System.Text.Json.JsonElement::EnumerateObject:System.Text.Json.JsonElement+ObjectEnumerator()
    public static void Method14()
    {
        try { if ((int)(JsonElement.EnumerateObject()) != (int)(JsonElement.EnumerateObject())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [15] System.Text.Json/System.Text.Json.JsonElement::get_Item:System.Text.Json.JsonElement(System.Int32)
    public static void Method15()
    {
        try { if ((int)(JsonElement[42]) != (int)(JsonElement[42])) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [16] System.Text.Json/System.Text.Json.JsonElement::get_ValueKind:System.Text.Json.JsonValueKind()
    public static void Method16()
    {
        try { if ((int)(JsonElement.ValueKind) != (int)(JsonElement.ValueKind)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [17] System.Text.Json/System.Text.Json.JsonElement::GetArrayLength:System.Int32()
    public static void Method17()
    {
        try { if (JsonElement.GetArrayLength() != JsonElement.GetArrayLength()) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [18] System.Text.Json/System.Text.Json.JsonElement::GetBoolean:System.Boolean()
    public static void Method18()
    {
        try { if (((JsonElement.GetBoolean()) ? 1 : 0) != ((JsonElement.GetBoolean()) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [19] System.Text.Json/System.Text.Json.JsonElement::GetByte:System.Byte()
    public static void Method19()
    {
        try { if ((int)(JsonElement.GetByte()) != (int)(JsonElement.GetByte())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [20] System.Text.Json/System.Text.Json.JsonElement::GetBytesFromBase64:System.Byte{}()
    public static void Method20()
    {
        try { if ((int)(JsonElement.GetBytesFromBase64()) != (int)(JsonElement.GetBytesFromBase64())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [21] System.Text.Json/System.Text.Json.JsonElement::GetDateTime:System.DateTime()
    public static void Method21()
    {
        try { if (((JsonElement.GetDateTime()).GetHashCode()) != ((JsonElement.GetDateTime()).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [22] System.Text.Json/System.Text.Json.JsonElement::GetDateTimeOffset:System.DateTimeOffset()
    public static void Method22()
    {
        try { if ((int)(JsonElement.GetDateTimeOffset()) != (int)(JsonElement.GetDateTimeOffset())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [23] System.Text.Json/System.Text.Json.JsonElement::GetDecimal:System.Decimal()
    public static void Method23()
    {
        try { if ((int)(JsonElement.GetDecimal()) != (int)(JsonElement.GetDecimal())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [24] System.Text.Json/System.Text.Json.JsonElement::GetDouble:System.Double()
    public static void Method24()
    {
        try { if ((int)(JsonElement.GetDouble()) != (int)(JsonElement.GetDouble())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [25] System.Text.Json/System.Text.Json.JsonElement::GetGuid:System.Guid()
    public static void Method25()
    {
        try { if (((JsonElement.GetGuid()).GetHashCode()) != ((JsonElement.GetGuid()).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [26] System.Text.Json/System.Text.Json.JsonElement::GetInt16:System.Int16()
    public static void Method26()
    {
        try { if ((int)(JsonElement.GetInt16()) != (int)(JsonElement.GetInt16())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [27] System.Text.Json/System.Text.Json.JsonElement::GetInt32:System.Int32()
    public static void Method27()
    {
        try { if (JsonElement.GetInt32() != JsonElement.GetInt32()) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [28] System.Text.Json/System.Text.Json.JsonElement::GetInt64:System.Int64()
    public static void Method28()
    {
        try { if ((int)(JsonElement.GetInt64()) != (int)(JsonElement.GetInt64())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [29] System.Text.Json/System.Text.Json.JsonElement::GetProperty:System.Text.Json.JsonElement(System.String)
    public static void Method29()
    {
        try { if ((int)(JsonElement.GetProperty("hello")) != (int)(JsonElement.GetProperty("hello"))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [30] System.Text.Json/System.Text.Json.JsonElement::GetProperty:System.Text.Json.JsonElement(System.ReadOnlySpan{System.Char})
    public static void Method30()
    {
        try { if ((int)(JsonElement.GetProperty(null!)) != (int)(JsonElement.GetProperty(null!))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [31] System.Text.Json/System.Text.Json.JsonElement::GetProperty:System.Text.Json.JsonElement(System.ReadOnlySpan{System.Byte})
    public static void Method31()
    {
        try { if ((int)(JsonElement.GetProperty(null!)) != (int)(JsonElement.GetProperty(null!))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [32] System.Text.Json/System.Text.Json.JsonElement::GetPropertyCount:System.Int32()
    public static void Method32()
    {
        try { if (JsonElement.GetPropertyCount() != JsonElement.GetPropertyCount()) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [33] System.Text.Json/System.Text.Json.JsonElement::GetRawText:System.String()
    public static void Method33()
    {
        try { if (((JsonElement.GetRawText()).Length) != ((JsonElement.GetRawText()).Length)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [34] System.Text.Json/System.Text.Json.JsonElement::GetSByte:System.SByte()
    public static void Method34()
    {
        try { if ((int)(JsonElement.GetSByte()) != (int)(JsonElement.GetSByte())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [35] System.Text.Json/System.Text.Json.JsonElement::GetSingle:System.Single()
    public static void Method35()
    {
        try { if ((int)(JsonElement.GetSingle()) != (int)(JsonElement.GetSingle())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [36] System.Text.Json/System.Text.Json.JsonElement::GetString:System.String()
    public static void Method36()
    {
        try { if (((JsonElement.GetString()).Length) != ((JsonElement.GetString()).Length)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [37] System.Text.Json/System.Text.Json.JsonElement::GetUInt16:System.UInt16()
    public static void Method37()
    {
        try { if ((int)(JsonElement.GetUInt16()) != (int)(JsonElement.GetUInt16())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [38] System.Text.Json/System.Text.Json.JsonElement::GetUInt32:System.UInt32()
    public static void Method38()
    {
        try { if ((int)(JsonElement.GetUInt32()) != (int)(JsonElement.GetUInt32())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [39] System.Text.Json/System.Text.Json.JsonElement::GetUInt64:System.UInt64()
    public static void Method39()
    {
        try { if ((int)(JsonElement.GetUInt64()) != (int)(JsonElement.GetUInt64())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [40] System.Text.Json/System.Text.Json.JsonElement::Parse:System.Text.Json.JsonElement(System.ReadOnlySpan{System.Byte},System.Text.Json.JsonDocumentOptions)
    public static void Method40()
    {
        try { if ((int)(JsonElement.Parse(null!, null!)) != (int)(JsonElement.Parse(null!, null!))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [41] System.Text.Json/System.Text.Json.JsonElement::Parse:System.Text.Json.JsonElement(System.ReadOnlySpan{System.Char},System.Text.Json.JsonDocumentOptions)
    public static void Method41()
    {
        try { if ((int)(JsonElement.Parse(null!, null!)) != (int)(JsonElement.Parse(null!, null!))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [42] System.Text.Json/System.Text.Json.JsonElement::Parse:System.Text.Json.JsonElement(System.String,System.Text.Json.JsonDocumentOptions)
    public static void Method42()
    {
        try { if ((int)(JsonElement.Parse("hello", null!)) != (int)(JsonElement.Parse("hello", null!))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [43] System.Text.Json/System.Text.Json.JsonElement::ParseValue:System.Text.Json.JsonElement(System.Text.Json.Utf8JsonReader&)
    public static void Method43()
    {
    var refLocal_0 = null!;
        try { if ((int)(JsonElement.ParseValue(ref refLocal_0)) != (int)(JsonElement.ParseValue(ref refLocal_0))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [44] System.Text.Json/System.Text.Json.JsonElement::ToString:System.String()
    public static void Method44()
    {
        try { if (((JsonElement.ToString()).Length) != ((JsonElement.ToString()).Length)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [45] System.Text.Json/System.Text.Json.JsonElement::TryGetByte:System.Boolean(System.Byte&)
    public static void Method45()
    {
    var refLocal_0 = (byte)42;
        try { if (((JsonElement.TryGetByte(ref refLocal_0)) ? 1 : 0) != ((JsonElement.TryGetByte(ref refLocal_0)) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [46] System.Text.Json/System.Text.Json.JsonElement::TryGetBytesFromBase64:System.Boolean(System.Byte{}&)
    public static void Method46()
    {
    var refLocal_0 = null!;
        try { if (((JsonElement.TryGetBytesFromBase64(ref refLocal_0)) ? 1 : 0) != ((JsonElement.TryGetBytesFromBase64(ref refLocal_0)) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [47] System.Text.Json/System.Text.Json.JsonElement::TryGetDateTime:System.Boolean(System.DateTime&)
    public static void Method47()
    {
    var refLocal_0 = DateTime.UtcNow;
        try { if (((JsonElement.TryGetDateTime(ref refLocal_0)) ? 1 : 0) != ((JsonElement.TryGetDateTime(ref refLocal_0)) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [48] System.Text.Json/System.Text.Json.JsonElement::TryGetDateTimeOffset:System.Boolean(System.DateTimeOffset&)
    public static void Method48()
    {
    var refLocal_0 = null!;
        try { if (((JsonElement.TryGetDateTimeOffset(ref refLocal_0)) ? 1 : 0) != ((JsonElement.TryGetDateTimeOffset(ref refLocal_0)) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [49] System.Text.Json/System.Text.Json.JsonElement::TryGetDecimal:System.Boolean(System.Decimal&)
    public static void Method49()
    {
    var refLocal_0 = 42m;
        try { if (((JsonElement.TryGetDecimal(ref refLocal_0)) ? 1 : 0) != ((JsonElement.TryGetDecimal(ref refLocal_0)) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [50] System.Text.Json/System.Text.Json.JsonElement::TryGetDouble:System.Boolean(System.Double&)
    public static void Method50()
    {
    var refLocal_0 = 42.0;
        try { if (((JsonElement.TryGetDouble(ref refLocal_0)) ? 1 : 0) != ((JsonElement.TryGetDouble(ref refLocal_0)) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [51] System.Text.Json/System.Text.Json.JsonElement::TryGetGuid:System.Boolean(System.Guid&)
    public static void Method51()
    {
    var refLocal_0 = Guid.Empty;
        try { if (((JsonElement.TryGetGuid(ref refLocal_0)) ? 1 : 0) != ((JsonElement.TryGetGuid(ref refLocal_0)) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [52] System.Text.Json/System.Text.Json.JsonElement::TryGetInt16:System.Boolean(System.Int16&)
    public static void Method52()
    {
    var refLocal_0 = (short)42;
        try { if (((JsonElement.TryGetInt16(ref refLocal_0)) ? 1 : 0) != ((JsonElement.TryGetInt16(ref refLocal_0)) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [53] System.Text.Json/System.Text.Json.JsonElement::TryGetInt32:System.Boolean(System.Int32&)
    public static void Method53()
    {
    var refLocal_0 = 42;
        try { if (((JsonElement.TryGetInt32(ref refLocal_0)) ? 1 : 0) != ((JsonElement.TryGetInt32(ref refLocal_0)) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [54] System.Text.Json/System.Text.Json.JsonElement::TryGetInt64:System.Boolean(System.Int64&)
    public static void Method54()
    {
    var refLocal_0 = 42L;
        try { if (((JsonElement.TryGetInt64(ref refLocal_0)) ? 1 : 0) != ((JsonElement.TryGetInt64(ref refLocal_0)) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [55] System.Text.Json/System.Text.Json.JsonElement::TryGetProperty:System.Boolean(System.String,System.Text.Json.JsonElement&)
    public static void Method55()
    {
    var refLocal_1 = null!;
        try { if (((JsonElement.TryGetProperty("hello", ref refLocal_1)) ? 1 : 0) != ((JsonElement.TryGetProperty("hello", ref refLocal_1)) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [56] System.Text.Json/System.Text.Json.JsonElement::TryGetProperty:System.Boolean(System.ReadOnlySpan{System.Char},System.Text.Json.JsonElement&)
    public static void Method56()
    {
    var refLocal_1 = null!;
        try { if (((JsonElement.TryGetProperty(null!, ref refLocal_1)) ? 1 : 0) != ((JsonElement.TryGetProperty(null!, ref refLocal_1)) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [57] System.Text.Json/System.Text.Json.JsonElement::TryGetProperty:System.Boolean(System.ReadOnlySpan{System.Byte},System.Text.Json.JsonElement&)
    public static void Method57()
    {
    var refLocal_1 = null!;
        try { if (((JsonElement.TryGetProperty(null!, ref refLocal_1)) ? 1 : 0) != ((JsonElement.TryGetProperty(null!, ref refLocal_1)) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [58] System.Text.Json/System.Text.Json.JsonElement::TryGetSByte:System.Boolean(System.SByte&)
    public static void Method58()
    {
    var refLocal_0 = (sbyte)42;
        try { if (((JsonElement.TryGetSByte(ref refLocal_0)) ? 1 : 0) != ((JsonElement.TryGetSByte(ref refLocal_0)) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [59] System.Text.Json/System.Text.Json.JsonElement::TryGetSingle:System.Boolean(System.Single&)
    public static void Method59()
    {
    var refLocal_0 = 42.0f;
        try { if (((JsonElement.TryGetSingle(ref refLocal_0)) ? 1 : 0) != ((JsonElement.TryGetSingle(ref refLocal_0)) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [60] System.Text.Json/System.Text.Json.JsonElement::TryGetUInt16:System.Boolean(System.UInt16&)
    public static void Method60()
    {
    var refLocal_0 = (ushort)42;
        try { if (((JsonElement.TryGetUInt16(ref refLocal_0)) ? 1 : 0) != ((JsonElement.TryGetUInt16(ref refLocal_0)) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [61] System.Text.Json/System.Text.Json.JsonElement::TryGetUInt32:System.Boolean(System.UInt32&)
    public static void Method61()
    {
    var refLocal_0 = 42u;
        try { if (((JsonElement.TryGetUInt32(ref refLocal_0)) ? 1 : 0) != ((JsonElement.TryGetUInt32(ref refLocal_0)) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [62] System.Text.Json/System.Text.Json.JsonElement::TryGetUInt64:System.Boolean(System.UInt64&)
    public static void Method62()
    {
    var refLocal_0 = 42uL;
        try { if (((JsonElement.TryGetUInt64(ref refLocal_0)) ? 1 : 0) != ((JsonElement.TryGetUInt64(ref refLocal_0)) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [63] System.Text.Json/System.Text.Json.JsonElement::TryParseValue:System.Boolean(System.Text.Json.Utf8JsonReader&,System.Nullable{System.Text.Json.JsonElement}&)
    public static void Method63()
    {
    var refLocal_0 = null!;
    var refLocal_1 = null!;
        try { if (((JsonElement.TryParseValue(ref refLocal_0, ref refLocal_1)) ? 1 : 0) != ((JsonElement.TryParseValue(ref refLocal_0, ref refLocal_1)) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [64] System.Text.Json/System.Text.Json.JsonElement::ValueEquals:System.Boolean(System.String)
    public static void Method64()
    {
        try { if (((JsonElement.ValueEquals("hello")) ? 1 : 0) != ((JsonElement.ValueEquals("hello")) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [65] System.Text.Json/System.Text.Json.JsonElement::ValueEquals:System.Boolean(System.ReadOnlySpan{System.Byte})
    public static void Method65()
    {
        try { if (((JsonElement.ValueEquals(null!)) ? 1 : 0) != ((JsonElement.ValueEquals(null!)) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [66] System.Text.Json/System.Text.Json.JsonElement::ValueEquals:System.Boolean(System.ReadOnlySpan{System.Char})
    public static void Method66()
    {
        try { if (((JsonElement.ValueEquals(null!)) ? 1 : 0) != ((JsonElement.ValueEquals(null!)) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [67] System.Text.Json/System.Text.Json.JsonElement::WriteTo:System.Void(System.Text.Json.Utf8JsonWriter)
    public static void Method67()
    {
        try { JsonElement.WriteTo(null!); }
        catch { _exitCode = 1; }
    }

    // [68] System.Text.Json/System.Text.Json.JsonElement::get_Item:System.Text.Json.JsonElement()
    public static void Method68()
    {
        try { if ((int)(JsonElement[]) != (int)(JsonElement[])) _exitCode = 1; }
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
            }
        }
        catch
        {
            _exitCode = 1;
        }
    }

}