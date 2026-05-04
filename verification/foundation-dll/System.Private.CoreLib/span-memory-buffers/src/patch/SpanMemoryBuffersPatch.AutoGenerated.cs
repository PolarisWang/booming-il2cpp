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

// Auto-generated patch-side skeletons for SpanMemoryBuffersPatch.
// Each method exercises a method from the patch side back to the host after hot-update.
public static partial class SpanMemoryBuffersPatch
{
    // Purpose: Verify Span.get_Item executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Private.CoreLib/System.Span`1::get_Item:T&(System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_SpanMemoryBuffers)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Span_1_get_Item_T_System_Int32() { }
    // Purpose: Verify Span.Slice executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Private.CoreLib/System.Span`1::Slice:System.Span`1(System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_SpanMemoryBuffers)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Span_1_Slice_System_Span_1_System_Int32() { }
    // Purpose: Verify Span.Slice executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Private.CoreLib/System.Span`1::Slice:System.Span`1(System.Int32,System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_SpanMemoryBuffers)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Span_1_Slice_System_Span_1_System_Int32_System_Int32() { }
    // Purpose: Verify Span.ToArray executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Private.CoreLib/System.Span`1::ToArray:T[](System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_SpanMemoryBuffers)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Span_1_ToArray_T_System_Int32() { }
    // Purpose: Verify Span.CopyTo executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Private.CoreLib/System.Span`1::CopyTo:System.Void(System.Span`1)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_SpanMemoryBuffers)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Span_1_CopyTo_System_Void_System_Span_1() { default(Span<byte>).CopyTo(default(Span<byte>)); }
    // Purpose: Verify Span.get_Empty executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Private.CoreLib/System.Span`1::get_Empty:System.Span`1()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_SpanMemoryBuffers)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Span_1_get_Empty_System_Span_1() { _ = Span<byte>.Empty; }
    // Purpose: Verify ReadOnlySpan.get_Item executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Private.CoreLib/System.ReadOnlySpan`1::get_Item:T&(System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_SpanMemoryBuffers)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_ReadOnlySpan_1_get_Item_T_System_Int32() { }
    // Purpose: Verify ReadOnlySpan.Slice executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Private.CoreLib/System.ReadOnlySpan`1::Slice:System.ReadOnlySpan`1(System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_SpanMemoryBuffers)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_ReadOnlySpan_1_Slice_System_ReadOnlySpan_1_System_Int32() { }
    // Purpose: Verify ReadOnlySpan.ToArray executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Private.CoreLib/System.ReadOnlySpan`1::ToArray:T[]()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_SpanMemoryBuffers)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_ReadOnlySpan_1_ToArray_T() { _ = default(ReadOnlySpan<byte>).ToArray(); }
    // Purpose: Verify Memory.Slice executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Private.CoreLib/System.Memory`1::Slice:System.Memory`1(System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_SpanMemoryBuffers)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Memory_1_Slice_System_Memory_1_System_Int32() { }
    // Purpose: Verify Memory.get_Span executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Private.CoreLib/System.Memory`1::get_Span:System.Span`1()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_SpanMemoryBuffers)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Memory_1_get_Span_System_Span_1() { _ = default(Memory<byte>).Span; }
    // Purpose: Verify Memory.ToArray executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Private.CoreLib/System.Memory`1::ToArray:T[]()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_SpanMemoryBuffers)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Memory_1_ToArray_T() { _ = default(Memory<byte>).ToArray(); }
    // Purpose: Verify MemoryMarshal.GetReference executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Private.CoreLib/System.Runtime.InteropServices.MemoryMarshal::GetReference:T&(System.ReadOnlySpan`1)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_SpanMemoryBuffers)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Runtime_InteropServices_MemoryMarshal_GetReference_T_System_ReadOnlySpan_1() { _ = MemoryMarshal.GetReference(default(ReadOnlySpan<byte>)); }
    // Purpose: Verify MemoryMarshal.GetReference executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Private.CoreLib/System.Runtime.InteropServices.MemoryMarshal::GetReference:T&(System.Span`1)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_SpanMemoryBuffers)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Runtime_InteropServices_MemoryMarshal_GetReference_T_System_Span_1() { _ = MemoryMarshal.GetReference(default(Span<byte>)); }}
