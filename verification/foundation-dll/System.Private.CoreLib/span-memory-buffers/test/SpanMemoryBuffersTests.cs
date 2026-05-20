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
using Xunit;

[CapabilityTest("family/System.Private.CoreLib/span/memory-buffers", IncludeBenchmark = true, IncludeHotUpdate = true)]
public partial class SpanMemoryBuffersTests
{
    // AI-generated tests for family/System.Private.CoreLib/span/memory-buffers

    // === mixed (smoke) ===

    [Fact]
    public void _System_Private_CoreLib_System_Span_1_get_Item_T_System_Int32()
    {
        // Purpose: Smoke — Span.get_Item with complex param(s)
            _ = new Span<byte>(new byte[4])[0];
    }

    [Fact]
    public void _System_Private_CoreLib_System_Span_1_Slice_System_Span_1_System_Int32()
    {
        // Purpose: Smoke — Span.Slice with complex param(s)
            _ = new Span<byte>(new byte[4]).Slice(1).Length;
    }

    [Fact]
    public void _System_Private_CoreLib_System_Span_1_Slice_System_Span_1_System_Int32_System_Int32()
    {
        // Purpose: Smoke — Span.Slice with complex param(s)
            _ = new Span<byte>(new byte[4]).Slice(1, 2).Length;
    }

    [Fact]
    public void _System_Private_CoreLib_System_Span_1_ToArray_T_System_Int32()
    {
        // Purpose: Smoke — Span.ToArray with complex param(s)
            _ = new byte[1].AsSpan().ToArray();
    }

    [Fact]
    public void _System_Private_CoreLib_System_Span_1_CopyTo_System_Void_System_Span_1()
    {
        // Purpose: Smoke — Span.CopyTo with complex param(s)
            default(Span<byte>).CopyTo(default(Span<byte>));
    }

    [Fact]
    public void _System_Private_CoreLib_System_Span_1_get_Empty_System_Span_1()
    {
        // Purpose: Smoke — Span.get_Empty with complex param(s)
            _ = Span<byte>.Empty;
    }

    [Fact]
    public void _System_Private_CoreLib_System_ReadOnlySpan_1_get_Item_T_System_Int32()
    {
        // Purpose: Smoke — ReadOnlySpan.get_Item with complex param(s)
            _ = new ReadOnlySpan<byte>(new byte[4])[0];
    }

    [Fact]
    public void _System_Private_CoreLib_System_ReadOnlySpan_1_Slice_System_ReadOnlySpan_1_System_Int32()
    {
        // Purpose: Smoke — ReadOnlySpan.Slice with complex param(s)
            _ = new ReadOnlySpan<byte>(new byte[4]).Slice(1).Length;
    }

    [Fact]
    public void _System_Private_CoreLib_System_ReadOnlySpan_1_ToArray_T()
    {
        // Purpose: Smoke — ReadOnlySpan.ToArray with complex param(s)
            _ = default(ReadOnlySpan<byte>).ToArray();
    }

    [Fact]
    public void _System_Private_CoreLib_System_Memory_1_Slice_System_Memory_1_System_Int32()
    {
        // Purpose: Smoke — Memory.Slice with complex param(s)
            _ = new Memory<byte>(new byte[4]).Slice(1).Length;
    }

    [Fact]
    public void _System_Private_CoreLib_System_Memory_1_get_Span_System_Span_1()
    {
        // Purpose: Smoke — Memory.get_Span with complex param(s)
            _ = default(Memory<byte>).Span;
    }

    [Fact]
    public void _System_Private_CoreLib_System_Memory_1_ToArray_T()
    {
        // Purpose: Smoke — Memory.ToArray with complex param(s)
            _ = default(Memory<byte>).ToArray();
    }

    [Fact]
    public void _System_Private_CoreLib_System_Runtime_InteropServices_MemoryMarshal_GetReference_T_System_ReadOnlySpan_1()
    {
        // Purpose: Smoke — MemoryMarshal.GetReference with complex param(s)
            _ = MemoryMarshal.GetReference(default(ReadOnlySpan<byte>));
    }

    [Fact]
    public void _System_Private_CoreLib_System_Runtime_InteropServices_MemoryMarshal_GetReference_T_System_Span_1()
    {
        // Purpose: Smoke — MemoryMarshal.GetReference with complex param(s)
            _ = MemoryMarshal.GetReference(default(Span<byte>));
    }
}
