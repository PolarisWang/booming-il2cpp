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

// Benchmark stubs for family/System.Collections.Immutable/immutable/interlocked
public static class ImmutableInterlockedBenchmarks
{
    // Benchmark ImmutableInterlocked.Update<string> native-runtime throughput
    [BenchmarkSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableInterlocked::Update<string>(System.String&,System.Func{System.String,System.String})")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemCollectionsImmutable_ImmutableInterlocked)]
    public static void Benchmark_System_Collections_Immutable_System_Collections_Immutable_ImmutableInterlocked_Update_string_System_String_System_Func_S() { }}
