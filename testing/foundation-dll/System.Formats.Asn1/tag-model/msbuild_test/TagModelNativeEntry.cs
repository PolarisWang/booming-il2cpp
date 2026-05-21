// Auto-generated native-AOT entry point
// Family: family/System.Formats.Asn1/tag-model
// Assembly: System.Formats.Asn1
// Variant: benchmark

using System;
using System.Collections.Generic;
using System.Formats.Asn1;
using System.Linq;

public static partial class TagModelNativeEntry
{
    // Inlined exit code — avoids SDK method call resolution in codegen
    public static int _exitCode;

    // [0] System.Formats.Asn1/System.Formats.Asn1.Asn1Tag::AsConstructed:System.Formats.Asn1.Asn1Tag()
    public static void Method0()
    {
        try { if ((int)(Asn1Tag.AsConstructed()) != (int)(Asn1Tag.AsConstructed())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [1] System.Formats.Asn1/System.Formats.Asn1.Asn1Tag::AsPrimitive:System.Formats.Asn1.Asn1Tag()
    public static void Method1()
    {
        try { if ((int)(Asn1Tag.AsPrimitive()) != (int)(Asn1Tag.AsPrimitive())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [2] System.Formats.Asn1/System.Formats.Asn1.Asn1Tag::CalculateEncodedSize:System.Int32()
    public static void Method2()
    {
        try { if (Asn1Tag.CalculateEncodedSize() != Asn1Tag.CalculateEncodedSize()) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [3] System.Formats.Asn1/System.Formats.Asn1.Asn1Tag::Decode:System.Formats.Asn1.Asn1Tag(System.ReadOnlySpan{System.Byte},System.Int32&)
    public static void Method3()
    {
    var refLocal_1 = 42;
        try { if ((int)(Asn1Tag.Decode(null!, ref refLocal_1)) != (int)(Asn1Tag.Decode(null!, ref refLocal_1))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [4] System.Formats.Asn1/System.Formats.Asn1.Asn1Tag::Encode:System.Int32(System.Span{System.Byte})
    public static void Method4()
    {
        try { if (Asn1Tag.Encode(null!) != Asn1Tag.Encode(null!)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [5] System.Formats.Asn1/System.Formats.Asn1.Asn1Tag::Equals:System.Boolean(System.Formats.Asn1.Asn1Tag)
    public static void Method5()
    {
        try { if (((Asn1Tag.Equals(null!)) ? 1 : 0) != ((Asn1Tag.Equals(null!)) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [6] System.Formats.Asn1/System.Formats.Asn1.Asn1Tag::Equals:System.Boolean(System.Object)
    public static void Method6()
    {
        try { if (((Asn1Tag.Equals(42)) ? 1 : 0) != ((Asn1Tag.Equals(42)) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [7] System.Formats.Asn1/System.Formats.Asn1.Asn1Tag::get_IsConstructed:System.Boolean()
    public static void Method7()
    {
        try { if (((Asn1Tag.IsConstructed) ? 1 : 0) != ((Asn1Tag.IsConstructed) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [8] System.Formats.Asn1/System.Formats.Asn1.Asn1Tag::get_TagClass:System.Formats.Asn1.TagClass()
    public static void Method8()
    {
        try { if ((int)(Asn1Tag.TagClass) != (int)(Asn1Tag.TagClass)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [9] System.Formats.Asn1/System.Formats.Asn1.Asn1Tag::get_TagValue:System.Int32()
    public static void Method9()
    {
        try { if (Asn1Tag.TagValue != Asn1Tag.TagValue) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [10] System.Formats.Asn1/System.Formats.Asn1.Asn1Tag::GetHashCode:System.Int32()
    public static void Method10()
    {
        try { if (Asn1Tag.GetHashCode() != Asn1Tag.GetHashCode()) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [11] System.Formats.Asn1/System.Formats.Asn1.Asn1Tag::HasSameClassAndValue:System.Boolean(System.Formats.Asn1.Asn1Tag)
    public static void Method11()
    {
        try { if (((Asn1Tag.HasSameClassAndValue(null!)) ? 1 : 0) != ((Asn1Tag.HasSameClassAndValue(null!)) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [12] System.Formats.Asn1/System.Formats.Asn1.Asn1Tag::op_Equality:System.Boolean(System.Formats.Asn1.Asn1Tag,System.Formats.Asn1.Asn1Tag)
    public static void Method12()
    {
        try { if (((Asn1Tag.op_Equality(null!, null!)) ? 1 : 0) != ((Asn1Tag.op_Equality(null!, null!)) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [13] System.Formats.Asn1/System.Formats.Asn1.Asn1Tag::op_Inequality:System.Boolean(System.Formats.Asn1.Asn1Tag,System.Formats.Asn1.Asn1Tag)
    public static void Method13()
    {
        try { if (((Asn1Tag.op_Inequality(null!, null!)) ? 1 : 0) != ((Asn1Tag.op_Inequality(null!, null!)) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [14] System.Formats.Asn1/System.Formats.Asn1.Asn1Tag::ToString:System.String()
    public static void Method14()
    {
        try { if (((Asn1Tag.ToString()).Length) != ((Asn1Tag.ToString()).Length)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [15] System.Formats.Asn1/System.Formats.Asn1.Asn1Tag::TryDecode:System.Boolean(System.ReadOnlySpan{System.Byte},System.Formats.Asn1.Asn1Tag&,System.Int32&)
    public static void Method15()
    {
    var refLocal_1 = null!;
    var refLocal_2 = 42;
        try { if (((Asn1Tag.TryDecode(null!, ref refLocal_1, ref refLocal_2)) ? 1 : 0) != ((Asn1Tag.TryDecode(null!, ref refLocal_1, ref refLocal_2)) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [16] System.Formats.Asn1/System.Formats.Asn1.Asn1Tag::TryEncode:System.Boolean(System.Span{System.Byte},System.Int32&)
    public static void Method16()
    {
    var refLocal_1 = 42;
        try { if (((Asn1Tag.TryEncode(null!, ref refLocal_1)) ? 1 : 0) != ((Asn1Tag.TryEncode(null!, ref refLocal_1)) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [17] System.Formats.Asn1/System.Formats.Asn1.Asn1Tag::.ctor:System.Void(System.Formats.Asn1.UniversalTagNumber,System.Boolean)
    public static void Method17()
    {
        try { new Asn1Tag(null!, true); }
        catch { _exitCode = 1; }
    }

    // [18] System.Formats.Asn1/System.Formats.Asn1.Asn1Tag::.ctor:System.Void(System.Formats.Asn1.TagClass,System.Int32,System.Boolean)
    public static void Method18()
    {
        try { new Asn1Tag(null!, 42, true); }
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
            }
        }
        catch
        {
            _exitCode = 1;
        }
    }

}