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

// Auto-generated patch-side skeletons for FamilySystem.runtime.serialization.formattersSurrogateSelectionPatch.
// Each method exercises a method from the patch side back to the host after hot-update.
public static class FamilySystem.runtime.serialization.formattersSurrogateSelectionPatch
{
    // Purpose: Verify ISerializationSurrogate.GetObjectData executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.ISerializationSurrogate::GetObjectData:System.Void(P0, P1, P2)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Runtime_Serialization_Formatters_System_Runtime_Serialization_ISerializationSurrogate_GetObjectData_System_Void_P() { ISerializationSurrogate.GetObjectData(null!, null!, null!); }
    // Purpose: Verify ISerializationSurrogate.SetObjectData executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.ISerializationSurrogate::SetObjectData:System.Void(P0, P1, P2, P3)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Runtime_Serialization_Formatters_System_Runtime_Serialization_ISerializationSurrogate_SetObjectData_System_Void_P() { ISerializationSurrogate.SetObjectData(null!, null!, null!, null!); }
    // Purpose: Verify ISurrogateSelector.ChainSelector executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.ISurrogateSelector::ChainSelector:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Runtime_Serialization_Formatters_System_Runtime_Serialization_ISurrogateSelector_ChainSelector_System_Void_P0() { ISurrogateSelector.ChainSelector(null!); }
    // Purpose: Verify ISurrogateSelector.GetNextSelector executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.ISurrogateSelector::GetNextSelector:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Runtime_Serialization_Formatters_System_Runtime_Serialization_ISurrogateSelector_GetNextSelector_System_Void() { ISurrogateSelector.GetNextSelector(); }
    // Purpose: Verify ISurrogateSelector.GetSurrogate executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.ISurrogateSelector::GetSurrogate:System.Void(P0, P1, P2)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Runtime_Serialization_Formatters_System_Runtime_Serialization_ISurrogateSelector_GetSurrogate_System_Void_P0_P1_P() { ISurrogateSelector.GetSurrogate(null!, null!, null!); }
    // Purpose: Verify SurrogateSelector.SurrogateSelector: executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.SurrogateSelector::SurrogateSelector::Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Runtime_Serialization_Formatters_System_Runtime_Serialization_SurrogateSelector_SurrogateSelector_Void() { _ = SurrogateSelector.SurrogateSelector:(); }
    // Purpose: Verify SurrogateSelector.AddSurrogate executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.SurrogateSelector::AddSurrogate:System.Void(P0, P1, P2)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Runtime_Serialization_Formatters_System_Runtime_Serialization_SurrogateSelector_AddSurrogate_System_Void_P0_P1_P2() { SurrogateSelector.AddSurrogate(null!, null!, null!); }
    // Purpose: Verify SurrogateSelector.ChainSelector executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.SurrogateSelector::ChainSelector:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Runtime_Serialization_Formatters_System_Runtime_Serialization_SurrogateSelector_ChainSelector_System_Void_P0() { SurrogateSelector.ChainSelector(null!); }
    // Purpose: Verify SurrogateSelector.GetNextSelector executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.SurrogateSelector::GetNextSelector:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Runtime_Serialization_Formatters_System_Runtime_Serialization_SurrogateSelector_GetNextSelector_System_Void() { SurrogateSelector.GetNextSelector(); }
    // Purpose: Verify SurrogateSelector.GetSurrogate executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.SurrogateSelector::GetSurrogate:System.Void(P0, P1, P2)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Runtime_Serialization_Formatters_System_Runtime_Serialization_SurrogateSelector_GetSurrogate_System_Void_P0_P1_P2() { SurrogateSelector.GetSurrogate(null!, null!, null!); }
    // Purpose: Verify SurrogateSelector.RemoveSurrogate executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.SurrogateSelector::RemoveSurrogate:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Runtime_Serialization_Formatters_System_Runtime_Serialization_SurrogateSelector_RemoveSurrogate_System_Void_P0_P1() { SurrogateSelector.RemoveSurrogate(null!, null!); }}
