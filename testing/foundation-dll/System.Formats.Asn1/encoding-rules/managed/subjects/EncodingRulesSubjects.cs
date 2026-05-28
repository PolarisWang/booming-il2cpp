// Auto-generated native-AOT entry point
// Family: family/System.Formats.Asn1/encoding/rules
// Assembly: System.Formats.Asn1
// Variant: subjects

using System;
using System.Collections.Generic;
using System.Formats.Asn1;
using System.Linq;

public static partial class EncodingRulesSubjects
{
    // Inlined exit code — avoids SDK method call resolution in codegen
    public static int _exitCode;

    // [0] System.Formats.Asn1/System.Formats.Asn1.AsnEncodingRules::.cctor:System.Void()
    public static void Subject_0()
    {
        try { new AsnEncodingRules(); }
        catch { _exitCode = 1; }
    }

    // [1] System.Formats.Asn1/System.Formats.Asn1.AsnEncodingRules::.ctor:System.Void(System.Int32)
    public static void Subject_1()
    {
        try { new AsnEncodingRules(42); }
        catch { _exitCode = 1; }
    }

    // [2] System.Formats.Asn1/System.Formats.Asn1.AsnEncodingRules::ToString:System.String()
    public static void Subject_2()
    {
        try { if (((AsnEncodingRules.ToString()).Length) != ((AsnEncodingRules.ToString()).Length)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [3] System.Formats.Asn1/System.Formats.Asn1.AsnEncodingRules::GetHashCode:System.Int32()
    public static void Subject_3()
    {
        try { if (AsnEncodingRules.GetHashCode() != AsnEncodingRules.GetHashCode()) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [4] System.Formats.Asn1/System.Formats.Asn1.AsnEncodingRules::Equals:System.Boolean(System.Object)
    public static void Subject_4()
    {
        try { if (((AsnEncodingRules.Equals(null!)) ? 1 : 0) != ((AsnEncodingRules.Equals(null!)) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

}