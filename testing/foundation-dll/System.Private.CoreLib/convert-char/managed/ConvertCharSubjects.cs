// Auto-generated native-AOT entry point
// Family: family/System.Private.CoreLib/convert/char
// Assembly: System.Private.CoreLib
// Variant: subjects

using Chaos.TestFramework;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Threading;

public static partial class ConvertCharSubjects
{
    // Inlined exit code — avoids SDK method call resolution in codegen
    public static int _exitCode;

    // [0] System.Private.CoreLib/System.Convert::ToChar:System.Char(System.Boolean)
    public static void Subject_0()
    {
        try { _ = (int)(Convert.ToChar(true)); }
        catch { _exitCode = 1; }
    }

    // [1] System.Private.CoreLib/System.Convert::ToChar:System.Char(System.Byte)
    public static void Subject_1()
    {
        try { _ = (int)(Convert.ToChar((byte)42)); }
        catch { _exitCode = 1; }
    }

    // [2] System.Private.CoreLib/System.Convert::ToChar:System.Char(System.Char)
    public static void Subject_2()
    {
        try { _ = (int)(Convert.ToChar('A')); }
        catch { _exitCode = 1; }
    }

    // [3] System.Private.CoreLib/System.Convert::ToChar:System.Char(System.DateTime)
    public static void Subject_3()
    {
        try { _ = (int)(Convert.ToChar(DateTime.UtcNow)); }
        catch { _exitCode = 1; }
    }

    // [4] System.Private.CoreLib/System.Convert::ToChar:System.Char(System.Decimal)
    public static void Subject_4()
    {
        try { _ = (int)(Convert.ToChar(42m)); }
        catch { _exitCode = 1; }
    }

    // [5] System.Private.CoreLib/System.Convert::ToChar:System.Char(System.Double)
    public static void Subject_5()
    {
        try { _ = (int)(Convert.ToChar(42.0)); }
        catch { _exitCode = 1; }
    }

    // [6] System.Private.CoreLib/System.Convert::ToChar:System.Char(System.Int16)
    public static void Subject_6()
    {
        try { _ = (int)(Convert.ToChar((short)42)); }
        catch { _exitCode = 1; }
    }

    // [7] System.Private.CoreLib/System.Convert::ToChar:System.Char(System.Int32)
    public static void Subject_7()
    {
        try { _ = (int)(Convert.ToChar(42)); }
        catch { _exitCode = 1; }
    }

    // [8] System.Private.CoreLib/System.Convert::ToChar:System.Char(System.Int64)
    public static void Subject_8()
    {
        try { _ = (int)(Convert.ToChar(42L)); }
        catch { _exitCode = 1; }
    }

    // [9] System.Private.CoreLib/System.Convert::ToChar:System.Char(System.Object)
    public static void Subject_9()
    {
        try { _ = (int)(Convert.ToChar(null!)); }
        catch { _exitCode = 1; }
    }

    // [10] System.Private.CoreLib/System.Convert::ToChar:System.Char(System.Object,System.IFormatProvider)
    public static void Subject_10()
    {
        try { _ = (int)(Convert.ToChar(null!, null)); }
        catch { _exitCode = 1; }
    }

    // [11] System.Private.CoreLib/System.Convert::ToChar:System.Char(System.SByte)
    public static void Subject_11()
    {
        try { _ = (int)(Convert.ToChar((sbyte)42)); }
        catch { _exitCode = 1; }
    }

    // [12] System.Private.CoreLib/System.Convert::ToChar:System.Char(System.Single)
    public static void Subject_12()
    {
        try { _ = (int)(Convert.ToChar(42.0f)); }
        catch { _exitCode = 1; }
    }

    // [13] System.Private.CoreLib/System.Convert::ToChar:System.Char(System.String)
    public static void Subject_13()
    {
        try { _ = (int)(Convert.ToChar("hello")); }
        catch { _exitCode = 1; }
    }

    // [14] System.Private.CoreLib/System.Convert::ToChar:System.Char(System.String,System.IFormatProvider)
    public static void Subject_14()
    {
        try { _ = (int)(Convert.ToChar("hello", null)); }
        catch { _exitCode = 1; }
    }

    // [15] System.Private.CoreLib/System.Convert::ToChar:System.Char(System.UInt16)
    public static void Subject_15()
    {
        try { _ = (int)(Convert.ToChar((ushort)42)); }
        catch { _exitCode = 1; }
    }

    // [16] System.Private.CoreLib/System.Convert::ToChar:System.Char(System.UInt32)
    public static void Subject_16()
    {
        try { _ = (int)(Convert.ToChar(42u)); }
        catch { _exitCode = 1; }
    }

    // [17] System.Private.CoreLib/System.Convert::ToChar:System.Char(System.UInt64)
    public static void Subject_17()
    {
        try { _ = (int)(Convert.ToChar(42uL)); }
        catch { _exitCode = 1; }
    }

}