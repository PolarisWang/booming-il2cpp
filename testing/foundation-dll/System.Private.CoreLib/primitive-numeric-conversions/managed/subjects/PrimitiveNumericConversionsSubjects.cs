// Auto-generated native-AOT entry point
// Family: family/System.Private.CoreLib/primitive/numeric/conversions
// Assembly: System.Private.CoreLib
// Variant: subjects

using Chaos.TestFramework;
using System;
using System.Collections.Generic;
using System.Linq;

public static partial class PrimitiveNumericConversionsSubjects
{
    // Inlined exit code — avoids SDK method call resolution in codegen
    public static int _exitCode;

    // [0] System.Private.CoreLib/System.Convert::ToBoolean:System.Boolean(System.String)
    public static void Subject_0()
    {
        try { var _ = ((Convert.ToBoolean("true")) ? 1 : 0); }
        catch { _exitCode = 1; }
    }

    // [1] System.Private.CoreLib/System.Convert::ToByte:System.Byte(System.String)
    public static void Subject_1()
    {
        try { var _ = (int)(Convert.ToByte("42")); }
        catch { _exitCode = 1; }
    }

    // [2] System.Private.CoreLib/System.Convert::ToInt16:System.Int16(System.String)
    public static void Subject_2()
    {
        try { var _ = (int)(Convert.ToInt16("42")); }
        catch { _exitCode = 1; }
    }

    // [3] System.Private.CoreLib/System.Convert::ToInt32:System.Int32(System.String)
    public static void Subject_3()
    {
        try { var _ = Convert.ToInt32("42"); }
        catch { _exitCode = 1; }
    }

    // [4] System.Private.CoreLib/System.Convert::ToInt64:System.Int64(System.String)
    public static void Subject_4()
    {
        try { var _ = (int)(Convert.ToInt64("42")); }
        catch { _exitCode = 1; }
    }

    // [5] System.Private.CoreLib/System.Convert::ToSingle:System.Single(System.String)
    public static void Subject_5()
    {
        try { var _ = (int)(Convert.ToSingle("42")); }
        catch { _exitCode = 1; }
    }

    // [6] System.Private.CoreLib/System.Convert::ToDouble:System.Double(System.String)
    public static void Subject_6()
    {
        try { var _ = (int)(Convert.ToDouble("42")); }
        catch { _exitCode = 1; }
    }

    // [7] System.Private.CoreLib/System.Convert::ToDecimal:System.Decimal(System.String)
    public static void Subject_7()
    {
        try { var _ = (int)(Convert.ToDecimal("42")); }
        catch { _exitCode = 1; }
    }

    // [8] System.Private.CoreLib/System.Convert::ToString:System.String(System.Int32)
    public static void Subject_8()
    {
        try { var _ = ((Convert.ToString(42)).Length); }
        catch { _exitCode = 1; }
    }

    // [9] System.Private.CoreLib/System.Convert::ToString:System.String(System.Double)
    public static void Subject_9()
    {
        try { var _ = ((Convert.ToString(42.0)).Length); }
        catch { _exitCode = 1; }
    }

    // [10] System.Private.CoreLib/System.Convert::ToDecimal:System.Decimal(System.Double)
    public static void Subject_10()
    {
        try { var _ = (int)(Convert.ToDecimal(42.0)); }
        catch { _exitCode = 1; }
    }

    // [11] System.Private.CoreLib/System.Convert::ToInt32:System.Int32(System.Double)
    public static void Subject_11()
    {
        try { var _ = Convert.ToInt32(42.0); }
        catch { _exitCode = 1; }
    }

    // [12] System.Private.CoreLib/System.Int32::Parse:System.Int32(System.String)
    public static void Subject_12()
    {
        try { var _ = int.Parse("42"); }
        catch { _exitCode = 1; }
    }

    // [13] System.Private.CoreLib/System.Int64::Parse:System.Int64(System.String)
    public static void Subject_13()
    {
        try { var _ = (int)(long.Parse("42")); }
        catch { _exitCode = 1; }
    }

    // [14] System.Private.CoreLib/System.Double::Parse:System.Double(System.String)
    public static void Subject_14()
    {
        try { var _ = (int)(double.Parse("42")); }
        catch { _exitCode = 1; }
    }

}