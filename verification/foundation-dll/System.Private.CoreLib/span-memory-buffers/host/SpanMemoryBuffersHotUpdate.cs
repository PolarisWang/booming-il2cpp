using System;
using System.IO;
using System.Threading;
using System.Threading.Tasks;
using System.Globalization;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;
using System.Collections.Generic;
using System.Reflection;
using Chaos.TestFramework;

// Hot-update stubs for family/System.Private.CoreLib/span/memory-buffers
public static class SpanMemoryBuffersHotUpdate
{
    // Verify Span.get_Item after hot-update (patch side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Span`1::get_Item:T&(System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_SpanMemoryBuffers)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Span_1_get_Item_T_System_Int32() { _ = new Span<byte>(new byte[4])[0]; }
    // Verify Span.Slice after hot-update (patch side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Span`1::Slice:System.Span`1(System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_SpanMemoryBuffers)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Span_1_Slice_System_Span_1_System_Int32() { _ = new Span<byte>(new byte[4]).Slice(1).Length; }
    // Verify Span.Slice after hot-update (patch side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Span`1::Slice:System.Span`1(System.Int32,System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_SpanMemoryBuffers)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Span_1_Slice_System_Span_1_System_Int32_System_Int32() { _ = new Span<byte>(new byte[4]).Slice(1, 2).Length; }
    // Verify Span.ToArray after hot-update (patch side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Span`1::ToArray:T[](System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_SpanMemoryBuffers)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Span_1_ToArray_T_System_Int32() { _ = new byte[1].AsSpan().ToArray(); }
    // Verify Span.CopyTo after hot-update (patch side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Span`1::CopyTo:System.Void(System.Span`1)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_SpanMemoryBuffers)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Span_1_CopyTo_System_Void_System_Span_1() { default(Span<byte>).CopyTo(default(Span<byte>)); }
    // Verify Span.get_Empty after hot-update (patch side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Span`1::get_Empty:System.Span`1()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_SpanMemoryBuffers)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Span_1_get_Empty_System_Span_1() { _ = Span<byte>.Empty; }
    // Verify ReadOnlySpan.get_Item after hot-update (patch side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.ReadOnlySpan`1::get_Item:T&(System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_SpanMemoryBuffers)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_ReadOnlySpan_1_get_Item_T_System_Int32() { _ = new ReadOnlySpan<byte>(new byte[4])[0]; }
    // Verify ReadOnlySpan.Slice after hot-update (patch side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.ReadOnlySpan`1::Slice:System.ReadOnlySpan`1(System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_SpanMemoryBuffers)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_ReadOnlySpan_1_Slice_System_ReadOnlySpan_1_System_Int32() { _ = new ReadOnlySpan<byte>(new byte[4]).Slice(1).Length; }
    // Verify ReadOnlySpan.ToArray after hot-update (patch side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.ReadOnlySpan`1::ToArray:T[]()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_SpanMemoryBuffers)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_ReadOnlySpan_1_ToArray_T() { _ = default(ReadOnlySpan<byte>).ToArray(); }
    // Verify Memory.Slice after hot-update (patch side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Memory`1::Slice:System.Memory`1(System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_SpanMemoryBuffers)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Memory_1_Slice_System_Memory_1_System_Int32() { _ = new Memory<byte>(new byte[4]).Slice(1).Length; }
    // Verify Memory.get_Span after hot-update (patch side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Memory`1::get_Span:System.Span`1()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_SpanMemoryBuffers)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Memory_1_get_Span_System_Span_1() { _ = default(Memory<byte>).Span; }
    // Verify Memory.ToArray after hot-update (patch side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Memory`1::ToArray:T[]()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_SpanMemoryBuffers)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Memory_1_ToArray_T() { _ = default(Memory<byte>).ToArray(); }
    // Verify MemoryMarshal.GetReference after hot-update (patch side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Runtime.InteropServices.MemoryMarshal::GetReference:T&(System.ReadOnlySpan`1)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_SpanMemoryBuffers)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Runtime_InteropServices_MemoryMarshal_GetReference_T_System_ReadOnlySpan_1() { _ = MemoryMarshal.GetReference(default(ReadOnlySpan<byte>)); }
    // Verify MemoryMarshal.GetReference after hot-update (patch side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Runtime.InteropServices.MemoryMarshal::GetReference:T&(System.Span`1)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_SpanMemoryBuffers)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Runtime_InteropServices_MemoryMarshal_GetReference_T_System_Span_1() { _ = MemoryMarshal.GetReference(default(Span<byte>)); }}
