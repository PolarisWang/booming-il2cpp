// Auto-generated native-AOT entry point
// Family: family/System.Formats.Asn1/reader
// Assembly: System.Formats.Asn1
// Variant: benchmark

using System;
using System.Collections;
using System.Collections.Generic;
using System.Formats.Asn1;
using System.Linq;
using System.Numerics;

public static partial class ReaderNativeEntry
{
    // Inlined exit code — avoids SDK method call resolution in codegen
    public static int _exitCode;

    // [0] System.Formats.Asn1/System.Formats.Asn1.AsnReader::Clone:System.Formats.Asn1.AsnReader()
    public static void Method0()
    {
        try { if ((int)(AsnReader.Clone()) != (int)(AsnReader.Clone())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [1] System.Formats.Asn1/System.Formats.Asn1.AsnReader::get_HasData:System.Boolean()
    public static void Method1()
    {
        try { if (((AsnReader.HasData) ? 1 : 0) != ((AsnReader.HasData) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [2] System.Formats.Asn1/System.Formats.Asn1.AsnReader::get_RuleSet:System.Formats.Asn1.AsnEncodingRules()
    public static void Method2()
    {
        try { if ((int)(AsnReader.RuleSet) != (int)(AsnReader.RuleSet)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [3] System.Formats.Asn1/System.Formats.Asn1.AsnReader::PeekContentBytes:System.ReadOnlyMemory{System.Byte}()
    public static void Method3()
    {
        try { if ((int)(AsnReader.PeekContentBytes()) != (int)(AsnReader.PeekContentBytes())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [4] System.Formats.Asn1/System.Formats.Asn1.AsnReader::PeekEncodedValue:System.ReadOnlyMemory{System.Byte}()
    public static void Method4()
    {
        try { if ((int)(AsnReader.PeekEncodedValue()) != (int)(AsnReader.PeekEncodedValue())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [5] System.Formats.Asn1/System.Formats.Asn1.AsnReader::PeekTag:System.Formats.Asn1.Asn1Tag()
    public static void Method5()
    {
        try { if ((int)(AsnReader.PeekTag()) != (int)(AsnReader.PeekTag())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [6] System.Formats.Asn1/System.Formats.Asn1.AsnReader::ReadBitString:System.Byte{}(System.Int32&,System.Nullable{System.Formats.Asn1.Asn1Tag})
    public static void Method6()
    {
    var refLocal_0 = 42;
        try { if ((int)(AsnReader.ReadBitString(ref refLocal_0, null!)) != (int)(AsnReader.ReadBitString(ref refLocal_0, null!))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [7] System.Formats.Asn1/System.Formats.Asn1.AsnReader::ReadBoolean:System.Boolean(System.Nullable{System.Formats.Asn1.Asn1Tag})
    public static void Method7()
    {
        try { if (((AsnReader.ReadBoolean(null!)) ? 1 : 0) != ((AsnReader.ReadBoolean(null!)) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [8] System.Formats.Asn1/System.Formats.Asn1.AsnReader::ReadCharacterString:System.String(System.Formats.Asn1.UniversalTagNumber,System.Nullable{System.Formats.Asn1.Asn1Tag})
    public static void Method8()
    {
        try { if (((AsnReader.ReadCharacterString(null!, null!)).Length) != ((AsnReader.ReadCharacterString(null!, null!)).Length)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [9] System.Formats.Asn1/System.Formats.Asn1.AsnReader::ReadEncodedValue:System.ReadOnlyMemory{System.Byte}()
    public static void Method9()
    {
        try { if ((int)(AsnReader.ReadEncodedValue()) != (int)(AsnReader.ReadEncodedValue())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [10] System.Formats.Asn1/System.Formats.Asn1.AsnReader::ReadEnumeratedBytes:System.ReadOnlyMemory{System.Byte}(System.Nullable{System.Formats.Asn1.Asn1Tag})
    public static void Method10()
    {
        try { if ((int)(AsnReader.ReadEnumeratedBytes(null!)) != (int)(AsnReader.ReadEnumeratedBytes(null!))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [11] System.Formats.Asn1/System.Formats.Asn1.AsnReader::ReadEnumeratedValue{TEnum}:TEnum(System.Nullable{System.Formats.Asn1.Asn1Tag})

    // [12] System.Formats.Asn1/System.Formats.Asn1.AsnReader::ReadEnumeratedValue:System.Enum(System.Type,System.Nullable{System.Formats.Asn1.Asn1Tag})
    public static void Method12()
    {
        try { if ((int)(AsnReader.ReadEnumeratedValue(typeof(byte), null!)) != (int)(AsnReader.ReadEnumeratedValue(typeof(byte), null!))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [13] System.Formats.Asn1/System.Formats.Asn1.AsnReader::ReadGeneralizedTime:System.DateTimeOffset(System.Nullable{System.Formats.Asn1.Asn1Tag})
    public static void Method13()
    {
        try { if ((int)(AsnReader.ReadGeneralizedTime(null!)) != (int)(AsnReader.ReadGeneralizedTime(null!))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [14] System.Formats.Asn1/System.Formats.Asn1.AsnReader::ReadInteger:System.Numerics.BigInteger(System.Nullable{System.Formats.Asn1.Asn1Tag})
    public static void Method14()
    {
        try { if ((int)(AsnReader.ReadInteger(null!)) != (int)(AsnReader.ReadInteger(null!))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [15] System.Formats.Asn1/System.Formats.Asn1.AsnReader::ReadIntegerBytes:System.ReadOnlyMemory{System.Byte}(System.Nullable{System.Formats.Asn1.Asn1Tag})
    public static void Method15()
    {
        try { if ((int)(AsnReader.ReadIntegerBytes(null!)) != (int)(AsnReader.ReadIntegerBytes(null!))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [16] System.Formats.Asn1/System.Formats.Asn1.AsnReader::ReadNamedBitList:System.Collections.BitArray(System.Nullable{System.Formats.Asn1.Asn1Tag})
    public static void Method16()
    {
        try { if ((int)(AsnReader.ReadNamedBitList(null!)) != (int)(AsnReader.ReadNamedBitList(null!))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [17] System.Formats.Asn1/System.Formats.Asn1.AsnReader::ReadNamedBitListValue{TFlagsEnum}:TFlagsEnum(System.Nullable{System.Formats.Asn1.Asn1Tag})

    // [18] System.Formats.Asn1/System.Formats.Asn1.AsnReader::ReadNamedBitListValue:System.Enum(System.Type,System.Nullable{System.Formats.Asn1.Asn1Tag})
    public static void Method18()
    {
        try { if ((int)(AsnReader.ReadNamedBitListValue(typeof(byte), null!)) != (int)(AsnReader.ReadNamedBitListValue(typeof(byte), null!))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [19] System.Formats.Asn1/System.Formats.Asn1.AsnReader::ReadNull:System.Void(System.Nullable{System.Formats.Asn1.Asn1Tag})
    public static void Method19()
    {
        try { AsnReader.ReadNull(null!); }
        catch { _exitCode = 1; }
    }

    // [20] System.Formats.Asn1/System.Formats.Asn1.AsnReader::ReadObjectIdentifier:System.String(System.Nullable{System.Formats.Asn1.Asn1Tag})
    public static void Method20()
    {
        try { if (((AsnReader.ReadObjectIdentifier(null!)).Length) != ((AsnReader.ReadObjectIdentifier(null!)).Length)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [21] System.Formats.Asn1/System.Formats.Asn1.AsnReader::ReadOctetString:System.Byte{}(System.Nullable{System.Formats.Asn1.Asn1Tag})
    public static void Method21()
    {
        try { if ((int)(AsnReader.ReadOctetString(null!)) != (int)(AsnReader.ReadOctetString(null!))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [22] System.Formats.Asn1/System.Formats.Asn1.AsnReader::ReadSequence:System.Formats.Asn1.AsnReader(System.Nullable{System.Formats.Asn1.Asn1Tag})
    public static void Method22()
    {
        try { if ((int)(AsnReader.ReadSequence(null!)) != (int)(AsnReader.ReadSequence(null!))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [23] System.Formats.Asn1/System.Formats.Asn1.AsnReader::ReadSetOf:System.Formats.Asn1.AsnReader(System.Nullable{System.Formats.Asn1.Asn1Tag})
    public static void Method23()
    {
        try { if ((int)(AsnReader.ReadSetOf(null!)) != (int)(AsnReader.ReadSetOf(null!))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [24] System.Formats.Asn1/System.Formats.Asn1.AsnReader::ReadSetOf:System.Formats.Asn1.AsnReader(System.Boolean,System.Nullable{System.Formats.Asn1.Asn1Tag})
    public static void Method24()
    {
        try { if ((int)(AsnReader.ReadSetOf(true, null!)) != (int)(AsnReader.ReadSetOf(true, null!))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [25] System.Formats.Asn1/System.Formats.Asn1.AsnReader::ReadUtcTime:System.DateTimeOffset(System.Nullable{System.Formats.Asn1.Asn1Tag})
    public static void Method25()
    {
        try { if ((int)(AsnReader.ReadUtcTime(null!)) != (int)(AsnReader.ReadUtcTime(null!))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [26] System.Formats.Asn1/System.Formats.Asn1.AsnReader::ReadUtcTime:System.DateTimeOffset(System.Int32,System.Nullable{System.Formats.Asn1.Asn1Tag})
    public static void Method26()
    {
        try { if ((int)(AsnReader.ReadUtcTime(42, null!)) != (int)(AsnReader.ReadUtcTime(42, null!))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [27] System.Formats.Asn1/System.Formats.Asn1.AsnReader::ThrowIfNotEmpty:System.Void()
    public static void Method27()
    {
        try { AsnReader.ThrowIfNotEmpty(); }
        catch { _exitCode = 1; }
    }

    // [28] System.Formats.Asn1/System.Formats.Asn1.AsnReader::TryReadBitString:System.Boolean(System.Span{System.Byte},System.Int32&,System.Int32&,System.Nullable{System.Formats.Asn1.Asn1Tag})
    public static void Method28()
    {
    var refLocal_1 = 42;
    var refLocal_2 = 42;
        try { if (((AsnReader.TryReadBitString(null!, ref refLocal_1, ref refLocal_2, null!)) ? 1 : 0) != ((AsnReader.TryReadBitString(null!, ref refLocal_1, ref refLocal_2, null!)) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [29] System.Formats.Asn1/System.Formats.Asn1.AsnReader::TryReadCharacterString:System.Boolean(System.Span{System.Char},System.Formats.Asn1.UniversalTagNumber,System.Int32&,System.Nullable{System.Formats.Asn1.Asn1Tag})
    public static void Method29()
    {
    var refLocal_2 = 42;
        try { if (((AsnReader.TryReadCharacterString(null!, null!, ref refLocal_2, null!)) ? 1 : 0) != ((AsnReader.TryReadCharacterString(null!, null!, ref refLocal_2, null!)) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [30] System.Formats.Asn1/System.Formats.Asn1.AsnReader::TryReadCharacterStringBytes:System.Boolean(System.Span{System.Byte},System.Formats.Asn1.Asn1Tag,System.Int32&)
    public static void Method30()
    {
    var refLocal_2 = 42;
        try { if (((AsnReader.TryReadCharacterStringBytes(null!, null!, ref refLocal_2)) ? 1 : 0) != ((AsnReader.TryReadCharacterStringBytes(null!, null!, ref refLocal_2)) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [31] System.Formats.Asn1/System.Formats.Asn1.AsnReader::TryReadInt32:System.Boolean(System.Int32&,System.Nullable{System.Formats.Asn1.Asn1Tag})
    public static void Method31()
    {
    var refLocal_0 = 42;
        try { if (((AsnReader.TryReadInt32(ref refLocal_0, null!)) ? 1 : 0) != ((AsnReader.TryReadInt32(ref refLocal_0, null!)) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [32] System.Formats.Asn1/System.Formats.Asn1.AsnReader::TryReadInt64:System.Boolean(System.Int64&,System.Nullable{System.Formats.Asn1.Asn1Tag})
    public static void Method32()
    {
    var refLocal_0 = 42L;
        try { if (((AsnReader.TryReadInt64(ref refLocal_0, null!)) ? 1 : 0) != ((AsnReader.TryReadInt64(ref refLocal_0, null!)) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [33] System.Formats.Asn1/System.Formats.Asn1.AsnReader::TryReadOctetString:System.Boolean(System.Span{System.Byte},System.Int32&,System.Nullable{System.Formats.Asn1.Asn1Tag})
    public static void Method33()
    {
    var refLocal_1 = 42;
        try { if (((AsnReader.TryReadOctetString(null!, ref refLocal_1, null!)) ? 1 : 0) != ((AsnReader.TryReadOctetString(null!, ref refLocal_1, null!)) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [34] System.Formats.Asn1/System.Formats.Asn1.AsnReader::TryReadPrimitiveBitString:System.Boolean(System.Int32&,System.ReadOnlyMemory{System.Byte}&,System.Nullable{System.Formats.Asn1.Asn1Tag})
    public static void Method34()
    {
    var refLocal_0 = 42;
    var refLocal_1 = null!;
        try { if (((AsnReader.TryReadPrimitiveBitString(ref refLocal_0, ref refLocal_1, null!)) ? 1 : 0) != ((AsnReader.TryReadPrimitiveBitString(ref refLocal_0, ref refLocal_1, null!)) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [35] System.Formats.Asn1/System.Formats.Asn1.AsnReader::TryReadPrimitiveCharacterStringBytes:System.Boolean(System.Formats.Asn1.Asn1Tag,System.ReadOnlyMemory{System.Byte}&)
    public static void Method35()
    {
    var refLocal_1 = null!;
        try { if (((AsnReader.TryReadPrimitiveCharacterStringBytes(null!, ref refLocal_1)) ? 1 : 0) != ((AsnReader.TryReadPrimitiveCharacterStringBytes(null!, ref refLocal_1)) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [36] System.Formats.Asn1/System.Formats.Asn1.AsnReader::TryReadPrimitiveOctetString:System.Boolean(System.ReadOnlyMemory{System.Byte}&,System.Nullable{System.Formats.Asn1.Asn1Tag})
    public static void Method36()
    {
    var refLocal_0 = null!;
        try { if (((AsnReader.TryReadPrimitiveOctetString(ref refLocal_0, null!)) ? 1 : 0) != ((AsnReader.TryReadPrimitiveOctetString(ref refLocal_0, null!)) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [37] System.Formats.Asn1/System.Formats.Asn1.AsnReader::TryReadUInt32:System.Boolean(System.UInt32&,System.Nullable{System.Formats.Asn1.Asn1Tag})
    public static void Method37()
    {
    var refLocal_0 = 42u;
        try { if (((AsnReader.TryReadUInt32(ref refLocal_0, null!)) ? 1 : 0) != ((AsnReader.TryReadUInt32(ref refLocal_0, null!)) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [38] System.Formats.Asn1/System.Formats.Asn1.AsnReader::TryReadUInt64:System.Boolean(System.UInt64&,System.Nullable{System.Formats.Asn1.Asn1Tag})
    public static void Method38()
    {
    var refLocal_0 = 42uL;
        try { if (((AsnReader.TryReadUInt64(ref refLocal_0, null!)) ? 1 : 0) != ((AsnReader.TryReadUInt64(ref refLocal_0, null!)) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [39] System.Formats.Asn1/System.Formats.Asn1.AsnReader::.ctor:System.Void(System.ReadOnlyMemory{System.Byte},System.Formats.Asn1.AsnEncodingRules,System.Formats.Asn1.AsnReaderOptions)
    public static void Method39()
    {
        try { new AsnReader(null!, null!, null!); }
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
                case 11: CustomEntryMethod11(); break;
                case 12: Method12(); break;
                case 13: Method13(); break;
                case 14: Method14(); break;
                case 15: Method15(); break;
                case 16: Method16(); break;
                case 17: CustomEntryMethod17(); break;
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
            }
        }
        catch
        {
            _exitCode = 1;
        }
    }

}