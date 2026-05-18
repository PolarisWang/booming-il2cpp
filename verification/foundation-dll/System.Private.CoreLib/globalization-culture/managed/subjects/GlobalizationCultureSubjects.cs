// Auto-generated native-AOT entry point
// Family: family/System.Private.CoreLib/globalization/culture
// Assembly: System.Private.CoreLib
// Variant: subjects

using System;
using System.Globalization;

public static partial class GlobalizationCultureSubjects
{
    // [0] System.Private.CoreLib/System.Globalization.CultureInfo::get_CurrentCulture:System.Globalization.CultureInfo()
    public static int Subject_0()
    {
        return ((CultureInfo.CurrentCulture).GetHashCode());
    }

    // [1] System.Private.CoreLib/System.Globalization.CultureInfo::get_InvariantCulture:System.Globalization.CultureInfo()
    public static int Subject_1()
    {
        return ((CultureInfo.InvariantCulture).GetHashCode());
    }

    // [2] System.Private.CoreLib/System.Globalization.CultureInfo::GetCultureInfo:System.Globalization.CultureInfo(System.String)
    public static int Subject_2()
    {
        return -1;  // cannot auto-generate call
    }

    // [3] System.Private.CoreLib/System.Globalization.CultureInfo::get_Name:System.String()
    public static int Subject_3()
    {
        return ((CultureInfo.InvariantCulture.Name).Length);
    }

    // [4] System.Private.CoreLib/System.Globalization.CultureInfo::get_DisplayName:System.String()
    public static int Subject_4()
    {
        return ((CultureInfo.InvariantCulture.DisplayName).Length);
    }

    // [5] System.Private.CoreLib/System.Globalization.CultureInfo::get_DateTimeFormat:System.Globalization.DateTimeFormatInfo()
    public static int Subject_5()
    {
        return ((CultureInfo.InvariantCulture.DateTimeFormat).GetHashCode());
    }

    // [6] System.Private.CoreLib/System.Globalization.CultureInfo::get_NumberFormat:System.Globalization.NumberFormatInfo()
    public static int Subject_6()
    {
        return ((CultureInfo.InvariantCulture.NumberFormat).GetHashCode());
    }

    // [7] System.Private.CoreLib/System.Globalization.CompareInfo::Compare:System.Int32(System.String,System.String)
    public static int Subject_7()
    {
        return CultureInfo.InvariantCulture.CompareInfo.Compare("hello", "hello");
    }

    // [8] System.Private.CoreLib/System.Globalization.CompareInfo::Compare:System.Int32(System.String,System.String,System.Globalization.CompareOptions)
    public static int Subject_8()
    {
        return CultureInfo.InvariantCulture.CompareInfo.Compare("hello", "hello", System.Globalization.CompareOptions.None);
    }

    // [9] System.Private.CoreLib/System.Globalization.CompareInfo::IndexOf:System.Int32(System.String,System.String)
    public static int Subject_9()
    {
        return CultureInfo.InvariantCulture.CompareInfo.IndexOf("hello", "hello");
    }

    // [10] System.Private.CoreLib/System.Globalization.TextInfo::ToUpper:System.String(System.String)
    public static int Subject_10()
    {
        return ((CultureInfo.InvariantCulture.TextInfo.ToUpper("hello")).Length);
    }

    // [11] System.Private.CoreLib/System.Globalization.TextInfo::ToLower:System.String(System.String)
    public static int Subject_11()
    {
        return ((CultureInfo.InvariantCulture.TextInfo.ToLower("hello")).Length);
    }

    // [12] System.Private.CoreLib/System.Globalization.TextInfo::get_CultureName:System.String()
    public static int Subject_12()
    {
        return ((CultureInfo.InvariantCulture.TextInfo.CultureName).Length);
    }

}