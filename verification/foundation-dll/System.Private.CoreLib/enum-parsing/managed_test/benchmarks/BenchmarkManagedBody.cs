// Auto-generated benchmark managed bodies
// Family: family/System.Private.CoreLib/enum/parsing
using System;

namespace Chaos.Benchmark.enum_parsing
{
    public static class BenchmarkManagedBody
    {
        internal static long BenchmarkChecksum;

        // System.Private.CoreLib/System.Enum::Format:System.String(System.Type,System.Object,System.String)
        public static void System_Private_CoreLib_System_Enum_Format_System_String_System_Type_System_Object_System_String()
        {
            BenchmarkChecksum += ((Enum.Format(typeof(byte), 42, "hello")).Length);
        }

        // System.Private.CoreLib/System.Enum::GetName:System.String(System.RuntimeType,System.UInt64)
        public static void System_Private_CoreLib_System_Enum_GetName_System_String_System_RuntimeType_System_UInt64()
        {
            BenchmarkChecksum += ((Enum.GetName(null!, 42uL)).Length);
        }

        // System.Private.CoreLib/System.Enum::GetName:System.String(System.Type,System.Object)
        public static void System_Private_CoreLib_System_Enum_GetName_System_String_System_Type_System_Object()
        {
            BenchmarkChecksum += ((Enum.GetName(typeof(byte), 42)).Length);
        }

        // System.Private.CoreLib/System.Enum::GetNames:System.String[](System.Type)
        public static void System_Private_CoreLib_System_Enum_GetNames_System_String_System_Type()
        {
            BenchmarkChecksum += ((Enum.GetNames(typeof(byte))).Length);
        }

        // System.Private.CoreLib/System.Enum::GetValues:System.Array(System.Type)
        public static void System_Private_CoreLib_System_Enum_GetValues_System_Array_System_Type()
        {
            BenchmarkChecksum += ((Enum.GetValues(typeof(byte))).GetHashCode());
        }

        // System.Private.CoreLib/System.Enum::IsDefined:System.Boolean(System.Type,System.Object)
        public static void System_Private_CoreLib_System_Enum_IsDefined_System_Boolean_System_Type_System_Object()
        {
            BenchmarkChecksum += ((Enum.IsDefined(typeof(byte), 42)) ? 1 : 0);
        }

        // System.Private.CoreLib/System.Enum::Parse:System.Object(System.Type,System.String)
        public static void System_Private_CoreLib_System_Enum_Parse_System_Object_System_Type_System_String()
        {
            BenchmarkChecksum += ((Enum.Parse(typeof(byte), "hello")).GetHashCode());
        }

        // System.Private.CoreLib/System.Enum::Parse:System.Object(System.Type,System.String,System.Boolean)
        public static void System_Private_CoreLib_System_Enum_Parse_System_Object_System_Type_System_String_System_Boolean()
        {
            BenchmarkChecksum += ((Enum.Parse(typeof(byte), "hello", true)).GetHashCode());
        }

        // System.Private.CoreLib/System.Enum::ToString:System.String()
        public static void System_Private_CoreLib_System_Enum_ToString_System_String()
        {
            BenchmarkChecksum += ((DayOfWeek.Monday.ToString()).Length);
        }

        // System.Private.CoreLib/System.Enum::ToString:System.String(System.String)
        public static void System_Private_CoreLib_System_Enum_ToString_System_String_System_String()
        {
            BenchmarkChecksum += ((DayOfWeek.Monday.ToString("hello")).Length);
        }

        // System.Private.CoreLib/System.Enum::TryParse:System.Boolean(System.Type,System.String,System.Boolean,System.Object&)
        public static void System_Private_CoreLib_System_Enum_TryParse_System_Boolean_System_Type_System_String_System_Boolean_System_Object_Ref()
        {
            BenchmarkChecksum += ((Enum.TryParse(typeof(byte), "hello", true, 42)) ? 1 : 0);
        }

        // System.Private.CoreLib/System.Enum::TryParse:System.Boolean(System.Type,System.String,System.Object&)
        public static void System_Private_CoreLib_System_Enum_TryParse_System_Boolean_System_Type_System_String_System_Object_Ref()
        {
            BenchmarkChecksum += ((Enum.TryParse(typeof(byte), "hello", 42)) ? 1 : 0);
        }

    }
}
