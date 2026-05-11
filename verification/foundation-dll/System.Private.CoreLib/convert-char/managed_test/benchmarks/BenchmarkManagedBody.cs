// Auto-generated benchmark managed bodies
// Family: family/System.Private.CoreLib/convert/char
using System;

namespace Chaos.Benchmark.convert_char
{
    public static class BenchmarkManagedBody
    {
        internal static long BenchmarkChecksum;

        // System.Private.CoreLib/System.Convert::ToChar:System.Char(System.Boolean)
        public static void System_Private_CoreLib_System_Convert_ToChar_System_Char_System_Boolean()
        {
            BenchmarkChecksum += (int)(Convert.ToChar(true));
        }

        // System.Private.CoreLib/System.Convert::ToChar:System.Char(System.Byte)
        public static void System_Private_CoreLib_System_Convert_ToChar_System_Char_System_Byte()
        {
            BenchmarkChecksum += (int)(Convert.ToChar((byte)42));
        }

        // System.Private.CoreLib/System.Convert::ToChar:System.Char(System.Char)
        public static void System_Private_CoreLib_System_Convert_ToChar_System_Char_System_Char()
        {
            BenchmarkChecksum += (int)(Convert.ToChar('A'));
        }

        // System.Private.CoreLib/System.Convert::ToChar:System.Char(System.DateTime)
        public static void System_Private_CoreLib_System_Convert_ToChar_System_Char_System_DateTime()
        {
            BenchmarkChecksum += (int)(Convert.ToChar(DateTime.UtcNow));
        }

        // System.Private.CoreLib/System.Convert::ToChar:System.Char(System.Decimal)
        public static void System_Private_CoreLib_System_Convert_ToChar_System_Char_System_Decimal()
        {
            BenchmarkChecksum += (int)(Convert.ToChar(42m));
        }

        // System.Private.CoreLib/System.Convert::ToChar:System.Char(System.Double)
        public static void System_Private_CoreLib_System_Convert_ToChar_System_Char_System_Double()
        {
            BenchmarkChecksum += (int)(Convert.ToChar(42.0));
        }

        // System.Private.CoreLib/System.Convert::ToChar:System.Char(System.Int16)
        public static void System_Private_CoreLib_System_Convert_ToChar_System_Char_System_Int16()
        {
            BenchmarkChecksum += (int)(Convert.ToChar((short)42));
        }

        // System.Private.CoreLib/System.Convert::ToChar:System.Char(System.Int32)
        public static void System_Private_CoreLib_System_Convert_ToChar_System_Char_System_Int32()
        {
            BenchmarkChecksum += (int)(Convert.ToChar(42));
        }

        // System.Private.CoreLib/System.Convert::ToChar:System.Char(System.Int64)
        public static void System_Private_CoreLib_System_Convert_ToChar_System_Char_System_Int64()
        {
            BenchmarkChecksum += (int)(Convert.ToChar(42L));
        }

        // System.Private.CoreLib/System.Convert::ToChar:System.Char(System.Object)
        public static void System_Private_CoreLib_System_Convert_ToChar_System_Char_System_Object()
        {
            BenchmarkChecksum += (int)(Convert.ToChar(42));
        }

        // System.Private.CoreLib/System.Convert::ToChar:System.Char(System.Object,System.IFormatProvider)
        public static void System_Private_CoreLib_System_Convert_ToChar_System_Char_System_Object_System_IFormatProvider()
        {
            BenchmarkChecksum += (int)(Convert.ToChar(42, null));
        }

        // System.Private.CoreLib/System.Convert::ToChar:System.Char(System.SByte)
        public static void System_Private_CoreLib_System_Convert_ToChar_System_Char_System_SByte()
        {
            BenchmarkChecksum += (int)(Convert.ToChar((sbyte)42));
        }

        // System.Private.CoreLib/System.Convert::ToChar:System.Char(System.Single)
        public static void System_Private_CoreLib_System_Convert_ToChar_System_Char_System_Single()
        {
            BenchmarkChecksum += (int)(Convert.ToChar(42.0f));
        }

        // System.Private.CoreLib/System.Convert::ToChar:System.Char(System.String)
        public static void System_Private_CoreLib_System_Convert_ToChar_System_Char_System_String()
        {
            BenchmarkChecksum += (int)(Convert.ToChar("hello"));
        }

        // System.Private.CoreLib/System.Convert::ToChar:System.Char(System.String,System.IFormatProvider)
        public static void System_Private_CoreLib_System_Convert_ToChar_System_Char_System_String_System_IFormatProvider()
        {
            BenchmarkChecksum += (int)(Convert.ToChar("hello", null));
        }

        // System.Private.CoreLib/System.Convert::ToChar:System.Char(System.UInt16)
        public static void System_Private_CoreLib_System_Convert_ToChar_System_Char_System_UInt16()
        {
            BenchmarkChecksum += (int)(Convert.ToChar((ushort)42));
        }

        // System.Private.CoreLib/System.Convert::ToChar:System.Char(System.UInt32)
        public static void System_Private_CoreLib_System_Convert_ToChar_System_Char_System_UInt32()
        {
            BenchmarkChecksum += (int)(Convert.ToChar(42u));
        }

        // System.Private.CoreLib/System.Convert::ToChar:System.Char(System.UInt64)
        public static void System_Private_CoreLib_System_Convert_ToChar_System_Char_System_UInt64()
        {
            BenchmarkChecksum += (int)(Convert.ToChar(42uL));
        }

    }
}
