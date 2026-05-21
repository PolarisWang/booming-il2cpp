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

// Auto-generated patch-side skeletons for ImmutableListPatch.
// Each method exercises a method from the patch side back to the host after hot-update.
public static partial class ImmutableListPatch
{
    // Purpose: Verify ImmutableList.Create<Byte> executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableList::Create<Byte>:System.Collections.Immutable.ImmutableList<System.Byte>()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemCollectionsImmutable_ImmutableList)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Collections_Immutable_System_Collections_Immutable_ImmutableList_Create_Byte_System_Collections_Immutable_Immutab() { _ = ImmutableList.Create<Byte>(); }
    // Purpose: Verify ImmutableList.Create<Byte> executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableList::Create<Byte>:System.Collections.Immutable.ImmutableList<System.Byte>(System.Byte)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemCollectionsImmutable_ImmutableList)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Collections_Immutable_System_Collections_Immutable_ImmutableList_Create_Byte_System_Collections_Immutable_Immutab() { _ = ImmutableList.Create<Byte>((byte)42); }
    // Purpose: Verify ImmutableList.Create<Byte> executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableList::Create<Byte>:System.Collections.Immutable.ImmutableList<System.Byte>(System.Byte,System.Byte)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemCollectionsImmutable_ImmutableList)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Collections_Immutable_System_Collections_Immutable_ImmutableList_Create_Byte_System_Collections_Immutable_Immutab() { _ = ImmutableList.Create<Byte>((byte)42, (byte)42); }
    // Purpose: Verify ImmutableList.Create<Int32> executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableList::Create<Int32>:System.Collections.Immutable.ImmutableList<System.Int32>()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemCollectionsImmutable_ImmutableList)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Collections_Immutable_System_Collections_Immutable_ImmutableList_Create_Int32_System_Collections_Immutable_Immuta() { _ = ImmutableList.Create<Int32>(); }
    // Purpose: Verify ImmutableList.Create<Int32> executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableList::Create<Int32>:System.Collections.Immutable.ImmutableList<System.Int32>(System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemCollectionsImmutable_ImmutableList)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Collections_Immutable_System_Collections_Immutable_ImmutableList_Create_Int32_System_Collections_Immutable_Immuta() { _ = ImmutableList.Create<Int32>(42); }
    // Purpose: Verify ImmutableList.Create<String> executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableList::Create<String>:System.Collections.Immutable.ImmutableList<System.String>(System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemCollectionsImmutable_ImmutableList)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Collections_Immutable_System_Collections_Immutable_ImmutableList_Create_String_System_Collections_Immutable_Immut() { _ = ImmutableList.Create<String>("hello"); }}
