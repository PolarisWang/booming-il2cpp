// Auto-generated native-AOT entry point
// Family: family/System.Private.CoreLib/primitive/numeric/conversions
// Assembly: System.Private.CoreLib
// Variant: subjects

using System;

public static partial class PrimitiveNumericConversionsSubjects
{
    // Inlined exit code — avoids SDK method call resolution in codegen
    public static int _exitCode;

    // [0] System.Private.CoreLib/System.Convert::ToBoolean:System.Boolean(System.String)
    public static void Subject_0()
    {
        if (((Convert.ToBoolean("true")) ? 1 : 0) != ((Convert.ToBoolean("true")) ? 1 : 0)) _exitCode = 1;
    }

    // [1] System.Private.CoreLib/System.Convert::ToByte:System.Byte(System.String)
    public static void Subject_1()
    {
        if ((int)(Convert.ToByte("42")) != (int)(Convert.ToByte("42"))) _exitCode = 1;
    }

    // [2] System.Private.CoreLib/System.Convert::ToInt16:System.Int16(System.String)
    public static void Subject_2()
    {
        if ((int)(Convert.ToInt16("42")) != (int)(Convert.ToInt16("42"))) _exitCode = 1;
    }

    // [3] System.Private.CoreLib/System.Convert::ToInt32:System.Int32(System.String)
    public static void Subject_3()
    {
        if (Convert.ToInt32("42") != Convert.ToInt32("42")) _exitCode = 1;
    }

    // [4] System.Private.CoreLib/System.Convert::ToInt64:System.Int64(System.String)
    public static void Subject_4()
    {
        if ((int)(Convert.ToInt64("42")) != (int)(Convert.ToInt64("42"))) _exitCode = 1;
    }

    // [5] System.Private.CoreLib/System.Convert::ToSingle:System.Single(System.String)
    public static void Subject_5()
    {
        if ((int)(Convert.ToSingle("42")) != (int)(Convert.ToSingle("42"))) _exitCode = 1;
    }

    // [6] System.Private.CoreLib/System.Convert::ToDouble:System.Double(System.String)
    public static void Subject_6()
    {
        if ((int)(Convert.ToDouble("42")) != (int)(Convert.ToDouble("42"))) _exitCode = 1;
    }

    // [7] System.Private.CoreLib/System.Convert::ToDecimal:System.Decimal(System.String)
    public static void Subject_7()
    {
        if ((int)(Convert.ToDecimal("42")) != (int)(Convert.ToDecimal("42"))) _exitCode = 1;
    }

    // [8] System.Private.CoreLib/System.Convert::ToString:System.String(System.Int32)
    public static void Subject_8()
    {
        if (((Convert.ToString(42)).Length) != ((Convert.ToString(42)).Length)) _exitCode = 1;
    }

    // [9] System.Private.CoreLib/System.Convert::ToString:System.String(System.Double)
    public static void Subject_9()
    {
        if (((Convert.ToString(42.0)).Length) != ((Convert.ToString(42.0)).Length)) _exitCode = 1;
    }

    // [10] System.Private.CoreLib/System.Convert::ToDecimal:System.Decimal(System.Double)
    public static void Subject_10()
    {
        if ((int)(Convert.ToDecimal("42")) != (int)(Convert.ToDecimal("42"))) _exitCode = 1;
    }

    // [11] System.Private.CoreLib/System.Convert::ToInt32:System.Int32(System.Double)
    public static void Subject_11()
    {
        if (Convert.ToInt32("42") != Convert.ToInt32("42")) _exitCode = 1;
    }

    // [12] System.Private.CoreLib/System.Int32::Parse:System.Int32(System.String)
    public static void Subject_12()
    {
        if (int.Parse("42") != int.Parse("42")) _exitCode = 1;
    }

    // [13] System.Private.CoreLib/System.Int64::Parse:System.Int64(System.String)
    public static void Subject_13()
    {
        if ((int)(long.Parse("42")) != (int)(long.Parse("42"))) _exitCode = 1;
    }

    // [14] System.Private.CoreLib/System.Double::Parse:System.Double(System.String)
    public static void Subject_14()
    {
        if ((int)(double.Parse("42")) != (int)(double.Parse("42"))) _exitCode = 1;
    }

    public static void Run(int entryIndex)
    {
        switch (entryIndex)
        {
            case 0: Subject_0(); break;
            case 1: Subject_1(); break;
            case 2: Subject_2(); break;
            case 3: Subject_3(); break;
            case 4: Subject_4(); break;
            case 5: Subject_5(); break;
            case 6: Subject_6(); break;
            case 7: Subject_7(); break;
            case 8: Subject_8(); break;
            case 9: Subject_9(); break;
            case 10: Subject_10(); break;
            case 11: Subject_11(); break;
            case 12: Subject_12(); break;
            case 13: Subject_13(); break;
            case 14: Subject_14(); break;
        }
    }

}