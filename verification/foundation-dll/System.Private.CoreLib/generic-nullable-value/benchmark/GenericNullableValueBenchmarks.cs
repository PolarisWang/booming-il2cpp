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

// Benchmark stubs for family/System.Private.CoreLib/generic/nullable-value
public static class GenericNullableValueBenchmarks
{
    // Benchmark Nullable.get_HasValue native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Nullable`1::get_HasValue:System.Boolean()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_GenericNullableValue)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Nullable_1_get_HasValue_System_Boolean() { _ = default(Nullable<int>).HasValue; }
    // Benchmark Nullable.get_Value native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Nullable`1::get_Value:T()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_GenericNullableValue)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Nullable_1_get_Value_T() { _ = ((int?)42).Value; }
    // Benchmark Nullable.GetValueOrDefault native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Nullable`1::GetValueOrDefault:T()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_GenericNullableValue)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Nullable_1_GetValueOrDefault_T() { _ = default(Nullable<int>).GetValueOrDefault(); }
    // Benchmark Nullable.GetValueOrDefault native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Nullable`1::GetValueOrDefault:T(T)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_GenericNullableValue)]
    public static void Benchmark_System_Private_CoreLib_System_Nullable_1_GetValueOrDefault_T_T() { }
    // Benchmark Nullable.Equals native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Nullable`1::Equals:System.Boolean(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_GenericNullableValue)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Nullable_1_Equals_System_Boolean_System_Object() { _ = default(Nullable<int>).Equals(42); }
    // Benchmark Nullable.GetHashCode native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Nullable`1::GetHashCode:System.Int32()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_GenericNullableValue)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Nullable_1_GetHashCode_System_Int32() { _ = default(Nullable<int>).GetHashCode(); }
    // Benchmark Nullable.ToString native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Nullable`1::ToString:System.String()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_GenericNullableValue)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Nullable_1_ToString_System_String() { _ = default(Nullable<int>).ToString(); }
    // Benchmark ValueType.Equals native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.ValueType::Equals:System.Boolean(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_GenericNullableValue)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_ValueType_Equals_System_Boolean_System_Object() { _ = ((ValueType)42).Equals(42); }
    // Benchmark ValueType.GetHashCode native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.ValueType::GetHashCode:System.Int32()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_GenericNullableValue)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_ValueType_GetHashCode_System_Int32() { _ = ((ValueType)42).GetHashCode(); }
    // Benchmark ValueType.ToString native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.ValueType::ToString:System.String()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_GenericNullableValue)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_ValueType_ToString_System_String() { _ = ((ValueType)42).ToString(); }}
