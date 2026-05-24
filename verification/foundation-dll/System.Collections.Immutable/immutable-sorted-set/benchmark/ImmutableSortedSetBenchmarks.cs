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

// Benchmark stubs for family/System.Collections.Immutable/immutable/sorted/set
public static class ImmutableSortedSetBenchmarks
{
    // Benchmark ImmutableSortedSet.Create<Byte> native-runtime throughput
    [BenchmarkSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableSortedSet::Create<Byte>:System.Collections.Immutable.ImmutableSortedSet<System.Byte>()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemCollectionsImmutable_ImmutableSortedSet)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Collections_Immutable_System_Collections_Immutable_ImmutableSortedSet_Create_Byte_System_Collections_Immutable_Im() { _ = ImmutableSortedSet.Create<Byte>(); }
    // Benchmark ImmutableSortedSet.Create<Byte> native-runtime throughput
    [BenchmarkSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableSortedSet::Create<Byte>:System.Collections.Immutable.ImmutableSortedSet<System.Byte>(System.Byte)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemCollectionsImmutable_ImmutableSortedSet)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Collections_Immutable_System_Collections_Immutable_ImmutableSortedSet_Create_Byte_System_Collections_Immutable_Im() { _ = ImmutableSortedSet.Create<Byte>((byte)42); }
    // Benchmark ImmutableSortedSet.Create<Byte> native-runtime throughput
    [BenchmarkSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableSortedSet::Create<Byte>:System.Collections.Immutable.ImmutableSortedSet<System.Byte>(System.Byte,System.Byte)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemCollectionsImmutable_ImmutableSortedSet)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Collections_Immutable_System_Collections_Immutable_ImmutableSortedSet_Create_Byte_System_Collections_Immutable_Im() { _ = ImmutableSortedSet.Create<Byte>((byte)42, (byte)42); }
    // Benchmark ImmutableSortedSet.Create<Int32> native-runtime throughput
    [BenchmarkSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableSortedSet::Create<Int32>:System.Collections.Immutable.ImmutableSortedSet<System.Int32>()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemCollectionsImmutable_ImmutableSortedSet)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Collections_Immutable_System_Collections_Immutable_ImmutableSortedSet_Create_Int32_System_Collections_Immutable_I() { _ = ImmutableSortedSet.Create<Int32>(); }
    // Benchmark ImmutableSortedSet.Create<Int32> native-runtime throughput
    [BenchmarkSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableSortedSet::Create<Int32>:System.Collections.Immutable.ImmutableSortedSet<System.Int32>(System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemCollectionsImmutable_ImmutableSortedSet)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Collections_Immutable_System_Collections_Immutable_ImmutableSortedSet_Create_Int32_System_Collections_Immutable_I() { _ = ImmutableSortedSet.Create<Int32>(42); }}
