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

// Benchmark stubs for family/System.Collections.Immutable/immutable/array
public static class ImmutableArrayBenchmarks
{
    // Benchmark ImmutableArray.Create<Byte> native-runtime throughput
    [BenchmarkSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableArray::Create<Byte>:System.Collections.Immutable.ImmutableArray<System.Byte>()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemCollectionsImmutable_ImmutableArray)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Collections_Immutable_System_Collections_Immutable_ImmutableArray_Create_Byte_System_Collections_Immutable_Immuta() { _ = ImmutableArray.Create<Byte>(); }
    // Benchmark ImmutableArray.Create<Byte> native-runtime throughput
    [BenchmarkSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableArray::Create<Byte>:System.Collections.Immutable.ImmutableArray<System.Byte>(System.Byte)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemCollectionsImmutable_ImmutableArray)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Collections_Immutable_System_Collections_Immutable_ImmutableArray_Create_Byte_System_Collections_Immutable_Immuta() { _ = ImmutableArray.Create<Byte>((byte)42); }
    // Benchmark ImmutableArray.Create<Byte> native-runtime throughput
    [BenchmarkSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableArray::Create<Byte>:System.Collections.Immutable.ImmutableArray<System.Byte>(System.Byte,System.Byte)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemCollectionsImmutable_ImmutableArray)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Collections_Immutable_System_Collections_Immutable_ImmutableArray_Create_Byte_System_Collections_Immutable_Immuta() { _ = ImmutableArray.Create<Byte>((byte)42, (byte)42); }
    // Benchmark ImmutableArray.Create<Int32> native-runtime throughput
    [BenchmarkSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableArray::Create<Int32>:System.Collections.Immutable.ImmutableArray<System.Int32>()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemCollectionsImmutable_ImmutableArray)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Collections_Immutable_System_Collections_Immutable_ImmutableArray_Create_Int32_System_Collections_Immutable_Immut() { _ = ImmutableArray.Create<Int32>(); }
    // Benchmark ImmutableArray.Create<Int32> native-runtime throughput
    [BenchmarkSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableArray::Create<Int32>:System.Collections.Immutable.ImmutableArray<System.Int32>(System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemCollectionsImmutable_ImmutableArray)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Collections_Immutable_System_Collections_Immutable_ImmutableArray_Create_Int32_System_Collections_Immutable_Immut() { _ = ImmutableArray.Create<Int32>(42); }
    // Benchmark ImmutableArray.Create<String> native-runtime throughput
    [BenchmarkSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableArray::Create<String>:System.Collections.Immutable.ImmutableArray<System.String>(System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemCollectionsImmutable_ImmutableArray)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Collections_Immutable_System_Collections_Immutable_ImmutableArray_Create_String_System_Collections_Immutable_Immu() { _ = ImmutableArray.Create<String>("hello"); }}
