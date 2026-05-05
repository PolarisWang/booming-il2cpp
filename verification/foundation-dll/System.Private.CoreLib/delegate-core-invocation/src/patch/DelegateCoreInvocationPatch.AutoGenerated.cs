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

// Auto-generated patch-side skeletons for DelegateCoreInvocationPatch.
// Each method exercises a method from the patch side back to the host after hot-update.
public static partial class DelegateCoreInvocationPatch
{
    // Purpose: Verify Delegate.Combine executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Private.CoreLib/System.Delegate::Combine:System.Delegate(System.Delegate,System.Delegate)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_DelegateCoreInvocation)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Delegate_Combine_System_Delegate_System_Delegate_System_Delegate() { _ = Delegate.Combine(null!, null!); }
    // Purpose: Verify Delegate.Remove executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Private.CoreLib/System.Delegate::Remove:System.Delegate(System.Delegate,System.Delegate)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_DelegateCoreInvocation)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Delegate_Remove_System_Delegate_System_Delegate_System_Delegate() { _ = Delegate.Remove(null!, null!); }
    // Purpose: Verify Delegate.DynamicInvoke executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Private.CoreLib/System.Delegate::DynamicInvoke:System.Object(System.Object[])")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_DelegateCoreInvocation)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Delegate_DynamicInvoke_System_Object_System_Object() { }
    // Purpose: Verify Delegate.get_Method executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Private.CoreLib/System.Delegate::get_Method:System.Reflection.MethodInfo()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_DelegateCoreInvocation)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Delegate_get_Method_System_Reflection_MethodInfo() { }
    // Purpose: Verify Delegate.get_Target executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Private.CoreLib/System.Delegate::get_Target:System.Object()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_DelegateCoreInvocation)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Delegate_get_Target_System_Object() { }
    // Purpose: Verify Delegate.CreateDelegate executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Private.CoreLib/System.Delegate::CreateDelegate:System.Delegate(System.Type,System.Object,System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_DelegateCoreInvocation)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Delegate_CreateDelegate_System_Delegate_System_Type_System_Object_System_String() { }
    // Purpose: Verify Delegate.CreateDelegate executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Private.CoreLib/System.Delegate::CreateDelegate:System.Delegate(System.Type,System.Reflection.MethodInfo)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_DelegateCoreInvocation)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Delegate_CreateDelegate_System_Delegate_System_Type_System_Reflection_MethodInfo() { }
    // Purpose: Verify MulticastDelegate.GetInvocationList executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Private.CoreLib/System.MulticastDelegate::GetInvocationList:System.Delegate[]()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_DelegateCoreInvocation)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_MulticastDelegate_GetInvocationList_System_Delegate() { }
    // Purpose: Verify Delegate.op_Equality executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Private.CoreLib/System.Delegate::op_Equality:System.Boolean(System.Delegate,System.Delegate)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_DelegateCoreInvocation)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Delegate_op_Equality_System_Boolean_System_Delegate_System_Delegate() { }
    // Purpose: Verify Delegate.op_Inequality executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Private.CoreLib/System.Delegate::op_Inequality:System.Boolean(System.Delegate,System.Delegate)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_DelegateCoreInvocation)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Delegate_op_Inequality_System_Boolean_System_Delegate_System_Delegate() { }}
