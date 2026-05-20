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

// Auto-generated skeleton for Frozen Collections (family/System.Collections.Immutable/frozen/collections).
// Contains test, benchmark, and hotupdate methods merged into one partial class.
// No xunit dependency — [Fact] attributes belong in the test exe project.
public partial class FrozenCollections
{
    // FrozenDictionary.Create<string,int>
    [MethodSubjectId("System.Collections.Immutable/System.Collections.Frozen.FrozenDictionary::Create<string,int>:System.Collections.Frozen.FrozenDictionary<System.String,System.Int32>()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemCollectionsImmutable_FrozenCollections)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Collections_Immutable_System_Collections_Frozen_FrozenDictionary_Create_string_int_System_Collections_Frozen_Froz()
    {
    _ = FrozenDictionary.Create<string,int>();
    }

    [BenchmarkSubjectId("System.Collections.Immutable/System.Collections.Frozen.FrozenDictionary::Create<string,int>:System.Collections.Frozen.FrozenDictionary<System.String,System.Int32>()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemCollectionsImmutable_FrozenCollections)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Collections_Immutable_System_Collections_Frozen_FrozenDictionary_Create_string_int_System_Collections_Frozen_Froz() { _ = FrozenDictionary.Create<string,int>(); }

    [HotUpdateSubjectId("System.Collections.Immutable/System.Collections.Frozen.FrozenDictionary::Create<string,int>:System.Collections.Frozen.FrozenDictionary<System.String,System.Int32>()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemCollectionsImmutable_FrozenCollections)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Collections_Immutable_System_Collections_Frozen_FrozenDictionary_Create_string_int_System_Collections_Frozen_Froz() { _ = FrozenDictionary.Create<string,int>(); }

    // FrozenDictionary.Create<Byte,Byte>
    [MethodSubjectId("System.Collections.Immutable/System.Collections.Frozen.FrozenDictionary::Create<Byte,Byte>:System.Collections.Frozen.FrozenDictionary<System.Byte,System.Byte>()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemCollectionsImmutable_FrozenCollections)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Collections_Immutable_System_Collections_Frozen_FrozenDictionary_Create_Byte_Byte_System_Collections_Frozen_Froze()
    {
    _ = FrozenDictionary.Create<Byte,Byte>();
    }

    [BenchmarkSubjectId("System.Collections.Immutable/System.Collections.Frozen.FrozenDictionary::Create<Byte,Byte>:System.Collections.Frozen.FrozenDictionary<System.Byte,System.Byte>()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemCollectionsImmutable_FrozenCollections)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Collections_Immutable_System_Collections_Frozen_FrozenDictionary_Create_Byte_Byte_System_Collections_Frozen_Froze() { _ = FrozenDictionary.Create<Byte,Byte>(); }

    [HotUpdateSubjectId("System.Collections.Immutable/System.Collections.Frozen.FrozenDictionary::Create<Byte,Byte>:System.Collections.Frozen.FrozenDictionary<System.Byte,System.Byte>()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemCollectionsImmutable_FrozenCollections)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Collections_Immutable_System_Collections_Frozen_FrozenDictionary_Create_Byte_Byte_System_Collections_Frozen_Froze() { _ = FrozenDictionary.Create<Byte,Byte>(); }

    // FrozenDictionary.Create<string,string>
    [MethodSubjectId("System.Collections.Immutable/System.Collections.Frozen.FrozenDictionary::Create<string,string>:System.Collections.Frozen.FrozenDictionary<System.String,System.String>()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemCollectionsImmutable_FrozenCollections)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Collections_Immutable_System_Collections_Frozen_FrozenDictionary_Create_string_string_System_Collections_Frozen_F()
    {
    _ = FrozenDictionary.Create<string,string>();
    }

    [BenchmarkSubjectId("System.Collections.Immutable/System.Collections.Frozen.FrozenDictionary::Create<string,string>:System.Collections.Frozen.FrozenDictionary<System.String,System.String>()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemCollectionsImmutable_FrozenCollections)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Collections_Immutable_System_Collections_Frozen_FrozenDictionary_Create_string_string_System_Collections_Frozen_F() { _ = FrozenDictionary.Create<string,string>(); }

    [HotUpdateSubjectId("System.Collections.Immutable/System.Collections.Frozen.FrozenDictionary::Create<string,string>:System.Collections.Frozen.FrozenDictionary<System.String,System.String>()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemCollectionsImmutable_FrozenCollections)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Collections_Immutable_System_Collections_Frozen_FrozenDictionary_Create_string_string_System_Collections_Frozen_F() { _ = FrozenDictionary.Create<string,string>(); }

    // FrozenSet.Create<Byte>
    [MethodSubjectId("System.Collections.Immutable/System.Collections.Frozen.FrozenSet::Create<Byte>:System.Collections.Frozen.FrozenSet<System.Byte>()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemCollectionsImmutable_FrozenCollections)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Collections_Immutable_System_Collections_Frozen_FrozenSet_Create_Byte_System_Collections_Frozen_FrozenSet_System_()
    {
    _ = FrozenSet.Create<Byte>();
    }

    [BenchmarkSubjectId("System.Collections.Immutable/System.Collections.Frozen.FrozenSet::Create<Byte>:System.Collections.Frozen.FrozenSet<System.Byte>()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemCollectionsImmutable_FrozenCollections)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Collections_Immutable_System_Collections_Frozen_FrozenSet_Create_Byte_System_Collections_Frozen_FrozenSet_System_() { _ = FrozenSet.Create<Byte>(); }

    [HotUpdateSubjectId("System.Collections.Immutable/System.Collections.Frozen.FrozenSet::Create<Byte>:System.Collections.Frozen.FrozenSet<System.Byte>()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemCollectionsImmutable_FrozenCollections)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Collections_Immutable_System_Collections_Frozen_FrozenSet_Create_Byte_System_Collections_Frozen_FrozenSet_System_() { _ = FrozenSet.Create<Byte>(); }

    // FrozenSet.Create<Byte>
    [MethodSubjectId("System.Collections.Immutable/System.Collections.Frozen.FrozenSet::Create<Byte>:System.Collections.Frozen.FrozenSet<System.Byte>(System.Byte)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemCollectionsImmutable_FrozenCollections)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Collections_Immutable_System_Collections_Frozen_FrozenSet_Create_Byte_System_Collections_Frozen_FrozenSet_System_()
    {
    _ = FrozenSet.Create<Byte>((byte)42);
    }

    [BenchmarkSubjectId("System.Collections.Immutable/System.Collections.Frozen.FrozenSet::Create<Byte>:System.Collections.Frozen.FrozenSet<System.Byte>(System.Byte)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemCollectionsImmutable_FrozenCollections)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Collections_Immutable_System_Collections_Frozen_FrozenSet_Create_Byte_System_Collections_Frozen_FrozenSet_System_() { _ = FrozenSet.Create<Byte>((byte)42); }

    [HotUpdateSubjectId("System.Collections.Immutable/System.Collections.Frozen.FrozenSet::Create<Byte>:System.Collections.Frozen.FrozenSet<System.Byte>(System.Byte)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemCollectionsImmutable_FrozenCollections)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Collections_Immutable_System_Collections_Frozen_FrozenSet_Create_Byte_System_Collections_Frozen_FrozenSet_System_() { _ = FrozenSet.Create<Byte>((byte)42); }

    // FrozenSet.Create<Byte>
    [MethodSubjectId("System.Collections.Immutable/System.Collections.Frozen.FrozenSet::Create<Byte>:System.Collections.Frozen.FrozenSet<System.Byte>(System.Byte,System.Byte)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemCollectionsImmutable_FrozenCollections)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Collections_Immutable_System_Collections_Frozen_FrozenSet_Create_Byte_System_Collections_Frozen_FrozenSet_System_()
    {
    _ = FrozenSet.Create<Byte>((byte)42, (byte)42);
    }

    [BenchmarkSubjectId("System.Collections.Immutable/System.Collections.Frozen.FrozenSet::Create<Byte>:System.Collections.Frozen.FrozenSet<System.Byte>(System.Byte,System.Byte)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemCollectionsImmutable_FrozenCollections)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Collections_Immutable_System_Collections_Frozen_FrozenSet_Create_Byte_System_Collections_Frozen_FrozenSet_System_() { _ = FrozenSet.Create<Byte>((byte)42, (byte)42); }

    [HotUpdateSubjectId("System.Collections.Immutable/System.Collections.Frozen.FrozenSet::Create<Byte>:System.Collections.Frozen.FrozenSet<System.Byte>(System.Byte,System.Byte)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemCollectionsImmutable_FrozenCollections)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Collections_Immutable_System_Collections_Frozen_FrozenSet_Create_Byte_System_Collections_Frozen_FrozenSet_System_() { _ = FrozenSet.Create<Byte>((byte)42, (byte)42); }

    // FrozenSet.Create<Int32>
    [MethodSubjectId("System.Collections.Immutable/System.Collections.Frozen.FrozenSet::Create<Int32>:System.Collections.Frozen.FrozenSet<System.Int32>()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemCollectionsImmutable_FrozenCollections)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Collections_Immutable_System_Collections_Frozen_FrozenSet_Create_Int32_System_Collections_Frozen_FrozenSet_System()
    {
    _ = FrozenSet.Create<Int32>();
    }

    [BenchmarkSubjectId("System.Collections.Immutable/System.Collections.Frozen.FrozenSet::Create<Int32>:System.Collections.Frozen.FrozenSet<System.Int32>()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemCollectionsImmutable_FrozenCollections)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Collections_Immutable_System_Collections_Frozen_FrozenSet_Create_Int32_System_Collections_Frozen_FrozenSet_System() { _ = FrozenSet.Create<Int32>(); }

    [HotUpdateSubjectId("System.Collections.Immutable/System.Collections.Frozen.FrozenSet::Create<Int32>:System.Collections.Frozen.FrozenSet<System.Int32>()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemCollectionsImmutable_FrozenCollections)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Collections_Immutable_System_Collections_Frozen_FrozenSet_Create_Int32_System_Collections_Frozen_FrozenSet_System() { _ = FrozenSet.Create<Int32>(); }

    // FrozenSet.Create<Int32>
    [MethodSubjectId("System.Collections.Immutable/System.Collections.Frozen.FrozenSet::Create<Int32>:System.Collections.Frozen.FrozenSet<System.Int32>(System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemCollectionsImmutable_FrozenCollections)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Collections_Immutable_System_Collections_Frozen_FrozenSet_Create_Int32_System_Collections_Frozen_FrozenSet_System()
    {
    _ = FrozenSet.Create<Int32>(42);
    }

    [BenchmarkSubjectId("System.Collections.Immutable/System.Collections.Frozen.FrozenSet::Create<Int32>:System.Collections.Frozen.FrozenSet<System.Int32>(System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemCollectionsImmutable_FrozenCollections)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Collections_Immutable_System_Collections_Frozen_FrozenSet_Create_Int32_System_Collections_Frozen_FrozenSet_System() { _ = FrozenSet.Create<Int32>(42); }

    [HotUpdateSubjectId("System.Collections.Immutable/System.Collections.Frozen.FrozenSet::Create<Int32>:System.Collections.Frozen.FrozenSet<System.Int32>(System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemCollectionsImmutable_FrozenCollections)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Collections_Immutable_System_Collections_Frozen_FrozenSet_Create_Int32_System_Collections_Frozen_FrozenSet_System() { _ = FrozenSet.Create<Int32>(42); }
}
