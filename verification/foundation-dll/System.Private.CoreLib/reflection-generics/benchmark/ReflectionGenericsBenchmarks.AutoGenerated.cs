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

// Auto-generated benchmark skeletons for ReflectionGenericsBenchmarks.
// Framework handles timing — body only needs to invoke the method under measurement.
public static class ReflectionGenericsBenchmarks
{
    // Purpose: Benchmark native-runtime performance of Type.GetGenericTypeDefinition with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Type::GetGenericTypeDefinition:System.Type()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionGenerics)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Type_GetGenericTypeDefinition_System_Type() { _ = typeof(byte).GetGenericTypeDefinition(); }
    // Purpose: Benchmark native-runtime performance of Type.MakeGenericType with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Type::MakeGenericType:System.Type(System.Type[])")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionGenerics)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Type_MakeGenericType_System_Type_System_Type() { _ = typeof(byte).MakeGenericType(Array.Empty<System.Type>()); }
    // Purpose: Benchmark native-runtime performance of Type.GetGenericArguments with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Type::GetGenericArguments:System.Type[]()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionGenerics)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Type_GetGenericArguments_System_Type() { _ = typeof(byte).GetGenericArguments(); }
    // Purpose: Benchmark native-runtime performance of Type.GetGenericParameterConstraints with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Type::GetGenericParameterConstraints:System.Type[]()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionGenerics)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Type_GetGenericParameterConstraints_System_Type() { _ = typeof(byte).GetGenericParameterConstraints(); }
    // Purpose: Benchmark native-runtime performance of Type.get_GenericParameterPosition with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Type::get_GenericParameterPosition:System.Int32()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionGenerics)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Type_get_GenericParameterPosition_System_Int32() { _ = typeof(byte).GenericParameterPosition; }
    // Purpose: Benchmark native-runtime performance of Type.ContainsGenericParameters with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Type::ContainsGenericParameters:System.Boolean()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionGenerics)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Type_ContainsGenericParameters_System_Boolean() { _ = typeof(byte).ContainsGenericParameters(); }
    // Purpose: Benchmark native-runtime performance of Type.get_IsGenericType with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Type::get_IsGenericType:System.Boolean()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionGenerics)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Type_get_IsGenericType_System_Boolean() { _ = typeof(byte).IsGenericType; }
    // Purpose: Benchmark native-runtime performance of Type.get_IsGenericTypeDefinition with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Type::get_IsGenericTypeDefinition:System.Boolean()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionGenerics)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Type_get_IsGenericTypeDefinition_System_Boolean() { _ = typeof(byte).IsGenericTypeDefinition; }
    // Purpose: Benchmark native-runtime performance of Type.get_IsConstructedGenericType with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Type::get_IsConstructedGenericType:System.Boolean()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionGenerics)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Type_get_IsConstructedGenericType_System_Boolean() { _ = typeof(byte).IsConstructedGenericType; }
    // Purpose: Benchmark native-runtime performance of Type.MakeArrayType with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Type::MakeArrayType:System.Type()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionGenerics)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Type_MakeArrayType_System_Type() { _ = typeof(byte).MakeArrayType(); }
    // Purpose: Benchmark native-runtime performance of Type.MakeByRefType with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Type::MakeByRefType:System.Type()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionGenerics)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Type_MakeByRefType_System_Type() { _ = typeof(byte).MakeByRefType(); }
    // Purpose: Benchmark native-runtime performance of Type.MakePointerType with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Type::MakePointerType:System.Type()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionGenerics)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Type_MakePointerType_System_Type() { _ = typeof(byte).MakePointerType(); }}
