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

// Auto-generated hot-update skeletons for RuntimeCompilerServicesHotUpdate.
// Each method exercises a method from the HostToPatch direction.
public static class RuntimeCompilerServicesHotUpdate
{
    // Purpose: Verify RuntimeHelpers.GetHashCode executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Runtime.CompilerServices.RuntimeHelpers::GetHashCode:System.Int32(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_RuntimeCompilerServices)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Runtime_CompilerServices_RuntimeHelpers_GetHashCode_System_Int32_System_Object() { _ = RuntimeHelpers.GetHashCode(42); }
    // Purpose: Verify RuntimeHelpers.Equals executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Runtime.CompilerServices.RuntimeHelpers::Equals:System.Boolean(System.Object,System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_RuntimeCompilerServices)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Runtime_CompilerServices_RuntimeHelpers_Equals_System_Boolean_System_Object_System_Object() { _ = RuntimeHelpers.Equals(42, 42); }
    // Purpose: Verify RuntimeHelpers.GetObjectValue executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Runtime.CompilerServices.RuntimeHelpers::GetObjectValue:System.Object(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_RuntimeCompilerServices)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Runtime_CompilerServices_RuntimeHelpers_GetObjectValue_System_Object_System_Object() { _ = RuntimeHelpers.GetObjectValue(42); }
    // Purpose: Verify RuntimeHelpers.RunClassConstructor executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Runtime.CompilerServices.RuntimeHelpers::RunClassConstructor:System.Void(System.RuntimeTypeHandle)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_RuntimeCompilerServices)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Runtime_CompilerServices_RuntimeHelpers_RunClassConstructor_System_Void_System_RuntimeType() { }
    // Purpose: Verify RuntimeHelpers.InitializeArray executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Runtime.CompilerServices.RuntimeHelpers::InitializeArray:System.Void(System.Array,System.RuntimeFieldHandle)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_RuntimeCompilerServices)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Runtime_CompilerServices_RuntimeHelpers_InitializeArray_System_Void_System_Array_System_Ru() { }
    // Purpose: Verify RuntimeHelpers.GetSubArray executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Runtime.CompilerServices.RuntimeHelpers::GetSubArray:T[](T[],System.Range)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_RuntimeCompilerServices)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Runtime_CompilerServices_RuntimeHelpers_GetSubArray_T_T_System_Range() { }
    // Purpose: Verify RuntimeHelpers.BoxEnum executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Runtime.CompilerServices.RuntimeHelpers::BoxEnum:System.Object(T)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_RuntimeCompilerServices)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Runtime_CompilerServices_RuntimeHelpers_BoxEnum_System_Object_T() { }
    // Purpose: Verify FormattableStringFactory.Create executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Runtime.CompilerServices.FormattableStringFactory::Create:System.Runtime.CompilerServices.FormattableString(System.String,System.Object[])")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_RuntimeCompilerServices)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Runtime_CompilerServices_FormattableStringFactory_Create_System_Runtime_CompilerServices_F() { _ = FormattableStringFactory.Create("hello", Array.Empty<System.Object>()); }
    // Purpose: Verify RuntimeWrappedException..ctor executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Runtime.CompilerServices.RuntimeWrappedException::.ctor:System.Void(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_RuntimeCompilerServices)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Runtime_CompilerServices_RuntimeWrappedException_ctor_System_Void_System_Object() { new RuntimeWrappedException(42); }
    // Purpose: Verify RuntimeWrappedException.get_WrappedException executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Runtime.CompilerServices.RuntimeWrappedException::get_WrappedException:System.Object()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_RuntimeCompilerServices)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Runtime_CompilerServices_RuntimeWrappedException_get_WrappedException_System_Object() { _ = new RuntimeWrappedException(42).WrappedException; }}
