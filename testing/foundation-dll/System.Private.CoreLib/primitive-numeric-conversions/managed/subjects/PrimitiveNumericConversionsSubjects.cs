// Auto-generated native-AOT entry point
// Family: family/System.Private.CoreLib/primitive/numeric/conversions
// Assembly: System.Private.CoreLib
// Variant: subjects

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
        try { if (((Convert.ToBoolean("true")) ? 1 : 0) != ((Convert.ToBoolean("true")) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [1] System.Private.CoreLib/System.Convert::ToByte:System.Byte(System.String)
    public static void Subject_1()
    {
        try { if ((int)(Convert.ToByte("42")) != (int)(Convert.ToByte("42"))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [2] System.Private.CoreLib/System.Convert::ToInt16:System.Int16(System.String)
    public static void Subject_2()
    {
        try { if ((int)(Convert.ToInt16("42")) != (int)(Convert.ToInt16("42"))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [3] System.Private.CoreLib/System.Convert::ToInt32:System.Int32(System.String)
    public static void Subject_3()
    {
        try { if (Convert.ToInt32("42") != Convert.ToInt32("42")) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [4] System.Private.CoreLib/System.Convert::ToInt64:System.Int64(System.String)
    public static void Subject_4()
    {
        try { if ((int)(Convert.ToInt64("42")) != (int)(Convert.ToInt64("42"))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [5] System.Private.CoreLib/System.Convert::ToSingle:System.Single(System.String)
    public static void Subject_5()
    {
        try { if ((int)(Convert.ToSingle("42")) != (int)(Convert.ToSingle("42"))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [6] System.Private.CoreLib/System.Convert::ToDouble:System.Double(System.String)
    public static void Subject_6()
    {
        try { if ((int)(Convert.ToDouble("42")) != (int)(Convert.ToDouble("42"))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [7] System.Private.CoreLib/System.Convert::ToDecimal:System.Decimal(System.String)
    public static void Subject_7()
    {
        try { if ((int)(Convert.ToDecimal("42")) != (int)(Convert.ToDecimal("42"))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [8] System.Private.CoreLib/System.Convert::ToString:System.String(System.Int32)
    public static void Subject_8()
    {
        try { if (((Convert.ToString(42)).Length) != ((Convert.ToString(42)).Length)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [9] System.Private.CoreLib/System.Convert::ToString:System.String(System.Double)
    public static void Subject_9()
    {
        try { if (((Convert.ToString(42.0)).Length) != ((Convert.ToString(42.0)).Length)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [10] System.Private.CoreLib/System.Convert::ToDecimal:System.Decimal(System.Double)
    public static void Subject_10()
    {
        try { if ((int)(Convert.ToDecimal(42.0)) != (int)(Convert.ToDecimal(42.0))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [11] System.Private.CoreLib/System.Convert::ToInt32:System.Int32(System.Double)
    public static void Subject_11()
    {
        try { if (Convert.ToInt32(42.0) != Convert.ToInt32(42.0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [12] System.Private.CoreLib/System.Int32::Parse:System.Int32(System.String)
    public static void Subject_12()
    {
        try { if (int.Parse("42") != int.Parse("42")) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [13] System.Private.CoreLib/System.Int64::Parse:System.Int64(System.String)
    public static void Subject_13()
    {
        try { if ((int)(long.Parse("42")) != (int)(long.Parse("42"))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [14] System.Private.CoreLib/System.Double::Parse:System.Double(System.String)
    public static void Subject_14()
    {
        try { if ((int)(double.Parse("42")) != (int)(double.Parse("42"))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

}