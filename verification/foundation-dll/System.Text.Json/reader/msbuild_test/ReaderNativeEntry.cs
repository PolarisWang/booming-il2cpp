// Auto-generated native-AOT entry point
// Family: family/System.Text.Json/reader
// Assembly: System.Text.Json
// Variant: benchmark

using System;
using System.Buffers;
using System.Collections.Generic;
using System.Linq;
using System.Text.Json;

public static partial class ReaderNativeEntry
{
    // Inlined exit code — avoids SDK method call resolution in codegen
    public static int _exitCode;

    // [0] System.Text.Json/System.Text.Json.Utf8JsonReader::CopyString:System.Int32(System.Span{System.Byte})
    public static void Method0()
    {
        try { if (Utf8JsonReader.CopyString(null!) != Utf8JsonReader.CopyString(null!)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [1] System.Text.Json/System.Text.Json.Utf8JsonReader::CopyString:System.Int32(System.Span{System.Char})
    public static void Method1()
    {
        try { if (Utf8JsonReader.CopyString(null!) != Utf8JsonReader.CopyString(null!)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [2] System.Text.Json/System.Text.Json.Utf8JsonReader::get_BytesConsumed:System.Int64()
    public static void Method2()
    {
        try { if ((int)(Utf8JsonReader.BytesConsumed) != (int)(Utf8JsonReader.BytesConsumed)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [3] System.Text.Json/System.Text.Json.Utf8JsonReader::get_CurrentDepth:System.Int32()
    public static void Method3()
    {
        try { if (Utf8JsonReader.CurrentDepth != Utf8JsonReader.CurrentDepth) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [4] System.Text.Json/System.Text.Json.Utf8JsonReader::get_CurrentState:System.Text.Json.JsonReaderState()
    public static void Method4()
    {
        try { if ((int)(Utf8JsonReader.CurrentState) != (int)(Utf8JsonReader.CurrentState)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [5] System.Text.Json/System.Text.Json.Utf8JsonReader::get_HasValueSequence:System.Boolean()
    public static void Method5()
    {
        try { if (((Utf8JsonReader.HasValueSequence) ? 1 : 0) != ((Utf8JsonReader.HasValueSequence) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [6] System.Text.Json/System.Text.Json.Utf8JsonReader::get_IsFinalBlock:System.Boolean()
    public static void Method6()
    {
        try { if (((Utf8JsonReader.IsFinalBlock) ? 1 : 0) != ((Utf8JsonReader.IsFinalBlock) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [7] System.Text.Json/System.Text.Json.Utf8JsonReader::get_Position:System.SequencePosition()
    public static void Method7()
    {
        try { if ((int)(Utf8JsonReader.Position) != (int)(Utf8JsonReader.Position)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [8] System.Text.Json/System.Text.Json.Utf8JsonReader::get_TokenStartIndex:System.Int64()
    public static void Method8()
    {
        try { if ((int)(Utf8JsonReader.TokenStartIndex) != (int)(Utf8JsonReader.TokenStartIndex)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [9] System.Text.Json/System.Text.Json.Utf8JsonReader::get_TokenType:System.Text.Json.JsonTokenType()
    public static void Method9()
    {
        try { if ((int)(Utf8JsonReader.TokenType) != (int)(Utf8JsonReader.TokenType)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [10] System.Text.Json/System.Text.Json.Utf8JsonReader::get_ValueIsEscaped:System.Boolean()
    public static void Method10()
    {
        try { if (((Utf8JsonReader.ValueIsEscaped) ? 1 : 0) != ((Utf8JsonReader.ValueIsEscaped) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [11] System.Text.Json/System.Text.Json.Utf8JsonReader::get_ValueSequence:System.Buffers.ReadOnlySequence{System.Byte}()
    public static void Method11()
    {
        try { if ((int)(Utf8JsonReader.ValueSequence) != (int)(Utf8JsonReader.ValueSequence)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [12] System.Text.Json/System.Text.Json.Utf8JsonReader::get_ValueSpan:System.ReadOnlySpan{System.Byte}()
    public static void Method12()
    {
        try { if ((int)(Utf8JsonReader.ValueSpan) != (int)(Utf8JsonReader.ValueSpan)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [13] System.Text.Json/System.Text.Json.Utf8JsonReader::GetBoolean:System.Boolean()
    public static void Method13()
    {
        try { if (((Utf8JsonReader.GetBoolean()) ? 1 : 0) != ((Utf8JsonReader.GetBoolean()) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [14] System.Text.Json/System.Text.Json.Utf8JsonReader::GetByte:System.Byte()
    public static void Method14()
    {
        try { if ((int)(Utf8JsonReader.GetByte()) != (int)(Utf8JsonReader.GetByte())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [15] System.Text.Json/System.Text.Json.Utf8JsonReader::GetBytesFromBase64:System.Byte{}()
    public static void Method15()
    {
        try { if ((int)(Utf8JsonReader.GetBytesFromBase64()) != (int)(Utf8JsonReader.GetBytesFromBase64())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [16] System.Text.Json/System.Text.Json.Utf8JsonReader::GetComment:System.String()
    public static void Method16()
    {
        try { if (((Utf8JsonReader.GetComment()).Length) != ((Utf8JsonReader.GetComment()).Length)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [17] System.Text.Json/System.Text.Json.Utf8JsonReader::GetDateTime:System.DateTime()
    public static void Method17()
    {
        try { if (((Utf8JsonReader.GetDateTime()).GetHashCode()) != ((Utf8JsonReader.GetDateTime()).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [18] System.Text.Json/System.Text.Json.Utf8JsonReader::GetDateTimeOffset:System.DateTimeOffset()
    public static void Method18()
    {
        try { if ((int)(Utf8JsonReader.GetDateTimeOffset()) != (int)(Utf8JsonReader.GetDateTimeOffset())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [19] System.Text.Json/System.Text.Json.Utf8JsonReader::GetDecimal:System.Decimal()
    public static void Method19()
    {
        try { if ((int)(Utf8JsonReader.GetDecimal()) != (int)(Utf8JsonReader.GetDecimal())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [20] System.Text.Json/System.Text.Json.Utf8JsonReader::GetDouble:System.Double()
    public static void Method20()
    {
        try { if ((int)(Utf8JsonReader.GetDouble()) != (int)(Utf8JsonReader.GetDouble())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [21] System.Text.Json/System.Text.Json.Utf8JsonReader::GetGuid:System.Guid()
    public static void Method21()
    {
        try { if (((Utf8JsonReader.GetGuid()).GetHashCode()) != ((Utf8JsonReader.GetGuid()).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [22] System.Text.Json/System.Text.Json.Utf8JsonReader::GetInt16:System.Int16()
    public static void Method22()
    {
        try { if ((int)(Utf8JsonReader.GetInt16()) != (int)(Utf8JsonReader.GetInt16())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [23] System.Text.Json/System.Text.Json.Utf8JsonReader::GetInt32:System.Int32()
    public static void Method23()
    {
        try { if (Utf8JsonReader.GetInt32() != Utf8JsonReader.GetInt32()) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [24] System.Text.Json/System.Text.Json.Utf8JsonReader::GetInt64:System.Int64()
    public static void Method24()
    {
        try { if ((int)(Utf8JsonReader.GetInt64()) != (int)(Utf8JsonReader.GetInt64())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [25] System.Text.Json/System.Text.Json.Utf8JsonReader::GetSByte:System.SByte()
    public static void Method25()
    {
        try { if ((int)(Utf8JsonReader.GetSByte()) != (int)(Utf8JsonReader.GetSByte())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [26] System.Text.Json/System.Text.Json.Utf8JsonReader::GetSingle:System.Single()
    public static void Method26()
    {
        try { if ((int)(Utf8JsonReader.GetSingle()) != (int)(Utf8JsonReader.GetSingle())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [27] System.Text.Json/System.Text.Json.Utf8JsonReader::GetString:System.String()
    public static void Method27()
    {
        try { if (((Utf8JsonReader.GetString()).Length) != ((Utf8JsonReader.GetString()).Length)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [28] System.Text.Json/System.Text.Json.Utf8JsonReader::GetUInt16:System.UInt16()
    public static void Method28()
    {
        try { if ((int)(Utf8JsonReader.GetUInt16()) != (int)(Utf8JsonReader.GetUInt16())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [29] System.Text.Json/System.Text.Json.Utf8JsonReader::GetUInt32:System.UInt32()
    public static void Method29()
    {
        try { if ((int)(Utf8JsonReader.GetUInt32()) != (int)(Utf8JsonReader.GetUInt32())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [30] System.Text.Json/System.Text.Json.Utf8JsonReader::GetUInt64:System.UInt64()
    public static void Method30()
    {
        try { if ((int)(Utf8JsonReader.GetUInt64()) != (int)(Utf8JsonReader.GetUInt64())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [31] System.Text.Json/System.Text.Json.Utf8JsonReader::Read:System.Boolean()
    public static void Method31()
    {
        try { if (((Utf8JsonReader.Read()) ? 1 : 0) != ((Utf8JsonReader.Read()) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [32] System.Text.Json/System.Text.Json.Utf8JsonReader::Skip:System.Void()
    public static void Method32()
    {
        try { Utf8JsonReader.Skip(); }
        catch { _exitCode = 1; }
    }

    // [33] System.Text.Json/System.Text.Json.Utf8JsonReader::TryGetByte:System.Boolean(System.Byte&)
    public static void Method33()
    {
    var refLocal_0 = (byte)42;
        try { if (((Utf8JsonReader.TryGetByte(ref refLocal_0)) ? 1 : 0) != ((Utf8JsonReader.TryGetByte(ref refLocal_0)) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [34] System.Text.Json/System.Text.Json.Utf8JsonReader::TryGetBytesFromBase64:System.Boolean(System.Byte{}&)
    public static void Method34()
    {
    var refLocal_0 = null!;
        try { if (((Utf8JsonReader.TryGetBytesFromBase64(ref refLocal_0)) ? 1 : 0) != ((Utf8JsonReader.TryGetBytesFromBase64(ref refLocal_0)) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [35] System.Text.Json/System.Text.Json.Utf8JsonReader::TryGetDateTime:System.Boolean(System.DateTime&)
    public static void Method35()
    {
    var refLocal_0 = DateTime.UtcNow;
        try { if (((Utf8JsonReader.TryGetDateTime(ref refLocal_0)) ? 1 : 0) != ((Utf8JsonReader.TryGetDateTime(ref refLocal_0)) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [36] System.Text.Json/System.Text.Json.Utf8JsonReader::TryGetDateTimeOffset:System.Boolean(System.DateTimeOffset&)
    public static void Method36()
    {
    var refLocal_0 = null!;
        try { if (((Utf8JsonReader.TryGetDateTimeOffset(ref refLocal_0)) ? 1 : 0) != ((Utf8JsonReader.TryGetDateTimeOffset(ref refLocal_0)) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [37] System.Text.Json/System.Text.Json.Utf8JsonReader::TryGetDecimal:System.Boolean(System.Decimal&)
    public static void Method37()
    {
    var refLocal_0 = 42m;
        try { if (((Utf8JsonReader.TryGetDecimal(ref refLocal_0)) ? 1 : 0) != ((Utf8JsonReader.TryGetDecimal(ref refLocal_0)) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [38] System.Text.Json/System.Text.Json.Utf8JsonReader::TryGetDouble:System.Boolean(System.Double&)
    public static void Method38()
    {
    var refLocal_0 = 42.0;
        try { if (((Utf8JsonReader.TryGetDouble(ref refLocal_0)) ? 1 : 0) != ((Utf8JsonReader.TryGetDouble(ref refLocal_0)) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [39] System.Text.Json/System.Text.Json.Utf8JsonReader::TryGetGuid:System.Boolean(System.Guid&)
    public static void Method39()
    {
    var refLocal_0 = Guid.Empty;
        try { if (((Utf8JsonReader.TryGetGuid(ref refLocal_0)) ? 1 : 0) != ((Utf8JsonReader.TryGetGuid(ref refLocal_0)) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [40] System.Text.Json/System.Text.Json.Utf8JsonReader::TryGetInt16:System.Boolean(System.Int16&)
    public static void Method40()
    {
    var refLocal_0 = (short)42;
        try { if (((Utf8JsonReader.TryGetInt16(ref refLocal_0)) ? 1 : 0) != ((Utf8JsonReader.TryGetInt16(ref refLocal_0)) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [41] System.Text.Json/System.Text.Json.Utf8JsonReader::TryGetInt32:System.Boolean(System.Int32&)
    public static void Method41()
    {
    var refLocal_0 = 42;
        try { if (((Utf8JsonReader.TryGetInt32(ref refLocal_0)) ? 1 : 0) != ((Utf8JsonReader.TryGetInt32(ref refLocal_0)) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [42] System.Text.Json/System.Text.Json.Utf8JsonReader::TryGetInt64:System.Boolean(System.Int64&)
    public static void Method42()
    {
    var refLocal_0 = 42L;
        try { if (((Utf8JsonReader.TryGetInt64(ref refLocal_0)) ? 1 : 0) != ((Utf8JsonReader.TryGetInt64(ref refLocal_0)) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [43] System.Text.Json/System.Text.Json.Utf8JsonReader::TryGetSByte:System.Boolean(System.SByte&)
    public static void Method43()
    {
    var refLocal_0 = (sbyte)42;
        try { if (((Utf8JsonReader.TryGetSByte(ref refLocal_0)) ? 1 : 0) != ((Utf8JsonReader.TryGetSByte(ref refLocal_0)) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [44] System.Text.Json/System.Text.Json.Utf8JsonReader::TryGetSingle:System.Boolean(System.Single&)
    public static void Method44()
    {
    var refLocal_0 = 42.0f;
        try { if (((Utf8JsonReader.TryGetSingle(ref refLocal_0)) ? 1 : 0) != ((Utf8JsonReader.TryGetSingle(ref refLocal_0)) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [45] System.Text.Json/System.Text.Json.Utf8JsonReader::TryGetUInt16:System.Boolean(System.UInt16&)
    public static void Method45()
    {
    var refLocal_0 = (ushort)42;
        try { if (((Utf8JsonReader.TryGetUInt16(ref refLocal_0)) ? 1 : 0) != ((Utf8JsonReader.TryGetUInt16(ref refLocal_0)) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [46] System.Text.Json/System.Text.Json.Utf8JsonReader::TryGetUInt32:System.Boolean(System.UInt32&)
    public static void Method46()
    {
    var refLocal_0 = 42u;
        try { if (((Utf8JsonReader.TryGetUInt32(ref refLocal_0)) ? 1 : 0) != ((Utf8JsonReader.TryGetUInt32(ref refLocal_0)) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [47] System.Text.Json/System.Text.Json.Utf8JsonReader::TryGetUInt64:System.Boolean(System.UInt64&)
    public static void Method47()
    {
    var refLocal_0 = 42uL;
        try { if (((Utf8JsonReader.TryGetUInt64(ref refLocal_0)) ? 1 : 0) != ((Utf8JsonReader.TryGetUInt64(ref refLocal_0)) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [48] System.Text.Json/System.Text.Json.Utf8JsonReader::TrySkip:System.Boolean()
    public static void Method48()
    {
        try { if (((Utf8JsonReader.TrySkip()) ? 1 : 0) != ((Utf8JsonReader.TrySkip()) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [49] System.Text.Json/System.Text.Json.Utf8JsonReader::ValueTextEquals:System.Boolean(System.ReadOnlySpan{System.Byte})
    public static void Method49()
    {
        try { if (((Utf8JsonReader.ValueTextEquals(null!)) ? 1 : 0) != ((Utf8JsonReader.ValueTextEquals(null!)) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [50] System.Text.Json/System.Text.Json.Utf8JsonReader::ValueTextEquals:System.Boolean(System.String)
    public static void Method50()
    {
        try { if (((Utf8JsonReader.ValueTextEquals("hello")) ? 1 : 0) != ((Utf8JsonReader.ValueTextEquals("hello")) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [51] System.Text.Json/System.Text.Json.Utf8JsonReader::ValueTextEquals:System.Boolean(System.ReadOnlySpan{System.Char})
    public static void Method51()
    {
        try { if (((Utf8JsonReader.ValueTextEquals(null!)) ? 1 : 0) != ((Utf8JsonReader.ValueTextEquals(null!)) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [52] System.Text.Json/System.Text.Json.Utf8JsonReader::.ctor:System.Void(System.ReadOnlySpan{System.Byte},System.Boolean,System.Text.Json.JsonReaderState)
    public static void Method52()
    {
        try { new Utf8JsonReader(null!, true, null!); }
        catch { _exitCode = 1; }
    }

    // [53] System.Text.Json/System.Text.Json.Utf8JsonReader::.ctor:System.Void(System.ReadOnlySpan{System.Byte},System.Text.Json.JsonReaderOptions)
    public static void Method53()
    {
        try { new Utf8JsonReader(null!, null!); }
        catch { _exitCode = 1; }
    }

    // [54] System.Text.Json/System.Text.Json.Utf8JsonReader::.ctor:System.Void(System.Buffers.ReadOnlySequence{System.Byte},System.Boolean,System.Text.Json.JsonReaderState)
    public static void Method54()
    {
        try { new Utf8JsonReader(null!, true, null!); }
        catch { _exitCode = 1; }
    }

    // [55] System.Text.Json/System.Text.Json.Utf8JsonReader::.ctor:System.Void(System.Buffers.ReadOnlySequence{System.Byte},System.Text.Json.JsonReaderOptions)
    public static void Method55()
    {
        try { new Utf8JsonReader(null!, null!); }
        catch { _exitCode = 1; }
    }

    // [56] System.Text.Json/System.Text.Json.Utf8JsonReader::set_ValueSpan:System.Void(System.ReadOnlySpan{System.Byte})
    public static void Method56()
    {
        try { Utf8JsonReader.ValueSpan = null!; }
        catch { _exitCode = 1; }
    }

    // [57] System.Text.Json/System.Text.Json.Utf8JsonReader::set_TokenStartIndex:System.Void(System.Int64)
    public static void Method57()
    {
        try { Utf8JsonReader.TokenStartIndex = 42L; }
        catch { _exitCode = 1; }
    }

    // [58] System.Text.Json/System.Text.Json.Utf8JsonReader::set_HasValueSequence:System.Void(System.Boolean)
    public static void Method58()
    {
        try { Utf8JsonReader.HasValueSequence = true; }
        catch { _exitCode = 1; }
    }

    // [59] System.Text.Json/System.Text.Json.Utf8JsonReader::set_ValueIsEscaped:System.Void(System.Boolean)
    public static void Method59()
    {
        try { Utf8JsonReader.ValueIsEscaped = true; }
        catch { _exitCode = 1; }
    }

    // [60] System.Text.Json/System.Text.Json.Utf8JsonReader::set_ValueSequence:System.Void(System.Buffers.ReadOnlySequence{System.Byte})
    public static void Method60()
    {
        try { Utf8JsonReader.ValueSequence = null!; }
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
            }
        }
        catch
        {
            _exitCode = 1;
        }
    }

}