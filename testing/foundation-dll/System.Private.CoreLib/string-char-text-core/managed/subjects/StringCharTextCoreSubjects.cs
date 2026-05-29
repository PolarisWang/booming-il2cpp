// Auto-generated native-AOT entry point
// Family: family/System.Private.CoreLib/string/char/text/core
// Assembly: System.Private.CoreLib
// Variant: subjects

using Chaos.TestFramework;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Threading;

public static partial class StringCharTextCoreSubjects
{
    // Inlined exit code — avoids SDK method call resolution in codegen
    public static int _exitCode;

    // [0] System.Private.CoreLib/System.String::IndexOf:System.Int32(System.String)
    public static void Subject_0()
    {
        // non-callable: System.Private.CoreLib/System.String::IndexOf:System.Int32(System.String)
    }

    // [1] System.Private.CoreLib/System.String::IndexOf:System.Int32(System.Char)
    public static void Subject_1()
    {
        // non-callable: System.Private.CoreLib/System.String::IndexOf:System.Int32(System.Char)
    }

    // [2] System.Private.CoreLib/System.String::Substring:System.String(System.Int32)
    public static void Subject_2()
    {
        try { _ = (("hello".Substring(1)).Length); }
        catch { _exitCode = 1; }
    }

    // [3] System.Private.CoreLib/System.String::Substring:System.String(System.Int32,System.Int32)
    public static void Subject_3()
    {
        try { _ = (("hello".Substring(1, 2)).Length); }
        catch { _exitCode = 1; }
    }

    // [4] System.Private.CoreLib/System.String::Compare:System.Int32(System.String,System.String)
    public static void Subject_4()
    {
        try { _ = String.Compare("hello", "hello"); }
        catch { _exitCode = 1; }
    }

    // [5] System.Private.CoreLib/System.String::Compare:System.Int32(System.String,System.Int32,System.String,System.Int32,System.Int32)
    public static void Subject_5()
    {
        try { _ = string.Compare("hello", 0, "world", 0, 3); }
        catch { _exitCode = 1; }
    }

    // [6] System.Private.CoreLib/System.String::Concat:System.String(System.String,System.String)
    public static void Subject_6()
    {
        try { _ = ((String.Concat("hello", "hello")).Length); }
        catch { _exitCode = 1; }
    }

    // [7] System.Private.CoreLib/System.String::Concat:System.String(System.String,System.String,System.String)
    public static void Subject_7()
    {
        try { _ = ((String.Concat("hello", "hello", "hello")).Length); }
        catch { _exitCode = 1; }
    }

    // [8] System.Private.CoreLib/System.String::Format:System.String(System.String,System.Object[])
    public static void Subject_8()
    {
        try { _ = ((String.Format("hello", Array.Empty<System.Object>())).Length); }
        catch { _exitCode = 1; }
    }

    // [9] System.Private.CoreLib/System.String::StartsWith:System.Boolean(System.String)
    public static void Subject_9()
    {
        // non-callable: System.Private.CoreLib/System.String::StartsWith:System.Boolean(System.String)
    }

    // [10] System.Private.CoreLib/System.String::Contains:System.Boolean(System.String)
    public static void Subject_10()
    {
        // non-callable: System.Private.CoreLib/System.String::Contains:System.Boolean(System.String)
    }

    // [11] System.Private.CoreLib/System.String::Replace:System.String(System.String,System.String)
    public static void Subject_11()
    {
        // non-callable: System.Private.CoreLib/System.String::Replace:System.String(System.String,System.String)
    }

    // [12] System.Private.CoreLib/System.String::Split:System.String[](System.Char)
    public static void Subject_12()
    {
        // non-callable: System.Private.CoreLib/System.String::Split:System.String[](System.Char)
    }

    // [13] System.Private.CoreLib/System.String::ToUpper:System.String()
    public static void Subject_13()
    {
        // non-callable: System.Private.CoreLib/System.String::ToUpper:System.String()
    }

    // [14] System.Private.CoreLib/System.String::ToLower:System.String()
    public static void Subject_14()
    {
        // non-callable: System.Private.CoreLib/System.String::ToLower:System.String()
    }

    // [15] System.Private.CoreLib/System.String::Trim:System.String()
    public static void Subject_15()
    {
        // non-callable: System.Private.CoreLib/System.String::Trim:System.String()
    }

    // [16] System.Private.CoreLib/System.String::Join:System.String(System.String,System.String[])
    public static void Subject_16()
    {
        try { _ = ((String.Join("hello", Array.Empty<System.String>())).Length); }
        catch { _exitCode = 1; }
    }

    // [17] System.Private.CoreLib/System.Char::IsDigit:System.Boolean(System.Char)
    public static void Subject_17()
    {
        // non-callable: System.Private.CoreLib/System.Char::IsDigit:System.Boolean(System.Char)
    }

    // [18] System.Private.CoreLib/System.Char::IsLetter:System.Boolean(System.Char)
    public static void Subject_18()
    {
        // non-callable: System.Private.CoreLib/System.Char::IsLetter:System.Boolean(System.Char)
    }

    // [19] System.Private.CoreLib/System.Char::IsWhiteSpace:System.Boolean(System.Char)
    public static void Subject_19()
    {
        // non-callable: System.Private.CoreLib/System.Char::IsWhiteSpace:System.Boolean(System.Char)
    }

}