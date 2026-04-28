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

// Auto-generated benchmark skeletons for DelegateCoreInvocationBenchmarks.
// Framework handles timing — body only needs to invoke the method under measurement.
public static class DelegateCoreInvocationBenchmarks
{
    // Purpose: Benchmark native-runtime performance of Delegate.Combine with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Delegate::Combine:System.Delegate(System.Delegate,System.Delegate)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_DelegateCoreInvocation)]
    public static void Benchmark_System_Private_CoreLib_System_Delegate_Combine_System_Delegate_System_Delegate_System_Delegate() { }
    // Purpose: Benchmark native-runtime performance of Delegate.Remove with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Delegate::Remove:System.Delegate(System.Delegate,System.Delegate)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_DelegateCoreInvocation)]
    public static void Benchmark_System_Private_CoreLib_System_Delegate_Remove_System_Delegate_System_Delegate_System_Delegate() { }
    // Purpose: Benchmark native-runtime performance of Delegate.DynamicInvoke with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Delegate::DynamicInvoke:System.Object(System.Object[])")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_DelegateCoreInvocation)]
    public static void Benchmark_System_Private_CoreLib_System_Delegate_DynamicInvoke_System_Object_System_Object() { }
    // Purpose: Benchmark native-runtime performance of Delegate.get_Method with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Delegate::get_Method:System.Reflection.MethodInfo()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_DelegateCoreInvocation)]
    public static void Benchmark_System_Private_CoreLib_System_Delegate_get_Method_System_Reflection_MethodInfo() { }
    // Purpose: Benchmark native-runtime performance of Delegate.get_Target with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Delegate::get_Target:System.Object()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_DelegateCoreInvocation)]
    public static void Benchmark_System_Private_CoreLib_System_Delegate_get_Target_System_Object() { }
    // Purpose: Benchmark native-runtime performance of Delegate.CreateDelegate with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Delegate::CreateDelegate:System.Delegate(System.Type,System.Object,System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_DelegateCoreInvocation)]
    public static void Benchmark_System_Private_CoreLib_System_Delegate_CreateDelegate_System_Delegate_System_Type_System_Object_System_String() { }
    // Purpose: Benchmark native-runtime performance of Delegate.CreateDelegate with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Delegate::CreateDelegate:System.Delegate(System.Type,System.Reflection.MethodInfo)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_DelegateCoreInvocation)]
    public static void Benchmark_System_Private_CoreLib_System_Delegate_CreateDelegate_System_Delegate_System_Type_System_Reflection_MethodInfo() { }
    // Purpose: Benchmark native-runtime performance of MulticastDelegate.GetInvocationList with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.MulticastDelegate::GetInvocationList:System.Delegate[]()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_DelegateCoreInvocation)]
    public static void Benchmark_System_Private_CoreLib_System_MulticastDelegate_GetInvocationList_System_Delegate() { }
    // Purpose: Benchmark native-runtime performance of Delegate.op_Equality with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Delegate::op_Equality:System.Boolean(System.Delegate,System.Delegate)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_DelegateCoreInvocation)]
    public static void Benchmark_System_Private_CoreLib_System_Delegate_op_Equality_System_Boolean_System_Delegate_System_Delegate() { }
    // Purpose: Benchmark native-runtime performance of Delegate.op_Inequality with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Delegate::op_Inequality:System.Boolean(System.Delegate,System.Delegate)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_DelegateCoreInvocation)]
    public static void Benchmark_System_Private_CoreLib_System_Delegate_op_Inequality_System_Boolean_System_Delegate_System_Delegate() { }}
