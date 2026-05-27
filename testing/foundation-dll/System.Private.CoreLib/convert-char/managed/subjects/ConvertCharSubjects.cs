// Auto-generated native-AOT entry point
// Family: family/System.Private.CoreLib/convert/char
// Assembly: System.Private.CoreLib
// Variant: subjects

using System;
using System.Collections.Generic;
using System.Linq;

public static partial class ConvertCharSubjects
{
    // Inlined exit code — avoids SDK method call resolution in codegen
    public static int _exitCode;

    // [0] System.Private.CoreLib/System.Convert::ToChar:System.Char(System.Boolean)
    public static void Subject_0()
    {
        try { if ((int)(Convert.ToChar(true)) != (int)(Convert.ToChar(true))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [1] System.Private.CoreLib/System.Convert::ToChar:System.Char(System.Byte)
    public static void Subject_1()
    {
        try { if ((int)(Convert.ToChar((byte)42)) != (int)(Convert.ToChar((byte)42))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [2] System.Private.CoreLib/System.Convert::ToChar:System.Char(System.Char)
    public static void Subject_2()
    {
        try { if ((int)(Convert.ToChar('A')) != (int)(Convert.ToChar('A'))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [3] System.Private.CoreLib/System.Convert::ToChar:System.Char(System.DateTime)
    public static void Subject_3()
    {
        try { if ((int)(Convert.ToChar(DateTime.UtcNow)) != (int)(Convert.ToChar(DateTime.UtcNow))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [4] System.Private.CoreLib/System.Convert::ToChar:System.Char(System.Decimal)
    public static void Subject_4()
    {
        try { if ((int)(Convert.ToChar(42m)) != (int)(Convert.ToChar(42m))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [5] System.Private.CoreLib/System.Convert::ToChar:System.Char(System.Double)
    public static void Subject_5()
    {
        try { if ((int)(Convert.ToChar(42.0)) != (int)(Convert.ToChar(42.0))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [6] System.Private.CoreLib/System.Convert::ToChar:System.Char(System.Int16)
    public static void Subject_6()
    {
        try { if ((int)(Convert.ToChar((short)42)) != (int)(Convert.ToChar((short)42))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [7] System.Private.CoreLib/System.Convert::ToChar:System.Char(System.Int32)
    public static void Subject_7()
    {
        try { if ((int)(Convert.ToChar(42)) != (int)(Convert.ToChar(42))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [8] System.Private.CoreLib/System.Convert::ToChar:System.Char(System.Int64)
    public static void Subject_8()
    {
        try { if ((int)(Convert.ToChar(42L)) != (int)(Convert.ToChar(42L))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [9] System.Private.CoreLib/System.Convert::ToChar:System.Char(System.Object)
    public static void Subject_9()
    {
        try { if ((int)(Convert.ToChar((object)42)) != (int)(Convert.ToChar((object)42))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [10] System.Private.CoreLib/System.Convert::ToChar:System.Char(System.Object,System.IFormatProvider)
    public static void Subject_10()
    {
        try { if ((int)(Convert.ToChar((object)42, null)) != (int)(Convert.ToChar((object)42, null))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [11] System.Private.CoreLib/System.Convert::ToChar:System.Char(System.SByte)
    public static void Subject_11()
    {
        try { if ((int)(Convert.ToChar((sbyte)42)) != (int)(Convert.ToChar((sbyte)42))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [12] System.Private.CoreLib/System.Convert::ToChar:System.Char(System.Single)
    public static void Subject_12()
    {
        try { if ((int)(Convert.ToChar(42.0f)) != (int)(Convert.ToChar(42.0f))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [13] System.Private.CoreLib/System.Convert::ToChar:System.Char(System.String)
    public static void Subject_13()
    {
        try { if ((int)(Convert.ToChar("hello")) != (int)(Convert.ToChar("hello"))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [14] System.Private.CoreLib/System.Convert::ToChar:System.Char(System.String,System.IFormatProvider)
    public static void Subject_14()
    {
        try { if ((int)(Convert.ToChar("hello", null)) != (int)(Convert.ToChar("hello", null))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [15] System.Private.CoreLib/System.Convert::ToChar:System.Char(System.UInt16)
    public static void Subject_15()
    {
        try { if ((int)(Convert.ToChar((ushort)42)) != (int)(Convert.ToChar((ushort)42))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [16] System.Private.CoreLib/System.Convert::ToChar:System.Char(System.UInt32)
    public static void Subject_16()
    {
        try { if ((int)(Convert.ToChar(42u)) != (int)(Convert.ToChar(42u))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [17] System.Private.CoreLib/System.Convert::ToChar:System.Char(System.UInt64)
    public static void Subject_17()
    {
        try { if ((int)(Convert.ToChar(42uL)) != (int)(Convert.ToChar(42uL))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

}