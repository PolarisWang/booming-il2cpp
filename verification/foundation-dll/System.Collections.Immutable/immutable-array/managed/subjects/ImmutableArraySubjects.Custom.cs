// Handwritten custom entries for immutable-array subjects.
// ImmutableArray<T> is a struct — cannot use (int) cast in auto-generated code.

using System;
using System.Collections.Immutable;

public static partial class ImmutableArraySubjects
{
    // [0] ImmutableArray.Create<Byte>() — empty array
    public static void CustomEntrySubject_0()
    {
        try { var arr = ImmutableArray.Create<byte>(); if (arr.Length != 0) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [1] ImmutableArray.Create<Byte>(Byte) — 1 element
    public static void CustomEntrySubject_1()
    {
        try { var arr = ImmutableArray.Create<byte>((byte)42); if (arr.Length != 1) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [2] ImmutableArray.Create<Byte>(Byte, Byte) — 2 elements
    public static void CustomEntrySubject_2()
    {
        try { var arr = ImmutableArray.Create<byte>((byte)42, (byte)43); if (arr.Length != 2) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [3] ImmutableArray.Create<Int32>() — empty array
    public static void CustomEntrySubject_3()
    {
        try { var arr = ImmutableArray.Create<int>(); if (arr.Length != 0) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [4] ImmutableArray.Create<Int32>(Int32) — 1 element
    public static void CustomEntrySubject_4()
    {
        try { var arr = ImmutableArray.Create<int>(42); if (arr.Length != 1) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [5] ImmutableArray.Create<String>(String) — 1 element
    public static void CustomEntrySubject_5()
    {
        try { var arr = ImmutableArray.Create<string>("hello"); if (arr.Length != 1) _exitCode = 1; }
        catch { _exitCode = 1; }
    }
}
