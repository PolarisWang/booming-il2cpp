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

// Benchmark stubs for family/System.Private.CoreLib/reflection/hotupdate
public static class ReflectionHotupdateBenchmarks
{
    // Benchmark Assembly.GetModules native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Reflection.Assembly::GetModules:System.Reflection.Module[]()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionHotupdate)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Reflection_Assembly_GetModules_System_Reflection_Module() { _ = typeof(byte).Assembly.GetModules(); }
    // Benchmark Assembly.GetTypes native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Reflection.Assembly::GetTypes:System.Type[]()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionHotupdate)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Reflection_Assembly_GetTypes_System_Type() { _ = typeof(byte).Assembly.GetTypes(); }
    // Benchmark Type.GetTypeFromHandle native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Type::GetTypeFromHandle:System.Type(System.RuntimeTypeHandle)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionHotupdate)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Type_GetTypeFromHandle_System_Type_System_RuntimeTypeHandle() { _ = Type.GetTypeFromHandle(default(System.RuntimeTypeHandle)); }
    // Benchmark Assembly.GetType native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Reflection.Assembly::GetType:System.Type(System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionHotupdate)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Reflection_Assembly_GetType_System_Type_System_String() { _ = typeof(byte).Assembly.GetType("hello"); }}
