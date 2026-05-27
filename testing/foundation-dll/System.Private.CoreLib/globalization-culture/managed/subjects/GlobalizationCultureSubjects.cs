// Auto-generated native-AOT entry point
// Family: family/System.Private.CoreLib/globalization/culture
// Assembly: System.Private.CoreLib
// Variant: subjects

using Chaos.TestFramework;
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
        try { var _ = ((CultureInfo.CurrentCulture).GetHashCode()); }
        catch { _exitCode = 1; }
    }

    // [1] System.Private.CoreLib/System.Globalization.CultureInfo::get_InvariantCulture:System.Globalization.CultureInfo()
    public static void Subject_1()
    {
        try { var _ = ((CultureInfo.InvariantCulture).GetHashCode()); }
        catch { _exitCode = 1; }
    }

    // [2] System.Private.CoreLib/System.Globalization.CultureInfo::GetCultureInfo:System.Globalization.CultureInfo(System.String)
    public static void Subject_2()
    {
        try { var _ = ((CultureInfo.GetCultureInfo("")).GetHashCode()); }
        catch { _exitCode = 1; }
    }

    // [3] System.Private.CoreLib/System.Globalization.CultureInfo::get_Name:System.String()
    public static void Subject_3()
    {
        try { var _ = ((CultureInfo.InvariantCulture.Name).Length); }
        catch { _exitCode = 1; }
    }

    // [4] System.Private.CoreLib/System.Globalization.CultureInfo::get_DisplayName:System.String()
    public static void Subject_4()
    {
        try { var _ = ((CultureInfo.InvariantCulture.DisplayName).Length); }
        catch { _exitCode = 1; }
    }

    // [5] System.Private.CoreLib/System.Globalization.CultureInfo::get_DateTimeFormat:System.Globalization.DateTimeFormatInfo()
    public static void Subject_5()
    {
        try { var _ = ((CultureInfo.InvariantCulture.DateTimeFormat).GetHashCode()); }
        catch { _exitCode = 1; }
    }

    // [6] System.Private.CoreLib/System.Globalization.CultureInfo::get_NumberFormat:System.Globalization.NumberFormatInfo()
    public static void Subject_6()
    {
        try { var _ = ((CultureInfo.InvariantCulture.NumberFormat).GetHashCode()); }
        catch { _exitCode = 1; }
    }

    // [7] System.Private.CoreLib/System.Globalization.CompareInfo::Compare:System.Int32(System.String,System.String)
    public static void Subject_7()
    {
        try { var _ = CultureInfo.InvariantCulture.CompareInfo.Compare("hello", "hello"); }
        catch { _exitCode = 1; }
    }

    // [8] System.Private.CoreLib/System.Globalization.CompareInfo::Compare:System.Int32(System.String,System.String,System.Globalization.CompareOptions)
    public static void Subject_8()
    {
        try { var _ = CultureInfo.InvariantCulture.CompareInfo.Compare("hello", "hello", System.Globalization.CompareOptions.None); }
        catch { _exitCode = 1; }
    }

    // [9] System.Private.CoreLib/System.Globalization.CompareInfo::IndexOf:System.Int32(System.String,System.String)
    public static void Subject_9()
    {
        try { var _ = CultureInfo.InvariantCulture.CompareInfo.IndexOf("hello", "hello"); }
        catch { _exitCode = 1; }
    }

    // [10] System.Private.CoreLib/System.Globalization.TextInfo::ToUpper:System.String(System.String)
    public static void Subject_10()
    {
        try { var _ = ((CultureInfo.InvariantCulture.TextInfo.ToUpper("hello")).Length); }
        catch { _exitCode = 1; }
    }

    // [11] System.Private.CoreLib/System.Globalization.TextInfo::ToLower:System.String(System.String)
    public static void Subject_11()
    {
        try { var _ = ((CultureInfo.InvariantCulture.TextInfo.ToLower("hello")).Length); }
        catch { _exitCode = 1; }
    }

    // [12] System.Private.CoreLib/System.Globalization.TextInfo::get_CultureName:System.String()
    public static void Subject_12()
    {
        try { var _ = ((CultureInfo.InvariantCulture.TextInfo.CultureName).Length); }
        catch { _exitCode = 1; }
    }

}