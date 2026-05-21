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

// Auto-generated patch-side skeletons for ImmutableSortedSetPatch.
// Each method exercises a method from the patch side back to the host after hot-update.
public static partial class ImmutableSortedSetPatch
{
    // Purpose: Verify ImmutableSortedSet.Create<Byte> executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableSortedSet::Create<Byte>:System.Collections.Immutable.ImmutableSortedSet<System.Byte>()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemCollectionsImmutable_ImmutableSortedSet)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Collections_Immutable_System_Collections_Immutable_ImmutableSortedSet_Create_Byte_System_Collections_Immutable_Im() { _ = ImmutableSortedSet.Create<Byte>(); }
    // Purpose: Verify ImmutableSortedSet.Create<Byte> executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableSortedSet::Create<Byte>:System.Collections.Immutable.ImmutableSortedSet<System.Byte>(System.Byte)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemCollectionsImmutable_ImmutableSortedSet)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Collections_Immutable_System_Collections_Immutable_ImmutableSortedSet_Create_Byte_System_Collections_Immutable_Im() { _ = ImmutableSortedSet.Create<Byte>((byte)42); }
    // Purpose: Verify ImmutableSortedSet.Create<Byte> executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableSortedSet::Create<Byte>:System.Collections.Immutable.ImmutableSortedSet<System.Byte>(System.Byte,System.Byte)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemCollectionsImmutable_ImmutableSortedSet)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Collections_Immutable_System_Collections_Immutable_ImmutableSortedSet_Create_Byte_System_Collections_Immutable_Im() { _ = ImmutableSortedSet.Create<Byte>((byte)42, (byte)42); }
    // Purpose: Verify ImmutableSortedSet.Create<Int32> executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableSortedSet::Create<Int32>:System.Collections.Immutable.ImmutableSortedSet<System.Int32>()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemCollectionsImmutable_ImmutableSortedSet)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Collections_Immutable_System_Collections_Immutable_ImmutableSortedSet_Create_Int32_System_Collections_Immutable_I() { _ = ImmutableSortedSet.Create<Int32>(); }
    // Purpose: Verify ImmutableSortedSet.Create<Int32> executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableSortedSet::Create<Int32>:System.Collections.Immutable.ImmutableSortedSet<System.Int32>(System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemCollectionsImmutable_ImmutableSortedSet)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Collections_Immutable_System_Collections_Immutable_ImmutableSortedSet_Create_Int32_System_Collections_Immutable_I() { _ = ImmutableSortedSet.Create<Int32>(42); }}
