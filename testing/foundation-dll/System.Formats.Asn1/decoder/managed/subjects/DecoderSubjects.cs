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
        try { AsnDecoder.DecodeLength(default(ReadOnlySpan<byte>), default, out _); _exitCode = 1; }
        catch (AsnContentException) { }
    }

    // [1] System.Formats.Asn1/System.Formats.Asn1.AsnDecoder::ReadBitString:System.Byte{}(System.ReadOnlySpan{System.Byte},System.Formats.Asn1.AsnEncodingRules,System.Int32&,System.Int32&,System.Nullable{System.Formats.Asn1.Asn1Tag})
    public static void Subject_1()
    {
        try { AsnDecoder.ReadBitString(default(ReadOnlySpan<byte>), default, out _, out _, default); _exitCode = 1; }
        catch (AsnContentException) { }
    }

    // [2] System.Formats.Asn1/System.Formats.Asn1.AsnDecoder::ReadBoolean:System.Boolean(System.ReadOnlySpan{System.Byte},System.Formats.Asn1.AsnEncodingRules,System.Int32&,System.Nullable{System.Formats.Asn1.Asn1Tag})
    public static void Subject_2()
    {
        try { AsnDecoder.ReadBoolean(default(ReadOnlySpan<byte>), default, out _, default); _exitCode = 1; }
        catch (AsnContentException) { }
    }

    // [3] System.Formats.Asn1/System.Formats.Asn1.AsnDecoder::ReadCharacterString:System.String(System.ReadOnlySpan{System.Byte},System.Formats.Asn1.AsnEncodingRules,System.Formats.Asn1.UniversalTagNumber,System.Int32&,System.Nullable{System.Formats.Asn1.Asn1Tag})
    public static void Subject_3()
    {
        try { AsnDecoder.ReadCharacterString(default(ReadOnlySpan<byte>), default, default, out _, default); _exitCode = 1; }
        catch (ArgumentOutOfRangeException) { }
    }

    // [4] System.Formats.Asn1/System.Formats.Asn1.AsnDecoder::ReadEncodedValue:System.Formats.Asn1.Asn1Tag(System.ReadOnlySpan{System.Byte},System.Formats.Asn1.AsnEncodingRules,System.Int32&,System.Int32&,System.Int32&)
    public static void Subject_4()
    {
        try { AsnDecoder.ReadEncodedValue(default(ReadOnlySpan<byte>), default, out _, out _, out _); _exitCode = 1; }
        catch (AsnContentException) { }
    }

    // [5] System.Formats.Asn1/System.Formats.Asn1.AsnDecoder::ReadEnumeratedBytes:System.ReadOnlySpan{System.Byte}(System.ReadOnlySpan{System.Byte},System.Formats.Asn1.AsnEncodingRules,System.Int32&,System.Nullable{System.Formats.Asn1.Asn1Tag})
    public static void Subject_5()
    {
        try { AsnDecoder.ReadEnumeratedBytes(default(ReadOnlySpan<byte>), default, out _, default); _exitCode = 1; }
        catch (AsnContentException) { }
    }

    // [6] System.Formats.Asn1/System.Formats.Asn1.AsnDecoder::ReadEnumeratedValue{TEnum}:TEnum(System.ReadOnlySpan{System.Byte},System.Formats.Asn1.AsnEncodingRules,System.Int32&,System.Nullable{System.Formats.Asn1.Asn1Tag})
    public static void CustomEntrySubject_6()
    {
    }

    // [7] System.Formats.Asn1/System.Formats.Asn1.AsnDecoder::ReadEnumeratedValue:System.Enum(System.ReadOnlySpan{System.Byte},System.Formats.Asn1.AsnEncodingRules,System.Type,System.Int32&,System.Nullable{System.Formats.Asn1.Asn1Tag})
    public static void Subject_7()
    {
        try { AsnDecoder.ReadEnumeratedValue(default(ReadOnlySpan<byte>), default, typeof(byte), out _, default); _exitCode = 1; }
        catch (ArgumentException) { }
    }

    // [8] System.Formats.Asn1/System.Formats.Asn1.AsnDecoder::ReadGeneralizedTime:System.DateTimeOffset(System.ReadOnlySpan{System.Byte},System.Formats.Asn1.AsnEncodingRules,System.Int32&,System.Nullable{System.Formats.Asn1.Asn1Tag})
    public static void Subject_8()
    {
        try { AsnDecoder.ReadGeneralizedTime(default(ReadOnlySpan<byte>), default, out _, default); _exitCode = 1; }
        catch (AsnContentException) { }
    }

    // [9] System.Formats.Asn1/System.Formats.Asn1.AsnDecoder::ReadInteger:System.Numerics.BigInteger(System.ReadOnlySpan{System.Byte},System.Formats.Asn1.AsnEncodingRules,System.Int32&,System.Nullable{System.Formats.Asn1.Asn1Tag})
    public static void Subject_9()
    {
        try { AsnDecoder.ReadInteger(default(ReadOnlySpan<byte>), default, out _, default); _exitCode = 1; }
        catch (AsnContentException) { }
    }

    // [10] System.Formats.Asn1/System.Formats.Asn1.AsnDecoder::ReadIntegerBytes:System.ReadOnlySpan{System.Byte}(System.ReadOnlySpan{System.Byte},System.Formats.Asn1.AsnEncodingRules,System.Int32&,System.Nullable{System.Formats.Asn1.Asn1Tag})
    public static void Subject_10()
    {
        try { AsnDecoder.ReadIntegerBytes(default(ReadOnlySpan<byte>), default, out _, default); _exitCode = 1; }
        catch (AsnContentException) { }
    }

    // [11] System.Formats.Asn1/System.Formats.Asn1.AsnDecoder::ReadNamedBitList:System.Collections.BitArray(System.ReadOnlySpan{System.Byte},System.Formats.Asn1.AsnEncodingRules,System.Int32&,System.Nullable{System.Formats.Asn1.Asn1Tag})
    public static void Subject_11()
    {
        try { AsnDecoder.ReadNamedBitList(default(ReadOnlySpan<byte>), default, out _, default); _exitCode = 1; }
        catch (AsnContentException) { }
    }

    // [12] System.Formats.Asn1/System.Formats.Asn1.AsnDecoder::ReadNamedBitListValue{TFlagsEnum}:TFlagsEnum(System.ReadOnlySpan{System.Byte},System.Formats.Asn1.AsnEncodingRules,System.Int32&,System.Nullable{System.Formats.Asn1.Asn1Tag})
    public static void CustomEntrySubject_12()
    {
    }

    // [13] System.Formats.Asn1/System.Formats.Asn1.AsnDecoder::ReadNamedBitListValue:System.Enum(System.ReadOnlySpan{System.Byte},System.Formats.Asn1.AsnEncodingRules,System.Type,System.Int32&,System.Nullable{System.Formats.Asn1.Asn1Tag})
    public static void Subject_13()
    {
        try { AsnDecoder.ReadNamedBitListValue(default(ReadOnlySpan<byte>), default, typeof(byte), out _, default); _exitCode = 1; }
        catch (ArgumentException) { }
    }

    // [14] System.Formats.Asn1/System.Formats.Asn1.AsnDecoder::ReadNull:System.Void(System.ReadOnlySpan{System.Byte},System.Formats.Asn1.AsnEncodingRules,System.Int32&,System.Nullable{System.Formats.Asn1.Asn1Tag})
    public static void Subject_14()
    {
        try { AsnDecoder.ReadNull(default(ReadOnlySpan<byte>), default, out _, default); _exitCode = 1; }
        catch (AsnContentException) { }
    }

    // [15] System.Formats.Asn1/System.Formats.Asn1.AsnDecoder::ReadObjectIdentifier:System.String(System.ReadOnlySpan{System.Byte},System.Formats.Asn1.AsnEncodingRules,System.Int32&,System.Nullable{System.Formats.Asn1.Asn1Tag})
    public static void Subject_15()
    {
        try { AsnDecoder.ReadObjectIdentifier(default(ReadOnlySpan<byte>), default, out _, default); _exitCode = 1; }
        catch (AsnContentException) { }
    }

    // [16] System.Formats.Asn1/System.Formats.Asn1.AsnDecoder::ReadOctetString:System.Byte{}(System.ReadOnlySpan{System.Byte},System.Formats.Asn1.AsnEncodingRules,System.Int32&,System.Nullable{System.Formats.Asn1.Asn1Tag})
    public static void Subject_16()
    {
        try { AsnDecoder.ReadOctetString(default(ReadOnlySpan<byte>), default, out _, default); _exitCode = 1; }
        catch (AsnContentException) { }
    }

    // [17] System.Formats.Asn1/System.Formats.Asn1.AsnDecoder::ReadSequence:System.Void(System.ReadOnlySpan{System.Byte},System.Formats.Asn1.AsnEncodingRules,System.Int32&,System.Int32&,System.Int32&,System.Nullable{System.Formats.Asn1.Asn1Tag})
    public static void Subject_17()
    {
        try { AsnDecoder.ReadSequence(default(ReadOnlySpan<byte>), default, out _, out _, out _, default); _exitCode = 1; }
        catch (AsnContentException) { }
    }

    // [18] System.Formats.Asn1/System.Formats.Asn1.AsnDecoder::ReadSetOf:System.Void(System.ReadOnlySpan{System.Byte},System.Formats.Asn1.AsnEncodingRules,System.Int32&,System.Int32&,System.Int32&,System.Boolean,System.Nullable{System.Formats.Asn1.Asn1Tag})
    public static void Subject_18()
    {
        try { AsnDecoder.ReadSetOf(default(ReadOnlySpan<byte>), default, out _, out _, out _, true, default); _exitCode = 1; }
        catch (AsnContentException) { }
    }

    // [19] System.Formats.Asn1/System.Formats.Asn1.AsnDecoder::ReadUtcTime:System.DateTimeOffset(System.ReadOnlySpan{System.Byte},System.Formats.Asn1.AsnEncodingRules,System.Int32&,System.Int32,System.Nullable{System.Formats.Asn1.Asn1Tag})
    public static void Subject_19()
    {
        try { AsnDecoder.ReadUtcTime(default(ReadOnlySpan<byte>), default, out _, 42, default); _exitCode = 1; }
        catch (AsnContentException) { }
    }

    // [20] System.Formats.Asn1/System.Formats.Asn1.AsnDecoder::TryDecodeLength:System.Boolean(System.ReadOnlySpan{System.Byte},System.Formats.Asn1.AsnEncodingRules,System.Nullable{System.Int32}&,System.Int32&)
    public static void Subject_20()
    {
        if (((AsnDecoder.TryDecodeLength(default(ReadOnlySpan<byte>), default, out _, out _)) ? 1 : 0) != 0) _exitCode = 1;
    }

    // [21] System.Formats.Asn1/System.Formats.Asn1.AsnDecoder::TryReadBitString:System.Boolean(System.ReadOnlySpan{System.Byte},System.Span{System.Byte},System.Formats.Asn1.AsnEncodingRules,System.Int32&,System.Int32&,System.Int32&,System.Nullable{System.Formats.Asn1.Asn1Tag})
    public static void Subject_21()
    {
        try { AsnDecoder.TryReadBitString(default(ReadOnlySpan<byte>), default(Span<byte>), default, out _, out _, out _, default); _exitCode = 1; }
        catch (AsnContentException) { }
    }

    // [22] System.Formats.Asn1/System.Formats.Asn1.AsnDecoder::TryReadCharacterString:System.Boolean(System.ReadOnlySpan{System.Byte},System.Span{System.Char},System.Formats.Asn1.AsnEncodingRules,System.Formats.Asn1.UniversalTagNumber,System.Int32&,System.Int32&,System.Nullable{System.Formats.Asn1.Asn1Tag})
    public static void Subject_22()
    {
        try { AsnDecoder.TryReadCharacterString(default(ReadOnlySpan<byte>), default(Span<char>), default, default, out _, out _, default); _exitCode = 1; }
        catch (ArgumentOutOfRangeException) { }
    }

    // [23] System.Formats.Asn1/System.Formats.Asn1.AsnDecoder::TryReadCharacterStringBytes:System.Boolean(System.ReadOnlySpan{System.Byte},System.Span{System.Byte},System.Formats.Asn1.AsnEncodingRules,System.Formats.Asn1.Asn1Tag,System.Int32&,System.Int32&)
    public static void Subject_23()
    {
        try { AsnDecoder.TryReadCharacterStringBytes(default(ReadOnlySpan<byte>), default(Span<byte>), default, default, out _, out _); _exitCode = 1; }
        catch (ArgumentException) { }
    }

    // [24] System.Formats.Asn1/System.Formats.Asn1.AsnDecoder::TryReadEncodedValue:System.Boolean(System.ReadOnlySpan{System.Byte},System.Formats.Asn1.AsnEncodingRules,System.Formats.Asn1.Asn1Tag&,System.Int32&,System.Int32&,System.Int32&)
    public static void Subject_24()
    {
        if (((AsnDecoder.TryReadEncodedValue(default(ReadOnlySpan<byte>), default, out _, out _, out _, out _)) ? 1 : 0) != 0) _exitCode = 1;
    }

    // [25] System.Formats.Asn1/System.Formats.Asn1.AsnDecoder::TryReadInt32:System.Boolean(System.ReadOnlySpan{System.Byte},System.Formats.Asn1.AsnEncodingRules,System.Int32&,System.Int32&,System.Nullable{System.Formats.Asn1.Asn1Tag})
    public static void Subject_25()
    {
        try { AsnDecoder.TryReadInt32(default(ReadOnlySpan<byte>), default, out _, out _, default); _exitCode = 1; }
        catch (AsnContentException) { }
    }

    // [26] System.Formats.Asn1/System.Formats.Asn1.AsnDecoder::TryReadInt64:System.Boolean(System.ReadOnlySpan{System.Byte},System.Formats.Asn1.AsnEncodingRules,System.Int64&,System.Int32&,System.Nullable{System.Formats.Asn1.Asn1Tag})
    public static void Subject_26()
    {
        try { AsnDecoder.TryReadInt64(default(ReadOnlySpan<byte>), default, out _, out _, default); _exitCode = 1; }
        catch (AsnContentException) { }
    }

    // [27] System.Formats.Asn1/System.Formats.Asn1.AsnDecoder::TryReadOctetString:System.Boolean(System.ReadOnlySpan{System.Byte},System.Span{System.Byte},System.Formats.Asn1.AsnEncodingRules,System.Int32&,System.Int32&,System.Nullable{System.Formats.Asn1.Asn1Tag})
    public static void Subject_27()
    {
        try { AsnDecoder.TryReadOctetString(default(ReadOnlySpan<byte>), default(Span<byte>), default, out _, out _, default); _exitCode = 1; }
        catch (AsnContentException) { }
    }

    // [28] System.Formats.Asn1/System.Formats.Asn1.AsnDecoder::TryReadPrimitiveBitString:System.Boolean(System.ReadOnlySpan{System.Byte},System.Formats.Asn1.AsnEncodingRules,System.Int32&,System.ReadOnlySpan{System.Byte}&,System.Int32&,System.Nullable{System.Formats.Asn1.Asn1Tag})
    public static void Subject_28()
    {
        try { AsnDecoder.TryReadPrimitiveBitString(default(ReadOnlySpan<byte>), default, out _, out _, out _, default); _exitCode = 1; }
        catch (AsnContentException) { }
    }

    // [29] System.Formats.Asn1/System.Formats.Asn1.AsnDecoder::TryReadPrimitiveCharacterStringBytes:System.Boolean(System.ReadOnlySpan{System.Byte},System.Formats.Asn1.AsnEncodingRules,System.Formats.Asn1.Asn1Tag,System.ReadOnlySpan{System.Byte}&,System.Int32&)
    public static void Subject_29()
    {
        try { AsnDecoder.TryReadPrimitiveCharacterStringBytes(default(ReadOnlySpan<byte>), default, default, out _, out _); _exitCode = 1; }
        catch (ArgumentException) { }
    }

    // [30] System.Formats.Asn1/System.Formats.Asn1.AsnDecoder::TryReadPrimitiveOctetString:System.Boolean(System.ReadOnlySpan{System.Byte},System.Formats.Asn1.AsnEncodingRules,System.ReadOnlySpan{System.Byte}&,System.Int32&,System.Nullable{System.Formats.Asn1.Asn1Tag})
    public static void Subject_30()
    {
        try { AsnDecoder.TryReadPrimitiveOctetString(default(ReadOnlySpan<byte>), default, out _, out _, default); _exitCode = 1; }
        catch (AsnContentException) { }
    }

    // [31] System.Formats.Asn1/System.Formats.Asn1.AsnDecoder::TryReadUInt32:System.Boolean(System.ReadOnlySpan{System.Byte},System.Formats.Asn1.AsnEncodingRules,System.UInt32&,System.Int32&,System.Nullable{System.Formats.Asn1.Asn1Tag})
    public static void Subject_31()
    {
        try { AsnDecoder.TryReadUInt32(default(ReadOnlySpan<byte>), default, out _, out _, default); _exitCode = 1; }
        catch (AsnContentException) { }
    }

    // [32] System.Formats.Asn1/System.Formats.Asn1.AsnDecoder::TryReadUInt64:System.Boolean(System.ReadOnlySpan{System.Byte},System.Formats.Asn1.AsnEncodingRules,System.UInt64&,System.Int32&,System.Nullable{System.Formats.Asn1.Asn1Tag})
    public static void Subject_32()
    {
        try { AsnDecoder.TryReadUInt64(default(ReadOnlySpan<byte>), default, out _, out _, default); _exitCode = 1; }
        catch (AsnContentException) { }
    }

}