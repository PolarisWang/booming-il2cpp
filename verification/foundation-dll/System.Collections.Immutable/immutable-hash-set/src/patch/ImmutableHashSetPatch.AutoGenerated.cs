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

// Auto-generated patch-side skeletons for ImmutableHashSetPatch.
// Each method exercises a method from the patch side back to the host after hot-update.
public static partial class ImmutableHashSetPatch
{
    // Purpose: Verify ImmutableHashSet.Create<Byte> executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableHashSet::Create<Byte>:System.Collections.Immutable.ImmutableHashSet<System.Byte>()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemCollectionsImmutable_ImmutableHashSet)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Collections_Immutable_System_Collections_Immutable_ImmutableHashSet_Create_Byte_System_Collections_Immutable_Immu() { _ = ImmutableHashSet.Create<Byte>(); }
    // Purpose: Verify ImmutableHashSet.Create<Byte> executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableHashSet::Create<Byte>:System.Collections.Immutable.ImmutableHashSet<System.Byte>(System.Byte)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemCollectionsImmutable_ImmutableHashSet)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Collections_Immutable_System_Collections_Immutable_ImmutableHashSet_Create_Byte_System_Collections_Immutable_Immu() { _ = ImmutableHashSet.Create<Byte>((byte)42); }
    // Purpose: Verify ImmutableHashSet.Create<Byte> executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableHashSet::Create<Byte>:System.Collections.Immutable.ImmutableHashSet<System.Byte>(System.Byte,System.Byte)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemCollectionsImmutable_ImmutableHashSet)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Collections_Immutable_System_Collections_Immutable_ImmutableHashSet_Create_Byte_System_Collections_Immutable_Immu() { _ = ImmutableHashSet.Create<Byte>((byte)42, (byte)42); }
    // Purpose: Verify ImmutableHashSet.Create<Int32> executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableHashSet::Create<Int32>:System.Collections.Immutable.ImmutableHashSet<System.Int32>()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemCollectionsImmutable_ImmutableHashSet)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Collections_Immutable_System_Collections_Immutable_ImmutableHashSet_Create_Int32_System_Collections_Immutable_Imm() { _ = ImmutableHashSet.Create<Int32>(); }
    // Purpose: Verify ImmutableHashSet.Create<Int32> executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableHashSet::Create<Int32>:System.Collections.Immutable.ImmutableHashSet<System.Int32>(System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemCollectionsImmutable_ImmutableHashSet)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Collections_Immutable_System_Collections_Immutable_ImmutableHashSet_Create_Int32_System_Collections_Immutable_Imm() { _ = ImmutableHashSet.Create<Int32>(42); }}
