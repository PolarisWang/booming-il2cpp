// Auto-generated native-AOT entry point
// Family: family/System.Private.CoreLib/convert/char
// Assembly: System.Private.CoreLib
// Variant: subjects

using System;

public static partial class ConvertCharSubjects
{
    // Inlined exit code — avoids SDK method call resolution in codegen
    public static int _exitCode;

    // [0] System.Private.CoreLib/System.Convert::ToChar:System.Char(System.Boolean)
    public static void Subject_0()
    {
        if ((int)(Convert.ToChar(true)) != (int)(Convert.ToChar(true))) _exitCode = 1;
    }

    // [1] System.Private.CoreLib/System.Convert::ToChar:System.Char(System.Byte)
    public static void Subject_1()
    {
        if ((int)(Convert.ToChar((byte)42)) != (int)(Convert.ToChar((byte)42))) _exitCode = 1;
    }

    // [2] System.Private.CoreLib/System.Convert::ToChar:System.Char(System.Char)
    public static void Subject_2()
    {
        if ((int)(Convert.ToChar('A')) != (int)(Convert.ToChar('A'))) _exitCode = 1;
    }

    // [3] System.Private.CoreLib/System.Convert::ToChar:System.Char(System.DateTime)
    public static void Subject_3()
    {
        if ((int)(Convert.ToChar(DateTime.UtcNow)) != (int)(Convert.ToChar(DateTime.UtcNow))) _exitCode = 1;
    }

    // [4] System.Private.CoreLib/System.Convert::ToChar:System.Char(System.Decimal)
    public static void Subject_4()
    {
        if ((int)(Convert.ToChar(42m)) != (int)(Convert.ToChar(42m))) _exitCode = 1;
    }

    // [5] System.Private.CoreLib/System.Convert::ToChar:System.Char(System.Double)
    public static void Subject_5()
    {
        if ((int)(Convert.ToChar(42.0)) != (int)(Convert.ToChar(42.0))) _exitCode = 1;
    }

    // [6] System.Private.CoreLib/System.Convert::ToChar:System.Char(System.Int16)
    public static void Subject_6()
    {
        if ((int)(Convert.ToChar((short)42)) != (int)(Convert.ToChar((short)42))) _exitCode = 1;
    }

    // [7] System.Private.CoreLib/System.Convert::ToChar:System.Char(System.Int32)
    public static void Subject_7()
    {
        if ((int)(Convert.ToChar(42)) != (int)(Convert.ToChar(42))) _exitCode = 1;
    }

    // [8] System.Private.CoreLib/System.Convert::ToChar:System.Char(System.Int64)
    public static void Subject_8()
    {
        if ((int)(Convert.ToChar(42L)) != (int)(Convert.ToChar(42L))) _exitCode = 1;
    }

    // [9] System.Private.CoreLib/System.Convert::ToChar:System.Char(System.Object)
    public static void Subject_9()
    {
        if ((int)(Convert.ToChar(42)) != (int)(Convert.ToChar(42))) _exitCode = 1;
    }

    // [10] System.Private.CoreLib/System.Convert::ToChar:System.Char(System.Object,System.IFormatProvider)
    public static void Subject_10()
    {
        if ((int)(Convert.ToChar(42, null)) != (int)(Convert.ToChar(42, null))) _exitCode = 1;
    }

    // [11] System.Private.CoreLib/System.Convert::ToChar:System.Char(System.SByte)
    public static void Subject_11()
    {
        if ((int)(Convert.ToChar((sbyte)42)) != (int)(Convert.ToChar((sbyte)42))) _exitCode = 1;
    }

    // [12] System.Private.CoreLib/System.Convert::ToChar:System.Char(System.Single)
    public static void Subject_12()
    {
        if ((int)(Convert.ToChar(42.0f)) != (int)(Convert.ToChar(42.0f))) _exitCode = 1;
    }

    // [13] System.Private.CoreLib/System.Convert::ToChar:System.Char(System.String)
    public static void Subject_13()
    {
        if ((int)(Convert.ToChar("hello")) != (int)(Convert.ToChar("hello"))) _exitCode = 1;
    }

    // [14] System.Private.CoreLib/System.Convert::ToChar:System.Char(System.String,System.IFormatProvider)
    public static void Subject_14()
    {
        if ((int)(Convert.ToChar("hello", null)) != (int)(Convert.ToChar("hello", null))) _exitCode = 1;
    }

    // [15] System.Private.CoreLib/System.Convert::ToChar:System.Char(System.UInt16)
    public static void Subject_15()
    {
        if ((int)(Convert.ToChar((ushort)42)) != (int)(Convert.ToChar((ushort)42))) _exitCode = 1;
    }

    // [16] System.Private.CoreLib/System.Convert::ToChar:System.Char(System.UInt32)
    public static void Subject_16()
    {
        if ((int)(Convert.ToChar(42u)) != (int)(Convert.ToChar(42u))) _exitCode = 1;
    }

    // [17] System.Private.CoreLib/System.Convert::ToChar:System.Char(System.UInt64)
    public static void Subject_17()
    {
        if ((int)(Convert.ToChar(42uL)) != (int)(Convert.ToChar(42uL))) _exitCode = 1;
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
            case 15: Subject_15(); break;
            case 16: Subject_16(); break;
            case 17: Subject_17(); break;
        }
    }

}