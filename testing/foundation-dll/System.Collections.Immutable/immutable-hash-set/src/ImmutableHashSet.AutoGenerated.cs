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

// Auto-generated skeleton for Immutable Hash Set (family/System.Collections.Immutable/immutable/hash/set).
// Contains test, benchmark, and hotupdate methods merged into one partial class.
// No xunit dependency — [Fact] attributes belong in the test exe project.
public partial class ImmutableHashSet
{
    // ImmutableHashSet.Create<Byte>
    [MethodSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableHashSet::Create<Byte>:System.Collections.Immutable.ImmutableHashSet<System.Byte>()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemCollectionsImmutable_ImmutableHashSet)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Collections_Immutable_System_Collections_Immutable_ImmutableHashSet_Create_Byte_System_Collections_Immutable_Immu()
    {
    _ = ImmutableHashSet.Create<Byte>();
    }

    [BenchmarkSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableHashSet::Create<Byte>:System.Collections.Immutable.ImmutableHashSet<System.Byte>()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemCollectionsImmutable_ImmutableHashSet)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Collections_Immutable_System_Collections_Immutable_ImmutableHashSet_Create_Byte_System_Collections_Immutable_Immu() { _ = ImmutableHashSet.Create<Byte>(); }

    [HotUpdateSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableHashSet::Create<Byte>:System.Collections.Immutable.ImmutableHashSet<System.Byte>()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemCollectionsImmutable_ImmutableHashSet)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Collections_Immutable_System_Collections_Immutable_ImmutableHashSet_Create_Byte_System_Collections_Immutable_Immu() { _ = ImmutableHashSet.Create<Byte>(); }

    // ImmutableHashSet.Create<Byte>
    [MethodSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableHashSet::Create<Byte>:System.Collections.Immutable.ImmutableHashSet<System.Byte>(System.Byte)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemCollectionsImmutable_ImmutableHashSet)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Collections_Immutable_System_Collections_Immutable_ImmutableHashSet_Create_Byte_System_Collections_Immutable_Immu()
    {
    _ = ImmutableHashSet.Create<Byte>((byte)42);
    }

    [BenchmarkSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableHashSet::Create<Byte>:System.Collections.Immutable.ImmutableHashSet<System.Byte>(System.Byte)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemCollectionsImmutable_ImmutableHashSet)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Collections_Immutable_System_Collections_Immutable_ImmutableHashSet_Create_Byte_System_Collections_Immutable_Immu() { _ = ImmutableHashSet.Create<Byte>((byte)42); }

    [HotUpdateSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableHashSet::Create<Byte>:System.Collections.Immutable.ImmutableHashSet<System.Byte>(System.Byte)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemCollectionsImmutable_ImmutableHashSet)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Collections_Immutable_System_Collections_Immutable_ImmutableHashSet_Create_Byte_System_Collections_Immutable_Immu() { _ = ImmutableHashSet.Create<Byte>((byte)42); }

    // ImmutableHashSet.Create<Byte>
    [MethodSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableHashSet::Create<Byte>:System.Collections.Immutable.ImmutableHashSet<System.Byte>(System.Byte,System.Byte)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemCollectionsImmutable_ImmutableHashSet)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Collections_Immutable_System_Collections_Immutable_ImmutableHashSet_Create_Byte_System_Collections_Immutable_Immu()
    {
    _ = ImmutableHashSet.Create<Byte>((byte)42, (byte)42);
    }

    [BenchmarkSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableHashSet::Create<Byte>:System.Collections.Immutable.ImmutableHashSet<System.Byte>(System.Byte,System.Byte)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemCollectionsImmutable_ImmutableHashSet)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Collections_Immutable_System_Collections_Immutable_ImmutableHashSet_Create_Byte_System_Collections_Immutable_Immu() { _ = ImmutableHashSet.Create<Byte>((byte)42, (byte)42); }

    [HotUpdateSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableHashSet::Create<Byte>:System.Collections.Immutable.ImmutableHashSet<System.Byte>(System.Byte,System.Byte)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemCollectionsImmutable_ImmutableHashSet)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Collections_Immutable_System_Collections_Immutable_ImmutableHashSet_Create_Byte_System_Collections_Immutable_Immu() { _ = ImmutableHashSet.Create<Byte>((byte)42, (byte)42); }

    // ImmutableHashSet.Create<Int32>
    [MethodSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableHashSet::Create<Int32>:System.Collections.Immutable.ImmutableHashSet<System.Int32>()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemCollectionsImmutable_ImmutableHashSet)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Collections_Immutable_System_Collections_Immutable_ImmutableHashSet_Create_Int32_System_Collections_Immutable_Imm()
    {
    _ = ImmutableHashSet.Create<Int32>();
    }

    [BenchmarkSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableHashSet::Create<Int32>:System.Collections.Immutable.ImmutableHashSet<System.Int32>()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemCollectionsImmutable_ImmutableHashSet)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Collections_Immutable_System_Collections_Immutable_ImmutableHashSet_Create_Int32_System_Collections_Immutable_Imm() { _ = ImmutableHashSet.Create<Int32>(); }

    [HotUpdateSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableHashSet::Create<Int32>:System.Collections.Immutable.ImmutableHashSet<System.Int32>()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemCollectionsImmutable_ImmutableHashSet)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Collections_Immutable_System_Collections_Immutable_ImmutableHashSet_Create_Int32_System_Collections_Immutable_Imm() { _ = ImmutableHashSet.Create<Int32>(); }

    // ImmutableHashSet.Create<Int32>
    [MethodSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableHashSet::Create<Int32>:System.Collections.Immutable.ImmutableHashSet<System.Int32>(System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemCollectionsImmutable_ImmutableHashSet)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Collections_Immutable_System_Collections_Immutable_ImmutableHashSet_Create_Int32_System_Collections_Immutable_Imm()
    {
    _ = ImmutableHashSet.Create<Int32>(42);
    }

    [BenchmarkSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableHashSet::Create<Int32>:System.Collections.Immutable.ImmutableHashSet<System.Int32>(System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemCollectionsImmutable_ImmutableHashSet)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Collections_Immutable_System_Collections_Immutable_ImmutableHashSet_Create_Int32_System_Collections_Immutable_Imm() { _ = ImmutableHashSet.Create<Int32>(42); }

    [HotUpdateSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableHashSet::Create<Int32>:System.Collections.Immutable.ImmutableHashSet<System.Int32>(System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemCollectionsImmutable_ImmutableHashSet)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Collections_Immutable_System_Collections_Immutable_ImmutableHashSet_Create_Int32_System_Collections_Immutable_Imm() { _ = ImmutableHashSet.Create<Int32>(42); }
}
