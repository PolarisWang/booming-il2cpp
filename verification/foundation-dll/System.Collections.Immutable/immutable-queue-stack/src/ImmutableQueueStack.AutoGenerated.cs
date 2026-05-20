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

// Auto-generated skeleton for Immutable Queue and Stack (family/System.Collections.Immutable/immutable/queue/stack).
// Contains test, benchmark, and hotupdate methods merged into one partial class.
// No xunit dependency — [Fact] attributes belong in the test exe project.
public partial class ImmutableQueueStack
{
    // ImmutableQueue.Create<Byte>
    [MethodSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableQueue::Create<Byte>:System.Collections.Immutable.ImmutableQueue<System.Byte>()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemCollectionsImmutable_ImmutableQueueStack)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Collections_Immutable_System_Collections_Immutable_ImmutableQueue_Create_Byte_System_Collections_Immutable_Immuta()
    {
    _ = ImmutableQueue.Create<Byte>();
    }

    [BenchmarkSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableQueue::Create<Byte>:System.Collections.Immutable.ImmutableQueue<System.Byte>()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemCollectionsImmutable_ImmutableQueueStack)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Collections_Immutable_System_Collections_Immutable_ImmutableQueue_Create_Byte_System_Collections_Immutable_Immuta() { _ = ImmutableQueue.Create<Byte>(); }

    [HotUpdateSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableQueue::Create<Byte>:System.Collections.Immutable.ImmutableQueue<System.Byte>()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemCollectionsImmutable_ImmutableQueueStack)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Collections_Immutable_System_Collections_Immutable_ImmutableQueue_Create_Byte_System_Collections_Immutable_Immuta() { _ = ImmutableQueue.Create<Byte>(); }

    // ImmutableQueue.Create<Byte>
    [MethodSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableQueue::Create<Byte>:System.Collections.Immutable.ImmutableQueue<System.Byte>(System.Byte)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemCollectionsImmutable_ImmutableQueueStack)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Collections_Immutable_System_Collections_Immutable_ImmutableQueue_Create_Byte_System_Collections_Immutable_Immuta()
    {
    _ = ImmutableQueue.Create<Byte>((byte)42);
    }

    [BenchmarkSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableQueue::Create<Byte>:System.Collections.Immutable.ImmutableQueue<System.Byte>(System.Byte)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemCollectionsImmutable_ImmutableQueueStack)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Collections_Immutable_System_Collections_Immutable_ImmutableQueue_Create_Byte_System_Collections_Immutable_Immuta() { _ = ImmutableQueue.Create<Byte>((byte)42); }

    [HotUpdateSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableQueue::Create<Byte>:System.Collections.Immutable.ImmutableQueue<System.Byte>(System.Byte)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemCollectionsImmutable_ImmutableQueueStack)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Collections_Immutable_System_Collections_Immutable_ImmutableQueue_Create_Byte_System_Collections_Immutable_Immuta() { _ = ImmutableQueue.Create<Byte>((byte)42); }

    // ImmutableQueue.Create<Byte>
    [MethodSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableQueue::Create<Byte>:System.Collections.Immutable.ImmutableQueue<System.Byte>(System.Byte,System.Byte)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemCollectionsImmutable_ImmutableQueueStack)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Collections_Immutable_System_Collections_Immutable_ImmutableQueue_Create_Byte_System_Collections_Immutable_Immuta()
    {
    _ = ImmutableQueue.Create<Byte>((byte)42, (byte)42);
    }

    [BenchmarkSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableQueue::Create<Byte>:System.Collections.Immutable.ImmutableQueue<System.Byte>(System.Byte,System.Byte)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemCollectionsImmutable_ImmutableQueueStack)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Collections_Immutable_System_Collections_Immutable_ImmutableQueue_Create_Byte_System_Collections_Immutable_Immuta() { _ = ImmutableQueue.Create<Byte>((byte)42, (byte)42); }

    [HotUpdateSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableQueue::Create<Byte>:System.Collections.Immutable.ImmutableQueue<System.Byte>(System.Byte,System.Byte)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemCollectionsImmutable_ImmutableQueueStack)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Collections_Immutable_System_Collections_Immutable_ImmutableQueue_Create_Byte_System_Collections_Immutable_Immuta() { _ = ImmutableQueue.Create<Byte>((byte)42, (byte)42); }

