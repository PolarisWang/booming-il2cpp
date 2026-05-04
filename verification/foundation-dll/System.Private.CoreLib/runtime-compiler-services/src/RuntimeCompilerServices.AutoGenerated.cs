using System;
using System.IO;
using System.Threading;
using System.Threading.Tasks;
using System.Globalization;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;
using System.Collections.Generic;
using System.Reflection;
using Chaos.TestFramework;

// Auto-generated skeleton for Runtime Compiler Services (family/System.Private.CoreLib/runtime/compiler-services).
// Contains test, benchmark, and hotupdate methods merged into one partial class.
// No xunit dependency — [Fact] attributes belong in the test exe project.
public partial class RuntimeCompilerServices
{
    // RuntimeHelpers.GetHashCode
    [MethodSubjectId("System.Private.CoreLib/System.Runtime.CompilerServices.RuntimeHelpers::GetHashCode:System.Int32(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_RuntimeCompilerServices)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Private_CoreLib_System_Runtime_CompilerServices_RuntimeHelpers_GetHashCode_System_Int32_System_Object()
    {
    var result = RuntimeHelpers.GetHashCode(42);
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.Runtime.CompilerServices.RuntimeHelpers::GetHashCode:System.Int32(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_RuntimeCompilerServices)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Runtime_CompilerServices_RuntimeHelpers_GetHashCode_System_Int32_System_Object() { _ = RuntimeHelpers.GetHashCode(42); }

    [HotUpdateSubjectId("System.Private.CoreLib/System.Runtime.CompilerServices.RuntimeHelpers::GetHashCode:System.Int32(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_RuntimeCompilerServices)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Runtime_CompilerServices_RuntimeHelpers_GetHashCode_System_Int32_System_Object() { _ = RuntimeHelpers.GetHashCode(42); }

    // RuntimeHelpers.Equals
    [MethodSubjectId("System.Private.CoreLib/System.Runtime.CompilerServices.RuntimeHelpers::Equals:System.Boolean(System.Object,System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_RuntimeCompilerServices)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Private_CoreLib_System_Runtime_CompilerServices_RuntimeHelpers_Equals_System_Boolean_System_Object_System_Object()
    {
    var result = RuntimeHelpers.Equals(42, 42);
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.Runtime.CompilerServices.RuntimeHelpers::Equals:System.Boolean(System.Object,System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_RuntimeCompilerServices)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Runtime_CompilerServices_RuntimeHelpers_Equals_System_Boolean_System_Object_System_Object() { _ = RuntimeHelpers.Equals(42, 42); }

    [HotUpdateSubjectId("System.Private.CoreLib/System.Runtime.CompilerServices.RuntimeHelpers::Equals:System.Boolean(System.Object,System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_RuntimeCompilerServices)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Runtime_CompilerServices_RuntimeHelpers_Equals_System_Boolean_System_Object_System_Object() { _ = RuntimeHelpers.Equals(42, 42); }

    // RuntimeHelpers.GetObjectValue
    [MethodSubjectId("System.Private.CoreLib/System.Runtime.CompilerServices.RuntimeHelpers::GetObjectValue:System.Object(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_RuntimeCompilerServices)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Private_CoreLib_System_Runtime_CompilerServices_RuntimeHelpers_GetObjectValue_System_Object_System_Object()
    {
    var result = RuntimeHelpers.GetObjectValue(42);
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.Runtime.CompilerServices.RuntimeHelpers::GetObjectValue:System.Object(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_RuntimeCompilerServices)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Runtime_CompilerServices_RuntimeHelpers_GetObjectValue_System_Object_System_Object() { _ = RuntimeHelpers.GetObjectValue(42); }

    [HotUpdateSubjectId("System.Private.CoreLib/System.Runtime.CompilerServices.RuntimeHelpers::GetObjectValue:System.Object(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_RuntimeCompilerServices)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Runtime_CompilerServices_RuntimeHelpers_GetObjectValue_System_Object_System_Object() { _ = RuntimeHelpers.GetObjectValue(42); }

    // RuntimeHelpers.RunClassConstructor
    [MethodSubjectId("System.Private.CoreLib/System.Runtime.CompilerServices.RuntimeHelpers::RunClassConstructor:System.Void(System.RuntimeTypeHandle)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_RuntimeCompilerServices)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Private_CoreLib_System_Runtime_CompilerServices_RuntimeHelpers_RunClassConstructor_System_Void_System_RuntimeType()
    {
    // TODO: needs-manual — RunClassConstructor with 1 params requires manual implementation
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.Runtime.CompilerServices.RuntimeHelpers::RunClassConstructor:System.Void(System.RuntimeTypeHandle)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_RuntimeCompilerServices)]
    public static void Benchmark_System_Private_CoreLib_System_Runtime_CompilerServices_RuntimeHelpers_RunClassConstructor_System_Void_System_RuntimeType() { }

    [HotUpdateSubjectId("System.Private.CoreLib/System.Runtime.CompilerServices.RuntimeHelpers::RunClassConstructor:System.Void(System.RuntimeTypeHandle)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_RuntimeCompilerServices)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Runtime_CompilerServices_RuntimeHelpers_RunClassConstructor_System_Void_System_RuntimeType() { }

    // RuntimeHelpers.InitializeArray
    [MethodSubjectId("System.Private.CoreLib/System.Runtime.CompilerServices.RuntimeHelpers::InitializeArray:System.Void(System.Array,System.RuntimeFieldHandle)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_RuntimeCompilerServices)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Private_CoreLib_System_Runtime_CompilerServices_RuntimeHelpers_InitializeArray_System_Void_System_Array_System_Ru()
    {
    // TODO: needs-manual — InitializeArray with 2 params requires manual implementation
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.Runtime.CompilerServices.RuntimeHelpers::InitializeArray:System.Void(System.Array,System.RuntimeFieldHandle)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_RuntimeCompilerServices)]
    public static void Benchmark_System_Private_CoreLib_System_Runtime_CompilerServices_RuntimeHelpers_InitializeArray_System_Void_System_Array_System_Ru() { }

    [HotUpdateSubjectId("System.Private.CoreLib/System.Runtime.CompilerServices.RuntimeHelpers::InitializeArray:System.Void(System.Array,System.RuntimeFieldHandle)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_RuntimeCompilerServices)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Runtime_CompilerServices_RuntimeHelpers_InitializeArray_System_Void_System_Array_System_Ru() { }

    // RuntimeHelpers.GetSubArray
    [MethodSubjectId("System.Private.CoreLib/System.Runtime.CompilerServices.RuntimeHelpers::GetSubArray:T[](T[],System.Range)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_RuntimeCompilerServices)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Private_CoreLib_System_Runtime_CompilerServices_RuntimeHelpers_GetSubArray_T_T_System_Range()
    {
    // TODO: needs-manual — GetSubArray with 2 params requires manual implementation
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.Runtime.CompilerServices.RuntimeHelpers::GetSubArray:T[](T[],System.Range)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_RuntimeCompilerServices)]
    public static void Benchmark_System_Private_CoreLib_System_Runtime_CompilerServices_RuntimeHelpers_GetSubArray_T_T_System_Range() { }

    [HotUpdateSubjectId("System.Private.CoreLib/System.Runtime.CompilerServices.RuntimeHelpers::GetSubArray:T[](T[],System.Range)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_RuntimeCompilerServices)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Runtime_CompilerServices_RuntimeHelpers_GetSubArray_T_T_System_Range() { }

    // RuntimeHelpers.BoxEnum
    [MethodSubjectId("System.Private.CoreLib/System.Runtime.CompilerServices.RuntimeHelpers::BoxEnum:System.Object(T)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_RuntimeCompilerServices)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Private_CoreLib_System_Runtime_CompilerServices_RuntimeHelpers_BoxEnum_System_Object_T()
    {
    // TODO: needs-manual — ref/pointer/unsafe parameter requires unsafe context
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.Runtime.CompilerServices.RuntimeHelpers::BoxEnum:System.Object(T)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_RuntimeCompilerServices)]
    public static void Benchmark_System_Private_CoreLib_System_Runtime_CompilerServices_RuntimeHelpers_BoxEnum_System_Object_T() { }

    [HotUpdateSubjectId("System.Private.CoreLib/System.Runtime.CompilerServices.RuntimeHelpers::BoxEnum:System.Object(T)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_RuntimeCompilerServices)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Runtime_CompilerServices_RuntimeHelpers_BoxEnum_System_Object_T() { }

    // FormattableStringFactory.Create
    [MethodSubjectId("System.Private.CoreLib/System.Runtime.CompilerServices.FormattableStringFactory::Create:System.Runtime.CompilerServices.FormattableString(System.String,System.Object[])")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_RuntimeCompilerServices)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Private_CoreLib_System_Runtime_CompilerServices_FormattableStringFactory_Create_System_Runtime_CompilerServices_F()
    {
    _ = FormattableStringFactory.Create("hello", Array.Empty<System.Object>());
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.Runtime.CompilerServices.FormattableStringFactory::Create:System.Runtime.CompilerServices.FormattableString(System.String,System.Object[])")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_RuntimeCompilerServices)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Runtime_CompilerServices_FormattableStringFactory_Create_System_Runtime_CompilerServices_F() { _ = FormattableStringFactory.Create("hello", Array.Empty<System.Object>()); }

    [HotUpdateSubjectId("System.Private.CoreLib/System.Runtime.CompilerServices.FormattableStringFactory::Create:System.Runtime.CompilerServices.FormattableString(System.String,System.Object[])")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_RuntimeCompilerServices)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Runtime_CompilerServices_FormattableStringFactory_Create_System_Runtime_CompilerServices_F() { _ = FormattableStringFactory.Create("hello", Array.Empty<System.Object>()); }

    // RuntimeWrappedException..ctor
    [MethodSubjectId("System.Private.CoreLib/System.Runtime.CompilerServices.RuntimeWrappedException::.ctor:System.Void(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_RuntimeCompilerServices)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Private_CoreLib_System_Runtime_CompilerServices_RuntimeWrappedException_ctor_System_Void_System_Object()
    {
    new RuntimeWrappedException(42);
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.Runtime.CompilerServices.RuntimeWrappedException::.ctor:System.Void(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_RuntimeCompilerServices)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Runtime_CompilerServices_RuntimeWrappedException_ctor_System_Void_System_Object() { new RuntimeWrappedException(42); }

    [HotUpdateSubjectId("System.Private.CoreLib/System.Runtime.CompilerServices.RuntimeWrappedException::.ctor:System.Void(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_RuntimeCompilerServices)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Runtime_CompilerServices_RuntimeWrappedException_ctor_System_Void_System_Object() { new RuntimeWrappedException(42); }

    // RuntimeWrappedException.get_WrappedException
    [MethodSubjectId("System.Private.CoreLib/System.Runtime.CompilerServices.RuntimeWrappedException::get_WrappedException:System.Object()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_RuntimeCompilerServices)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Private_CoreLib_System_Runtime_CompilerServices_RuntimeWrappedException_get_WrappedException_System_Object()
    {
    var result = new RuntimeWrappedException(42).WrappedException;
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.Runtime.CompilerServices.RuntimeWrappedException::get_WrappedException:System.Object()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_RuntimeCompilerServices)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Runtime_CompilerServices_RuntimeWrappedException_get_WrappedException_System_Object() { _ = new RuntimeWrappedException(42).WrappedException; }

    [HotUpdateSubjectId("System.Private.CoreLib/System.Runtime.CompilerServices.RuntimeWrappedException::get_WrappedException:System.Object()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_RuntimeCompilerServices)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Runtime_CompilerServices_RuntimeWrappedException_get_WrappedException_System_Object() { _ = new RuntimeWrappedException(42).WrappedException; }
}
