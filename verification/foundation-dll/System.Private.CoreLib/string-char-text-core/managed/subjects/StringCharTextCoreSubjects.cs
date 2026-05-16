// Auto-generated native-AOT entry point
// Family: family/System.Private.CoreLib/string/char/text/core
// Assembly: System.Private.CoreLib
// Variant: subjects

using System;

public static partial class StringCharTextCoreSubjects
{
    // Inlined exit code — avoids SDK method call resolution in codegen
    public static int _exitCode;

    // [0] System.Private.CoreLib/System.String::IndexOf:System.Int32(System.String)
    public static void Subject_0()
    {
        if ("hello".IndexOf("hello") != "hello".IndexOf("hello")) _exitCode = 1;
    }

    // [1] System.Private.CoreLib/System.String::IndexOf:System.Int32(System.Char)
    public static void Subject_1()
    {
        if ("hello".IndexOf('A') != "hello".IndexOf('A')) _exitCode = 1;
    }

    // [2] System.Private.CoreLib/System.String::Substring:System.String(System.Int32)
    public static void Subject_2()
    {
        if ((("hello".Substring(1)).Length) != (("hello".Substring(1)).Length)) _exitCode = 1;
    }

    // [3] System.Private.CoreLib/System.String::Substring:System.String(System.Int32,System.Int32)
    public static void Subject_3()
    {
        if ((("hello".Substring(1, 2)).Length) != (("hello".Substring(1, 2)).Length)) _exitCode = 1;
    }

    // [4] System.Private.CoreLib/System.String::Compare:System.Int32(System.String,System.String)
    public static void Subject_4()
    {
        if (String.Compare("hello", "hello") != String.Compare("hello", "hello")) _exitCode = 1;
    }

    // [5] System.Private.CoreLib/System.String::Compare:System.Int32(System.String,System.Int32,System.String,System.Int32,System.Int32)
    public static void Subject_5()
    {
        if (string.Compare("hello", 0, "world", 0, 3) != string.Compare("hello", 0, "world", 0, 3)) _exitCode = 1;
    }

    // [6] System.Private.CoreLib/System.String::Concat:System.String(System.String,System.String)
    public static void Subject_6()
    {
        if (((String.Concat("hello", "hello")).Length) != ((String.Concat("hello", "hello")).Length)) _exitCode = 1;
    }

    // [7] System.Private.CoreLib/System.String::Concat:System.String(System.String,System.String,System.String)
    public static void Subject_7()
    {
        if (((String.Concat("hello", "hello", "hello")).Length) != ((String.Concat("hello", "hello", "hello")).Length)) _exitCode = 1;
    }

    // [8] System.Private.CoreLib/System.String::Format:System.String(System.String,System.Object[])
    public static void Subject_8()
    {
        if (((String.Format("hello", Array.Empty<System.Object>())).Length) != ((String.Format("hello", Array.Empty<System.Object>())).Length)) _exitCode = 1;
    }

    // [9] System.Private.CoreLib/System.String::StartsWith:System.Boolean(System.String)
    public static void Subject_9()
    {
        if ((("hello".StartsWith("hello")) ? 1 : 0) != (("hello".StartsWith("hello")) ? 1 : 0)) _exitCode = 1;
    }

    // [10] System.Private.CoreLib/System.String::Contains:System.Boolean(System.String)
    public static void Subject_10()
    {
        if ((("hello".Contains("hello")) ? 1 : 0) != (("hello".Contains("hello")) ? 1 : 0)) _exitCode = 1;
    }

    // [11] System.Private.CoreLib/System.String::Replace:System.String(System.String,System.String)
    public static void Subject_11()
    {
        if ((("hello".Replace("hello", "hello")).Length) != (("hello".Replace("hello", "hello")).Length)) _exitCode = 1;
    }

    // [12] System.Private.CoreLib/System.String::Split:System.String[](System.Char)
    public static void Subject_12()
    {
        if ((("hello".Split('A')).Length) != (("hello".Split('A')).Length)) _exitCode = 1;
    }

    // [13] System.Private.CoreLib/System.String::ToUpper:System.String()
    public static void Subject_13()
    {
        if ((("hello".ToUpper()).Length) != (("hello".ToUpper()).Length)) _exitCode = 1;
    }

    // [14] System.Private.CoreLib/System.String::ToLower:System.String()
    public static void Subject_14()
    {
        if ((("hello".ToLower()).Length) != (("hello".ToLower()).Length)) _exitCode = 1;
    }

    // [15] System.Private.CoreLib/System.String::Trim:System.String()
    public static void Subject_15()
    {
        if ((("hello".Trim()).Length) != (("hello".Trim()).Length)) _exitCode = 1;
    }

    // [16] System.Private.CoreLib/System.String::Join:System.String(System.String,System.String[])
    public static void Subject_16()
    {
        if (((String.Join("hello", Array.Empty<System.String>())).Length) != ((String.Join("hello", Array.Empty<System.String>())).Length)) _exitCode = 1;
    }

    // [17] System.Private.CoreLib/System.Char::IsDigit:System.Boolean(System.Char)
    public static void Subject_17()
    {
        if (((Char.IsDigit('A')) ? 1 : 0) != ((Char.IsDigit('A')) ? 1 : 0)) _exitCode = 1;
    }

    // [18] System.Private.CoreLib/System.Char::IsLetter:System.Boolean(System.Char)
    public static void Subject_18()
    {
        if (((Char.IsLetter('A')) ? 1 : 0) != ((Char.IsLetter('A')) ? 1 : 0)) _exitCode = 1;
    }

    // [19] System.Private.CoreLib/System.Char::IsWhiteSpace:System.Boolean(System.Char)
    public static void Subject_19()
    {
        if (((Char.IsWhiteSpace('A')) ? 1 : 0) != ((Char.IsWhiteSpace('A')) ? 1 : 0)) _exitCode = 1;
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
            case 18: Subject_18(); break;
            case 19: Subject_19(); break;
        }
    }

}