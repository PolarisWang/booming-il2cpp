// Auto-generated native-AOT entry point
// Family: family/System.Private.CoreLib/string/char/text/core
// Assembly: System.Private.CoreLib
// Variant: subjects

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
        try { if ("hello".IndexOf("hello") != "hello".IndexOf("hello")) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [1] System.Private.CoreLib/System.String::IndexOf:System.Int32(System.Char)
    public static void Subject_1()
    {
        try { if ("hello".IndexOf('A') != "hello".IndexOf('A')) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [2] System.Private.CoreLib/System.String::Substring:System.String(System.Int32)
    public static void Subject_2()
    {
        try { if ((("hello".Substring(1)).Length) != (("hello".Substring(1)).Length)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [3] System.Private.CoreLib/System.String::Substring:System.String(System.Int32,System.Int32)
    public static void Subject_3()
    {
        try { if ((("hello".Substring(1, 2)).Length) != (("hello".Substring(1, 2)).Length)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [4] System.Private.CoreLib/System.String::Compare:System.Int32(System.String,System.String)
    public static void Subject_4()
    {
        try { if (String.Compare("hello", "hello") != String.Compare("hello", "hello")) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [5] System.Private.CoreLib/System.String::Compare:System.Int32(System.String,System.Int32,System.String,System.Int32,System.Int32)
    public static void Subject_5()
    {
        try { if (string.Compare("hello", 0, "world", 0, 3) != string.Compare("hello", 0, "world", 0, 3)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [6] System.Private.CoreLib/System.String::Concat:System.String(System.String,System.String)
    public static void Subject_6()
    {
        try { if (((String.Concat("hello", "hello")).Length) != ((String.Concat("hello", "hello")).Length)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [7] System.Private.CoreLib/System.String::Concat:System.String(System.String,System.String,System.String)
    public static void Subject_7()
    {
        try { if (((String.Concat("hello", "hello", "hello")).Length) != ((String.Concat("hello", "hello", "hello")).Length)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [8] System.Private.CoreLib/System.String::Format:System.String(System.String,System.Object[])
    public static void Subject_8()
    {
        try { if (((String.Format("hello", Array.Empty<System.Object>())).Length) != ((String.Format("hello", Array.Empty<System.Object>())).Length)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [9] System.Private.CoreLib/System.String::StartsWith:System.Boolean(System.String)
    public static void Subject_9()
    {
        try { if ((("hello".StartsWith("hello")) ? 1 : 0) != (("hello".StartsWith("hello")) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [10] System.Private.CoreLib/System.String::Contains:System.Boolean(System.String)
    public static void Subject_10()
    {
        try { if ((("hello".Contains("hello")) ? 1 : 0) != (("hello".Contains("hello")) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [11] System.Private.CoreLib/System.String::Replace:System.String(System.String,System.String)
    public static void Subject_11()
    {
        try { if ((("hello".Replace("hello", "hello")).Length) != (("hello".Replace("hello", "hello")).Length)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [12] System.Private.CoreLib/System.String::Split:System.String[](System.Char)
    public static void Subject_12()
    {
        try { if ((("hello".Split('A')).Length) != (("hello".Split('A')).Length)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [13] System.Private.CoreLib/System.String::ToUpper:System.String()
    public static void Subject_13()
    {
        try { if ((("hello".ToUpper()).Length) != (("hello".ToUpper()).Length)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [14] System.Private.CoreLib/System.String::ToLower:System.String()
    public static void Subject_14()
    {
        try { if ((("hello".ToLower()).Length) != (("hello".ToLower()).Length)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [15] System.Private.CoreLib/System.String::Trim:System.String()
    public static void Subject_15()
    {
        try { if ((("hello".Trim()).Length) != (("hello".Trim()).Length)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [16] System.Private.CoreLib/System.String::Join:System.String(System.String,System.String[])
    public static void Subject_16()
    {
        try { if (((String.Join("hello", Array.Empty<System.String>())).Length) != ((String.Join("hello", Array.Empty<System.String>())).Length)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [17] System.Private.CoreLib/System.Char::IsDigit:System.Boolean(System.Char)
    public static void Subject_17()
    {
        try { if (((Char.IsDigit('A')) ? 1 : 0) != ((Char.IsDigit('A')) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [18] System.Private.CoreLib/System.Char::IsLetter:System.Boolean(System.Char)
    public static void Subject_18()
    {
        try { if (((Char.IsLetter('A')) ? 1 : 0) != ((Char.IsLetter('A')) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [19] System.Private.CoreLib/System.Char::IsWhiteSpace:System.Boolean(System.Char)
    public static void Subject_19()
    {
        try { if (((Char.IsWhiteSpace('A')) ? 1 : 0) != ((Char.IsWhiteSpace('A')) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

}