// Auto-generated native-AOT entry point
// Family: family/System.Private.CoreLib/string/char/text/core
// Assembly: System.Private.CoreLib
// Variant: benchmark

using System;

public static class StringCharTextCoreNativeEntry
{
    public static int Run(int entryIndex)
    {
        switch (entryIndex)
        {
            case 0: return Method0();
            case 1: return Method1();
            case 2: return Method2();
            case 3: return Method3();
            case 4: return Method4();
            case 5: return Method5();
            case 6: return Method6();
            case 7: return Method7();
            case 8: return Method8();
            case 9: return Method9();
            case 10: return Method10();
            case 11: return Method11();
            case 12: return Method12();
            case 13: return Method13();
            case 14: return Method14();
            case 15: return Method15();
            case 16: return Method16();
            case 17: return Method17();
            case 18: return Method18();
            case 19: return Method19();
            default: return -1;
        }
    }

    // [0] System.Private.CoreLib/System.String::IndexOf:System.Int32(System.String)
    static int Method0()
    {
        return "hello".IndexOf("hello");
    }

    // [1] System.Private.CoreLib/System.String::IndexOf:System.Int32(System.Char)
    static int Method1()
    {
        return "hello".IndexOf('A');
    }

    // [2] System.Private.CoreLib/System.String::Substring:System.String(System.Int32)
    static int Method2()
    {
        return (int)("hello".Substring(1));
    }

    // [3] System.Private.CoreLib/System.String::Substring:System.String(System.Int32,System.Int32)
    static int Method3()
    {
        return (int)("hello".Substring(1, 2));
    }

    // [4] System.Private.CoreLib/System.String::Compare:System.Int32(System.String,System.String)
    static int Method4()
    {
        return String.Compare("hello", "hello");
    }

    // [5] System.Private.CoreLib/System.String::Compare:System.Int32(System.String,System.Int32,System.String,System.Int32,System.Int32)
    static int Method5()
    {
        return string.Compare("hello", 0, "world", 0, 3);
    }

    // [6] System.Private.CoreLib/System.String::Concat:System.String(System.String,System.String)
    static int Method6()
    {
        return (int)(String.Concat("hello", "hello"));
    }

    // [7] System.Private.CoreLib/System.String::Concat:System.String(System.String,System.String,System.String)
    static int Method7()
    {
        return (int)(String.Concat("hello", "hello", "hello"));
    }

    // [8] System.Private.CoreLib/System.String::Format:System.String(System.String,System.Object[])
    static int Method8()
    {
        return (int)(String.Format("hello", Array.Empty<System.Object>()));
    }

    // [9] System.Private.CoreLib/System.String::StartsWith:System.Boolean(System.String)
    static int Method9()
    {
        return (int)("hello".StartsWith("hello"));
    }

    // [10] System.Private.CoreLib/System.String::Contains:System.Boolean(System.String)
    static int Method10()
    {
        return (int)("hello".Contains("hello"));
    }

    // [11] System.Private.CoreLib/System.String::Replace:System.String(System.String,System.String)
    static int Method11()
    {
        return (int)("hello".Replace("hello", "hello"));
    }

    // [12] System.Private.CoreLib/System.String::Split:System.String[](System.Char)
    static int Method12()
    {
        return (int)("hello".Split('A'));
    }

    // [13] System.Private.CoreLib/System.String::ToUpper:System.String()
    static int Method13()
    {
        return (int)("hello".ToUpper());
    }

    // [14] System.Private.CoreLib/System.String::ToLower:System.String()
    static int Method14()
    {
        return (int)("hello".ToLower());
    }

    // [15] System.Private.CoreLib/System.String::Trim:System.String()
    static int Method15()
    {
        return (int)("hello".Trim());
    }

    // [16] System.Private.CoreLib/System.String::Join:System.String(System.String,System.String[])
    static int Method16()
    {
        return (int)(String.Join("hello", Array.Empty<System.String>()));
    }

    // [17] System.Private.CoreLib/System.Char::IsDigit:System.Boolean(System.Char)
    static int Method17()
    {
        return (int)(Char.IsDigit('A'));
    }

    // [18] System.Private.CoreLib/System.Char::IsLetter:System.Boolean(System.Char)
    static int Method18()
    {
        return (int)(Char.IsLetter('A'));
    }

    // [19] System.Private.CoreLib/System.Char::IsWhiteSpace:System.Boolean(System.Char)
    static int Method19()
    {
        return (int)(Char.IsWhiteSpace('A'));
    }

}