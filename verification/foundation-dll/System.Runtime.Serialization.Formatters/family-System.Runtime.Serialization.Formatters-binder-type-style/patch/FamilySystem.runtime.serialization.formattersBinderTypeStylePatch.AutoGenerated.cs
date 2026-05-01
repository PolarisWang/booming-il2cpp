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

// Auto-generated patch-side skeletons for FamilySystem.runtime.serialization.formattersBinderTypeStylePatch.
// Each method exercises a method from the patch side back to the host after hot-update.
public static class FamilySystem.runtime.serialization.formattersBinderTypeStylePatch
{
    // Purpose: Verify SerializationBinder.SerializationBinder: executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.SerializationBinder::SerializationBinder::Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Runtime_Serialization_Formatters_System_Runtime_Serialization_SerializationBinder_SerializationBinder_Void() { _ = SerializationBinder.SerializationBinder:(); }
    // Purpose: Verify SerializationBinder.BindToName executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.SerializationBinder::BindToName:System.Void(P0, P1, P2)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Runtime_Serialization_Formatters_System_Runtime_Serialization_SerializationBinder_BindToName_System_Void_P0_P1_P2() { SerializationBinder.BindToName(null!, null!, null!); }
    // Purpose: Verify SerializationBinder.BindToType executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.SerializationBinder::BindToType:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Runtime_Serialization_Formatters_System_Runtime_Serialization_SerializationBinder_BindToType_System_Void_P0_P1() { SerializationBinder.BindToType(null!, null!); }}
