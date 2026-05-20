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

// Auto-generated patch-side skeletons for ImmutableSortedDictionaryPatch.
// Each method exercises a method from the patch side back to the host after hot-update.
public static partial class ImmutableSortedDictionaryPatch
{
    // Purpose: Verify ImmutableSortedDictionary.Create<string,int> executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableSortedDictionary::Create<string,int>:System.Collections.Immutable.ImmutableSortedDictionary<System.String,System.Int32>()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemCollectionsImmutable_ImmutableSortedDictionary)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Collections_Immutable_System_Collections_Immutable_ImmutableSortedDictionary_Create_string_int_System_Collections() { _ = ImmutableSortedDictionary.Create<string,int>(); }
    // Purpose: Verify ImmutableSortedDictionary.Create<Byte,Byte> executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableSortedDictionary::Create<Byte,Byte>:System.Collections.Immutable.ImmutableSortedDictionary<System.Byte,System.Byte>()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemCollectionsImmutable_ImmutableSortedDictionary)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Collections_Immutable_System_Collections_Immutable_ImmutableSortedDictionary_Create_Byte_Byte_System_Collections_() { _ = ImmutableSortedDictionary.Create<Byte,Byte>(); }}
