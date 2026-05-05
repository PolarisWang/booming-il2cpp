// Auto-generated benchmark managed bodies
// Family: family/System.Private.CoreLib/globalization/culture
using System;

namespace Chaos.Benchmark.globalization_culture
{
    public static class BenchmarkManagedBody
    {
        internal static long BenchmarkChecksum;

        // System.Private.CoreLib/System.Globalization.CultureInfo::get_CurrentCulture:System.Globalization.CultureInfo()
        public static void System_Private_CoreLib_System_Globalization_CultureInfo_get_CurrentCulture_System_Globalization_CultureInfo()
        {
            BenchmarkChecksum += ((CultureInfo.CurrentCulture).GetHashCode());
        }

        // System.Private.CoreLib/System.Globalization.CultureInfo::get_InvariantCulture:System.Globalization.CultureInfo()
        public static void System_Private_CoreLib_System_Globalization_CultureInfo_get_InvariantCulture_System_Globalization_CultureInfo()
        {
            BenchmarkChecksum += ((CultureInfo.InvariantCulture).GetHashCode());
        }

        // System.Private.CoreLib/System.Globalization.CultureInfo::GetCultureInfo:System.Globalization.CultureInfo(System.String)
        public static void System_Private_CoreLib_System_Globalization_CultureInfo_GetCultureInfo_System_Globalization_CultureInfo_System_String()
        {
            BenchmarkChecksum += ((CultureInfo.GetCultureInfo("hello")).GetHashCode());
        }

        // System.Private.CoreLib/System.Globalization.CultureInfo::get_Name:System.String()
        public static void System_Private_CoreLib_System_Globalization_CultureInfo_get_Name_System_String()
        {
            BenchmarkChecksum += ((CultureInfo.InvariantCulture.Name).Length);
        }

        // System.Private.CoreLib/System.Globalization.CultureInfo::get_DisplayName:System.String()
        public static void System_Private_CoreLib_System_Globalization_CultureInfo_get_DisplayName_System_String()
        {
            BenchmarkChecksum += ((CultureInfo.InvariantCulture.DisplayName).Length);
        }

        // System.Private.CoreLib/System.Globalization.CultureInfo::get_DateTimeFormat:System.Globalization.DateTimeFormatInfo()
        public static void System_Private_CoreLib_System_Globalization_CultureInfo_get_DateTimeFormat_System_Globalization_DateTimeFormatInfo()
        {
            BenchmarkChecksum += ((CultureInfo.InvariantCulture.DateTimeFormat).GetHashCode());
        }

        // System.Private.CoreLib/System.Globalization.CultureInfo::get_NumberFormat:System.Globalization.NumberFormatInfo()
        public static void System_Private_CoreLib_System_Globalization_CultureInfo_get_NumberFormat_System_Globalization_NumberFormatInfo()
        {
            BenchmarkChecksum += ((CultureInfo.InvariantCulture.NumberFormat).GetHashCode());
        }

        // System.Private.CoreLib/System.Globalization.CompareInfo::Compare:System.Int32(System.String,System.String)
        public static void System_Private_CoreLib_System_Globalization_CompareInfo_Compare_System_Int32_System_String_System_String()
        {
            BenchmarkChecksum += CultureInfo.InvariantCulture.CompareInfo.Compare("hello", "hello");
        }

        // System.Private.CoreLib/System.Globalization.CompareInfo::Compare:System.Int32(System.String,System.String,System.Globalization.CompareOptions)
        public static void System_Private_CoreLib_System_Globalization_CompareInfo_Compare_System_Int32_System_String_System_String_System_Globalization_CompareOptions()
        {
            BenchmarkChecksum += CultureInfo.InvariantCulture.CompareInfo.Compare("hello", "hello", System.Globalization.CompareOptions.None);
        }

        // System.Private.CoreLib/System.Globalization.CompareInfo::IndexOf:System.Int32(System.String,System.String)
        public static void System_Private_CoreLib_System_Globalization_CompareInfo_IndexOf_System_Int32_System_String_System_String()
        {
            BenchmarkChecksum += CultureInfo.InvariantCulture.CompareInfo.IndexOf("hello", "hello");
        }

        // System.Private.CoreLib/System.Globalization.TextInfo::ToUpper:System.String(System.String)
        public static void System_Private_CoreLib_System_Globalization_TextInfo_ToUpper_System_String_System_String()
        {
            BenchmarkChecksum += ((CultureInfo.InvariantCulture.TextInfo.ToUpper("hello")).Length);
        }

        // System.Private.CoreLib/System.Globalization.TextInfo::ToLower:System.String(System.String)
        public static void System_Private_CoreLib_System_Globalization_TextInfo_ToLower_System_String_System_String()
        {
            BenchmarkChecksum += ((CultureInfo.InvariantCulture.TextInfo.ToLower("hello")).Length);
        }

        // System.Private.CoreLib/System.Globalization.TextInfo::get_CultureName:System.String()
        public static void System_Private_CoreLib_System_Globalization_TextInfo_get_CultureName_System_String()
        {
            BenchmarkChecksum += ((CultureInfo.InvariantCulture.TextInfo.CultureName).Length);
        }

    }
}
