// Auto-generated benchmark managed bodies
// Family: family/System.Private.CoreLib/generic/nullable-value
using System;

namespace Chaos.Benchmark.generic_nullable_value
{
    public static class BenchmarkManagedBody
    {
        internal static long BenchmarkChecksum;

        // System.Private.CoreLib/System.Nullable`1::get_HasValue:System.Boolean()
        public static void System_Private_CoreLib_System_Nullable_1_get_HasValue_System_Boolean()
        {
            BenchmarkChecksum += ((default(Nullable<int>).HasValue) ? 1 : 0);
        }

        // System.Private.CoreLib/System.Nullable`1::get_Value:T()
        public static void System_Private_CoreLib_System_Nullable_1_get_Value_T()
        {
            BenchmarkChecksum += (int)(default(Nullable<int>).Value);
        }

        // System.Private.CoreLib/System.Nullable`1::GetValueOrDefault:T()
        public static void System_Private_CoreLib_System_Nullable_1_GetValueOrDefault_T()
        {
            BenchmarkChecksum += (int)(default(Nullable<int>).GetValueOrDefault());
        }

        // System.Private.CoreLib/System.Nullable`1::GetValueOrDefault:T(T)
        public static void System_Private_CoreLib_System_Nullable_1_GetValueOrDefault_T_T()
        {
            BenchmarkChecksum += (int)(default(Nullable<int>).GetValueOrDefault(42));
        }

        // System.Private.CoreLib/System.Nullable`1::Equals:System.Boolean(System.Object)
        public static void System_Private_CoreLib_System_Nullable_1_Equals_System_Boolean_System_Object()
        {
            BenchmarkChecksum += ((default(Nullable<int>).Equals(42)) ? 1 : 0);
        }

        // System.Private.CoreLib/System.Nullable`1::GetHashCode:System.Int32()
        public static void System_Private_CoreLib_System_Nullable_1_GetHashCode_System_Int32()
        {
            BenchmarkChecksum += default(Nullable<int>).GetHashCode();
        }

        // System.Private.CoreLib/System.Nullable`1::ToString:System.String()
        public static void System_Private_CoreLib_System_Nullable_1_ToString_System_String()
        {
            BenchmarkChecksum += ((default(Nullable<int>).ToString()).Length);
        }

        // System.Private.CoreLib/System.ValueType::Equals:System.Boolean(System.Object)
        public static void System_Private_CoreLib_System_ValueType_Equals_System_Boolean_System_Object()
        {
            BenchmarkChecksum += ((((ValueType)42).Equals(42)) ? 1 : 0);
        }

        // System.Private.CoreLib/System.ValueType::GetHashCode:System.Int32()
        public static void System_Private_CoreLib_System_ValueType_GetHashCode_System_Int32()
        {
            BenchmarkChecksum += ((ValueType)42).GetHashCode();
        }

        // System.Private.CoreLib/System.ValueType::ToString:System.String()
        public static void System_Private_CoreLib_System_ValueType_ToString_System_String()
        {
            BenchmarkChecksum += ((((ValueType)42).ToString()).Length);
        }

    }
}
