// Auto-generated benchmark managed bodies
// Family: family/System.Private.CoreLib/delegate/core-invocation
using System;

namespace Chaos.Benchmark.delegate_core_invocation
{
    public static class BenchmarkManagedBody
    {
        internal static long BenchmarkChecksum;

        // System.Private.CoreLib/System.Delegate::Combine:System.Delegate(System.Delegate,System.Delegate)
        public static void System_Private_CoreLib_System_Delegate_Combine_System_Delegate_System_Delegate_System_Delegate()
        {
            BenchmarkChecksum += ((Delegate.Combine(null!, null!)).GetHashCode());
        }

        // System.Private.CoreLib/System.Delegate::Remove:System.Delegate(System.Delegate,System.Delegate)
        public static void System_Private_CoreLib_System_Delegate_Remove_System_Delegate_System_Delegate_System_Delegate()
        {
            BenchmarkChecksum += ((Delegate.Remove(null!, null!)).GetHashCode());
        }

        // System.Private.CoreLib/System.Delegate::DynamicInvoke:System.Object(System.Object[])
        public static void System_Private_CoreLib_System_Delegate_DynamicInvoke_System_Object_System_Object()
        {
            BenchmarkChecksum += ((default(Delegate)!.DynamicInvoke(Array.Empty<System.Object>())).GetHashCode());
        }

        // System.Private.CoreLib/System.Delegate::get_Method:System.Reflection.MethodInfo()
        public static void System_Private_CoreLib_System_Delegate_get_Method_System_Reflection_MethodInfo()
        {
            BenchmarkChecksum += ((default(Delegate)!.Method).GetHashCode());
        }

        // System.Private.CoreLib/System.Delegate::get_Target:System.Object()
        public static void System_Private_CoreLib_System_Delegate_get_Target_System_Object()
        {
            BenchmarkChecksum += ((default(Delegate)!.Target).GetHashCode());
        }

        // System.Private.CoreLib/System.Delegate::CreateDelegate:System.Delegate(System.Type,System.Object,System.String)
        public static void System_Private_CoreLib_System_Delegate_CreateDelegate_System_Delegate_System_Type_System_Object_System_String()
        {
            BenchmarkChecksum += ((Delegate.CreateDelegate(typeof(byte), 42, "hello")).GetHashCode());
        }

        // System.Private.CoreLib/System.Delegate::CreateDelegate:System.Delegate(System.Type,System.Reflection.MethodInfo)
        public static void System_Private_CoreLib_System_Delegate_CreateDelegate_System_Delegate_System_Type_System_Reflection_MethodInfo()
        {
            BenchmarkChecksum += ((Delegate.CreateDelegate(typeof(byte), null!)).GetHashCode());
        }

        // System.Private.CoreLib/System.MulticastDelegate::GetInvocationList:System.Delegate[]()
        public static void System_Private_CoreLib_System_MulticastDelegate_GetInvocationList_System_Delegate()
        {
            BenchmarkChecksum += ((default(MulticastDelegate)!.GetInvocationList()).Length);
        }

        // System.Private.CoreLib/System.Delegate::op_Equality:System.Boolean(System.Delegate,System.Delegate)
        public static void System_Private_CoreLib_System_Delegate_op_Equality_System_Boolean_System_Delegate_System_Delegate()
        {
            BenchmarkChecksum += ((default(Delegate)!.op_Equality(null!, null!)) ? 1 : 0);
        }

        // System.Private.CoreLib/System.Delegate::op_Inequality:System.Boolean(System.Delegate,System.Delegate)
        public static void System_Private_CoreLib_System_Delegate_op_Inequality_System_Boolean_System_Delegate_System_Delegate()
        {
            BenchmarkChecksum += ((default(Delegate)!.op_Inequality(null!, null!)) ? 1 : 0);
        }

    }
}
