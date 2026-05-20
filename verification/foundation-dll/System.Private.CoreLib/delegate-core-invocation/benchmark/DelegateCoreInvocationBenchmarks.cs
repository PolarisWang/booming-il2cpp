using System;
using System.IO;
using System.Threading;
using System.Threading.Tasks;
using System.Globalization;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;
using System.Collections.Generic;
using System.Reflection;
using Chaos.TestFramework;

// Benchmark stubs for family/System.Private.CoreLib/delegate/core-invocation
public static class DelegateCoreInvocationBenchmarks
{
    // Benchmark Delegate.Combine native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Delegate::Combine:System.Delegate(System.Delegate,System.Delegate)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_DelegateCoreInvocation)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Delegate_Combine_System_Delegate_System_Delegate_System_Delegate() { _ = Delegate.Combine(null!, null!); }
    // Benchmark Delegate.Remove native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Delegate::Remove:System.Delegate(System.Delegate,System.Delegate)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_DelegateCoreInvocation)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Delegate_Remove_System_Delegate_System_Delegate_System_Delegate() { _ = Delegate.Remove(null!, null!); }
    // Benchmark Delegate.DynamicInvoke native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Delegate::DynamicInvoke:System.Object(System.Object[])")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_DelegateCoreInvocation)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Delegate_DynamicInvoke_System_Object_System_Object() { _ = new System.Action(() => {}).DynamicInvoke(); }
    // Benchmark Delegate.get_Method native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Delegate::get_Method:System.Reflection.MethodInfo()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_DelegateCoreInvocation)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Delegate_get_Method_System_Reflection_MethodInfo() { _ = new System.Action(() => {}).Method; }
    // Benchmark Delegate.get_Target native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Delegate::get_Target:System.Object()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_DelegateCoreInvocation)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Delegate_get_Target_System_Object() { _ = new System.Action(() => {}).Target; }
    // Benchmark Delegate.CreateDelegate native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Delegate::CreateDelegate:System.Delegate(System.Type,System.Object,System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_DelegateCoreInvocation)]
    public static void Benchmark_System_Private_CoreLib_System_Delegate_CreateDelegate_System_Delegate_System_Type_System_Object_System_String() { }
    // Benchmark Delegate.CreateDelegate native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Delegate::CreateDelegate:System.Delegate(System.Type,System.Reflection.MethodInfo)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_DelegateCoreInvocation)]
    public static void Benchmark_System_Private_CoreLib_System_Delegate_CreateDelegate_System_Delegate_System_Type_System_Reflection_MethodInfo() { }
    // Benchmark MulticastDelegate.GetInvocationList native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.MulticastDelegate::GetInvocationList:System.Delegate[]()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_DelegateCoreInvocation)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_MulticastDelegate_GetInvocationList_System_Delegate() { _ = new System.Action(() => {}).GetInvocationList(); }
    // Benchmark Delegate.op_Equality native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Delegate::op_Equality:System.Boolean(System.Delegate,System.Delegate)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_DelegateCoreInvocation)]
    public static void Benchmark_System_Private_CoreLib_System_Delegate_op_Equality_System_Boolean_System_Delegate_System_Delegate() { }
    // Benchmark Delegate.op_Inequality native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Delegate::op_Inequality:System.Boolean(System.Delegate,System.Delegate)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_DelegateCoreInvocation)]
    public static void Benchmark_System_Private_CoreLib_System_Delegate_op_Inequality_System_Boolean_System_Delegate_System_Delegate() { }}
