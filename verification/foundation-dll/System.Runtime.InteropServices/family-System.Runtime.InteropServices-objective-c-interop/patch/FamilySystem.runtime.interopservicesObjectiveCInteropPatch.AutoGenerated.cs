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

// Auto-generated patch-side skeletons for FamilySystem.runtime.interopservicesObjectiveCInteropPatch.
// Each method exercises a method from the patch side back to the host after hot-update.
public static class FamilySystem.runtime.interopservicesObjectiveCInteropPatch
{
    // Purpose: Verify ObjectiveCMarshal.CreateReferenceTrackingHandle executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ObjectiveC.ObjectiveCMarshal::CreateReferenceTrackingHandle:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Runtime_InteropServices_System_Runtime_InteropServices_ObjectiveC_ObjectiveCMarshal_CreateReferenceTrackingHandle() { ObjectiveCMarshal.CreateReferenceTrackingHandle(null!, null!); }
    // Purpose: Verify ObjectiveCMarshal.Initialize executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ObjectiveC.ObjectiveCMarshal::Initialize:System.Void(P0, P1, P2, P3)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Runtime_InteropServices_System_Runtime_InteropServices_ObjectiveC_ObjectiveCMarshal_Initialize_System_Void_P0_P1_() { ObjectiveCMarshal.Initialize(null!, null!, null!, null!); }
    // Purpose: Verify ObjectiveCMarshal.SetMessageSendCallback executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ObjectiveC.ObjectiveCMarshal::SetMessageSendCallback:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Runtime_InteropServices_System_Runtime_InteropServices_ObjectiveC_ObjectiveCMarshal_SetMessageSendCallback_System() { ObjectiveCMarshal.SetMessageSendCallback(null!, null!); }
    // Purpose: Verify ObjectiveCMarshal.SetMessageSendPendingException executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ObjectiveC.ObjectiveCMarshal::SetMessageSendPendingException:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Runtime_InteropServices_System_Runtime_InteropServices_ObjectiveC_ObjectiveCMarshal_SetMessageSendPendingExceptio() { ObjectiveCMarshal.SetMessageSendPendingException(null!); }
    // Purpose: Verify ObjectiveCMarshal+UnhandledExceptionPropagationHandler.ObjectiveCMarshal+UnhandledExceptionPropagationHandler: executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ObjectiveC.ObjectiveCMarshal+UnhandledExceptionPropagationHandler::ObjectiveCMarshal+UnhandledExceptionPropagationHandler::Void(System.Object, System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Runtime_InteropServices_System_Runtime_InteropServices_ObjectiveC_ObjectiveCMarshal_UnhandledExceptionPropagation() { _ = ObjectiveCMarshal+UnhandledExceptionPropagationHandler.ObjectiveCMarshal+UnhandledExceptionPropagationHandler:(42, 42); }
    // Purpose: Verify ObjectiveCMarshal+UnhandledExceptionPropagationHandler.BeginInvoke executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ObjectiveC.ObjectiveCMarshal+UnhandledExceptionPropagationHandler::BeginInvoke:System.Void(P0, P1, P2, P3, P4)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Runtime_InteropServices_System_Runtime_InteropServices_ObjectiveC_ObjectiveCMarshal_UnhandledExceptionPropagation() { ObjectiveCMarshal+UnhandledExceptionPropagationHandler.BeginInvoke(null!, null!, null!, null!, null!); }
    // Purpose: Verify ObjectiveCMarshal+UnhandledExceptionPropagationHandler.EndInvoke executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ObjectiveC.ObjectiveCMarshal+UnhandledExceptionPropagationHandler::EndInvoke:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Runtime_InteropServices_System_Runtime_InteropServices_ObjectiveC_ObjectiveCMarshal_UnhandledExceptionPropagation() { ObjectiveCMarshal+UnhandledExceptionPropagationHandler.EndInvoke(null!, null!); }
    // Purpose: Verify ObjectiveCMarshal+UnhandledExceptionPropagationHandler.Invoke executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ObjectiveC.ObjectiveCMarshal+UnhandledExceptionPropagationHandler::Invoke:System.Void(P0, P1, P2)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Runtime_InteropServices_System_Runtime_InteropServices_ObjectiveC_ObjectiveCMarshal_UnhandledExceptionPropagation() { ObjectiveCMarshal+UnhandledExceptionPropagationHandler.Invoke(null!, null!, null!); }
    // Purpose: Verify ObjectiveCTrackedTypeAttribute.ObjectiveCTrackedTypeAttribute: executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ObjectiveC.ObjectiveCTrackedTypeAttribute::ObjectiveCTrackedTypeAttribute::Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Runtime_InteropServices_System_Runtime_InteropServices_ObjectiveC_ObjectiveCTrackedTypeAttribute_ObjectiveCTracke() { _ = ObjectiveCTrackedTypeAttribute.ObjectiveCTrackedTypeAttribute:(); }}
