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

// Auto-generated skeleton for Generic Nullable Value Semantics (family/System.Private.CoreLib/generic/nullable-value).
// Contains test, benchmark, and hotupdate methods merged into one partial class.
// No xunit dependency — [Fact] attributes belong in the test exe project.
public partial class GenericNullableValue
{
    // Nullable.get_HasValue
    [MethodSubjectId("System.Private.CoreLib/System.Nullable`1::get_HasValue:System.Boolean()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_GenericNullableValue)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Private_CoreLib_System_Nullable_1_get_HasValue_System_Boolean()
    {
    var result = default(Nullable<int>).HasValue;
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.Nullable`1::get_HasValue:System.Boolean()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_GenericNullableValue)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Nullable_1_get_HasValue_System_Boolean() { _ = default(Nullable<int>).HasValue; }

    [HotUpdateSubjectId("System.Private.CoreLib/System.Nullable`1::get_HasValue:System.Boolean()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_GenericNullableValue)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Nullable_1_get_HasValue_System_Boolean() { _ = default(Nullable<int>).HasValue; }

    // Nullable.get_Value
    [MethodSubjectId("System.Private.CoreLib/System.Nullable`1::get_Value:T()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_GenericNullableValue)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Private_CoreLib_System_Nullable_1_get_Value_T()
    {
    // TODO: needs-manual — get_Value with 0 params requires manual implementation
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.Nullable`1::get_Value:T()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_GenericNullableValue)]
    public static void Benchmark_System_Private_CoreLib_System_Nullable_1_get_Value_T() { }

    [HotUpdateSubjectId("System.Private.CoreLib/System.Nullable`1::get_Value:T()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_GenericNullableValue)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Nullable_1_get_Value_T() { }

    // Nullable.GetValueOrDefault
    [MethodSubjectId("System.Private.CoreLib/System.Nullable`1::GetValueOrDefault:T()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_GenericNullableValue)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Private_CoreLib_System_Nullable_1_GetValueOrDefault_T()
    {
    _ = default(Nullable<int>).GetValueOrDefault();
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.Nullable`1::GetValueOrDefault:T()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_GenericNullableValue)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Nullable_1_GetValueOrDefault_T() { _ = default(Nullable<int>).GetValueOrDefault(); }

    [HotUpdateSubjectId("System.Private.CoreLib/System.Nullable`1::GetValueOrDefault:T()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_GenericNullableValue)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Nullable_1_GetValueOrDefault_T() { _ = default(Nullable<int>).GetValueOrDefault(); }

    // Nullable.GetValueOrDefault
    [MethodSubjectId("System.Private.CoreLib/System.Nullable`1::GetValueOrDefault:T(T)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_GenericNullableValue)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Private_CoreLib_System_Nullable_1_GetValueOrDefault_T_T()
    {
    // TODO: needs-manual — ref/pointer/unsafe parameter requires unsafe context
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.Nullable`1::GetValueOrDefault:T(T)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_GenericNullableValue)]
    public static void Benchmark_System_Private_CoreLib_System_Nullable_1_GetValueOrDefault_T_T() { }

    [HotUpdateSubjectId("System.Private.CoreLib/System.Nullable`1::GetValueOrDefault:T(T)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_GenericNullableValue)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Nullable_1_GetValueOrDefault_T_T() { }

    // Nullable.Equals
    [MethodSubjectId("System.Private.CoreLib/System.Nullable`1::Equals:System.Boolean(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_GenericNullableValue)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Private_CoreLib_System_Nullable_1_Equals_System_Boolean_System_Object()
    {
    var result = default(Nullable<int>).Equals(42);
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.Nullable`1::Equals:System.Boolean(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_GenericNullableValue)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Nullable_1_Equals_System_Boolean_System_Object() { _ = default(Nullable<int>).Equals(42); }

    [HotUpdateSubjectId("System.Private.CoreLib/System.Nullable`1::Equals:System.Boolean(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_GenericNullableValue)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Nullable_1_Equals_System_Boolean_System_Object() { _ = default(Nullable<int>).Equals(42); }

    // Nullable.GetHashCode
    [MethodSubjectId("System.Private.CoreLib/System.Nullable`1::GetHashCode:System.Int32()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_GenericNullableValue)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Private_CoreLib_System_Nullable_1_GetHashCode_System_Int32()
    {
    var result = default(Nullable<int>).GetHashCode();
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.Nullable`1::GetHashCode:System.Int32()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_GenericNullableValue)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Nullable_1_GetHashCode_System_Int32() { _ = default(Nullable<int>).GetHashCode(); }

    [HotUpdateSubjectId("System.Private.CoreLib/System.Nullable`1::GetHashCode:System.Int32()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_GenericNullableValue)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Nullable_1_GetHashCode_System_Int32() { _ = default(Nullable<int>).GetHashCode(); }

    // Nullable.ToString
    [MethodSubjectId("System.Private.CoreLib/System.Nullable`1::ToString:System.String()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_GenericNullableValue)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Private_CoreLib_System_Nullable_1_ToString_System_String()
    {
    var result = default(Nullable<int>).ToString();
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.Nullable`1::ToString:System.String()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_GenericNullableValue)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Nullable_1_ToString_System_String() { _ = default(Nullable<int>).ToString(); }

    [HotUpdateSubjectId("System.Private.CoreLib/System.Nullable`1::ToString:System.String()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_GenericNullableValue)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Nullable_1_ToString_System_String() { _ = default(Nullable<int>).ToString(); }

    // ValueType.Equals
    [MethodSubjectId("System.Private.CoreLib/System.ValueType::Equals:System.Boolean(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_GenericNullableValue)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Private_CoreLib_System_ValueType_Equals_System_Boolean_System_Object()
    {
    var result = ((ValueType)42).Equals(42);
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.ValueType::Equals:System.Boolean(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_GenericNullableValue)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_ValueType_Equals_System_Boolean_System_Object() { _ = ((ValueType)42).Equals(42); }

    [HotUpdateSubjectId("System.Private.CoreLib/System.ValueType::Equals:System.Boolean(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_GenericNullableValue)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_ValueType_Equals_System_Boolean_System_Object() { _ = ((ValueType)42).Equals(42); }

    // ValueType.GetHashCode
    [MethodSubjectId("System.Private.CoreLib/System.ValueType::GetHashCode:System.Int32()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_GenericNullableValue)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Private_CoreLib_System_ValueType_GetHashCode_System_Int32()
    {
    var result = ((ValueType)42).GetHashCode();
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.ValueType::GetHashCode:System.Int32()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_GenericNullableValue)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_ValueType_GetHashCode_System_Int32() { _ = ((ValueType)42).GetHashCode(); }

    [HotUpdateSubjectId("System.Private.CoreLib/System.ValueType::GetHashCode:System.Int32()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_GenericNullableValue)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_ValueType_GetHashCode_System_Int32() { _ = ((ValueType)42).GetHashCode(); }

    // ValueType.ToString
    [MethodSubjectId("System.Private.CoreLib/System.ValueType::ToString:System.String()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_GenericNullableValue)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Private_CoreLib_System_ValueType_ToString_System_String()
    {
    var result = ((ValueType)42).ToString();
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.ValueType::ToString:System.String()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_GenericNullableValue)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_ValueType_ToString_System_String() { _ = ((ValueType)42).ToString(); }

    [HotUpdateSubjectId("System.Private.CoreLib/System.ValueType::ToString:System.String()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_GenericNullableValue)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_ValueType_ToString_System_String() { _ = ((ValueType)42).ToString(); }
}
