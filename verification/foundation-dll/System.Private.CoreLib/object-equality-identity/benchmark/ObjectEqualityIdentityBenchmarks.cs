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

// Benchmark stubs for family/System.Private.CoreLib/object/equality-identity
public static class ObjectEqualityIdentityBenchmarks
{
    // Benchmark Object.Equals native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Object::Equals:System.Boolean(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ObjectEqualityIdentity)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Object_Equals_System_Boolean_System_Object() { _ = new object().Equals(42); }
    // Benchmark Object.Equals native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Object::Equals:System.Boolean(System.Object,System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ObjectEqualityIdentity)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Object_Equals_System_Boolean_System_Object_System_Object() { _ = Object.Equals(42, 42); }
    // Benchmark Object.ReferenceEquals native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Object::ReferenceEquals:System.Boolean(System.Object,System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ObjectEqualityIdentity)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Object_ReferenceEquals_System_Boolean_System_Object_System_Object() { _ = Object.ReferenceEquals(42, 42); }
    // Benchmark Object.GetHashCode native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Object::GetHashCode:System.Int32()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ObjectEqualityIdentity)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Object_GetHashCode_System_Int32() { _ = new object().GetHashCode(); }
    // Benchmark Object.ToString native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Object::ToString:System.String()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ObjectEqualityIdentity)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Object_ToString_System_String() { _ = new object().ToString(); }
    // Benchmark Object.GetType native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Object::GetType:System.Type()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ObjectEqualityIdentity)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Object_GetType_System_Type() { _ = new object().GetType(); }
    // Benchmark Object.MemberwiseClone native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Object::MemberwiseClone:System.Object()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ObjectEqualityIdentity)]
    public static void Benchmark_System_Private_CoreLib_System_Object_MemberwiseClone_System_Object() { }}
