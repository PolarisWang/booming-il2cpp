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

// Auto-generated patch-side skeletons for ReflectionGenericsPatch.
// Each method exercises a method from the patch side back to the host after hot-update.
public static class ReflectionGenericsPatch
{
    // Purpose: Verify Type.GetGenericTypeDefinition executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Private.CoreLib/System.Type::GetGenericTypeDefinition:System.Type()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionGenerics)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Type_GetGenericTypeDefinition_System_Type() { _ = typeof(byte).GetGenericTypeDefinition(); }
    // Purpose: Verify Type.MakeGenericType executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Private.CoreLib/System.Type::MakeGenericType:System.Type(System.Type[])")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionGenerics)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Type_MakeGenericType_System_Type_System_Type() { _ = typeof(byte).MakeGenericType(Array.Empty<System.Type>()); }
    // Purpose: Verify Type.GetGenericArguments executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Private.CoreLib/System.Type::GetGenericArguments:System.Type[]()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionGenerics)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Type_GetGenericArguments_System_Type() { _ = typeof(byte).GetGenericArguments(); }
    // Purpose: Verify Type.GetGenericParameterConstraints executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Private.CoreLib/System.Type::GetGenericParameterConstraints:System.Type[]()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionGenerics)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Type_GetGenericParameterConstraints_System_Type() { _ = typeof(byte).GetGenericParameterConstraints(); }
    // Purpose: Verify Type.get_GenericParameterPosition executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Private.CoreLib/System.Type::get_GenericParameterPosition:System.Int32()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionGenerics)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Type_get_GenericParameterPosition_System_Int32() { _ = typeof(byte).GenericParameterPosition; }
    // Purpose: Verify Type.ContainsGenericParameters executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Private.CoreLib/System.Type::ContainsGenericParameters:System.Boolean()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionGenerics)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Type_ContainsGenericParameters_System_Boolean() { _ = typeof(byte).ContainsGenericParameters(); }
    // Purpose: Verify Type.get_IsGenericType executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Private.CoreLib/System.Type::get_IsGenericType:System.Boolean()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionGenerics)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Type_get_IsGenericType_System_Boolean() { _ = typeof(byte).IsGenericType; }
    // Purpose: Verify Type.get_IsGenericTypeDefinition executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Private.CoreLib/System.Type::get_IsGenericTypeDefinition:System.Boolean()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionGenerics)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Type_get_IsGenericTypeDefinition_System_Boolean() { _ = typeof(byte).IsGenericTypeDefinition; }
    // Purpose: Verify Type.get_IsConstructedGenericType executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Private.CoreLib/System.Type::get_IsConstructedGenericType:System.Boolean()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionGenerics)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Type_get_IsConstructedGenericType_System_Boolean() { _ = typeof(byte).IsConstructedGenericType; }
    // Purpose: Verify Type.MakeArrayType executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Private.CoreLib/System.Type::MakeArrayType:System.Type()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionGenerics)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Type_MakeArrayType_System_Type() { _ = typeof(byte).MakeArrayType(); }
    // Purpose: Verify Type.MakeByRefType executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Private.CoreLib/System.Type::MakeByRefType:System.Type()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionGenerics)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Type_MakeByRefType_System_Type() { _ = typeof(byte).MakeByRefType(); }
    // Purpose: Verify Type.MakePointerType executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Private.CoreLib/System.Type::MakePointerType:System.Type()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionGenerics)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Type_MakePointerType_System_Type() { _ = typeof(byte).MakePointerType(); }}
