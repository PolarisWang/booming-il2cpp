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
        if (((Span<byte>.Empty).GetHashCode()) != ((Span<byte>.Empty).GetHashCode())) _exitCode = 1;
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
        if (((default(ReadOnlySpan<byte>).ToArray()).Length) != ((default(ReadOnlySpan<byte>).ToArray()).Length)) _exitCode = 1;
    }

    // [9] System.Private.CoreLib/System.Memory`1::Slice:System.Memory`1(System.Int32)
    public static void Subject_9()
    {
        // TODO: System.Private.CoreLib/System.Memory`1::Slice:System.Memory`1(System.Int32) could not be auto-generated
    }

    // [10] System.Private.CoreLib/System.Memory`1::get_Span:System.Span`1()
    public static void Subject_10()
    {
        if (((default(Memory<byte>).Span).GetHashCode()) != ((default(Memory<byte>).Span).GetHashCode())) _exitCode = 1;
    }

    // [11] System.Private.CoreLib/System.Memory`1::ToArray:T[]()
    public static void Subject_11()
    {
        if (((default(Memory<byte>).ToArray()).Length) != ((default(Memory<byte>).ToArray()).Length)) _exitCode = 1;
    }

    // [12] System.Private.CoreLib/System.Runtime.InteropServices.MemoryMarshal::GetReference:T&(System.ReadOnlySpan`1)
    public static void Subject_12()
    {
        if ((int)(MemoryMarshal.GetReference(default(ReadOnlySpan<byte>))) != (int)(MemoryMarshal.GetReference(default(ReadOnlySpan<byte>)))) _exitCode = 1;
    }

    // [13] System.Private.CoreLib/System.Runtime.InteropServices.MemoryMarshal::GetReference:T&(System.Span`1)
    public static void Subject_13()
    {
        if ((int)(MemoryMarshal.GetReference(default(Span<byte>))) != (int)(MemoryMarshal.GetReference(default(Span<byte>)))) _exitCode = 1;
    }

    public static void Run(int entryIndex)
    {
        switch (entryIndex)
        {
            case 0: Subject_0(); break;
            case 1: Subject_1(); break;
            case 2: Subject_2(); break;
            case 3: Subject_3(); break;
            case 4: Subject_4(); break;
            case 5: Subject_5(); break;
            case 6: Subject_6(); break;
            case 7: Subject_7(); break;
            case 8: Subject_8(); break;
            case 9: Subject_9(); break;
            case 10: Subject_10(); break;
            case 11: Subject_11(); break;
            case 12: Subject_12(); break;
            case 13: Subject_13(); break;
        }
    }

}