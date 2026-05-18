// Auto-generated native-AOT entry point
// Family: family/System.Private.CoreLib/span/memory/buffers
// Assembly: System.Private.CoreLib
// Variant: subjects

using System;
using System.Runtime.InteropServices;

public static partial class SpanMemoryBuffersSubjects
{
    // Inlined exit code — avoids SDK method call resolution in codegen
    public static int _exitCode;

    // [0] System.Private.CoreLib/System.Span`1::get_Item:T&(System.Int32)
    public static void Subject_0()
    {
        // TODO: System.Private.CoreLib/System.Span`1::get_Item:T&(System.Int32) could not be auto-generated
    }

    // [1] System.Private.CoreLib/System.Span`1::Slice:System.Span`1(System.Int32)
    public static void Subject_1()
    {
        // TODO: System.Private.CoreLib/System.Span`1::Slice:System.Span`1(System.Int32) could not be auto-generated
    }

    // [2] System.Private.CoreLib/System.Span`1::Slice:System.Span`1(System.Int32,System.Int32)
    public static void Subject_2()
    {
        // TODO: System.Private.CoreLib/System.Span`1::Slice:System.Span`1(System.Int32,System.Int32) could not be auto-generated
    }

    // [3] System.Private.CoreLib/System.Span`1::ToArray:T[](System.Int32)
    public static void Subject_3()
    {
        // TODO: System.Private.CoreLib/System.Span`1::ToArray:T[](System.Int32) could not be auto-generated
    }

    // [4] System.Private.CoreLib/System.Span`1::CopyTo:System.Void(System.Span`1)
    public static void Subject_4()
    {
        default(Span<byte>).CopyTo(default(Span<byte>));
    }

    // [5] System.Private.CoreLib/System.Span`1::get_Empty:System.Span`1()
    public static void Subject_5()
    {
        try { _ = Span<byte>.Empty; _exitCode = 1; }
        catch (NotSupportedException) { }
    }

    // [6] System.Private.CoreLib/System.ReadOnlySpan`1::get_Item:T&(System.Int32)
    public static void Subject_6()
    {
        // TODO: System.Private.CoreLib/System.ReadOnlySpan`1::get_Item:T&(System.Int32) could not be auto-generated
    }

    // [7] System.Private.CoreLib/System.ReadOnlySpan`1::Slice:System.ReadOnlySpan`1(System.Int32)
    public static void Subject_7()
    {
        // TODO: System.Private.CoreLib/System.ReadOnlySpan`1::Slice:System.ReadOnlySpan`1(System.Int32) could not be auto-generated
    }

    // [8] System.Private.CoreLib/System.ReadOnlySpan`1::ToArray:T[]()
    public static void Subject_8()
    {
        if (((default(ReadOnlySpan<byte>).ToArray()).Length) != 0) _exitCode = 1;
    }

    // [9] System.Private.CoreLib/System.Memory`1::Slice:System.Memory`1(System.Int32)
    public static void Subject_9()
    {
        // TODO: System.Private.CoreLib/System.Memory`1::Slice:System.Memory`1(System.Int32) could not be auto-generated
    }

    // [10] System.Private.CoreLib/System.Memory`1::get_Span:System.Span`1()
    public static void Subject_10()
    {
        try { _ = default(Memory<byte>).Span; _exitCode = 1; }
        catch (NotSupportedException) { }
    }

    // [11] System.Private.CoreLib/System.Memory`1::ToArray:T[]()
    public static void Subject_11()
    {
        if (((default(Memory<byte>).ToArray()).Length) != 0) _exitCode = 1;
    }

    // [12] System.Private.CoreLib/System.Runtime.InteropServices.MemoryMarshal::GetReference:T&(System.ReadOnlySpan`1)
    public static void Subject_12()
    {
        try { MemoryMarshal.GetReference(default(ReadOnlySpan<byte>)); _exitCode = 1; }
        catch (NullReferenceException) { }
    }

    // [13] System.Private.CoreLib/System.Runtime.InteropServices.MemoryMarshal::GetReference:T&(System.Span`1)
    public static void Subject_13()
    {
        try { MemoryMarshal.GetReference(default(Span<byte>)); _exitCode = 1; }
        catch (NullReferenceException) { }
    }

}