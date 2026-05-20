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
        if (((CultureInfo.CurrentCulture).GetHashCode()) != 1837774308) _exitCode = 1;
    }

    // [1] System.Private.CoreLib/System.Globalization.CultureInfo::get_InvariantCulture:System.Globalization.CultureInfo()
    public static void Subject_1()
    {
        if (((CultureInfo.InvariantCulture).GetHashCode()) != 411464452) _exitCode = 1;
    }

    // [2] System.Private.CoreLib/System.Globalization.CultureInfo::GetCultureInfo:System.Globalization.CultureInfo(System.String)
    public static void Subject_2()
    {
        if (((CultureInfo.GetCultureInfo("")).GetHashCode()) != 411464452) _exitCode = 1;
    }

    // [3] System.Private.CoreLib/System.Globalization.CultureInfo::get_Name:System.String()
    public static void Subject_3()
    {
        if (((CultureInfo.InvariantCulture.Name).Length) != 0) _exitCode = 1;
    }

    // [4] System.Private.CoreLib/System.Globalization.CultureInfo::get_DisplayName:System.String()
    public static void Subject_4()
    {
        if (((CultureInfo.InvariantCulture.DisplayName).Length) != 38) _exitCode = 1;
    }

    // [5] System.Private.CoreLib/System.Globalization.CultureInfo::get_DateTimeFormat:System.Globalization.DateTimeFormatInfo()
    public static void Subject_5()
    {
        if (((CultureInfo.InvariantCulture.DateTimeFormat).GetHashCode()) != 35342034) _exitCode = 1;
    }

    // [6] System.Private.CoreLib/System.Globalization.CultureInfo::get_NumberFormat:System.Globalization.NumberFormatInfo()
    public static void Subject_6()
    {
        if (((CultureInfo.InvariantCulture.NumberFormat).GetHashCode()) != 56793269) _exitCode = 1;
    }

    // [7] System.Private.CoreLib/System.Globalization.CompareInfo::Compare:System.Int32(System.String,System.String)
    public static void Subject_7()
    {
        if (CultureInfo.InvariantCulture.CompareInfo.Compare("hello", "hello") != 0) _exitCode = 1;
    }

    // [8] System.Private.CoreLib/System.Globalization.CompareInfo::Compare:System.Int32(System.String,System.String,System.Globalization.CompareOptions)
    public static void Subject_8()
    {
        if (CultureInfo.InvariantCulture.CompareInfo.Compare("hello", "hello", System.Globalization.CompareOptions.None) != 0) _exitCode = 1;
    }

    // [9] System.Private.CoreLib/System.Globalization.CompareInfo::IndexOf:System.Int32(System.String,System.String)
    public static void Subject_9()
    {
        if (CultureInfo.InvariantCulture.CompareInfo.IndexOf("hello", "hello") != 0) _exitCode = 1;
    }

    // [10] System.Private.CoreLib/System.Globalization.TextInfo::ToUpper:System.String(System.String)
    public static void Subject_10()
    {
        if (((CultureInfo.InvariantCulture.TextInfo.ToUpper("hello")).Length) != 5) _exitCode = 1;
    }

    // [11] System.Private.CoreLib/System.Globalization.TextInfo::ToLower:System.String(System.String)
    public static void Subject_11()
    {
        if (((CultureInfo.InvariantCulture.TextInfo.ToLower("hello")).Length) != 5) _exitCode = 1;
    }

    // [12] System.Private.CoreLib/System.Globalization.TextInfo::get_CultureName:System.String()
    public static void Subject_12()
    {
        if (((CultureInfo.InvariantCulture.TextInfo.CultureName).Length) != 0) _exitCode = 1;
    }

}