    // ImmutableQueue.Create<Int32>
    [MethodSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableQueue::Create<Int32>:System.Collections.Immutable.ImmutableQueue<System.Int32>()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemCollectionsImmutable_ImmutableQueueStack)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Collections_Immutable_System_Collections_Immutable_ImmutableQueue_Create_Int32_System_Collections_Immutable_Immut()
    {
    _ = ImmutableQueue.Create<Int32>();
    }

    [BenchmarkSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableQueue::Create<Int32>:System.Collections.Immutable.ImmutableQueue<System.Int32>()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemCollectionsImmutable_ImmutableQueueStack)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Collections_Immutable_System_Collections_Immutable_ImmutableQueue_Create_Int32_System_Collections_Immutable_Immut() { _ = ImmutableQueue.Create<Int32>(); }

    [HotUpdateSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableQueue::Create<Int32>:System.Collections.Immutable.ImmutableQueue<System.Int32>()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemCollectionsImmutable_ImmutableQueueStack)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Collections_Immutable_System_Collections_Immutable_ImmutableQueue_Create_Int32_System_Collections_Immutable_Immut() { _ = ImmutableQueue.Create<Int32>(); }

    // ImmutableQueue.Create<Int32>
    [MethodSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableQueue::Create<Int32>:System.Collections.Immutable.ImmutableQueue<System.Int32>(System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemCollectionsImmutable_ImmutableQueueStack)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Collections_Immutable_System_Collections_Immutable_ImmutableQueue_Create_Int32_System_Collections_Immutable_Immut()
    {
    _ = ImmutableQueue.Create<Int32>(42);
    }

    [BenchmarkSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableQueue::Create<Int32>:System.Collections.Immutable.ImmutableQueue<System.Int32>(System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemCollectionsImmutable_ImmutableQueueStack)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Collections_Immutable_System_Collections_Immutable_ImmutableQueue_Create_Int32_System_Collections_Immutable_Immut() { _ = ImmutableQueue.Create<Int32>(42); }

    [HotUpdateSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableQueue::Create<Int32>:System.Collections.Immutable.ImmutableQueue<System.Int32>(System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemCollectionsImmutable_ImmutableQueueStack)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Collections_Immutable_System_Collections_Immutable_ImmutableQueue_Create_Int32_System_Collections_Immutable_Immut() { _ = ImmutableQueue.Create<Int32>(42); }

    // ImmutableStack.Create<Byte>
    [MethodSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableStack::Create<Byte>:System.Collections.Immutable.ImmutableStack<System.Byte>()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemCollectionsImmutable_ImmutableQueueStack)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Collections_Immutable_System_Collections_Immutable_ImmutableStack_Create_Byte_System_Collections_Immutable_Immuta()
    {
    _ = ImmutableStack.Create<Byte>();
    }

    [BenchmarkSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableStack::Create<Byte>:System.Collections.Immutable.ImmutableStack<System.Byte>()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemCollectionsImmutable_ImmutableQueueStack)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Collections_Immutable_System_Collections_Immutable_ImmutableStack_Create_Byte_System_Collections_Immutable_Immuta() { _ = ImmutableStack.Create<Byte>(); }

    [HotUpdateSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableStack::Create<Byte>:System.Collections.Immutable.ImmutableStack<System.Byte>()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemCollectionsImmutable_ImmutableQueueStack)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Collections_Immutable_System_Collections_Immutable_ImmutableStack_Create_Byte_System_Collections_Immutable_Immuta() { _ = ImmutableStack.Create<Byte>(); }

    // ImmutableStack.Create<Byte>
    [MethodSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableStack::Create<Byte>:System.Collections.Immutable.ImmutableStack<System.Byte>(System.Byte)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemCollectionsImmutable_ImmutableQueueStack)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Collections_Immutable_System_Collections_Immutable_ImmutableStack_Create_Byte_System_Collections_Immutable_Immuta()
    {
    _ = ImmutableStack.Create<Byte>((byte)42);
    }

    [BenchmarkSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableStack::Create<Byte>:System.Collections.Immutable.ImmutableStack<System.Byte>(System.Byte)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemCollectionsImmutable_ImmutableQueueStack)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Collections_Immutable_System_Collections_Immutable_ImmutableStack_Create_Byte_System_Collections_Immutable_Immuta() { _ = ImmutableStack.Create<Byte>((byte)42); }

    [HotUpdateSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableStack::Create<Byte>:System.Collections.Immutable.ImmutableStack<System.Byte>(System.Byte)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemCollectionsImmutable_ImmutableQueueStack)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Collections_Immutable_System_Collections_Immutable_ImmutableStack_Create_Byte_System_Collections_Immutable_Immuta() { _ = ImmutableStack.Create<Byte>((byte)42); }

    // ImmutableStack.Create<Byte>
    [MethodSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableStack::Create<Byte>:System.Collections.Immutable.ImmutableStack<System.Byte>(System.Byte,System.Byte)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemCollectionsImmutable_ImmutableQueueStack)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Collections_Immutable_System_Collections_Immutable_ImmutableStack_Create_Byte_System_Collections_Immutable_Immuta()
    {
    _ = ImmutableStack.Create<Byte>((byte)42, (byte)42);
    }

    [BenchmarkSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableStack::Create<Byte>:System.Collections.Immutable.ImmutableStack<System.Byte>(System.Byte,System.Byte)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemCollectionsImmutable_ImmutableQueueStack)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Collections_Immutable_System_Collections_Immutable_ImmutableStack_Create_Byte_System_Collections_Immutable_Immuta() { _ = ImmutableStack.Create<Byte>((byte)42, (byte)42); }

    [HotUpdateSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableStack::Create<Byte>:System.Collections.Immutable.ImmutableStack<System.Byte>(System.Byte,System.Byte)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemCollectionsImmutable_ImmutableQueueStack)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Collections_Immutable_System_Collections_Immutable_ImmutableStack_Create_Byte_System_Collections_Immutable_Immuta() { _ = ImmutableStack.Create<Byte>((byte)42, (byte)42); }

    // ImmutableStack.Create<Int32>
    [MethodSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableStack::Create<Int32>:System.Collections.Immutable.ImmutableStack<System.Int32>()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemCollectionsImmutable_ImmutableQueueStack)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Collections_Immutable_System_Collections_Immutable_ImmutableStack_Create_Int32_System_Collections_Immutable_Immut()
    {
    _ = ImmutableStack.Create<Int32>();
    }

    [BenchmarkSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableStack::Create<Int32>:System.Collections.Immutable.ImmutableStack<System.Int32>()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemCollectionsImmutable_ImmutableQueueStack)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Collections_Immutable_System_Collections_Immutable_ImmutableStack_Create_Int32_System_Collections_Immutable_Immut() { _ = ImmutableStack.Create<Int32>(); }

    [HotUpdateSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableStack::Create<Int32>:System.Collections.Immutable.ImmutableStack<System.Int32>()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemCollectionsImmutable_ImmutableQueueStack)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Collections_Immutable_System_Collections_Immutable_ImmutableStack_Create_Int32_System_Collections_Immutable_Immut() { _ = ImmutableStack.Create<Int32>(); }

    // ImmutableStack.Create<Int32>
    [MethodSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableStack::Create<Int32>:System.Collections.Immutable.ImmutableStack<System.Int32>(System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemCollectionsImmutable_ImmutableQueueStack)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Collections_Immutable_System_Collections_Immutable_ImmutableStack_Create_Int32_System_Collections_Immutable_Immut()
    {
    _ = ImmutableStack.Create<Int32>(42);
    }

    [BenchmarkSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableStack::Create<Int32>:System.Collections.Immutable.ImmutableStack<System.Int32>(System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemCollectionsImmutable_ImmutableQueueStack)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Collections_Immutable_System_Collections_Immutable_ImmutableStack_Create_Int32_System_Collections_Immutable_Immut() { _ = ImmutableStack.Create<Int32>(42); }

    [HotUpdateSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableStack::Create<Int32>:System.Collections.Immutable.ImmutableStack<System.Int32>(System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemCollectionsImmutable_ImmutableQueueStack)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Collections_Immutable_System_Collections_Immutable_ImmutableStack_Create_Int32_System_Collections_Immutable_Immut() { _ = ImmutableStack.Create<Int32>(42); }
}
