// Auto-generated benchmark managed bodies
// Family: family/System.Private.CoreLib/attributes/custom-metadata
using System;

namespace Chaos.Benchmark.attributes_custom_metadata
{
    public static class BenchmarkManagedBody
    {
        internal static long BenchmarkChecksum;

        // System.Private.CoreLib/System.Attribute::GetCustomAttribute:System.Attribute(System.Reflection.MemberInfo,System.Type)
        public static void System_Private_CoreLib_System_Attribute_GetCustomAttribute_System_Attribute_System_Reflection_MemberInfo_System_Type()
        {
            BenchmarkChecksum += ((typeof(byte).Assembly.GetCustomAttribute(typeof(AssemblyDescriptionAttribute))).GetHashCode());
        }

        // System.Private.CoreLib/System.Attribute::GetCustomAttribute:System.Attribute(System.Reflection.Assembly,System.Type)
        public static void System_Private_CoreLib_System_Attribute_GetCustomAttribute_System_Attribute_System_Reflection_Assembly_System_Type()
        {
            BenchmarkChecksum += ((typeof(byte).Assembly.GetCustomAttribute(typeof(AssemblyDescriptionAttribute))).GetHashCode());
        }

        // System.Private.CoreLib/System.Attribute::GetCustomAttributes:System.Attribute[](System.Reflection.MemberInfo,System.Type)
        public static void System_Private_CoreLib_System_Attribute_GetCustomAttributes_System_Attribute_System_Reflection_MemberInfo_System_Type()
        {
            BenchmarkChecksum += ((Attribute.GetCustomAttributes(null!, typeof(byte))).Length);
        }

        // System.Private.CoreLib/System.Attribute::GetCustomAttributes:System.Attribute[](System.Reflection.Assembly,System.Type)
        public static void System_Private_CoreLib_System_Attribute_GetCustomAttributes_System_Attribute_System_Reflection_Assembly_System_Type()
        {
            BenchmarkChecksum += ((Attribute.GetCustomAttributes(null!, typeof(byte))).Length);
        }

        // System.Private.CoreLib/System.Attribute::GetCustomAttributes:System.Attribute[](System.Reflection.MemberInfo)
        public static void System_Private_CoreLib_System_Attribute_GetCustomAttributes_System_Attribute_System_Reflection_MemberInfo()
        {
            BenchmarkChecksum += ((Attribute.GetCustomAttributes(null!)).Length);
        }

        // System.Private.CoreLib/System.Attribute::GetCustomAttributes:System.Attribute[](System.Reflection.Assembly)
        public static void System_Private_CoreLib_System_Attribute_GetCustomAttributes_System_Attribute_System_Reflection_Assembly()
        {
            BenchmarkChecksum += ((Attribute.GetCustomAttributes(null!)).Length);
        }

        // System.Private.CoreLib/System.Attribute::IsDefined:System.Boolean(System.Reflection.MemberInfo,System.Type)
        public static void System_Private_CoreLib_System_Attribute_IsDefined_System_Boolean_System_Reflection_MemberInfo_System_Type()
        {
            BenchmarkChecksum += ((typeof(byte).Assembly.IsDefined(typeof(AssemblyDescriptionAttribute))) ? 1 : 0);
        }

        // System.Private.CoreLib/System.Attribute::IsDefined:System.Boolean(System.Reflection.Assembly,System.Type)
        public static void System_Private_CoreLib_System_Attribute_IsDefined_System_Boolean_System_Reflection_Assembly_System_Type()
        {
            BenchmarkChecksum += ((typeof(byte).Assembly.IsDefined(typeof(AssemblyDescriptionAttribute))) ? 1 : 0);
        }

        // System.Private.CoreLib/System.Attribute::get_TypeId:System.Object()
        public static void System_Private_CoreLib_System_Attribute_get_TypeId_System_Object()
        {
            BenchmarkChecksum += ((default(Attribute)!.TypeId).GetHashCode());
        }

    }
}
