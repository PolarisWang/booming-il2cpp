// Auto-generated native-AOT entry point
// Family: family/System.Formats.Asn1/reader
// Assembly: System.Formats.Asn1
// Variant: subjects

using System;
using System.Collections;
using System.Collections.Generic;
using System.Formats.Asn1;
using System.Linq;
using System.Numerics;

public static partial class ReaderSubjects
{
    // Inlined exit code — avoids SDK method call resolution in codegen
    public static int _exitCode;

    // [0] System.Formats.Asn1/System.Formats.Asn1.AsnReader::Clone:System.Formats.Asn1.AsnReader()
    public static void Subject_0()
    {
        try { if (((default(AsnReader)!.Clone()).GetHashCode()) != ((default(AsnReader)!.Clone()).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [1] System.Formats.Asn1/System.Formats.Asn1.AsnReader::get_HasData:System.Boolean()
    public static void Subject_1()
    {
        try { if (((default(AsnReader)!.HasData) ? 1 : 0) != ((default(AsnReader)!.HasData) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [2] System.Formats.Asn1/System.Formats.Asn1.AsnReader::get_RuleSet:System.Formats.Asn1.AsnEncodingRules()
    public static void Subject_2()
    {
        try { if (((default(AsnReader)!.RuleSet).GetHashCode()) != ((default(AsnReader)!.RuleSet).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [3] System.Formats.Asn1/System.Formats.Asn1.AsnReader::PeekContentBytes:System.ReadOnlyMemory{System.Byte}()
    public static void Subject_3()
    {
        try { if (((default(AsnReader)!.PeekContentBytes()).GetHashCode()) != ((default(AsnReader)!.PeekContentBytes()).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [4] System.Formats.Asn1/System.Formats.Asn1.AsnReader::PeekEncodedValue:System.ReadOnlyMemory{System.Byte}()
    public static void Subject_4()
    {
        try { if (((default(AsnReader)!.PeekEncodedValue()).GetHashCode()) != ((default(AsnReader)!.PeekEncodedValue()).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [5] System.Formats.Asn1/System.Formats.Asn1.AsnReader::PeekTag:System.Formats.Asn1.Asn1Tag()
    public static void Subject_5()
    {
        try { if (((default(AsnReader)!.PeekTag()).GetHashCode()) != ((default(AsnReader)!.PeekTag()).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [6] System.Formats.Asn1/System.Formats.Asn1.AsnReader::ReadBitString:System.Byte{}(System.Int32&,System.Nullable{System.Formats.Asn1.Asn1Tag})
    public static void Subject_6()
    {
        try { if (((default(AsnReader)!.ReadBitString(out _, default)).Length) != ((default(AsnReader)!.ReadBitString(out _, default)).Length)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [7] System.Formats.Asn1/System.Formats.Asn1.AsnReader::ReadBoolean:System.Boolean(System.Nullable{System.Formats.Asn1.Asn1Tag})
    public static void Subject_7()
    {
        try { if (((default(AsnReader)!.ReadBoolean(default)) ? 1 : 0) != ((default(AsnReader)!.ReadBoolean(default)) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [8] System.Formats.Asn1/System.Formats.Asn1.AsnReader::ReadCharacterString:System.String(System.Formats.Asn1.UniversalTagNumber,System.Nullable{System.Formats.Asn1.Asn1Tag})
    public static void Subject_8()
    {
        try { if (((default(AsnReader)!.ReadCharacterString(default, default)).Length) != ((default(AsnReader)!.ReadCharacterString(default, default)).Length)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [9] System.Formats.Asn1/System.Formats.Asn1.AsnReader::ReadEncodedValue:System.ReadOnlyMemory{System.Byte}()
    public static void Subject_9()
    {
        try { if (((default(AsnReader)!.ReadEncodedValue()).GetHashCode()) != ((default(AsnReader)!.ReadEncodedValue()).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [10] System.Formats.Asn1/System.Formats.Asn1.AsnReader::ReadEnumeratedBytes:System.ReadOnlyMemory{System.Byte}(System.Nullable{System.Formats.Asn1.Asn1Tag})
    public static void Subject_10()
    {
        try { if (((default(AsnReader)!.ReadEnumeratedBytes(default)).GetHashCode()) != ((default(AsnReader)!.ReadEnumeratedBytes(default)).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [11] System.Formats.Asn1/System.Formats.Asn1.AsnReader::ReadEnumeratedValue{TEnum}:TEnum(System.Nullable{System.Formats.Asn1.Asn1Tag})
    public static void CustomEntrySubject_11()
    {
    }

    // [12] System.Formats.Asn1/System.Formats.Asn1.AsnReader::ReadEnumeratedValue:System.Enum(System.Type,System.Nullable{System.Formats.Asn1.Asn1Tag})
    public static void Subject_12()
    {
        try { if (((default(AsnReader)!.ReadEnumeratedValue(typeof(byte), default)).GetHashCode()) != ((default(AsnReader)!.ReadEnumeratedValue(typeof(byte), default)).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [13] System.Formats.Asn1/System.Formats.Asn1.AsnReader::ReadGeneralizedTime:System.DateTimeOffset(System.Nullable{System.Formats.Asn1.Asn1Tag})
    public static void Subject_13()
    {
        try { if (((default(AsnReader)!.ReadGeneralizedTime(default)).GetHashCode()) != ((default(AsnReader)!.ReadGeneralizedTime(default)).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [14] System.Formats.Asn1/System.Formats.Asn1.AsnReader::ReadInteger:System.Numerics.BigInteger(System.Nullable{System.Formats.Asn1.Asn1Tag})
    public static void Subject_14()
    {
        try { if (((default(AsnReader)!.ReadInteger(default)).GetHashCode()) != ((default(AsnReader)!.ReadInteger(default)).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [15] System.Formats.Asn1/System.Formats.Asn1.AsnReader::ReadIntegerBytes:System.ReadOnlyMemory{System.Byte}(System.Nullable{System.Formats.Asn1.Asn1Tag})
    public static void Subject_15()
    {
        try { if (((default(AsnReader)!.ReadIntegerBytes(default)).GetHashCode()) != ((default(AsnReader)!.ReadIntegerBytes(default)).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [16] System.Formats.Asn1/System.Formats.Asn1.AsnReader::ReadNamedBitList:System.Collections.BitArray(System.Nullable{System.Formats.Asn1.Asn1Tag})
    public static void Subject_16()
    {
        try { if (((default(AsnReader)!.ReadNamedBitList(default)).GetHashCode()) != ((default(AsnReader)!.ReadNamedBitList(default)).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [17] System.Formats.Asn1/System.Formats.Asn1.AsnReader::ReadNamedBitListValue{TFlagsEnum}:TFlagsEnum(System.Nullable{System.Formats.Asn1.Asn1Tag})
    public static void CustomEntrySubject_17()
    {
    }

    // [18] System.Formats.Asn1/System.Formats.Asn1.AsnReader::ReadNamedBitListValue:System.Enum(System.Type,System.Nullable{System.Formats.Asn1.Asn1Tag})
    public static void Subject_18()
    {
        try { if (((default(AsnReader)!.ReadNamedBitListValue(typeof(byte), default)).GetHashCode()) != ((default(AsnReader)!.ReadNamedBitListValue(typeof(byte), default)).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [19] System.Formats.Asn1/System.Formats.Asn1.AsnReader::ReadNull:System.Void(System.Nullable{System.Formats.Asn1.Asn1Tag})
    public static void Subject_19()
    {
        try { default(AsnReader)!.ReadNull(default); }
        catch { _exitCode = 1; }
    }

    // [20] System.Formats.Asn1/System.Formats.Asn1.AsnReader::ReadObjectIdentifier:System.String(System.Nullable{System.Formats.Asn1.Asn1Tag})
    public static void Subject_20()
    {
        try { if (((default(AsnReader)!.ReadObjectIdentifier(default)).Length) != ((default(AsnReader)!.ReadObjectIdentifier(default)).Length)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [21] System.Formats.Asn1/System.Formats.Asn1.AsnReader::ReadOctetString:System.Byte{}(System.Nullable{System.Formats.Asn1.Asn1Tag})
    public static void Subject_21()
    {
        try { if (((default(AsnReader)!.ReadOctetString(default)).Length) != ((default(AsnReader)!.ReadOctetString(default)).Length)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [22] System.Formats.Asn1/System.Formats.Asn1.AsnReader::ReadSequence:System.Formats.Asn1.AsnReader(System.Nullable{System.Formats.Asn1.Asn1Tag})
    public static void Subject_22()
    {
        try { if (((default(AsnReader)!.ReadSequence(default)).GetHashCode()) != ((default(AsnReader)!.ReadSequence(default)).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [23] System.Formats.Asn1/System.Formats.Asn1.AsnReader::ReadSetOf:System.Formats.Asn1.AsnReader(System.Nullable{System.Formats.Asn1.Asn1Tag})
    public static void Subject_23()
    {
        try { if (((default(AsnReader)!.ReadSetOf(default)).GetHashCode()) != ((default(AsnReader)!.ReadSetOf(default)).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [24] System.Formats.Asn1/System.Formats.Asn1.AsnReader::ReadSetOf:System.Formats.Asn1.AsnReader(System.Boolean,System.Nullable{System.Formats.Asn1.Asn1Tag})
    public static void Subject_24()
    {
        try { if (((default(AsnReader)!.ReadSetOf(true, default)).GetHashCode()) != ((default(AsnReader)!.ReadSetOf(true, default)).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [25] System.Formats.Asn1/System.Formats.Asn1.AsnReader::ReadUtcTime:System.DateTimeOffset(System.Nullable{System.Formats.Asn1.Asn1Tag})
    public static void Subject_25()
    {
        try { if (((default(AsnReader)!.ReadUtcTime(default)).GetHashCode()) != ((default(AsnReader)!.ReadUtcTime(default)).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [26] System.Formats.Asn1/System.Formats.Asn1.AsnReader::ReadUtcTime:System.DateTimeOffset(System.Int32,System.Nullable{System.Formats.Asn1.Asn1Tag})
    public static void Subject_26()
    {
        try { if (((default(AsnReader)!.ReadUtcTime(42, default)).GetHashCode()) != ((default(AsnReader)!.ReadUtcTime(42, default)).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [27] System.Formats.Asn1/System.Formats.Asn1.AsnReader::ThrowIfNotEmpty:System.Void()
    public static void Subject_27()
    {
        try { default(AsnReader)!.ThrowIfNotEmpty(); }
        catch { _exitCode = 1; }
    }

    // [28] System.Formats.Asn1/System.Formats.Asn1.AsnReader::TryReadBitString:System.Boolean(System.Span{System.Byte},System.Int32&,System.Int32&,System.Nullable{System.Formats.Asn1.Asn1Tag})
    public static void Subject_28()
    {
        try { if (((default(AsnReader)!.TryReadBitString(default(Span<byte>), out _, out _, default)) ? 1 : 0) != ((default(AsnReader)!.TryReadBitString(default(Span<byte>), out _, out _, default)) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [29] System.Formats.Asn1/System.Formats.Asn1.AsnReader::TryReadCharacterString:System.Boolean(System.Span{System.Char},System.Formats.Asn1.UniversalTagNumber,System.Int32&,System.Nullable{System.Formats.Asn1.Asn1Tag})
    public static void Subject_29()
    {
        try { if (((default(AsnReader)!.TryReadCharacterString(default(Span<char>), default, out _, default)) ? 1 : 0) != ((default(AsnReader)!.TryReadCharacterString(default(Span<char>), default, out _, default)) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [30] System.Formats.Asn1/System.Formats.Asn1.AsnReader::TryReadCharacterStringBytes:System.Boolean(System.Span{System.Byte},System.Formats.Asn1.Asn1Tag,System.Int32&)
    public static void Subject_30()
    {
        try { if (((default(AsnReader)!.TryReadCharacterStringBytes(default(Span<byte>), default, out _)) ? 1 : 0) != ((default(AsnReader)!.TryReadCharacterStringBytes(default(Span<byte>), default, out _)) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [31] System.Formats.Asn1/System.Formats.Asn1.AsnReader::TryReadInt32:System.Boolean(System.Int32&,System.Nullable{System.Formats.Asn1.Asn1Tag})
    public static void Subject_31()
    {
        try { if (((default(AsnReader)!.TryReadInt32(out _, default)) ? 1 : 0) != ((default(AsnReader)!.TryReadInt32(out _, default)) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [32] System.Formats.Asn1/System.Formats.Asn1.AsnReader::TryReadInt64:System.Boolean(System.Int64&,System.Nullable{System.Formats.Asn1.Asn1Tag})
    public static void Subject_32()
    {
        try { if (((default(AsnReader)!.TryReadInt64(out _, default)) ? 1 : 0) != ((default(AsnReader)!.TryReadInt64(out _, default)) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [33] System.Formats.Asn1/System.Formats.Asn1.AsnReader::TryReadOctetString:System.Boolean(System.Span{System.Byte},System.Int32&,System.Nullable{System.Formats.Asn1.Asn1Tag})
    public static void Subject_33()
    {
        try { if (((default(AsnReader)!.TryReadOctetString(default(Span<byte>), out _, default)) ? 1 : 0) != ((default(AsnReader)!.TryReadOctetString(default(Span<byte>), out _, default)) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [34] System.Formats.Asn1/System.Formats.Asn1.AsnReader::TryReadPrimitiveBitString:System.Boolean(System.Int32&,System.ReadOnlyMemory{System.Byte}&,System.Nullable{System.Formats.Asn1.Asn1Tag})
    public static void Subject_34()
    {
        try { if (((default(AsnReader)!.TryReadPrimitiveBitString(out _, out _, default)) ? 1 : 0) != ((default(AsnReader)!.TryReadPrimitiveBitString(out _, out _, default)) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [35] System.Formats.Asn1/System.Formats.Asn1.AsnReader::TryReadPrimitiveCharacterStringBytes:System.Boolean(System.Formats.Asn1.Asn1Tag,System.ReadOnlyMemory{System.Byte}&)
    public static void Subject_35()
    {
        try { if (((default(AsnReader)!.TryReadPrimitiveCharacterStringBytes(default, out _)) ? 1 : 0) != ((default(AsnReader)!.TryReadPrimitiveCharacterStringBytes(default, out _)) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [36] System.Formats.Asn1/System.Formats.Asn1.AsnReader::TryReadPrimitiveOctetString:System.Boolean(System.ReadOnlyMemory{System.Byte}&,System.Nullable{System.Formats.Asn1.Asn1Tag})
    public static void Subject_36()
    {
        try { if (((default(AsnReader)!.TryReadPrimitiveOctetString(out _, default)) ? 1 : 0) != ((default(AsnReader)!.TryReadPrimitiveOctetString(out _, default)) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [37] System.Formats.Asn1/System.Formats.Asn1.AsnReader::TryReadUInt32:System.Boolean(System.UInt32&,System.Nullable{System.Formats.Asn1.Asn1Tag})
    public static void Subject_37()
    {
        try { if (((default(AsnReader)!.TryReadUInt32(out _, default)) ? 1 : 0) != ((default(AsnReader)!.TryReadUInt32(out _, default)) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [38] System.Formats.Asn1/System.Formats.Asn1.AsnReader::TryReadUInt64:System.Boolean(System.UInt64&,System.Nullable{System.Formats.Asn1.Asn1Tag})
    public static void Subject_38()
    {
        try { if (((default(AsnReader)!.TryReadUInt64(out _, default)) ? 1 : 0) != ((default(AsnReader)!.TryReadUInt64(out _, default)) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [39] System.Formats.Asn1/System.Formats.Asn1.AsnReader::.ctor:System.Void(System.ReadOnlyMemory{System.Byte},System.Formats.Asn1.AsnEncodingRules,System.Formats.Asn1.AsnReaderOptions)
    public static void Subject_39()
    {
        try { new AsnReader(default(ReadOnlyMemory<byte>), default, default); }
        catch { _exitCode = 1; }
    }

}