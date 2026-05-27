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

// Hot-update stubs for family/System.Collections.Immutable/immutable/hash/set
public static class ImmutableHashSetPatch
{
    // Verify ImmutableHashSet.Create<Byte> after hot-update (host side)
    [HotUpdateSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableHashSet::Create<Byte>:System.Collections.Immutable.ImmutableHashSet<System.Byte>()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemCollectionsImmutable_ImmutableHashSet)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Collections_Immutable_System_Collections_Immutable_ImmutableHashSet_Create_Byte_System_Collections_Immutable_Immu() { _ = ImmutableHashSet.Create<Byte>(); }
    // Verify ImmutableHashSet.Create<Byte> after hot-update (host side)
    [HotUpdateSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableHashSet::Create<Byte>:System.Collections.Immutable.ImmutableHashSet<System.Byte>(System.Byte)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemCollectionsImmutable_ImmutableHashSet)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Collections_Immutable_System_Collections_Immutable_ImmutableHashSet_Create_Byte_System_Collections_Immutable_Immu() { _ = ImmutableHashSet.Create<Byte>((byte)42); }
    // Verify ImmutableHashSet.Create<Byte> after hot-update (host side)
    [HotUpdateSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableHashSet::Create<Byte>:System.Collections.Immutable.ImmutableHashSet<System.Byte>(System.Byte,System.Byte)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemCollectionsImmutable_ImmutableHashSet)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Collections_Immutable_System_Collections_Immutable_ImmutableHashSet_Create_Byte_System_Collections_Immutable_Immu() { _ = ImmutableHashSet.Create<Byte>((byte)42, (byte)42); }
    // Verify ImmutableHashSet.Create<Int32> after hot-update (host side)
    [HotUpdateSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableHashSet::Create<Int32>:System.Collections.Immutable.ImmutableHashSet<System.Int32>()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemCollectionsImmutable_ImmutableHashSet)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Collections_Immutable_System_Collections_Immutable_ImmutableHashSet_Create_Int32_System_Collections_Immutable_Imm() { _ = ImmutableHashSet.Create<Int32>(); }
    // Verify ImmutableHashSet.Create<Int32> after hot-update (host side)
    [HotUpdateSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableHashSet::Create<Int32>:System.Collections.Immutable.ImmutableHashSet<System.Int32>(System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemCollectionsImmutable_ImmutableHashSet)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Collections_Immutable_System_Collections_Immutable_ImmutableHashSet_Create_Int32_System_Collections_Immutable_Imm() { _ = ImmutableHashSet.Create<Int32>(42); }}
