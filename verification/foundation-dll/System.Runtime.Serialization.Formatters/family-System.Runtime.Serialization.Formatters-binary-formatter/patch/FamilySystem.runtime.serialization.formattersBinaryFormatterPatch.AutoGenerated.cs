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

// Auto-generated patch-side skeletons for FamilySystem.runtime.serialization.formattersBinaryFormatterPatch.
// Each method exercises a method from the patch side back to the host after hot-update.
public static class FamilySystem.runtime.serialization.formattersBinaryFormatterPatch
{
    // Purpose: Verify BinaryFormatter.BinaryFormatter: executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.Formatters.Binary.BinaryFormatter::BinaryFormatter::Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Runtime_Serialization_Formatters_System_Runtime_Serialization_Formatters_Binary_BinaryFormatter_BinaryFormatter_V() { _ = BinaryFormatter.BinaryFormatter:(); }
    // Purpose: Verify BinaryFormatter.BinaryFormatter: executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.Formatters.Binary.BinaryFormatter::BinaryFormatter::Void(System.Object, System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Runtime_Serialization_Formatters_System_Runtime_Serialization_Formatters_Binary_BinaryFormatter_BinaryFormatter_V() { _ = BinaryFormatter.BinaryFormatter:(42, 42); }
    // Purpose: Verify BinaryFormatter.Deserialize executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.Formatters.Binary.BinaryFormatter::Deserialize:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Runtime_Serialization_Formatters_System_Runtime_Serialization_Formatters_Binary_BinaryFormatter_Deserialize_Syste() { BinaryFormatter.Deserialize(null!); }
    // Purpose: Verify BinaryFormatter.Serialize executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.Formatters.Binary.BinaryFormatter::Serialize:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Runtime_Serialization_Formatters_System_Runtime_Serialization_Formatters_Binary_BinaryFormatter_Serialize_System_() { BinaryFormatter.Serialize(null!, null!); }
    // Purpose: Verify BinaryFormatter.get_AssemblyFormat executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.Formatters.Binary.BinaryFormatter::get_AssemblyFormat:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Runtime_Serialization_Formatters_System_Runtime_Serialization_Formatters_Binary_BinaryFormatter_get_AssemblyForma() { BinaryFormatter.AssemblyFormat; }
    // Purpose: Verify BinaryFormatter.get_Binder executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.Formatters.Binary.BinaryFormatter::get_Binder:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Runtime_Serialization_Formatters_System_Runtime_Serialization_Formatters_Binary_BinaryFormatter_get_Binder_System() { BinaryFormatter.Binder; }
    // Purpose: Verify BinaryFormatter.get_Context executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.Formatters.Binary.BinaryFormatter::get_Context:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Runtime_Serialization_Formatters_System_Runtime_Serialization_Formatters_Binary_BinaryFormatter_get_Context_Syste() { BinaryFormatter.Context; }
    // Purpose: Verify BinaryFormatter.get_FilterLevel executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.Formatters.Binary.BinaryFormatter::get_FilterLevel:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Runtime_Serialization_Formatters_System_Runtime_Serialization_Formatters_Binary_BinaryFormatter_get_FilterLevel_S() { BinaryFormatter.FilterLevel; }
    // Purpose: Verify BinaryFormatter.get_SurrogateSelector executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.Formatters.Binary.BinaryFormatter::get_SurrogateSelector:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Runtime_Serialization_Formatters_System_Runtime_Serialization_Formatters_Binary_BinaryFormatter_get_SurrogateSele() { BinaryFormatter.SurrogateSelector; }
    // Purpose: Verify BinaryFormatter.get_TypeFormat executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.Formatters.Binary.BinaryFormatter::get_TypeFormat:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Runtime_Serialization_Formatters_System_Runtime_Serialization_Formatters_Binary_BinaryFormatter_get_TypeFormat_Sy() { BinaryFormatter.TypeFormat; }
    // Purpose: Verify BinaryFormatter.set_AssemblyFormat executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.Formatters.Binary.BinaryFormatter::set_AssemblyFormat:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Runtime_Serialization_Formatters_System_Runtime_Serialization_Formatters_Binary_BinaryFormatter_set_AssemblyForma() { BinaryFormatter.AssemblyFormat = null!; }
    // Purpose: Verify BinaryFormatter.set_Binder executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.Formatters.Binary.BinaryFormatter::set_Binder:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Runtime_Serialization_Formatters_System_Runtime_Serialization_Formatters_Binary_BinaryFormatter_set_Binder_System() { BinaryFormatter.Binder = null!; }
    // Purpose: Verify BinaryFormatter.set_Context executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.Formatters.Binary.BinaryFormatter::set_Context:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Runtime_Serialization_Formatters_System_Runtime_Serialization_Formatters_Binary_BinaryFormatter_set_Context_Syste() { BinaryFormatter.Context = null!; }
    // Purpose: Verify BinaryFormatter.set_FilterLevel executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.Formatters.Binary.BinaryFormatter::set_FilterLevel:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Runtime_Serialization_Formatters_System_Runtime_Serialization_Formatters_Binary_BinaryFormatter_set_FilterLevel_S() { BinaryFormatter.FilterLevel = null!; }
    // Purpose: Verify BinaryFormatter.set_SurrogateSelector executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.Formatters.Binary.BinaryFormatter::set_SurrogateSelector:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Runtime_Serialization_Formatters_System_Runtime_Serialization_Formatters_Binary_BinaryFormatter_set_SurrogateSele() { BinaryFormatter.SurrogateSelector = null!; }
    // Purpose: Verify BinaryFormatter.set_TypeFormat executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.Formatters.Binary.BinaryFormatter::set_TypeFormat:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Runtime_Serialization_Formatters_System_Runtime_Serialization_Formatters_Binary_BinaryFormatter_set_TypeFormat_Sy() { BinaryFormatter.TypeFormat = null!; }}
