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
        try { Convert.ToChar(true); _exitCode = 1; }
        catch (InvalidCastException) { }
    }

    // [1] System.Private.CoreLib/System.Convert::ToChar:System.Char(System.Byte)
    public static void Subject_1()
    {
        if ((int)(Convert.ToChar((byte)42)) != 42) _exitCode = 1;
    }

    // [2] System.Private.CoreLib/System.Convert::ToChar:System.Char(System.Char)
    public static void Subject_2()
    {
        if ((int)(Convert.ToChar('A')) != 65) _exitCode = 1;
    }

    // [3] System.Private.CoreLib/System.Convert::ToChar:System.Char(System.DateTime)
    public static void Subject_3()
    {
        try { Convert.ToChar(DateTime.UtcNow); _exitCode = 1; }
        catch (InvalidCastException) { }
    }

    // [4] System.Private.CoreLib/System.Convert::ToChar:System.Char(System.Decimal)
    public static void Subject_4()
    {
        try { Convert.ToChar(42m); _exitCode = 1; }
        catch (InvalidCastException) { }
    }

    // [5] System.Private.CoreLib/System.Convert::ToChar:System.Char(System.Double)
    public static void Subject_5()
    {
        try { Convert.ToChar(42.0); _exitCode = 1; }
        catch (InvalidCastException) { }
    }

    // [6] System.Private.CoreLib/System.Convert::ToChar:System.Char(System.Int16)
    public static void Subject_6()
    {
        if ((int)(Convert.ToChar((short)42)) != 42) _exitCode = 1;
    }

    // [7] System.Private.CoreLib/System.Convert::ToChar:System.Char(System.Int32)
    public static void Subject_7()
    {
        if ((int)(Convert.ToChar(42)) != 42) _exitCode = 1;
    }

    // [8] System.Private.CoreLib/System.Convert::ToChar:System.Char(System.Int64)
    public static void Subject_8()
    {
        if ((int)(Convert.ToChar(42L)) != 42) _exitCode = 1;
    }

    // [9] System.Private.CoreLib/System.Convert::ToChar:System.Char(System.Object)
    public static void Subject_9()
    {
        if ((int)(Convert.ToChar(42)) != 42) _exitCode = 1;
    }

    // [10] System.Private.CoreLib/System.Convert::ToChar:System.Char(System.Object,System.IFormatProvider)
    public static void Subject_10()
    {
        if ((int)(Convert.ToChar(42, null)) != 42) _exitCode = 1;
    }

    // [11] System.Private.CoreLib/System.Convert::ToChar:System.Char(System.SByte)
    public static void Subject_11()
    {
        if ((int)(Convert.ToChar((sbyte)42)) != 42) _exitCode = 1;
    }

    // [12] System.Private.CoreLib/System.Convert::ToChar:System.Char(System.Single)
    public static void Subject_12()
    {
        try { Convert.ToChar(42.0f); _exitCode = 1; }
        catch (InvalidCastException) { }
    }

    // [13] System.Private.CoreLib/System.Convert::ToChar:System.Char(System.String)
    public static void Subject_13()
    {
        try { Convert.ToChar("hello"); _exitCode = 1; }
        catch (FormatException) { }
    }

    // [14] System.Private.CoreLib/System.Convert::ToChar:System.Char(System.String,System.IFormatProvider)
    public static void Subject_14()
    {
        try { Convert.ToChar("hello", null); _exitCode = 1; }
        catch (FormatException) { }
    }

    // [15] System.Private.CoreLib/System.Convert::ToChar:System.Char(System.UInt16)
    public static void Subject_15()
    {
        if ((int)(Convert.ToChar((ushort)42)) != 42) _exitCode = 1;
    }

    // [16] System.Private.CoreLib/System.Convert::ToChar:System.Char(System.UInt32)
    public static void Subject_16()
    {
        if ((int)(Convert.ToChar(42u)) != 42) _exitCode = 1;
    }

    // [17] System.Private.CoreLib/System.Convert::ToChar:System.Char(System.UInt64)
    public static void Subject_17()
    {
        if ((int)(Convert.ToChar(42uL)) != 42) _exitCode = 1;
    }

}