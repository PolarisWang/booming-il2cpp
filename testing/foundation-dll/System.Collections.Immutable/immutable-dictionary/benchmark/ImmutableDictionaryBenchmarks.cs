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

// Benchmark stubs for family/System.Collections.Immutable/immutable/dictionary
public static class ImmutableDictionaryBenchmarks
{
    // Benchmark ImmutableDictionary.Create<string,int> native-runtime throughput
    [BenchmarkSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableDictionary::Create<string,int>:System.Collections.Immutable.ImmutableDictionary<System.String,System.Int32>()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemCollectionsImmutable_ImmutableDictionary)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Collections_Immutable_System_Collections_Immutable_ImmutableDictionary_Create_string_int_System_Collections_Immut() { _ = ImmutableDictionary.Create<string,int>(); }
    // Benchmark ImmutableDictionary.Create<Byte,Byte> native-runtime throughput
    [BenchmarkSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableDictionary::Create<Byte,Byte>:System.Collections.Immutable.ImmutableDictionary<System.Byte,System.Byte>()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemCollectionsImmutable_ImmutableDictionary)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Collections_Immutable_System_Collections_Immutable_ImmutableDictionary_Create_Byte_Byte_System_Collections_Immuta() { _ = ImmutableDictionary.Create<Byte,Byte>(); }
    // Benchmark ImmutableDictionary.Create<string,string> native-runtime throughput
    [BenchmarkSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableDictionary::Create<string,string>:System.Collections.Immutable.ImmutableDictionary<System.String,System.String>()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemCollectionsImmutable_ImmutableDictionary)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Collections_Immutable_System_Collections_Immutable_ImmutableDictionary_Create_string_string_System_Collections_Im() { _ = ImmutableDictionary.Create<string,string>(); }}
