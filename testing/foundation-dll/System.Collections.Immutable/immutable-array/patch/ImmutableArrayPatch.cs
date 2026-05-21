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

// Hot-update stubs for family/System.Collections.Immutable/immutable/array
public static class ImmutableArrayPatch
{
    // Verify ImmutableArray.Create<Byte> after hot-update (host side)
    [HotUpdateSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableArray::Create<Byte>:System.Collections.Immutable.ImmutableArray<System.Byte>()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemCollectionsImmutable_ImmutableArray)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Collections_Immutable_System_Collections_Immutable_ImmutableArray_Create_Byte_System_Collections_Immutable_Immuta() { _ = ImmutableArray.Create<Byte>(); }
    // Verify ImmutableArray.Create<Byte> after hot-update (host side)
    [HotUpdateSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableArray::Create<Byte>:System.Collections.Immutable.ImmutableArray<System.Byte>(System.Byte)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemCollectionsImmutable_ImmutableArray)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Collections_Immutable_System_Collections_Immutable_ImmutableArray_Create_Byte_System_Collections_Immutable_Immuta() { _ = ImmutableArray.Create<Byte>((byte)42); }
    // Verify ImmutableArray.Create<Byte> after hot-update (host side)
    [HotUpdateSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableArray::Create<Byte>:System.Collections.Immutable.ImmutableArray<System.Byte>(System.Byte,System.Byte)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemCollectionsImmutable_ImmutableArray)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Collections_Immutable_System_Collections_Immutable_ImmutableArray_Create_Byte_System_Collections_Immutable_Immuta() { _ = ImmutableArray.Create<Byte>((byte)42, (byte)42); }
    // Verify ImmutableArray.Create<Int32> after hot-update (host side)
    [HotUpdateSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableArray::Create<Int32>:System.Collections.Immutable.ImmutableArray<System.Int32>()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemCollectionsImmutable_ImmutableArray)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Collections_Immutable_System_Collections_Immutable_ImmutableArray_Create_Int32_System_Collections_Immutable_Immut() { _ = ImmutableArray.Create<Int32>(); }
    // Verify ImmutableArray.Create<Int32> after hot-update (host side)
    [HotUpdateSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableArray::Create<Int32>:System.Collections.Immutable.ImmutableArray<System.Int32>(System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemCollectionsImmutable_ImmutableArray)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Collections_Immutable_System_Collections_Immutable_ImmutableArray_Create_Int32_System_Collections_Immutable_Immut() { _ = ImmutableArray.Create<Int32>(42); }
    // Verify ImmutableArray.Create<String> after hot-update (host side)
    [HotUpdateSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableArray::Create<String>:System.Collections.Immutable.ImmutableArray<System.String>(System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemCollectionsImmutable_ImmutableArray)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Collections_Immutable_System_Collections_Immutable_ImmutableArray_Create_String_System_Collections_Immutable_Immu() { _ = ImmutableArray.Create<String>("hello"); }}
