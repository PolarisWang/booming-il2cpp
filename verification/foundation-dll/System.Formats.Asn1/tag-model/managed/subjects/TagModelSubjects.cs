// Auto-generated native-AOT entry point
// Family: family/System.Formats.Asn1/tag/model
// Assembly: System.Formats.Asn1
// Variant: subjects

using System;
using System.Collections.Generic;
using System.Formats.Asn1;
using System.Linq;

public static partial class TagModelSubjects
{
    // Inlined exit code — avoids SDK method call resolution in codegen
    public static int _exitCode;

    // [0] System.Formats.Asn1/System.Formats.Asn1.Asn1Tag::AsConstructed:System.Formats.Asn1.Asn1Tag()
    public static void Subject_0()
    {
        if (((default(Asn1Tag).AsConstructed()).GetHashCode()) != 536870912) _exitCode = 1;
    }

    // [1] System.Formats.Asn1/System.Formats.Asn1.Asn1Tag::AsPrimitive:System.Formats.Asn1.Asn1Tag()
    public static void Subject_1()
    {
        if (((default(Asn1Tag).AsPrimitive()).GetHashCode()) != 0) _exitCode = 1;
    }

    // [2] System.Formats.Asn1/System.Formats.Asn1.Asn1Tag::CalculateEncodedSize:System.Int32()
    public static void Subject_2()
    {
        if (default(Asn1Tag).CalculateEncodedSize() != 1) _exitCode = 1;
    }

    // [3] System.Formats.Asn1/System.Formats.Asn1.Asn1Tag::Decode:System.Formats.Asn1.Asn1Tag(System.ReadOnlySpan{System.Byte},System.Int32&)
    public static void Subject_3()
    {
        try { Asn1Tag.Decode(default, out _); _exitCode = 1; }
        catch (AsnContentException) { }
    }

    // [4] System.Formats.Asn1/System.Formats.Asn1.Asn1Tag::Encode:System.Int32(System.Span{System.Byte})
    public static void Subject_4()
    {
        try { default(Asn1Tag).Encode(default(Span<byte>)); _exitCode = 1; }
        catch (ArgumentException) { }
    }

    // [5] System.Formats.Asn1/System.Formats.Asn1.Asn1Tag::Equals:System.Boolean(System.Formats.Asn1.Asn1Tag)
    public static void Subject_5()
    {
        if (((default(Asn1Tag).Equals(default)) ? 1 : 0) != 1) _exitCode = 1;
    }

    // [6] System.Formats.Asn1/System.Formats.Asn1.Asn1Tag::Equals:System.Boolean(System.Object)
    public static void Subject_6()
    {
        if (((default(Asn1Tag).Equals(42)) ? 1 : 0) != 0) _exitCode = 1;
    }

    // [7] System.Formats.Asn1/System.Formats.Asn1.Asn1Tag::get_IsConstructed:System.Boolean()
    public static void Subject_7()
    {
        if (((default(Asn1Tag).IsConstructed) ? 1 : 0) != 0) _exitCode = 1;
    }

    // [8] System.Formats.Asn1/System.Formats.Asn1.Asn1Tag::get_TagClass:System.Formats.Asn1.TagClass()
    public static void Subject_8()
    {
        if (((default(Asn1Tag).TagClass).GetHashCode()) != 0) _exitCode = 1;
    }

    // [9] System.Formats.Asn1/System.Formats.Asn1.Asn1Tag::get_TagValue:System.Int32()
    public static void Subject_9()
    {
        if (default(Asn1Tag).TagValue != 0) _exitCode = 1;
    }

    // [10] System.Formats.Asn1/System.Formats.Asn1.Asn1Tag::GetHashCode:System.Int32()
    public static void Subject_10()
    {
        if (default(Asn1Tag).GetHashCode() != 0) _exitCode = 1;
    }

    // [11] System.Formats.Asn1/System.Formats.Asn1.Asn1Tag::HasSameClassAndValue:System.Boolean(System.Formats.Asn1.Asn1Tag)
    public static void Subject_11()
    {
        if (((default(Asn1Tag).HasSameClassAndValue(default)) ? 1 : 0) != 1) _exitCode = 1;
    }

    // [12] System.Formats.Asn1/System.Formats.Asn1.Asn1Tag::op_Equality:System.Boolean(System.Formats.Asn1.Asn1Tag,System.Formats.Asn1.Asn1Tag)
    public static void Subject_12()
    {
        if (((default(Asn1Tag) == default(Asn1Tag)) ? 1 : 0) != 1) _exitCode = 1;
    }

    // [13] System.Formats.Asn1/System.Formats.Asn1.Asn1Tag::op_Inequality:System.Boolean(System.Formats.Asn1.Asn1Tag,System.Formats.Asn1.Asn1Tag)
    public static void Subject_13()
    {
        if (((default(Asn1Tag) != default(Asn1Tag)) ? 1 : 0) != 0) _exitCode = 1;
    }

    // [14] System.Formats.Asn1/System.Formats.Asn1.Asn1Tag::ToString:System.String()
    public static void Subject_14()
    {
        if (((default(Asn1Tag).ToString()).Length) != 13) _exitCode = 1;
    }

    // [15] System.Formats.Asn1/System.Formats.Asn1.Asn1Tag::TryDecode:System.Boolean(System.ReadOnlySpan{System.Byte},System.Formats.Asn1.Asn1Tag&,System.Int32&)
    public static void Subject_15()
    {
        if (((Asn1Tag.TryDecode(default, out _, out _)) ? 1 : 0) != 0) _exitCode = 1;
    }

    // [16] System.Formats.Asn1/System.Formats.Asn1.Asn1Tag::TryEncode:System.Boolean(System.Span{System.Byte},System.Int32&)
    public static void Subject_16()
    {
        if (((default(Asn1Tag).TryEncode(default(Span<byte>), out _)) ? 1 : 0) != 0) _exitCode = 1;
    }

    // [17] System.Formats.Asn1/System.Formats.Asn1.Asn1Tag::.ctor:System.Void(System.Formats.Asn1.UniversalTagNumber,System.Boolean)
    public static void Subject_17()
    {
        new Asn1Tag(default, true);
    }

    // [18] System.Formats.Asn1/System.Formats.Asn1.Asn1Tag::.ctor:System.Void(System.Formats.Asn1.TagClass,System.Int32,System.Boolean)
    public static void Subject_18()
    {
        new Asn1Tag(default, 42, true);
    }

}