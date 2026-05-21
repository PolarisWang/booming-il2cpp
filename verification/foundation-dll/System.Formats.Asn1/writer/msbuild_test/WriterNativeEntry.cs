// Auto-generated native-AOT entry point
// Family: family/System.Formats.Asn1/writer
// Assembly: System.Formats.Asn1
// Variant: benchmark

using System;
using System.Collections;
using System.Collections.Generic;
using System.Formats.Asn1;
using System.Linq;
using System.Numerics;

public static partial class WriterNativeEntry
{
    // Inlined exit code — avoids SDK method call resolution in codegen
    public static int _exitCode;

    // [0] System.Formats.Asn1/System.Formats.Asn1.AsnWriter::CopyTo:System.Void(System.Formats.Asn1.AsnWriter)
    public static void Method0()
    {
        try { AsnWriter.CopyTo(null!); }
        catch { _exitCode = 1; }
    }

    // [1] System.Formats.Asn1/System.Formats.Asn1.AsnWriter::Encode:System.Int32(System.Span{System.Byte})
    public static void Method1()
    {
        try { if (AsnWriter.Encode(null!) != AsnWriter.Encode(null!)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [2] System.Formats.Asn1/System.Formats.Asn1.AsnWriter::Encode:System.Byte{}()
    public static void Method2()
    {
        try { if ((int)(AsnWriter.Encode()) != (int)(AsnWriter.Encode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [3] System.Formats.Asn1/System.Formats.Asn1.AsnWriter::Encode{TReturn}:TReturn(System.Func{System.ReadOnlySpan{System.Byte},TReturn})

    // [4] System.Formats.Asn1/System.Formats.Asn1.AsnWriter::Encode{TState,TReturn}:TReturn(TState,System.Func{TState,System.ReadOnlySpan{System.Byte},TReturn})

    // [5] System.Formats.Asn1/System.Formats.Asn1.AsnWriter::Encode{TState}:System.Void(TState,System.Action{TState,System.ReadOnlySpan{System.Byte}})

    // [6] System.Formats.Asn1/System.Formats.Asn1.AsnWriter::EncodedValueEquals:System.Boolean(System.ReadOnlySpan{System.Byte})
    public static void Method6()
    {
        try { if (((AsnWriter.EncodedValueEquals(null!)) ? 1 : 0) != ((AsnWriter.EncodedValueEquals(null!)) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [7] System.Formats.Asn1/System.Formats.Asn1.AsnWriter::EncodedValueEquals:System.Boolean(System.Formats.Asn1.AsnWriter)
    public static void Method7()
    {
        try { if (((AsnWriter.EncodedValueEquals(null!)) ? 1 : 0) != ((AsnWriter.EncodedValueEquals(null!)) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [8] System.Formats.Asn1/System.Formats.Asn1.AsnWriter::get_RuleSet:System.Formats.Asn1.AsnEncodingRules()
    public static void Method8()
    {
        try { if ((int)(AsnWriter.RuleSet) != (int)(AsnWriter.RuleSet)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [9] System.Formats.Asn1/System.Formats.Asn1.AsnWriter::GetEncodedLength:System.Int32()
    public static void Method9()
    {
        try { if (AsnWriter.GetEncodedLength() != AsnWriter.GetEncodedLength()) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [10] System.Formats.Asn1/System.Formats.Asn1.AsnWriter::PopOctetString:System.Void(System.Nullable{System.Formats.Asn1.Asn1Tag})
    public static void Method10()
    {
        try { AsnWriter.PopOctetString(null!); }
        catch { _exitCode = 1; }
    }

    // [11] System.Formats.Asn1/System.Formats.Asn1.AsnWriter::PopSequence:System.Void(System.Nullable{System.Formats.Asn1.Asn1Tag})
    public static void Method11()
    {
        try { AsnWriter.PopSequence(null!); }
        catch { _exitCode = 1; }
    }

    // [12] System.Formats.Asn1/System.Formats.Asn1.AsnWriter::PopSetOf:System.Void(System.Nullable{System.Formats.Asn1.Asn1Tag})
    public static void Method12()
    {
        try { AsnWriter.PopSetOf(null!); }
        catch { _exitCode = 1; }
    }

    // [13] System.Formats.Asn1/System.Formats.Asn1.AsnWriter::PushOctetString:System.Formats.Asn1.AsnWriter+Scope(System.Nullable{System.Formats.Asn1.Asn1Tag})
    public static void Method13()
    {
        try { if ((int)(AsnWriter.PushOctetString(null!)) != (int)(AsnWriter.PushOctetString(null!))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [14] System.Formats.Asn1/System.Formats.Asn1.AsnWriter::PushSequence:System.Formats.Asn1.AsnWriter+Scope(System.Nullable{System.Formats.Asn1.Asn1Tag})
    public static void Method14()
    {
        try { if ((int)(AsnWriter.PushSequence(null!)) != (int)(AsnWriter.PushSequence(null!))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [15] System.Formats.Asn1/System.Formats.Asn1.AsnWriter::PushSetOf:System.Formats.Asn1.AsnWriter+Scope(System.Nullable{System.Formats.Asn1.Asn1Tag})
    public static void Method15()
    {
        try { if ((int)(AsnWriter.PushSetOf(null!)) != (int)(AsnWriter.PushSetOf(null!))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [16] System.Formats.Asn1/System.Formats.Asn1.AsnWriter::Reset:System.Void()
    public static void Method16()
    {
        try { AsnWriter.Reset(); }
        catch { _exitCode = 1; }
    }

    // [17] System.Formats.Asn1/System.Formats.Asn1.AsnWriter::TryEncode:System.Boolean(System.Span{System.Byte},System.Int32&)
    public static void Method17()
    {
    var refLocal_1 = 42;
        try { if (((AsnWriter.TryEncode(null!, ref refLocal_1)) ? 1 : 0) != ((AsnWriter.TryEncode(null!, ref refLocal_1)) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [18] System.Formats.Asn1/System.Formats.Asn1.AsnWriter::WriteBitString:System.Void(System.ReadOnlySpan{System.Byte},System.Int32,System.Nullable{System.Formats.Asn1.Asn1Tag})
    public static void Method18()
    {
        try { AsnWriter.WriteBitString(null!, 42, null!); }
        catch { _exitCode = 1; }
    }

    // [19] System.Formats.Asn1/System.Formats.Asn1.AsnWriter::WriteBoolean:System.Void(System.Boolean,System.Nullable{System.Formats.Asn1.Asn1Tag})
    public static void Method19()
    {
        try { AsnWriter.WriteBoolean(true, null!); }
        catch { _exitCode = 1; }
    }

    // [20] System.Formats.Asn1/System.Formats.Asn1.AsnWriter::WriteCharacterString:System.Void(System.Formats.Asn1.UniversalTagNumber,System.String,System.Nullable{System.Formats.Asn1.Asn1Tag})
    public static void Method20()
    {
        try { AsnWriter.WriteCharacterString(null!, "hello", null!); }
        catch { _exitCode = 1; }
    }

    // [21] System.Formats.Asn1/System.Formats.Asn1.AsnWriter::WriteCharacterString:System.Void(System.Formats.Asn1.UniversalTagNumber,System.ReadOnlySpan{System.Char},System.Nullable{System.Formats.Asn1.Asn1Tag})
    public static void Method21()
    {
        try { AsnWriter.WriteCharacterString(null!, null!, null!); }
        catch { _exitCode = 1; }
    }

    // [22] System.Formats.Asn1/System.Formats.Asn1.AsnWriter::WriteEncodedValue:System.Void(System.ReadOnlySpan{System.Byte})
    public static void Method22()
    {
        try { AsnWriter.WriteEncodedValue(null!); }
        catch { _exitCode = 1; }
    }

    // [23] System.Formats.Asn1/System.Formats.Asn1.AsnWriter::WriteEnumeratedValue:System.Void(System.Enum,System.Nullable{System.Formats.Asn1.Asn1Tag})
    public static void Method23()
    {
        try { AsnWriter.WriteEnumeratedValue(DayOfWeek.Monday, null!); }
        catch { _exitCode = 1; }
    }

    // [24] System.Formats.Asn1/System.Formats.Asn1.AsnWriter::WriteEnumeratedValue{TEnum}:System.Void(TEnum,System.Nullable{System.Formats.Asn1.Asn1Tag})

    // [25] System.Formats.Asn1/System.Formats.Asn1.AsnWriter::WriteGeneralizedTime:System.Void(System.DateTimeOffset,System.Boolean,System.Nullable{System.Formats.Asn1.Asn1Tag})
    public static void Method25()
    {
        try { AsnWriter.WriteGeneralizedTime(null!, true, null!); }
        catch { _exitCode = 1; }
    }

    // [26] System.Formats.Asn1/System.Formats.Asn1.AsnWriter::WriteInteger:System.Void(System.Int64,System.Nullable{System.Formats.Asn1.Asn1Tag})
    public static void Method26()
    {
        try { AsnWriter.WriteInteger(42L, null!); }
        catch { _exitCode = 1; }
    }

    // [27] System.Formats.Asn1/System.Formats.Asn1.AsnWriter::WriteInteger:System.Void(System.UInt64,System.Nullable{System.Formats.Asn1.Asn1Tag})
    public static void Method27()
    {
        try { AsnWriter.WriteInteger(42uL, null!); }
        catch { _exitCode = 1; }
    }

    // [28] System.Formats.Asn1/System.Formats.Asn1.AsnWriter::WriteInteger:System.Void(System.Numerics.BigInteger,System.Nullable{System.Formats.Asn1.Asn1Tag})
    public static void Method28()
    {
        try { AsnWriter.WriteInteger(null!, null!); }
        catch { _exitCode = 1; }
    }

    // [29] System.Formats.Asn1/System.Formats.Asn1.AsnWriter::WriteInteger:System.Void(System.ReadOnlySpan{System.Byte},System.Nullable{System.Formats.Asn1.Asn1Tag})
    public static void Method29()
    {
        try { AsnWriter.WriteInteger(null!, null!); }
        catch { _exitCode = 1; }
    }

    // [30] System.Formats.Asn1/System.Formats.Asn1.AsnWriter::WriteIntegerUnsigned:System.Void(System.ReadOnlySpan{System.Byte},System.Nullable{System.Formats.Asn1.Asn1Tag})
    public static void Method30()
    {
        try { AsnWriter.WriteIntegerUnsigned(null!, null!); }
        catch { _exitCode = 1; }
    }

    // [31] System.Formats.Asn1/System.Formats.Asn1.AsnWriter::WriteNamedBitList:System.Void(System.Enum,System.Nullable{System.Formats.Asn1.Asn1Tag})
    public static void Method31()
    {
        try { AsnWriter.WriteNamedBitList(DayOfWeek.Monday, null!); }
        catch { _exitCode = 1; }
    }

    // [32] System.Formats.Asn1/System.Formats.Asn1.AsnWriter::WriteNamedBitList{TEnum}:System.Void(TEnum,System.Nullable{System.Formats.Asn1.Asn1Tag})

    // [33] System.Formats.Asn1/System.Formats.Asn1.AsnWriter::WriteNamedBitList:System.Void(System.Collections.BitArray,System.Nullable{System.Formats.Asn1.Asn1Tag})
    public static void Method33()
    {
        try { AsnWriter.WriteNamedBitList(null!, null!); }
        catch { _exitCode = 1; }
    }

    // [34] System.Formats.Asn1/System.Formats.Asn1.AsnWriter::WriteNull:System.Void(System.Nullable{System.Formats.Asn1.Asn1Tag})
    public static void Method34()
    {
        try { AsnWriter.WriteNull(null!); }
        catch { _exitCode = 1; }
    }

    // [35] System.Formats.Asn1/System.Formats.Asn1.AsnWriter::WriteObjectIdentifier:System.Void(System.String,System.Nullable{System.Formats.Asn1.Asn1Tag})
    public static void Method35()
    {
        try { AsnWriter.WriteObjectIdentifier("hello", null!); }
        catch { _exitCode = 1; }
    }

    // [36] System.Formats.Asn1/System.Formats.Asn1.AsnWriter::WriteObjectIdentifier:System.Void(System.ReadOnlySpan{System.Char},System.Nullable{System.Formats.Asn1.Asn1Tag})
    public static void Method36()
    {
        try { AsnWriter.WriteObjectIdentifier(null!, null!); }
        catch { _exitCode = 1; }
    }

    // [37] System.Formats.Asn1/System.Formats.Asn1.AsnWriter::WriteOctetString:System.Void(System.ReadOnlySpan{System.Byte},System.Nullable{System.Formats.Asn1.Asn1Tag})
    public static void Method37()
    {
        try { AsnWriter.WriteOctetString(null!, null!); }
        catch { _exitCode = 1; }
    }

    // [38] System.Formats.Asn1/System.Formats.Asn1.AsnWriter::WriteUtcTime:System.Void(System.DateTimeOffset,System.Nullable{System.Formats.Asn1.Asn1Tag})
    public static void Method38()
    {
        try { AsnWriter.WriteUtcTime(null!, null!); }
        catch { _exitCode = 1; }
    }

    // [39] System.Formats.Asn1/System.Formats.Asn1.AsnWriter::WriteUtcTime:System.Void(System.DateTimeOffset,System.Int32,System.Nullable{System.Formats.Asn1.Asn1Tag})
    public static void Method39()
    {
        try { AsnWriter.WriteUtcTime(null!, 42, null!); }
        catch { _exitCode = 1; }
    }

    // [40] System.Formats.Asn1/System.Formats.Asn1.AsnWriter::.ctor:System.Void(System.Formats.Asn1.AsnEncodingRules)
    public static void Method40()
    {
        try { new AsnWriter(null!); }
        catch { _exitCode = 1; }
    }

    // [41] System.Formats.Asn1/System.Formats.Asn1.AsnWriter::.ctor:System.Void(System.Formats.Asn1.AsnEncodingRules,System.Int32)
    public static void Method41()
    {
        try { new AsnWriter(null!, 42); }
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
                case 3: CustomEntryMethod3(); break;
                case 4: CustomEntryMethod4(); break;
                case 5: CustomEntryMethod5(); break;
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
                case 24: CustomEntryMethod24(); break;
                case 25: Method25(); break;
                case 26: Method26(); break;
                case 27: Method27(); break;
                case 28: Method28(); break;
                case 29: Method29(); break;
                case 30: Method30(); break;
                case 31: Method31(); break;
                case 32: CustomEntryMethod32(); break;
                case 33: Method33(); break;
                case 34: Method34(); break;
                case 35: Method35(); break;
                case 36: Method36(); break;
                case 37: Method37(); break;
                case 38: Method38(); break;
                case 39: Method39(); break;
                case 40: Method40(); break;
                case 41: Method41(); break;
            }
        }
        catch
        {
            _exitCode = 1;
        }
    }

}