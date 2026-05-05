// Auto-generated benchmark managed bodies
// Family: family/System.Private.CoreLib/runtime/compiler-services
using System;

namespace Chaos.Benchmark.runtime_compiler_services
{
    public static class BenchmarkManagedBody
    {
        internal static long BenchmarkChecksum;

        // System.Private.CoreLib/System.Runtime.CompilerServices.RuntimeHelpers::GetHashCode:System.Int32(System.Object)
        public static void System_Private_CoreLib_System_Runtime_CompilerServices_RuntimeHelpers_GetHashCode_System_Int32_System_Object()
        {
            BenchmarkChecksum += RuntimeHelpers.GetHashCode(42);
        }

        // System.Private.CoreLib/System.Runtime.CompilerServices.RuntimeHelpers::Equals:System.Boolean(System.Object,System.Object)
        public static void System_Private_CoreLib_System_Runtime_CompilerServices_RuntimeHelpers_Equals_System_Boolean_System_Object_System_Object()
        {
            BenchmarkChecksum += ((RuntimeHelpers.Equals(42, 42)) ? 1 : 0);
        }

        // System.Private.CoreLib/System.Runtime.CompilerServices.RuntimeHelpers::GetObjectValue:System.Object(System.Object)
        public static void System_Private_CoreLib_System_Runtime_CompilerServices_RuntimeHelpers_GetObjectValue_System_Object_System_Object()
        {
            BenchmarkChecksum += ((RuntimeHelpers.GetObjectValue(42)).GetHashCode());
        }

        // System.Private.CoreLib/System.Runtime.CompilerServices.RuntimeHelpers::RunClassConstructor:System.Void(System.RuntimeTypeHandle)
        public static void System_Private_CoreLib_System_Runtime_CompilerServices_RuntimeHelpers_RunClassConstructor_System_Void_System_RuntimeTypeHandle()
        {
            BenchmarkChecksum += (int)(RuntimeHelpers.RunClassConstructor(default(System.RuntimeTypeHandle)));
        }

        // System.Private.CoreLib/System.Runtime.CompilerServices.RuntimeHelpers::InitializeArray:System.Void(System.Array,System.RuntimeFieldHandle)
        public static void System_Private_CoreLib_System_Runtime_CompilerServices_RuntimeHelpers_InitializeArray_System_Void_System_Array_System_RuntimeFieldHandle()
        {
            BenchmarkChecksum += (int)(RuntimeHelpers.InitializeArray(Array.Empty<byte>(), default(System.RuntimeFieldHandle)));
        }

        // System.Private.CoreLib/System.Runtime.CompilerServices.RuntimeHelpers::GetSubArray:T[](T[],System.Range)
        public static void System_Private_CoreLib_System_Runtime_CompilerServices_RuntimeHelpers_GetSubArray_T_T_System_Range()
        {
            BenchmarkChecksum += ((RuntimeHelpers.GetSubArray(Array.Empty<T>(), null!)).Length);
        }

        // System.Private.CoreLib/System.Runtime.CompilerServices.RuntimeHelpers::BoxEnum:System.Object(T)
        public static void System_Private_CoreLib_System_Runtime_CompilerServices_RuntimeHelpers_BoxEnum_System_Object_T()
        {
            BenchmarkChecksum += ((RuntimeHelpers.BoxEnum(null!)).GetHashCode());
        }

        // System.Private.CoreLib/System.Runtime.CompilerServices.FormattableStringFactory::Create:System.Runtime.CompilerServices.FormattableString(System.String,System.Object[])
        public static void System_Private_CoreLib_System_Runtime_CompilerServices_FormattableStringFactory_Create_System_Runtime_CompilerServices_FormattableString_System_String_System_Object()
        {
            BenchmarkChecksum += ((FormattableStringFactory.Create("hello", Array.Empty<System.Object>())).GetHashCode());
        }

        // System.Private.CoreLib/System.Runtime.CompilerServices.RuntimeWrappedException::.ctor:System.Void(System.Object)
        public static void System_Private_CoreLib_System_Runtime_CompilerServices_RuntimeWrappedException_ctor_System_Void_System_Object()
        {
            BenchmarkChecksum += (int)(new RuntimeWrappedException(42));
        }

        // System.Private.CoreLib/System.Runtime.CompilerServices.RuntimeWrappedException::get_WrappedException:System.Object()
        public static void System_Private_CoreLib_System_Runtime_CompilerServices_RuntimeWrappedException_get_WrappedException_System_Object()
        {
            BenchmarkChecksum += ((new RuntimeWrappedException(42).WrappedException).GetHashCode());
        }

    }
}
