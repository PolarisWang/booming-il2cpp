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

// Auto-generated patch-side skeletons for ImmutableQueueStackPatch.
// Each method exercises a method from the patch side back to the host after hot-update.
public static partial class ImmutableQueueStackPatch
{
    // Purpose: Verify ImmutableQueue.Create<Byte> executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableQueue::Create<Byte>:System.Collections.Immutable.ImmutableQueue<System.Byte>()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemCollectionsImmutable_ImmutableQueueStack)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Collections_Immutable_System_Collections_Immutable_ImmutableQueue_Create_Byte_System_Collections_Immutable_Immuta() { _ = ImmutableQueue.Create<Byte>(); }
    // Purpose: Verify ImmutableQueue.Create<Byte> executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableQueue::Create<Byte>:System.Collections.Immutable.ImmutableQueue<System.Byte>(System.Byte)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemCollectionsImmutable_ImmutableQueueStack)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Collections_Immutable_System_Collections_Immutable_ImmutableQueue_Create_Byte_System_Collections_Immutable_Immuta() { _ = ImmutableQueue.Create<Byte>((byte)42); }
    // Purpose: Verify ImmutableQueue.Create<Byte> executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableQueue::Create<Byte>:System.Collections.Immutable.ImmutableQueue<System.Byte>(System.Byte,System.Byte)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemCollectionsImmutable_ImmutableQueueStack)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Collections_Immutable_System_Collections_Immutable_ImmutableQueue_Create_Byte_System_Collections_Immutable_Immuta() { _ = ImmutableQueue.Create<Byte>((byte)42, (byte)42); }
    // Purpose: Verify ImmutableQueue.Create<Int32> executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableQueue::Create<Int32>:System.Collections.Immutable.ImmutableQueue<System.Int32>()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemCollectionsImmutable_ImmutableQueueStack)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Collections_Immutable_System_Collections_Immutable_ImmutableQueue_Create_Int32_System_Collections_Immutable_Immut() { _ = ImmutableQueue.Create<Int32>(); }
    // Purpose: Verify ImmutableQueue.Create<Int32> executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableQueue::Create<Int32>:System.Collections.Immutable.ImmutableQueue<System.Int32>(System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemCollectionsImmutable_ImmutableQueueStack)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Collections_Immutable_System_Collections_Immutable_ImmutableQueue_Create_Int32_System_Collections_Immutable_Immut() { _ = ImmutableQueue.Create<Int32>(42); }
    // Purpose: Verify ImmutableStack.Create<Byte> executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableStack::Create<Byte>:System.Collections.Immutable.ImmutableStack<System.Byte>()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemCollectionsImmutable_ImmutableQueueStack)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Collections_Immutable_System_Collections_Immutable_ImmutableStack_Create_Byte_System_Collections_Immutable_Immuta() { _ = ImmutableStack.Create<Byte>(); }
    // Purpose: Verify ImmutableStack.Create<Byte> executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableStack::Create<Byte>:System.Collections.Immutable.ImmutableStack<System.Byte>(System.Byte)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemCollectionsImmutable_ImmutableQueueStack)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Collections_Immutable_System_Collections_Immutable_ImmutableStack_Create_Byte_System_Collections_Immutable_Immuta() { _ = ImmutableStack.Create<Byte>((byte)42); }
    // Purpose: Verify ImmutableStack.Create<Byte> executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableStack::Create<Byte>:System.Collections.Immutable.ImmutableStack<System.Byte>(System.Byte,System.Byte)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemCollectionsImmutable_ImmutableQueueStack)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Collections_Immutable_System_Collections_Immutable_ImmutableStack_Create_Byte_System_Collections_Immutable_Immuta() { _ = ImmutableStack.Create<Byte>((byte)42, (byte)42); }
    // Purpose: Verify ImmutableStack.Create<Int32> executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableStack::Create<Int32>:System.Collections.Immutable.ImmutableStack<System.Int32>()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemCollectionsImmutable_ImmutableQueueStack)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Collections_Immutable_System_Collections_Immutable_ImmutableStack_Create_Int32_System_Collections_Immutable_Immut() { _ = ImmutableStack.Create<Int32>(); }
    // Purpose: Verify ImmutableStack.Create<Int32> executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableStack::Create<Int32>:System.Collections.Immutable.ImmutableStack<System.Int32>(System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemCollectionsImmutable_ImmutableQueueStack)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Collections_Immutable_System_Collections_Immutable_ImmutableStack_Create_Int32_System_Collections_Immutable_Immut() { _ = ImmutableStack.Create<Int32>(42); }}
