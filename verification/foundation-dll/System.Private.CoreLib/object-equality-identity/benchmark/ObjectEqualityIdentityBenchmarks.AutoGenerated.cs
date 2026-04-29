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

// Auto-generated benchmark skeletons for ObjectEqualityIdentityBenchmarks.
// Framework handles timing — body only needs to invoke the method under measurement.
public static class ObjectEqualityIdentityBenchmarks
{
    // Purpose: Benchmark native-runtime performance of Object.Equals with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Object::Equals:System.Boolean(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ObjectEqualityIdentity)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Object_Equals_System_Boolean_System_Object() { _ = new object().Equals(42); }
    // Purpose: Benchmark native-runtime performance of Object.Equals with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Object::Equals:System.Boolean(System.Object,System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ObjectEqualityIdentity)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Object_Equals_System_Boolean_System_Object_System_Object() { _ = Object.Equals(42, 42); }
    // Purpose: Benchmark native-runtime performance of Object.ReferenceEquals with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Object::ReferenceEquals:System.Boolean(System.Object,System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ObjectEqualityIdentity)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Object_ReferenceEquals_System_Boolean_System_Object_System_Object() { _ = Object.ReferenceEquals(42, 42); }
    // Purpose: Benchmark native-runtime performance of Object.GetHashCode with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Object::GetHashCode:System.Int32()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ObjectEqualityIdentity)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Object_GetHashCode_System_Int32() { _ = new object().GetHashCode(); }
    // Purpose: Benchmark native-runtime performance of Object.ToString with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Object::ToString:System.String()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ObjectEqualityIdentity)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Object_ToString_System_String() { _ = new object().ToString(); }
    // Purpose: Benchmark native-runtime performance of Object.GetType with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Object::GetType:System.Type()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ObjectEqualityIdentity)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Object_GetType_System_Type() { _ = new object().GetType(); }
    // Purpose: Benchmark native-runtime performance of Object.MemberwiseClone with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Object::MemberwiseClone:System.Object()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ObjectEqualityIdentity)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Object_MemberwiseClone_System_Object() { _ = new object().MemberwiseClone(); }}
