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

// Hot-update stubs for family/System.Private.CoreLib/guid/random/hashcode
public static class GuidRandomHashcodePatch
{
    // Verify Guid..ctor after hot-update (host side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Guid::.ctor:System.Void(System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_GuidRandomHashcode)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Guid_ctor_System_Void_System_String() { new Guid("00000000-0000-0000-0000-000000000000"); }
    // Verify Guid..ctor after hot-update (host side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Guid::.ctor:System.Void(System.Byte[])")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_GuidRandomHashcode)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Guid_ctor_System_Void_System_Byte() { new Guid("00000000-0000-0000-0000-000000000000"); }
    // Verify Guid.NewGuid after hot-update (host side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Guid::NewGuid:System.Guid()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_GuidRandomHashcode)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Guid_NewGuid_System_Guid() { _ = Guid.NewGuid(); }
    // Verify Guid.Parse after hot-update (host side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Guid::Parse:System.Guid(System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_GuidRandomHashcode)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Guid_Parse_System_Guid_System_String() { _ = Guid.Parse("00000000-0000-0000-0000-000000000000"); }
    // Verify Guid.TryParse after hot-update (host side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Guid::TryParse:System.Boolean(System.String,System.Guid&)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_GuidRandomHashcode)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Guid_TryParse_System_Boolean_System_String_System_Guid() { }
    // Verify Guid.ToString after hot-update (host side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Guid::ToString:System.String()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_GuidRandomHashcode)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Guid_ToString_System_String() { _ = Guid.NewGuid().ToString(); }
    // Verify Random..ctor after hot-update (host side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Random::.ctor:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_GuidRandomHashcode)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Random_ctor_System_Void() { new Random(); }
    // Verify Random.Next after hot-update (host side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Random::Next:System.Int32()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_GuidRandomHashcode)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Random_Next_System_Int32() { _ = new Random().Next(); }
    // Verify Random.Next after hot-update (host side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Random::Next:System.Int32(System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_GuidRandomHashcode)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Random_Next_System_Int32_System_Int32() { _ = new Random().Next(42); }
    // Verify Random.NextDouble after hot-update (host side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Random::NextDouble:System.Double()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_GuidRandomHashcode)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Random_NextDouble_System_Double() { _ = new Random().NextDouble(); }
    // Verify Random.NextBytes after hot-update (host side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Random::NextBytes:System.Void(System.Byte[])")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_GuidRandomHashcode)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Random_NextBytes_System_Void_System_Byte() { new Random().NextBytes(new byte[] { 1, 2, 3 }); }
    // Verify HashCode.Add after hot-update (host side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.HashCode::Add:System.Void(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_GuidRandomHashcode)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_HashCode_Add_System_Void_System_Object() { default(HashCode).Add(42); }
    // Verify HashCode.ToHashCode after hot-update (host side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.HashCode::ToHashCode:System.Int32()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_GuidRandomHashcode)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_HashCode_ToHashCode_System_Int32() { _ = default(HashCode).ToHashCode(); }
    // Verify HashCode.Combine after hot-update (host side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.HashCode::Combine:System.Int32(System.Int32,System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_GuidRandomHashcode)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_HashCode_Combine_System_Int32_System_Int32_System_Int32() { _ = HashCode.Combine(42, 42); }}
