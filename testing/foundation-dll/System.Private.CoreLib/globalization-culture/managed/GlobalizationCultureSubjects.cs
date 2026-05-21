// Auto-generated native-AOT entry point
// Family: family/System.Private.CoreLib/globalization/culture
// Assembly: System.Private.CoreLib
// Variant: subjects

using System;
using System.Collections.Generic;
using System.Globalization;
using System.Linq;

public static partial class GlobalizationCultureSubjects
{
    // Inlined exit code — avoids SDK method call resolution in codegen
    public static int _exitCode;

    // [0] System.Private.CoreLib/System.Globalization.CultureInfo::get_CurrentCulture:System.Globalization.CultureInfo()
    public static void Subject_0()
    {
        try { if (((CultureInfo.CurrentCulture).GetHashCode()) != ((CultureInfo.CurrentCulture).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [1] System.Private.CoreLib/System.Globalization.CultureInfo::get_InvariantCulture:System.Globalization.CultureInfo()
    public static void Subject_1()
    {
        try { if (((CultureInfo.InvariantCulture).GetHashCode()) != ((CultureInfo.InvariantCulture).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [2] System.Private.CoreLib/System.Globalization.CultureInfo::GetCultureInfo:System.Globalization.CultureInfo(System.String)
    public static void Subject_2()
    {
        try { if (((CultureInfo.GetCultureInfo("")).GetHashCode()) != ((CultureInfo.GetCultureInfo("")).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [3] System.Private.CoreLib/System.Globalization.CultureInfo::get_Name:System.String()
    public static void Subject_3()
    {
        try { if (((CultureInfo.InvariantCulture.Name).Length) != ((CultureInfo.InvariantCulture.Name).Length)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [4] System.Private.CoreLib/System.Globalization.CultureInfo::get_DisplayName:System.String()
    public static void Subject_4()
    {
        try { if (((CultureInfo.InvariantCulture.DisplayName).Length) != ((CultureInfo.InvariantCulture.DisplayName).Length)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [5] System.Private.CoreLib/System.Globalization.CultureInfo::get_DateTimeFormat:System.Globalization.DateTimeFormatInfo()
    public static void Subject_5()
    {
        try { if (((CultureInfo.InvariantCulture.DateTimeFormat).GetHashCode()) != ((CultureInfo.InvariantCulture.DateTimeFormat).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [6] System.Private.CoreLib/System.Globalization.CultureInfo::get_NumberFormat:System.Globalization.NumberFormatInfo()
    public static void Subject_6()
    {
        try { if (((CultureInfo.InvariantCulture.NumberFormat).GetHashCode()) != ((CultureInfo.InvariantCulture.NumberFormat).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [7] System.Private.CoreLib/System.Globalization.CompareInfo::Compare:System.Int32(System.String,System.String)
    public static void Subject_7()
    {
        try { if (CultureInfo.InvariantCulture.CompareInfo.Compare("hello", "hello") != CultureInfo.InvariantCulture.CompareInfo.Compare("hello", "hello")) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [8] System.Private.CoreLib/System.Globalization.CompareInfo::Compare:System.Int32(System.String,System.String,System.Globalization.CompareOptions)
    public static void Subject_8()
    {
        try { if (CultureInfo.InvariantCulture.CompareInfo.Compare("hello", "hello", System.Globalization.CompareOptions.None) != CultureInfo.InvariantCulture.CompareInfo.Compare("hello", "hello", System.Globalization.CompareOptions.None)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [9] System.Private.CoreLib/System.Globalization.CompareInfo::IndexOf:System.Int32(System.String,System.String)
    public static void Subject_9()
    {
        try { if (CultureInfo.InvariantCulture.CompareInfo.IndexOf("hello", "hello") != CultureInfo.InvariantCulture.CompareInfo.IndexOf("hello", "hello")) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [10] System.Private.CoreLib/System.Globalization.TextInfo::ToUpper:System.String(System.String)
    public static void Subject_10()
    {
        try { if (((CultureInfo.InvariantCulture.TextInfo.ToUpper("hello")).Length) != ((CultureInfo.InvariantCulture.TextInfo.ToUpper("hello")).Length)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [11] System.Private.CoreLib/System.Globalization.TextInfo::ToLower:System.String(System.String)
    public static void Subject_11()
    {
        try { if (((CultureInfo.InvariantCulture.TextInfo.ToLower("hello")).Length) != ((CultureInfo.InvariantCulture.TextInfo.ToLower("hello")).Length)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [12] System.Private.CoreLib/System.Globalization.TextInfo::get_CultureName:System.String()
    public static void Subject_12()
    {
        try { if (((CultureInfo.InvariantCulture.TextInfo.CultureName).Length) != ((CultureInfo.InvariantCulture.TextInfo.CultureName).Length)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

}