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

// Hot-update stubs for family/System.Collections.Immutable/immutable/sorted/dictionary
public static class ImmutableSortedDictionaryPatch
{
    // Verify ImmutableSortedDictionary.Create<string,int> after hot-update (host side)
    [HotUpdateSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableSortedDictionary::Create<string,int>:System.Collections.Immutable.ImmutableSortedDictionary<System.String,System.Int32>()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemCollectionsImmutable_ImmutableSortedDictionary)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Collections_Immutable_System_Collections_Immutable_ImmutableSortedDictionary_Create_string_int_System_Collections() { _ = ImmutableSortedDictionary.Create<string,int>(); }
    // Verify ImmutableSortedDictionary.Create<Byte,Byte> after hot-update (host side)
    [HotUpdateSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableSortedDictionary::Create<Byte,Byte>:System.Collections.Immutable.ImmutableSortedDictionary<System.Byte,System.Byte>()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemCollectionsImmutable_ImmutableSortedDictionary)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Collections_Immutable_System_Collections_Immutable_ImmutableSortedDictionary_Create_Byte_Byte_System_Collections_() { _ = ImmutableSortedDictionary.Create<Byte,Byte>(); }}
