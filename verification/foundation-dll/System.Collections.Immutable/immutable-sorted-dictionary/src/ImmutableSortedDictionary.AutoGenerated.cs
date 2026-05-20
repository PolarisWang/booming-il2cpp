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

// Auto-generated skeleton for Immutable Sorted Dictionary (family/System.Collections.Immutable/immutable/sorted/dictionary).
// Contains test, benchmark, and hotupdate methods merged into one partial class.
// No xunit dependency — [Fact] attributes belong in the test exe project.
public partial class ImmutableSortedDictionary
{
    // ImmutableSortedDictionary.Create<string,int>
    [MethodSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableSortedDictionary::Create<string,int>:System.Collections.Immutable.ImmutableSortedDictionary<System.String,System.Int32>()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemCollectionsImmutable_ImmutableSortedDictionary)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Collections_Immutable_System_Collections_Immutable_ImmutableSortedDictionary_Create_string_int_System_Collections()
    {
    _ = ImmutableSortedDictionary.Create<string,int>();
    }

    [BenchmarkSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableSortedDictionary::Create<string,int>:System.Collections.Immutable.ImmutableSortedDictionary<System.String,System.Int32>()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemCollectionsImmutable_ImmutableSortedDictionary)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Collections_Immutable_System_Collections_Immutable_ImmutableSortedDictionary_Create_string_int_System_Collections() { _ = ImmutableSortedDictionary.Create<string,int>(); }

    [HotUpdateSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableSortedDictionary::Create<string,int>:System.Collections.Immutable.ImmutableSortedDictionary<System.String,System.Int32>()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemCollectionsImmutable_ImmutableSortedDictionary)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Collections_Immutable_System_Collections_Immutable_ImmutableSortedDictionary_Create_string_int_System_Collections() { _ = ImmutableSortedDictionary.Create<string,int>(); }

    // ImmutableSortedDictionary.Create<Byte,Byte>
    [MethodSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableSortedDictionary::Create<Byte,Byte>:System.Collections.Immutable.ImmutableSortedDictionary<System.Byte,System.Byte>()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemCollectionsImmutable_ImmutableSortedDictionary)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Collections_Immutable_System_Collections_Immutable_ImmutableSortedDictionary_Create_Byte_Byte_System_Collections_()
    {
    _ = ImmutableSortedDictionary.Create<Byte,Byte>();
    }

    [BenchmarkSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableSortedDictionary::Create<Byte,Byte>:System.Collections.Immutable.ImmutableSortedDictionary<System.Byte,System.Byte>()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemCollectionsImmutable_ImmutableSortedDictionary)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Collections_Immutable_System_Collections_Immutable_ImmutableSortedDictionary_Create_Byte_Byte_System_Collections_() { _ = ImmutableSortedDictionary.Create<Byte,Byte>(); }

    [HotUpdateSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableSortedDictionary::Create<Byte,Byte>:System.Collections.Immutable.ImmutableSortedDictionary<System.Byte,System.Byte>()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemCollectionsImmutable_ImmutableSortedDictionary)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Collections_Immutable_System_Collections_Immutable_ImmutableSortedDictionary_Create_Byte_Byte_System_Collections_() { _ = ImmutableSortedDictionary.Create<Byte,Byte>(); }
}
