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

// Auto-generated patch-side skeletons for FrozenCollectionsPatch.
// Each method exercises a method from the patch side back to the host after hot-update.
public static partial class FrozenCollectionsPatch
{
    // Purpose: Verify FrozenDictionary.Create<string,int> executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Collections.Immutable/System.Collections.Frozen.FrozenDictionary::Create<string,int>:System.Collections.Frozen.FrozenDictionary<System.String,System.Int32>()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemCollectionsImmutable_FrozenCollections)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Collections_Immutable_System_Collections_Frozen_FrozenDictionary_Create_string_int_System_Collections_Frozen_Froz() { _ = FrozenDictionary.Create<string,int>(); }
    // Purpose: Verify FrozenDictionary.Create<Byte,Byte> executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Collections.Immutable/System.Collections.Frozen.FrozenDictionary::Create<Byte,Byte>:System.Collections.Frozen.FrozenDictionary<System.Byte,System.Byte>()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemCollectionsImmutable_FrozenCollections)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Collections_Immutable_System_Collections_Frozen_FrozenDictionary_Create_Byte_Byte_System_Collections_Frozen_Froze() { _ = FrozenDictionary.Create<Byte,Byte>(); }
    // Purpose: Verify FrozenDictionary.Create<string,string> executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Collections.Immutable/System.Collections.Frozen.FrozenDictionary::Create<string,string>:System.Collections.Frozen.FrozenDictionary<System.String,System.String>()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemCollectionsImmutable_FrozenCollections)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Collections_Immutable_System_Collections_Frozen_FrozenDictionary_Create_string_string_System_Collections_Frozen_F() { _ = FrozenDictionary.Create<string,string>(); }
    // Purpose: Verify FrozenSet.Create<Byte> executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Collections.Immutable/System.Collections.Frozen.FrozenSet::Create<Byte>:System.Collections.Frozen.FrozenSet<System.Byte>()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemCollectionsImmutable_FrozenCollections)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Collections_Immutable_System_Collections_Frozen_FrozenSet_Create_Byte_System_Collections_Frozen_FrozenSet_System_() { _ = FrozenSet.Create<Byte>(); }
    // Purpose: Verify FrozenSet.Create<Byte> executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Collections.Immutable/System.Collections.Frozen.FrozenSet::Create<Byte>:System.Collections.Frozen.FrozenSet<System.Byte>(System.Byte)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemCollectionsImmutable_FrozenCollections)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Collections_Immutable_System_Collections_Frozen_FrozenSet_Create_Byte_System_Collections_Frozen_FrozenSet_System_() { _ = FrozenSet.Create<Byte>((byte)42); }
    // Purpose: Verify FrozenSet.Create<Byte> executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Collections.Immutable/System.Collections.Frozen.FrozenSet::Create<Byte>:System.Collections.Frozen.FrozenSet<System.Byte>(System.Byte,System.Byte)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemCollectionsImmutable_FrozenCollections)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Collections_Immutable_System_Collections_Frozen_FrozenSet_Create_Byte_System_Collections_Frozen_FrozenSet_System_() { _ = FrozenSet.Create<Byte>((byte)42, (byte)42); }
    // Purpose: Verify FrozenSet.Create<Int32> executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Collections.Immutable/System.Collections.Frozen.FrozenSet::Create<Int32>:System.Collections.Frozen.FrozenSet<System.Int32>()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemCollectionsImmutable_FrozenCollections)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Collections_Immutable_System_Collections_Frozen_FrozenSet_Create_Int32_System_Collections_Frozen_FrozenSet_System() { _ = FrozenSet.Create<Int32>(); }
    // Purpose: Verify FrozenSet.Create<Int32> executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Collections.Immutable/System.Collections.Frozen.FrozenSet::Create<Int32>:System.Collections.Frozen.FrozenSet<System.Int32>(System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemCollectionsImmutable_FrozenCollections)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Collections_Immutable_System_Collections_Frozen_FrozenSet_Create_Int32_System_Collections_Frozen_FrozenSet_System() { _ = FrozenSet.Create<Int32>(42); }}
