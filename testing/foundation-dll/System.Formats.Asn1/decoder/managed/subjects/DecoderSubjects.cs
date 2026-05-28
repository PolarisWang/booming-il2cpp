// Auto-generated native-AOT entry point
// Family: family/System.Formats.Asn1/decoder
// Assembly: System.Formats.Asn1
// Variant: subjects

using System;
using System.Collections;
using System.Collections.Generic;
using System.Formats.Asn1;
using System.Linq;
using System.Numerics;

public static partial class DecoderSubjects
{
    // Inlined exit code — avoids SDK method call resolution in codegen
    public static int _exitCode;

    // [0] System.Formats.Asn1/System.Formats.Asn1.AsnDecoder::DecodeLength:System.Nullable{System.Int32}(System.ReadOnlySpan{System.Byte},System.Formats.Asn1.AsnEncodingRules,System.Int32&)
    public static void Subject_0()
    {
            System.Int32 refLocal_2 = 42;
        try { if (((AsnDecoder.DecodeLength(default(ReadOnlySpan<byte>), default, out refLocal_2)).GetHashCode()) != ((AsnDecoder.DecodeLength(default(ReadOnlySpan<byte>), default, out refLocal_2)).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [1] System.Formats.Asn1/System.Formats.Asn1.AsnDecoder::ReadBitString:System.Byte{}(System.ReadOnlySpan{System.Byte},System.Formats.Asn1.AsnEncodingRules,System.Int32&,System.Int32&,System.Nullable{System.Formats.Asn1.Asn1Tag})
    public static void Subject_1()
    {
            System.Int32 refLocal_2 = 42;
            System.Int32 refLocal_3 = 42;
        try { if (((AsnDecoder.ReadBitString(default(ReadOnlySpan<byte>), default, out refLocal_2, out refLocal_3, default)).Length) != ((AsnDecoder.ReadBitString(default(ReadOnlySpan<byte>), default, out refLocal_2, out refLocal_3, default)).Length)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [2] System.Formats.Asn1/System.Formats.Asn1.AsnDecoder::ReadBoolean:System.Boolean(System.ReadOnlySpan{System.Byte},System.Formats.Asn1.AsnEncodingRules,System.Int32&,System.Nullable{System.Formats.Asn1.Asn1Tag})
    public static void Subject_2()
    {
            System.Int32 refLocal_2 = 42;
        try { if (((AsnDecoder.ReadBoolean(default(ReadOnlySpan<byte>), default, out refLocal_2, default)) ? 1 : 0) != ((AsnDecoder.ReadBoolean(default(ReadOnlySpan<byte>), default, out refLocal_2, default)) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [3] System.Formats.Asn1/System.Formats.Asn1.AsnDecoder::ReadCharacterString:System.String(System.ReadOnlySpan{System.Byte},System.Formats.Asn1.AsnEncodingRules,System.Formats.Asn1.UniversalTagNumber,System.Int32&,System.Nullable{System.Formats.Asn1.Asn1Tag})
    public static void Subject_3()
    {
            System.Int32 refLocal_3 = 42;
        try { if (((AsnDecoder.ReadCharacterString(default(ReadOnlySpan<byte>), default, default, out refLocal_3, default)).Length) != ((AsnDecoder.ReadCharacterString(default(ReadOnlySpan<byte>), default, default, out refLocal_3, default)).Length)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [4] System.Formats.Asn1/System.Formats.Asn1.AsnDecoder::ReadEncodedValue:System.Formats.Asn1.Asn1Tag(System.ReadOnlySpan{System.Byte},System.Formats.Asn1.AsnEncodingRules,System.Int32&,System.Int32&,System.Int32&)
    public static void Subject_4()
    {
            System.Int32 refLocal_2 = 42;
            System.Int32 refLocal_3 = 42;
            System.Int32 refLocal_4 = 42;
        try { if (((AsnDecoder.ReadEncodedValue(default(ReadOnlySpan<byte>), default, out refLocal_2, out refLocal_3, out refLocal_4)).GetHashCode()) != ((AsnDecoder.ReadEncodedValue(default(ReadOnlySpan<byte>), default, out refLocal_2, out refLocal_3, out refLocal_4)).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [5] System.Formats.Asn1/System.Formats.Asn1.AsnDecoder::ReadEnumeratedBytes:System.ReadOnlySpan{System.Byte}(System.ReadOnlySpan{System.Byte},System.Formats.Asn1.AsnEncodingRules,System.Int32&,System.Nullable{System.Formats.Asn1.Asn1Tag})
    public static void Subject_5()
    {
            System.Int32 refLocal_2 = 42;
        try { if (((AsnDecoder.ReadEnumeratedBytes(default(ReadOnlySpan<byte>), default, out refLocal_2, default)).GetHashCode()) != ((AsnDecoder.ReadEnumeratedBytes(default(ReadOnlySpan<byte>), default, out refLocal_2, default)).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [6] System.Formats.Asn1/System.Formats.Asn1.AsnDecoder::ReadEnumeratedValue{TEnum}:TEnum(System.ReadOnlySpan{System.Byte},System.Formats.Asn1.AsnEncodingRules,System.Int32&,System.Nullable{System.Formats.Asn1.Asn1Tag})
    public static void CustomEntrySubject_6()
    {
    }

    // [7] System.Formats.Asn1/System.Formats.Asn1.AsnDecoder::ReadEnumeratedValue:System.Enum(System.ReadOnlySpan{System.Byte},System.Formats.Asn1.AsnEncodingRules,System.Type,System.Int32&,System.Nullable{System.Formats.Asn1.Asn1Tag})
    public static void Subject_7()
    {
            System.Int32 refLocal_3 = 42;
        try { if (((AsnDecoder.ReadEnumeratedValue(default(ReadOnlySpan<byte>), default, typeof(byte), out refLocal_3, default)).GetHashCode()) != ((AsnDecoder.ReadEnumeratedValue(default(ReadOnlySpan<byte>), default, typeof(byte), out refLocal_3, default)).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [8] System.Formats.Asn1/System.Formats.Asn1.AsnDecoder::ReadGeneralizedTime:System.DateTimeOffset(System.ReadOnlySpan{System.Byte},System.Formats.Asn1.AsnEncodingRules,System.Int32&,System.Nullable{System.Formats.Asn1.Asn1Tag})
    public static void Subject_8()
    {
            System.Int32 refLocal_2 = 42;
        try { if (((AsnDecoder.ReadGeneralizedTime(default(ReadOnlySpan<byte>), default, out refLocal_2, default)).GetHashCode()) != ((AsnDecoder.ReadGeneralizedTime(default(ReadOnlySpan<byte>), default, out refLocal_2, default)).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [9] System.Formats.Asn1/System.Formats.Asn1.AsnDecoder::ReadInteger:System.Numerics.BigInteger(System.ReadOnlySpan{System.Byte},System.Formats.Asn1.AsnEncodingRules,System.Int32&,System.Nullable{System.Formats.Asn1.Asn1Tag})
    public static void Subject_9()
    {
            System.Int32 refLocal_2 = 42;
        try { if (((AsnDecoder.ReadInteger(default(ReadOnlySpan<byte>), default, out refLocal_2, default)).GetHashCode()) != ((AsnDecoder.ReadInteger(default(ReadOnlySpan<byte>), default, out refLocal_2, default)).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [10] System.Formats.Asn1/System.Formats.Asn1.AsnDecoder::ReadIntegerBytes:System.ReadOnlySpan{System.Byte}(System.ReadOnlySpan{System.Byte},System.Formats.Asn1.AsnEncodingRules,System.Int32&,System.Nullable{System.Formats.Asn1.Asn1Tag})
    public static void Subject_10()
    {
            System.Int32 refLocal_2 = 42;
        try { if (((AsnDecoder.ReadIntegerBytes(default(ReadOnlySpan<byte>), default, out refLocal_2, default)).GetHashCode()) != ((AsnDecoder.ReadIntegerBytes(default(ReadOnlySpan<byte>), default, out refLocal_2, default)).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [11] System.Formats.Asn1/System.Formats.Asn1.AsnDecoder::ReadNamedBitList:System.Collections.BitArray(System.ReadOnlySpan{System.Byte},System.Formats.Asn1.AsnEncodingRules,System.Int32&,System.Nullable{System.Formats.Asn1.Asn1Tag})
    public static void Subject_11()
    {
            System.Int32 refLocal_2 = 42;
        try { if (((AsnDecoder.ReadNamedBitList(default(ReadOnlySpan<byte>), default, out refLocal_2, default)).GetHashCode()) != ((AsnDecoder.ReadNamedBitList(default(ReadOnlySpan<byte>), default, out refLocal_2, default)).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [12] System.Formats.Asn1/System.Formats.Asn1.AsnDecoder::ReadNamedBitListValue{TFlagsEnum}:TFlagsEnum(System.ReadOnlySpan{System.Byte},System.Formats.Asn1.AsnEncodingRules,System.Int32&,System.Nullable{System.Formats.Asn1.Asn1Tag})
    public static void CustomEntrySubject_12()
    {
    }

    // [13] System.Formats.Asn1/System.Formats.Asn1.AsnDecoder::ReadNamedBitListValue:System.Enum(System.ReadOnlySpan{System.Byte},System.Formats.Asn1.AsnEncodingRules,System.Type,System.Int32&,System.Nullable{System.Formats.Asn1.Asn1Tag})
    public static void Subject_13()
    {
            System.Int32 refLocal_3 = 42;
        try { if (((AsnDecoder.ReadNamedBitListValue(default(ReadOnlySpan<byte>), default, typeof(byte), out refLocal_3, default)).GetHashCode()) != ((AsnDecoder.ReadNamedBitListValue(default(ReadOnlySpan<byte>), default, typeof(byte), out refLocal_3, default)).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [14] System.Formats.Asn1/System.Formats.Asn1.AsnDecoder::ReadNull:System.Void(System.ReadOnlySpan{System.Byte},System.Formats.Asn1.AsnEncodingRules,System.Int32&,System.Nullable{System.Formats.Asn1.Asn1Tag})
    public static void Subject_14()
    {
            System.Int32 refLocal_2 = 42;
        try { AsnDecoder.ReadNull(default(ReadOnlySpan<byte>), default, out refLocal_2, default); }
        catch { _exitCode = 1; }
    }

    // [15] System.Formats.Asn1/System.Formats.Asn1.AsnDecoder::ReadObjectIdentifier:System.String(System.ReadOnlySpan{System.Byte},System.Formats.Asn1.AsnEncodingRules,System.Int32&,System.Nullable{System.Formats.Asn1.Asn1Tag})
    public static void Subject_15()
    {
            System.Int32 refLocal_2 = 42;
        try { if (((AsnDecoder.ReadObjectIdentifier(default(ReadOnlySpan<byte>), default, out refLocal_2, default)).Length) != ((AsnDecoder.ReadObjectIdentifier(default(ReadOnlySpan<byte>), default, out refLocal_2, default)).Length)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [16] System.Formats.Asn1/System.Formats.Asn1.AsnDecoder::ReadOctetString:System.Byte{}(System.ReadOnlySpan{System.Byte},System.Formats.Asn1.AsnEncodingRules,System.Int32&,System.Nullable{System.Formats.Asn1.Asn1Tag})
    public static void Subject_16()
    {
            System.Int32 refLocal_2 = 42;
        try { if (((AsnDecoder.ReadOctetString(default(ReadOnlySpan<byte>), default, out refLocal_2, default)).Length) != ((AsnDecoder.ReadOctetString(default(ReadOnlySpan<byte>), default, out refLocal_2, default)).Length)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [17] System.Formats.Asn1/System.Formats.Asn1.AsnDecoder::ReadSequence:System.Void(System.ReadOnlySpan{System.Byte},System.Formats.Asn1.AsnEncodingRules,System.Int32&,System.Int32&,System.Int32&,System.Nullable{System.Formats.Asn1.Asn1Tag})
    public static void Subject_17()
    {
            System.Int32 refLocal_2 = 42;
            System.Int32 refLocal_3 = 42;
            System.Int32 refLocal_4 = 42;
        try { AsnDecoder.ReadSequence(default(ReadOnlySpan<byte>), default, out refLocal_2, out refLocal_3, out refLocal_4, default); }
        catch { _exitCode = 1; }
    }

    // [18] System.Formats.Asn1/System.Formats.Asn1.AsnDecoder::ReadSetOf:System.Void(System.ReadOnlySpan{System.Byte},System.Formats.Asn1.AsnEncodingRules,System.Int32&,System.Int32&,System.Int32&,System.Boolean,System.Nullable{System.Formats.Asn1.Asn1Tag})
    public static void Subject_18()
    {
            System.Int32 refLocal_2 = 42;
            System.Int32 refLocal_3 = 42;
            System.Int32 refLocal_4 = 42;
        try { AsnDecoder.ReadSetOf(default(ReadOnlySpan<byte>), default, out refLocal_2, out refLocal_3, out refLocal_4, true, default); }
        catch { _exitCode = 1; }
    }

    // [19] System.Formats.Asn1/System.Formats.Asn1.AsnDecoder::ReadUtcTime:System.DateTimeOffset(System.ReadOnlySpan{System.Byte},System.Formats.Asn1.AsnEncodingRules,System.Int32&,System.Int32,System.Nullable{System.Formats.Asn1.Asn1Tag})
    public static void Subject_19()
    {
            System.Int32 refLocal_2 = 42;
        try { if (((AsnDecoder.ReadUtcTime(default(ReadOnlySpan<byte>), default, out refLocal_2, 42, default)).GetHashCode()) != ((AsnDecoder.ReadUtcTime(default(ReadOnlySpan<byte>), default, out refLocal_2, 42, default)).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [20] System.Formats.Asn1/System.Formats.Asn1.AsnDecoder::TryDecodeLength:System.Boolean(System.ReadOnlySpan{System.Byte},System.Formats.Asn1.AsnEncodingRules,System.Nullable{System.Int32}&,System.Int32&)
    public static void Subject_20()
    {
            System.Nullable<System.Int32> refLocal_2 = default;
            System.Int32 refLocal_3 = 42;
        try { if (((AsnDecoder.TryDecodeLength(default(ReadOnlySpan<byte>), default, out refLocal_2, out refLocal_3)) ? 1 : 0) != ((AsnDecoder.TryDecodeLength(default(ReadOnlySpan<byte>), default, out refLocal_2, out refLocal_3)) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [21] System.Formats.Asn1/System.Formats.Asn1.AsnDecoder::TryReadBitString:System.Boolean(System.ReadOnlySpan{System.Byte},System.Span{System.Byte},System.Formats.Asn1.AsnEncodingRules,System.Int32&,System.Int32&,System.Int32&,System.Nullable{System.Formats.Asn1.Asn1Tag})
    public static void Subject_21()
    {
            System.Int32 refLocal_3 = 42;
            System.Int32 refLocal_4 = 42;
            System.Int32 refLocal_5 = 42;
        try { if (((AsnDecoder.TryReadBitString(default(ReadOnlySpan<byte>), default(Span<byte>), default, out refLocal_3, out refLocal_4, out refLocal_5, default)) ? 1 : 0) != ((AsnDecoder.TryReadBitString(default(ReadOnlySpan<byte>), default(Span<byte>), default, out refLocal_3, out refLocal_4, out refLocal_5, default)) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [22] System.Formats.Asn1/System.Formats.Asn1.AsnDecoder::TryReadCharacterString:System.Boolean(System.ReadOnlySpan{System.Byte},System.Span{System.Char},System.Formats.Asn1.AsnEncodingRules,System.Formats.Asn1.UniversalTagNumber,System.Int32&,System.Int32&,System.Nullable{System.Formats.Asn1.Asn1Tag})
    public static void Subject_22()
    {
            System.Int32 refLocal_4 = 42;
            System.Int32 refLocal_5 = 42;
        try { if (((AsnDecoder.TryReadCharacterString(default(ReadOnlySpan<byte>), default(Span<char>), default, default, out refLocal_4, out refLocal_5, default)) ? 1 : 0) != ((AsnDecoder.TryReadCharacterString(default(ReadOnlySpan<byte>), default(Span<char>), default, default, out refLocal_4, out refLocal_5, default)) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [23] System.Formats.Asn1/System.Formats.Asn1.AsnDecoder::TryReadCharacterStringBytes:System.Boolean(System.ReadOnlySpan{System.Byte},System.Span{System.Byte},System.Formats.Asn1.AsnEncodingRules,System.Formats.Asn1.Asn1Tag,System.Int32&,System.Int32&)
    public static void Subject_23()
    {
            System.Int32 refLocal_4 = 42;
            System.Int32 refLocal_5 = 42;
        try { if (((AsnDecoder.TryReadCharacterStringBytes(default(ReadOnlySpan<byte>), default(Span<byte>), default, default, out refLocal_4, out refLocal_5)) ? 1 : 0) != ((AsnDecoder.TryReadCharacterStringBytes(default(ReadOnlySpan<byte>), default(Span<byte>), default, default, out refLocal_4, out refLocal_5)) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [24] System.Formats.Asn1/System.Formats.Asn1.AsnDecoder::TryReadEncodedValue:System.Boolean(System.ReadOnlySpan{System.Byte},System.Formats.Asn1.AsnEncodingRules,System.Formats.Asn1.Asn1Tag&,System.Int32&,System.Int32&,System.Int32&)
    public static void Subject_24()
    {
            System.Formats.Asn1.Asn1Tag refLocal_2 = default;
            System.Int32 refLocal_3 = 42;
            System.Int32 refLocal_4 = 42;
            System.Int32 refLocal_5 = 42;
        try { if (((AsnDecoder.TryReadEncodedValue(default(ReadOnlySpan<byte>), default, out refLocal_2, out refLocal_3, out refLocal_4, out refLocal_5)) ? 1 : 0) != ((AsnDecoder.TryReadEncodedValue(default(ReadOnlySpan<byte>), default, out refLocal_2, out refLocal_3, out refLocal_4, out refLocal_5)) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [25] System.Formats.Asn1/System.Formats.Asn1.AsnDecoder::TryReadInt32:System.Boolean(System.ReadOnlySpan{System.Byte},System.Formats.Asn1.AsnEncodingRules,System.Int32&,System.Int32&,System.Nullable{System.Formats.Asn1.Asn1Tag})
    public static void Subject_25()
    {
            System.Int32 refLocal_2 = 42;
            System.Int32 refLocal_3 = 42;
        try { if (((AsnDecoder.TryReadInt32(default(ReadOnlySpan<byte>), default, out refLocal_2, out refLocal_3, default)) ? 1 : 0) != ((AsnDecoder.TryReadInt32(default(ReadOnlySpan<byte>), default, out refLocal_2, out refLocal_3, default)) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [26] System.Formats.Asn1/System.Formats.Asn1.AsnDecoder::TryReadInt64:System.Boolean(System.ReadOnlySpan{System.Byte},System.Formats.Asn1.AsnEncodingRules,System.Int64&,System.Int32&,System.Nullable{System.Formats.Asn1.Asn1Tag})
    public static void Subject_26()
    {
            System.Int64 refLocal_2 = 42L;
            System.Int32 refLocal_3 = 42;
        try { if (((AsnDecoder.TryReadInt64(default(ReadOnlySpan<byte>), default, out refLocal_2, out refLocal_3, default)) ? 1 : 0) != ((AsnDecoder.TryReadInt64(default(ReadOnlySpan<byte>), default, out refLocal_2, out refLocal_3, default)) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [27] System.Formats.Asn1/System.Formats.Asn1.AsnDecoder::TryReadOctetString:System.Boolean(System.ReadOnlySpan{System.Byte},System.Span{System.Byte},System.Formats.Asn1.AsnEncodingRules,System.Int32&,System.Int32&,System.Nullable{System.Formats.Asn1.Asn1Tag})
    public static void Subject_27()
    {
            System.Int32 refLocal_3 = 42;
            System.Int32 refLocal_4 = 42;
        try { if (((AsnDecoder.TryReadOctetString(default(ReadOnlySpan<byte>), default(Span<byte>), default, out refLocal_3, out refLocal_4, default)) ? 1 : 0) != ((AsnDecoder.TryReadOctetString(default(ReadOnlySpan<byte>), default(Span<byte>), default, out refLocal_3, out refLocal_4, default)) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [28] System.Formats.Asn1/System.Formats.Asn1.AsnDecoder::TryReadPrimitiveBitString:System.Boolean(System.ReadOnlySpan{System.Byte},System.Formats.Asn1.AsnEncodingRules,System.Int32&,System.ReadOnlySpan{System.Byte}&,System.Int32&,System.Nullable{System.Formats.Asn1.Asn1Tag})
    public static void Subject_28()
    {
            System.Int32 refLocal_2 = 42;
            System.ReadOnlySpan<System.Byte> refLocal_3 = default;
            System.Int32 refLocal_4 = 42;
        try { if (((AsnDecoder.TryReadPrimitiveBitString(default(ReadOnlySpan<byte>), default, out refLocal_2, out refLocal_3, out refLocal_4, default)) ? 1 : 0) != ((AsnDecoder.TryReadPrimitiveBitString(default(ReadOnlySpan<byte>), default, out refLocal_2, out refLocal_3, out refLocal_4, default)) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [29] System.Formats.Asn1/System.Formats.Asn1.AsnDecoder::TryReadPrimitiveCharacterStringBytes:System.Boolean(System.ReadOnlySpan{System.Byte},System.Formats.Asn1.AsnEncodingRules,System.Formats.Asn1.Asn1Tag,System.ReadOnlySpan{System.Byte}&,System.Int32&)
    public static void Subject_29()
    {
            System.ReadOnlySpan<System.Byte> refLocal_3 = default;
            System.Int32 refLocal_4 = 42;
        try { if (((AsnDecoder.TryReadPrimitiveCharacterStringBytes(default(ReadOnlySpan<byte>), default, default, out refLocal_3, out refLocal_4)) ? 1 : 0) != ((AsnDecoder.TryReadPrimitiveCharacterStringBytes(default(ReadOnlySpan<byte>), default, default, out refLocal_3, out refLocal_4)) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [30] System.Formats.Asn1/System.Formats.Asn1.AsnDecoder::TryReadPrimitiveOctetString:System.Boolean(System.ReadOnlySpan{System.Byte},System.Formats.Asn1.AsnEncodingRules,System.ReadOnlySpan{System.Byte}&,System.Int32&,System.Nullable{System.Formats.Asn1.Asn1Tag})
    public static void Subject_30()
    {
            System.ReadOnlySpan<System.Byte> refLocal_2 = default;
            System.Int32 refLocal_3 = 42;
        try { if (((AsnDecoder.TryReadPrimitiveOctetString(default(ReadOnlySpan<byte>), default, out refLocal_2, out refLocal_3, default)) ? 1 : 0) != ((AsnDecoder.TryReadPrimitiveOctetString(default(ReadOnlySpan<byte>), default, out refLocal_2, out refLocal_3, default)) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [31] System.Formats.Asn1/System.Formats.Asn1.AsnDecoder::TryReadUInt32:System.Boolean(System.ReadOnlySpan{System.Byte},System.Formats.Asn1.AsnEncodingRules,System.UInt32&,System.Int32&,System.Nullable{System.Formats.Asn1.Asn1Tag})
    public static void Subject_31()
    {
            System.UInt32 refLocal_2 = 42u;
            System.Int32 refLocal_3 = 42;
        try { if (((AsnDecoder.TryReadUInt32(default(ReadOnlySpan<byte>), default, out refLocal_2, out refLocal_3, default)) ? 1 : 0) != ((AsnDecoder.TryReadUInt32(default(ReadOnlySpan<byte>), default, out refLocal_2, out refLocal_3, default)) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [32] System.Formats.Asn1/System.Formats.Asn1.AsnDecoder::TryReadUInt64:System.Boolean(System.ReadOnlySpan{System.Byte},System.Formats.Asn1.AsnEncodingRules,System.UInt64&,System.Int32&,System.Nullable{System.Formats.Asn1.Asn1Tag})
    public static void Subject_32()
    {
            System.UInt64 refLocal_2 = 42uL;
            System.Int32 refLocal_3 = 42;
        try { if (((AsnDecoder.TryReadUInt64(default(ReadOnlySpan<byte>), default, out refLocal_2, out refLocal_3, default)) ? 1 : 0) != ((AsnDecoder.TryReadUInt64(default(ReadOnlySpan<byte>), default, out refLocal_2, out refLocal_3, default)) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

}