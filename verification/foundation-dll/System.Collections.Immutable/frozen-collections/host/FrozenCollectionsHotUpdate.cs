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

// Hot-update stubs for family/System.Collections.Immutable/frozen/collections
public static class FrozenCollectionsHotUpdate
{
    // Verify FrozenDictionary.Create<string,int> after hot-update (patch side)
    [HotUpdateSubjectId("System.Collections.Immutable/System.Collections.Frozen.FrozenDictionary::Create<string,int>:System.Collections.Frozen.FrozenDictionary<System.String,System.Int32>()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemCollectionsImmutable_FrozenCollections)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Collections_Immutable_System_Collections_Frozen_FrozenDictionary_Create_string_int_System_Collections_Frozen_Froz() { _ = FrozenDictionary.Create<string,int>(); }
    // Verify FrozenDictionary.Create<Byte,Byte> after hot-update (patch side)
    [HotUpdateSubjectId("System.Collections.Immutable/System.Collections.Frozen.FrozenDictionary::Create<Byte,Byte>:System.Collections.Frozen.FrozenDictionary<System.Byte,System.Byte>()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemCollectionsImmutable_FrozenCollections)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Collections_Immutable_System_Collections_Frozen_FrozenDictionary_Create_Byte_Byte_System_Collections_Frozen_Froze() { _ = FrozenDictionary.Create<Byte,Byte>(); }
    // Verify FrozenDictionary.Create<string,string> after hot-update (patch side)
    [HotUpdateSubjectId("System.Collections.Immutable/System.Collections.Frozen.FrozenDictionary::Create<string,string>:System.Collections.Frozen.FrozenDictionary<System.String,System.String>()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemCollectionsImmutable_FrozenCollections)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Collections_Immutable_System_Collections_Frozen_FrozenDictionary_Create_string_string_System_Collections_Frozen_F() { _ = FrozenDictionary.Create<string,string>(); }
    // Verify FrozenSet.Create<Byte> after hot-update (patch side)
    [HotUpdateSubjectId("System.Collections.Immutable/System.Collections.Frozen.FrozenSet::Create<Byte>:System.Collections.Frozen.FrozenSet<System.Byte>()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemCollectionsImmutable_FrozenCollections)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Collections_Immutable_System_Collections_Frozen_FrozenSet_Create_Byte_System_Collections_Frozen_FrozenSet_System_() { _ = FrozenSet.Create<Byte>(); }
    // Verify FrozenSet.Create<Byte> after hot-update (patch side)
    [HotUpdateSubjectId("System.Collections.Immutable/System.Collections.Frozen.FrozenSet::Create<Byte>:System.Collections.Frozen.FrozenSet<System.Byte>(System.Byte)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemCollectionsImmutable_FrozenCollections)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Collections_Immutable_System_Collections_Frozen_FrozenSet_Create_Byte_System_Collections_Frozen_FrozenSet_System_() { _ = FrozenSet.Create<Byte>((byte)42); }
    // Verify FrozenSet.Create<Byte> after hot-update (patch side)
    [HotUpdateSubjectId("System.Collections.Immutable/System.Collections.Frozen.FrozenSet::Create<Byte>:System.Collections.Frozen.FrozenSet<System.Byte>(System.Byte,System.Byte)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemCollectionsImmutable_FrozenCollections)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Collections_Immutable_System_Collections_Frozen_FrozenSet_Create_Byte_System_Collections_Frozen_FrozenSet_System_() { _ = FrozenSet.Create<Byte>((byte)42, (byte)42); }
    // Verify FrozenSet.Create<Int32> after hot-update (patch side)
    [HotUpdateSubjectId("System.Collections.Immutable/System.Collections.Frozen.FrozenSet::Create<Int32>:System.Collections.Frozen.FrozenSet<System.Int32>()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemCollectionsImmutable_FrozenCollections)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Collections_Immutable_System_Collections_Frozen_FrozenSet_Create_Int32_System_Collections_Frozen_FrozenSet_System() { _ = FrozenSet.Create<Int32>(); }
    // Verify FrozenSet.Create<Int32> after hot-update (patch side)
    [HotUpdateSubjectId("System.Collections.Immutable/System.Collections.Frozen.FrozenSet::Create<Int32>:System.Collections.Frozen.FrozenSet<System.Int32>(System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemCollectionsImmutable_FrozenCollections)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Collections_Immutable_System_Collections_Frozen_FrozenSet_Create_Int32_System_Collections_Frozen_FrozenSet_System() { _ = FrozenSet.Create<Int32>(42); }}
