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

// Auto-generated benchmark skeletons for GenericNullableValueBenchmarks.
// Framework handles timing — body only needs to invoke the method under measurement.
public static class GenericNullableValueBenchmarks
{
    // Purpose: Benchmark native-runtime performance of Nullable.get_HasValue with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Nullable`1::get_HasValue:System.Boolean()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_GenericNullableValue)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Nullable_1_get_HasValue_System_Boolean() { _ = default(Nullable<int>).HasValue; }
    // Purpose: Benchmark native-runtime performance of Nullable.get_Value with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Nullable`1::get_Value:T()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_GenericNullableValue)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Nullable_1_get_Value_T() { _ = default(Nullable<int>).Value; }
    // Purpose: Benchmark native-runtime performance of Nullable.GetValueOrDefault with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Nullable`1::GetValueOrDefault:T()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_GenericNullableValue)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Nullable_1_GetValueOrDefault_T() { _ = default(Nullable<int>).GetValueOrDefault(); }
    // Purpose: Benchmark native-runtime performance of Nullable.GetValueOrDefault with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Nullable`1::GetValueOrDefault:T(T)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_GenericNullableValue)]
    public static void Benchmark_System_Private_CoreLib_System_Nullable_1_GetValueOrDefault_T_T() { }
    // Purpose: Benchmark native-runtime performance of Nullable.Equals with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Nullable`1::Equals:System.Boolean(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_GenericNullableValue)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Nullable_1_Equals_System_Boolean_System_Object() { _ = default(Nullable<int>).Equals(42); }
    // Purpose: Benchmark native-runtime performance of Nullable.GetHashCode with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Nullable`1::GetHashCode:System.Int32()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_GenericNullableValue)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Nullable_1_GetHashCode_System_Int32() { _ = default(Nullable<int>).GetHashCode(); }
    // Purpose: Benchmark native-runtime performance of Nullable.ToString with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Nullable`1::ToString:System.String()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_GenericNullableValue)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Nullable_1_ToString_System_String() { _ = default(Nullable<int>).ToString(); }
    // Purpose: Benchmark native-runtime performance of ValueType.Equals with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.ValueType::Equals:System.Boolean(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_GenericNullableValue)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_ValueType_Equals_System_Boolean_System_Object() { _ = ((ValueType)42).Equals(42); }
    // Purpose: Benchmark native-runtime performance of ValueType.GetHashCode with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.ValueType::GetHashCode:System.Int32()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_GenericNullableValue)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_ValueType_GetHashCode_System_Int32() { _ = ((ValueType)42).GetHashCode(); }
    // Purpose: Benchmark native-runtime performance of ValueType.ToString with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.ValueType::ToString:System.String()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_GenericNullableValue)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_ValueType_ToString_System_String() { _ = ((ValueType)42).ToString(); }}
