// Auto-generated benchmark managed bodies
// Family: family/System.Private.CoreLib/string/char-text-core
using System;

namespace Chaos.Benchmark.string_char_text_core
{
    public static class BenchmarkManagedBody
    {
        internal static long BenchmarkChecksum;

        // System.Private.CoreLib/System.String::IndexOf:System.Int32(System.String)
        public static void System_Private_CoreLib_System_String_IndexOf_System_Int32_System_String()
        {
            BenchmarkChecksum += "hello".IndexOf("hello");
        }

        // System.Private.CoreLib/System.String::IndexOf:System.Int32(System.Char)
        public static void System_Private_CoreLib_System_String_IndexOf_System_Int32_System_Char()
        {
            BenchmarkChecksum += "hello".IndexOf('A');
        }

        // System.Private.CoreLib/System.String::Substring:System.String(System.Int32)
        public static void System_Private_CoreLib_System_String_Substring_System_String_System_Int32()
        {
            BenchmarkChecksum += (("hello".Substring(1)).Length);
        }

        // System.Private.CoreLib/System.String::Substring:System.String(System.Int32,System.Int32)
        public static void System_Private_CoreLib_System_String_Substring_System_String_System_Int32_System_Int32()
        {
            BenchmarkChecksum += (("hello".Substring(1, 2)).Length);
        }

        // System.Private.CoreLib/System.String::Compare:System.Int32(System.String,System.String)
        public static void System_Private_CoreLib_System_String_Compare_System_Int32_System_String_System_String()
        {
            BenchmarkChecksum += String.Compare("hello", "hello");
        }

        // System.Private.CoreLib/System.String::Compare:System.Int32(System.String,System.Int32,System.String,System.Int32,System.Int32)
        public static void System_Private_CoreLib_System_String_Compare_System_Int32_System_String_System_Int32_System_String_System_Int32_System_Int32()
        {
            BenchmarkChecksum += string.Compare("hello", 0, "world", 0, 3);
        }

        // System.Private.CoreLib/System.String::Concat:System.String(System.String,System.String)
        public static void System_Private_CoreLib_System_String_Concat_System_String_System_String_System_String()
        {
            BenchmarkChecksum += ((String.Concat("hello", "hello")).Length);
        }

        // System.Private.CoreLib/System.String::Concat:System.String(System.String,System.String,System.String)
        public static void System_Private_CoreLib_System_String_Concat_System_String_System_String_System_String_System_String()
        {
            BenchmarkChecksum += ((String.Concat("hello", "hello", "hello")).Length);
        }

        // System.Private.CoreLib/System.String::Format:System.String(System.String,System.Object[])
        public static void System_Private_CoreLib_System_String_Format_System_String_System_String_System_Object()
        {
            BenchmarkChecksum += ((String.Format("hello", Array.Empty<System.Object>())).Length);
        }

        // System.Private.CoreLib/System.String::StartsWith:System.Boolean(System.String)
        public static void System_Private_CoreLib_System_String_StartsWith_System_Boolean_System_String()
        {
            BenchmarkChecksum += (("hello".StartsWith("hello")) ? 1 : 0);
        }

        // System.Private.CoreLib/System.String::Contains:System.Boolean(System.String)
        public static void System_Private_CoreLib_System_String_Contains_System_Boolean_System_String()
        {
            BenchmarkChecksum += (("hello".Contains("hello")) ? 1 : 0);
        }

        // System.Private.CoreLib/System.String::Replace:System.String(System.String,System.String)
        public static void System_Private_CoreLib_System_String_Replace_System_String_System_String_System_String()
        {
            BenchmarkChecksum += (("hello".Replace("hello", "hello")).Length);
        }

        // System.Private.CoreLib/System.String::Split:System.String[](System.Char)
        public static void System_Private_CoreLib_System_String_Split_System_String_System_Char()
        {
            BenchmarkChecksum += (("hello".Split('A')).Length);
        }

        // System.Private.CoreLib/System.String::ToUpper:System.String()
        public static void System_Private_CoreLib_System_String_ToUpper_System_String()
        {
            BenchmarkChecksum += (("hello".ToUpper()).Length);
        }

        // System.Private.CoreLib/System.String::ToLower:System.String()
        public static void System_Private_CoreLib_System_String_ToLower_System_String()
        {
            BenchmarkChecksum += (("hello".ToLower()).Length);
        }

        // System.Private.CoreLib/System.String::Trim:System.String()
        public static void System_Private_CoreLib_System_String_Trim_System_String()
        {
            BenchmarkChecksum += (("hello".Trim()).Length);
        }

        // System.Private.CoreLib/System.String::Join:System.String(System.String,System.String[])
        public static void System_Private_CoreLib_System_String_Join_System_String_System_String_System_String()
        {
            BenchmarkChecksum += ((String.Join("hello", Array.Empty<System.String>())).Length);
        }

        // System.Private.CoreLib/System.Char::IsDigit:System.Boolean(System.Char)
        public static void System_Private_CoreLib_System_Char_IsDigit_System_Boolean_System_Char()
        {
            BenchmarkChecksum += ((Char.IsDigit('A')) ? 1 : 0);
        }

        // System.Private.CoreLib/System.Char::IsLetter:System.Boolean(System.Char)
        public static void System_Private_CoreLib_System_Char_IsLetter_System_Boolean_System_Char()
        {
            BenchmarkChecksum += ((Char.IsLetter('A')) ? 1 : 0);
        }

        // System.Private.CoreLib/System.Char::IsWhiteSpace:System.Boolean(System.Char)
        public static void System_Private_CoreLib_System_Char_IsWhiteSpace_System_Boolean_System_Char()
        {
            BenchmarkChecksum += ((Char.IsWhiteSpace('A')) ? 1 : 0);
        }

    }
}
