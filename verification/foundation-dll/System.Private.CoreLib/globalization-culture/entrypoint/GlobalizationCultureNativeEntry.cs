// Auto-generated native-AOT entry point
// Family: family/System.Private.CoreLib/globalization/culture
// Assembly: System.Private.CoreLib
// Variant: benchmark

using System;
using System.Globalization;

public static class GlobalizationCultureNativeEntry
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
            default: return -1;
        }
    }

    // [0] System.Private.CoreLib/System.Globalization.CultureInfo::get_CurrentCulture:System.Globalization.CultureInfo()
    static int Method0()
    {
        return CultureInfo.CurrentCulture.GetHashCode();
    }

    // [1] System.Private.CoreLib/System.Globalization.CultureInfo::get_InvariantCulture:System.Globalization.CultureInfo()
    static int Method1()
    {
        return CultureInfo.InvariantCulture.GetHashCode();
    }

    // [2] System.Private.CoreLib/System.Globalization.CultureInfo::GetCultureInfo:System.Globalization.CultureInfo(System.String)
    static int Method2()
    {
        // TODO: System.Private.CoreLib/System.Globalization.CultureInfo::GetCultureInfo:System.Globalization.CultureInfo(System.String) could not be auto-generated
        return 0;
    }

    // [3] System.Private.CoreLib/System.Globalization.CultureInfo::get_Name:System.String()
    static int Method3()
    {
        return (CultureInfo.InvariantCulture.Name?.GetHashCode() ?? 0);
    }

    // [4] System.Private.CoreLib/System.Globalization.CultureInfo::get_DisplayName:System.String()
    static int Method4()
    {
        return (CultureInfo.InvariantCulture.DisplayName?.GetHashCode() ?? 0);
    }

    // [5] System.Private.CoreLib/System.Globalization.CultureInfo::get_DateTimeFormat:System.Globalization.DateTimeFormatInfo()
    static int Method5()
    {
        return CultureInfo.InvariantCulture.DateTimeFormat.GetHashCode();
    }

    // [6] System.Private.CoreLib/System.Globalization.CultureInfo::get_NumberFormat:System.Globalization.NumberFormatInfo()
    static int Method6()
    {
        return CultureInfo.InvariantCulture.NumberFormat.GetHashCode();
    }

    // [7] System.Private.CoreLib/System.Globalization.CompareInfo::Compare:System.Int32(System.String,System.String)
    static int Method7()
    {
        return CultureInfo.InvariantCulture.CompareInfo.Compare("hello", "hello");
    }

    // [8] System.Private.CoreLib/System.Globalization.CompareInfo::Compare:System.Int32(System.String,System.String,System.Globalization.CompareOptions)
    static int Method8()
    {
        return CultureInfo.InvariantCulture.CompareInfo.Compare("hello", "hello", System.Globalization.CompareOptions.None);
    }

    // [9] System.Private.CoreLib/System.Globalization.CompareInfo::IndexOf:System.Int32(System.String,System.String)
    static int Method9()
    {
        return CultureInfo.InvariantCulture.CompareInfo.IndexOf("hello", "hello");
    }

    // [10] System.Private.CoreLib/System.Globalization.TextInfo::ToUpper:System.String(System.String)
    static int Method10()
    {
        return (CultureInfo.InvariantCulture.TextInfo.ToUpper("hello")?.GetHashCode() ?? 0);
    }

    // [11] System.Private.CoreLib/System.Globalization.TextInfo::ToLower:System.String(System.String)
    static int Method11()
    {
        return (CultureInfo.InvariantCulture.TextInfo.ToLower("hello")?.GetHashCode() ?? 0);
    }

    // [12] System.Private.CoreLib/System.Globalization.TextInfo::get_CultureName:System.String()
    static int Method12()
    {
        return (CultureInfo.InvariantCulture.TextInfo.CultureName?.GetHashCode() ?? 0);
    }

}