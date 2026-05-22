// Auto-generated native-AOT entry point
// Family: family/System.Formats.Asn1/writer
// Assembly: System.Formats.Asn1
// Variant: subjects

using System;
using System.Collections;
using System.Collections.Generic;
using System.Formats.Asn1;
using System.Linq;
using System.Numerics;

public static partial class WriterSubjects
{
    // Inlined exit code — avoids SDK method call resolution in codegen
    public static int _exitCode;

    // [0] System.Formats.Asn1/System.Formats.Asn1.AsnWriter::CopyTo:System.Void(System.Formats.Asn1.AsnWriter)
    public static void Subject_0()
    {
        try { default(AsnWriter)!.CopyTo(default); }
        catch { _exitCode = 1; }
    }

    // [1] System.Formats.Asn1/System.Formats.Asn1.AsnWriter::Encode:System.Int32(System.Span{System.Byte})
    public static void Subject_1()
    {
        try { if (default(AsnWriter)!.Encode(default(Span<byte>)) != default(AsnWriter)!.Encode(default(Span<byte>))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [2] System.Formats.Asn1/System.Formats.Asn1.AsnWriter::Encode:System.Byte{}()
    public static void Subject_2()
    {
        try { if (((default(AsnWriter)!.Encode()).Length) != ((default(AsnWriter)!.Encode()).Length)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [3] System.Formats.Asn1/System.Formats.Asn1.AsnWriter::Encode{TReturn}:TReturn(System.Func{System.ReadOnlySpan{System.Byte},TReturn})
    public static void CustomEntrySubject_3()
    {
    }

    // [4] System.Formats.Asn1/System.Formats.Asn1.AsnWriter::Encode{TState,TReturn}:TReturn(TState,System.Func{TState,System.ReadOnlySpan{System.Byte},TReturn})
    public static void CustomEntrySubject_4()
    {
    }

    // [5] System.Formats.Asn1/System.Formats.Asn1.AsnWriter::Encode{TState}:System.Void(TState,System.Action{TState,System.ReadOnlySpan{System.Byte}})
    public static void CustomEntrySubject_5()
    {
    }

    // [6] System.Formats.Asn1/System.Formats.Asn1.AsnWriter::EncodedValueEquals:System.Boolean(System.ReadOnlySpan{System.Byte})
    public static void Subject_6()
    {
        try { if (((default(AsnWriter)!.EncodedValueEquals(default(AsnWriter)!)) ? 1 : 0) != ((default(AsnWriter)!.EncodedValueEquals(default(AsnWriter)!)) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [7] System.Formats.Asn1/System.Formats.Asn1.AsnWriter::EncodedValueEquals:System.Boolean(System.Formats.Asn1.AsnWriter)
    public static void Subject_7()
    {
        try { if (((default(AsnWriter)!.EncodedValueEquals(default(AsnWriter)!)) ? 1 : 0) != ((default(AsnWriter)!.EncodedValueEquals(default(AsnWriter)!)) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [8] System.Formats.Asn1/System.Formats.Asn1.AsnWriter::get_RuleSet:System.Formats.Asn1.AsnEncodingRules()
    public static void Subject_8()
    {
        try { if (((default(AsnWriter)!.RuleSet).GetHashCode()) != ((default(AsnWriter)!.RuleSet).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [9] System.Formats.Asn1/System.Formats.Asn1.AsnWriter::GetEncodedLength:System.Int32()
    public static void Subject_9()
    {
        try { if (default(AsnWriter)!.GetEncodedLength() != default(AsnWriter)!.GetEncodedLength()) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [10] System.Formats.Asn1/System.Formats.Asn1.AsnWriter::PopOctetString:System.Void(System.Nullable{System.Formats.Asn1.Asn1Tag})
    public static void Subject_10()
    {
        try { default(AsnWriter)!.PopOctetString(default); }
        catch { _exitCode = 1; }
    }

    // [11] System.Formats.Asn1/System.Formats.Asn1.AsnWriter::PopSequence:System.Void(System.Nullable{System.Formats.Asn1.Asn1Tag})
    public static void Subject_11()
    {
        try { default(AsnWriter)!.PopSequence(default); }
        catch { _exitCode = 1; }
    }

    // [12] System.Formats.Asn1/System.Formats.Asn1.AsnWriter::PopSetOf:System.Void(System.Nullable{System.Formats.Asn1.Asn1Tag})
    public static void Subject_12()
    {
        try { default(AsnWriter)!.PopSetOf(default); }
        catch { _exitCode = 1; }
    }

    // [13] System.Formats.Asn1/System.Formats.Asn1.AsnWriter::PushOctetString:System.Formats.Asn1.AsnWriter+Scope(System.Nullable{System.Formats.Asn1.Asn1Tag})
    public static void Subject_13()
    {
        try { if (((default(AsnWriter)!.PushOctetString(default)).GetHashCode()) != ((default(AsnWriter)!.PushOctetString(default)).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [14] System.Formats.Asn1/System.Formats.Asn1.AsnWriter::PushSequence:System.Formats.Asn1.AsnWriter+Scope(System.Nullable{System.Formats.Asn1.Asn1Tag})
    public static void Subject_14()
    {
        try { if (((default(AsnWriter)!.PushSequence(default)).GetHashCode()) != ((default(AsnWriter)!.PushSequence(default)).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [15] System.Formats.Asn1/System.Formats.Asn1.AsnWriter::PushSetOf:System.Formats.Asn1.AsnWriter+Scope(System.Nullable{System.Formats.Asn1.Asn1Tag})
    public static void Subject_15()
    {
        try { if (((default(AsnWriter)!.PushSetOf(default)).GetHashCode()) != ((default(AsnWriter)!.PushSetOf(default)).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [16] System.Formats.Asn1/System.Formats.Asn1.AsnWriter::Reset:System.Void()
    public static void Subject_16()
    {
        try { default(AsnWriter)!.Reset(); }
        catch { _exitCode = 1; }
    }

    // [17] System.Formats.Asn1/System.Formats.Asn1.AsnWriter::TryEncode:System.Boolean(System.Span{System.Byte},System.Int32&)
    public static void Subject_17()
    {
        try { if (((default(AsnWriter)!.TryEncode(default(Span<byte>), out _)) ? 1 : 0) != ((default(AsnWriter)!.TryEncode(default(Span<byte>), out _)) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [18] System.Formats.Asn1/System.Formats.Asn1.AsnWriter::WriteBitString:System.Void(System.ReadOnlySpan{System.Byte},System.Int32,System.Nullable{System.Formats.Asn1.Asn1Tag})
    public static void Subject_18()
    {
        try { default(AsnWriter)!.WriteBitString(default(ReadOnlySpan<byte>), 42, default); }
        catch { _exitCode = 1; }
    }

    // [19] System.Formats.Asn1/System.Formats.Asn1.AsnWriter::WriteBoolean:System.Void(System.Boolean,System.Nullable{System.Formats.Asn1.Asn1Tag})
    public static void Subject_19()
    {
        try { default(AsnWriter)!.WriteBoolean(true, default); }
        catch { _exitCode = 1; }
    }

    // [20] System.Formats.Asn1/System.Formats.Asn1.AsnWriter::WriteCharacterString:System.Void(System.Formats.Asn1.UniversalTagNumber,System.String,System.Nullable{System.Formats.Asn1.Asn1Tag})
    public static void Subject_20()
    {
        try { default(AsnWriter)!.WriteCharacterString(default, "hello", default); }
        catch { _exitCode = 1; }
    }

    // [21] System.Formats.Asn1/System.Formats.Asn1.AsnWriter::WriteCharacterString:System.Void(System.Formats.Asn1.UniversalTagNumber,System.ReadOnlySpan{System.Char},System.Nullable{System.Formats.Asn1.Asn1Tag})
    public static void Subject_21()
    {
        try { default(AsnWriter)!.WriteCharacterString(default, default(ReadOnlySpan<char>), default); }
        catch { _exitCode = 1; }
    }

    // [22] System.Formats.Asn1/System.Formats.Asn1.AsnWriter::WriteEncodedValue:System.Void(System.ReadOnlySpan{System.Byte})
    public static void Subject_22()
    {
        try { default(AsnWriter)!.WriteEncodedValue(default(ReadOnlySpan<byte>)); }
        catch { _exitCode = 1; }
    }

    // [23] System.Formats.Asn1/System.Formats.Asn1.AsnWriter::WriteEnumeratedValue:System.Void(System.Enum,System.Nullable{System.Formats.Asn1.Asn1Tag})
    public static void Subject_23()
    {
        try { default(AsnWriter)!.WriteEnumeratedValue(DayOfWeek.Monday, default); }
        catch { _exitCode = 1; }
    }

    // [24] System.Formats.Asn1/System.Formats.Asn1.AsnWriter::WriteEnumeratedValue{TEnum}:System.Void(TEnum,System.Nullable{System.Formats.Asn1.Asn1Tag})
    public static void CustomEntrySubject_24()
    {
    }

    // [25] System.Formats.Asn1/System.Formats.Asn1.AsnWriter::WriteGeneralizedTime:System.Void(System.DateTimeOffset,System.Boolean,System.Nullable{System.Formats.Asn1.Asn1Tag})
    public static void Subject_25()
    {
        try { default(AsnWriter)!.WriteGeneralizedTime(default, true, default); }
        catch { _exitCode = 1; }
    }

    // [26] System.Formats.Asn1/System.Formats.Asn1.AsnWriter::WriteInteger:System.Void(System.Int64,System.Nullable{System.Formats.Asn1.Asn1Tag})
    public static void Subject_26()
    {
        try { default(AsnWriter)!.WriteInteger(42L, default); }
        catch { _exitCode = 1; }
    }

    // [27] System.Formats.Asn1/System.Formats.Asn1.AsnWriter::WriteInteger:System.Void(System.UInt64,System.Nullable{System.Formats.Asn1.Asn1Tag})
    public static void Subject_27()
    {
        try { default(AsnWriter)!.WriteInteger(42L, default); }
        catch { _exitCode = 1; }
    }

    // [28] System.Formats.Asn1/System.Formats.Asn1.AsnWriter::WriteInteger:System.Void(System.Numerics.BigInteger,System.Nullable{System.Formats.Asn1.Asn1Tag})
    public static void Subject_28()
    {
        try { default(AsnWriter)!.WriteInteger(42L, default); }
        catch { _exitCode = 1; }
    }

    // [29] System.Formats.Asn1/System.Formats.Asn1.AsnWriter::WriteInteger:System.Void(System.ReadOnlySpan{System.Byte},System.Nullable{System.Formats.Asn1.Asn1Tag})
    public static void Subject_29()
    {
        try { default(AsnWriter)!.WriteInteger(42L, default); }
        catch { _exitCode = 1; }
    }

    // [30] System.Formats.Asn1/System.Formats.Asn1.AsnWriter::WriteIntegerUnsigned:System.Void(System.ReadOnlySpan{System.Byte},System.Nullable{System.Formats.Asn1.Asn1Tag})
    public static void Subject_30()
    {
        try { default(AsnWriter)!.WriteIntegerUnsigned(default(ReadOnlySpan<byte>), default); }
        catch { _exitCode = 1; }
    }

    // [31] System.Formats.Asn1/System.Formats.Asn1.AsnWriter::WriteNamedBitList:System.Void(System.Enum,System.Nullable{System.Formats.Asn1.Asn1Tag})
    public static void Subject_31()
    {
        try { default(AsnWriter)!.WriteNamedBitList(DayOfWeek.Monday, default); }
        catch { _exitCode = 1; }
    }

    // [32] System.Formats.Asn1/System.Formats.Asn1.AsnWriter::WriteNamedBitList{TEnum}:System.Void(TEnum,System.Nullable{System.Formats.Asn1.Asn1Tag})
    public static void CustomEntrySubject_32()
    {
    }

    // [33] System.Formats.Asn1/System.Formats.Asn1.AsnWriter::WriteNamedBitList:System.Void(System.Collections.BitArray,System.Nullable{System.Formats.Asn1.Asn1Tag})
    public static void Subject_33()
    {
        try { default(AsnWriter)!.WriteNamedBitList(DayOfWeek.Monday, default); }
        catch { _exitCode = 1; }
    }

    // [34] System.Formats.Asn1/System.Formats.Asn1.AsnWriter::WriteNull:System.Void(System.Nullable{System.Formats.Asn1.Asn1Tag})
    public static void Subject_34()
    {
        try { default(AsnWriter)!.WriteNull(default); }
        catch { _exitCode = 1; }
    }

    // [35] System.Formats.Asn1/System.Formats.Asn1.AsnWriter::WriteObjectIdentifier:System.Void(System.String,System.Nullable{System.Formats.Asn1.Asn1Tag})
    public static void Subject_35()
    {
        try { default(AsnWriter)!.WriteObjectIdentifier("hello", default); }
        catch { _exitCode = 1; }
    }

    // [36] System.Formats.Asn1/System.Formats.Asn1.AsnWriter::WriteObjectIdentifier:System.Void(System.ReadOnlySpan{System.Char},System.Nullable{System.Formats.Asn1.Asn1Tag})
    public static void Subject_36()
    {
        try { default(AsnWriter)!.WriteObjectIdentifier(default(ReadOnlySpan<char>), default); }
        catch { _exitCode = 1; }
    }

    // [37] System.Formats.Asn1/System.Formats.Asn1.AsnWriter::WriteOctetString:System.Void(System.ReadOnlySpan{System.Byte},System.Nullable{System.Formats.Asn1.Asn1Tag})
    public static void Subject_37()
    {
        try { default(AsnWriter)!.WriteOctetString(default(ReadOnlySpan<byte>), default); }
        catch { _exitCode = 1; }
    }

    // [38] System.Formats.Asn1/System.Formats.Asn1.AsnWriter::WriteUtcTime:System.Void(System.DateTimeOffset,System.Nullable{System.Formats.Asn1.Asn1Tag})
    public static void Subject_38()
    {
        try { default(AsnWriter)!.WriteUtcTime(default, default); }
        catch { _exitCode = 1; }
    }

    // [39] System.Formats.Asn1/System.Formats.Asn1.AsnWriter::WriteUtcTime:System.Void(System.DateTimeOffset,System.Int32,System.Nullable{System.Formats.Asn1.Asn1Tag})
    public static void Subject_39()
    {
        try { default(AsnWriter)!.WriteUtcTime(default, 42, default); }
        catch { _exitCode = 1; }
    }

    // [40] System.Formats.Asn1/System.Formats.Asn1.AsnWriter::.ctor:System.Void(System.Formats.Asn1.AsnEncodingRules)
    public static void Subject_40()
    {
        try { new AsnWriter(default); }
        catch { _exitCode = 1; }
    }

    // [41] System.Formats.Asn1/System.Formats.Asn1.AsnWriter::.ctor:System.Void(System.Formats.Asn1.AsnEncodingRules,System.Int32)
    public static void Subject_41()
    {
        try { new AsnWriter(default, 42); }
        catch { _exitCode = 1; }
    }

}