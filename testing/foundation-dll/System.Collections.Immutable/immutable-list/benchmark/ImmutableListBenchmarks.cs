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

// Benchmark stubs for family/System.Collections.Immutable/immutable/list
public static class ImmutableListBenchmarks
{
    // Benchmark ImmutableList.Create<Byte> native-runtime throughput
    [BenchmarkSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableList::Create<Byte>:System.Collections.Immutable.ImmutableList<System.Byte>()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemCollectionsImmutable_ImmutableList)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Collections_Immutable_System_Collections_Immutable_ImmutableList_Create_Byte_System_Collections_Immutable_Immutab() { _ = ImmutableList.Create<Byte>(); }
    // Benchmark ImmutableList.Create<Byte> native-runtime throughput
    [BenchmarkSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableList::Create<Byte>:System.Collections.Immutable.ImmutableList<System.Byte>(System.Byte)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemCollectionsImmutable_ImmutableList)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Collections_Immutable_System_Collections_Immutable_ImmutableList_Create_Byte_System_Collections_Immutable_Immutab() { _ = ImmutableList.Create<Byte>((byte)42); }
    // Benchmark ImmutableList.Create<Byte> native-runtime throughput
    [BenchmarkSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableList::Create<Byte>:System.Collections.Immutable.ImmutableList<System.Byte>(System.Byte,System.Byte)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemCollectionsImmutable_ImmutableList)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Collections_Immutable_System_Collections_Immutable_ImmutableList_Create_Byte_System_Collections_Immutable_Immutab() { _ = ImmutableList.Create<Byte>((byte)42, (byte)42); }
    // Benchmark ImmutableList.Create<Int32> native-runtime throughput
    [BenchmarkSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableList::Create<Int32>:System.Collections.Immutable.ImmutableList<System.Int32>()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemCollectionsImmutable_ImmutableList)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Collections_Immutable_System_Collections_Immutable_ImmutableList_Create_Int32_System_Collections_Immutable_Immuta() { _ = ImmutableList.Create<Int32>(); }
    // Benchmark ImmutableList.Create<Int32> native-runtime throughput
    [BenchmarkSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableList::Create<Int32>:System.Collections.Immutable.ImmutableList<System.Int32>(System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemCollectionsImmutable_ImmutableList)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Collections_Immutable_System_Collections_Immutable_ImmutableList_Create_Int32_System_Collections_Immutable_Immuta() { _ = ImmutableList.Create<Int32>(42); }
    // Benchmark ImmutableList.Create<String> native-runtime throughput
    [BenchmarkSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableList::Create<String>:System.Collections.Immutable.ImmutableList<System.String>(System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemCollectionsImmutable_ImmutableList)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Collections_Immutable_System_Collections_Immutable_ImmutableList_Create_String_System_Collections_Immutable_Immut() { _ = ImmutableList.Create<String>("hello"); }}
