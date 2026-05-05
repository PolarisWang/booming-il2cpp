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

// Auto-generated skeleton for Delegate Core Invocation (family/System.Private.CoreLib/delegate/core-invocation).
// Contains test, benchmark, and hotupdate methods merged into one partial class.
// No xunit dependency — [Fact] attributes belong in the test exe project.
public partial class DelegateCoreInvocation
{
    // Delegate.Combine
    [MethodSubjectId("System.Private.CoreLib/System.Delegate::Combine:System.Delegate(System.Delegate,System.Delegate)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_DelegateCoreInvocation)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Private_CoreLib_System_Delegate_Combine_System_Delegate_System_Delegate_System_Delegate()
    {
    _ = Delegate.Combine(null!, null!);
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.Delegate::Combine:System.Delegate(System.Delegate,System.Delegate)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_DelegateCoreInvocation)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Delegate_Combine_System_Delegate_System_Delegate_System_Delegate() { _ = Delegate.Combine(null!, null!); }

    [HotUpdateSubjectId("System.Private.CoreLib/System.Delegate::Combine:System.Delegate(System.Delegate,System.Delegate)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_DelegateCoreInvocation)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Delegate_Combine_System_Delegate_System_Delegate_System_Delegate() { _ = Delegate.Combine(null!, null!); }

    // Delegate.Remove
    [MethodSubjectId("System.Private.CoreLib/System.Delegate::Remove:System.Delegate(System.Delegate,System.Delegate)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_DelegateCoreInvocation)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Private_CoreLib_System_Delegate_Remove_System_Delegate_System_Delegate_System_Delegate()
    {
    _ = Delegate.Remove(null!, null!);
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.Delegate::Remove:System.Delegate(System.Delegate,System.Delegate)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_DelegateCoreInvocation)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Delegate_Remove_System_Delegate_System_Delegate_System_Delegate() { _ = Delegate.Remove(null!, null!); }

    [HotUpdateSubjectId("System.Private.CoreLib/System.Delegate::Remove:System.Delegate(System.Delegate,System.Delegate)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_DelegateCoreInvocation)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Delegate_Remove_System_Delegate_System_Delegate_System_Delegate() { _ = Delegate.Remove(null!, null!); }

    // Delegate.DynamicInvoke
    [MethodSubjectId("System.Private.CoreLib/System.Delegate::DynamicInvoke:System.Object(System.Object[])")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_DelegateCoreInvocation)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Private_CoreLib_System_Delegate_DynamicInvoke_System_Object_System_Object()
    {
    // TODO: needs-manual — DynamicInvoke with 1 params requires manual implementation
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.Delegate::DynamicInvoke:System.Object(System.Object[])")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_DelegateCoreInvocation)]
    public static void Benchmark_System_Private_CoreLib_System_Delegate_DynamicInvoke_System_Object_System_Object() { }

    [HotUpdateSubjectId("System.Private.CoreLib/System.Delegate::DynamicInvoke:System.Object(System.Object[])")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_DelegateCoreInvocation)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Delegate_DynamicInvoke_System_Object_System_Object() { }

    // Delegate.get_Method
    [MethodSubjectId("System.Private.CoreLib/System.Delegate::get_Method:System.Reflection.MethodInfo()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_DelegateCoreInvocation)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Private_CoreLib_System_Delegate_get_Method_System_Reflection_MethodInfo()
    {
    // TODO: needs-manual — get_Method with 0 params requires manual implementation
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.Delegate::get_Method:System.Reflection.MethodInfo()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_DelegateCoreInvocation)]
    public static void Benchmark_System_Private_CoreLib_System_Delegate_get_Method_System_Reflection_MethodInfo() { }

    [HotUpdateSubjectId("System.Private.CoreLib/System.Delegate::get_Method:System.Reflection.MethodInfo()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_DelegateCoreInvocation)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Delegate_get_Method_System_Reflection_MethodInfo() { }

    // Delegate.get_Target
    [MethodSubjectId("System.Private.CoreLib/System.Delegate::get_Target:System.Object()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_DelegateCoreInvocation)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Private_CoreLib_System_Delegate_get_Target_System_Object()
    {
    // TODO: needs-manual — get_Target with 0 params requires manual implementation
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.Delegate::get_Target:System.Object()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_DelegateCoreInvocation)]
    public static void Benchmark_System_Private_CoreLib_System_Delegate_get_Target_System_Object() { }

    [HotUpdateSubjectId("System.Private.CoreLib/System.Delegate::get_Target:System.Object()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_DelegateCoreInvocation)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Delegate_get_Target_System_Object() { }

    // Delegate.CreateDelegate
    [MethodSubjectId("System.Private.CoreLib/System.Delegate::CreateDelegate:System.Delegate(System.Type,System.Object,System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_DelegateCoreInvocation)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Private_CoreLib_System_Delegate_CreateDelegate_System_Delegate_System_Type_System_Object_System_String()
    {
    // TODO: needs-manual — CreateDelegate with 3 params requires manual implementation
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.Delegate::CreateDelegate:System.Delegate(System.Type,System.Object,System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_DelegateCoreInvocation)]
    public static void Benchmark_System_Private_CoreLib_System_Delegate_CreateDelegate_System_Delegate_System_Type_System_Object_System_String() { }

    [HotUpdateSubjectId("System.Private.CoreLib/System.Delegate::CreateDelegate:System.Delegate(System.Type,System.Object,System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_DelegateCoreInvocation)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Delegate_CreateDelegate_System_Delegate_System_Type_System_Object_System_String() { }

    // Delegate.CreateDelegate
    [MethodSubjectId("System.Private.CoreLib/System.Delegate::CreateDelegate:System.Delegate(System.Type,System.Reflection.MethodInfo)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_DelegateCoreInvocation)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Private_CoreLib_System_Delegate_CreateDelegate_System_Delegate_System_Type_System_Reflection_MethodInfo()
    {
    // TODO: needs-manual — CreateDelegate with 2 params requires manual implementation
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.Delegate::CreateDelegate:System.Delegate(System.Type,System.Reflection.MethodInfo)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_DelegateCoreInvocation)]
    public static void Benchmark_System_Private_CoreLib_System_Delegate_CreateDelegate_System_Delegate_System_Type_System_Reflection_MethodInfo() { }

    [HotUpdateSubjectId("System.Private.CoreLib/System.Delegate::CreateDelegate:System.Delegate(System.Type,System.Reflection.MethodInfo)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_DelegateCoreInvocation)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Delegate_CreateDelegate_System_Delegate_System_Type_System_Reflection_MethodInfo() { }

    // MulticastDelegate.GetInvocationList
    [MethodSubjectId("System.Private.CoreLib/System.MulticastDelegate::GetInvocationList:System.Delegate[]()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_DelegateCoreInvocation)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Private_CoreLib_System_MulticastDelegate_GetInvocationList_System_Delegate()
    {
    // TODO: needs-manual — GetInvocationList with 0 params requires manual implementation
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.MulticastDelegate::GetInvocationList:System.Delegate[]()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_DelegateCoreInvocation)]
    public static void Benchmark_System_Private_CoreLib_System_MulticastDelegate_GetInvocationList_System_Delegate() { }

    [HotUpdateSubjectId("System.Private.CoreLib/System.MulticastDelegate::GetInvocationList:System.Delegate[]()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_DelegateCoreInvocation)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_MulticastDelegate_GetInvocationList_System_Delegate() { }

    // Delegate.op_Equality
    [MethodSubjectId("System.Private.CoreLib/System.Delegate::op_Equality:System.Boolean(System.Delegate,System.Delegate)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_DelegateCoreInvocation)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Private_CoreLib_System_Delegate_op_Equality_System_Boolean_System_Delegate_System_Delegate()
    {
    // TODO: needs-manual — op_Equality requires manual implementation
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.Delegate::op_Equality:System.Boolean(System.Delegate,System.Delegate)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_DelegateCoreInvocation)]
    public static void Benchmark_System_Private_CoreLib_System_Delegate_op_Equality_System_Boolean_System_Delegate_System_Delegate() { }

    [HotUpdateSubjectId("System.Private.CoreLib/System.Delegate::op_Equality:System.Boolean(System.Delegate,System.Delegate)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_DelegateCoreInvocation)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Delegate_op_Equality_System_Boolean_System_Delegate_System_Delegate() { }

    // Delegate.op_Inequality
    [MethodSubjectId("System.Private.CoreLib/System.Delegate::op_Inequality:System.Boolean(System.Delegate,System.Delegate)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_DelegateCoreInvocation)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Private_CoreLib_System_Delegate_op_Inequality_System_Boolean_System_Delegate_System_Delegate()
    {
    // TODO: needs-manual — op_Inequality requires manual implementation
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.Delegate::op_Inequality:System.Boolean(System.Delegate,System.Delegate)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_DelegateCoreInvocation)]
    public static void Benchmark_System_Private_CoreLib_System_Delegate_op_Inequality_System_Boolean_System_Delegate_System_Delegate() { }

    [HotUpdateSubjectId("System.Private.CoreLib/System.Delegate::op_Inequality:System.Boolean(System.Delegate,System.Delegate)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_DelegateCoreInvocation)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Delegate_op_Inequality_System_Boolean_System_Delegate_System_Delegate() { }
}
