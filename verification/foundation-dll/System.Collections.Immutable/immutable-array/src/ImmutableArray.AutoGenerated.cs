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

// Auto-generated skeleton for Immutable Array (family/System.Collections.Immutable/immutable/array).
// Contains test, benchmark, and hotupdate methods merged into one partial class.
// No xunit dependency — [Fact] attributes belong in the test exe project.
public partial class ImmutableArray
{
    // ImmutableArray.Create<Byte>
    [MethodSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableArray::Create<Byte>:System.Collections.Immutable.ImmutableArray<System.Byte>()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemCollectionsImmutable_ImmutableArray)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Collections_Immutable_System_Collections_Immutable_ImmutableArray_Create_Byte_System_Collections_Immutable_Immuta()
    {
    _ = ImmutableArray.Create<Byte>();
    }

    [BenchmarkSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableArray::Create<Byte>:System.Collections.Immutable.ImmutableArray<System.Byte>()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemCollectionsImmutable_ImmutableArray)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Collections_Immutable_System_Collections_Immutable_ImmutableArray_Create_Byte_System_Collections_Immutable_Immuta() { _ = ImmutableArray.Create<Byte>(); }

    [HotUpdateSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableArray::Create<Byte>:System.Collections.Immutable.ImmutableArray<System.Byte>()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemCollectionsImmutable_ImmutableArray)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Collections_Immutable_System_Collections_Immutable_ImmutableArray_Create_Byte_System_Collections_Immutable_Immuta() { _ = ImmutableArray.Create<Byte>(); }

    // ImmutableArray.Create<Byte>
    [MethodSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableArray::Create<Byte>:System.Collections.Immutable.ImmutableArray<System.Byte>(System.Byte)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemCollectionsImmutable_ImmutableArray)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Collections_Immutable_System_Collections_Immutable_ImmutableArray_Create_Byte_System_Collections_Immutable_Immuta()
    {
    _ = ImmutableArray.Create<Byte>((byte)42);
    }

    [BenchmarkSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableArray::Create<Byte>:System.Collections.Immutable.ImmutableArray<System.Byte>(System.Byte)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemCollectionsImmutable_ImmutableArray)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Collections_Immutable_System_Collections_Immutable_ImmutableArray_Create_Byte_System_Collections_Immutable_Immuta() { _ = ImmutableArray.Create<Byte>((byte)42); }

    [HotUpdateSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableArray::Create<Byte>:System.Collections.Immutable.ImmutableArray<System.Byte>(System.Byte)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemCollectionsImmutable_ImmutableArray)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Collections_Immutable_System_Collections_Immutable_ImmutableArray_Create_Byte_System_Collections_Immutable_Immuta() { _ = ImmutableArray.Create<Byte>((byte)42); }

    // ImmutableArray.Create<Byte>
    [MethodSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableArray::Create<Byte>:System.Collections.Immutable.ImmutableArray<System.Byte>(System.Byte,System.Byte)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemCollectionsImmutable_ImmutableArray)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Collections_Immutable_System_Collections_Immutable_ImmutableArray_Create_Byte_System_Collections_Immutable_Immuta()
    {
    _ = ImmutableArray.Create<Byte>((byte)42, (byte)42);
    }

    [BenchmarkSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableArray::Create<Byte>:System.Collections.Immutable.ImmutableArray<System.Byte>(System.Byte,System.Byte)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemCollectionsImmutable_ImmutableArray)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Collections_Immutable_System_Collections_Immutable_ImmutableArray_Create_Byte_System_Collections_Immutable_Immuta() { _ = ImmutableArray.Create<Byte>((byte)42, (byte)42); }

    [HotUpdateSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableArray::Create<Byte>:System.Collections.Immutable.ImmutableArray<System.Byte>(System.Byte,System.Byte)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemCollectionsImmutable_ImmutableArray)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Collections_Immutable_System_Collections_Immutable_ImmutableArray_Create_Byte_System_Collections_Immutable_Immuta() { _ = ImmutableArray.Create<Byte>((byte)42, (byte)42); }

    // ImmutableArray.Create<Int32>
    [MethodSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableArray::Create<Int32>:System.Collections.Immutable.ImmutableArray<System.Int32>()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemCollectionsImmutable_ImmutableArray)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Collections_Immutable_System_Collections_Immutable_ImmutableArray_Create_Int32_System_Collections_Immutable_Immut()
    {
    _ = ImmutableArray.Create<Int32>();
    }

    [BenchmarkSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableArray::Create<Int32>:System.Collections.Immutable.ImmutableArray<System.Int32>()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemCollectionsImmutable_ImmutableArray)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Collections_Immutable_System_Collections_Immutable_ImmutableArray_Create_Int32_System_Collections_Immutable_Immut() { _ = ImmutableArray.Create<Int32>(); }

    [HotUpdateSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableArray::Create<Int32>:System.Collections.Immutable.ImmutableArray<System.Int32>()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemCollectionsImmutable_ImmutableArray)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Collections_Immutable_System_Collections_Immutable_ImmutableArray_Create_Int32_System_Collections_Immutable_Immut() { _ = ImmutableArray.Create<Int32>(); }

    // ImmutableArray.Create<Int32>
    [MethodSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableArray::Create<Int32>:System.Collections.Immutable.ImmutableArray<System.Int32>(System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemCollectionsImmutable_ImmutableArray)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Collections_Immutable_System_Collections_Immutable_ImmutableArray_Create_Int32_System_Collections_Immutable_Immut()
    {
    _ = ImmutableArray.Create<Int32>(42);
    }

    [BenchmarkSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableArray::Create<Int32>:System.Collections.Immutable.ImmutableArray<System.Int32>(System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemCollectionsImmutable_ImmutableArray)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Collections_Immutable_System_Collections_Immutable_ImmutableArray_Create_Int32_System_Collections_Immutable_Immut() { _ = ImmutableArray.Create<Int32>(42); }

    [HotUpdateSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableArray::Create<Int32>:System.Collections.Immutable.ImmutableArray<System.Int32>(System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemCollectionsImmutable_ImmutableArray)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Collections_Immutable_System_Collections_Immutable_ImmutableArray_Create_Int32_System_Collections_Immutable_Immut() { _ = ImmutableArray.Create<Int32>(42); }

    // ImmutableArray.Create<String>
    [MethodSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableArray::Create<String>:System.Collections.Immutable.ImmutableArray<System.String>(System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemCollectionsImmutable_ImmutableArray)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Collections_Immutable_System_Collections_Immutable_ImmutableArray_Create_String_System_Collections_Immutable_Immu()
    {
    _ = ImmutableArray.Create<String>("hello");
    }

    [BenchmarkSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableArray::Create<String>:System.Collections.Immutable.ImmutableArray<System.String>(System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemCollectionsImmutable_ImmutableArray)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Collections_Immutable_System_Collections_Immutable_ImmutableArray_Create_String_System_Collections_Immutable_Immu() { _ = ImmutableArray.Create<String>("hello"); }

    [HotUpdateSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableArray::Create<String>:System.Collections.Immutable.ImmutableArray<System.String>(System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemCollectionsImmutable_ImmutableArray)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Collections_Immutable_System_Collections_Immutable_ImmutableArray_Create_String_System_Collections_Immutable_Immu() { _ = ImmutableArray.Create<String>("hello"); }
}
