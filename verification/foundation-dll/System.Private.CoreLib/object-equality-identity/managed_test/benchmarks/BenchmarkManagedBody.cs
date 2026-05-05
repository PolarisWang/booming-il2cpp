// Auto-generated benchmark managed bodies
// Family: family/System.Private.CoreLib/object/equality-identity
using System;

namespace Chaos.Benchmark.object_equality_identity
{
    public static class BenchmarkManagedBody
    {
        internal static long BenchmarkChecksum;

        // System.Private.CoreLib/System.Object::Equals:System.Boolean(System.Object)
        public static void System_Private_CoreLib_System_Object_Equals_System_Boolean_System_Object()
        {
            BenchmarkChecksum += ((new object().Equals(42)) ? 1 : 0);
        }

        // System.Private.CoreLib/System.Object::Equals:System.Boolean(System.Object,System.Object)
        public static void System_Private_CoreLib_System_Object_Equals_System_Boolean_System_Object_System_Object()
        {
            BenchmarkChecksum += ((Object.Equals(42, 42)) ? 1 : 0);
        }

        // System.Private.CoreLib/System.Object::ReferenceEquals:System.Boolean(System.Object,System.Object)
        public static void System_Private_CoreLib_System_Object_ReferenceEquals_System_Boolean_System_Object_System_Object()
        {
            BenchmarkChecksum += ((Object.ReferenceEquals(42, 42)) ? 1 : 0);
        }

        // System.Private.CoreLib/System.Object::GetHashCode:System.Int32()
        public static void System_Private_CoreLib_System_Object_GetHashCode_System_Int32()
        {
            BenchmarkChecksum += new object().GetHashCode();
        }

        // System.Private.CoreLib/System.Object::ToString:System.String()
        public static void System_Private_CoreLib_System_Object_ToString_System_String()
        {
            BenchmarkChecksum += ((new object().ToString()).Length);
        }

        // System.Private.CoreLib/System.Object::GetType:System.Type()
        public static void System_Private_CoreLib_System_Object_GetType_System_Type()
        {
            BenchmarkChecksum += ((new object().GetType()).GetHashCode());
        }

        // System.Private.CoreLib/System.Object::MemberwiseClone:System.Object()
        public static void System_Private_CoreLib_System_Object_MemberwiseClone_System_Object()
        {
            BenchmarkChecksum += ((new object().MemberwiseClone()).GetHashCode());
        }

    }
}
