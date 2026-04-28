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
using Xunit;

// Auto-generated skeleton for Span Memory Buffers (family/System.Private.CoreLib/span/memory-buffers).
public partial class SpanMemoryBuffersTests
{
    [MethodSubjectId("System.Private.CoreLib/System.Span`1::get_Item:T&(System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_SpanMemoryBuffers)]
    [VerificationRoute(VerificationRoute.Native)]
    [Fact(Skip = "needs-manual — get_Item with 1 params requires manual implementation")]
    public void Method_System_Private_CoreLib_System_Span_1_get_Item_T_System_Int32()
    {
    // TODO: needs-manual — get_Item with 1 params requires manual implementation
    }

    [MethodSubjectId("System.Private.CoreLib/System.Span`1::Slice:System.Span`1(System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_SpanMemoryBuffers)]
    [VerificationRoute(VerificationRoute.Native)]
    [Fact(Skip = "needs-manual — Slice with 1 params requires manual implementation")]
    public void Method_System_Private_CoreLib_System_Span_1_Slice_System_Span_1_System_Int32()
    {
    // TODO: needs-manual — Slice with 1 params requires manual implementation
    }

    [MethodSubjectId("System.Private.CoreLib/System.Span`1::Slice:System.Span`1(System.Int32,System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_SpanMemoryBuffers)]
    [VerificationRoute(VerificationRoute.Native)]
    [Fact(Skip = "needs-manual — Slice with 2 params requires manual implementation")]
    public void Method_System_Private_CoreLib_System_Span_1_Slice_System_Span_1_System_Int32_System_Int32()
    {
    // TODO: needs-manual — Slice with 2 params requires manual implementation
    }

    [MethodSubjectId("System.Private.CoreLib/System.Span`1::ToArray:T[](System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_SpanMemoryBuffers)]
    [VerificationRoute(VerificationRoute.Native)]
    [Fact(Skip = "needs-manual — ToArray with 1 params requires manual implementation")]
    public void Method_System_Private_CoreLib_System_Span_1_ToArray_T_System_Int32()
    {
    // TODO: needs-manual — ToArray with 1 params requires manual implementation
    }

    [MethodSubjectId("System.Private.CoreLib/System.Span`1::CopyTo:System.Void(System.Span`1)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_SpanMemoryBuffers)]
    [VerificationRoute(VerificationRoute.Native)]
    [Fact]
    public void Method_System_Private_CoreLib_System_Span_1_CopyTo_System_Void_System_Span_1()
    {
    default(Span<byte>).CopyTo(default(Span<byte>));
    }

    [MethodSubjectId("System.Private.CoreLib/System.Span`1::get_Empty:System.Span`1()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_SpanMemoryBuffers)]
    [VerificationRoute(VerificationRoute.Native)]
    [Fact]
    public void Method_System_Private_CoreLib_System_Span_1_get_Empty_System_Span_1()
    {
    _ = Span<byte>.Empty;
    }

    [MethodSubjectId("System.Private.CoreLib/System.ReadOnlySpan`1::get_Item:T&(System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_SpanMemoryBuffers)]
    [VerificationRoute(VerificationRoute.Native)]
    [Fact(Skip = "needs-manual — get_Item with 1 params requires manual implementation")]
    public void Method_System_Private_CoreLib_System_ReadOnlySpan_1_get_Item_T_System_Int32()
    {
    // TODO: needs-manual — get_Item with 1 params requires manual implementation
    }

    [MethodSubjectId("System.Private.CoreLib/System.ReadOnlySpan`1::Slice:System.ReadOnlySpan`1(System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_SpanMemoryBuffers)]
    [VerificationRoute(VerificationRoute.Native)]
    [Fact(Skip = "needs-manual — Slice with 1 params requires manual implementation")]
    public void Method_System_Private_CoreLib_System_ReadOnlySpan_1_Slice_System_ReadOnlySpan_1_System_Int32()
    {
    // TODO: needs-manual — Slice with 1 params requires manual implementation
    }

    [MethodSubjectId("System.Private.CoreLib/System.ReadOnlySpan`1::ToArray:T[]()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_SpanMemoryBuffers)]
    [VerificationRoute(VerificationRoute.Native)]
    [Fact]
    public void Method_System_Private_CoreLib_System_ReadOnlySpan_1_ToArray_T()
    {
    _ = default(ReadOnlySpan<byte>).ToArray();
    }

    [MethodSubjectId("System.Private.CoreLib/System.Memory`1::Slice:System.Memory`1(System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_SpanMemoryBuffers)]
    [VerificationRoute(VerificationRoute.Native)]
    [Fact(Skip = "needs-manual — Slice with 1 params requires manual implementation")]
    public void Method_System_Private_CoreLib_System_Memory_1_Slice_System_Memory_1_System_Int32()
    {
    // TODO: needs-manual — Slice with 1 params requires manual implementation
    }

    [MethodSubjectId("System.Private.CoreLib/System.Memory`1::get_Span:System.Span`1()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_SpanMemoryBuffers)]
    [VerificationRoute(VerificationRoute.Native)]
    [Fact]
    public void Method_System_Private_CoreLib_System_Memory_1_get_Span_System_Span_1()
    {
    _ = default(Memory<byte>).Span;
    }

    [MethodSubjectId("System.Private.CoreLib/System.Memory`1::ToArray:T[]()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_SpanMemoryBuffers)]
    [VerificationRoute(VerificationRoute.Native)]
    [Fact]
    public void Method_System_Private_CoreLib_System_Memory_1_ToArray_T()
    {
    _ = default(Memory<byte>).ToArray();
    }

    [MethodSubjectId("System.Private.CoreLib/System.Runtime.InteropServices.MemoryMarshal::GetReference:T&(System.ReadOnlySpan`1)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_SpanMemoryBuffers)]
    [VerificationRoute(VerificationRoute.Native)]
    [Fact]
    public void Method_System_Private_CoreLib_System_Runtime_InteropServices_MemoryMarshal_GetReference_T_System_ReadOnlySpan_1()
    {
    _ = MemoryMarshal.GetReference(default(ReadOnlySpan<byte>));
    }

    [MethodSubjectId("System.Private.CoreLib/System.Runtime.InteropServices.MemoryMarshal::GetReference:T&(System.Span`1)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_SpanMemoryBuffers)]
    [VerificationRoute(VerificationRoute.Native)]
    [Fact]
    public void Method_System_Private_CoreLib_System_Runtime_InteropServices_MemoryMarshal_GetReference_T_System_Span_1()
    {
    _ = MemoryMarshal.GetReference(default(Span<byte>));
    }}
