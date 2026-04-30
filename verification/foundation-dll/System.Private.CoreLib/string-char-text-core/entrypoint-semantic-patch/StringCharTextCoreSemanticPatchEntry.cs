// Auto-generated native-AOT entry point
// Family: family/System.Private.CoreLib/string/char/text/core
// Assembly: System.Private.CoreLib
// Variant: semantic-patch

using System;

public static class StringCharTextCoreSemanticPatchEntry
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
        return "world".IndexOf("99");
    }

    // [1] System.Private.CoreLib/System.String::IndexOf:System.Int32(System.Char)
    static int Method1()
    {
        return "world".IndexOf('Z');
    }

    // [2] System.Private.CoreLib/System.String::Substring:System.String(System.Int32)
    static int Method2()
    {
        return ("hello".Substring(1)?.GetHashCode() ?? 0);
    }

    // [3] System.Private.CoreLib/System.String::Substring:System.String(System.Int32,System.Int32)
    static int Method3()
    {
        return ("hello".Substring(1, 2)?.GetHashCode() ?? 0);
    }

    // [4] System.Private.CoreLib/System.String::Compare:System.Int32(System.String,System.String)
    static int Method4()
    {
        return String.Compare("99", "99");
    }

    // [5] System.Private.CoreLib/System.String::Compare:System.Int32(System.String,System.Int32,System.String,System.Int32,System.Int32)
    static int Method5()
    {
        return string.Compare("hello", 0, "world", 0, 3);
    }

    // [6] System.Private.CoreLib/System.String::Concat:System.String(System.String,System.String)
    static int Method6()
    {
        return (String.Concat("99", "99")?.GetHashCode() ?? 0);
    }

    // [7] System.Private.CoreLib/System.String::Concat:System.String(System.String,System.String,System.String)
    static int Method7()
    {
        return (String.Concat("99", "99", "99")?.GetHashCode() ?? 0);
    }

    // [8] System.Private.CoreLib/System.String::Format:System.String(System.String,System.Object[])
    static int Method8()
    {
        return (String.Format("99", Array.Empty<System.Object>())?.GetHashCode() ?? 0);
    }

    // [9] System.Private.CoreLib/System.String::StartsWith:System.Boolean(System.String)
    static int Method9()
    {
        return ("world".StartsWith("99") ? 1 : 0);
    }

    // [10] System.Private.CoreLib/System.String::Contains:System.Boolean(System.String)
    static int Method10()
    {
        return ("world".Contains("99") ? 1 : 0);
    }

    // [11] System.Private.CoreLib/System.String::Replace:System.String(System.String,System.String)
    static int Method11()
    {
        return ("world".Replace("99", "99")?.GetHashCode() ?? 0);
    }

    // [12] System.Private.CoreLib/System.String::Split:System.String[](System.Char)
    static int Method12()
    {
        return "world".Split('Z').GetHashCode();
    }

    // [13] System.Private.CoreLib/System.String::ToUpper:System.String()
    static int Method13()
    {
        return ("world".ToUpper()?.GetHashCode() ?? 0);
    }

    // [14] System.Private.CoreLib/System.String::ToLower:System.String()
    static int Method14()
    {
        return ("world".ToLower()?.GetHashCode() ?? 0);
    }

    // [15] System.Private.CoreLib/System.String::Trim:System.String()
    static int Method15()
    {
        return ("world".Trim()?.GetHashCode() ?? 0);
    }

    // [16] System.Private.CoreLib/System.String::Join:System.String(System.String,System.String[])
    static int Method16()
    {
        return (String.Join("99", Array.Empty<System.String>())?.GetHashCode() ?? 0);
    }

    // [17] System.Private.CoreLib/System.Char::IsDigit:System.Boolean(System.Char)
    static int Method17()
    {
        return (Char.IsDigit('Z') ? 1 : 0);
    }

    // [18] System.Private.CoreLib/System.Char::IsLetter:System.Boolean(System.Char)
    static int Method18()
    {
        return (Char.IsLetter('Z') ? 1 : 0);
    }

    // [19] System.Private.CoreLib/System.Char::IsWhiteSpace:System.Boolean(System.Char)
    static int Method19()
    {
        return (Char.IsWhiteSpace('Z') ? 1 : 0);
    }

}