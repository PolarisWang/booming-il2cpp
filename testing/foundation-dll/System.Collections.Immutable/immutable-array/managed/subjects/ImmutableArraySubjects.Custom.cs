// Handwritten custom entries for immutable-array subjects.
// ImmutableArray<T> is a struct — cannot use (int) cast in auto-generated code.

using System;
using System.Collections.Immutable;
using Chaos.TestFramework;

public static partial class ImmutableArraySubjects
{
    // [0] ImmutableArray.Create<Byte>() — empty array
    [Fact]
    public static void CustomEntrySubject_0()
    {
        var arr = ImmutableArray.Create<byte>();
        Assert.AreEqual(0, arr.Length);
    }

    // [1] ImmutableArray.Create<Byte>(Byte) — 1 element
    [Fact]
    public static void CustomEntrySubject_1()
    {
        var arr = ImmutableArray.Create<byte>((byte)42);
        Assert.AreEqual(1, arr.Length);
    }

    // [2] ImmutableArray.Create<Byte>(Byte, Byte) — 2 elements
    [Fact]
    public static void CustomEntrySubject_2()
    {
        var arr = ImmutableArray.Create<byte>((byte)42, (byte)43);
        Assert.AreEqual(2, arr.Length);
    }

    // [3] ImmutableArray.Create<Int32>() — empty array
    [Fact]
    public static void CustomEntrySubject_3()
    {
        var arr = ImmutableArray.Create<int>();
        Assert.AreEqual(0, arr.Length);
    }

    // [4] ImmutableArray.Create<Int32>(Int32) — 1 element
    [Fact]
    public static void CustomEntrySubject_4()
    {
        var arr = ImmutableArray.Create<int>(42);
        Assert.AreEqual(1, arr.Length);
    }

    // [5] ImmutableArray.Create<String>(String) — 1 element
    [Fact]
    public static void CustomEntrySubject_5()
    {
        var arr = ImmutableArray.Create<string>("hello");
        Assert.AreEqual(1, arr.Length);
    }
}
