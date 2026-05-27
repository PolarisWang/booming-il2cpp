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

// Hot-update stubs for family/System.Collections.Immutable/immutable/sorted/set
public static class ImmutableSortedSetPatch
{
    // Verify ImmutableSortedSet.Create<Byte> after hot-update (host side)
    [HotUpdateSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableSortedSet::Create<Byte>:System.Collections.Immutable.ImmutableSortedSet<System.Byte>()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemCollectionsImmutable_ImmutableSortedSet)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Collections_Immutable_System_Collections_Immutable_ImmutableSortedSet_Create_Byte_System_Collections_Immutable_Im() { _ = ImmutableSortedSet.Create<Byte>(); }
    // Verify ImmutableSortedSet.Create<Byte> after hot-update (host side)
    [HotUpdateSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableSortedSet::Create<Byte>:System.Collections.Immutable.ImmutableSortedSet<System.Byte>(System.Byte)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemCollectionsImmutable_ImmutableSortedSet)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Collections_Immutable_System_Collections_Immutable_ImmutableSortedSet_Create_Byte_System_Collections_Immutable_Im() { _ = ImmutableSortedSet.Create<Byte>((byte)42); }
    // Verify ImmutableSortedSet.Create<Byte> after hot-update (host side)
    [HotUpdateSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableSortedSet::Create<Byte>:System.Collections.Immutable.ImmutableSortedSet<System.Byte>(System.Byte,System.Byte)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemCollectionsImmutable_ImmutableSortedSet)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Collections_Immutable_System_Collections_Immutable_ImmutableSortedSet_Create_Byte_System_Collections_Immutable_Im() { _ = ImmutableSortedSet.Create<Byte>((byte)42, (byte)42); }
    // Verify ImmutableSortedSet.Create<Int32> after hot-update (host side)
    [HotUpdateSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableSortedSet::Create<Int32>:System.Collections.Immutable.ImmutableSortedSet<System.Int32>()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemCollectionsImmutable_ImmutableSortedSet)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Collections_Immutable_System_Collections_Immutable_ImmutableSortedSet_Create_Int32_System_Collections_Immutable_I() { _ = ImmutableSortedSet.Create<Int32>(); }
    // Verify ImmutableSortedSet.Create<Int32> after hot-update (host side)
    [HotUpdateSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableSortedSet::Create<Int32>:System.Collections.Immutable.ImmutableSortedSet<System.Int32>(System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemCollectionsImmutable_ImmutableSortedSet)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Collections_Immutable_System_Collections_Immutable_ImmutableSortedSet_Create_Int32_System_Collections_Immutable_I() { _ = ImmutableSortedSet.Create<Int32>(42); }}
