// Auto-generated native-AOT entry point
// Family: family/System.Private.CoreLib/span/memory/buffers
// Assembly: System.Private.CoreLib
// Variant: semantic-patch

using System;
using System.Runtime.InteropServices;

public static class SpanMemoryBuffersSemanticPatchEntry
{
    public static int Run(int entryIndex)
    {
        switch (entryIndex)
        {
            case 0: return Method0();
            case 1: return Method1();
            case 2: return Method2();
            case 3: return Method3();
            case 4: return Method4();
            case 5: return Method5();
            case 6: return Method6();
            case 7: return Method7();
            case 8: return Method8();
            case 9: return Method9();
            case 10: return Method10();
            case 11: return Method11();
            case 12: return Method12();
            case 13: return Method13();
            default: return -1;
        }
    }

    // [0] System.Private.CoreLib/System.Span`1::get_Item:T&(System.Int32)
    static int Method0()
    {
        // TODO: System.Private.CoreLib/System.Span`1::get_Item:T&(System.Int32) could not be auto-generated for semantic-patch
        return 0;
    }

    // [1] System.Private.CoreLib/System.Span`1::Slice:System.Span`1(System.Int32)
    static int Method1()
    {
        // TODO: System.Private.CoreLib/System.Span`1::Slice:System.Span`1(System.Int32) could not be auto-generated for semantic-patch
        return 0;
    }

    // [2] System.Private.CoreLib/System.Span`1::Slice:System.Span`1(System.Int32,System.Int32)
    static int Method2()
    {
        // TODO: System.Private.CoreLib/System.Span`1::Slice:System.Span`1(System.Int32,System.Int32) could not be auto-generated for semantic-patch
        return 0;
    }

    // [3] System.Private.CoreLib/System.Span`1::ToArray:T[](System.Int32)
    static int Method3()
    {
        // TODO: System.Private.CoreLib/System.Span`1::ToArray:T[](System.Int32) could not be auto-generated for semantic-patch
        return 0;
    }

    // [4] System.Private.CoreLib/System.Span`1::CopyTo:System.Void(System.Span`1)
    static int Method4()
    {
        new Span<byte>(new byte[] { 9, 8, 7 }).CopyTo(new Span<byte>(new byte[] { 9, 8, 7 }));
        return (int)0;
    }

    // [5] System.Private.CoreLib/System.Span`1::get_Empty:System.Span`1()
    static int Method5()
    {
        return Span<byte>.Empty.GetHashCode();
    }

    // [6] System.Private.CoreLib/System.ReadOnlySpan`1::get_Item:T&(System.Int32)
    static int Method6()
    {
        // TODO: System.Private.CoreLib/System.ReadOnlySpan`1::get_Item:T&(System.Int32) could not be auto-generated for semantic-patch
        return 0;
    }

    // [7] System.Private.CoreLib/System.ReadOnlySpan`1::Slice:System.ReadOnlySpan`1(System.Int32)
    static int Method7()
    {
        // TODO: System.Private.CoreLib/System.ReadOnlySpan`1::Slice:System.ReadOnlySpan`1(System.Int32) could not be auto-generated for semantic-patch
        return 0;
    }

    // [8] System.Private.CoreLib/System.ReadOnlySpan`1::ToArray:T[]()
    static int Method8()
    {
        return new ReadOnlySpan<byte>(new byte[] { 9, 8, 7 }).ToArray().GetHashCode();
    }

    // [9] System.Private.CoreLib/System.Memory`1::Slice:System.Memory`1(System.Int32)
    static int Method9()
    {
        // TODO: System.Private.CoreLib/System.Memory`1::Slice:System.Memory`1(System.Int32) could not be auto-generated for semantic-patch
        return 0;
    }

    // [10] System.Private.CoreLib/System.Memory`1::get_Span:System.Span`1()
    static int Method10()
    {
        return new Memory<byte>(new byte[] { 9, 8, 7 }).Span.GetHashCode();
    }

    // [11] System.Private.CoreLib/System.Memory`1::ToArray:T[]()
    static int Method11()
    {
        return new Memory<byte>(new byte[] { 9, 8, 7 }).ToArray().GetHashCode();
    }

    // [12] System.Private.CoreLib/System.Runtime.InteropServices.MemoryMarshal::GetReference:T&(System.ReadOnlySpan`1)
    static int Method12()
    {
        return MemoryMarshal.GetReference(new ReadOnlySpan<byte>(new byte[] { 9, 8, 7 })).GetHashCode();
    }

    // [13] System.Private.CoreLib/System.Runtime.InteropServices.MemoryMarshal::GetReference:T&(System.Span`1)
    static int Method13()
    {
        return MemoryMarshal.GetReference(new Span<byte>(new byte[] { 9, 8, 7 })).GetHashCode();
    }

}