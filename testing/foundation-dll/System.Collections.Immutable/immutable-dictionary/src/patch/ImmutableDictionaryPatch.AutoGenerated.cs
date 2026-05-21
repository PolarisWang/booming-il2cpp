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

// Auto-generated patch-side skeletons for ImmutableDictionaryPatch.
// Each method exercises a method from the patch side back to the host after hot-update.
public static partial class ImmutableDictionaryPatch
{
    // Purpose: Verify ImmutableDictionary.Create<string,int> executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableDictionary::Create<string,int>:System.Collections.Immutable.ImmutableDictionary<System.String,System.Int32>()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemCollectionsImmutable_ImmutableDictionary)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Collections_Immutable_System_Collections_Immutable_ImmutableDictionary_Create_string_int_System_Collections_Immut() { _ = ImmutableDictionary.Create<string,int>(); }
    // Purpose: Verify ImmutableDictionary.Create<Byte,Byte> executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableDictionary::Create<Byte,Byte>:System.Collections.Immutable.ImmutableDictionary<System.Byte,System.Byte>()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemCollectionsImmutable_ImmutableDictionary)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Collections_Immutable_System_Collections_Immutable_ImmutableDictionary_Create_Byte_Byte_System_Collections_Immuta() { _ = ImmutableDictionary.Create<Byte,Byte>(); }
    // Purpose: Verify ImmutableDictionary.Create<string,string> executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableDictionary::Create<string,string>:System.Collections.Immutable.ImmutableDictionary<System.String,System.String>()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemCollectionsImmutable_ImmutableDictionary)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Collections_Immutable_System_Collections_Immutable_ImmutableDictionary_Create_string_string_System_Collections_Im() { _ = ImmutableDictionary.Create<string,string>(); }}
