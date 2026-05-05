// Auto-generated benchmark managed bodies
// Family: family/System.Private.CoreLib/reflection/member-basics
using System;

namespace Chaos.Benchmark.reflection_member_basics
{
    public static class BenchmarkManagedBody
    {
        internal static long BenchmarkChecksum;

        // System.Private.CoreLib/System.Reflection.MemberInfo::get_Name:System.String()
        public static void System_Private_CoreLib_System_Reflection_MemberInfo_get_Name_System_String()
        {
            BenchmarkChecksum += ((default(MemberInfo)!.Name).Length);
        }

        // System.Private.CoreLib/System.Reflection.MemberInfo::get_MemberType:System.Reflection.MemberTypes()
        public static void System_Private_CoreLib_System_Reflection_MemberInfo_get_MemberType_System_Reflection_MemberTypes()
        {
            BenchmarkChecksum += (int)(default(MemberInfo)!.MemberType);
        }

        // System.Private.CoreLib/System.Reflection.MemberInfo::get_DeclaringType:System.Type()
        public static void System_Private_CoreLib_System_Reflection_MemberInfo_get_DeclaringType_System_Type()
        {
            BenchmarkChecksum += ((default(MemberInfo)!.DeclaringType).GetHashCode());
        }

        // System.Private.CoreLib/System.Reflection.MethodBase::Invoke:System.Object(System.Object,System.Object[])
        public static void System_Private_CoreLib_System_Reflection_MethodBase_Invoke_System_Object_System_Object_System_Object()
        {
            BenchmarkChecksum += ((default(MethodInfo)!.Invoke(42, Array.Empty<System.Object>())).GetHashCode());
        }

        // System.Private.CoreLib/System.Reflection.MethodInfo::GetParameters:System.Reflection.ParameterInfo[]()
        public static void System_Private_CoreLib_System_Reflection_MethodInfo_GetParameters_System_Reflection_ParameterInfo()
        {
            BenchmarkChecksum += ((default(MethodInfo)!.GetParameters()).Length);
        }

        // System.Private.CoreLib/System.Reflection.MethodInfo::get_ReturnType:System.Type()
        public static void System_Private_CoreLib_System_Reflection_MethodInfo_get_ReturnType_System_Type()
        {
            BenchmarkChecksum += ((default(MethodInfo)!.ReturnType).GetHashCode());
        }

        // System.Private.CoreLib/System.Reflection.ConstructorInfo::Invoke:System.Object(System.Object[])
        public static void System_Private_CoreLib_System_Reflection_ConstructorInfo_Invoke_System_Object_System_Object()
        {
            BenchmarkChecksum += ((default(ConstructorInfo)!.Invoke(Array.Empty<System.Object>())).GetHashCode());
        }

        // System.Private.CoreLib/System.Reflection.FieldInfo::GetValue:System.Object(System.Object)
        public static void System_Private_CoreLib_System_Reflection_FieldInfo_GetValue_System_Object_System_Object()
        {
            BenchmarkChecksum += ((default(FieldInfo)!.GetValue(42)).GetHashCode());
        }

        // System.Private.CoreLib/System.Reflection.FieldInfo::SetValue:System.Void(System.Object,System.Object)
        public static void System_Private_CoreLib_System_Reflection_FieldInfo_SetValue_System_Void_System_Object_System_Object()
        {
            BenchmarkChecksum += (int)(default(FieldInfo)!.SetValue(42, 42));
        }

        // System.Private.CoreLib/System.Reflection.FieldInfo::get_FieldType:System.Type()
        public static void System_Private_CoreLib_System_Reflection_FieldInfo_get_FieldType_System_Type()
        {
            BenchmarkChecksum += ((default(FieldInfo)!.FieldType).GetHashCode());
        }

        // System.Private.CoreLib/System.Reflection.PropertyInfo::GetValue:System.Object(System.Object)
        public static void System_Private_CoreLib_System_Reflection_PropertyInfo_GetValue_System_Object_System_Object()
        {
            BenchmarkChecksum += ((default(PropertyInfo)!.GetValue(42)).GetHashCode());
        }

        // System.Private.CoreLib/System.Reflection.PropertyInfo::GetValue:System.Object(System.Object,System.Object[])
        public static void System_Private_CoreLib_System_Reflection_PropertyInfo_GetValue_System_Object_System_Object_System_Object()
        {
            BenchmarkChecksum += ((default(PropertyInfo)!.GetValue(42, Array.Empty<System.Object>())).GetHashCode());
        }

        // System.Private.CoreLib/System.Reflection.PropertyInfo::SetValue:System.Void(System.Object,System.Object)
        public static void System_Private_CoreLib_System_Reflection_PropertyInfo_SetValue_System_Void_System_Object_System_Object()
        {
            BenchmarkChecksum += (int)(default(PropertyInfo)!.SetValue(42, 42));
        }

        // System.Private.CoreLib/System.Reflection.PropertyInfo::get_PropertyType:System.Type()
        public static void System_Private_CoreLib_System_Reflection_PropertyInfo_get_PropertyType_System_Type()
        {
            BenchmarkChecksum += ((default(PropertyInfo)!.PropertyType).GetHashCode());
        }

    }
}
