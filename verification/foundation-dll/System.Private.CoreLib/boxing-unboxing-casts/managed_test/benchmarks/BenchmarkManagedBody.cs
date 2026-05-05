// Auto-generated benchmark managed bodies
// Family: family/System.Private.CoreLib/boxing/unboxing-casts
using System;

namespace Chaos.Benchmark.boxing_unboxing_casts
{
    public static class BenchmarkManagedBody
    {
        internal static long BenchmarkChecksum;

        // System.Private.CoreLib/System.Convert::ChangeType:System.Object(System.Object,System.Type)
        public static void System_Private_CoreLib_System_Convert_ChangeType_System_Object_System_Object_System_Type()
        {
            BenchmarkChecksum += ((Convert.ChangeType(42, typeof(byte))).GetHashCode());
        }

        // System.Private.CoreLib/System.Convert::ChangeType:System.Object(System.Object,System.Type,System.IFormatProvider)
        public static void System_Private_CoreLib_System_Convert_ChangeType_System_Object_System_Object_System_Type_System_IFormatProvider()
        {
            BenchmarkChecksum += ((Convert.ChangeType(42, typeof(byte), null!)).GetHashCode());
        }

        // System.Private.CoreLib/System.Type::IsAssignableFrom:System.Boolean(System.Type)
        public static void System_Private_CoreLib_System_Type_IsAssignableFrom_System_Boolean_System_Type()
        {
            BenchmarkChecksum += ((typeof(byte).IsAssignableFrom(typeof(byte))) ? 1 : 0);
        }

        // System.Private.CoreLib/System.Type::IsInstanceOfType:System.Boolean(System.Object)
        public static void System_Private_CoreLib_System_Type_IsInstanceOfType_System_Boolean_System_Object()
        {
            BenchmarkChecksum += ((typeof(byte).IsInstanceOfType(42)) ? 1 : 0);
        }

        // System.Private.CoreLib/System.Type::IsSubclassOf:System.Boolean(System.Type)
        public static void System_Private_CoreLib_System_Type_IsSubclassOf_System_Boolean_System_Type()
        {
            BenchmarkChecksum += ((typeof(byte).IsSubclassOf(typeof(byte))) ? 1 : 0);
        }

        // System.Private.CoreLib/System.Type::IsAssignableTo:System.Boolean(System.Type)
        public static void System_Private_CoreLib_System_Type_IsAssignableTo_System_Boolean_System_Type()
        {
            BenchmarkChecksum += ((typeof(byte).IsAssignableTo(typeof(byte))) ? 1 : 0);
        }

        // System.Private.CoreLib/System.Object::MemberwiseClone:System.Object()
        public static void System_Private_CoreLib_System_Object_MemberwiseClone_System_Object()
        {
            BenchmarkChecksum += ((new object().MemberwiseClone()).GetHashCode());
        }

        // System.Private.CoreLib/System.Runtime.CompilerServices.RuntimeHelpers::GetObjectValue:System.Object(System.Object)
        public static void System_Private_CoreLib_System_Runtime_CompilerServices_RuntimeHelpers_GetObjectValue_System_Object_System_Object()
        {
            BenchmarkChecksum += ((RuntimeHelpers.GetObjectValue(42)).GetHashCode());
        }

        // System.Private.CoreLib/System.Array::CreateInstance:System.Array(System.Type,System.Int32)
        public static void System_Private_CoreLib_System_Array_CreateInstance_System_Array_System_Type_System_Int32()
        {
            BenchmarkChecksum += ((Array.CreateInstance(typeof(byte), 42)).GetHashCode());
        }

        // System.Private.CoreLib/System.Array::CreateInstance:System.Array(System.Type,System.Int32,System.Int32)
        public static void System_Private_CoreLib_System_Array_CreateInstance_System_Array_System_Type_System_Int32_System_Int32()
        {
            BenchmarkChecksum += ((Array.CreateInstance(typeof(byte), 42, 42)).GetHashCode());
        }

        // System.Private.CoreLib/System.ValueType::Equals:System.Boolean(System.Object)
        public static void System_Private_CoreLib_System_ValueType_Equals_System_Boolean_System_Object()
        {
            BenchmarkChecksum += ((((ValueType)42).Equals(42)) ? 1 : 0);
        }

    }
}
