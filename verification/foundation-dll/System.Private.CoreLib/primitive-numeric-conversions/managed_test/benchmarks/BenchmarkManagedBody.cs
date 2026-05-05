// Auto-generated benchmark managed bodies
// Family: family/System.Private.CoreLib/primitive/numeric-conversions
using System;

namespace Chaos.Benchmark.primitive_numeric_conversions
{
    public static class BenchmarkManagedBody
    {
        internal static long BenchmarkChecksum;

        // System.Private.CoreLib/System.Convert::ToBoolean:System.Boolean(System.String)
        public static void System_Private_CoreLib_System_Convert_ToBoolean_System_Boolean_System_String()
        {
            BenchmarkChecksum += ((Convert.ToBoolean("hello")) ? 1 : 0);
        }

        // System.Private.CoreLib/System.Convert::ToByte:System.Byte(System.String)
        public static void System_Private_CoreLib_System_Convert_ToByte_System_Byte_System_String()
        {
            BenchmarkChecksum += (int)(Convert.ToByte("hello"));
        }

        // System.Private.CoreLib/System.Convert::ToInt16:System.Int16(System.String)
        public static void System_Private_CoreLib_System_Convert_ToInt16_System_Int16_System_String()
        {
            BenchmarkChecksum += (int)(Convert.ToInt16("hello"));
        }

        // System.Private.CoreLib/System.Convert::ToInt32:System.Int32(System.String)
        public static void System_Private_CoreLib_System_Convert_ToInt32_System_Int32_System_String()
        {
            BenchmarkChecksum += Convert.ToInt32("hello");
        }

        // System.Private.CoreLib/System.Convert::ToInt64:System.Int64(System.String)
        public static void System_Private_CoreLib_System_Convert_ToInt64_System_Int64_System_String()
        {
            BenchmarkChecksum += (int)(Convert.ToInt64("hello"));
        }

        // System.Private.CoreLib/System.Convert::ToSingle:System.Single(System.String)
        public static void System_Private_CoreLib_System_Convert_ToSingle_System_Single_System_String()
        {
            BenchmarkChecksum += (int)(Convert.ToSingle("hello"));
        }

        // System.Private.CoreLib/System.Convert::ToDouble:System.Double(System.String)
        public static void System_Private_CoreLib_System_Convert_ToDouble_System_Double_System_String()
        {
            BenchmarkChecksum += (int)(Convert.ToDouble("hello"));
        }

        // System.Private.CoreLib/System.Convert::ToDecimal:System.Decimal(System.String)
        public static void System_Private_CoreLib_System_Convert_ToDecimal_System_Decimal_System_String()
        {
            BenchmarkChecksum += (int)(Convert.ToDecimal("hello"));
        }

        // System.Private.CoreLib/System.Convert::ToString:System.String(System.Int32)
        public static void System_Private_CoreLib_System_Convert_ToString_System_String_System_Int32()
        {
            BenchmarkChecksum += ((Convert.ToString(42)).Length);
        }

        // System.Private.CoreLib/System.Convert::ToString:System.String(System.Double)
        public static void System_Private_CoreLib_System_Convert_ToString_System_String_System_Double()
        {
            BenchmarkChecksum += ((Convert.ToString(42.0)).Length);
        }

        // System.Private.CoreLib/System.Convert::ToDecimal:System.Decimal(System.Double)
        public static void System_Private_CoreLib_System_Convert_ToDecimal_System_Decimal_System_Double()
        {
            BenchmarkChecksum += (int)(Convert.ToDecimal(42.0));
        }

        // System.Private.CoreLib/System.Convert::ToInt32:System.Int32(System.Double)
        public static void System_Private_CoreLib_System_Convert_ToInt32_System_Int32_System_Double()
        {
            BenchmarkChecksum += Convert.ToInt32(42.0);
        }

        // System.Private.CoreLib/System.Int32::Parse:System.Int32(System.String)
        public static void System_Private_CoreLib_System_Int32_Parse_System_Int32_System_String()
        {
            BenchmarkChecksum += Int32.Parse("hello");
        }

        // System.Private.CoreLib/System.Int64::Parse:System.Int64(System.String)
        public static void System_Private_CoreLib_System_Int64_Parse_System_Int64_System_String()
        {
            BenchmarkChecksum += (int)(Int64.Parse("hello"));
        }

        // System.Private.CoreLib/System.Double::Parse:System.Double(System.String)
        public static void System_Private_CoreLib_System_Double_Parse_System_Double_System_String()
        {
            BenchmarkChecksum += (int)(Double.Parse("hello"));
        }

    }
}
