// Auto-generated native-AOT entry point
// Family: family/System.Formats.Asn1/decoder
// Assembly: System.Formats.Asn1
// Variant: benchmark

using System;
using System.Collections;
using System.Collections.Generic;
using System.Formats.Asn1;
using System.Linq;
using System.Numerics;

public static partial class DecoderNativeEntry
{
    // Inlined exit code — avoids SDK method call resolution in codegen
    public static int _exitCode;

    // [0] System.Formats.Asn1/System.Formats.Asn1.AsnDecoder::DecodeLength:System.Nullable{System.Int32}(System.ReadOnlySpan{System.Byte},System.Formats.Asn1.AsnEncodingRules,System.Int32&)
    public static void Method0()
    {
    var refLocal_2 = 42;
        try { if ((int)(AsnDecoder.DecodeLength(null!, null!, ref refLocal_2)) != (int)(AsnDecoder.DecodeLength(null!, null!, ref refLocal_2))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [1] System.Formats.Asn1/System.Formats.Asn1.AsnDecoder::ReadBitString:System.Byte{}(System.ReadOnlySpan{System.Byte},System.Formats.Asn1.AsnEncodingRules,System.Int32&,System.Int32&,System.Nullable{System.Formats.Asn1.Asn1Tag})
    public static void Method1()
    {
    var refLocal_2 = 42;
    var refLocal_3 = 42;
        try { if ((int)(AsnDecoder.ReadBitString(null!, null!, ref refLocal_2, ref refLocal_3, null!)) != (int)(AsnDecoder.ReadBitString(null!, null!, ref refLocal_2, ref refLocal_3, null!))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [2] System.Formats.Asn1/System.Formats.Asn1.AsnDecoder::ReadBoolean:System.Boolean(System.ReadOnlySpan{System.Byte},System.Formats.Asn1.AsnEncodingRules,System.Int32&,System.Nullable{System.Formats.Asn1.Asn1Tag})
    public static void Method2()
    {
    var refLocal_2 = 42;
        try { if (((AsnDecoder.ReadBoolean(null!, null!, ref refLocal_2, null!)) ? 1 : 0) != ((AsnDecoder.ReadBoolean(null!, null!, ref refLocal_2, null!)) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [3] System.Formats.Asn1/System.Formats.Asn1.AsnDecoder::ReadCharacterString:System.String(System.ReadOnlySpan{System.Byte},System.Formats.Asn1.AsnEncodingRules,System.Formats.Asn1.UniversalTagNumber,System.Int32&,System.Nullable{System.Formats.Asn1.Asn1Tag})
    public static void Method3()
    {
    var refLocal_3 = 42;
        try { if (((AsnDecoder.ReadCharacterString(null!, null!, null!, ref refLocal_3, null!)).Length) != ((AsnDecoder.ReadCharacterString(null!, null!, null!, ref refLocal_3, null!)).Length)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [4] System.Formats.Asn1/System.Formats.Asn1.AsnDecoder::ReadEncodedValue:System.Formats.Asn1.Asn1Tag(System.ReadOnlySpan{System.Byte},System.Formats.Asn1.AsnEncodingRules,System.Int32&,System.Int32&,System.Int32&)
    public static void Method4()
    {
    var refLocal_2 = 42;
    var refLocal_3 = 42;
    var refLocal_4 = 42;
        try { if ((int)(AsnDecoder.ReadEncodedValue(null!, null!, ref refLocal_2, ref refLocal_3, ref refLocal_4)) != (int)(AsnDecoder.ReadEncodedValue(null!, null!, ref refLocal_2, ref refLocal_3, ref refLocal_4))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [5] System.Formats.Asn1/System.Formats.Asn1.AsnDecoder::ReadEnumeratedBytes:System.ReadOnlySpan{System.Byte}(System.ReadOnlySpan{System.Byte},System.Formats.Asn1.AsnEncodingRules,System.Int32&,System.Nullable{System.Formats.Asn1.Asn1Tag})
    public static void Method5()
    {
    var refLocal_2 = 42;
        try { if ((int)(AsnDecoder.ReadEnumeratedBytes(null!, null!, ref refLocal_2, null!)) != (int)(AsnDecoder.ReadEnumeratedBytes(null!, null!, ref refLocal_2, null!))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [6] System.Formats.Asn1/System.Formats.Asn1.AsnDecoder::ReadEnumeratedValue{TEnum}:TEnum(System.ReadOnlySpan{System.Byte},System.Formats.Asn1.AsnEncodingRules,System.Int32&,System.Nullable{System.Formats.Asn1.Asn1Tag})

    // [7] System.Formats.Asn1/System.Formats.Asn1.AsnDecoder::ReadEnumeratedValue:System.Enum(System.ReadOnlySpan{System.Byte},System.Formats.Asn1.AsnEncodingRules,System.Type,System.Int32&,System.Nullable{System.Formats.Asn1.Asn1Tag})
    public static void Method7()
    {
    var refLocal_3 = 42;
        try { if ((int)(AsnDecoder.ReadEnumeratedValue(null!, null!, typeof(byte), ref refLocal_3, null!)) != (int)(AsnDecoder.ReadEnumeratedValue(null!, null!, typeof(byte), ref refLocal_3, null!))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [8] System.Formats.Asn1/System.Formats.Asn1.AsnDecoder::ReadGeneralizedTime:System.DateTimeOffset(System.ReadOnlySpan{System.Byte},System.Formats.Asn1.AsnEncodingRules,System.Int32&,System.Nullable{System.Formats.Asn1.Asn1Tag})
    public static void Method8()
    {
    var refLocal_2 = 42;
        try { if ((int)(AsnDecoder.ReadGeneralizedTime(null!, null!, ref refLocal_2, null!)) != (int)(AsnDecoder.ReadGeneralizedTime(null!, null!, ref refLocal_2, null!))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [9] System.Formats.Asn1/System.Formats.Asn1.AsnDecoder::ReadInteger:System.Numerics.BigInteger(System.ReadOnlySpan{System.Byte},System.Formats.Asn1.AsnEncodingRules,System.Int32&,System.Nullable{System.Formats.Asn1.Asn1Tag})
    public static void Method9()
    {
    var refLocal_2 = 42;
        try { if ((int)(AsnDecoder.ReadInteger(null!, null!, ref refLocal_2, null!)) != (int)(AsnDecoder.ReadInteger(null!, null!, ref refLocal_2, null!))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [10] System.Formats.Asn1/System.Formats.Asn1.AsnDecoder::ReadIntegerBytes:System.ReadOnlySpan{System.Byte}(System.ReadOnlySpan{System.Byte},System.Formats.Asn1.AsnEncodingRules,System.Int32&,System.Nullable{System.Formats.Asn1.Asn1Tag})
    public static void Method10()
    {
    var refLocal_2 = 42;
        try { if ((int)(AsnDecoder.ReadIntegerBytes(null!, null!, ref refLocal_2, null!)) != (int)(AsnDecoder.ReadIntegerBytes(null!, null!, ref refLocal_2, null!))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [11] System.Formats.Asn1/System.Formats.Asn1.AsnDecoder::ReadNamedBitList:System.Collections.BitArray(System.ReadOnlySpan{System.Byte},System.Formats.Asn1.AsnEncodingRules,System.Int32&,System.Nullable{System.Formats.Asn1.Asn1Tag})
    public static void Method11()
    {
    var refLocal_2 = 42;
        try { if ((int)(AsnDecoder.ReadNamedBitList(null!, null!, ref refLocal_2, null!)) != (int)(AsnDecoder.ReadNamedBitList(null!, null!, ref refLocal_2, null!))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [12] System.Formats.Asn1/System.Formats.Asn1.AsnDecoder::ReadNamedBitListValue{TFlagsEnum}:TFlagsEnum(System.ReadOnlySpan{System.Byte},System.Formats.Asn1.AsnEncodingRules,System.Int32&,System.Nullable{System.Formats.Asn1.Asn1Tag})

    // [13] System.Formats.Asn1/System.Formats.Asn1.AsnDecoder::ReadNamedBitListValue:System.Enum(System.ReadOnlySpan{System.Byte},System.Formats.Asn1.AsnEncodingRules,System.Type,System.Int32&,System.Nullable{System.Formats.Asn1.Asn1Tag})
    public static void Method13()
    {
    var refLocal_3 = 42;
        try { if ((int)(AsnDecoder.ReadNamedBitListValue(null!, null!, typeof(byte), ref refLocal_3, null!)) != (int)(AsnDecoder.ReadNamedBitListValue(null!, null!, typeof(byte), ref refLocal_3, null!))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [14] System.Formats.Asn1/System.Formats.Asn1.AsnDecoder::ReadNull:System.Void(System.ReadOnlySpan{System.Byte},System.Formats.Asn1.AsnEncodingRules,System.Int32&,System.Nullable{System.Formats.Asn1.Asn1Tag})
    public static void Method14()
    {
    var refLocal_2 = 42;
        try { AsnDecoder.ReadNull(null!, null!, ref refLocal_2, null!); }
        catch { _exitCode = 1; }
    }

    // [15] System.Formats.Asn1/System.Formats.Asn1.AsnDecoder::ReadObjectIdentifier:System.String(System.ReadOnlySpan{System.Byte},System.Formats.Asn1.AsnEncodingRules,System.Int32&,System.Nullable{System.Formats.Asn1.Asn1Tag})
    public static void Method15()
    {
    var refLocal_2 = 42;
        try { if (((AsnDecoder.ReadObjectIdentifier(null!, null!, ref refLocal_2, null!)).Length) != ((AsnDecoder.ReadObjectIdentifier(null!, null!, ref refLocal_2, null!)).Length)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [16] System.Formats.Asn1/System.Formats.Asn1.AsnDecoder::ReadOctetString:System.Byte{}(System.ReadOnlySpan{System.Byte},System.Formats.Asn1.AsnEncodingRules,System.Int32&,System.Nullable{System.Formats.Asn1.Asn1Tag})
    public static void Method16()
    {
    var refLocal_2 = 42;
        try { if ((int)(AsnDecoder.ReadOctetString(null!, null!, ref refLocal_2, null!)) != (int)(AsnDecoder.ReadOctetString(null!, null!, ref refLocal_2, null!))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [17] System.Formats.Asn1/System.Formats.Asn1.AsnDecoder::ReadSequence:System.Void(System.ReadOnlySpan{System.Byte},System.Formats.Asn1.AsnEncodingRules,System.Int32&,System.Int32&,System.Int32&,System.Nullable{System.Formats.Asn1.Asn1Tag})
    public static void Method17()
    {
    var refLocal_2 = 42;
    var refLocal_3 = 42;
    var refLocal_4 = 42;
        try { AsnDecoder.ReadSequence(null!, null!, ref refLocal_2, ref refLocal_3, ref refLocal_4, null!); }
        catch { _exitCode = 1; }
    }

    // [18] System.Formats.Asn1/System.Formats.Asn1.AsnDecoder::ReadSetOf:System.Void(System.ReadOnlySpan{System.Byte},System.Formats.Asn1.AsnEncodingRules,System.Int32&,System.Int32&,System.Int32&,System.Boolean,System.Nullable{System.Formats.Asn1.Asn1Tag})
    public static void Method18()
    {
    var refLocal_2 = 42;
    var refLocal_3 = 42;
    var refLocal_4 = 42;
        try { AsnDecoder.ReadSetOf(null!, null!, ref refLocal_2, ref refLocal_3, ref refLocal_4, true, null!); }
        catch { _exitCode = 1; }
    }

    // [19] System.Formats.Asn1/System.Formats.Asn1.AsnDecoder::ReadUtcTime:System.DateTimeOffset(System.ReadOnlySpan{System.Byte},System.Formats.Asn1.AsnEncodingRules,System.Int32&,System.Int32,System.Nullable{System.Formats.Asn1.Asn1Tag})
    public static void Method19()
    {
    var refLocal_2 = 42;
        try { if ((int)(AsnDecoder.ReadUtcTime(null!, null!, ref refLocal_2, 42, null!)) != (int)(AsnDecoder.ReadUtcTime(null!, null!, ref refLocal_2, 42, null!))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [20] System.Formats.Asn1/System.Formats.Asn1.AsnDecoder::TryDecodeLength:System.Boolean(System.ReadOnlySpan{System.Byte},System.Formats.Asn1.AsnEncodingRules,System.Nullable{System.Int32}&,System.Int32&)
    public static void Method20()
    {
    var refLocal_2 = null!;
    var refLocal_3 = 42;
        try { if (((AsnDecoder.TryDecodeLength(null!, null!, ref refLocal_2, ref refLocal_3)) ? 1 : 0) != ((AsnDecoder.TryDecodeLength(null!, null!, ref refLocal_2, ref refLocal_3)) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [21] System.Formats.Asn1/System.Formats.Asn1.AsnDecoder::TryReadBitString:System.Boolean(System.ReadOnlySpan{System.Byte},System.Span{System.Byte},System.Formats.Asn1.AsnEncodingRules,System.Int32&,System.Int32&,System.Int32&,System.Nullable{System.Formats.Asn1.Asn1Tag})
    public static void Method21()
    {
    var refLocal_3 = 42;
    var refLocal_4 = 42;
    var refLocal_5 = 42;
        try { if (((AsnDecoder.TryReadBitString(null!, null!, null!, ref refLocal_3, ref refLocal_4, ref refLocal_5, null!)) ? 1 : 0) != ((AsnDecoder.TryReadBitString(null!, null!, null!, ref refLocal_3, ref refLocal_4, ref refLocal_5, null!)) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [22] System.Formats.Asn1/System.Formats.Asn1.AsnDecoder::TryReadCharacterString:System.Boolean(System.ReadOnlySpan{System.Byte},System.Span{System.Char},System.Formats.Asn1.AsnEncodingRules,System.Formats.Asn1.UniversalTagNumber,System.Int32&,System.Int32&,System.Nullable{System.Formats.Asn1.Asn1Tag})
    public static void Method22()
    {
    var refLocal_4 = 42;
    var refLocal_5 = 42;
        try { if (((AsnDecoder.TryReadCharacterString(null!, null!, null!, null!, ref refLocal_4, ref refLocal_5, null!)) ? 1 : 0) != ((AsnDecoder.TryReadCharacterString(null!, null!, null!, null!, ref refLocal_4, ref refLocal_5, null!)) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [23] System.Formats.Asn1/System.Formats.Asn1.AsnDecoder::TryReadCharacterStringBytes:System.Boolean(System.ReadOnlySpan{System.Byte},System.Span{System.Byte},System.Formats.Asn1.AsnEncodingRules,System.Formats.Asn1.Asn1Tag,System.Int32&,System.Int32&)
    public static void Method23()
    {
    var refLocal_4 = 42;
    var refLocal_5 = 42;
        try { if (((AsnDecoder.TryReadCharacterStringBytes(null!, null!, null!, null!, ref refLocal_4, ref refLocal_5)) ? 1 : 0) != ((AsnDecoder.TryReadCharacterStringBytes(null!, null!, null!, null!, ref refLocal_4, ref refLocal_5)) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [24] System.Formats.Asn1/System.Formats.Asn1.AsnDecoder::TryReadEncodedValue:System.Boolean(System.ReadOnlySpan{System.Byte},System.Formats.Asn1.AsnEncodingRules,System.Formats.Asn1.Asn1Tag&,System.Int32&,System.Int32&,System.Int32&)
    public static void Method24()
    {
    var refLocal_2 = null!;
    var refLocal_3 = 42;
    var refLocal_4 = 42;
    var refLocal_5 = 42;
        try { if (((AsnDecoder.TryReadEncodedValue(null!, null!, ref refLocal_2, ref refLocal_3, ref refLocal_4, ref refLocal_5)) ? 1 : 0) != ((AsnDecoder.TryReadEncodedValue(null!, null!, ref refLocal_2, ref refLocal_3, ref refLocal_4, ref refLocal_5)) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [25] System.Formats.Asn1/System.Formats.Asn1.AsnDecoder::TryReadInt32:System.Boolean(System.ReadOnlySpan{System.Byte},System.Formats.Asn1.AsnEncodingRules,System.Int32&,System.Int32&,System.Nullable{System.Formats.Asn1.Asn1Tag})
    public static void Method25()
    {
    var refLocal_2 = 42;
    var refLocal_3 = 42;
        try { if (((AsnDecoder.TryReadInt32(null!, null!, ref refLocal_2, ref refLocal_3, null!)) ? 1 : 0) != ((AsnDecoder.TryReadInt32(null!, null!, ref refLocal_2, ref refLocal_3, null!)) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [26] System.Formats.Asn1/System.Formats.Asn1.AsnDecoder::TryReadInt64:System.Boolean(System.ReadOnlySpan{System.Byte},System.Formats.Asn1.AsnEncodingRules,System.Int64&,System.Int32&,System.Nullable{System.Formats.Asn1.Asn1Tag})
    public static void Method26()
    {
    var refLocal_2 = 42L;
    var refLocal_3 = 42;
        try { if (((AsnDecoder.TryReadInt64(null!, null!, ref refLocal_2, ref refLocal_3, null!)) ? 1 : 0) != ((AsnDecoder.TryReadInt64(null!, null!, ref refLocal_2, ref refLocal_3, null!)) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [27] System.Formats.Asn1/System.Formats.Asn1.AsnDecoder::TryReadOctetString:System.Boolean(System.ReadOnlySpan{System.Byte},System.Span{System.Byte},System.Formats.Asn1.AsnEncodingRules,System.Int32&,System.Int32&,System.Nullable{System.Formats.Asn1.Asn1Tag})
    public static void Method27()
    {
    var refLocal_3 = 42;
    var refLocal_4 = 42;
        try { if (((AsnDecoder.TryReadOctetString(null!, null!, null!, ref refLocal_3, ref refLocal_4, null!)) ? 1 : 0) != ((AsnDecoder.TryReadOctetString(null!, null!, null!, ref refLocal_3, ref refLocal_4, null!)) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [28] System.Formats.Asn1/System.Formats.Asn1.AsnDecoder::TryReadPrimitiveBitString:System.Boolean(System.ReadOnlySpan{System.Byte},System.Formats.Asn1.AsnEncodingRules,System.Int32&,System.ReadOnlySpan{System.Byte}&,System.Int32&,System.Nullable{System.Formats.Asn1.Asn1Tag})
    public static void Method28()
    {
    var refLocal_2 = 42;
    var refLocal_3 = null!;
    var refLocal_4 = 42;
        try { if (((AsnDecoder.TryReadPrimitiveBitString(null!, null!, ref refLocal_2, ref refLocal_3, ref refLocal_4, null!)) ? 1 : 0) != ((AsnDecoder.TryReadPrimitiveBitString(null!, null!, ref refLocal_2, ref refLocal_3, ref refLocal_4, null!)) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [29] System.Formats.Asn1/System.Formats.Asn1.AsnDecoder::TryReadPrimitiveCharacterStringBytes:System.Boolean(System.ReadOnlySpan{System.Byte},System.Formats.Asn1.AsnEncodingRules,System.Formats.Asn1.Asn1Tag,System.ReadOnlySpan{System.Byte}&,System.Int32&)
    public static void Method29()
    {
    var refLocal_3 = null!;
    var refLocal_4 = 42;
        try { if (((AsnDecoder.TryReadPrimitiveCharacterStringBytes(null!, null!, null!, ref refLocal_3, ref refLocal_4)) ? 1 : 0) != ((AsnDecoder.TryReadPrimitiveCharacterStringBytes(null!, null!, null!, ref refLocal_3, ref refLocal_4)) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [30] System.Formats.Asn1/System.Formats.Asn1.AsnDecoder::TryReadPrimitiveOctetString:System.Boolean(System.ReadOnlySpan{System.Byte},System.Formats.Asn1.AsnEncodingRules,System.ReadOnlySpan{System.Byte}&,System.Int32&,System.Nullable{System.Formats.Asn1.Asn1Tag})
    public static void Method30()
    {
    var refLocal_2 = null!;
    var refLocal_3 = 42;
        try { if (((AsnDecoder.TryReadPrimitiveOctetString(null!, null!, ref refLocal_2, ref refLocal_3, null!)) ? 1 : 0) != ((AsnDecoder.TryReadPrimitiveOctetString(null!, null!, ref refLocal_2, ref refLocal_3, null!)) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [31] System.Formats.Asn1/System.Formats.Asn1.AsnDecoder::TryReadUInt32:System.Boolean(System.ReadOnlySpan{System.Byte},System.Formats.Asn1.AsnEncodingRules,System.UInt32&,System.Int32&,System.Nullable{System.Formats.Asn1.Asn1Tag})
    public static void Method31()
    {
    var refLocal_2 = 42u;
    var refLocal_3 = 42;
        try { if (((AsnDecoder.TryReadUInt32(null!, null!, ref refLocal_2, ref refLocal_3, null!)) ? 1 : 0) != ((AsnDecoder.TryReadUInt32(null!, null!, ref refLocal_2, ref refLocal_3, null!)) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [32] System.Formats.Asn1/System.Formats.Asn1.AsnDecoder::TryReadUInt64:System.Boolean(System.ReadOnlySpan{System.Byte},System.Formats.Asn1.AsnEncodingRules,System.UInt64&,System.Int32&,System.Nullable{System.Formats.Asn1.Asn1Tag})
    public static void Method32()
    {
    var refLocal_2 = 42uL;
    var refLocal_3 = 42;
        try { if (((AsnDecoder.TryReadUInt64(null!, null!, ref refLocal_2, ref refLocal_3, null!)) ? 1 : 0) != ((AsnDecoder.TryReadUInt64(null!, null!, ref refLocal_2, ref refLocal_3, null!)) ? 1 : 0)) _exitCode = 1; }
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
                case 6: CustomEntryMethod6(); break;
                case 7: Method7(); break;
                case 8: Method8(); break;
                case 9: Method9(); break;
                case 10: Method10(); break;
                case 11: Method11(); break;
                case 12: CustomEntryMethod12(); break;
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
            }
        }
        catch
        {
            _exitCode = 1;
        }
    }

}