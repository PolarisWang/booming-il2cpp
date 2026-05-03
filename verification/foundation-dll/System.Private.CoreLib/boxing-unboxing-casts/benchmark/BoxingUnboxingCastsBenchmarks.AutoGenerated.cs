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

// Auto-generated benchmark skeletons for BoxingUnboxingCastsBenchmarks.
// Framework handles timing — body only needs to invoke the method under measurement.
public static class BoxingUnboxingCastsBenchmarks
{
    // Purpose: Benchmark native-runtime performance of Convert.ChangeType with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Convert::ChangeType:System.Object(System.Object,System.Type)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_BoxingUnboxingCasts)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Convert_ChangeType_System_Object_System_Object_System_Type() { _ = Convert.ChangeType(42, typeof(byte)); }
    // Purpose: Benchmark native-runtime performance of Convert.ChangeType with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Convert::ChangeType:System.Object(System.Object,System.Type,System.IFormatProvider)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_BoxingUnboxingCasts)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Convert_ChangeType_System_Object_System_Object_System_Type_System_IFormatProvider() { _ = Convert.ChangeType(42, typeof(byte), null!); }
    // Purpose: Benchmark native-runtime performance of Type.IsAssignableFrom with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Type::IsAssignableFrom:System.Boolean(System.Type)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_BoxingUnboxingCasts)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Type_IsAssignableFrom_System_Boolean_System_Type() { _ = typeof(byte).IsAssignableFrom(typeof(byte)); }
    // Purpose: Benchmark native-runtime performance of Type.IsInstanceOfType with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Type::IsInstanceOfType:System.Boolean(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_BoxingUnboxingCasts)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Type_IsInstanceOfType_System_Boolean_System_Object() { _ = typeof(byte).IsInstanceOfType(42); }
    // Purpose: Benchmark native-runtime performance of Type.IsSubclassOf with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Type::IsSubclassOf:System.Boolean(System.Type)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_BoxingUnboxingCasts)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Type_IsSubclassOf_System_Boolean_System_Type() { _ = typeof(byte).IsSubclassOf(typeof(byte)); }
    // Purpose: Benchmark native-runtime performance of Type.IsAssignableTo with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Type::IsAssignableTo:System.Boolean(System.Type)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_BoxingUnboxingCasts)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Type_IsAssignableTo_System_Boolean_System_Type() { _ = typeof(byte).IsAssignableTo(typeof(byte)); }
    // Purpose: Benchmark native-runtime performance of Object.MemberwiseClone with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Object::MemberwiseClone:System.Object()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_BoxingUnboxingCasts)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Object_MemberwiseClone_System_Object() { }
    // Purpose: Benchmark native-runtime performance of RuntimeHelpers.GetObjectValue with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Runtime.CompilerServices.RuntimeHelpers::GetObjectValue:System.Object(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_BoxingUnboxingCasts)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Runtime_CompilerServices_RuntimeHelpers_GetObjectValue_System_Object_System_Object() { _ = RuntimeHelpers.GetObjectValue(42); }
    // Purpose: Benchmark native-runtime performance of Array.CreateInstance with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Array::CreateInstance:System.Array(System.Type,System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_BoxingUnboxingCasts)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Array_CreateInstance_System_Array_System_Type_System_Int32() { _ = Array.CreateInstance(typeof(byte), 42); }
    // Purpose: Benchmark native-runtime performance of Array.CreateInstance with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Array::CreateInstance:System.Array(System.Type,System.Int32,System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_BoxingUnboxingCasts)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Array_CreateInstance_System_Array_System_Type_System_Int32_System_Int32() { _ = Array.CreateInstance(typeof(byte), 42, 42); }
    // Purpose: Benchmark native-runtime performance of ValueType.Equals with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.ValueType::Equals:System.Boolean(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_BoxingUnboxingCasts)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_ValueType_Equals_System_Boolean_System_Object() { _ = ((ValueType)42).Equals(42); }}
