// Auto-generated native-AOT entry point
// Family: family/System.Private.CoreLib/string/char/text/core
// Assembly: System.Private.CoreLib
// Variant: subjects

using Chaos.TestFramework;
using System;
using System.Collections.Generic;
using System.Linq;

public static partial class StringCharTextCoreSubjects
{
    // Inlined exit code — avoids SDK method call resolution in codegen
    public static int _exitCode;

    // [0] System.Private.CoreLib/System.String::IndexOf:System.Int32(System.String)
    public static void Subject_0()
    {
        try { var _ = "hello".IndexOf("hello"); }
        catch { _exitCode = 1; }
    }

    // [1] System.Private.CoreLib/System.String::IndexOf:System.Int32(System.Char)
    public static void Subject_1()
    {
        try { var _ = "hello".IndexOf('A'); }
        catch { _exitCode = 1; }
    }

    // [2] System.Private.CoreLib/System.String::Substring:System.String(System.Int32)
    public static void Subject_2()
    {
        try { var _ = (("hello".Substring(1)).Length); }
        catch { _exitCode = 1; }
    }

    // [3] System.Private.CoreLib/System.String::Substring:System.String(System.Int32,System.Int32)
    public static void Subject_3()
    {
        try { var _ = (("hello".Substring(1, 2)).Length); }
        catch { _exitCode = 1; }
    }

    // [4] System.Private.CoreLib/System.String::Compare:System.Int32(System.String,System.String)
    public static void Subject_4()
    {
        try { var _ = String.Compare("hello", "hello"); }
        catch { _exitCode = 1; }
    }

    // [5] System.Private.CoreLib/System.String::Compare:System.Int32(System.String,System.Int32,System.String,System.Int32,System.Int32)
    public static void Subject_5()
    {
        try { var _ = string.Compare("hello", 0, "world", 0, 3); }
        catch { _exitCode = 1; }
    }

    // [6] System.Private.CoreLib/System.String::Concat:System.String(System.String,System.String)
    public static void Subject_6()
    {
        try { var _ = ((String.Concat("hello", "hello")).Length); }
        catch { _exitCode = 1; }
    }

    // [7] System.Private.CoreLib/System.String::Concat:System.String(System.String,System.String,System.String)
    public static void Subject_7()
    {
        try { var _ = ((String.Concat("hello", "hello", "hello")).Length); }
        catch { _exitCode = 1; }
    }

    // [8] System.Private.CoreLib/System.String::Format:System.String(System.String,System.Object[])
    public static void Subject_8()
    {
        try { var _ = ((String.Format("hello", Array.Empty<System.Object>())).Length); }
        catch { _exitCode = 1; }
    }

    // [9] System.Private.CoreLib/System.String::StartsWith:System.Boolean(System.String)
    public static void Subject_9()
    {
        try { var _ = (("hello".StartsWith("hello")) ? 1 : 0); }
        catch { _exitCode = 1; }
    }

    // [10] System.Private.CoreLib/System.String::Contains:System.Boolean(System.String)
    public static void Subject_10()
    {
        try { var _ = (("hello".Contains("hello")) ? 1 : 0); }
        catch { _exitCode = 1; }
    }

    // [11] System.Private.CoreLib/System.String::Replace:System.String(System.String,System.String)
    public static void Subject_11()
    {
        try { var _ = (("hello".Replace("hello", "hello")).Length); }
        catch { _exitCode = 1; }
    }

    // [12] System.Private.CoreLib/System.String::Split:System.String[](System.Char)
    public static void Subject_12()
    {
        try { var _ = (("hello".Split('A')).Length); }
        catch { _exitCode = 1; }
    }

    // [13] System.Private.CoreLib/System.String::ToUpper:System.String()
    public static void Subject_13()
    {
        try { var _ = (("hello".ToUpper()).Length); }
        catch { _exitCode = 1; }
    }

    // [14] System.Private.CoreLib/System.String::ToLower:System.String()
    public static void Subject_14()
    {
        try { var _ = (("hello".ToLower()).Length); }
        catch { _exitCode = 1; }
    }

    // [15] System.Private.CoreLib/System.String::Trim:System.String()
    public static void Subject_15()
    {
        try { var _ = (("hello".Trim()).Length); }
        catch { _exitCode = 1; }
    }

    // [16] System.Private.CoreLib/System.String::Join:System.String(System.String,System.String[])
    public static void Subject_16()
    {
        try { var _ = ((String.Join("hello", Array.Empty<System.String>())).Length); }
        catch { _exitCode = 1; }
    }

    // [17] System.Private.CoreLib/System.Char::IsDigit:System.Boolean(System.Char)
    public static void Subject_17()
    {
        try { var _ = ((Char.IsDigit('A')) ? 1 : 0); }
        catch { _exitCode = 1; }
    }

    // [18] System.Private.CoreLib/System.Char::IsLetter:System.Boolean(System.Char)
    public static void Subject_18()
    {
        try { var _ = ((Char.IsLetter('A')) ? 1 : 0); }
        catch { _exitCode = 1; }
    }

    // [19] System.Private.CoreLib/System.Char::IsWhiteSpace:System.Boolean(System.Char)
    public static void Subject_19()
    {
        try { var _ = ((Char.IsWhiteSpace('A')) ? 1 : 0); }
        catch { _exitCode = 1; }
    }

}