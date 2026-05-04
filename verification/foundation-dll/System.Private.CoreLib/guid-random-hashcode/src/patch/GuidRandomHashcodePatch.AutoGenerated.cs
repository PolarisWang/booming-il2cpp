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

// Auto-generated patch-side skeletons for GuidRandomHashcodePatch.
// Each method exercises a method from the patch side back to the host after hot-update.
public static partial class GuidRandomHashcodePatch
{
    // Purpose: Verify Guid..ctor executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Private.CoreLib/System.Guid::.ctor:System.Void(System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_GuidRandomHashcode)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Guid_ctor_System_Void_System_String() { }
    // Purpose: Verify Guid..ctor executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Private.CoreLib/System.Guid::.ctor:System.Void(System.Byte[])")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_GuidRandomHashcode)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Guid_ctor_System_Void_System_Byte() { }
    // Purpose: Verify Guid.NewGuid executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Private.CoreLib/System.Guid::NewGuid:System.Guid()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_GuidRandomHashcode)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Guid_NewGuid_System_Guid() { _ = Guid.NewGuid(); }
    // Purpose: Verify Guid.Parse executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Private.CoreLib/System.Guid::Parse:System.Guid(System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_GuidRandomHashcode)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Guid_Parse_System_Guid_System_String() { }
    // Purpose: Verify Guid.TryParse executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Private.CoreLib/System.Guid::TryParse:System.Boolean(System.String,System.Guid&)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_GuidRandomHashcode)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Guid_TryParse_System_Boolean_System_String_System_Guid() { }
    // Purpose: Verify Guid.ToString executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Private.CoreLib/System.Guid::ToString:System.String()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_GuidRandomHashcode)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Guid_ToString_System_String() { _ = Guid.NewGuid().ToString(); }
    // Purpose: Verify Random..ctor executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Private.CoreLib/System.Random::.ctor:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_GuidRandomHashcode)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Random_ctor_System_Void() { new Random(); }
    // Purpose: Verify Random.Next executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Private.CoreLib/System.Random::Next:System.Int32()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_GuidRandomHashcode)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Random_Next_System_Int32() { _ = new Random().Next(); }
    // Purpose: Verify Random.Next executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Private.CoreLib/System.Random::Next:System.Int32(System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_GuidRandomHashcode)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Random_Next_System_Int32_System_Int32() { _ = new Random().Next(42); }
    // Purpose: Verify Random.NextDouble executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Private.CoreLib/System.Random::NextDouble:System.Double()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_GuidRandomHashcode)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Random_NextDouble_System_Double() { _ = new Random().NextDouble(); }
    // Purpose: Verify Random.NextBytes executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Private.CoreLib/System.Random::NextBytes:System.Void(System.Byte[])")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_GuidRandomHashcode)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Random_NextBytes_System_Void_System_Byte() { new Random().NextBytes(new byte[] { 1, 2, 3 }); }
    // Purpose: Verify HashCode.Add executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Private.CoreLib/System.HashCode::Add:System.Void(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_GuidRandomHashcode)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_HashCode_Add_System_Void_System_Object() { default(HashCode).Add(42); }
    // Purpose: Verify HashCode.ToHashCode executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Private.CoreLib/System.HashCode::ToHashCode:System.Int32()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_GuidRandomHashcode)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_HashCode_ToHashCode_System_Int32() { _ = default(HashCode).ToHashCode(); }
    // Purpose: Verify HashCode.Combine executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Private.CoreLib/System.HashCode::Combine:System.Int32(System.Int32,System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_GuidRandomHashcode)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_HashCode_Combine_System_Int32_System_Int32_System_Int32() { _ = HashCode.Combine(42, 42); }}
