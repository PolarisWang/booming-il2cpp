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

// Benchmark stubs for family/System.Private.CoreLib/primitive/numeric-conversions-core
public static class PrimitiveNumericConversionsCoreBenchmarks
{
    // Benchmark PrimitiveNumericConversionsCoreNativeEntry.TestByteToInt native-runtime throughput
    [BenchmarkSubjectId("PrimitiveNumericConversionsCoreNativeEntry/PrimitiveNumericConversionsCoreNativeEntry::TestByteToInt:System.Int32()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_PrimitiveNumericConversionsCore)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_PrimitiveNumericConversionsCoreNativeEntry_PrimitiveNumericConversionsCoreNativeEntry_TestByteToInt_System_Int32() { _ = PrimitiveNumericConversionsCoreNativeEntry.TestByteToInt(); }
    // Benchmark PrimitiveNumericConversionsCoreNativeEntry.TestIntToLong native-runtime throughput
    [BenchmarkSubjectId("PrimitiveNumericConversionsCoreNativeEntry/PrimitiveNumericConversionsCoreNativeEntry::TestIntToLong:System.Int32()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_PrimitiveNumericConversionsCore)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_PrimitiveNumericConversionsCoreNativeEntry_PrimitiveNumericConversionsCoreNativeEntry_TestIntToLong_System_Int32() { _ = PrimitiveNumericConversionsCoreNativeEntry.TestIntToLong(); }
    // Benchmark PrimitiveNumericConversionsCoreNativeEntry.TestIntToShort native-runtime throughput
    [BenchmarkSubjectId("PrimitiveNumericConversionsCoreNativeEntry/PrimitiveNumericConversionsCoreNativeEntry::TestIntToShort:System.Int32()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_PrimitiveNumericConversionsCore)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_PrimitiveNumericConversionsCoreNativeEntry_PrimitiveNumericConversionsCoreNativeEntry_TestIntToShort_System_Int32() { _ = PrimitiveNumericConversionsCoreNativeEntry.TestIntToShort(); }
    // Benchmark PrimitiveNumericConversionsCoreNativeEntry.TestIntToByte native-runtime throughput
    [BenchmarkSubjectId("PrimitiveNumericConversionsCoreNativeEntry/PrimitiveNumericConversionsCoreNativeEntry::TestIntToByte:System.Int32()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_PrimitiveNumericConversionsCore)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_PrimitiveNumericConversionsCoreNativeEntry_PrimitiveNumericConversionsCoreNativeEntry_TestIntToByte_System_Int32() { _ = PrimitiveNumericConversionsCoreNativeEntry.TestIntToByte(); }
    // Benchmark PrimitiveNumericConversionsCoreNativeEntry.TestFloatToDouble native-runtime throughput
    [BenchmarkSubjectId("PrimitiveNumericConversionsCoreNativeEntry/PrimitiveNumericConversionsCoreNativeEntry::TestFloatToDouble:System.Int32()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_PrimitiveNumericConversionsCore)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_PrimitiveNumericConversionsCoreNativeEntry_PrimitiveNumericConversionsCoreNativeEntry_TestFloatToDouble_System_Int32() { _ = PrimitiveNumericConversionsCoreNativeEntry.TestFloatToDouble(); }
    // Benchmark PrimitiveNumericConversionsCoreNativeEntry.TestDoubleToFloat native-runtime throughput
    [BenchmarkSubjectId("PrimitiveNumericConversionsCoreNativeEntry/PrimitiveNumericConversionsCoreNativeEntry::TestDoubleToFloat:System.Int32()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_PrimitiveNumericConversionsCore)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_PrimitiveNumericConversionsCoreNativeEntry_PrimitiveNumericConversionsCoreNativeEntry_TestDoubleToFloat_System_Int32() { _ = PrimitiveNumericConversionsCoreNativeEntry.TestDoubleToFloat(); }
    // Benchmark PrimitiveNumericConversionsCoreNativeEntry.TestIntToDouble native-runtime throughput
    [BenchmarkSubjectId("PrimitiveNumericConversionsCoreNativeEntry/PrimitiveNumericConversionsCoreNativeEntry::TestIntToDouble:System.Int32()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_PrimitiveNumericConversionsCore)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_PrimitiveNumericConversionsCoreNativeEntry_PrimitiveNumericConversionsCoreNativeEntry_TestIntToDouble_System_Int32() { _ = PrimitiveNumericConversionsCoreNativeEntry.TestIntToDouble(); }
    // Benchmark PrimitiveNumericConversionsCoreNativeEntry.TestDoubleToInt native-runtime throughput
    [BenchmarkSubjectId("PrimitiveNumericConversionsCoreNativeEntry/PrimitiveNumericConversionsCoreNativeEntry::TestDoubleToInt:System.Int32()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_PrimitiveNumericConversionsCore)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_PrimitiveNumericConversionsCoreNativeEntry_PrimitiveNumericConversionsCoreNativeEntry_TestDoubleToInt_System_Int32() { _ = PrimitiveNumericConversionsCoreNativeEntry.TestDoubleToInt(); }
    // Benchmark PrimitiveNumericConversionsCoreNativeEntry.TestUintToLong native-runtime throughput
    [BenchmarkSubjectId("PrimitiveNumericConversionsCoreNativeEntry/PrimitiveNumericConversionsCoreNativeEntry::TestUintToLong:System.Int32()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_PrimitiveNumericConversionsCore)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_PrimitiveNumericConversionsCoreNativeEntry_PrimitiveNumericConversionsCoreNativeEntry_TestUintToLong_System_Int32() { _ = PrimitiveNumericConversionsCoreNativeEntry.TestUintToLong(); }
    // Benchmark PrimitiveNumericConversionsCoreNativeEntry.TestLongToInt native-runtime throughput
    [BenchmarkSubjectId("PrimitiveNumericConversionsCoreNativeEntry/PrimitiveNumericConversionsCoreNativeEntry::TestLongToInt:System.Int32()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_PrimitiveNumericConversionsCore)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_PrimitiveNumericConversionsCoreNativeEntry_PrimitiveNumericConversionsCoreNativeEntry_TestLongToInt_System_Int32() { _ = PrimitiveNumericConversionsCoreNativeEntry.TestLongToInt(); }}
