// Auto-generated benchmark managed bodies
// Family: family/System.Private.CoreLib/type/runtime-handles
using System;

namespace Chaos.Benchmark.type_runtime_handles
{
    public static class BenchmarkManagedBody
    {
        internal static long BenchmarkChecksum;

        // System.Private.CoreLib/System.Type::GetType:System.Type(System.String)
        public static void System_Private_CoreLib_System_Type_GetType_System_Type_System_String()
        {
            BenchmarkChecksum += ((Type.GetType("hello")).GetHashCode());
        }

        // System.Private.CoreLib/System.Type::GetType:System.Type(System.String,System.Boolean)
        public static void System_Private_CoreLib_System_Type_GetType_System_Type_System_String_System_Boolean()
        {
            BenchmarkChecksum += ((Type.GetType("hello", true)).GetHashCode());
        }

        // System.Private.CoreLib/System.Type::GetType:System.Type(System.String,System.Boolean,System.Boolean)
        public static void System_Private_CoreLib_System_Type_GetType_System_Type_System_String_System_Boolean_System_Boolean()
        {
            BenchmarkChecksum += ((Type.GetType("hello", true, true)).GetHashCode());
        }

        // System.Private.CoreLib/System.Type::GetTypeFromHandle:System.Type(System.RuntimeTypeHandle)
        public static void System_Private_CoreLib_System_Type_GetTypeFromHandle_System_Type_System_RuntimeTypeHandle()
        {
            BenchmarkChecksum += ((Type.GetTypeFromHandle(default(System.RuntimeTypeHandle))).GetHashCode());
        }

        // System.Private.CoreLib/System.Type::get_IsValueType:System.Boolean()
        public static void System_Private_CoreLib_System_Type_get_IsValueType_System_Boolean()
        {
            BenchmarkChecksum += ((typeof(byte).IsValueType) ? 1 : 0);
        }

        // System.Private.CoreLib/System.Type::get_IsEnum:System.Boolean()
        public static void System_Private_CoreLib_System_Type_get_IsEnum_System_Boolean()
        {
            BenchmarkChecksum += ((typeof(byte).IsEnum) ? 1 : 0);
        }

        // System.Private.CoreLib/System.Type::get_IsArray:System.Boolean()
        public static void System_Private_CoreLib_System_Type_get_IsArray_System_Boolean()
        {
            BenchmarkChecksum += ((typeof(byte).IsArray) ? 1 : 0);
        }

        // System.Private.CoreLib/System.Type::get_TypeHandle:System.RuntimeTypeHandle()
        public static void System_Private_CoreLib_System_Type_get_TypeHandle_System_RuntimeTypeHandle()
        {
            BenchmarkChecksum += ((typeof(byte).TypeHandle).GetHashCode());
        }

        // System.Private.CoreLib/System.Type::get_FullName:System.String()
        public static void System_Private_CoreLib_System_Type_get_FullName_System_String()
        {
            BenchmarkChecksum += ((typeof(byte).FullName).Length);
        }

        // System.Private.CoreLib/System.Type::get_Name:System.String()
        public static void System_Private_CoreLib_System_Type_get_Name_System_String()
        {
            BenchmarkChecksum += ((typeof(byte).Name).Length);
        }

        // System.Private.CoreLib/System.Type::get_BaseType:System.Type()
        public static void System_Private_CoreLib_System_Type_get_BaseType_System_Type()
        {
            BenchmarkChecksum += ((typeof(byte).BaseType).GetHashCode());
        }

        // System.Private.CoreLib/System.Type::GetMethods:System.Reflection.MethodInfo[]()
        public static void System_Private_CoreLib_System_Type_GetMethods_System_Reflection_MethodInfo()
        {
            BenchmarkChecksum += ((typeof(byte).GetMethods()).Length);
        }

        // System.Private.CoreLib/System.Type::GetFields:System.Reflection.FieldInfo[]()
        public static void System_Private_CoreLib_System_Type_GetFields_System_Reflection_FieldInfo()
        {
            BenchmarkChecksum += ((typeof(byte).GetFields()).Length);
        }

    }
}
