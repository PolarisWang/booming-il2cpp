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

// Benchmark stubs for family/System.Collections.Immutable/frozen/collections
public static class FrozenCollectionsBenchmarks
{
    // Benchmark FrozenDictionary.Create<string,int> native-runtime throughput
    [BenchmarkSubjectId("System.Collections.Immutable/System.Collections.Frozen.FrozenDictionary::Create<string,int>:System.Collections.Frozen.FrozenDictionary<System.String,System.Int32>()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemCollectionsImmutable_FrozenCollections)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Collections_Immutable_System_Collections_Frozen_FrozenDictionary_Create_string_int_System_Collections_Frozen_Froz() { _ = FrozenDictionary.Create<string,int>(); }
    // Benchmark FrozenDictionary.Create<Byte,Byte> native-runtime throughput
    [BenchmarkSubjectId("System.Collections.Immutable/System.Collections.Frozen.FrozenDictionary::Create<Byte,Byte>:System.Collections.Frozen.FrozenDictionary<System.Byte,System.Byte>()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemCollectionsImmutable_FrozenCollections)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Collections_Immutable_System_Collections_Frozen_FrozenDictionary_Create_Byte_Byte_System_Collections_Frozen_Froze() { _ = FrozenDictionary.Create<Byte,Byte>(); }}
