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

// Auto-generated benchmark skeletons for FamilySystem.runtime.interopservicesMarshallingAttributesBenchmarks.
// Framework handles timing — body only needs to invoke the method under measurement.
public static class FamilySystem.runtime.interopservicesMarshallingAttributesBenchmarks
{
    // Purpose: Benchmark native-runtime performance of MarshalAsAttribute.MarshalAsAttribute: with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.MarshalAsAttribute::MarshalAsAttribute::Void(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_MarshalAsAttribute_MarshalAsAttribute_Void_System_Object() { _ = MarshalAsAttribute.MarshalAsAttribute:(42); }
    // Purpose: Benchmark native-runtime performance of MarshalAsAttribute.get_Value with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.MarshalAsAttribute::get_Value:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_MarshalAsAttribute_get_Value_System_Void() { MarshalAsAttribute.Value; }
    // Purpose: Benchmark native-runtime performance of MarshalAsAttribute.set_Value with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.MarshalAsAttribute::set_Value:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_MarshalAsAttribute_set_Value_System_Void_P0() { MarshalAsAttribute.Value = null!; }
    // Purpose: Benchmark native-runtime performance of OptionalAttribute.OptionalAttribute: with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.OptionalAttribute::OptionalAttribute::Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_OptionalAttribute_OptionalAttribute_Void() { _ = OptionalAttribute.OptionalAttribute:(); }
    // Purpose: Benchmark native-runtime performance of PreserveSigAttribute.PreserveSigAttribute: with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.PreserveSigAttribute::PreserveSigAttribute::Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_PreserveSigAttribute_PreserveSigAttribute_Void() { _ = PreserveSigAttribute.PreserveSigAttribute:(); }}
